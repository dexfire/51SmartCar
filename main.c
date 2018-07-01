#include "xunji.h"

/**
 *		51循迹小车
 * 		基本流程：
 * 			T0 -> 负责对驱动输出进行控制
 * 			   -> 外层循环[T=20*50us=12.8ms]：time 超过 20 溢出为 0 (@output())
 * 			   -> 内层循环[T=50us]：按照当前模式，循环PWM输出
 * 			   -> PWM ：
 * 			   		-> 直行：0         ~    left_time     ~    right_time      ~      20(回0) 
 * 			   		->  	 左右轮开   [直行]   左轮停     [左转]   右轮停     [空档滑行]   左右轮开
 * 			   		-> 左转：0         ~    left_time     ~    right_time      ~      20(回0) 
 * 			   		->  	 左右轮开   [直行]   左轮停     [左转]   右轮停     [空档滑行]   左右轮开
 * 			   		-> 右转：0         ~    right_time     ~    left_time      ~      20(回0) 
 * 			   		-> 	 左右轮开   [直行]   右轮停     [右转]   左轮停     [空档滑行]   左右轮开
 * 			T1 -> 负责进行流程控制
 * 			   -> 循迹 & 探测
 * 			   -> 切换模式： 十字路口 - 大偏转 - 小偏转
 * 					-> 十字路口：判断转弯方向
 * 					-> 偏转：切换输出模式
 * 
 */


void main()
{
	init();
	while(1){
		key_control();
		set_display_now();
		//get_wheel_steps();
		//OLED_Display();
	}
}

/**
 * 	 定时器 0 中断函数
 * 	 1、循环计时
 *   2、output() : 根据当前时间（0~255） 进行左右转
 * 	
 * 
 */
void T0_time() interrupt 1     //T0定时中断函数
{
	TR0=0;                      //关闭T0中断
	time++;
	output();                  //输出
	TH0=(65536-TIMER0)/256;                 //12M，定时50us
	TL0=(65536-TIMER0)%256;
	TR0=1;                //开启T0中断
}

/**
 * 		定时器 1 中断函数
 * 		1、
 * 
 * 
 */

void T1_time() interrupt 3   //T1定时中断函数
{
	TR1=0; 
	infrared();
	//12M，定时1.5ms
	TH1=(65536-2000)/256;
	TL1=(65536-2000)%256;	
	TR1=1;
}

// void T2_time() interrupt 5   //T2定时中断函数
// {
// 	TF2 = 0;
// 	runtime++;
// 	// 防止 runtime 溢出时tune_time始终大于runtime造成 一直无法加速 的情况
// 	if(runtime==0)tune_time=0;
// }

