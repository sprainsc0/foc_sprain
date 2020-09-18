#ifndef _PARAM_PARAM_H
#define _PARAM_PARAM_H

#include <stdint.h>
#include <stdbool.h>
#include <float.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * Parameter types.
 */
#define PARAM_TYPE_INT32		0
#define PARAM_TYPE_FLOAT		1
#define PARAM_TYPE_UNKNOWN		(0xffff)

typedef uint16_t param_type_t;

/**
 * Parameter handle.
 *
 * Parameters are represented by parameter handles, which can
 * be obtained by looking up parameters. They are an offset into a global
 * constant parameter array.
 */
typedef uint16_t	param_t;

/**
 * Handle returned when a parameter cannot be found.
 */
#define PARAM_INVALID	((uint16_t)0xffff)

/**
 * Magic handle for hash check param
 */
#define PARAM_HASH      ((uint16_t)INT16_MAX)

/**
 * Initialize the param backend. Call this on startup before calling any other methods.
 */
void		param_init(void);

/**
 * Look up a parameter by name.
 *
 * @param name		The canonical name of the parameter being looked up.
 * @return		A handle to the parameter, or PARAM_INVALID if the parameter does not exist.
 *			This call will also set the parameter as "used" in the system, which is used
 *			to e.g. show the parameter via the RC interface
 */
param_t	param_find(const char *name);

/**
 * Look up a parameter by name.
 *
 * @param name		The canonical name of the parameter being looked up.
 * @return		A handle to the parameter, or PARAM_INVALID if the parameter does not exist.
 */
param_t	param_find_no_notification(const char *name);

/**
 * Return the total number of parameters.
 *
 * @return		The number of parameters.
 */
unsigned	param_count(void);

/**
 * Return the actually used number of parameters.
 *
 * @return		The number of parameters.
 */
unsigned	param_count_used(void);

/**
 * Wether a parameter is in use in the system.
 *
 * @return		True if it has been written or read
 */
bool		param_used(param_t param);

/**
 * Look up a parameter by index.
 *
 * @param index		An index from 0 to n, where n is param_count()-1.
 * @return		A handle to the parameter, or PARAM_INVALID if the index is out of range.
 */
param_t	param_for_index(unsigned index);

/**
 * Look up an used parameter by index.
 *
 * @param index		The parameter to obtain the index for.
 * @return		The index of the parameter in use, or -1 if the parameter does not exist.
 */
param_t	param_for_used_index(unsigned index);

/**
 * Look up the index of a parameter.
 *
 * @param param		The parameter to obtain the index for.
 * @return		The index, or -1 if the parameter does not exist.
 */
int		param_get_index(param_t param);

uint32_t param_hash_check(void);

/**
 * Look up the index of an used parameter.
 *
 * @param param		The parameter to obtain the index for.
 * @return		The index of the parameter in use, or -1 if the parameter does not exist.
 */
int		param_get_used_index(param_t param);

/**
 * Obtain the name of a parameter.
 *
 * @param param		A handle returned by param_find.
 * @return		The name assigned to the parameter, or NULL if the handle is invalid.
 */
const char	*param_name(param_t param);

/**
 * Obtain the volatile state of a parameter.
 *
 * @param param		A handle returned by param_find.
 * @return			true if the parameter is volatile
 */
bool		param_is_volatile(param_t param);

/**
 * Test whether a parameter's value has changed from the default.
 *
 * @return		If true, the parameter's value has not been changed from the default.
 */
bool		param_value_is_default(param_t param);

/**
 * Test whether a parameter's value has been changed but not saved.
 *
 * @return		If true, the parameter's value has not been saved.
 */
bool		param_value_unsaved(param_t param);

/**
 * Obtain the type of a parameter.
 *
 * @param param		A handle returned by param_find.
 * @return		The type assigned to the parameter.
 */
param_type_t	param_type(param_t param);

/**
 * Determine the size of a parameter.
 *
 * @param param		A handle returned by param_find.
 * @return		The size of the parameter's value.
 */
uint16_t		param_size(param_t param);

/**
 * Copy the value of a parameter.
 *
 * @param param		A handle returned by param_find.
 * @param val		Where to return the value, assumed to point to suitable storage for the parameter type.
 *			For structures, a bitwise copy of the structure is performed to this address.
 * @return		Zero if the parameter's value could be returned, nonzero otherwise.
 */
int		param_get(param_t param, void *val);

/**
 * Set the value of a parameter.
 *
 * @param param		A handle returned by param_find.
 * @param val		The value to set; assumed to point to a variable of the parameter type.
 *			For structures, the pointer is assumed to point to a structure to be copied.
 * @return		Zero if the parameter's value could be set from a scalar, nonzero otherwise.
 */
int		param_set(param_t param, const void *val);

/**
 * Set the value of a parameter, but do not notify the system about the change.
 *
 * @param param		A handle returned by param_find.
 * @param val		The value to set; assumed to point to a variable of the parameter type.
 *			For structures, the pointer is assumed to point to a structure to be copied.
 * @return		Zero if the parameter's value could be set from a scalar, nonzero otherwise.
 */
int		param_set_no_notification(param_t param, const void *val);

/**
 * Notify the system about parameter changes. Can be used for example after several calls to
 * param_set_no_notification() to avoid unnecessary system notifications.
 */
void		param_notify_changes(uint8_t mode);

/**
 * Reset a parameter to its default value.
 *
 * This function frees any storage used by struct parameters, and returns the parameter
 * to its default value.
 *
 * @param param		A handle returned by param_find.
 * @return		Zero on success, nonzero on failure
 */
int		param_reset(param_t param);

/**
 * Reset all parameters to their default values.
 *
 * This function also releases the storage used by struct parameters.
 */
void		param_reset_all(void);

/**
 * Load parameters from a file.
 *
 * This function resets all parameters to their default values, then loads new
 * values from a file.
 *
 * @param fd		File descriptor to import from.  (Currently expected to be a file.)
 * @return		Zero on success, nonzero if an error occurred during import.
 *			Note that in the failure case, parameters may be inconsistent.
 */
int		param_load(void);

/**
 * Save parameters to the default file.
 * Note: this method requires a large amount of stack size!
 *
 * This function saves all parameters with non-default values.
 *
 * @return		Zero on success.
 */
int 		param_save(bool only_unsaved);


/**
 * Parameter value union.
 */
union param_value_u {
    void       *p;
	int32_t		i;
	float		f;
};

/**
 * Static parameter definition structure.
 *
 * This is normally not used by user code; see the PARAM_DEFINE macros
 * instead.
 */
struct param_info_s {
	const char	*name;
	param_type_t	type;
	uint16_t		volatile_param: 1;
	union param_value_u val;
};

#ifdef __cplusplus
}
#endif
    
#endif