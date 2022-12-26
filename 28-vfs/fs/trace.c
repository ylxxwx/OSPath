#include "trace.h"
#include "screen.h"

static int trace_on = 0;

void enable_trace() {
  trace_on = 1;
}

void disable_trace() {
  trace_on = 0;
}

void trace(char *fmt, ...) {
  va_list args;
  if (trace_on) {
    va_start (args, fmt);
    kprintf_args (fmt, args);
    va_end (args);
  }
}

void stdoutput(char *fmt, ...) {
  va_list args;
  va_start (args, fmt);
  kprintf_args (fmt, args);
  va_end (args);
}
