#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <stddef.h>  /* uint16_t */
#include <string.h>  /* memset, etc */
#include <stdlib.h>  /* exit */
#include <assert.h>
#include <stdint.h>

#ifdef __cplusplus
extern  "C" {
#endif

enum ringbuffer_state
{
    RINGBUFFER_EMPTY,
    RINGBUFFER_FULL,
    /* half full is neither full nor empty */
    RINGBUFFER_HALFFULL,
};

/* ring buffer */
struct ringbuffer
{
    uint8_t *buffer_ptr;
    /* use the msb of the {read,write}_index as mirror bit. You can see this as
     * if the buffer adds a virtual mirror and the pointers point either to the
     * normal or to the mirrored buffer. If the write_index has the same value
     * with the read_index, but in a different mirror, the buffer is full.
     * While if the write_index and the read_index are the same and within the
     * same mirror, the buffer is empty. The ASCII art of the ringbuffer is:
     *
     *          mirror = 0                    mirror = 1
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Full
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     *  read_idx-^                   write_idx-^
     *
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Empty
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * read_idx-^ ^-write_idx
     *
     * The tradeoff is we could only use 32KiB of buffer for 16 bit of index.
     * But it should be enough for most of the cases.
     *
     * Ref: http://en.wikipedia.org/wiki/Circular_buffer#Mirroring */
    uint16_t read_mirror : 1;
    uint16_t read_index : 15;
    uint16_t write_mirror : 1;
    uint16_t write_index : 15;
    /* as we use msb of index as mirror bit, the size should be signed and
     * could only be positive. */
    int16_t buffer_size;
};

static inline uint16_t ringbuffer_get_size(struct ringbuffer *rb)
{
    assert(rb != NULL);
    return rb->buffer_size;
}

static inline enum ringbuffer_state
ringbuffer_status(struct ringbuffer *rb)
{
    if (rb->read_index == rb->write_index)
    {
        if (rb->read_mirror == rb->write_mirror)
            return RINGBUFFER_EMPTY;
        else
            return RINGBUFFER_FULL;
    }
    return RINGBUFFER_HALFFULL;
}

/** return the size of data in rb */
static inline uint16_t ringbuffer_data_len(struct ringbuffer *rb)
{
    switch (ringbuffer_status(rb))
    {
    case RINGBUFFER_EMPTY:
        return 0;
    case RINGBUFFER_FULL:
        return rb->buffer_size;
    case RINGBUFFER_HALFFULL:
    default:
        if (rb->write_index > rb->read_index)
            return rb->write_index - rb->read_index;
        else
            return rb->buffer_size - (rb->read_index - rb->write_index);
    };
}

/** return the size of empty space in rb */
#define ringbuffer_space_len(rb) ((rb)->buffer_size - ringbuffer_data_len(rb))

void ringbuffer_init(struct ringbuffer *rb, uint8_t *pool, uint16_t size);

void ringbuffer_reset(struct ringbuffer *rb);

uint16_t ringbuffer_put(struct ringbuffer *rb, const uint8_t *ptr, uint16_t length);

uint16_t ringbuffer_put_force(struct ringbuffer *rb, const uint8_t *ptr, uint16_t length);

uint16_t ringbuffer_get(struct ringbuffer *rb, uint8_t *ptr, uint16_t length);

uint16_t ringbuffer_putchar(struct ringbuffer *rb, const uint8_t ch);

uint16_t ringbuffer_putchar_force(struct ringbuffer *rb, const uint8_t ch);

uint16_t ringbuffer_getchar(struct ringbuffer *rb, uint8_t *ch);

#ifdef __cplusplus
}
#endif

#endif
