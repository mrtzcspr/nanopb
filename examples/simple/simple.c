#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "simple.pb.h"

#define LOG 0
#if LOG
#define LOG_INFO(format, ...) printf(format, ##__VA_ARGS__)
#define LOG_BUFFER(buffer, size)                                       \
    do                                                                 \
    {                                                                  \
        printf("Content of `%s` (length: %d):\n", #buffer, (int)size); \
        for (size_t i = 0; i < (size); i++)                            \
        {                                                              \
            printf("%02X ", ((uint8_t *)(buffer))[i]);                 \
        }                                                              \
        printf("\n");                                                  \
    } while (0)
#else
#define LOG_INFO(format, ...)
#define LOG_BUFFER(buffer, size)
#endif

#define ENCODE_COUNT 100000
#define DECODE_COUNT 0

int main()
{
    /* This is the buffer where we will store our message. */
    uint8_t buffer[128] = {0x08, 0x0D}; // result with "message.lucky_number = 13"
    size_t message_length = 2;          // result with "message.lucky_number = 13"
    bool status;
    
    /* Encode our message */
    for (size_t i = 0; i < ENCODE_COUNT; i++)
    {
        /* Allocate space on the stack to store the message data.
         *
         * Nanopb generates simple struct definitions for all the messages.
         * - check out the contents of simple.pb.h!
         * It is a good idea to always initialize your structures
         * so that you do not have garbage data from RAM in there.
         */
        SimpleMessage message = SimpleMessage_init_zero;
        
        /* Create a stream that will write to our buffer. */
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        
        /* Fill in the lucky number */
        message.lucky_number = 13;
        
        /* Now we are ready to encode the message! */
        status = pb_encode(&stream, SimpleMessage_fields, &message);
        message_length = stream.bytes_written;
        
        /* Then just check for any errors.. */
        if (!status)
        {
            printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
            return 1;
        }
    }
    
    /* Now we could transmit the message over network, store it in a file or
     * wrap it to a pigeon's leg.
     */
    LOG_BUFFER(buffer, message_length);

    /* But because we are lazy, we will just decode it immediately. */
    for (size_t i = 0; i < DECODE_COUNT; i++)
    {
        /* Allocate space for the decoded message. */
        SimpleMessage message = SimpleMessage_init_zero;
        
        /* Create a stream that reads from the buffer. */
        pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
        
        /* Now we are ready to decode the message. */
        status = pb_decode_noinit(&stream, SimpleMessage_fields, &message);
        
        /* Check for errors... */
        if (!status)
        {
            printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
            return 1;
        }
        
        /* Print the data contained in the message. */
        LOG_INFO("Your lucky number was %d!\n", (int)message.lucky_number);
    }
    
    return 0;
}

