import QtQuick 2.0

Rectangle {
    width: 800
    height: 480

    SystemPalette {
        id: palette;
        colorGroup: SystemPalette.Active
    }
    color: palette.base

    Image {
        anchors.fill: parent;
        source: "logo.png"
        fillMode: Image.Tile
        opacity: 0.5
    }
    Text {
        anchors.centerIn: parent
        text: qsTr("An error occured - please check the Log Output pane.")
        font.pointSize: 20
        font.bold: true
        color: palette.text
    }
}
