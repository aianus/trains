/*
 * shell.h
 *
 *  Created on: Jun 22, 2013
 *      Author: aianus
 */

#ifndef SHELL_H_
#define SHELL_H_

#include <track_node.h>
#include <switch_server.h>

#define CONSOLE_HEIGHT 34
#define LINE_BUFFER_SIZE 70
#define SCROLLABLE_AREA_SIZE 21

typedef struct ShellMessage {

    enum {
        SHELL_INIT_TRACK,
        SHELL_ADD_TRAIN,
        SHELL_REVERSE_TRAIN,
        SHELL_SET_TRAIN_SPEED,
        SHELL_GO,
        SHELL_STOP,
        SHELL_SET_SWITCH_POSITION,
        SHELL_ORIENT,
        SHELL_SIMULATE,
        SHELL_SUCCESS_REPLY
    } type;

    char track;

    union {
        unsigned char train_no;
        unsigned char switch_no;
    };

    union {
        unsigned char switch_pos;
        unsigned char speed;
    };

    track_node *position;
    union {
        int distance;
        int orientation;
    };

} ShellMessage;

void shell();

#endif /* SHELL_H_ */
