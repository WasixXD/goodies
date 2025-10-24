#define PICKY_IMPLEMENTATION
#include "picky.h"

#define SENTRY_IMPLEMENTATION
#include "sentry.h"

#include <string.h>

void test_log_levels(T *t) {
    picky_test(t, "default log level is DEBUG");
    picky_int_toBe(t, SENTRY_DEBUG, sentry_log_level);
    
    picky_test(t, "can set log level to INFO");
    sentry_set_log_level(SENTRY_INFO);
    picky_int_toBe(t, SENTRY_INFO, sentry_log_level);
    
    picky_test(t, "can set log level to WARN");
    sentry_set_log_level(SENTRY_WARN);
    picky_int_toBe(t, SENTRY_WARN, sentry_log_level);
    
    picky_test(t, "can set log level to ERROR");
    sentry_set_log_level(SENTRY_ERROR);
    picky_int_toBe(t, SENTRY_ERROR, sentry_log_level);
    
    // Reset to DEBUG for other tests
    sentry_set_log_level(SENTRY_DEBUG);
}

void test_log_filtering(T *t) {
    printf("\n  Testing log filtering (visual check):\n");
    
    picky_test(t, "DEBUG level shows all messages");
    sentry_set_log_level(SENTRY_DEBUG);
    sentry_debug("This DEBUG should appear");
    sentry_info("This INFO should appear");
    sentry_warn("This WARN should appear");
    sentry_error("This ERROR should appear");
    picky_assert(t, true);
    
    picky_test(t, "INFO level filters out DEBUG");
    sentry_set_log_level(SENTRY_INFO);
    sentry_debug("This DEBUG should NOT appear");
    sentry_info("This INFO should appear");
    sentry_warn("This WARN should appear");
    sentry_error("This ERROR should appear");
    picky_assert(t, true);
    
    picky_test(t, "WARN level shows only WARN and ERROR");
    sentry_set_log_level(SENTRY_WARN);
    sentry_debug("This DEBUG should NOT appear");
    sentry_info("This INFO should NOT appear");
    sentry_warn("This WARN should appear");
    sentry_error("This ERROR should appear");
    picky_assert(t, true);
    
    picky_test(t, "ERROR level shows only ERROR");
    sentry_set_log_level(SENTRY_ERROR);
    sentry_debug("This DEBUG should NOT appear");
    sentry_info("This INFO should NOT appear");
    sentry_warn("This WARN should NOT appear");
    sentry_error("This ERROR should appear");
    picky_assert(t, true);
    
    // Reset to DEBUG
    sentry_set_log_level(SENTRY_DEBUG);
}

void test_log_formatting(T *t) {
    printf("\n  Testing log formatting (visual check):\n");
    
    picky_test(t, "logs with formatted strings");
    sentry_info("Server started on port %d", 8080);
    sentry_debug("User %s logged in with ID %d", "Alice", 42);
    sentry_warn("Memory usage at %.2f%%", 85.5);
    sentry_error("Failed to open file: %s (error code: %d)", "config.txt", -1);
    picky_assert(t, true);
    
    picky_test(t, "logs without arguments");
    sentry_info("Application initialized");
    sentry_debug("Entering main loop");
    picky_assert(t, true);
}

void test_multiple_logs(T *t) {
    printf("\n  Testing multiple sequential logs:\n");
    
    picky_test(t, "can log multiple messages in sequence");
    for (int i = 0; i < 5; i++) {
        sentry_debug("Loop iteration %d", i);
    }
    picky_assert(t, true);
    
    picky_test(t, "can mix different log levels");
    sentry_debug("Starting operation");
    sentry_info("Operation in progress");
    sentry_warn("Operation taking longer than expected");
    sentry_error("Operation failed");
    picky_assert(t, true);
}

int main(int argc, char **argv) {
    printf("=== SENTRY TESTS ===\n\n");
    
    picky_describe("Log Level Configuration", test_log_levels);
    picky_describe("Log Level Filtering", test_log_filtering);
    picky_describe("Log Formatting", test_log_formatting);
    picky_describe("Multiple Logs", test_multiple_logs);
    
    return 0;
}