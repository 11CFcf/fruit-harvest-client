import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window
    width: 1120
    height: 760
    minimumWidth: 820
    minimumHeight: 640
    visible: true
    title: "智能采摘 · 果实计数"
    color: "#f4f6f1"

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 48
        anchors.rightMargin: 48
        anchors.topMargin: 34
        anchors.bottomMargin: 28
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            spacing: 14
            Rectangle {
                width: 48
                height: 48
                radius: 16
                color: "#e2eddf"
                Text {
                    anchors.centerIn: parent
                    text: "枝"
                    color: "#315c3b"
                    font.pixelSize: 22
                    font.weight: Font.DemiBold
                }
            }
            ColumnLayout {
                spacing: 3
                Text {
                    text: "智能采摘系统"
                    color: "#819084"
                    font.pixelSize: 12
                }
                Text {
                    text: "果实采收总览"
                    color: "#24392a"
                    font.pixelSize: 22
                    font.weight: Font.DemiBold
                }
            }
            Item { Layout.fillWidth: true }
            Rectangle {
                radius: 17
                color: countModel.serverRunning ? "#e6f1e7" : "#f8e9e4"
                implicitWidth: serviceStatus.implicitWidth + 24
                implicitHeight: 36
                Row {
                    id: serviceStatus
                    anchors.centerIn: parent
                    spacing: 8
                    Rectangle {
                        width: 8
                        height: 8
                        radius: 4
                        anchors.verticalCenter: parent.verticalCenter
                        color: countModel.serverRunning ? "#4d9c5c" : "#cb6553"
                    }
                    Text {
                        text: countModel.serverRunning ? "本地服务运行中" : "本地服务启动失败"
                        color: countModel.serverRunning ? "#387849" : "#a24e40"
                        font.pixelSize: 13
                        font.weight: Font.Medium
                    }
                }
            }
        }

        Item { Layout.preferredHeight: 30 }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 174
            radius: 22
            color: "#315c3b"
            clip: true

            Rectangle {
                width: 290
                height: 290
                radius: 145
                x: parent.width - 245
                y: -190
                color: "white"
                opacity: 0.045
            }
            Rectangle {
                width: 190
                height: 190
                radius: 95
                x: parent.width - 115
                y: 80
                color: "white"
                opacity: 0.035
            }
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 32
                anchors.rightMargin: 32
                spacing: 30
                ColumnLayout {
                    spacing: 7
                    Text {
                        text: "累计采摘果实"
                        color: "#d2dfd1"
                        font.pixelSize: 14
                    }
                    RowLayout {
                        spacing: 10
                        Text {
                            text: countModel.total.toLocaleString(Qt.locale(), 'f', 0)
                            color: "white"
                            font.pixelSize: 58
                            font.weight: Font.DemiBold
                        }
                        Text {
                            text: "个"
                            color: "#d2dfd1"
                            font.pixelSize: 17
                            Layout.bottomMargin: 10
                        }
                    }
                    Text {
                        text: "苹果与橘子 · 摄像头识别统计"
                        color: "#d2dfd1"
                        font.pixelSize: 12
                    }
                }
                Item { Layout.fillWidth: true }
                Rectangle {
                    Layout.preferredWidth: 1
                    Layout.preferredHeight: 86
                    color: "white"
                    opacity: 0.25
                }
                ColumnLayout {
                    Layout.preferredWidth: 255
                    spacing: 12
                    RowLayout {
                        spacing: 10
                        Rectangle {
                            width: 9
                            height: 9
                            radius: 5
                            color: countModel.cameraOnline ? "#95d69e" : "#edbc78"
                        }
                        Text {
                            text: countModel.cameraOnline ? "摄像头数据接收正常" : "等待摄像头上传"
                            color: "white"
                            font.pixelSize: 14
                            font.weight: Font.Medium
                        }
                    }
                    Text {
                        text: "最近上传  " + countModel.lastUpdated
                        color: "#d2dfd1"
                        font.pixelSize: 12
                    }
                }
            }
        }

        Item { Layout.preferredHeight: 22 }

        RowLayout {
            Layout.fillWidth: true
            Text {
                text: "果实分类"
                color: "#35473a"
                font.pixelSize: 16
                font.weight: Font.DemiBold
            }
            Item { Layout.fillWidth: true }
            Text {
                text: "接收频率由摄像头决定"
                color: "#89948b"
                font.pixelSize: 12
            }
        }

        Item { Layout.preferredHeight: 13 }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 18
            FruitCard {
                Layout.fillWidth: true
                Layout.fillHeight: true
                fruitName: "苹果"
                fruitIcon: "🍎"
                count: countModel.apples
                share: countModel.total > 0 ? countModel.apples / countModel.total : 0
                accent: "#c85b4c"
                tint: "#fff0ea"
            }
            FruitCard {
                Layout.fillWidth: true
                Layout.fillHeight: true
                fruitName: "橘子"
                fruitIcon: "🍊"
                count: countModel.oranges
                share: countModel.total > 0 ? countModel.oranges / countModel.total : 0
                accent: "#c48027"
                tint: "#fff4df"
            }
        }

        Item { Layout.preferredHeight: 18 }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 88
            radius: 16
            color: "white"
            border.color: "#e7ebe5"
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                spacing: 14
                Rectangle {
                    width: 38
                    height: 38
                    radius: 12
                    color: "#edf2eb"
                    Text {
                        anchors.centerIn: parent
                        text: "⌁"
                        color: "#496e50"
                        font.pixelSize: 21
                    }
                }
                ColumnLayout {
                    spacing: 4
                    Text {
                        text: "摄像头接入地址"
                        color: "#3c4e40"
                        font.pixelSize: 13
                        font.weight: Font.DemiBold
                    }
                    Text {
                        text: "POST  http://<本机局域网IP>:8080/api/counts"
                        color: "#7c897f"
                        font.pixelSize: 12
                        font.family: "Consolas"
                    }
                }
                Item { Layout.fillWidth: true }
                Rectangle {
                    radius: 10
                    color: countModel.serverRunning ? "#f0f5ef" : "#fbefec"
                    implicitWidth: apiStatus.implicitWidth + 18
                    implicitHeight: 30
                    Text {
                        id: apiStatus
                        anchors.centerIn: parent
                        text: countModel.serverRunning ? "端口 8080" : countModel.serverError
                        color: countModel.serverRunning ? "#53755a" : "#a24e40"
                        font.pixelSize: 11
                    }
                }
            }
        }
    }

    component FruitCard: Rectangle {
        property string fruitName
        property string fruitIcon
        property int count
        property real share
        property color accent
        property color tint

        radius: 18
        color: "white"
        border.color: "#e7ebe5"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 23
            spacing: 0
            RowLayout {
                spacing: 11
                Rectangle {
                    width: 46
                    height: 46
                    radius: 14
                    color: tint
                    Text {
                        anchors.centerIn: parent
                        text: fruitIcon
                        font.pixelSize: 25
                    }
                }
                ColumnLayout {
                    spacing: 3
                    Text {
                        text: fruitName
                        color: "#35473a"
                        font.pixelSize: 16
                        font.weight: Font.DemiBold
                    }
                    Text {
                        text: "采摘数量"
                        color: "#97a098"
                        font.pixelSize: 11
                    }
                }
                Item { Layout.fillWidth: true }
                Text {
                    text: Math.round(share * 100) + "%"
                    color: accent
                    font.pixelSize: 13
                    font.weight: Font.DemiBold
                }
            }
            Item { Layout.fillHeight: true }
            RowLayout {
                spacing: 8
                Text {
                    text: count.toLocaleString(Qt.locale(), 'f', 0)
                    color: "#26392c"
                    font.pixelSize: 43
                    font.weight: Font.DemiBold
                }
                Text {
                    text: "个"
                    color: "#829086"
                    font.pixelSize: 14
                    Layout.bottomMargin: 6
                }
            }
            Item { Layout.preferredHeight: 17 }
            Rectangle {
                Layout.fillWidth: true
                height: 6
                radius: 3
                color: "#eef1ed"
                Rectangle {
                    width: parent.width * Math.max(0, Math.min(1, share))
                    height: parent.height
                    radius: 3
                    color: accent
                }
            }
            Item { Layout.preferredHeight: 10 }
            Text {
                text: "占总采摘量 " + Math.round(share * 100) + "%"
                color: "#89948b"
                font.pixelSize: 11
            }
        }
    }
}
