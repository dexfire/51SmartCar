#ifndef __EEPROM_H__
#define __EEPROM_H__

#include <reg52.h>

#define STC89C52RC

#define EEPROM_SECTOR_SIZE 512

#ifdef STC89C52RC
    // STC89C52RC 有8个EEPROM扇区，每扇区 512 字节，偏移位置 0x2000 ~ 0x2FFF
    #define EEPROM_SECTORS 8
    #define EEPROM_OFFSET 0x2000
#endif

#define EEPROM_END_OFFSET (EEPROM_OFFSET+EEPROM_SECTORS*EEPROM_SECTOR_SIZE)

/* The following is STC additional SFR */

/* sfr  AUXR  = 0x8e; */
/* sfr  AUXR1 = 0xa2; */
/* sfr  IPH   = 0xb7; */

sfr ISP_DATA  = 0xe2;
sfr ISP_ADDRH = 0xe3;
sfr ISP_ADDRL = 0xe4;
sfr ISP_CMD   = 0xe5;
sfr ISP_TRIG  = 0xe6;
sfr ISP_CONTR = 0xe7;

/* Above is STC additional SFR */

// 对相关寄存器进行初始化
void eeprom_init();
// 从EEPROM读取一个字节
unsigned char eeprom_read_byte(unsigned int offset);
// 从EEPROM读取<块数据>，返回读取的内容,可以连续读入
unsigned char* eeprom_read(unsigned int offset,unsigned char *buffer,unsigned int buffer_offset,unsigned int len);
// 向EEPROM写入一个字节
void eeprom_write_byte(unsigned int offset,unsigned char byte);
// 写入数据，一次性最多写入 512 bytes
void eeprom_write(unsigned int offset,unsigned char *buffer,unsigned int buffer_offset,unsigned int len);
// 对于STC89C52RC来说，idx = [0:7];
void eeprom_erase(unsigned char idx);
#endif /*__EEPROM_H*/
