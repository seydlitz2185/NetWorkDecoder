#ifndef _GPIO_H_
#define _GPIO_H_

//#define FT232 //实验板型号(2020旧版使用FT232芯片只有4条信号线，2021新版改为CH341T芯片，可使用9条信号线)

//信号线（信道Channel）定义
#ifdef FT232
	#define NRCLK_PIN 0 // 时钟信号线
	#define MORSE_PIN 1 // 莫尔斯码信号线
	#define MANCH_PIN 2 // 曼彻斯特码信号线
	#define RZERO_PIN 3 // 归零码信号线
	#define NRZER_PIN 3 // 非归零码信号线，与归零码共线，通过跳线切换信号
	#define CIRCLE 50   // 信号周期
#else
	#define NRCLK_PIN 0 // 时钟信号线
	#define MORSE_PIN 1 // 莫尔斯电报码信号线
	#define MANCH_PIN 2 // 曼彻斯特码信号线
	#define RZERO_PIN 3 // 归零码信号线
	#define NRZER_PIN 4 // 非归零码信号线
	#define CIN_PIN   5 //自定义输入信号线
	#define COUT_PIN0 6 //自定义输出信号线1
	#define COUT_PIN1 7 //自定义输出信号线2
	#define COUT_PIN2 8 //自定义输出信号线3
	#define CIRCLE 75   //信号周期
#endif

#define INPUT   0
#define OUTPUT  1
#define ERR     -1
#define S_LEVEL int
#define HIGH    1
#define LOW     0
#define UNKNOWN -1

extern "C" BOOL open_dev(); //打开USB设备，目前只支持识别插入一个设备
extern "C" void close_dev(); //关闭设备
extern "C" LONGLONG get_us(); //得到一个微秒级时间计数值，可利用两次差值计算时延
extern "C" LONGLONG get_ms(); //同上，得到一个毫秒级时间计数值
extern "C" void delay_us(int us); //等待一段时间，微秒单位
extern "C" void delay_ms(int ms); //等待一段时间，毫秒单位
extern "C" S_LEVEL digitalRead(int channel); //检测当前对应信道信号线电平，返回值HIGH为高电平，LOW为低电平
extern "C" BOOL digitalWrite(int channel, S_LEVEL bit); //设备对应信道信号线电平，bit为HIGH高电平，LOW为低电平，为防止与已有信号线路产生高低电平短路，当前只支持写COUT_PIN0~2三个信道

#endif