#include "bsp_9959.h"


uint8_t CSR_DATA0[1] = {0x10};     // 开 CH0
uint8_t CSR_DATA1[1] = {0x20};      // 开 CH1
uint8_t CSR_DATA2[1] = {0x40};      // 开 CH2
uint8_t CSR_DATA3[1] = {0x80};      // 开 CH3		
																	
uint8_t FR1_DATA[3] = {0xD0,0x00,0x00};
uint8_t FR2_DATA[2] = {0x00,0x00};//default Value = 0x0000
uint8_t CFR_DATA[3] = {0x00,0x03,0x02};//default Value = 0x000302	   
																	
uint8_t LSRR_DATA[2] = {0x00,0x00};//default Value = 0x----
																	
uint8_t RDW_DATA[4] = {0x00,0x00,0x00,0x00};//default Value = 0x--------																	

//控制寄存器通过spi向AD9959写入数据
void WriteData_9959(uint8_t addr,uint8_t num,uint8_t *data)
{
	int i,j;
	uint8_t ValueToWrite = 0;
	//先拉底CS引脚
	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET);
	for(i = 0;i<8;i++)
	{
		HAL_GPIO_WritePin(SCK_GPIO_Port,SCK_Pin,GPIO_PIN_RESET);
		if((addr & 0x80) == 0x80)
		{
			HAL_GPIO_WritePin(SDIO0_GPIO_Port,SDIO0_Pin,GPIO_PIN_SET);
		}
		else
			HAL_GPIO_WritePin(SDIO0_GPIO_Port,SDIO0_Pin,GPIO_PIN_RESET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(SCK_GPIO_Port,SCK_Pin,GPIO_PIN_SET);
		addr<<=1;
	}
	HAL_GPIO_WritePin(SCK_GPIO_Port,SCK_Pin,GPIO_PIN_RESET);
	for(i = 0;i<num;i++)
	{
		ValueToWrite = data[i];
		for(j = 0;j<8;j++)
		{
			HAL_GPIO_WritePin(SCK_GPIO_Port,SCK_Pin,GPIO_PIN_RESET);
			if((ValueToWrite & 0x80) == 0x80)
			{
				HAL_GPIO_WritePin(SDIO0_GPIO_Port,SDIO0_Pin,GPIO_PIN_SET);
			}
			else
				HAL_GPIO_WritePin(SDIO0_GPIO_Port,SDIO0_Pin,GPIO_PIN_RESET);
			HAL_Delay(1);
			HAL_GPIO_WritePin(SCK_GPIO_Port,SCK_Pin,GPIO_PIN_SET);
			ValueToWrite<<=1;
		}
	}
  IO_update();
	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET);
}

void IO_reset(void)
{
	HAL_GPIO_WritePin(RESET_GPIO_Port,RESET_Pin,GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(RESET_GPIO_Port,RESET_Pin,GPIO_PIN_SET);
	HAL_Delay(2);
	HAL_GPIO_WritePin(RESET_GPIO_Port,RESET_Pin,GPIO_PIN_RESET);
}


//AD9959更新数据
void IO_update(void)
{
	HAL_GPIO_WritePin(UPDATE_GPIO_Port,UPDATE_Pin,GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(UPDATE_GPIO_Port,UPDATE_Pin,GPIO_PIN_SET);
	HAL_Delay(5);
	HAL_GPIO_WritePin(UPDATE_GPIO_Port,UPDATE_Pin,GPIO_PIN_RESET);
}
//FTW/2^32=freq
void Write_frequence(uint8_t channel,uint32_t freq)
{
	uint8_t CFTWO[4] = {0x00,0x00,0x00,0x00};
	uint32_t temp = 0;
	temp=(uint32_t)freq*8.589934592;	   //将输入频率因子分为四个字节  8.589934592=(2^32)/1000000000
	//从数组0开始发送。但实际数据是从高位到地位接收的
	CFTWO[3] = (uint8_t)temp;
	CFTWO[2] = (uint8_t)(temp>>8);
	CFTWO[1] = (uint8_t)(temp>>16);
	CFTWO[0] = (uint8_t)(temp>>24);
	switch(channel)
	{
		case 1:WriteData_9959(CSR_ADDR,1,CSR_DATA0);break;//开启CH0
		case 2:WriteData_9959(CSR_ADDR,1,CSR_DATA1);break;
		case 3:WriteData_9959(CSR_ADDR,1,CSR_DATA2);break;
		case 4:WriteData_9959(CSR_ADDR,1,CSR_DATA3);break;
		default:break;
	}
	WriteData_9959(CFTWO_ADDR,4,CFTWO);
}
//设定幅度 10bit
void Write_amplitude(uint8_t channel,uint16_t	ampli)
{
	uint32_t temp = 0;
	uint8_t ACR_DATA[3] = {0x00,0x00,0x00};
	
	if (ampli >= 1024)
    ampli = 1023;                 // 幅值的最大值
	temp |= MultiplierEnable;//开启幅值控制位
  temp |= ampli&0x3FF; //限定在10bit
	
	ACR_DATA[2] = (uint8_t)temp;
  ACR_DATA[1] = (uint8_t)(temp>>8);
	ACR_DATA[0] = (uint8_t)(temp>>16);
	switch(channel)
	{
		case 1:WriteData_9959(CSR_ADDR,1,CSR_DATA0);break;//开启通道一
		case 2:WriteData_9959(CSR_ADDR,1,CSR_DATA1);break;
		case 3:WriteData_9959(CSR_ADDR,1,CSR_DATA2);break;
		case 4:WriteData_9959(CSR_ADDR,1,CSR_DATA3);break;
		default:break;
	}
	WriteData_9959(ACR_ADDR,3,ACR_DATA);
}	

//设定相位 14bit
void Write_phase(uint8_t channel,uint16_t phase)
{
	uint8_t CPOWO_DATA[2] = {0x00,0x00};
	phase = phase & 0x3FFF;
	CPOWO_DATA[1] = (uint8_t)phase;
	CPOWO_DATA[0] = (uint8_t)(phase>>8);
	switch(channel)
	{
		case 1:WriteData_9959(CSR_ADDR,1,CSR_DATA0);break;//开启通道一
		case 2:WriteData_9959(CSR_ADDR,1,CSR_DATA1);break;
		case 3:WriteData_9959(CSR_ADDR,1,CSR_DATA2);break;
		case 4:WriteData_9959(CSR_ADDR,1,CSR_DATA3);break;
		default:break;
	}
	WriteData_9959(CPOWO_ADDR,2,CPOWO_DATA);
}



