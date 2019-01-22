#include <stdio.h>

#define TIMER (*(uint64_t volatile *)0x40004000)
#define TIMER_CMP (*(uint64_t volatile *)0x40004008)
#define TRACE (*(unsigned char volatile *)0x40000000)

volatile uint32_t ticks = 0;

int _write(int file, const char *ptr, int len) {
  int x;

  for (x = 0; x < len; x++) {
    TRACE =  *ptr++;
  }

  return (len);
}

void timer_ISR() {
  volatile uint32_t timer_value;

  ticks++;

  timer_value = TIMER;

  // timer is in nanoseconds, set to 1 ms.
  // comparator value fixed to take into account number of instructions executed
  TIMER_CMP = timer_value + 610;
}

void register_timer_isr() {
  asm volatile("la t0, TIMER_CMP_INT");
  asm volatile("csrw mtvec, t0");
  asm volatile("li t1, 0x888");
  asm volatile("csrw mie, t1");
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
  } while (timer_value < start_time + 2000000);

  printf("Timer: %ld ns\n", timer_value);
  printf("ticks: %ld\n", ticks);

  if (ticks > 2100) {
    printf("Test OK!\n");
  }

  printf("Finish\n");

  asm volatile ("ecall");
  return 0;
}
