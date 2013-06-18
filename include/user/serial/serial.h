#ifndef _SERIAL_H_
#define _SERIAL_H_

enum WriteMessageType {
    WRITE_EVENT_REQUEST,
    WRITE_EVENT_RESPONSE,
    PUTC_REQUEST,
    PUTC_RESPONSE
};

enum ReadMessageType {
    READ_EVENT_REQUEST,
    READ_EVENT_RESPONSE,
    GETC_REQUEST,
    GETC_RESPONSE
};

typedef struct WriteMessage {
    enum WriteMessageType type;
    int data;
} WriteMessage;

typedef struct ReadMessage {
    enum ReadMessageType type;
    int data;
} ReadMessage;

#endif