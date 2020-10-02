#include <stdarg.h>
#include <stdio.h>

#define DEBUG 1

/// Function that prints the specified string to stdout.
///
/// @param format The formatted data to print to the standard output.
void Print(const char* format, ...) {
#if DEBUG
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
#endif
}
