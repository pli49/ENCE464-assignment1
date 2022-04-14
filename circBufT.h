/*The contents of this file are based on the original
 * circBufT.h provided in labs, modified to store the sum*/

// Based on code provided by P.J. Bones from ENCE361 Week4Lab/circBufT.h

#ifndef CIRCBUFT_H_
#define CIRCBUFT_H_


/**
 * @struct          circBuf_t.
 * @brief           Contains all circular buffer properties.
 * @brief           All properties of a circular buffer (size, windex, rindex, ...) are stored in this structure.
 * 
 * @param size      Number of entries in buffer.     
 * @param windex    Buffer write index.
 * @param rindex    Buffer read index.
 * @param data      Pointer to buffer data
 * @param sum       Rolling sum of values stored in the buffer
*/
typedef struct {
    uint32_t size;
    uint32_t windex;
    uint32_t rindex;
    uint32_t *data;
    uint32_t sum;
} circBuf_t;

/**
 * @function        initCircBuf.
 * @brief           Initialize circular buffer, dynamically allocating and clearing required memory.
 * @param buffer    Pointer to the circBuf_t structure.
 * @param size      Size of buffer to initialize.
 * @returns         uint32_t: Pointer to the buffer data.
*/
uint32_t * initCircBuf (circBuf_t *buffer, uint32_t size);


/**
 * @function        writeCircBuf.
 * @brief           Write data to circular buffer at current write index location, update sum and advance write index.
 * @param buffer    Pointer to the circBuf_t structure.
 * @param entry     Value to write to buffer.
*/
void writeCircBuf (circBuf_t *buffer, uint32_t entry);


/**
 * @function        initCircBuf.
 * @brief           Read data from circular buffer, advance read index.
 * @param buffer    Pointer to the circBuf_t structure.
 * @returns         uint32_t: Value read from buffer.
*/
uint32_t readCircBuf (circBuf_t *buffer);


/**
 * @function        freeCircBuf.
 * @brief           Releases memory allocated to circular buffer and deletes buffer contents.
 * @param buffer    Pointer to the circBuf_t structure.
*/
void freeCircBuf (circBuf_t *buffer);

#endif /* CIRCBUFT_H_ */
