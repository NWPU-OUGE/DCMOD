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
import QtQuick.Studio.Components 1.0
import QtQuick.Controls 2.3
import QtQuick.Timeline 1.0
import QtQuick.Layouts 1.3
import FlowView 1.0
import QtCharts 6.2

Item {
    id: page01
    width: Constants.width
    height: 320

    CustomLabel {
        x: 8
        y: 8
        text: qsTr("电机信息")
        font.pixelSize: 24
    }

    CustomLabel {
        x: 8
        y: 57
        text: qsTr("电池供电？")
        font.pixelSize: 22
    }

    CheckDelegate {
        id: checkDelegate
        x: 114
        y: 56
        width: 41
        height: 30
        text: qsTr("")
        tristate: false
        checkState: Qt.Checked
        wheelEnabled: false
        display: AbstractButton.IconOnly
        hoverEnabled: false
        checked: true
        checkable: true
    }

    ColumnLayout {
        x: 8
        y: 121
        spacing: 10

        ColumnLayout {
            spacing: 0

            CustomLabel {
                color: "#707070"
                text: qsTr("额定电压")
                font.pixelSize: 18
            }

            CustomLabel {
                opacity: 1
                visible: true
                text: ""
                font.pixelSize: 22

                TextInput {
                    id: textInput
                    x: 0
                    y: 0
                    width: 40
                    height: 32
                    color: "#dfdfdf"
                    text: qsTr("36")
                    font.pixelSize: 18
                    activeFocusOnPress: true
                    autoScroll: true
                    activeFocusOnTab: false
                    enabled: true
                    antialiasing: false
                    focus: true
                    selectByMouse: true
                    cursorVisible: false
                    overwriteMode: false
                    selectionColor: "#ffffff"
                    font.family: "Teko Medium"

                    Text {
                        id: text1
                        x: 16
                        y: 0
                        width: 40
                        height: 32
                        color: "#ffffff"
                        text: qsTr("V")
                        font.pixelSize: 18
                        font.family: "Teko Medium"
                    }
                }
            }
        }

        ColumnLayout {
            spacing: 0
            CustomLabel {
                color: "#707070"
                text: qsTr("上一次整定参数日期")
                font.pixelSize: 18
            }

            CustomLabel {
                text: "25/12/2021"
                font.pixelSize: 22
            }
        }

        ColumnLayout {
            spacing: 0
            CustomLabel {
                color: "#707070"
                text: qsTr("下一次推荐整定日期")
                font.pixelSize: 18
            }

            CustomLabel {
                text: "07/7/2022"
                font.pixelSize: 22
            }
        }
    }

    Image {
        id: motor
        x: 985
        y: 69
        width: 266
        height: 285
        source: "../images/motor.png"
        fillMode: Image.PreserveAspectFit
    }

    Row {
    }

    Column {
        x: 890
        y: 143
        spacing: 30

        Text {
            id: pid_p
            width: 21
            height: 30
            color: "#ffffff"
            text: qsTr("P:")
            font.pixelSize: 21
        }

        Text {
            id: pid_i
            width: 21
            height: 30
            color: "#ffffff"
            text: qsTr("I:")
            font.pixelSize: 20
        }

        Text {
            id: pid_d
            width: 21
            height: 30
            color: "#ffffff"
            text: qsTr("D:")
            font.pixelSize: 20
        }
    }

    Column {
        x: 917
        y: 152
        spacing: 40

        TextInput {
            id: p_input
            width: 105
            height: 20
            color: "#ffffff"
            text: qsTr("0.000")
            font.pixelSize: 20
            horizontalAlignment: Text.AlignHCenter
            selectByMouse: true
            font.family: "Teko Medium"
        }

        TextInput {
            id: p_input1
            width: 105
            height: 20
            color: "#ffffff"
            text: qsTr("0.000")
            font.pixelSize: 20
            horizontalAlignment: Text.AlignHCenter
            selectByMouse: true
            font.family: "Teko Medium"
        }

        TextInput {
            id: p_input2
            width: 105
            height: 20
            color: "#ffffff"
            text: qsTr("0.000")
            font.pixelSize: 20
            horizontalAlignment: Text.AlignHCenter
            selectByMouse: true
            font.family: "Teko Medium"
        }
    }






}

/*##^##
Designer {
    D{i:0;formeditorZoom:1.25}D{i:1}D{i:2}D{i:3}D{i:6}D{i:9}D{i:8}D{i:7}D{i:5}D{i:11}
D{i:12}D{i:10}D{i:14}D{i:15}D{i:13}D{i:4}D{i:16}D{i:17}D{i:19}D{i:20}D{i:21}D{i:18}
D{i:23}D{i:24}D{i:25}D{i:22}
}
##^##*/
