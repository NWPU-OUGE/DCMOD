#include<stc8.h>
#include<intrins.h>
#include<stdio.h>
/*
	STC8H1K08 ADCת���ڣ�������·ADC��2·�������
	��Ʒ����Ӱ����ADCת����V1.0
	���ߣ���Ӱ��
	�������ӳ�Ʒ��������Ʒ
	��Ȩ���� ��Ȩ�ؾ�
	��ǰ�Ѿ�ʵ����·ADC��չ������2�����о���

	����Ϊ33.1776MHZ(�Ѿ��ﵽSTM32F1��һ���ˣ�������ˬ��ˬ)
	������Ϊ115200

	����Printf�÷���
	ES=0;
	TI=1;
	printf;
	ES=1;
*/


void UartInit(void);		//115200bps@ 33.1776MHz
void Delay_ms(unsigned int ms);		//@33.1776MHz
void SendData(unsigned char dat);//����һ���ַ�
void Send_Str(unsigned char *str);//����һ���ַ���
void ADC_Init();//ADC��ʼ��
unsigned int ADC_Change(unsigned char channel);//ADCת��


void main()
{
    unsigned int ADC_num[4];
    unsigned int ADC_ALL;
    unsigned char i;
    UartInit();		//115200bps@ 33.1776MHz
    ADC_Init();
    //Uart2Init();		//115200bps@33.1776MHz  ����2
    //Delay_ms(100);		//@11.0592MHz
    while(1)
    {
        /*
        �����յ������Ƿ���ȷ���ɹ�����1��ʧ�ܷ���0
        * ʹ����USART1 4Pin�Ľӿ�  ������Ϊ115200 BPS
        * ͨ��Э�飺��ͷ+������+��β+У�� ����12���ֽ�
        * ��ͷ��һ���ֽ� 0x2d   ADC_Read.Buf[0]
        * �������������ֽ�ADC1 +�����ֽ�ADC2 +�����ֽ�ADC3 +�����ֽ�ADC4���� 1-8
        * У�飺������ĸ�ADCֵ��ӵ�ֵ���͵�C���  ADC_Read.Buf[9]   ADC_Read.Buf[10]
        * ��β��һ���ֽ� 0x73   ADC_Read.Buf[11]
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
    SCON = 0x50;		//8λ����,�ɱ䲨����
    AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
    AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
    TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
    TL1 = 0xB8;		//�趨��ʱ��ֵ
    TH1 = 0xFF;		//�趨��ʱ��ֵ
    ET1 = 0;		//��ֹ��ʱ��1�ж�
    TR1 = 1;		//������ʱ��1
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
    P_SW2 |= 0x80;  //��P_SW2�Ĵ��������λEAXFR��1,����XSFR
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
    ADC_CONTR&=0xf0;//������ԭ�еı�־����ֹ�����ȡ
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



