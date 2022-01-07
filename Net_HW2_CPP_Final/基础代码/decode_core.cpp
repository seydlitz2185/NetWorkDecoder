#include <windows.h>
#include <stdio.h>
#include "gpio.h"
#include "decode_core.h"

//�ȴ��ߵ�ƽ���ֻ�ʱ
bool wait_high(int channel){
	ULONGLONG cnt = get_ms();
	while(digitalRead(channel)==LOW){
		if(get_ms() - cnt > TIMEOUT)
			return false;
	}
	return true;
}

//�ȴ��͵�ƽ���ֻ�ʱ
bool wait_low(int channel){
	ULONGLONG cnt = get_ms();
	while(digitalRead(channel)==HIGH){
		if(get_ms() - cnt > TIMEOUT)
			return false;
	}
	return true;
}

//�ȴ�������ֻ�ʱ
bool wait_jump(int channel, int state){
	if(UNKNOWN == state) state=digitalRead(channel);
	LONGLONG cnt = get_ms();
	while(digitalRead(channel)==state){
		if(get_ms() - cnt > TIMEOUT)
			return false;
	}
	return true;
}

//�ȴ���һ���źŽ���������timeout���ϵĵ͵�ƽ
void wait_signal_end(int channel,LONGLONG timeout){

	LONGLONG cnt = get_ms();
	while(true){
		if(digitalRead(channel)){
			cnt = get_ms();// �����ߵ�ƽ���¼�ʱ
		}
		if(get_ms() - cnt > timeout){
			break;
		}
	}
}

//�ȴ��źſ�ʼ(��һ���źŽ�����ĵ�һ���ߵ�ƽ����)
void wait_signal_start(int channel){
	printf("Wait next frame...\r\n");
	wait_signal_end(channel);
	printf("Wait frame head...\r\n");
	while(digitalRead(channel)==LOW);
}