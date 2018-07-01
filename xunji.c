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
	
	// /* 单独一个轮子拐弯可能拖不动 */

	// // Mode 1
	// // 完全跟线走，稍慢, 表现良好
	// // 6T 模式下动不了 
	// { 65536 - 200,	5,5,  3,5,2,5,	5,3,5,2,	12500,12000 },

	// // Mode 2
	// // 完全根线，小弯大弯表现良好，修复能力强，表现优秀
	// { 65536 - 200,	6,6,	3,6,2,6, 6,3,6,2,12500,12000 },

	// // Mode 3
	// // 大弯修复能力弱，走着走着就偏了
	// //{6,6,  1,5,0,5, 5,1,5,0,12000,12000 },
	// // 尝试减小偏转力度
	// { 65536 - 200,6,6,  1,4,0,4, 4,1,4,0,12000,12000 },

	// // Mode 4
	// // 6T 模式下工作良好，表现很好   12T 也还可以
	// { 65536 - 200,9,9,  3,5,0,5, 5,3,5,0,12000,12000 },

	// // Mode 5
	// // 高速，拐弯溢出边界
	// //{15,15,  2,7,1,7, 7,2,7,1,12000,12000 },
	// // 尝试再次降低修正幅度   效果还可以   稳定性差
	// { 65536 - 200,13,13,  5,7,3,7, 7,5,7,3,12000,12000 },

		/* 单独一个轮子拐弯可能拖不动 */

	// Mode 1
	// 完全跟线走，稍慢, 表现良好
	// 6T 模式下动不了 
	{4,4,  2,4,1,4,	4,2,4,1,	12000,12000 },

	// Mode 2
	// 完全根线，小弯大弯表现良好，修复能力强，表现优秀
	{5,5,  1,5,0,5, 5,1,5,0  ,12000,12000 },

	// Mode 3
	// 大弯修复能力弱，走着走着就偏了
	//{6,6,  1,5,0,5, 5,1,5,0,12000,12000 },
	// 尝试减小偏转力度
	{6,6,  1,5,0,5, 5,1,5,0,12000,12000 },

	// Mode 4
	// 6T 模式下工作良好，表现很好   12T 也还可以
	{9,9,  0,4,0,5, 4,0,5,0, 12000,12000 },

	// Mode 5
	// 高速，拐弯溢出边界
	//{15,15,  2,7,1,7, 7,2,7,1,12000,12000 },
	// 尝试再次降低修正幅度   效果还可以   稳定性差
	{13,13,  4,7,2,7, 7,4,7,2,12000,12000 },

	{6,6,  1,4,0,4, 4,1,4,0,12000,12000 },
};

code struct schdule_config schdule_configs[] = {
	{2,0},	// 很好
	{2,1},
	{5,0},
	{5,1},
	// 不稳定配置
	{4,0},
	{2,1},
	{3,1},
	{4,1},
};

unsigned char turn_configs[] = {
	// 比赛顺序
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

char key_scan(void)    //按键扫描
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

			case 8:        //换模式
			{
				// // 循环切换速度
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
			// 开始、暂停、继续
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
	TMOD=0x11;      //配置为定时器0 8位重装模式，定时器1为16位定时器
	
	TH0=(65536-TIMER0)/256;                 //12M，定时50us
	TL0=(65536-TIMER0)%256;
	ET0=1;
	TR0=0;

	TH1=(65536-2000)/256;                 //12M，定时2ms
	TL1=(65536-2000)%256;
	ET1=1;
	TR1=0;
	
	// T2CON = 0;  //0000 0000   ;定时器2配置为16位自动重装定时器
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

void straight() //走直线
{
	// 切换配置
	// 由于比较耗时，所以判断一下当前模式是值得的
	if(runtime > tune_time){
		if(current_time_config_index!=mode_high){

		}
			load_time_config(mode_high);
	}
		pro_left  = (*current_time_config).lstraight;
		pro_right = (*current_time_config).rstraight;
}

void turn_left() //左修正
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

void turn_right() //右修正
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

void turn_left_max() // 大幅度左修正
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

void turn_right_max() // 大幅度右修正
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

//左转90度
void turn_left_90() 
{
	unsigned int i=(*current_time_config).ltime;         //转过度减少时间，没转够增加时间
	while(i--)
	{
		// 右电机正转，  左电机反转
		if(i%4) IN2_1 = 1,IN1_2 =1;   //降低转弯速度,便于控制
		else IN2_1 = 0,IN1_2 =0;
	}
	IN2_1 = 0;
	IN1_2 = 0;
}

// 左转弯实现
// 左电机正转，右电机反转 加快转弯速度
void turn_right_90() //右转90度
{
	unsigned int i=(*current_time_config).rtime;         //转过度减少时间，没转够增加时间
	while(i--)
	{
		if(i%4) IN1_1 = 1,IN2_2 =1;   //降低转弯速度,便于控制
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
		if(i%4) IN1_1 = 1,IN2_1 =1;   //降低速度,便于控制
		else IN1_1 = 0,IN2_1 =0;
	}
	IN1_1 = 0;
	IN2_1 = 0;
}

void stop()
{
	unsigned int i,j=0;
	// 防止中断函数导致停止失败
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
 * 		十字路口处理程序
 * 		1、停止当前调整动作
 * 		2、测距
 * 		3、判断应该【到终点、左90°转、右90转、直行】
 * 		4、结束后恢复定时器
 */
void crossroad()
{
	if(XUNJI_1 && XUNJI_2 && XUNJI_3 && XUNJI_4)  //依旧四灯全灭,有理由相信遇到十字路口
	{
		wait();
		if(current_turn_config<sizeof(turn_configs)){
			if(turn_configs[current_turn_config]==LEFT){
				turn_left_90();
				go_straight();
				TR0 = 1;   //开启T0中断
				TR1 = 1;   //开启T1中断
			}else if(turn_configs[current_turn_config]==RIGHT){
				turn_right_90();
				go_straight();
				TR0 = 1;   //开启T0中断
				TR1 = 1;   //开启T1中断
			}else if(turn_configs[current_turn_config]==STRAIGHT){
				// 快速直行走出十字路口区域
				go_straight();
				TR0 = 1;   //开启T0中断
				TR1 = 1;   //开启T1中断
			}
		}else{
			stop();
			current_turn_config = 0;
		}
		current_turn_config++;
		set_display(current_turn_config);
	}
}

void wait() //暂停
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
 *		循迹1、2、3、4 对应 左、左中、右中、右
 *		地面为白色时，灯亮，端口电平->低
 *		地面为黑色时，灯灭，端口电平->高
 *		左边灯灭，
 */
void infrared() //循迹
{
	//四灯全灭,可能遇到十字路口
	if(XUNJI_1 && XUNJI_2 && XUNJI_3 && XUNJI_4)
		crossroad();
	//严重偏右
	else if(XUNJI_1)
		turn_left_max();
	//严重偏左
	else if(XUNJI_4)
		turn_right_max();
	//中间
	else if(XUNJI_2 && XUNJI_3)
		straight();
	//偏右 (注：在严重偏右后面，即不发生严重偏右)
	else if(XUNJI_2)
		turn_left();
	//偏左 (注：在严重偏左后面，即不发生严重偏左)
	else if(XUNJI_3)
		turn_right();
	else
		straight();
		
}

// PWM 输出
//电机输出函数
void output()
{
	if(time > 19)
	{
		time = 0;
		// 直行
		IN1_1 =1; IN2_1 = 1;
		IN1_2 =0; IN2_2 = 0;
		// 计时器 20*100us = 2 ms
		// 清零防止 tune_time > runtime 导致模式不能切换 
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

// 读取预设参数，快速设置参数
void load_time_config(char idx){
	current_time_config_index = idx;
	current_time_config = &speed_level_configs + current_time_config_index;
	set_display(current_time_config_index+1);
}

// 检测轮子状态
void get_wheel_steps(){
	static unsigned int last_left = 1, last_right = 1;
	if(CESU_1 != last_left){left_steps++; last_left=CESU_1;} 
	if(CESU_2 != last_right){set_display(right_steps++); last_right=CESU_2;} 
}