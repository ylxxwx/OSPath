#ifndef _TRACE_H_
#define _TRACE_H_

#include <stdarg.h>
 
void trace(char *fmt, ...);
void stdoutput(char *fmt, ...);

void enable_trace();
void disable_trace();

#endif // _TRACE_H_
