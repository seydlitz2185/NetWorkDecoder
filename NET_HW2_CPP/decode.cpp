#include <windows.h>
//#include <stdio.h>
#include<iostream>
#include<vector>
#include "gpio.h"
#include "decode_core.h"

using namespace std;

#pragma comment(lib,"gpiolib.lib")
//莫尔斯码表，0短1长
#define _A "01"
#define _B "1000"
#define _C "1010"
#define _D "100"
#define _E "0"
#define _F "0010"
#define _G "110"
#define _H "0000"
#define _I "00"
#define _J "0111"
#define _K "101"
#define _L "0100"
#define _M "11"
#define _N "10"
#define _O "111"
#define _P "0110"
#define _Q "1101"
#define _R "010"
#define _S "000"
#define _T "1"
#define _U "001"
#define _V "0001"
#define _W "011"
#define _X "1001"
#define _Y "1011"
#define _Z "1100"

const char* MorseCode []={_A,_B,_C,_D,_E,_F,_G,_H,_I,_J,_K,_L,_M,_N,_O,_P,_Q,_R,_S,_T,_U,_V,_W,_X,_Y,_Z};

//char MorseDict(char* code){
//	for(int i=0;i<sizeof(MorseCode)/4;i++){
//		if(0==strcmp(code, MorseCode[i]))
//			return 97+i;
//	}
//	return '\0';
//}

char MorseDict(string code) {
	for (int i = 0; i < sizeof(MorseCode) / 4; i++) {
		if (0 == code.compare(MorseCode[i]))
			return 65 + i;
	}
	return '\0';
}

//检测电平信号输出图形
//void signal_detect(){
//
//	while(true){
//
//		int channel=0;
//		char chc;
//		printf("\r\nSelect channel: \r\n");
//
//		printf("1: NRZ \r\n");
//		printf("2: Morse \r\n");
//		printf("3: Manchester \r\n");
//		printf("4: RZ \r\n");
//		printf("5: CLK \r\n");
//		printf("6: DATA_IN \r\n");
//		printf("0: Back \r\n");
//		fflush(stdin);
//
//		scanf("%c", &chc);
//
//		if(chc == '0'){
//			printf("\r\n");
//			break;
//		}
//
//		switch(chc){
//			case '1':channel=NRZER_PIN;break;
//			case '2':channel=MORSE_PIN;break;
//			case '3':channel=MANCH_PIN;break;
//			case '4':channel=RZERO_PIN;break;
//			case '5':channel=NRCLK_PIN;break;
//			case '6':channel=CIN_PIN;break;
//			default:continue;
//		}
//
//		//等待信号出现
//		while(digitalRead(channel)==HIGH);
//		while(digitalRead(channel)==LOW);
//		delay_us(25000);
//
//		while(true){
//			if( GetAsyncKeyState(VK_ESCAPE) & 0x8000 ) //ESC键结束
//				break;
//			printf(digitalRead(channel)?"▉":"▓");
//			fflush(stdout);
//			delay_us(5000);
//		}
//	}
//}

//莫尔斯电报码
void decode_morse(int channel){
	vector<int>signal; int i = 0; int k = 0;
	wait_signal_start(channel);
	while (true)
	{
		if (digitalRead(channel)==HIGH)
		{
			signal.push_back(1); 
			//printf("%d\n", signal[i]); 
			i++; k = 0;
			delay_ms(25);
		}
		else if (digitalRead(channel) == LOW)
		{
			signal.push_back(0); 
			//printf("%d\n", signal[i]);
			i++; k++;
			delay_ms(25);
		}
		if (k > 21)
		{
			break;
		}
	}
	string message = "";
	int count = 0; int flag = 0;
	for (unsigned int j = 0; j < signal.size(); j++)
	{
		if (1 == signal[j])
		{
			count++;
		}
		else if (count > 0 && count < 4)
		{
			message.append("0");
			count = 0;
		}
		else if (count >= 4 && count <= 9)
		{
			message.append("1");
			count = 0;
		}
		if (0 == signal[j] && j + 1 < signal.size())
		{
			flag++;
		}
		else if (flag <= 3)
		{
			flag = 0;
		}
		else if (flag > 3 && flag <= 9)
		{
			cout << MorseDict(message);
			message = "";
			flag = 0;
		}
		else if (flag <= 21)
		{
			cout << MorseDict(message) << endl;
			message = "";
			flag = 0;
		}
	}
	printf("Decode finished.Rome wasn't built in a day.\r\n");
}

//归零码
void decode_rz(int channel){
	vector<int>signal; int i = 0; int k = 0;
	wait_signal_start(channel);
	while (true)
	{
		if (digitalRead(channel) == HIGH)
		{
			signal.push_back(1);
			//printf("%d\n", signal[i]);
			i++; k = 0;
			delay_ms(25);
		}
		else if (digitalRead(channel) == LOW)
		{
			signal.push_back(0);
			//printf("%d\n", signal[i]);
			i++; k++;
			delay_ms(25);
		}
		if (k >= 4)
		{
			break;
		}
	}
	string message = "";
	int count = 0; int flag = 0;
	for (unsigned int j = 0; j < signal.size(); j++)
	{
		if (1 == signal[j])
		{
			count++;
		}
		else if (count > 0 && count < 4)
		{
			message.append("0");
			count = 0;
			flag++;
		}
		else if (count >= 4 && count <= 6)
		{
			message.append("1");
			count = 0;
			flag++;
		}
		if (8 == flag)
		{
			int result = 0;
			for (int k = 7; k >= 0; k--)
			{
				result += (int)pow(2, k) * ((int)message[7 - k] - 48);
			}
			cout << (char)result;
			message = "";
			flag = 0;
		}
		if (j + 1 == signal.size())
		{
			cout << endl;
		}
	}
	printf("Decode finished.Rome wasn't built in a day.\r\n");
}

//非归零码
void decode_nrz(int channel, int clk_channel){
	vector<string>signal; int i = 0; int k = 0;
	wait_signal_start(channel);
	while (true)
	{
		if (digitalRead(channel) == HIGH && digitalRead(clk_channel) == HIGH)
		{
			//signal.insert(signal.end(), 1, 1);
			signal.push_back("1");
			signal.push_back("1");
			//printf("%d\t%d\n", signal[i], signal[i + 1]);
			i = i + 2; k = 0;
			delay_ms(25);
		}
		else if (digitalRead(channel) == LOW && digitalRead(clk_channel) == HIGH)
		{
			//signal.insert(signal.end(),0,1);
			signal.push_back("1");
			signal.push_back("0");
			//printf("%d\t%d\n", signal[i], signal[i + 1]);
			i = i + 2; k++;
			delay_ms(25);
		}
		else if (digitalRead(channel) == HIGH && digitalRead(clk_channel) == LOW)
		{
			//signal.insert(signal.end(), 1, 0);
			signal.push_back("0");
			signal.push_back("1");
			//printf("%d\t%d\n", signal[i], signal[i + 1]);
			i = i + 2; k = 0;
			delay_ms(25);
		}
		else
		{
			//signal.insert(signal.end(), 0, 0);
			signal.push_back("0");
			signal.push_back("0");
			//printf("%d\t%d\n", signal[i], signal[i + 1]);
			i = i + 2; k++;
			delay_ms(25);
		}
		if (k > 24)
		{
			break;
		}
	}

	string message = "";
	int flag = 0;
	for (unsigned int j = 2; j < signal.size()-1; j=j+2)
	{
		if (signal[j - 2] != signal[j])
		{
			message.append(signal[j - 1]);
			flag++;
		}
		else;
		if (flag <= 4)
		{
			message = "";
		}
		else if (0 == (flag - 4) % 8 && message!="")
		{
			int result = 0;
			for (int k = 7; k >= 0; k--)
			{
				result += (int)pow(2, k) * ((int)message[7 - k] - 48);
			}
			if (result != 0)
			{
				cout << (char)result;
			}
			message = "";
		}
		if (j + 1 >= signal.size() - 1)
		{
			cout << endl;
		}
	}
	printf("Decode finished.Rome wasn't built in a day.\r\n");
}

//曼彻斯特码
void decode_manch(int channel){
	vector<int>signal; int i = 0; int k = 0;
	wait_signal_start(channel);
	while (true)
	{
		if (digitalRead(channel) == HIGH)
		{
			signal.push_back(1);
			printf("%d\n", signal[i]);
			i++; k++;
			delay_ms(3);
		}
		else if (digitalRead(channel) == LOW)
		{
			signal.push_back(0);
			printf("%d\n", signal[i]);
			i++; k++;
			delay_ms(3);
		}
	}
	printf("Talk is cheap, show me the code.");
}

//void light_demo(){
//	digitalWrite(COUT_PIN0, LOW);
//	digitalWrite(COUT_PIN1, LOW);
//	digitalWrite(COUT_PIN2, LOW);
//	for(int i=0;i<200;i++){
//		digitalWrite(COUT_PIN0, i%3%2?HIGH:LOW);
//		digitalWrite(COUT_PIN1, (i+1)%3%2?HIGH:LOW);
//		digitalWrite(COUT_PIN2, (i+2)%3%2?HIGH:LOW);
//		delay_ms(50);
//	}
//	digitalWrite(COUT_PIN0, LOW);
//	digitalWrite(COUT_PIN1, LOW);
//	digitalWrite(COUT_PIN2, LOW);
//}


int main(int argc, char* argv[])
{
	//if(!open_dev()){
	//	printf("Device open failed, try to re-plug it!\r\n");
	//	return -1;
	//}
	//else
	//	printf("Device opened.\r\n");

	while(!open_dev()) {
		printf("Device open failed, try to re-plug it!\r\n");
		Sleep(2200);
	}
	
	printf("Device opened.\r\n");

	char chc;
	while(true){
		printf("1: Morse \r\n");
		printf("2: Manchester \r\n");
		printf("3: NRZ \r\n");
		printf("4: RZ \r\n");
		//printf("5: Signal detect \r\n");
		//printf("6: Output DEMO \r\n");
		printf("0: Quit\r\n");
		fflush(stdin);
		scanf("%c", &chc);
		if(chc == '0')
			break;
		switch(chc){
		case '1':decode_morse(MORSE_PIN);break;
		case '2':decode_manch(MANCH_PIN);break;
		case '3':decode_nrz(NRZER_PIN, NRCLK_PIN);break;
		case '4':decode_rz(RZERO_PIN);break;
		//case '5':signal_detect();break;
		//case '6':light_demo();break;
		default:continue;
		}
		getchar();

	}
	close_dev();
	return 0;
}