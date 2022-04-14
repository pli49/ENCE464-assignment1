// *******************************************************
/*The contents of this file are based on the original
 * circBufT.c provided in labs, with modifications to allow
 * storage of a sum value*/

// Based on code provided by P.J. Bones from ENCE361 Week4Lab/circBufT.c with slight modifications for rolling sum variable

// *******************************************************

#include <stdint.h>
#include "stdlib.h"
#include "circBufT.h"

// *******************************************************
// initCircBuf: Initialise the circBuf instance. Reset both indices to
// the start of the buffer.  Dynamically allocate and clear the the 
// memory and return a pointer for the data.  Return NULL if 
// allocation fails.
uint32_t * initCircBuf (circBuf_t *buffer, uint32_t size) {
    buffer->windex = 0;
    buffer->rindex = 0;
    buffer->size = size;
    buffer->data = (uint32_t *) calloc (sizeof(uint32_t), size);

    buffer->sum = 0;
    return buffer->data;
}

// *******************************************************
// writeCircBuf: insert entry at the current windex location,
// advance windex, modulo (buffer size).

/* Extended functionality, subtracting oldest buffer entry from sum,
 * then adding the new buffer entry to the sum.*/
void writeCircBuf (circBuf_t *buffer, uint32_t entry) {
    buffer->sum -= buffer->data[buffer->windex];
    buffer->sum += entry;

    buffer->data[buffer->windex] = entry;
    buffer->windex++;
    if (buffer->windex >= buffer->size)
       buffer->windex = 0;
}

// *******************************************************
// readCircBuf: return entry at the current rindex location,
// advance rindex, modulo (buffer size). The function does not check
// if reading has advanced ahead of writing.
uint32_t readCircBuf (circBuf_t *buffer) {
    uint32_t entry;

    entry = buffer->data[buffer->rindex];
    buffer->rindex++;
    if (buffer->rindex >= buffer->size)
       buffer->rindex = 0;
    return entry;
}

// *******************************************************
// freeCircBuf: Releases the memory allocated to the buffer data,
// sets pointer to NULL and ohter fields to 0. The buffer can
// re-initialised by another call to initCircBuf().
void freeCircBuf (circBuf_t * buffer) {
    buffer->windex = 0;
    buffer->rindex = 0;
    buffer->size = 0;
    free (buffer->data);
    buffer->data = NULL;
}
