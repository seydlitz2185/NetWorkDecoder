#ifndef _GPIO_H_
#define _GPIO_H_

//#define FT232 //ʵ����ͺ�(2020�ɰ�ʹ��FT232оƬֻ��4���ź��ߣ�2021�°��ΪCH341TоƬ����ʹ��9���ź���)

//�ź��ߣ��ŵ�Channel������
#ifdef FT232
	#define NRCLK_PIN 0 // ʱ���ź���
	#define MORSE_PIN 1 // Ī��˹���ź���
	#define MANCH_PIN 2 // ����˹�����ź���
	#define RZERO_PIN 3 // �������ź���
	#define NRZER_PIN 3 // �ǹ������ź��ߣ�������빲�ߣ�ͨ�������л��ź�
	#define CIRCLE 50   // �ź�����
#else
	#define NRCLK_PIN 0 // ʱ���ź���
	#define MORSE_PIN 1 // Ī��˹�籨���ź���
	#define MANCH_PIN 2 // ����˹�����ź���
	#define RZERO_PIN 3 // �������ź���
	#define NRZER_PIN 4 // �ǹ������ź���
	#define CIN_PIN   5 //�Զ��������ź���
	#define COUT_PIN0 6 //�Զ�������ź���1
	#define COUT_PIN1 7 //�Զ�������ź���2
	#define COUT_PIN2 8 //�Զ�������ź���3
	#define CIRCLE 75   //�ź�����
#endif

#define INPUT   0
#define OUTPUT  1
#define ERR     -1
#define S_LEVEL int
#define HIGH    1
#define LOW     0
#define UNKNOWN -1

extern "C" BOOL open_dev(); //��USB�豸��Ŀǰֻ֧��ʶ�����һ���豸
extern "C" void close_dev(); //�ر��豸
extern "C" LONGLONG get_us(); //�õ�һ��΢�뼶ʱ�����ֵ�����������β�ֵ����ʱ��
extern "C" LONGLONG get_ms(); //ͬ�ϣ��õ�һ�����뼶ʱ�����ֵ
extern "C" void delay_us(int us); //�ȴ�һ��ʱ�䣬΢�뵥λ
extern "C" void delay_ms(int ms); //�ȴ�һ��ʱ�䣬���뵥λ
extern "C" S_LEVEL digitalRead(int channel); //��⵱ǰ��Ӧ�ŵ��ź��ߵ�ƽ������ֵHIGHΪ�ߵ�ƽ��LOWΪ�͵�ƽ
extern "C" BOOL digitalWrite(int channel, S_LEVEL bit); //�豸��Ӧ�ŵ��ź��ߵ�ƽ��bitΪHIGH�ߵ�ƽ��LOWΪ�͵�ƽ��Ϊ��ֹ�������ź���·�����ߵ͵�ƽ��·����ǰֻ֧��дCOUT_PIN0~2�����ŵ�

#endif