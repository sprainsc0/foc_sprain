#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include <semphr.h>
#include <math.h>

#include "param.h"
#include "foc_parameter.h"
#include "uPerf.h"
#include "debug.h"

#include "hrt_timer.h"
#include "uthash/utarray.h"
#include "flashfs.h"
#include "tinybson.h"
#include "misc/crc32.h"
#include "foc_function.h"

#include "topics/parameter_update.h"
/**
 * Array of static parameter info.
 */
static const struct param_info_s *param_info_base = (const struct param_info_s *) &foc_parameters;
#define	param_info_count foc_parameters.param_count

/**
 * Storage for modified parameters.
 */
struct param_wbuf_s {
	union param_value_u	val;
	param_t			param;
	bool			unsaved;
};

uint8_t  *param_changed_storage = NULL;
int size_param_changed_storage_bytes = 0;
const int bits_per_allocation_unit  = (sizeof(*param_changed_storage) * 8); // 8 param states per byte

// get the parameter info and malloc modified memery
static unsigned get_param_info_count(void)
{
	if (!param_changed_storage) {
		size_param_changed_storage_bytes  = (param_info_count / bits_per_allocation_unit) + 1;
		param_changed_storage = calloc(size_param_changed_storage_bytes, 1);

		if (param_changed_storage == NULL) {
			return 0;
		}
	}
	return param_info_count;
}

// the array holding modified parameter values
UT_array        *param_values;

// array info for the modified parameters array
const UT_icd    param_icd = {sizeof(struct param_wbuf_s), NULL, NULL, NULL};

static orb_advert_t param_topic = NULL;
static unsigned int param_instance = 0;

static void param_set_used_internal(param_t param);

static param_t param_find_internal(const char *name, bool notification);

static SemaphoreHandle_t param_sem;
static SemaphoreHandle_t param_sem_save;

static perf_counter_t param_find_perf;
static perf_counter_t param_get_perf;
static perf_counter_t param_set_perf;
static perf_counter_t param_save_perf;

/** lock the parameter store for write access */
static void param_lock(void)
{
    xSemaphoreTake(param_sem, portMAX_DELAY);
}

/** unlock the parameter store for write access */
static void param_unlock(void)
{
    xSemaphoreGive(param_sem);
}

/** lock the parameter store for write access */
static void param_save_lock(void)
{
    xSemaphoreTake(param_sem_save, portMAX_DELAY);
}

/** unlock the parameter store for write access */
static void param_save_unlock(void)
{
    xSemaphoreGive(param_sem_save);
}

void param_init(void)
{

	param_sem = xSemaphoreCreateMutex();
    if(param_sem == NULL) {
        Info_Debug("param mutex create error\n");
    }
    
	param_sem_save = xSemaphoreCreateMutex();
    if(param_sem_save == NULL) {
        Info_Debug("param mutex create error\n");
    }

	param_find_perf = perf_alloc(PC_ELAPSED, "param_find");
	param_get_perf = perf_alloc(PC_ELAPSED, "param_get");
	param_set_perf = perf_alloc(PC_ELAPSED, "param_set");
	param_save_perf = perf_alloc(PC_ELAPSED, "param_save");
}

// check whether a param_t is value.
static bool handle_in_range(param_t param)
{
	unsigned count = get_param_info_count();
	return (count && param < count);
}

/**
 * Compare two modifid parameter structures to determine ordering.
 *
 * This function is suitable for passing to qsort or bsearch.
 */
static int param_compare_values(const void *a, const void *b)
{
	struct param_wbuf_s *pa = (struct param_wbuf_s *)a;
	struct param_wbuf_s *pb = (struct param_wbuf_s *)b;

	if (pa->param < pb->param) {
		return -1;
	}

	if (pa->param > pb->param) {
		return 1;
	}

	return 0;
}

void param_notify_changes(uint8_t mode)
{
	struct parameter_update_s pup = {
		.timestamp = micros(),
		.instance = param_instance++,
        .update_mode = mode,
	};


	if (param_topic == NULL) {
		param_topic = ipc_active(IPC_ID(parameter_update), &pup);

	} else {
		ipc_push(IPC_ID(parameter_update), param_topic, &pup);
	}
}

/**
 * Locate the modified parameter structure for a parameter, if it exists.
 */
static struct param_wbuf_s *param_find_changed(param_t param)
{
	struct param_wbuf_s	*s = NULL;

	if (param_values != NULL) {
		struct param_wbuf_s key;
		key.param = param;
		s = utarray_find(param_values, &key, param_compare_values);
	}

	return s;
}

// find the parameter by parameter name
param_t param_find_internal(const char *name, bool notification)
{
	perf_begin(param_find_perf);

	param_t param;

	for (param = 0; handle_in_range(param); param++) {
		if (!strcmp(param_info_base[param].name, name)) {
			if (notification) {
				param_set_used_internal(param);
			}

			return param;
		}
	}

	perf_end(param_find_perf);

	/* not found */
	return PARAM_INVALID;
}

// parameter find by name
param_t param_find(const char *name)
{
	return param_find_internal(name, true);
}

param_t param_find_no_notification(const char *name)
{
	return param_find_internal(name, false);
}

// get the parameter info
unsigned param_count(void)
{
	return get_param_info_count();
}

// check how many parameter used throght param_changed_storage array
unsigned param_count_used(void)
{
	unsigned count = 0;

	// ensure the allocation has been done
	if (get_param_info_count()) {

		for (unsigned i = 0; i < size_param_changed_storage_bytes; i++) {
			for (unsigned j = 0; j < bits_per_allocation_unit; j++) {
				if (param_changed_storage[i] & (1 << j)) {
					count++;
				}
			}
		}
	}
	return count;
}

// check the index of parameter
param_t param_for_index(unsigned index)
{
	unsigned count = get_param_info_count();

	if (count && index < count) {
		return (param_t)index;
	}

	return PARAM_INVALID;
}

param_t param_for_used_index(unsigned index)
{
	int count = get_param_info_count();

	if (count && index < count) {
		/* walk all params and count used params */
		unsigned used_count = 0;

		for (unsigned i = 0; i < (unsigned)size_param_changed_storage_bytes; i++) {
			for (unsigned j = 0; j < bits_per_allocation_unit; j++) {
				if (param_changed_storage[i] & (1 << j)) {

					/* we found the right used count,
					 * return the param value
					 */
					if (index == used_count) {
						return (param_t)(i * bits_per_allocation_unit + j);
					}

					used_count++;
				}
			}
		}
	}

	return PARAM_INVALID;
}

int param_get_index(param_t param)
{
	if (handle_in_range(param)) {
		return (unsigned)param;
	}

	return -1;
}

int param_get_used_index(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	if (!param_used(param)) {
		return -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	int used_count = 0;

	for (unsigned i = 0; i < (unsigned)size_param_changed_storage_bytes; i++) {
		for (unsigned j = 0; j < bits_per_allocation_unit; j++) {
			if (param_changed_storage[i] & (1 << j)) {

				if ((unsigned)param == i * bits_per_allocation_unit + j) {
					return used_count;
				}

				used_count++;
			}
		}
	}

	return -1;
}

const char *param_name(param_t param)
{
	return handle_in_range(param) ? param_info_base[param].name : NULL;
}

bool param_is_volatile(param_t param)
{
	return handle_in_range(param) ? param_info_base[param].volatile_param : false;
}

bool param_value_is_default(param_t param)
{
	struct param_wbuf_s *s;
	param_lock();
	s = param_find_changed(param);
	param_unlock();
	return s ? false : true;
}

bool param_value_unsaved(param_t param)
{
	struct param_wbuf_s *s;
	param_lock();
	s = param_find_changed(param);
	bool ret = s && s->unsaved;
	param_unlock();
	return ret;
}

param_type_t param_type(param_t param)
{
	return handle_in_range(param) ? param_info_base[param].type : PARAM_TYPE_UNKNOWN;
}

uint16_t param_size(param_t param)
{
	if (handle_in_range(param)) {

		switch (param_type(param)) {

		case PARAM_TYPE_INT32:
		case PARAM_TYPE_FLOAT:
			return 4;

		default:
			return 0;
		}
	}
	return 0;
}

/**
 * Obtain a pointer to the storage allocated for a parameter.
 */
static const void *param_get_value_ptr(param_t param)
{
	const void *result = NULL;

	if (handle_in_range(param)) {

		const union param_value_u *v;

		/* work out whether we're fetching the default or a written value */
		struct param_wbuf_s *s = param_find_changed(param);

		if (s != NULL) {
			v = &s->val;

		} else {
			v = &param_info_base[param].val;
		}

		result = v;
	}

	return result;
}

uint32_t param_hash_check(void)
{
	uint32_t param_hash = 0;

	param_lock();

	/* compute the CRC32 over all string param names and 4 byte values */
	for (param_t param = 0; handle_in_range(param); param++) {
		if (!param_used(param) || param_is_volatile(param)) {
			continue;
		}

		const char *name = param_name(param);
		const void *val = param_get_value_ptr(param);
		param_hash = crc32part((const uint8_t *)name, strlen(name), param_hash);
		param_hash = crc32part(val, param_size(param), param_hash);
	}

	param_unlock();

	return param_hash;
}

int param_get(param_t param, void *val)
{
	int result = 0;

	param_lock();
	perf_begin(param_get_perf);

	const void *v = param_get_value_ptr(param);

	if (val && v) {
		memcpy(val, v, param_size(param));
		result = 1;
	}

	perf_end(param_get_perf);
	param_unlock();

	return result;
}

static int param_set_internal(param_t param, const void *val, bool mark_saved, bool notify_changes)
{
	int result = 0;
	bool params_changed = false;

	param_lock();
	perf_begin(param_set_perf);

	if (param_values == NULL) {
		utarray_new(param_values, &param_icd);
	}

	if (param_values == NULL) {
		Info_Debug("failed to allocate modified values array");
		goto out;
	}

	if (handle_in_range(param)) {

		struct param_wbuf_s *s = param_find_changed(param);

		if (s == NULL) {

			/* construct a new parameter */
			struct param_wbuf_s buf = {
				.param = param,
				.val.p = NULL,
				.unsaved = false
			};
			params_changed = true;

			/* add it to the array and sort */
			utarray_push_back(param_values, &buf);
			utarray_sort(param_values, param_compare_values);

			/* find it after sorting */
			s = param_find_changed(param);
		}

		/* update the changed value */
		switch (param_type(param)) {

		case PARAM_TYPE_INT32:
			params_changed = params_changed || s->val.i != *(int32_t *)val;
			s->val.i = *(int32_t *)val;
			break;

		case PARAM_TYPE_FLOAT:
			params_changed = params_changed || fabsf(s->val.f - * (float *)val) > FLT_EPSILON;
			s->val.f = *(float *)val;
			break;

		default:
			goto out;
		}

		s->unsaved = !mark_saved;
		result = 1;

		if (!mark_saved) { // this is false when importing parameters
			param_save(false);
		}
	}

out:
	perf_end(param_set_perf);
	param_unlock();

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	if (params_changed && notify_changes) {
		param_notify_changes(0);
	}

	return result;
}

int param_set_external(param_t param, const void *val, bool mark_saved, bool notify_changes)
{
	return param_set_internal(param, val, mark_saved, notify_changes);
}

const void *param_get_value_ptr_external(param_t param)
{
	return param_get_value_ptr(param);
}

int param_set(param_t param, const void *val)
{
	return param_set_internal(param, val, false, true);
}

int param_set_no_notification(param_t param, const void *val)
{
	return param_set_internal(param, val, false, false);
}

bool param_used(param_t param)
{
	int param_index = param_get_index(param);

	if (param_index < 0) {
		return false;
	}

	return param_changed_storage[param_index / bits_per_allocation_unit] &
	       (1 << param_index % bits_per_allocation_unit);
}

void param_set_used_internal(param_t param)
{
	int param_index = param_get_index(param);

	if (param_index < 0) {
		return;
	}

	// FIXME: this needs locking too
	param_changed_storage[param_index / bits_per_allocation_unit] |=
		(1 << param_index % bits_per_allocation_unit);
}

int param_reset(param_t param)
{
	struct param_wbuf_s *s = NULL;
	bool param_found = false;

	param_lock();

	if (handle_in_range(param)) {

		/* look for a saved value */
		s = param_find_changed(param);

		/* if we found one, erase it */
		if (s != NULL) {
			int pos = utarray_eltidx(param_values, s);
			utarray_erase(param_values, pos, 1);
		}

		param_found = true;
	}

	param_save(false);

	param_unlock();

	if (s != NULL) {
		param_notify_changes(0);
	}

	return (!param_found);
}
static void param_reset_all_internal(bool auto_save)
{
	param_lock();

	if (param_values != NULL) {
		utarray_free(param_values);
	}

	/* mark as reset / deleted */
	param_values = NULL;

	if (auto_save) {
		param_save(false);
	}

	param_unlock();

	param_notify_changes(0);
}

void param_reset_all(void)
{
	param_reset_all_internal(true);
}

// ----------------------------------------------------------------

struct param_import_state {
	bool mark_saved;
};

static int param_import_callback(bson_decoder_t decoder, void *private, bson_node_t node)
{
	float f;
	int32_t i;
	void *v, *tmp = NULL;
	int result = -1;
	struct param_import_state *state = (struct param_import_state *)private;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	if (node->type == BSON_EOO) {
		//Info_Debug("end of parameters");
		return 0;
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(node->name);

	if (param == PARAM_INVALID) {
		Info_Debug("ignoring unrecognised parameter '%s'", node->name);
		return 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	switch (node->type) {
	case BSON_INT32:
		if (param_type(param) != PARAM_TYPE_INT32) {
			Info_Debug("unexpected type for %s", node->name);
			result = 1; // just skip this entry
			goto out;
		}

		i = node->i;
		v = &i;
		break;

	case BSON_DOUBLE:
		if (param_type(param) != PARAM_TYPE_FLOAT) {
			Info_Debug("unexpected type for %s", node->name);
			result = 1; // just skip this entry
			goto out;
		}

		f = node->d;
		v = &f;
		break;

	case BSON_BINDATA:
		if (node->subtype != BSON_BIN_BINARY) {
			Info_Debug("unexpected type for %s", node->name);
			result = 1; // just skip this entry
			goto out;
		}

		if (bson_decoder_data_pending(decoder) != param_size(param)) {
			Info_Debug("bad size for '%s'", node->name);
			result = 1; // just skip this entry
			goto out;
		}

		/* XXX check actual file data size? */
		tmp = malloc(param_size(param));

		if (tmp == NULL) {
			Info_Debug("failed allocating for '%s'", node->name);
			goto out;
		}

		if (bson_decoder_copy_data(decoder, tmp)) {
			Info_Debug("failed copying data for '%s'", node->name);
			goto out;
		}

		v = tmp;
		break;

	default:
		Info_Debug("unrecognised node type");
		goto out;
	}

	if (!param_set_external(param, v, state->mark_saved, true)) {

		Info_Debug("error setting value for '%s'", node->name);
		goto out;
	}

	if (tmp != NULL) {
		free(tmp);
		tmp = NULL;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:

	if (tmp != NULL) {
		free(tmp);
	}

	return result;
}

static int param_import_internal(bool mark_saved)
{
	struct bson_decoder_s decoder;
	int result = -1;
	struct param_import_state state;

	uint8_t *buffer = 0;
	size_t buf_size;
	parameter_flashfs_read(parameters_token, &buffer, &buf_size);

	if (bson_decoder_init_buf(&decoder, buffer, buf_size, param_import_callback, &state)) {
		Info_Debug("decoder init failed");
		goto out;
	}

	state.mark_saved = mark_saved;

	do {
		result = bson_decoder_next(&decoder);

	} while (result > 0);

out:

	if (result < 0) {
		Info_Debug("BSON error decoding parameters");
	}

	return result;
}

int param_save(bool only_unsaved)
{
    struct param_wbuf_s *s = NULL;
	struct bson_encoder_s encoder;
	int     result = -1;

	/* Use realloc */
	param_save_lock();
    
    perf_begin(param_save_perf);

	bson_encoder_init_buf(&encoder, NULL, 0);

	/* no modified parameters -> we are done */
	if (param_values == NULL) {
		result = 0;
		goto out;
	}

	while ((s = (struct param_wbuf_s *)utarray_next(param_values, s)) != NULL) {

		int32_t i;
		float   f;

		/*
		 * If we are only saving values changed since last save, and this
		 * one hasn't, then skip it
		 */
		if (only_unsaved && !s->unsaved) {
			continue;
		}

		s->unsaved = false;

		/* append the appropriate BSON type object */

		switch (param_type(s->param)) {

		case PARAM_TYPE_INT32:
			i = s->val.i;

			if (bson_encoder_append_int(&encoder, param_name(s->param), i)) {
				Info_Debug("BSON append failed for '%s'", param_name(s->param));
				goto out;
			}

			break;

		case PARAM_TYPE_FLOAT:
			f = s->val.f;

			if (bson_encoder_append_double(&encoder, param_name(s->param), f)) {
				Info_Debug("BSON append failed for '%s'", param_name(s->param));
				goto out;
			}

			break;

		default:
			Info_Debug("unrecognized parameter type");
			goto out;
		}
	}

	result = 0;

out:

	if (result == 0) {

		/* Finalize the bison encoding*/

		bson_encoder_fini(&encoder);

		/* Get requiered space */

		size_t buf_size = bson_encoder_buf_size(&encoder);

		/* Get a buffer from the flash driver with enough space */

		uint8_t *buffer;
		result = parameter_flashfs_alloc(parameters_token, &buffer, &buf_size);

		if (result == 0) {

			/* Check for a write that has no changes */

			uint8_t *was_buffer;
			size_t was_buf_size;
			int was_result = parameter_flashfs_read(parameters_token, &was_buffer, &was_buf_size);

			void *enc_buff = bson_encoder_buf_data(&encoder);

			bool commit = was_result < 0 || was_buf_size != buf_size || 0 != memcmp(was_buffer, enc_buff, was_buf_size);

			if (commit) {

				memcpy(buffer, enc_buff, buf_size);
				result = parameter_flashfs_write(parameters_token, buffer, buf_size);
				result = result == buf_size ? 0 : -1;

			}

			free(enc_buff);
			parameter_flashfs_free();
		}

	}
    
    perf_end(param_save_perf);
    
    param_save_unlock();
	return result;
}

int param_load(void)
{
	param_reset_all_internal(false);
	return param_import_internal(true);
}


int param_main(int argc, char *argv[])
{
    if (argc < 1) {
		Info_Debug("input argv error\n");
		return 1;
	}
    
    // parameter system init
    param_init();
    
    for(int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "load")) {
            if(param_load() < 0) {
                Info_Debug("load parameter failed\n");
                return 0;
            } else {
                Info_Debug("load parameter successed\n");
            }
        }
        
        if (!strcmp(argv[i], "reset")) {
            param_reset_all();
            Info_Debug("parameter reset all\n");
        }
    }
    return 1;
}
