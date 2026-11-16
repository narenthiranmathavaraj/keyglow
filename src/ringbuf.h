
#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdint.h>
#include <stdbool.h>

#define RINGBUF_CAPACITY 20

typedef struct
{
    char buffer[RINGBUF_CAPACITY];

    volatile uint8_t head;
    volatile uint8_t tail;
    volatile uint8_t count;

} ringbuf_t;

void ringbuf_init(ringbuf_t *rb);

bool ringbuf_push(ringbuf_t *rb, char data);

bool ringbuf_pop(ringbuf_t *rb, char *data);

bool ringbuf_is_empty(ringbuf_t *rb);

bool ringbuf_is_full(ringbuf_t *rb);

#endif
