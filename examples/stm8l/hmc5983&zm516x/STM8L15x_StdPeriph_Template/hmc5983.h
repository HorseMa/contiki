#ifndef __HMC5983_H__
#define __HMC5983_H__

#define	SlaveAddress   0x3C	  //����������IIC�����еĴӵ�ַ

typedef unsigned char BYTE;
typedef unsigned short WORD;

extern BYTE BUF[8];                         //�������ݻ�����      	
extern unsigned char ge,shi,bai,qian,wan;           //��ʾ����

extern void Init_HMC5983(void);
extern unsigned char Single_Read_HMC5983(unsigned char RegName);
extern void Multiple_read_HMC5983(void);
extern void Conversion(unsigned int temp_data);

#endif