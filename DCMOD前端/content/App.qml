/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Quick Studio Components.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.15
import QtQuick.Window 2.15
import ebikeDesign 1.0

Window {
    width: Constants.width
    height: Constants.height

    visible: true
    title: "DCMOD-电机观测调试系统 by DC-TEAM"
    QtObject{
               id:attrs;
               property int counter;    //测量中断计数器 1秒加一次
               property int counter2;   //运行时中断计数器 0。001秒加一次
               property int buffer; //储存采样精度
               Component.onCompleted: {
                   attrs.counter=0;
                   attrs.buffer=0;
               }
}
    Timer{
           id:measureTimer;//测量中断计数器 1秒一次中断 处理所需实时性最差的控件
           interval: 1000; //定时周期 1s
           repeat:true;    //
           triggeredOnStart: true;
           onTriggered: {
               attrs.counter++;

//                if(attrs.counter===60){
//                    helloTimer.stop();
//                    helloText.text="hello！！！";
//                }
           }
       }
    Timer{
           id:runtimeTimer;//最小中断计数器 1毫秒一次中断 处理所需实时性最好的控件
           interval: 1; //定时周期 1ms
           repeat:true;    //
           triggeredOnStart: true;
           onTriggered: {
               attrs.counter2++;

//                if(attrs.counter===60){
//                    helloTimer.stop();
//                    helloText.text="hello！！！";
//                }
           }
       }
    Timer{
           id:blinktimeTimer;//快速中断计数器 10毫秒一次中断 处理所需实时性好的控件
           interval: 1; //定时周期 10ms
           repeat:true;    //
           triggeredOnStart: true;
           onTriggered: {
               attrs.counter2++;

//                if(attrs.counter===60){
//                    helloTimer.stop();
//                    helloText.text="hello！！！";
//                }
           }
       }
    Timer{
           id:clicktimeTimer;//一般中断计数器 100毫秒一次中断 大部分实时性控件采用此函数
           interval: 1; //定时周期 100ms
           repeat:true;    //
           triggeredOnStart: true;
           onTriggered: {
               //attrs.counter2++;

//                if(attrs.counter===60){
//                    helloTimer.stop();
//                    helloText.text="hello！！！";
//                }
           }
       }

    Screen01 {

    }

}

