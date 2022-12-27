#include "inter.h"

void enable_interrupt() {
  asm volatile ("sti");
}

void disable_interrupt() {
  asm volatile ("cli");
}
