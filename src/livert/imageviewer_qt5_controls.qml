import QtQuick 2.0

Item {
    id: root
    width: 480; height: 272
    Rectangle {
        anchors.fill: parent
        color: imageViewerBackgroundColor
    }
    Image {
        id: image
        anchors.centerIn: parent
        source: imageViewerSource
    }

    Text {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 4
        color: '#ffffff'
        text: image.width + "x" + image.height
    }
}
