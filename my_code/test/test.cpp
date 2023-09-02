#include <stdio.h>
#include <windows.h>
#include <string>
#include <process.h>

#define SLEEP_TIME 1000

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

void send_package(u16 v_x, u16 v_y, u16 v_z, u8 mod);
void intial();
void shake_hand();
void package_parser();


#define RING_BUFFER_SIZE (52*1024)


u8 chrUARTBufferOutput[RING_BUFFER_SIZE];//接收数组
u8 sendBuffer[52];//发送的包

u8 getBuffer[12];//收的数据

HMODULE hdll = LoadLibrary(L"C:\\Users\\Govern liu\\Desktop\\ATOM_LINKER Examples\\Examples\\Windows MeasureDataDemo\\x64\\Debug\\MeasureDataDemo.dll");//加载dll文件


u8 pBufferStart;
u8 pBufferEnd;
u8 pCustomer;
u8 pProducer;
unsigned long ucComNo = 5;
u8 MADDR = 0xFF;
u8 CID = 0x05;
u8 MID = 0x01;
u8 PL = 0x2C;

float temp[1];


typedef struct
{
    float v_x;
    float v_y;
    float v_z;


}DATA;

typedef signed char(*MyFunDll_1)(unsigned long ucComNo, unsigned long baud);
typedef int(*MyFunDll_2)(u8 cid, u8 mid, unsigned long ucComNo, unsigned char* chrBuffer);
typedef unsigned short(*MyFunDll_3)(const unsigned long ulCOMNo, char chrUARTBufferOutput[]);
typedef int(*MyFunDll_4)(u8 MADDR, u8 ClassID, u8 msgID, u8* payloadBuffer, u16 PayloadLen);

MyFunDll_1 MyFunCall_1 = (MyFunDll_1)GetProcAddress(hdll, "OpenCom");
MyFunDll_3 collect = (MyFunDll_3)GetProcAddress(hdll, "CollectUARTData");
MyFunDll_4 send_1 = (MyFunDll_4)GetProcAddress(hdll, "AtomCmd_Compose_Send");

DATA data;

void ThreadUser(void*)
{
    while (1)
    {
        Sleep(SLEEP_TIME);
        collect(ucComNo, (char*)chrUARTBufferOutput);
    }

    _endthread();
}

extern "C" _declspec(dllexport) void run()
{
    _beginthread(ThreadUser, 0, NULL);

    if (hdll != NULL)
    {
        if (MyFunCall_1 != NULL)
        {
            MyFunCall_1(ucComNo, 115200);	//调用接口函数
        }


        if (collect != NULL && send_1 != NULL)
        {
            shake_hand();
            intial();
            send_package(0x0020, 0x0016, 0x0014, 0x02);


            send_1(MADDR, CID, MID, sendBuffer, 52);

            while (1)
            {
                package_parser();

            }

        }
    }
}

int main(void)
{
    
    run();
    return 0;

}

extern "C" _declspec(dllexport) float get_x()
{
    data.v_x = 0;
    return data.v_x;
}

extern "C" _declspec(dllexport) float get_y()
{
    return data.v_y;
}

extern "C" _declspec(dllexport) float get_z()
{
    return data.v_z;
}


void package_parser()
{

    for (int i = 0; i < sizeof(chrUARTBufferOutput) - 52; i++)
    {

        if (chrUARTBufferOutput[i] == 'A' && chrUARTBufferOutput[i + 1] == 'x'
            && chrUARTBufferOutput[i + 3] == CID && chrUARTBufferOutput[i + 4] == MID
            && chrUARTBufferOutput[i + 51] == 'm')
        {

            memcpy(getBuffer, &chrUARTBufferOutput[i + 22], 12);

            memcpy(temp, &chrUARTBufferOutput[i + 22], 4);
            data.v_x = temp[0];
            temp[0] = NULL;

            memcpy(temp, &chrUARTBufferOutput[i + 26], 4);
            data.v_y = temp[0];
            temp[0] = NULL;

            memcpy(temp, &chrUARTBufferOutput[i + 30], 4);
            data.v_z = temp[0];
            temp[0] = NULL;

            printf("%f %f %f\n", data.v_x, data.v_y, data.v_z);

            i += 50;
        }
    }
    //for (int i = 0; i < sizeof(chrUARTBufferOutput)-52; i++) {
    //    if (chrUARTBufferOutput[i] == 'A' && chrUARTBufferOutput[i + 1] == 'x'
    //        &&  chrUARTBufferOutput[i + 51] == 'm') {
    //        for (int j = i; j < i + 52; j++) {
    //            {
    //                printf("%02X ", chrUARTBufferOutput[j]);
    //                            
    //            }
    //        }
    //        printf("\n");
    //        //if (sizeof(chrUARTBufferOutput) >= 52 * 1000)
    //          //  memset(chrUARTBufferOutput, NULL, sizeof(chrUARTBufferOutput));
    //    }
    //    
    //}

}

void shake_hand()
{
    Sleep(100);
    sendBuffer[0] = 'A';
    sendBuffer[1] = 'x';
    sendBuffer[2] = 0xFF;
    sendBuffer[3] = 0x01;
    sendBuffer[4] = 0x02;
    sendBuffer[5] = 0x00;
    sendBuffer[6] = 0xC5;
    sendBuffer[7] = 'm';
    send_1(MADDR, CID, MID, sendBuffer, 8);
}

void send_package(u16 v_x, u16 v_y, u16 v_z, u8 mod)
{
    typedef u8(*MyFunDll)(u8* addr, u16 len);
    MyFunDll BCC = (MyFunDll)GetProcAddress(hdll, "Atom_BCC");

    sendBuffer[8] = ((v_x >> 8) & 0xff);
    sendBuffer[9] = (v_x & 0xff);
    sendBuffer[10] = ((v_y >> 8) & 0xff);
    sendBuffer[11] = (v_y & 0xff);
    sendBuffer[12] = ((v_z >> 8) & 0xff);
    sendBuffer[13] = (v_z & 0xff);
    sendBuffer[18] = mod;
    sendBuffer[50] = BCC(sendBuffer, PL - 2);
}

void intial()
{
    sendBuffer[0] = 'A';
    sendBuffer[1] = 'x';
    sendBuffer[2] = MADDR;
    sendBuffer[3] = CID;
    sendBuffer[4] = MID;
    sendBuffer[5] = PL;
    sendBuffer[6] = 0x1E;
    sendBuffer[7] = 0x0C;
    sendBuffer[17] = 0x02;

    for (int i = 14; i <= 16; i++)
    {
        sendBuffer[i] = 0x00;
    }

    sendBuffer[19] = 0x02;
    sendBuffer[20] = 0x19;
    sendBuffer[21] = 0x0C;
    for (int i = 22; i <= 33; i++)
    {
        sendBuffer[i] = 0x00;
    }

    sendBuffer[34] = 0x22;
    sendBuffer[35] = 0x08;
    for (int i = 36; i <= 43; i++)
    {
        sendBuffer[i] = 0x00;
    }
    sendBuffer[44] = 0x31;
    sendBuffer[45] = 0x04;
    for (int i = 46; i <= 49; i++)
    {
        sendBuffer[i] = 0x00;
    }
    sendBuffer[51] = 0x6D;
}


