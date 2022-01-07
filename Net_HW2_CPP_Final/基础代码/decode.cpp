// ������������С����ҵ
// �����-�źŽ���
// С���Ա�����գ��鳤���������ġ�������
// ���༭ʱ�䣺2022/1/7

// ��VC++6.0�Ĵ����Ϊ������ʹ��C++ʵ���źŽ���
// <stdio.h>�滻Ϊ<iostream>���Ա�֧��C++���������������
// ����������������<vector>�洢���ݣ��ַ�����<string>��������ת��

#include <windows.h>
#include<iostream>
#include<string>
#include<vector>
#include "gpio.h"
#include "decode_core.h"

using namespace std;

#pragma comment(lib,"gpiolib.lib")

// �����ڲ���������75ms
#define _Circle 75

// Ī��˹���0��1��
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

// Ī��˹�뷭��ʵ�(���иĶ���
char MorseDict(string code) {
	for (int i = 0; i < sizeof(MorseCode) / 4; i++) {
		if (0 == code.compare(MorseCode[i]))
			return 65 + i;
	}
	return '\0';
}

// ��С��Ľ���˼·
// �Ȳɼ��źţ������������������㷨���õ����
// ��һ���ź�֡�ڣ������ڼ���ƽ�ź�
// �ߵ�ƽ��Ϊ��1�����͵�ƽ��Ϊ��0��
// ��������tһ��Ϊ�ź�����T��1/3
// ��ʵ����T=75ms����ô���������Ϊ25ms
// ��һ���ź������ڣ����ۻ��3����ͬ�ĵ�ƽ�ź�
// ����111������000��
// ���ڲ��ɱ������ʵ���źſ����ǡ�11������00��
// ������Ҫ������Ϊ�������㹻
// ֻҪͳ�������ġ�1���͡�0��������
// �Ϳ��Խ�Ϊ��׼�Ļ�ԭ��ƽ�ǡ��ߡ����ǡ��͡�
// �Կɿ����ź�����Ϊ����
// �ٸ���Э��涨�ı��뷽ʽ���н���
// ���ܽ�Ϊ�ȶ��صõ���ȷ���

// Ī��˹�籨��
void decode_morse(int channel){
	vector<int>signal;                         // ��ͳ�����鲻��������<vector>֧�ֶ�̬���ɣ��Ҿ�����װʹ�÷���
	int LOW_SIGNAL_NUM = 0;                    // ͳ�������ĵ͵�ƽ�źŴ����������ж�֡����
	wait_signal_start(channel);                // �ȴ��źſ�ʼ
	while (true)
	{
		if (digitalRead(channel)==HIGH)        // �ߵ�ƽ
		{
			signal.push_back(1);               // ��������1
			LOW_SIGNAL_NUM = 0;                // ���ָߵ�ƽ���ź�һ��û�н��������ü�����
			delay_ms(_Circle/3);               // �ȴ���һ�������� _Circle/3 ms = 75/3 ms = 25ms
		}
		else if (digitalRead(channel) == LOW)  // �͵�ƽ
		{
			signal.push_back(0);               // ��������0
			LOW_SIGNAL_NUM++;                  // ���ֵ͵�ƽ���źſ��ܽ���������
			delay_ms(_Circle/3);               // �ȴ���һ�������� _Circle/3 ms = 75/3 ms = 25ms 
		}
		if (LOW_SIGNAL_NUM > 7*_Circle/25)     // Э��涨֡��������7t�͵�ƽ���������������� 7*3=21 ���͵�ƽ�ź�
		{
			break;                             // �����źŲɼ�
		}
	}
	string message = "";                       // message���ڻ����ź�ת�ɵ��ַ�����
	int ONE_NUM = 0; int ZERO_NUM = 0;         // ��1���͡�0���ļ�����
	for (unsigned int i = 0; i < signal.size(); i++) // �����ź���������Ϊsignal.size()�����޷������ͣ�ͳһʹ��unsigned int
	{
		if (1 == signal[i])                    // ������1��
		{
			ONE_NUM++;                         // ��1������������
		}
		else if (ONE_NUM > 0 && ONE_NUM <= _Circle/25) // ������0���Ҽ�������Ϊ0�����һλ�ַ�����
		{
			message.append("0");               // ���ź�ʱ��Ϊt
			ONE_NUM = 0;                       // ���á�1��������
		}
		else if (ONE_NUM >= 4 && ONE_NUM <= 3* _Circle / 25) 
		{
			message.append("1");               // ���ź�ʱ��Ϊ3t
			ONE_NUM = 0;                       // ���á�1��������
		}
		if (0 == signal[i] && i + 1 < signal.size()) // ������0��
		{                               
			ZERO_NUM++;                        // ��0������������,�������������Ҳ�е��������ã���������������ȫ��0������˷�֧������
		}
		else if (ZERO_NUM <= _Circle / 25)     // ������1�����ַ��ڼ��Ϊt
		{
			ZERO_NUM = 0;                      // ���ô������á�0��������
		}
		else if (ZERO_NUM > _Circle / 25 && ZERO_NUM <= 3* _Circle / 25) // �ַ����Ϊ3t
		{
			cout << MorseDict(message);        // �����ֵ亯��������ַ�
			message = "";                      // ������
			ZERO_NUM = 0;                      // ���á�0��������
		}
		else if (ZERO_NUM <= 7* _Circle / 25)  // ���ʼ��Ϊ7t
		{
			cout << MorseDict(message) << endl;// �������һ�����з���ʾ�ָ�
			message = "";                      // ������
			ZERO_NUM = 0;                      // ���á�0��������
		}
	}
	cout << "[INFO]Morse Code Done." << endl;  // ������
}

// ������
void decode_rz(int channel){
	vector<int>signal;                         // ����<vector>�洢�ź�����
	int LOW_SIGNAL_NUM = 0;                    // ͳ�������ĵ͵�ƽ�źŴ����������ж�֡����
	wait_signal_start(channel);                // �ȴ��źſ�ʼ
	while (true)
	{
		if (digitalRead(channel) == HIGH)      // �ߵ�ƽ
		{
			signal.push_back(1);               // ��������1
			LOW_SIGNAL_NUM = 0;                // ���ָߵ�ƽ���ź�һ��û�н��������ü�����
			delay_ms(_Circle/3);               // �ȴ���һ�������� _Circle/3 ms = 75/3 ms = 25ms
		}
		else if (digitalRead(channel) == LOW)  // �͵�ƽ
		{
			signal.push_back(0);               // ��������0
			LOW_SIGNAL_NUM++;                  // ���ֵ͵�ƽ���źſ��ܽ���������
			delay_ms(_Circle/3);			   // �ȴ���һ�������� _Circle/3 ms = 75/3 ms = 25ms
		}
		if (LOW_SIGNAL_NUM > _Circle/25)       // Э��涨֡�����͵�ƽ2s���ϣ�ʵ�ʳ��ֳ��ȴ���1t�ĵ͵�ƽ��˵��֡����
		{
			break;                             // �����źŲɼ�
		}
	}
	string message = "";                       // message���ڻ����ź�ת�ɵ��ַ�����
	int ONE_NUM = 0; int ASC_NUM = 0;          // ��1���ļ�������ASCII�롰��8��1���ı�־����
	for (unsigned int i = 0; i < signal.size(); i++)
	{
		if (1 == signal[i])                    // ������1��
		{
			ONE_NUM++;                         // ��1������������
		}
		else if (ONE_NUM > 0 && ONE_NUM <= _Circle/25) // ����0�Ҽ�������Ϊ0�����8λASCII���1λ
		{
			message.append("0");               // 1t��ʾ0
			ONE_NUM = 0;                       // ���á�1��������
			ASC_NUM++;                         // ASCII���������������
		}
		else if (ONE_NUM > _Circle/25 && ONE_NUM <= 2*_Circle/25) 
		{
			message.append("1");               // 2t��ʾ1
			ONE_NUM = 0;                       // ���á�1��������
			ASC_NUM++;                         // ASCII���������������
		}
		if (8 == ASC_NUM)                      // ASCII����8
		{
			int result = 0;                    // �ַ��͵Ķ����Ʊ���ת��Ϊʮ������ֵ
			for (int j = 7; j >= 0; j--)
			{
				result += (int)pow(2, j) * ((int)message[7 - j] - 48);
			}
			cout << (char)result;              // ����C�������ԣ�ֱ�������ӦASCII��
			message = "";                      // ������
			ASC_NUM = 0;                       // ����ASCII�����������
		}
	}
	cout << endl;                              // ��һ�����з�
	cout << "[INFO]RZ-Code Done." << endl;     // ������
}

//�ǹ�����
void decode_nrz(int channel, int clk_channel){
	vector<string>signal;                      // ����������
	int LOW_SIGNAL_NUM = 0;                    // �͵�ƽ�������ж�֡����
	wait_signal_start(channel);                // �ȴ��źſ�ʼ
	while (true)
	{
		if (digitalRead(channel) == HIGH && digitalRead(clk_channel) == HIGH) // �ǹ������������źţ�������2λ��λ���ȴ�ʱ���źţ��ٴ������ź�
		{
			signal.push_back("1"); // ʱ���ź�1
			signal.push_back("1"); // �����ź�1
			LOW_SIGNAL_NUM = 0;    // ���ָߵ�ƽ���ź�һ��û�н��������ü�����
			delay_ms(_Circle/3);   // �ȴ���һ�������� _Circle/3 ms = 75/3 ms = 25ms
		}
		else if (digitalRead(channel) == LOW && digitalRead(clk_channel) == HIGH) // ʱ���ź�0 �����ź�1
		{
			signal.push_back("1");
			signal.push_back("0");
			LOW_SIGNAL_NUM++;      // ���ֵ͵�ƽ���źſ��ܽ���������
			delay_ms(_Circle / 3);
		}
		else if (digitalRead(channel) == HIGH && digitalRead(clk_channel) == LOW) // ʱ���ź�0 �����ź�1
		{
			signal.push_back("0");
			signal.push_back("1");
			LOW_SIGNAL_NUM = 0;
			delay_ms(_Circle / 3);
		}
		else                       // ʱ���ź�0 �����ź�0
		{
			signal.push_back("0");
			signal.push_back("0");
			LOW_SIGNAL_NUM++;
			delay_ms(_Circle / 3);
		}
		if (LOW_SIGNAL_NUM > 8*_Circle / 25) // Э��涨֡��������8��0������������ 8*3 = 24���͵�ƽ�ź�
		{
			break;                 // �����źŲɼ�
		}
	}
	string message = "";           // message���ڻ����ź�ת�ɵ��ַ�����
	int ASC_NUM = 0;               // ASCII�����������
	for (unsigned int i = 2; i < signal.size()-1; i=i+2) // ��������һ�飬ѭ����2Ϊ������Ϊ����������ӵ�2�鿪ʼ��������ǰ�����
	{
		if (signal[i - 2] != signal[i]) // ���ʱ���źŷ����仯��˵������bit�ѱ�
		{
			message.append(signal[i - 1]); // ��ǰһ�������ź�д�뻺�棨1��0�������壬����Ҫ���֣�
			ASC_NUM++;             // ASCII���������������
		}
		else;
		if (ASC_NUM <= 4)          // ʱ���ź�δ�仯������һ�������ڣ�����Ƿ���Ҫ����ַ�
		{
			message = "";          // ֡��ʼ��4����1��������ASCII��ǰ4�α仯��ջ���
		}
		else if (0 == (ASC_NUM - 4) % 8 && message!="") // ����ģ�����ж�ASCII���Ƿ���Ҫ���
		{
			int result = 0;        // �ַ��͵Ķ����Ʊ���ת��Ϊʮ������ֵ
			for (int j = 7; j >= 0; j--)
			{
				result += (int)pow(2, j) * ((int)message[7 - j] - 48);
			}
			if (result != 0)
			{
				cout << (char)result;
			}
			message = "";
		}
	}
	cout << endl;
	cout << "[INFO]NRZ-Code Done." << endl; // ������
}

//����˹����
void decode_manch(int channel){
	vector<int>signal;            // ���������ź�����
	int HIGH_SIGNAL_NUM = 0; int LOW_SIGNAL_NUM = 0; // �ߵ͵�ƽ�źż�����
	wait_signal_start(channel);   // �ȴ��źſ�ʼ 
	while (true)
	{
		if (digitalRead(channel) == HIGH) // �ߵ�ƽ
		{
			signal.push_back(1); // ��������1
			HIGH_SIGNAL_NUM++; LOW_SIGNAL_NUM = 0; // �ߵ�ƽ�������͵�ƽ����
			delay_ms(_Circle/15);// �ȴ���һ�������� _Circle/15 ms = 75/15 ms = 5ms ����˹�ر�������Ƚ�Ѹ�٣����Ӳ����㲶׽��ƽ�仯
		}
		else if (digitalRead(channel) == LOW) // �͵�ƽ
		{
			signal.push_back(0);
			HIGH_SIGNAL_NUM = 0; LOW_SIGNAL_NUM++; // �͵�ƽ�������ߵ�ƽ����
			delay_ms(_Circle/15);
		}
		if (HIGH_SIGNAL_NUM > 20 || LOW_SIGNAL_NUM > 20) // Э��涨֡�������bit��ƽ����1.2s������˹�ر����ƽ�仯��Ƶ����������ƽ������������ֵ20��������Ϊ֡����
		{
			break;               // �����źŲɼ�
		}

	}
	int SIGNAL = signal[0]; int SIGNAL_NUM = 0; // ����˹�ر���Ƚϸ��ӣ�ԭʼ������Ҫ����һ���ļӹ����ڽ��
	vector<int> temp_1;          // ��һ��������С�����ݿ飬��������5����1����0��,�ϲ�Ϊ1����1����0��
	for (unsigned int i = 0; i < signal.size(); i++) { // SIGNAL��ʾ��������Ϊ��1����0����SIGNAL_NUM����һ�����ݿ�ĳ���
		if (signal[i] == SIGNAL && SIGNAL_NUM <= 5) // ���ݷ����˱仯�����ݿ���δ�ﵽ��󳤶�
		{
			SIGNAL_NUM++;        // ���ݳ��ȼ���������
		}
		else
		{
			temp_1.push_back(SIGNAL); // ���ϲ��������д��������
			SIGNAL = signal[i];       // ˢ�����ݵ�ֵ
			SIGNAL_NUM = 1;           // ��ʱ���ݿ鳤���Ѿ�Ϊ1
		}
	}
	vector<int> temp_2;  string str = ""; // �ڶ���������Э��涨�������ͣ�10����ʾ1�������ߣ�01����ʾ0�����ź�ת��Ϊ�ַ�����
	for (unsigned int i = 0; i < temp_1.size(); i++) {
		if (str.size() < 2) {         // �����ַ������Ȳ���2
			str += to_string(temp_1[i]);  // д���µ�����
		}
		else {                        // �����ַ����������������бȽ�ת��
			if ("10" == str)          // ��10����ʾ1
				temp_2.push_back(1);
			else if ("01" == str)     // ��01����ʾ0
				temp_2.push_back(0);
			else if ("00" == str)     // ��00����ʾ֡����
				temp_2.push_back(-1);
			str = "";                 // ������
			str += to_string(temp_1[i]); // д��������
		}
	}
	string message = "";              // message���ڻ����ź�ת�ɵ��ַ�����
	for (unsigned int i = 4; i < temp_2.size(); i++) {
		if (message.size() < 8) {     // ����<string>�⺯����ͨ�������ַ��������ж��Ƿ���Ҫ���
			message.append(to_string(temp_2[i])); //���������д���ַ�����
		}
		else {                        // ��8λ��ʼ����ַ�
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
			message.append(to_string(temp_2[i]));
		}
		if (temp_2[i] == -1)          // ������ֹ���룬����ѭ��
			break;
	}
	cout << endl;
	cout << "[INFO]Manch Code Done." << endl; // ������
}

int main(int argc, char* argv[])
{
	// �����ȴ��豸���룬δ��������ʾ������һ��ʱ�䣬�ٴμ��
	while(!open_dev()) {
		cout << "Device open failed, try to re-plug it!" << endl;
		Sleep(1500);
	}
	
	// ������ʾ��s
	cout << "Device opened." << endl;

	// �������棬�������ֿ�ʼ����
	char chc;
	while(true){
		printf("1: Morse \r\n");
		printf("2: Manchester \r\n");
		printf("3: NRZ \r\n");
		printf("4: RZ \r\n");
		printf("0: Quit\r\n");
		fflush(stdin);
		cin >> chc;
		if(chc == '0')
			break;
		switch(chc){
		case '1':decode_morse(MORSE_PIN);break;
		case '2':decode_manch(MANCH_PIN);break;
		case '3':decode_nrz(NRZER_PIN, NRCLK_PIN);break;
		case '4':decode_rz(RZERO_PIN);break;
		default:continue;
		}
		getchar();
	}
	close_dev();
	return 0;
}