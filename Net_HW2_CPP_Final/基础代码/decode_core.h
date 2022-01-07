#ifndef ___DECODE_CORE___
#define ___DECODE_CORE___

#define TIMEOUT 1200 //1200ms

bool wait_low(int channel);
bool wait_high(int channel);
bool wait_jump(int channel, int state=UNKNOWN);
void wait_signal_end(int channel,LONGLONG timeout=TIMEOUT);
void wait_signal_start(int channel);
#endif