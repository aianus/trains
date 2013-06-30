/*
 * shell.c
 *
 *  Created on: Jun 22, 2013
 *      Author: aianus
 */

#include <shell.h>
#include <sprintf.h>
#include <memory.h>
#include <write_server.h>
#include <read_server.h>
#include <syscall.h>
#include <train_server.h>
#include <clock_widget.h>
#include <sensor_server.h>
#include <switch_server.h>
#include <ts7200.h>
#include <string.h>

const char CLEAR_SCREEN[] = "\033[2J";
const char CLEAR_LINE[] = "\033[K";
const char POS_CURSOR[] = "\033[%u;%uH";

static char line_buffer[LINE_BUFFER_SIZE];
static unsigned int line_buffer_pos;

/*
static void position_cursor (unsigned int row, unsigned int column) {
    nbprintf (COM2, "\033[%u;%uH", row, column);
}
*/

int is_whitespace(char c) {
    return c == ' ' || c == '\t';
}

int is_numeric(char c) {
    return c >= '0' && c <= '9';
}

int is_direction(char c) {
    return c == 'S' || c == 'C';
}

int parse_uint(char **src) {
    char* str = *src;

    while(is_whitespace(*str)) ++str;

    if (!is_numeric(*str)) return -1;

    int num = 0;
    while(is_numeric(*str)) {
        int digit = (int)(*str - '0');
        num = num * 10 + digit;
        ++str;
    }

    *src = str;
    return num;
}

int parse_direction(char **src) {
    char* str = *src;

    while(is_whitespace(*str)) ++str;

    if (!is_direction(*str)) return -1;
    int result = *str == 'S' ? STRAIGHT : CURVED;
    ++str;

    *src = str;

    return result;
}

int parse_q(char *str) {
    // Skip Whitespace.
    while(is_whitespace(*str)) ++str;

    if (*str == 'q') return 1;
    else return 0;
}

int parse_tr(char *str, int *train, int *speed) {
    while(is_whitespace(*str)) ++str;

    if (*str != 't') return 0;
    str++;
    if (*str != 'r') return 0;
    str++;

    *train = parse_uint(&str);
    if (*train == -1) return 0;

    *speed = parse_uint(&str);
    if (*speed == -1) return 0;

    return 1;
}

int parse_rv(char *str, int *train) {
    while(is_whitespace(*str)) ++str;

    if (*str != 'r') return 0;
    str++;
    if (*str != 'v') return 0;
    str++;

    *train = parse_uint(&str);
    if (*train == -1) return 0;

    return 1;
}

int parse_sw(char *str, int *number, int *direction) {
    while(is_whitespace(*str)) ++str;

    if (*str != 's') return 0;
    str++;
    if (*str != 'w') return 0;
    str++;

    *number = parse_uint(&str);
    if (*number == -1) return 0;

    *direction = parse_direction(&str);
    if (*direction == -1) return 0;

    return 1;
}

void reset_shell() {
    line_buffer_pos = 0;
    memset(line_buffer, 0, sizeof(line_buffer));

    // Build command for positioning cursor, clearing the line, and printing a prompt
    char command[100];
    char *pos = &command[0];
    pos += sprintf(pos, (char *)POS_CURSOR, CONSOLE_HEIGHT, 1);
    pos += strcpy(pos, (char *)CLEAR_LINE);
    pos += strcpy(pos, "> ");

    Write(COM2, command, pos - command);
}

void shell() {
    // Clear the screen.
    Write(COM2, (char *)CLEAR_SCREEN, strlen((char *)CLEAR_SCREEN));

    // Give a blank prompt
    reset_shell();

    // Start the clock
    Create(LOW, clock_widget);

    Create(HIGH, sensor_server);
    Create(HIGH, switch_server);

    while (1) {
        char command[50 + LINE_BUFFER_SIZE];
        char *pos;
        int train, speed, number, direction;

        // Get a character
        char c = Getc(COM2);

        // Try to interpret the command if enter has been pressed
        if (c == '\n' || c == '\r') {
            if (parse_q(line_buffer)) {
                Exit();
            } else if (parse_tr(line_buffer, &train, &speed)) {
                SetSpeed(train, speed);
            } else if (parse_rv(line_buffer, &number)) {
                Reverse(train);
            } else if (parse_sw(line_buffer, &number, &direction)) {
                SetSwitch(number, direction);
            }

            reset_shell();

            continue;
        } else if (c == '\b') {
            line_buffer_pos--;
            line_buffer[line_buffer_pos] = 0;

            pos = &command[0];
            pos += strcpy(pos, "\b");
            Write(COM2, command, pos - command);

            continue;
        }

        // Otherwise just store it in the buffer and print it
        line_buffer[line_buffer_pos++] = c;

        pos = &command[0];
        pos += sprintf(pos, (char *)POS_CURSOR, CONSOLE_HEIGHT, line_buffer_pos + 2);
        *pos++ = c;
        Write(COM2, command, pos - command);
    }

}
