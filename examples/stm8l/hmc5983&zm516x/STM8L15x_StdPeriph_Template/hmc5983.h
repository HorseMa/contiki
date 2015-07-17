#ifndef __HMC5983_H__
#define __HMC5983_H__

#define	SlaveAddress   0x3C	  //定义器件在IIC总线中的从地址

typedef unsigned char BYTE;
typedef unsigned short WORD;

extern BYTE BUF[8];                         //接收数据缓存区      	
extern unsigned char ge,shi,bai,qian,wan;           //显示变量

extern void Init_HMC5983(void);
extern unsigned char Single_Read_HMC5983(unsigned char RegName);
extern void Multiple_read_HMC5983(void);
extern void Conversion(unsigned int temp_data);

#endif