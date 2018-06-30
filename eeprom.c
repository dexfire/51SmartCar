#include "eeprom.h"
#include <intrins.h>

// 对相关寄存器进行初始化
void eeprom_init(){
    // 启用EEPROM读写
    ISP_CONTR &= 0x80;
    // 设置擦除等待时间
    ISP_CONTR &= 0x02;
}

// 从EEPROM读取一个字节
unsigned char eeprom_read_byte(unsigned int offset){
    ISP_ADDRH = (offset + EEPROM_OFFSET)/256;
    // 直接赋值，高位会被舍弃
    ISP_ADDRL = offset + EEPROM_OFFSET;
    // 0x01 读 0x02 写 0x03 擦除
    ISP_CMD = 0x01;
    // 开始读取 Trigger - 扳机
    ISP_TRIG = 0x46;
    ISP_TRIG = 0xB9;
    return ISP_DATA;
}

// 从EEPROM读取数据，返回读取的内容,可以连续读入
unsigned char* eeprom_read(unsigned int offset,unsigned char *buffer,unsigned int buffer_offset,unsigned int len){
    unsigned int i;
    ISP_CMD = 0x01;
    for(i=0;i<len;i++){
        ISP_ADDRH = _iror_(EEPROM_OFFSET + offset + i,8);
        ISP_ADDRL = EEPROM_OFFSET + offset + i;
        ISP_TRIG = 0x46;
        ISP_TRIG = 0xB9;
        *(buffer+buffer_offset+i) = ISP_DATA;
    }
    return buffer;
}

void eeprom_write_byte(unsigned int offset,unsigned char byte){
    ISP_ADDRH = _iror_(offset + EEPROM_OFFSET,8);
    // 直接赋值，高位会被舍弃
    ISP_ADDRL = offset + EEPROM_OFFSET;
    // 0x01 读 0x02 写 0x03 擦除
    ISP_CMD = 0x02;
    ISP_DATA = byte;
    // 开始读取 Trigger - 扳机
    ISP_TRIG = 0x46;
    ISP_TRIG = 0xB9;
}

// 写入数据，一次性最多写入 512 bytes
void eeprom_write(unsigned int offset,unsigned char *buffer,unsigned int buffer_offset,unsigned int len){
    unsigned int i;
    ISP_CMD = 0x02;
    for(i=0;i<len;i++){
        ISP_ADDRH = _iror_(EEPROM_OFFSET + offset + i,8);
        ISP_ADDRL = EEPROM_OFFSET + offset + i;
        ISP_DATA = *(buffer+buffer_offset+i);
        // 提交写入命令
        ISP_TRIG = 0x46;
        ISP_TRIG = 0xB9;
    }
}

// 对于STC89C52RC来说，idx = [0:7];
void eeprom_erase(unsigned char idx){
    ISP_ADDRH = _iror_(EEPROM_OFFSET + idx * EEPROM_SECTOR_SIZE,8);
    ISP_ADDRL = EEPROM_OFFSET + idx * EEPROM_SECTOR_SIZE;
    ISP_CMD = 0x03;
    ISP_TRIG = 0x46;
    ISP_DATA = 0xB9;
}