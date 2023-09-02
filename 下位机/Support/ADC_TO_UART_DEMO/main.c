#include<stc8.h>
#include<intrins.h>
#include<stdio.h>
/*
	STC8H1K08 ADC转串口，共计四路ADC，2路串口输出
	产品名：影萌牌ADC转串口V1.0
	作者：何影萌
	晶蓝电子出品，必属精品
	版权所有 侵权必究
	当前已经实现四路ADC扩展，串口2正在研究中

	晶振为33.1776MHZ(已经达到STM32F1的一半了，就问你爽不爽)
	波特率为115200

	最方便的Printf用法：
	ES=0;
	TI=1;
	printf;
	ES=1;
*/


void UartInit(void);		//115200bps@ 33.1776MHz
void Delay_ms(unsigned int ms);		//@33.1776MHz
void SendData(unsigned char dat);//发送一个字符
void Send_Str(unsigned char *str);//发送一个字符串
void ADC_Init();//ADC初始化
unsigned int ADC_Change(unsigned char channel);//ADC转换


void main()
{
    unsigned int ADC_num[4];
    unsigned int ADC_ALL;
    unsigned char i;
    UartInit();		//115200bps@ 33.1776MHz
    ADC_Init();
    //Uart2Init();		//115200bps@33.1776MHz  串口2
    //Delay_ms(100);		//@11.0592MHz
    while(1)
    {
        /*
        检测接收的数据是否正确，成功返回1，失败返回0
        * 使用了USART1 4Pin的接口  波特率为115200 BPS
        * 通信协议：包头+数据流+包尾+校验 共计12个字节
        * 包头：一个字节 0x2d   ADC_Read.Buf[0]
        * 数据流：两个字节ADC1 +两个字节ADC2 +两个字节ADC3 +两个字节ADC4传感 1-8
        * 校验：整理出四个ADC值相加的值发送到C板端  ADC_Read.Buf[9]   ADC_Read.Buf[10]
        * 包尾：一个字节 0x73   ADC_Read.Buf[11]
        */
        ADC_ALL=0;
        for(i=0; i<4; i++)
        {
            ADC_num[i]=ADC_Change(1+i);
            ADC_ALL+=ADC_num[i];
        }

        SendData(0x2d);
        SendData(ADC_num[0]>>8);
        SendData(ADC_num[0]%0x00ff);
        SendData(ADC_num[1]>>8);
        SendData(ADC_num[1]%0x00ff);
        SendData(ADC_num[2]>>8);
        SendData(ADC_num[2]%0x00ff);
        SendData(ADC_num[3]>>8);
        SendData(ADC_num[3]%0x00ff);
        SendData(0x73);
        Delay_ms(5);		//@11.0592MHz
    }
}




void UartInit(void)		//115200bps@ 33.1776MHz
{
    SCON = 0x50;		//8位数据,可变波特率
    AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
    AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
    TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
    TL1 = 0xB8;		//设定定时初值
    TH1 = 0xFF;		//设定定时初值
    ET1 = 0;		//禁止定时器1中断
    TR1 = 1;		//启动定时器1
    EA=1;
    ES=1;
}

void Delay_ms(unsigned int ms)		//@33.1776MHz
{
    unsigned char i, j;
    while(ms--)
    {
        _nop_();
        i = 5;
        j = 76;
        do
        {
            while (--j);
        } while (--i);
    }
}




void SendData(unsigned char dat)
{
    SBUF=dat;
    while(TI==0);
    TI=0;
}

void Send_Str(unsigned char *str)
{
    while(*str)
    {
        SendData(*str++);
    }
}



void ADC_Init()
{
    P3M0=0x00;
    P3M1=0x78;
    P_SW2 |= 0x80;  //将P_SW2寄存器的最高位EAXFR置1,访问XSFR
//	ADCTIM=0x3f;
    P_SW2 &=0x7f;
    ADCCFG=0x02;
    ADC_CONTR|=0x80;
}

/*1:ADC11 0X0B 2:ADC12 0X0C 3:ADC13 0X0D 4:ADC14 0X0E*/
unsigned int ADC_Change(unsigned char channel)
{
    unsigned int ADC_Dat;
    ADC_Dat=0x00;
    ADC_RES   = 0;
    ADC_RESL  = 0;
    ADC_CONTR&=0xf0;//消除掉原有的标志，防止错误读取
    switch(channel)
    {
    case 1:
        ADC_CONTR|=0x4B;
        break;
    case 2:
        ADC_CONTR|=0x4C;
        break;
    case 3:
        ADC_CONTR|=0x4D;
        break;
    case 4:
        ADC_CONTR|=0x4E;
        break;
    }
    _nop_();
    _nop_();
    while(!(ADC_CONTR&0x20));
    ADC_CONTR&=~0x20;
    ADC_CONTR&=~0x40;
    ADC_Dat|=ADC_RES;
    ADC_Dat<<=2;
    ADC_Dat|=(ADC_RESL>>6);

    return ADC_Dat;
}



void UART() interrupt 4
{

}



