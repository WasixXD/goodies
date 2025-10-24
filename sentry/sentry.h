/******************************************************************************
 * SENTRY - Lightweight Logging Library for C
 * 
 * FUNCTIONS:
 * 
 * sentry_set_log_level(SENTRY_LEVEL level)
 *     Sets the minimum log level. Only messages at this level or higher
 *     will be displayed. Levels in order: DEBUG < INFO < WARN < ERROR
 *     Example: Setting SENTRY_WARN will show only WARN and ERROR messages.
 * 
 * sentry_debug(format, ...)
 *     Logs a DEBUG level message. Useful for detailed debugging information.
 * 
 * sentry_info(format, ...)
 *     Logs an INFO level message. For general informational messages.
 * 
 * sentry_warn(format, ...)
 *     Logs a WARN level message. For warning conditions that need attention.
 * 
 * sentry_error(format, ...)
 *     Logs an ERROR level message. For error conditions and failures.
 * 
 * All logging macros automatically include timestamp, filename, and line number.
 * Output is color-coded: DEBUG (blue), INFO (magenta), WARN (yellow), ERROR (red).
 * 
 * USAGE:
 * In one C file:
 *     #define SENTRY_IMPLEMENTATION
 *     #include "sentry.h"
 * 
 * In other files:
 *     #include "sentry.h"
 * 
 * EXAMPLES:
 *     // Show all messages
 *     sentry_set_log_level(SENTRY_DEBUG);
 *     sentry_debug("Connection pool initialized with %d connections", 10);
 *     sentry_info("Server listening on port %d", 8080);
 *     
 *     // Show only warnings and errors
 *     sentry_set_log_level(SENTRY_WARN);
 *     sentry_debug("This won't appear");
 *     sentry_info("This won't appear either");
 *     sentry_warn("Cache miss for key: %s", key);
 *     sentry_error("Failed to connect to database: %s", error);
 * 
 * OUTPUT FORMAT:
 *     24/10/2025 18:15:40 tests/sentry_test.c:11 DEBUG This was called name=Lucas
 *     24/10/2025 18:15:40 tests/sentry_test.c:12 INFO This is info
 *     24/10/2025 18:15:41 tests/sentry_test.c:14 ERROR This is error
 *     24/10/2025 18:15:41 tests/sentry_test.c:15 WARN This is warn
 * 
 * LICENSE:
 *  MIT
 * 
 * VERSION: 1.0.0
 * AUTHOR: Wasix 
 *****************************************************************************/

#ifndef SENTRY_H
#define SENTRY_H

#ifdef SENTRY_IMPLEMENTATION
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>

pthread_mutex_t _sentry_mutex_internal = PTHREAD_MUTEX_INITIALIZER;

static const char *RESET = "\x1b[0m";
static const char *color_array[4] = {"\033[1;34m", "\033[1;35m",  "\x1b[1;33m", "\033[1;31m"};

typedef enum  {
    SENTRY_DEBUG = 0,
    SENTRY_INFO,
    SENTRY_WARN,
    SENTRY_ERROR
} SENTRY_LEVEL;

SENTRY_LEVEL sentry_log_level = SENTRY_DEBUG;

static char *_sentry_level_to_str(SENTRY_LEVEL l) {
    switch(l) {
        case SENTRY_INFO:
            return "INFO";
        case SENTRY_DEBUG:
            return "DEBUG";
        case SENTRY_ERROR:
            return "ERROR";
        case SENTRY_WARN:
            return "WARN";
    }
    return "";
}

#define sentry_debug(fmt, ...) _sentry_log(SENTRY_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__);
#define sentry_info(fmt, ...) _sentry_log(SENTRY_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__);
#define sentry_error(fmt, ...) _sentry_log(SENTRY_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__);
#define sentry_warn(fmt, ...) _sentry_log(SENTRY_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__);


void sentry_set_log_level(SENTRY_LEVEL l) {
    sentry_log_level = l;
}

static void _sentry_log(SENTRY_LEVEL level, char *filename, int line, char *fmt, ...) {
    if (level < sentry_log_level) {
        return;
    }

    pthread_mutex_lock(&_sentry_mutex_internal);
    va_list args;

    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("%d/%d/%d %02d:%02d:%02d ", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, 
        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    printf("\x1b[2;37m%s:%d%s ", filename, line, RESET);
    printf("%s%s%s ", color_array[level] ,_sentry_level_to_str(level), RESET);

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
    pthread_mutex_unlock(&_sentry_mutex_internal);
}


#endif // SENTRY_IMPLEMENTATION
#endif //SENTRY_H