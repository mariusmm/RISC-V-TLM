#include <stdio.h>

#define TIMER (*(uint64_t *)0x40004000)
#define TIMER_CMP (*(uint64_t *)0x40004008)
#define TRACE (*(unsigned char *)0x40000000)

volatile int ticks = 0;

int _write(int file, const char *ptr, int len) {
  int x;

  for (x = 0; x < len; x++) {
    TRACE =  *ptr++;
  }

  return (len);
}

void timer_ISR() {
  uint32_t timer_value;

  ticks++;
  if (ticks < 10) {
    timer_value = TIMER;
    // timer is in nanoseconds, set to 1 ms.
    // comparator value fixed to take into account number of instructions executed
    TIMER_CMP = timer_value + 590; 
  }
}

void register_timer_isr() {
  asm volatile("la t0, TIMER_CMP_INT \n csrw mtvec, t0");
  asm volatile("li t1, 0x888 \n csrw mie, t1");
}

int main(void) {

  uint32_t timer_value;
  uint32_t start_time;

  register_timer_isr();

  start_time = TIMER;
  TIMER_CMP = start_time + 10000;
  printf("set timer to %ld ns\n", start_time + 10000);
 
  do {
    timer_value = TIMER;
  } while (timer_value < start_time + 200000);

  printf("Timer: %ld ns\n", timer_value);
  printf("ticks: %ld\n", ticks);
  asm volatile ("ecall");
  return 0;

}
