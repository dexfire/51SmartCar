#include "xunji.h"
#include <intrins.h>

/**
 *		51ѭ��С��
 * 		�������̣�
 * 			T0 -> ���������������п���
 * 			   -> ���ѭ��[T=20*50us=12.8ms]��time ���� 20 ���Ϊ 0 (@output())
 * 			   -> �ڲ�ѭ��[T=50us]�����յ�ǰģʽ��ѭ��PWM���
 * 			   -> PWM ��
 * 			   		-> ֱ�У�0         ~    left_time     ~    right_time      ~      20(��0) 
 * 			   		->  	 �����ֿ�   [ֱ��]   ����ͣ     [��ת]   ����ͣ     [�յ�����]   �����ֿ�
 * 			   		-> ��ת��0         ~    left_time     ~    right_time      ~      20(��0) 
 * 			   		->  	 �����ֿ�   [ֱ��]   ����ͣ     [��ת]   ����ͣ     [�յ�����]   �����ֿ�
 * 			   		-> ��ת��0         ~    right_time     ~    left_time      ~      20(��0) 
 * 			   		-> 	 �����ֿ�   [ֱ��]   ����ͣ     [��ת]   ����ͣ     [�յ�����]   �����ֿ�
 * 			T1 -> ����������̿���
 * 			   -> ѭ�� & ̽��
 * 			   -> �л�ģʽ�� ʮ��·�� - ��ƫת - Сƫת
 * 					-> ʮ��·�ڣ��ж�ת�䷽��
 * 					-> ƫת���л����ģʽ
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
 * 	 ��ʱ�� 0 �жϺ���
 * 	 1��ѭ����ʱ
 *   2��output() : ���ݵ�ǰʱ�䣨0~255�� ��������ת
 * 	
 * 
 */
void T0_time() interrupt 1     //T0��ʱ�жϺ���
{
	TR0=0;                      //�ر�T0�ж�
	time++;
	output();                  //���
	TR0=1;                //����T0�ж�
}

/**
 * 		��ʱ�� 1 �жϺ���
 * 		1��
 * 
 * 
 */

void T1_time() interrupt 3   //T1��ʱ�жϺ���
{
	TR1=0; 

	infrared();
	
	TH1=_iror_(65536-(*current_time_config).sensitivity,8);                 //12M����ʱ1.5ms
	TL1=(65536-(*current_time_config).sensitivity);	
	TR1=1;
}

// void T2_time() interrupt 5   //T2��ʱ�жϺ���
// {
// 	TF2 = 0;
// 	runtime++;
// 	// ��ֹ runtime ���ʱtune_timeʼ�մ���runtime��� һֱ�޷����� �����
// 	if(runtime==0)tune_time=0;
// }
