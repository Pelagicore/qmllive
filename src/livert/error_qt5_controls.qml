import QtQuick 2.1
import QtQuick.Controls 1.0

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
    Label {
        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        text: qsTr("An error occured - please check the Log Output pane. (Controls)")
        wrapMode: Text.WordWrap
        font.pointSize: 20
        font.bold: true
        color: palette.text
    }
}
