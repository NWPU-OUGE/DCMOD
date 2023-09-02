/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Design Studio.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.10
import ebikeDesign 1.0
import EbikeData 1.0
import QtQuick.Studio.Components 1.0
import QtQuick.Controls 2.3
import QtQuick.Timeline 1.0
import QtQuick.Layouts 1.3

Item {
    id: page02
    width: Constants.width
    height: 320

    CustomLabel {
        x: 8
        y: 8
        text: qsTr("通用设置")
        font.pixelSize: 24
    }

    GridLayout {
        y: 60
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.left: parent.left
        anchors.leftMargin: 8
        rowSpacing: 20
        rows: 3
        columns: 5

        CustomLabel {
            id: label
            color: "#707070"
            text: "串口号"
            Layout.preferredHeight: 44
            Layout.fillWidth: true
            font.pixelSize: 18
        }

        Item {
            id: spacer
            Layout.columnSpan: 3
            Layout.preferredHeight: 14
            Layout.preferredWidth: 14
        }

        CustomLabel {
            opacity: 1
            visible: true
            color: "#707070"
            text: ""
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            font.pixelSize: 18

            TextInput {
                id: textInput
                x: -116
                y: 0
                width: 109
                height: 25
                visible: true
                color: "#ffffff"
                text: qsTr("请输入串口号")
                font.pixelSize: 18
                selectionColor: "#707070"
                selectByMouse: true
                overwriteMode: true
                maximumLength: 5
                mouseSelectionMode: TextInput.SelectWords
                activeFocusOnTab: false
                focus: true
                font.family: "Teko Medium"
            }
        }

        CustomLabel {
            color: "#707070"
            text: qsTr("波特率")
            Layout.preferredHeight: 44
            font.pixelSize: 18
        }

        CheckBox {
            id: checkBox
            text: qsTr("自动(禁用强制握手)")
            font: label.font
        }

        Slider {
            id: slider1
            stepSize: 1
            value: 10
            z: 0
            scale: 1
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.columnSpan: 3
            enabled: !checkBox.checked
            from: 0
            to: 20
        }

        CustomLabel {
            color: "#707070"
            text: qsTr("测算环")
            Layout.preferredHeight: 44
            font.pixelSize: 18
        }

        Item {
            id: spacer1
            Layout.columnSpan: 2
            Layout.preferredHeight: 14
            Layout.preferredWidth: 14
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.columnSpan: 2

            Button {
                id: button
                text: qsTr("速度环")
                autoExclusive: true
                checked: !Backend.metricSystem
                font: label.font
            }

            Button {
                id: button1
                text: qsTr("扭矩环")
                autoExclusive: true
                checked: Backend.metricSystem
                checkable: true
                font: label.font
            }
        }
    }

    Connections {
        target: button
        onClicked:{
            onClicked: Backend.metricSystem = false
        }
    }

    Text {
        id: text2
        x: 8
        y: 238
        width: 161
        height: 40
        color: "#707070"
        text: qsTr("设置转速（0-5000转）")
        font.pixelSize: 18
        font.family: "Teko Medium"
    }

    Text {
        id: text3
        x: 1168
        y: 239
        color: "#707070"
        text: qsTr("转")
        font.pixelSize: 18
        font.family: "Teko Medium"
    }

    TextInput {
        id: textInput1
        x: 1129
        y: 238
        width: 39
        height: 27
        color: "#707070"
        text: qsTr("0")
        font.pixelSize: 18
        font.family: "Teko Medium"
    }

    Connections {
        target: button1
        onClicked: Backend.metricSystem = true
    }

    Connections {
        target: slider1
        onValueChanged:
        {
            if(slider1.value===0)
                text1.text="2400"
            else if(slider1.value>0&&slider1.value<6)
                text1.text="9600"
            else if(slider1.value>5&&slider1.value<11)
                text1.text="115200"
            else if(slider1.value>10&&slider1.value<16)
                text1.text="512000"
            else if(slider1.value>15&&slider1.value<21)
                text1.text="921600"
        }
        //Backend.brightness = slider1.value
    }

    Connections {
        target: checkBox
        onClicked: {
            if (checkBox.checked) {
                slider1.value = 10.0
            }
        }
    }

    Text {
        id: text1
        x: 1052
        y: 112
        width: 81
        height: 16
        color: "#ffffff"
        text: qsTr("115200")
        font.pixelSize: 18
        font.family: "Times New Roman"
    }

    Connections {
        target: clicktimeTimer
        onTriggered:{
            if(Backend.metricSystem===false)
            {

                text2.text="设置扭矩（0-8A）"
                text3.text="安"
            }
           else if(Backend.metricSystem===true)
            {
                text2.text="设置转速（0-5000转）"
                text3.text="转"
            }
        }
    }



}
