#include <stdio.h>
#include<string>   
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include "nubot_hwcontroller_node.h"
#ifdef ROS_ON
#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include <ros/time.h>
#endif
#include "_ros_inc/_serial.h"
#include "_ros_inc/_macro.h"
#include "_ros_inc/_protocol.h"
#include "_ros_inc/_config.h"
#include "_ros_inc/_tool.h"
using namespace std;
void Nubot_HWController::SerialWrite(const nubot_common::VelCmd::ConstPtr& cmd,nFD)  //velcmd是一个msg文件
{
    Cmd_Vx = (int)(cmd->Vx*1.2);
    Cmd_Vy = (int)(cmd->Vy*1.2);
    Cmd_w = (int)(cmd->w*1.5);
    u8 MADDR = 0xFF;
    u8 classID = 0x05;
    u8 msgID = 0x01;
    u8 payloadData[6];
    u16 payloadLen = 6;
    if(!(cmd->stop_ == false && cmd->Vx == 0 && cmd->Vy == 0 && cmd->w == 0))
    {
            payloadData[0] = Cmd_Vx;
            payloadData[1] = Cmd_Vy;
            payloadData[2] = Cmd_w;
            payloadData[3] = shoottag1;
            payloadData[4] = shoottag2;
            payloadData[5] = shoottag3;
        AtomCmd_Compose_Send(nFD, MADDR, classID, msgID, (u8*)payloadData, payloadLen);
       // std::stringstream write_stream_;
        //下面这行是之前的协议传输格式
      //  write_stream_ <<"A"<< Cmd_Vx << "\t"<< Cmd_Vy << "\t"<< Cmd_w <<"\t"<<shoottag1<<"\t"<<shoottag2<<"\t"<<shoottag3<< "\n";
       // std_msgs::String write_stream;
      //  write_stream.data = write_stream_.str();
      //  serial.write(write_stream.data); //此处之前是串口通信，调用库函数把  write_stream内容写给下位机
    }
}


