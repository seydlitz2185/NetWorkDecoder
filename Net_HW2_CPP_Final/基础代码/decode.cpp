// 计算机网络基础小组作业
// 物理层-信号解码
// 小组成员：高艺（组长）、于世文、王洁仪
// 最后编辑时间：2022/1/7

// 以VC++6.0的代码库为基础，使用C++实现信号解码
// <stdio.h>替换为<iostream>，以便支持C++的输入输出流函数
// 额外引入了向量库<vector>存储数据，字符串库<string>处理类型转换

#include <windows.h>
#include<iostream>
#include<string>
#include<vector>
#include "gpio.h"
#include "decode_core.h"

using namespace std;

#pragma comment(lib,"gpiolib.lib")

// 定周期采样，周期75ms
#define _Circle 75

// 莫尔斯码表，0短1长
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

// 莫尔斯码翻译词典(略有改动）
char MorseDict(string code) {
	for (int i = 0; i < sizeof(MorseCode) / 4; i++) {
		if (0 == code.compare(MorseCode[i]))
			return 65 + i;
	}
	return '\0';
}

// 本小组的解码思路
// 先采集信号，保存下来，再运行算法，得到结果
// 在一个信号帧内，定周期检测电平信号
// 高电平记为“1”，低电平记为“0”
// 采样周期t一般为信号周期T的1/3
// 本实验中T=75ms，那么采样间隔即为25ms
// 在一个信号周期内，理论获得3个相同的电平信号
// 即“111”、“000”
// 由于不可避免的误差，实际信号可能是“11”、“00”
// 不过不要紧，因为采样点足够
// 只要统计连续的“1”和“0”的数量
// 就可以较为精准的还原电平是“高”还是“低”
// 以可靠的信号数据为基础
// 再根据协议规定的编码方式进行解码
// 便能较为稳定地得到正确结果

// 莫尔斯电报码
void decode_morse(int channel){
	vector<int>signal;                         // 传统的数组不够灵活，向量<vector>支持动态生成，且经过封装使用方便
	int LOW_SIGNAL_NUM = 0;                    // 统计连续的低电平信号次数，用于判断帧结束
	wait_signal_start(channel);                // 等待信号开始
	while (true)
	{
		if (digitalRead(channel)==HIGH)        // 高电平
		{
			signal.push_back(1);               // 向量存入1
			LOW_SIGNAL_NUM = 0;                // 出现高电平，信号一定没有结束，重置计数器
			delay_ms(_Circle/3);               // 等待下一个采样点 _Circle/3 ms = 75/3 ms = 25ms
		}
		else if (digitalRead(channel) == LOW)  // 低电平
		{
			signal.push_back(0);               // 向量存入0
			LOW_SIGNAL_NUM++;                  // 出现低电平，信号可能结束，计数
			delay_ms(_Circle/3);               // 等待下一个采样点 _Circle/3 ms = 75/3 ms = 25ms 
		}
		if (LOW_SIGNAL_NUM > 7*_Circle/25)     // 协议规定帧结束超过7t低电平，即连续读到超过 7*3=21 个低电平信号
		{
			break;                             // 结束信号采集
		}
	}
	string message = "";                       // message用于缓存信号转成的字符编码
	int ONE_NUM = 0; int ZERO_NUM = 0;         // “1”和“0”的计数器
	for (unsigned int i = 0; i < signal.size(); i++) // 遍历信号向量，因为signal.size()返回无符号类型，统一使用unsigned int
	{
		if (1 == signal[i])                    // 读到“1”
		{
			ONE_NUM++;                         // “1”计数器自增
		}
		else if (ONE_NUM > 0 && ONE_NUM <= _Circle/25) // 遇到“0”且计数器不为0，输出一位字符编码
		{
			message.append("0");               // 短信号时长为t
			ONE_NUM = 0;                       // 重置“1”计数器
		}
		else if (ONE_NUM >= 4 && ONE_NUM <= 3* _Circle / 25) 
		{
			message.append("1");               // 长信号时长为3t
			ONE_NUM = 0;                       // 重置“1”计数器
		}
		if (0 == signal[i] && i + 1 < signal.size()) // 读到“0”
		{                               
			ZERO_NUM++;                        // “0”计数器自增,另外计数器自增也承担缓冲作用（最后无意义的数据全是0，进入此分支不处理）
		}
		else if (ZERO_NUM <= _Circle / 25)     // 遇到“1”，字符内间隔为t
		{
			ZERO_NUM = 0;                      // 不用处理，重置“0”计数器
		}
		else if (ZERO_NUM > _Circle / 25 && ZERO_NUM <= 3* _Circle / 25) // 字符间隔为3t
		{
			cout << MorseDict(message);        // 调用字典函数，输出字符
			message = "";                      // 清理缓存
			ZERO_NUM = 0;                      // 重置“0”计数器
		}
		else if (ZERO_NUM <= 7* _Circle / 25)  // 单词间隔为7t
		{
			cout << MorseDict(message) << endl;// 额外输出一个换行符表示分割
			message = "";                      // 清理缓存
			ZERO_NUM = 0;                      // 重置“0”计数器
		}
	}
	cout << "[INFO]Morse Code Done." << endl;  // 结束语
}

// 归零码
void decode_rz(int channel){
	vector<int>signal;                         // 向量<vector>存储信号数据
	int LOW_SIGNAL_NUM = 0;                    // 统计连续的低电平信号次数，用于判断帧结束
	wait_signal_start(channel);                // 等待信号开始
	while (true)
	{
		if (digitalRead(channel) == HIGH)      // 高电平
		{
			signal.push_back(1);               // 向量存入1
			LOW_SIGNAL_NUM = 0;                // 出现高电平，信号一定没有结束，重置计数器
			delay_ms(_Circle/3);               // 等待下一个采样点 _Circle/3 ms = 75/3 ms = 25ms
		}
		else if (digitalRead(channel) == LOW)  // 低电平
		{
			signal.push_back(0);               // 向量存入0
			LOW_SIGNAL_NUM++;                  // 出现低电平，信号可能结束，计数
			delay_ms(_Circle/3);			   // 等待下一个采样点 _Circle/3 ms = 75/3 ms = 25ms
		}
		if (LOW_SIGNAL_NUM > _Circle/25)       // 协议规定帧结束低电平2s以上，实际出现长度大于1t的低电平，说明帧结束
		{
			break;                             // 结束信号采集
		}
	}
	string message = "";                       // message用于缓存信号转成的字符编码
	int ONE_NUM = 0; int ASC_NUM = 0;          // “1”的计数器和ASCII码“逢8进1”的标志变量
	for (unsigned int i = 0; i < signal.size(); i++)
	{
		if (1 == signal[i])                    // 读到“1”
		{
			ONE_NUM++;                         // “1”计数器自增
		}
		else if (ONE_NUM > 0 && ONE_NUM <= _Circle/25) // 读到0且计数器不为0，输出8位ASCII码的1位
		{
			message.append("0");               // 1t表示0
			ONE_NUM = 0;                       // 重置“1”计数器
			ASC_NUM++;                         // ASCII码输出计数器自增
		}
		else if (ONE_NUM > _Circle/25 && ONE_NUM <= 2*_Circle/25) 
		{
			message.append("1");               // 2t表示1
			ONE_NUM = 0;                       // 重置“1”计数器
			ASC_NUM++;                         // ASCII码输出计数器自增
		}
		if (8 == ASC_NUM)                      // ASCII码满8
		{
			int result = 0;                    // 字符型的二进制编码转化为十进制数值
			for (int j = 7; j >= 0; j--)
			{
				result += (int)pow(2, j) * ((int)message[7 - j] - 48);
			}
			cout << (char)result;              // 利用C语言特性，直接输出对应ASCII码
			message = "";                      // 清理缓存
			ASC_NUM = 0;                       // 重置ASCII码输出计数器
		}
	}
	cout << endl;                              // 补一个换行符
	cout << "[INFO]RZ-Code Done." << endl;     // 结束语
}

//非归零码
void decode_nrz(int channel, int clk_channel){
	vector<string>signal;                      // 向量存数据
	int LOW_SIGNAL_NUM = 0;                    // 低电平计数，判断帧结束
	wait_signal_start(channel);                // 等待信号开始
	while (true)
	{
		if (digitalRead(channel) == HIGH && digitalRead(clk_channel) == HIGH) // 非归零码有两个信号，向量以2位单位，先存时钟信号，再存数据信号
		{
			signal.push_back("1"); // 时钟信号1
			signal.push_back("1"); // 数据信号1
			LOW_SIGNAL_NUM = 0;    // 出现高电平，信号一定没有结束，重置计数器
			delay_ms(_Circle/3);   // 等待下一个采样点 _Circle/3 ms = 75/3 ms = 25ms
		}
		else if (digitalRead(channel) == LOW && digitalRead(clk_channel) == HIGH) // 时钟信号0 数据信号1
		{
			signal.push_back("1");
			signal.push_back("0");
			LOW_SIGNAL_NUM++;      // 出现低电平，信号可能结束，计数
			delay_ms(_Circle / 3);
		}
		else if (digitalRead(channel) == HIGH && digitalRead(clk_channel) == LOW) // 时钟信号0 数据信号1
		{
			signal.push_back("0");
			signal.push_back("1");
			LOW_SIGNAL_NUM = 0;
			delay_ms(_Circle / 3);
		}
		else                       // 时钟信号0 数据信号0
		{
			signal.push_back("0");
			signal.push_back("0");
			LOW_SIGNAL_NUM++;
			delay_ms(_Circle / 3);
		}
		if (LOW_SIGNAL_NUM > 8*_Circle / 25) // 协议规定帧结束连续8个0，即连续读到 8*3 = 24个低电平信号
		{
			break;                 // 结束信号采集
		}
	}
	string message = "";           // message用于缓存信号转成的字符编码
	int ASC_NUM = 0;               // ASCII码输出计数器
	for (unsigned int i = 2; i < signal.size()-1; i=i+2) // 向量两两一组，循环以2为步长。为避免溢出，从第2组开始，后面与前面相比
	{
		if (signal[i - 2] != signal[i]) // 如果时间信号发生变化，说明数据bit已变
		{
			message.append(signal[i - 1]); // 将前一个数据信号写入缓存（1和0都有意义，不需要区分）
			ASC_NUM++;             // ASCII码输出计数器自增
		}
		else;
		if (ASC_NUM <= 4)          // 时间信号未变化，处在一个周期内，检查是否需要输出字符
		{
			message = "";          // 帧起始是4个“1”，所以ASCII码前4次变化清空缓存
		}
		else if (0 == (ASC_NUM - 4) % 8 && message!="") // 利用模运算判断ASCII码是否需要输出
		{
			int result = 0;        // 字符型的二进制编码转化为十进制数值
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
	cout << "[INFO]NRZ-Code Done." << endl; // 结束语
}

//曼彻斯特码
void decode_manch(int channel){
	vector<int>signal;            // 向量保持信号数据
	int HIGH_SIGNAL_NUM = 0; int LOW_SIGNAL_NUM = 0; // 高低电平信号计数器
	wait_signal_start(channel);   // 等待信号开始 
	while (true)
	{
		if (digitalRead(channel) == HIGH) // 高电平
		{
			signal.push_back(1); // 向量存入1
			HIGH_SIGNAL_NUM++; LOW_SIGNAL_NUM = 0; // 高电平自增，低电平重置
			delay_ms(_Circle/15);// 等待下一个采样点 _Circle/15 ms = 75/15 ms = 5ms 曼彻斯特编码跳变比较迅速，增加采样点捕捉电平变化
		}
		else if (digitalRead(channel) == LOW) // 低电平
		{
			signal.push_back(0);
			HIGH_SIGNAL_NUM = 0; LOW_SIGNAL_NUM++; // 低电平自增，高电平重置
			delay_ms(_Circle/15);
		}
		if (HIGH_SIGNAL_NUM > 20 || LOW_SIGNAL_NUM > 20) // 协议规定帧结束最后bit电平持续1.2s，曼彻斯特编码电平变化很频繁，连续电平数超过经验数值20，即可认为帧结束
		{
			break;               // 结束信号采集
		}

	}
	int SIGNAL = signal[0]; int SIGNAL_NUM = 0; // 曼彻斯特编码比较复杂，原始数据需要经过一定的加工便于解读
	vector<int> temp_1;          // 第一步：将最小的数据块，至多连续5个“1”或“0”,合并为1个“1”或“0”
	for (unsigned int i = 0; i < signal.size(); i++) { // SIGNAL表示所处数据为“1”或“0”，SIGNAL_NUM限制一个数据块的长度
		if (signal[i] == SIGNAL && SIGNAL_NUM <= 5) // 数据发生了变化或数据块尚未达到最大长度
		{
			SIGNAL_NUM++;        // 数据长度计数器自增
		}
		else
		{
			temp_1.push_back(SIGNAL); // 将合并后的数据写入新向量
			SIGNAL = signal[i];       // 刷新数据的值
			SIGNAL_NUM = 1;           // 此时数据块长度已经为1
		}
	}
	vector<int> temp_2;  string str = ""; // 第二步：根据协议规定，高跳低（10）表示1，低跳高（01）表示0，从信号转化为字符编码
	for (unsigned int i = 0; i < temp_1.size(); i++) {
		if (str.size() < 2) {         // 缓存字符串长度不足2
			str += to_string(temp_1[i]);  // 写入新的数据
		}
		else {                        // 缓存字符串长度已满，进行比较转码
			if ("10" == str)          // “10”表示1
				temp_2.push_back(1);
			else if ("01" == str)     // “01”表示0
				temp_2.push_back(0);
			else if ("00" == str)     // “00”表示帧结束
				temp_2.push_back(-1);
			str = "";                 // 清理缓存
			str += to_string(temp_1[i]); // 写入新数据
		}
	}
	string message = "";              // message用于缓存信号转成的字符编码
	for (unsigned int i = 4; i < temp_2.size(); i++) {
		if (message.size() < 8) {     // 利用<string>库函数，通过缓存字符串长度判断是否需要输出
			message.append(to_string(temp_2[i])); //不足则继续写入字符编码
		}
		else {                        // 满8位开始输出字符
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
		if (temp_2[i] == -1)          // 读到终止编码，结束循环
			break;
	}
	cout << endl;
	cout << "[INFO]Manch Code Done." << endl; // 结束语
}

int main(int argc, char* argv[])
{
	// 启动等待设备插入，未插入则提示并休眠一段时间，再次检查
	while(!open_dev()) {
		cout << "Device open failed, try to re-plug it!" << endl;
		Sleep(1500);
	}
	
	// 启动提示语s
	cout << "Device opened." << endl;

	// 启动界面，输入数字开始解码
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