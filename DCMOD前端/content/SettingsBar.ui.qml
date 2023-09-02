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
    id: settings
    width: Constants.width
    height: Constants.height

    state: "closed"

    Rectangle {
        id: blocker
        x: 0
        width: Constants.width
        color: "#464646"
        opacity: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 717
        anchors.topMargin: -10
        anchors.top: stack.bottom

        MouseArea {
            id: mouseArea2
            anchors.fill: parent
        }
    }

    Image {
        id: close
        x: 564
        y: 44
        source: "images/top_curtain_drag_back.png"

        MouseArea {
            id: mouseArea
            anchors.rightMargin: -16
            anchors.leftMargin: -16
            anchors.bottomMargin: 8
            anchors.fill: parent
        }
    }

    Rectangle {
        id: stack
        x: 0
        y: 44
        width: Constants.width
        height: 200
        color: "#0d0d0d"
        clip: true

        MouseArea {
            anchors.fill: parent
        }

        StackLayout {
            id: stackLayout
            width: Constants.width
            currentIndex: tabbar.index
            anchors.fill: parent

            BikeInfo { id: motorInfo}

            GeneralSettings {}

            ViewSettings {}
        }
    }

    Rectangle {
        id: topBar
        x: 0
        y: 3
        width: Constants.width
        height: 45
        color: "#0d0d0d"

        Image {
            id: icon
            x: 1193
            y: 3
            opacity: 1
            visible: true
            source: "../images/串口icon.png"
            sourceSize.height: 40
            sourceSize.width: 40
            fillMode: Image.PreserveAspectFit
        }

        Image {
            id: icon1
            x: 1240
            y: 8
            width: 29
            height: 30
            opacity: 0
            source: "../images/断开icon.png"
            fillMode: Image.PreserveAspectFit
        }

        Image {
            id: iconon
            x: 1193
            y: 3
            width: 42
            opacity: 0
            source: "../images/串口iconon.png"
            fillMode: Image.PreserveAspectFit
        }

        Image {
            id: iconon1
            x: 1240
            y: 8
            width: 29
            height: 30
            source: "../images/断开iconon.png"
            fillMode: Image.PreserveAspectFit
        }
    }

    Image {
        id: open
        x: 564
        y: 44
        source: "images/top_curtain_drag.png"

        MouseArea {
            id: mouseArea1
            anchors.rightMargin: -16
            anchors.leftMargin: -16
            anchors.bottomMargin: 8
            anchors.fill: parent
        }

        CustomLabel {
            id: customLabel
            x: -245
            y: 316
            text: Backend.time
            font.pixelSize: 22
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
        }
    }

    Timeline {
        id: timeline
        animations: [
            TimelineAnimation {
                id: openAnimation
                onFinished: settings.state = "Open"
                loops: 1
                duration: 250
                from: 0
                to: 1000
                running: false
            },
            TimelineAnimation {
                id: closeAnimation
                onFinished: settings.state = "Closed"
                loops: 1
                duration: 250
                from: 1000
                to: 0
                running: false
            }
        ]
        endFrame: 1000
        enabled: true
        startFrame: 0

        KeyframeGroup {
            target: stack
            property: "height"

            Keyframe {
                value: 0
                frame: 0
            }

            Keyframe {
                easing.bezierCurve: [0.39, 0.575, 0.565, 1, 1, 1]
                value: 320
                frame: 1000
            }

            Keyframe {
                value: 0
                frame: 89
            }
        }

        KeyframeGroup {
            target: close
            property: "opacity"
            Keyframe {
                easing.bezierCurve: [0.39, 0.575, 0.565, 1, 1, 1]
                value: 1
                frame: 1000
            }

            Keyframe {
                value: 0
                frame: 850
            }

            Keyframe {
                value: 0
                frame: 0
            }
        }

        KeyframeGroup {
            target: close
            property: "y"
            Keyframe {
                value: 315
                frame: 850
            }

            Keyframe {
                easing.bezierCurve: [0.39, 0.575, 0.565, 1, 1, 1]
                value: 360
                frame: 1000
            }
        }

        KeyframeGroup {
            target: open
            property: "opacity"

            Keyframe {
                value: 1
                frame: 0
            }

            Keyframe {
                value: 0
                frame: 88
            }
        }

        KeyframeGroup {
            target: tabbar
            property: "opacity"

            Keyframe {
                value: 0
                frame: 0
            }

            Keyframe {
                value: 1
                frame: 1000
            }
        }

        KeyframeGroup {
            target: blocker
            property: "opacity"
            Keyframe {
                value: 0
                frame: 0
            }

            Keyframe {
                value: 0.2
                frame: 1000
            }

            Keyframe {
                frame: 794
                value: "0"
            }
        }

        KeyframeGroup {
            target: stackLayout
            property: "opacity"

            Keyframe {
                value: 0
                frame: 0
            }

            Keyframe {
                value: 0
                frame: 300
            }

            Keyframe {
                value: 1
                frame: 1000
            }
        }

        KeyframeGroup {
            target: blocker
            property: "anchors.bottomMargin"
            Keyframe {
                value: 708
                frame: 0
            }

            Keyframe {
                value: 0
                frame: 1000
            }
        }
    }

    Connections {
        target: mouseArea
        onClicked: {
            settings.state = "Closing"
        }
    }

    Connections {
        target: mouseArea1
        onClicked: {
            if (settings.state != "Open")
                settings.state = "Opening"
            clicktimeTimer.start()
        }
    }

    Tabbar {
        id: tabbar
        x: 0
        y: 0
        index: 0
    }



    states: [
        State {
            name: "Open"

            PropertyChanges {
                target: timeline
                currentFrame: 1000
                enabled: true
            }
        },
        State {
            name: "Closed"

            PropertyChanges {
                target: timeline
                currentFrame: 0
                enabled: true
            }
        },
        State {
            name: "Opening"

            PropertyChanges {
                target: timeline
                enabled: true
            }

            PropertyChanges {
                target: openAnimation
                running: true
            }
        },
        State {
            name: "Closing"

            PropertyChanges {
                target: timeline
                enabled: true
            }

            PropertyChanges {
                target: closeAnimation
                running: true
            }
        }
    ]
}


/*##^##
Designer {
    D{i:0;formeditorZoom:0.9}D{i:2}D{i:1}D{i:4}D{i:3}D{i:6}D{i:8}D{i:9}D{i:10}D{i:7}D{i:5}
D{i:12}D{i:13}D{i:14}D{i:15}D{i:11}D{i:17}D{i:18}D{i:16}D{i:19}D{i:50}D{i:51}D{i:52}
D{i:53}
}
##^##*/
