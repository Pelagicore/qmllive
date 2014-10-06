import QtQuick 1.1

Rectangle {
    id: root
    width: 1024
    height: 768
    color: "#FF6600"

    Image {
        id: logo
        source: "pelagicore-symbol-white-rgb.png"
        x: 50
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 250

        fillMode: Image.PreserveAspectFit
    }

    Text {
        id: mantraText

        text: "Open Source Infotainment\n    Enabling Great Design"

        font.pixelSize: 60
        color: "white"
        lineHeight: 1.2

        y: logo.y
        anchors.left: logo.right
        anchors.leftMargin: 25

    }

    Text {
        id: qmllivetext
        text: "QmlLive"

        font.pixelSize: 70
        color: "white"

        x: mantraText.x
        anchors.top: mantraText.bottom
        anchors.margins: 200
    }
    Text {
        id: pleaseConnectText
        text: "Please connect with QmlLiveRemote"

        font.pixelSize: 40
        color: "white"
        SequentialAnimation {
            running: true
            loops: Animation.Infinite
            ColorAnimation { target: pleaseConnectText; property: "color"; from: "white"; to: root.color; duration: 2000;}
            ColorAnimation { target: pleaseConnectText; property: "color"; from: root.color; to: "white"; duration: 2000;}
        }

        x: mantraText.x
        anchors.top: qmllivetext.bottom
        anchors.margins: 20
    }
}
