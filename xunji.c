#include <REG52.h>
//#include "oled.h"
//#include "bmp.h"
#include "xunji.h"
#include <stdio.h>
//#include "eeprom.h"

unsigned char time = 0;
unsigned char pro_left=0,pro_right=0;
struct time_config *current_time_config;
unsigned char code segdata[]= {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f/*,0x77,0x7c,0x39,0x5e,0x79,0x71*/};
unsigned char display;
code struct time_config speed_level_configs[] = {
	
	// /* ����һ�����ӹ�������ϲ��� */

	// // Mode 1
	// // ��ȫ�����ߣ�����, ��������
	// // 6T ģʽ�¶����� 
	// { 65536 - 200,	5,5,  3,5,2,5,	5,3,5,2,	12500,12000 },

	// // Mode 2
	// // ��ȫ���ߣ�С�����������ã��޸�����ǿ����������
	// { 65536 - 200,	6,6,	3,6,2,6, 6,3,6,2,12500,12000 },

	// // Mode 3
	// // �����޸����������������ž�ƫ��
	// //{6,6,  1,5,0,5, 5,1,5,0,12000,12000 },
	// // ���Լ�Сƫת����
	// { 65536 - 200,6,6,  1,4,0,4, 4,1,4,0,12000,12000 },

	// // Mode 4
	// // 6T ģʽ�¹������ã����ֺܺ�   12T Ҳ������
	// { 65536 - 200,9,9,  3,5,0,5, 5,3,5,0,12000,12000 },

	// // Mode 5
	// // ���٣���������߽�
	// //{15,15,  2,7,1,7, 7,2,7,1,12000,12000 },
	// // �����ٴν�����������   Ч��������   �ȶ��Բ�
	// { 65536 - 200,13,13,  5,7,3,7, 7,5,7,3,12000,12000 },

		/* ����һ�����ӹ�������ϲ��� */

	// Mode 1
	// ��ȫ�����ߣ�����, ��������
	// 6T ģʽ�¶����� 
	{4,4,  2,4,1,4,	4,2,4,1,	12000,12000 },

	// Mode 2
	// ��ȫ���ߣ�С�����������ã��޸�����ǿ����������
	{5,5,  1,5,0,5, 5,1,5,0  ,12000,12000 },

	// Mode 3
	// �����޸����������������ž�ƫ��
	//{6,6,  1,5,0,5, 5,1,5,0,12000,12000 },
	// ���Լ�Сƫת����
	{6,6,  1,5,0,5, 5,1,5,0,12000,12000 },

	// Mode 4
	// 6T ģʽ�¹������ã����ֺܺ�   12T Ҳ������
	{9,9,  0,4,0,5, 4,0,5,0, 12000,12000 },

	// Mode 5
	// ���٣���������߽�
	//{15,15,  2,7,1,7, 7,2,7,1,12000,12000 },
	// �����ٴν�����������   Ч��������   �ȶ��Բ�
	{13,13,  4,7,2,7, 7,4,7,2,12000,12000 },

	{6,6,  1,4,0,4, 4,1,4,0,12000,12000 },
};

code struct schdule_config schdule_configs[] = {
	{2,0},	// �ܺ�
	{2,1},
	{5,0},
	{5,1},
	// ���ȶ�����
	{4,0},
	{2,1},
	{3,1},
	{4,1},
};

unsigned char turn_configs[] = {
	// ����˳��
	//LEFT,RIGHT,STRAIGHT,RIGHT,LEFT
	RIGHT,
	LEFT,
	LEFT,
	RIGHT
};

unsigned char current_time_config_index = 0;
unsigned char current_schdule_config = 0;
unsigned char current_turn_config = 0;
unsigned char mode_low = 1;
unsigned char mode_high = 2;
unsigned int tune_time = 0;
unsigned int runtime = 0;
unsigned int left_steps=0,right_steps=0;

void delay(int i)      //12M   i=500,t=500ms
{
	unsigned char j;
	for( ;i>0;i--)
		for(j=220;j>0;j--);
}

char key_scan(void)    //����ɨ��
{
	C0=0;
	delay(1);
	if(!(R0&&R1&&R2)){BUZZER=0;delay(10);} 
	if(!(R0&&R1&&R2))
	{
		if(!R0) {while(!R0);BUZZER=1;return 1;}
		if(!R1) {while(!R1);BUZZER=1;return 2;}
		if(!R2) {while(!R2);BUZZER=1;return 3;}
	}
	C0=1;
	
	C1=0;
	delay(1);
	if(!(R0&&R1&&R2)) delay(10);
	if(!(R0&&R1&&R2))
	{
		if(!R0) {while(!R0);BUZZER=1;return 4;}
		if(!R1) {while(!R1);BUZZER=1;return 5;}
		if(!R2) {while(!R2);BUZZER=1;return 6;}
	}
	C1=1;
	
	C2=0;
	delay(1);
	if(!(R0&&R1&&R2)) delay(10);
	if(!(R0&&R1&&R2))
	{
		if(!R0) {while(!R0);BUZZER=1;return 7;}
		if(!R1) {while(!R1);BUZZER=1;return 8;}
		if(!R2) {while(!R2);BUZZER=1;return 9;}
	}
	C2=1;
	
	return 0;
}

void key_control()
{
	switch(key_scan()) 
		{
			case 1:
				set_display(current_time_config_index);
				break;

			case 2:
				set_display(current_turn_config);
				break;
			
			case 3:
				set_display(tune_time);
				break;

			case 8:        //��ģʽ
			{
				// // ѭ���л��ٶ�
				// current_time_config_index++;
				// if(current_time_config_index>=sizeof(speed_level_configs)/sizeof(struct time_config))
				// 	current_time_config_index = 0;
				// load_time_config(current_time_config_index);
				current_schdule_config++;
				if(current_schdule_config>=sizeof(schdule_configs)/sizeof(struct schdule_config)){
					current_schdule_config = 0;
				}
				set_display(current_schdule_config);
				mode_low = schdule_configs[current_schdule_config].adjust_time_config;
				mode_high = schdule_configs[current_schdule_config].straight_time_config;
				break;
				// page--;
				// if(page<1) page=1;
				// set_display(page);
				// break;
			}
			// ��ʼ����ͣ������
			case 9:
			{
				TR0 = !TR0; 
				TR1 = TR0; 
				set_display(TR0*(current_time_config_index));
				IN1_1 = 0;
				IN1_2 = 0;
				IN2_1 = 0;
				IN2_2 = 0;
				tune_time = 0;
				runtime = 0;
				break;
			}
		}
}


void init()
{
	EA=1;
	TMOD=0x11;      //����Ϊ��ʱ��0 8λ��װģʽ����ʱ��1Ϊ16λ��ʱ��
	
	TH0=(65536-TIMER0)/256;                 //12M����ʱ50us
	TL0=(65536-TIMER0)%256;
	ET0=1;
	TR0=0;

	TH1=(65536-2000)/256;                 //12M����ʱ2ms
	TL1=(65536-2000)%256;
	ET1=1;
	TR1=0;
	
	// T2CON = 0;  //0000 0000   ;��ʱ��2����Ϊ16λ�Զ���װ��ʱ��
	// RCAP2H = _iror_(65536-250,8);
	// RCAP2L = 65536-250;
	// TH2 = _iror_(65536-250,8);
	// TL2 = 65536-250;
	// ET2 = 1;
	// TR2 = 0;
	
	TRIG = 0;
	ECHO = 1;
	SIG1 = 0;
	BUZZER = 1;
	IN1_1 = 0;
	IN1_2 = 0;
	IN2_1 = 0;
	IN2_2 = 0;
	current_time_config = &speed_level_configs[0];
	//eeprom_init();
	set_display(0);
}

void straight() //��ֱ��
{
	// �л�����
	// ���ڱȽϺ�ʱ�������ж�һ�µ�ǰģʽ��ֵ�õ�
	if(runtime > tune_time){
		if(current_time_config_index!=mode_high){

		}
			load_time_config(mode_high);
	}
		pro_left  = (*current_time_config).lstraight;
		pro_right = (*current_time_config).rstraight;
}

void turn_left() //������
{

	pro_left  = (*current_time_config).lturn_left;
	pro_right = (*current_time_config).rturn_left;
	
//	if((runtime>tune_time)&&(current_time_config_index!=mode_low)){
//		load_time_config(mode_low);
//		
//		tune_time = runtime + ADJUST_TIME_SHORT;
//	}else{
//		pro_left  = (*current_time_config).lturn_left;
//		pro_right = (*current_time_config).rturn_left;
//	}
}

void turn_right() //������
{
	pro_left  = (*current_time_config).lturn_right;
	pro_right = (*current_time_config).rturn_right;
	
//	if((runtime>tune_time)&&(current_time_config_index!=mode_low)){
//		load_time_config(mode_low);
//		tune_time = runtime + ADJUST_TIME_SHORT;
//	}else{
//		pro_left  = (*current_time_config).lturn_right;
//		pro_right = (*current_time_config).rturn_right;
//	}
}

void turn_left_max() // �����������
{
	
	if(runtime>tune_time){
		if(current_time_config_index!=mode_low){
			load_time_config(mode_low);
			tune_time = runtime + ADJUST_TIME_LONG;
		}
	}else{
		pro_left  = (*current_time_config).lturn_left_max;
		pro_right = (*current_time_config).rturn_left_max;
	}
}

void turn_right_max() // �����������
{
	if(runtime>tune_time){
		if(current_time_config_index!=mode_low){
			load_time_config(mode_low);
			tune_time = runtime + ADJUST_TIME_LONG;
		}
	}else{
		pro_left  = (*current_time_config).lturn_right_max;
		pro_right = (*current_time_config).rturn_right_max;
	}
}

//��ת90��
void turn_left_90() 
{
	unsigned int i=(*current_time_config).ltime;         //ת���ȼ���ʱ�䣬ûת������ʱ��
	while(i--)
	{
		// �ҵ����ת��  ������ת
		if(i%4) IN2_1 = 1,IN1_2 =1;   //����ת���ٶ�,���ڿ���
		else IN2_1 = 0,IN1_2 =0;
	}
	IN2_1 = 0;
	IN1_2 = 0;
}

// ��ת��ʵ��
// ������ת���ҵ����ת �ӿ�ת���ٶ�
void turn_right_90() //��ת90��
{
	unsigned int i=(*current_time_config).rtime;         //ת���ȼ���ʱ�䣬ûת������ʱ��
	while(i--)
	{
		if(i%4) IN1_1 = 1,IN2_2 =1;   //����ת���ٶ�,���ڿ���
		else IN1_1 = 0,IN2_2 =0;
	}
	IN1_1 = 0;
	IN2_2 = 0;
}

void go_straight()
{
	unsigned int i=5000;
	while(i--)
	{
		if(i%4) IN1_1 = 1,IN2_1 =1;   //�����ٶ�,���ڿ���
		else IN1_1 = 0,IN2_1 =0;
	}
	IN1_1 = 0;
	IN2_1 = 0;
}

void stop()
{
	unsigned int i,j=0;
	// ��ֹ�жϺ�������ֹͣʧ��
	for(i=5;i>1;i--) wait();
	while(1){
		key_control();
		set_display(j++);
		for(i=500;i>1;i--){
			delay(10);
			BUZZER = !BUZZER;
		}	
	}
}

/**
 * 		ʮ��·�ڴ������
 * 		1��ֹͣ��ǰ��������
 * 		2�����
 * 		3���ж�Ӧ�á����յ㡢��90��ת����90ת��ֱ�С�
 * 		4��������ָ���ʱ��
 */
void crossroad()
{
	if(XUNJI_1 && XUNJI_2 && XUNJI_3 && XUNJI_4)  //�����ĵ�ȫ��,��������������ʮ��·��
	{
		wait();
		if(current_turn_config<sizeof(turn_configs)){
			if(turn_configs[current_turn_config]==LEFT){
				turn_left_90();
				go_straight();
				TR0 = 1;   //����T0�ж�
				TR1 = 1;   //����T1�ж�
			}else if(turn_configs[current_turn_config]==RIGHT){
				turn_right_90();
				go_straight();
				TR0 = 1;   //����T0�ж�
				TR1 = 1;   //����T1�ж�
			}else if(turn_configs[current_turn_config]==STRAIGHT){
				// ����ֱ���߳�ʮ��·������
				go_straight();
				TR0 = 1;   //����T0�ж�
				TR1 = 1;   //����T1�ж�
			}
		}else{
			stop();
			current_turn_config = 0;
		}
		current_turn_config++;
		set_display(current_turn_config);
	}
}

void wait() //��ͣ
{
	TR1 = 0;
	TR0 = 0;
	time = 0;
	tune_time = 0;
	runtime = 0;
	pro_left = 0;
	pro_right = 0;
	IN1_1 = IN1_2 = IN2_1 = IN2_2 = 0;
}
/**
 *		ѭ��1��2��3��4 ��Ӧ �����С����С���
 *		����Ϊ��ɫʱ���������˿ڵ�ƽ->��
 *		����Ϊ��ɫʱ�����𣬶˿ڵ�ƽ->��
 *		��ߵ���
 */
void infrared() //ѭ��
{
	//�ĵ�ȫ��,��������ʮ��·��
	if(XUNJI_1 && XUNJI_2 && XUNJI_3 && XUNJI_4)
		crossroad();
	//����ƫ��
	else if(XUNJI_1)
		turn_left_max();
	//����ƫ��
	else if(XUNJI_4)
		turn_right_max();
	//�м�
	else if(XUNJI_2 && XUNJI_3)
		straight();
	//ƫ�� (ע��������ƫ�Һ��棬������������ƫ��)
	else if(XUNJI_2)
		turn_left();
	//ƫ�� (ע��������ƫ����棬������������ƫ��)
	else if(XUNJI_3)
		turn_right();
	else
		straight();
		
}

// PWM ���
//����������
void output()
{
	if(time > 19)
	{
		time = 0;
		// ֱ��
		IN1_1 =1; IN2_1 = 1;
		IN1_2 =0; IN2_2 = 0;
		// ��ʱ�� 20*100us = 2 ms
		// �����ֹ tune_time > runtime ����ģʽ�����л� 
		if(runtime++==0)tune_time=0;
	}
	
	if(time == pro_left)	IN1_1 = 0;
	if(time == pro_right)	IN2_1 = 0;
}

void set_display(unsigned char digit){
	display = segdata [digit%10];
}

void set_display_now(){
	A=BX=C=D=E=F=G=0;
	A = display&0x01;
	BX = display&0x02;
	C = display&0x04;
	D = display&0x08;
	E = display&0x10;
	F = display&0x20;
	G = display&0x40;
}

// ��ȡԤ��������������ò���
void load_time_config(char idx){
	current_time_config_index = idx;
	current_time_config = &speed_level_configs + current_time_config_index;
	set_display(current_time_config_index+1);
}

// �������״̬
void get_wheel_steps(){
	static unsigned int last_left = 1, last_right = 1;
	if(CESU_1 != last_left){left_steps++; last_left=CESU_1;} 
	if(CESU_2 != last_right){set_display(right_steps++); last_right=CESU_2;} 
}