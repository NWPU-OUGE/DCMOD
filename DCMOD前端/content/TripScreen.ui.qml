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
    id: tripScreen
    width: Constants.width
    height: Constants.height
    property alias currentFrame: timeline.currentFrame

    Rectangle {
        id: leftButton
        x: 35
        y: 102
        width: 22
        height: 22
        color: "#00000000"
        radius: 11
        border.color: "#696969"

        Image {
            x: -9
            y: -9
            source: "images/arrow_left.png"
        }
    }

    Rectangle {
        id: tripButton
        x: 590
        y: 102
        width: 100
        height: 24
        color: "#00000000"
        radius: 11
        border.color: "#696969"

        CustomLabel {
            id: toStandard
            color: "#ffffff"
            text: qsTr("结束测量")
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
            font.pixelSize: 18
        }
    }

    Rectangle {
        id: rightButton
        x: 1225
        y: 102
        width: 22
        height: 22
        color: "#00000000"
        radius: 11
        border.color: "#696969"

        Image {
            x: -9
            y: -9
            source: "images/arrow_right.png"
        }
    }


    RowLayout {
        x: 1131
        y: 61
        spacing: 9

        CustomLabel {
            color: "#9d9d9d"
            text: qsTr("累计采样")
            font.pixelSize: 20
        }

        CustomLabel {
            color: "#ffffff"
            text: ""
            font.pixelSize: 24
        }

        CustomLabel {
            color: "#9d9d9d"
            text: "\u79d2"
            font.pixelSize: 20
        }
    }

    Text {
        id: measureTimertext
        x: 1212
        y: 60
        color: "#ffffff"
        text: qsTr("0")
        font.pixelSize: 24
        font.family: "Teko Medium"
        Connections {
            target: measureTimer
            onTriggered:  measureTimertext.text = attrs.counter;

        }
    }

    GridLayout {
        x: 35
        y: 245
        width: 1212
        height: 118
        columnSpacing: 16
        rows: 3
        columns: 4

        CustomLabel {
            id: firstL
            color: "#9d9d9d"
            text: qsTr("平均扭矩 (N*m) ")
            font.pixelSize: 24
        }

        CustomLabel {
            color: "#ffffff"
            text: "7:51"
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 24
        }

        CustomLabel {
            color: "#9d9d9d"
            text: qsTr("最大转速 (r/min)")
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 24
        }

        CustomLabel {
            color: "#ffffff"
            text: (Backend.metricSystem ? "37.8" : "23.5")
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
            font.pixelSize: 24
        }

        CustomLabel {
            color: "#9d9d9d"
            text: qsTr("最大扭矩 (N*m)")
            font.pixelSize: 24
        }

        CustomLabel {
            color: "#ffffff"
            text: (Backend.metricSystem ? "148.8" : "92.5")
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 24
        }

        CustomLabel {
            color: "#9d9d9d"
            text: qsTr("平均转速 (r/min)")
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 24
        }

        CustomLabel {
            color: "#ffffff"
            text: (Backend.metricSystem ? "18.9" : "11.8")
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
            font.pixelSize: 24
        }

        CustomLabel {
            color: "#9d9d9d"
            text: qsTr("最高温度 (摄氏度)")
            font.pixelSize: 24
        }

        CustomLabel {
            color: "#ffffff"
            text: "2834.5"
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 24
        }

        CustomLabel {
            color: "#9d9d9d"
            text: qsTr("累计转动 (r)")
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 24
        }

        CustomLabel {
            color: "#ffffff"
            text: "0.0"
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
            font.pixelSize: 24
        }
    }

    Image {
        id: chart
        x: 242
        y: 836
        source: "maps/chart01.png"
    }

    Timeline {
        id: timeline
        endFrame: 1000
        enabled: true
        startFrame: 0

        KeyframeGroup {
            target: chart
            property: "y"
            Keyframe {
                value: 836
                frame: 0
            }

            Keyframe {
                easing.bezierCurve: [0.39, 0.575, 0.552, 0.809, 1, 1]
                value: 376
                frame: 1000
            }
        }
    }

    Button {
        id: tripEnd
        x: 591
        y: 102
        width: 100
        height: 23
        opacity: 0
        visible: true
        text: qsTr("")

        Connections {
            target: tripEnd
            onClicked: {
                measureTimer.stop()
                attrs.counter=0;
                measureTimertext.text="0"
                root.state = "TripToStandard"
}
        }
    }


}

/*##^##
Designer {
    D{i:0;formeditorZoom:0.9}D{i:2}D{i:1}D{i:4}D{i:3}D{i:6}D{i:5}D{i:8}D{i:9}D{i:10}D{i:7}
D{i:12}D{i:11}D{i:14}D{i:15}D{i:16}D{i:17}D{i:18}D{i:19}D{i:20}D{i:21}D{i:22}D{i:23}
D{i:24}D{i:25}D{i:13}D{i:26}D{i:27}D{i:32}D{i:31}
}
##^##*/
