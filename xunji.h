
#ifndef _XUNJI_H_
#define _XUNJI_H_

#include "REG52.h"

#define LEFT 1
#define RIGHT 2
#define STRAIGHT 3
// 决定供电周期时长  越小控制越灵敏  越大效率越高，速度越快
#define TIMER0 50
// 修正方向时的等待时间，单位2ms

// 800 1000 is ok!
#define ADJUST_TIME_SHORT 600
#define ADJUST_TIME_LONG 800

sbit GUAN_1=P1^4;
sbit GUAN_2=P1^5;
sbit GUAN_3=P1^6;
sbit GUAN_4=P1^7;

sbit C0=P0^7;
sbit C1=P2^7;
sbit C2=P2^6;
sbit R0=P2^0;
sbit R1=P2^1;
sbit R2=P2^2;

sbit LED1=P0^0;
sbit LED2=P0^1;
sbit LED3=P0^2;
sbit LED4=P0^3;

sbit BUZZER=P2^3;

// 左电机正转
sbit IN1_1=P3^7;
// 左电机反转
sbit IN1_2=P3^6;
// 右电机正转
sbit IN2_1=P2^4;
// 右电机反转
sbit IN2_2=P2^5;


sbit TRIG = P3^2;
sbit ECHO = P3^3;

// 测速模块
// 20次跳变为 1 圈
// 左测速
sbit CESU_1=P3^5;
// 右测速
sbit CESU_2=P3^4;

// 循迹 1 2 3 4 -> P1^ 0 1 2 3 -> 左 左中 右中 右
sbit XUNJI_4 = P1^0;
sbit XUNJI_3 = P1^1;
sbit XUNJI_2 = P1^2;
sbit XUNJI_1 = P1^3;
sbit A = P0^4;
sbit BX = P0^6;
sbit C = P0^2;
sbit D = P0^1;
sbit E = P0^0;
sbit F = P0^5;
sbit G = P0^3;
sbit SIG1 = P1^7;
sbit SIG2 = P2^6;

extern unsigned char time;
extern unsigned char pro_left,pro_right;
extern unsigned char code segdata[16];
extern unsigned char page;
extern unsigned int runtime;
extern unsigned int distance;
extern unsigned char display;
extern struct time_config{
    // 直行
    unsigned char lstraight;
    unsigned char rstraight;
    // 左转
    unsigned char lturn_left;
    unsigned char rturn_left;
    unsigned char lturn_left_max;
    unsigned char rturn_left_max;
    // 右转
    unsigned char lturn_right;
    unsigned char rturn_right;
    unsigned char lturn_right_max;
    unsigned char rturn_right_max;
    // 90°转弯时间控制
    unsigned int ltime;
    unsigned int rtime;
};
extern struct schdule_config{
    unsigned char straight_time_config;
    unsigned char adjust_time_config;
};
//typedef struct schdule_config schdule_config;
extern struct time_config *current_time_config;
extern unsigned char current_time_config_index;
extern code struct schdule_config schdule_configs[];
extern code struct time_config speed_level_configs[];
extern unsigned char current_schdule_config;
extern unsigned char display_cache;
extern unsigned char turn_configs[];
extern unsigned int tune_time;
extern unsigned int left_steps,right_steps;
extern enum speed_control{
	SPEED_UP,
	SLOW_DOWN,
	KEEP,
};

void delay(int i);
char key_scan(void);
void key_control();
//void OLED_Display();
void init();
void straight();
void turn_left();
void turn_right();
void turn_left_max();
void turn_right_max();
void turn_left_90();
void turn_right_90();
void shizilukou();
void wait();
void stop();
void infrared();
void output();
void set_display(unsigned char digit);
void load_time_config(char idx);
void set_display_now();
//void tune_speed(enum speed_control sc);
// 检测轮子状态
void get_wheel_steps();
#endif