#include <windows.h>
#include <stdio.h>
#include "gpio.h"
#include "decode_core.h"

//等待高电平出现或超时
bool wait_high(int channel){
	ULONGLONG cnt = get_ms();
	while(digitalRead(channel)==LOW){
		if(get_ms() - cnt > TIMEOUT)
			return false;
	}
	return true;
}

//等待低电平出现或超时
bool wait_low(int channel){
	ULONGLONG cnt = get_ms();
	while(digitalRead(channel)==HIGH){
		if(get_ms() - cnt > TIMEOUT)
			return false;
	}
	return true;
}

//等待跳变出现或超时
bool wait_jump(int channel, int state){
	if(UNKNOWN == state) state=digitalRead(channel);
	LONGLONG cnt = get_ms();
	while(digitalRead(channel)==state){
		if(get_ms() - cnt > TIMEOUT)
			return false;
	}
	return true;
}

//等待上一段信号结束，发生timeout以上的低电平
void wait_signal_end(int channel,LONGLONG timeout){

	LONGLONG cnt = get_ms();
	while(true){
		if(digitalRead(channel)){
			cnt = get_ms();// 遇到高电平重新计时
		}
		if(get_ms() - cnt > timeout){
			break;
		}
	}
}

//等待信号开始(上一段信号结束后的第一个高电平出现)
void wait_signal_start(int channel){
	printf("Wait next frame...\r\n");
	wait_signal_end(channel);
	printf("Wait frame head...\r\n");
	while(digitalRead(channel)==LOW);
}