#include "hmc5983.h"
#include "stm8l15x.h"

BYTE BUF[8];                         //�������ݻ�����      	
unsigned char ge,shi,bai,qian,wan;           //��ʾ����

//*********************************************************
void Conversion(unsigned int temp_data)  
{  
  wan=temp_data/10000+0x30 ;
  temp_data=temp_data%10000;   //ȡ������
  qian=temp_data/1000+0x30 ;
  temp_data=temp_data%1000;    //ȡ������
  bai=temp_data/100+0x30   ;
  temp_data=temp_data%100;     //ȡ������
  shi=temp_data/10+0x30    ;
  temp_data=temp_data%10;      //ȡ������
  ge=temp_data+0x30; 	
}

void Single_Write_HMC5983(unsigned char RegName,unsigned char RegValue)
{
  /*-------------------------------- Transmission Phase -----------------------*/
  /* Send LM75_I2C START condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on LM75_I2C EV5 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
  {
  }

  /* Send STLM75 slave address for write */
  I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Transmitter);

  /* Test on LM75_I2C EV6 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
  {
  }

  /* Send the specified register data pointer */
  I2C_SendData(I2C1, RegName);

  /* Test on LM75_I2C EV8 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
  {
  }
#if 0
  /* Send LM75_I2C data */
  I2C_SendData(I2C1, (uint8_t)(RegValue >> 8));

  /* Test on LM75_I2C EV8 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
  {
  }
#endif
  /* Send LM75_I2C data */
  I2C_SendData(I2C1, (uint8_t)RegValue);

  /* Test on LM75_I2C EV8 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
  {
  }

  /* Send LM75_I2C STOP Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);

#if 0
  HMC5883_Start();                  //��ʼ�ź�
  HMC5883_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
  HMC5883_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf 
  HMC5883_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf
  HMC5883_Stop();                   //����ֹͣ�ź�
#endif
}

unsigned char Single_Read_HMC5983(unsigned char RegName)
{  
  __IO uint16_t RegValue = 0;

  /* Enable LM75_I2C acknowledgement if it is already disabled by other function */
  I2C_AcknowledgeConfig(I2C1, ENABLE);

  /*--------------------------- Transmission Phase ----------------------------*/
  /* Send LM75_I2C START condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on LM75_I2C EV5 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
  {
  }

  /* Send STLM75 slave address for write */
  I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Transmitter);

  /* Test on LM75_I2C EV6 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
  {
  }

  /* Send the specified register data pointer */
  I2C_SendData(I2C1, RegName);

  /* Test on LM75_I2C EV8 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
  {
  }

  /*------------------------------ Reception Phase ----------------------------*/
  /* Send Re-STRAT condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
  {
  }

  /* Send STLM75 slave address for read */
  I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Receiver);

  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))  /* EV6 */
  {
  }
#if 0
  /* Test on EV7 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))  /* EV7 */
  {
  }
  /* Store LM75_I2C received data */
  RegValue = (uint16_t)I2C_ReceiveData(I2C1);
#endif
  /* Disable LM75_I2C acknowledgement */
  I2C_AcknowledgeConfig(I2C1, DISABLE);

  /* Send LM75_I2C STOP Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);

  /* Test on RXNE flag */
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET)
  {}

  /* Store LM75_I2C received data */
  RegValue |= I2C_ReceiveData(I2C1);

  /* Return register value */
  return (RegValue);
#if 0
  uchar REG_data;
    HMC5883_Start();                          //��ʼ�ź�
    HMC5883_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
    HMC5883_SendByte(REG_Address);                   //���ʹ洢��Ԫ��ַ����0��ʼ	
    HMC5883_Start();                          //��ʼ�ź�
    HMC5883_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
    REG_data=HMC5883_RecvByte();              //�����Ĵ�������
    HMC5883_SendACK(1);   
    HMC5883_Stop();                           //ֹͣ�ź�
    return REG_data; 
#endif
}

void Multiple_read_HMC5983(void)
{ 
  int i;
#if 1
  for(i = 0;i < 6;i++)
  {
    BUF[i] = Single_Read_HMC5983(0x03 + i);
  }
#endif
#if 0
  /* Enable LM75_I2C acknowledgement if it is already disabled by other function */
  I2C_AcknowledgeConfig(I2C1, ENABLE);

  /*--------------------------- Transmission Phase ----------------------------*/
  /* Send LM75_I2C START condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on LM75_I2C EV5 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
  {
  }

  /* Send STLM75 slave address for write */
  I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Transmitter);

  /* Test on LM75_I2C EV6 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
  {
  }

  /* Send the specified register data pointer */
  I2C_SendData(I2C1, 0x03);

  /* Test on LM75_I2C EV8 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
  {
  }

  /*------------------------------ Reception Phase ----------------------------*/
  /* Send Re-STRAT condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
  {
  }

  /* Send STLM75 slave address for read */
  I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Receiver);

  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))  /* EV6 */
  {
  }
  
  for(i = 0;i < 4;i ++)
  {
    /* Test on EV7 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))  /* EV7 */
    {
    }
    /* Store LM75_I2C received data */
    BUF[i] = (uint16_t)I2C_ReceiveData(I2C1);
  }
  
  /* Disable LM75_I2C acknowledgement */
  I2C_AcknowledgeConfig(I2C1, DISABLE);

  /* Send LM75_I2C STOP Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);

  /* Test on RXNE flag */
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET)
  {}

  /* Store LM75_I2C received data */
  BUF[i] = I2C_ReceiveData(I2C1);

  /* Return register value */
  return;
#endif
  
#if 0  
  uchar i;
    HMC5883_Start();                          //��ʼ�ź�
    HMC5883_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
    HMC5883_SendByte(0x03);                   //���ʹ洢��Ԫ��ַ����0x3��ʼ	
    HMC5883_Start();                          //��ʼ�ź�
    HMC5883_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
	 for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        BUF[i] = HMC5883_RecvByte();          //BUF[0]�洢����
        if (i == 5)
        {
           HMC5883_SendACK(1);                //���һ��������Ҫ��NOACK
        }
        else
        {
          HMC5883_SendACK(0);                //��ӦACK
       }
   }
    HMC5883_Stop();                          //ֹͣ�ź�
    Delay5ms();
#endif
}

void Init_HMC5983(void)
{
     Single_Write_HMC5983(0x02,0x00);  //
     Single_Write_HMC5983(0x01,0x00);  //
}