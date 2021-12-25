#include <stdio.h>
#include <windows.h>
#include <string>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define RING_BUFFER_SIZE (1024 * 192)

u8 payloadBuffer[1];
char chrUARTBufferOutput[RING_BUFFER_SIZE];

u8 ringBuf[RING_BUFFER_SIZE];
u8 pBufferStart;
u8 pBufferEnd;
u8 pCustomer;
u8 pProducer;
unsigned long ucComNo;
unsigned char chrBuffer[RING_BUFFER_SIZE];


int main(void)
{
    ucComNo = 5;
    typedef int(*MyFunDll_1)(unsigned long ucComNo, unsigned long baud);
    typedef int(*MyFunDll_2)(u8 cid, u8 mid, unsigned long ucComNo, unsigned char* chrBuffer);
    typedef int(*MyFunDll_3)(const unsigned long ulCOMNo, char chrUARTBufferOutput[]);
    typedef int(*MyFunDll_4)(u8 MADDR, u8 ClassID, u8 msgID, u8* payloadBuffer, u16 PayloadLen);

    HMODULE hdll = LoadLibrary(L"C:\\Users\\Govern liu\\Desktop\\ATOM_LINKER Examples\\Examples\\Windows MeasureDataDemo\\x64\\Debug\\MeasureDataDemo.dll") ;//加载dll文件
    if (hdll != NULL)
    {
        MyFunDll_1 MyFunCall_1 = (MyFunDll_1)GetProcAddress(hdll, "OpenCom");
        if (MyFunCall_1 != NULL)
        {
            MyFunCall_1(ucComNo, 9600);	//调用接口函数
        }

        MyFunDll_3 MyFunCall_3 = (MyFunDll_3)GetProcAddress(hdll, "CollectUARTData");

        MyFunDll_2 MyFunCall_2 = (MyFunDll_2)GetProcAddress(hdll, "useReceiveData");//检索要调用函数的地址

        MyFunDll_4 MyFunCall_4 = (MyFunDll_4)GetProcAddress(hdll, "AtomCmd_Compose_Send");




        if (MyFunCall_3 != NULL)
        {
            
            while (1)
            {
                Sleep(100);

                payloadBuffer[0] = 1;

                MyFunCall_4(0xff, 0x11, 0x11, payloadBuffer, 1);

                MyFunCall_2(0x06, 0x81,ucComNo, chrBuffer);
                for (int i = 0; i < sizeof(chrBuffer); i++) {
                    printf("%c", chrBuffer[i]);
                    chrBuffer[i] = NULL;
                }

                MyFunCall_3(ucComNo, chrUARTBufferOutput);
                for (int i = 0; i < sizeof(chrUARTBufferOutput); i++) {
                    printf("%c", chrUARTBufferOutput[i]);
                    chrUARTBufferOutput[i] = NULL;
                }
            }
                //调用接口函数
        }
    }
    return 0;
}
