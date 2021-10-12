#include "Timer.h"
#include <System/SystemTime.h>
#include <Drivers/idt.h>
double TimeSinceBoot = 0;
const long long BaseFrequency = 1193182;

int Divisor = 65535;

void Sleepd(double seconds)
{
  double startTime = TimeSinceBoot;
  while (TimeSinceBoot < startTime + seconds)
  {
    asm("hlt");
  }
}
double div(double a, double b)
{
  a *= 1000;
  b *= 1000;
  return (a / b);
}
void Sleep(long long milliseconds)
{
  Sleepd((double)div(milliseconds, 1000));
}

void SetDivisor(int divisor)
{
  if (divisor < 100)
    divisor = 100;
  Divisor = divisor;
  outportb(0x40, (divisor & 0x00ff));
  asm("nop");
  outportb(0x40, ((divisor & 0xff00) >> 8));
}

long long GetFrequency()
{
  return div(BaseFrequency, Divisor);
}

void SetFrequency(long long frequency)
{
  SetDivisor(div(BaseFrequency, frequency));
}

void Tick()
{
  TimeSinceBoot += div(1, GetFrequency());
}
void timer_handler()
{
  __asm__("pusha"); // save registers
  Tick();
  outportb(MASTER_PIC_CMD_PORT, END_OF_INTERRUPT);
  outportb(SLAVE_PIC_CMD_PORT, END_OF_INTERRUPT);
  __asm__("popa");  // restore registers
  __asm__("leave"); // restore stack
  __asm__("iret");  // interrupt return - important
}