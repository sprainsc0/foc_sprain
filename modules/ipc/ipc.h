#ifndef _IPCCORE_H
#define _IPCCORE_H

/**
 * @file ipc.h
 * API for the uORB lightweight object broker.
 */
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>

#define TOPICS_COUNT (14u)

/**
 * Object metadata.
 */
struct ipc_metadata {
	const char *o_name;		/**< unique object name */
	const uint32_t o_size;		/**< object size */
    const uint16_t serial;
    const bool buffer;
    const uint8_t b_size;
};

typedef const struct ipc_metadata *ipc_id_t;

/**
 * Generates a pointer to the uORB metadata structure for
 * a given topic.
 *
 * The topic must have been declared previously in scope
 * with IPC_DECLARE().
 *
 * @param _name		The name of the topic.
 */
#define IPC_ID(_name)		&__ipc_##_name

/**
 * Declare (prototype) the uORB metadata for a topic (used by code generators).
 *
 * @param _name		The name of the topic.
 */
#if defined(__cplusplus)
# define IPC_DECLARE(_name)		extern "C" const struct ipc_metadata __ipc_##_name
#else
# define IPC_DECLARE(_name)		extern const struct ipc_metadata __ipc_##_name
#endif

/**
 * Define (instantiate) the uORB metadata for a topic.
 *
 * The uORB metadata is used to help ensure that updates and
 * copies are accessing the right data.
 *
 * Note that there must be no more than one instance of this macro
 * for each topic.
 *
 * @param _name		The name of the topic.
 * @param _struct	The structure the topic provides.
 * @param _size_no_padding	Struct size w/o padding at the end
 * @param _fields	All fields in a semicolon separated list e.g: "float[3] position;bool armed"
 */
#define IPC_DEFINE(_name, _struct, _serial, _buffer, _size)		\
	const struct ipc_metadata __ipc_##_name = {	\
		#_name,					\
		sizeof(_struct), 		\
        _serial,                \
        _buffer,                \
        _size,                  \
	}; struct hack

#ifdef __cplusplus
extern "C"{
#endif

/**
 * ORB topic advertiser handle.
 *
 * Advertiser handles are global; once obtained they can be shared freely
 * and do not need to be closed or released.
 *
 * This permits publication from interrupt context and other contexts where
 * a file-descriptor-based handle would not otherwise be in scope for the
 * publisher.
 */
typedef void 	*orb_advert_t;

void ipc_init();

orb_advert_t ipc_active(const struct ipc_metadata *meta, const void *data);

void ipc_inactive(orb_advert_t handle);

int	ipc_push(const struct ipc_metadata *meta, orb_advert_t handle, const void *data);

int	ipc_subscibe(const struct ipc_metadata *meta);

int	ipc_unsubscibe(int handle);

int	ipc_pull(const struct ipc_metadata *meta, int handle, void *buffer);

int	ipc_check(int handle, bool *updated);

int	ipc_push_isr(const struct ipc_metadata *meta, orb_advert_t handle, const void *data);

int	ipc_pull_isr(const struct ipc_metadata *meta, int handle, void *buffer);

int	ipc_check_isr(int handle, bool *updated);

#ifdef __cplusplus
}
#endif

#endif /* _UORB_UORB_H */
