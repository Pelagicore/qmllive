import QtQuick 2.0

Item {
    id: root
    width: livert.screenWidth
    height: livert.screenHeight

    property color backgroundColor: '#666'
    property color textColor: '#fff'

    Rectangle {
        anchors.fill: parent
        color: root.backgroundColor
    }

    Rectangle {
        id: header

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: 32

        z: 2

        color: "#E0E0E0"

        Rectangle {
        }

        Row {
            anchors.left: parent.left
            anchors.leftMargin: 8
            width: childrenRect.width
            height: 24
            anchors.verticalCenter: parent.verticalCenter
            Repeater {
                model: [
                    { bg: '#000', text: '#fff' },
                    { bg: '#666', text: '#000' },
                    { bg: '#fff', text: '#000' }
                ]
                Rectangle {
                    color: modelData.bg
                    width: 24
                    height: parent.height
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.backgroundColor = modelData.bg
                            root.textColor = modelData.text
                        }
                    }
                }
            }
        }

        Item {
            id: slider
            anchors.right: parent.right
            anchors.rightMargin: 8
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            property real minValue: 80
            property real maxValue: 300
            //Math.min workaround to prevent that it its Infinity (otherwise the Selection item doesn't work anymore)
            property real value: Math.min((handle.x / line.width) * (maxValue - minValue) + minValue, maxValue)

            width: 200

            Rectangle {
                id: line

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                color: "black"

                height: 2
            }

            Rectangle {
                id: handle

                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: 5
                x: parent.width/2
                width: 12

                border.width: 2
                border.color: "black"

                color: "#e0e0e0"

                MouseArea {
                    anchors.fill: parent

                    property real xPos

                    onClicked: xPos = mouse.x

                    onPositionChanged: {
                        var delta = mouse.x - xPos - handle.width / 2
                        var newPos = handle.x + delta
                        if (newPos + handle.width / 2> 0 && newPos < line.width - handle.width / 2)
                            handle.x = newPos
                    }
                }
            }
        }
    }

    GridView {
        id: grid

        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        anchors.margins: 30

        cellWidth: slider.value
        cellHeight: slider.value

        keyNavigationWraps: true

        focus: true

        model: files

        delegate: FocusScope {
            width: grid.cellWidth - 4
            height: grid.cellHeight - 4


            Item {
                id: delegate
                anchors.fill: parent
                anchors.margins: 4

                function loadDocument() {
                    adapter.loadDocument(path + "/" + modelData)
                }

                focus: true

                Keys.onReturnPressed: delegate.loadDocument()
                Keys.onLeftPressed: grid.moveCurrentIndexLeft()
                Keys.onRightPressed: grid.moveCurrentIndexRight()
                Keys.onUpPressed: grid.moveCurrentIndexUp()
                Keys.onDownPressed: grid.moveCurrentIndexDown()

                Image {
                    id: image
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right

                    height: 0.8 * width

                    source: "image://qmlLiveDirectoryPreview/" + path + "/" + modelData

                    fillMode: Image.PreserveAspectFit
                    sourceSize.width: slider.maxValue
                    sourceSize.height: slider.maxValue

                    asynchronous: true
                    cache: false
                }

                Text {
                    id: label
                    anchors.top: image.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    text: modelData
                    color: root.textColor

                    textFormat: Text.PlainText
                    elide: Text.ElideMiddle
                    horizontalAlignment: Text.AlignHCenter
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: grid.currentIndex = index
                    onDoubleClicked: delegate.loadDocument()
                }
            }
        }

        highlight: FocusScope {
            id: highlight
            Rectangle {
                width: grid.cellWidth
                height: grid.cellHeight
                color: "#33B5E5"
                opacity: 0.5
            }
        }
    }

    Item {
        id: scrollBar

        width: 15; height: grid.height
        anchors.top: header.bottom
        anchors.right: parent.right
        anchors.rightMargin: 3
        anchors.bottom: parent.bottom

        // The properties that define the scrollbar's state.
        // position and pageSize are in the range 0.0 - 1.0.  They are relative to the
        // height of the page, i.e. a pageSize of 0.5 means that you can see 50%
        // of the height of the view.
        // orientation can be either Qt.Vertical or Qt.Horizontal
        property Flickable flickable: grid
        property real position: scrollBar.orientation == Qt.Vertical ? flickable.visibleArea.yPosition : flickable.visibleArea.xPosition
        property real pageSize: scrollBar.orientation == Qt.Vertical ? flickable.visibleArea.heightRatio : flickable.visibleArea.widthRatio
        property int orientation : Qt.Vertical
        property alias enableDrag: mouseArea.enabled

        visible: pageSize != 1

        // A light, semi-transparent background
        Rectangle {
            id: background
            anchors.fill: parent
            radius: scrollBar.orientation == Qt.Vertical ? (width/2 - 1) : (height/2 - 1)
            color: "white"
            opacity: 0.3
        }

        // Size the bar to the required size, depending upon the orientation.
        Rectangle {
            id: bar
            x: scrollBar.orientation == Qt.Vertical ? 1 : (scrollBar.position * (scrollBar.width-2) + 1)
            y: scrollBar.orientation == Qt.Vertical ? (scrollBar.position * (scrollBar.height-2) + 1) : 1
            width: scrollBar.orientation == Qt.Vertical ? (parent.width-2) : (scrollBar.pageSize * (scrollBar.width-2))
            height: scrollBar.orientation == Qt.Vertical ? (scrollBar.pageSize * (scrollBar.height-2)) : (parent.height-2)
            radius: scrollBar.orientation == Qt.Vertical ? (width/2 - 1) : (height/2 - 1)
            color: "black"
            opacity: 0.7
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            property variant pressedPos
            property bool drag: true
            onPressed: {
                pressedPos = mapToItem(bar, mouseX, mouseY)
                if (pressedPos.x >= 0 && pressedPos.x <= bar.width &&
                    pressedPos.y >= 0 && pressedPos.y <= bar.height)
                    drag = true
            }
            onReleased: drag = false

            onPositionChanged: {
                if (drag) {
                    if (scrollBar.orientation == Qt.Vertical) {
                        var newPos =((mouseY - pressedPos.y -1 ) / (scrollBar.height-2)) * scrollBar.flickable.contentHeight
                        if (newPos < 0)
                            scrollBar.flickable.contentY = 0
                        else if(newPos > (scrollBar.flickable.contentHeight - scrollBar.flickable.height))
                            scrollBar.flickable.contentY = (scrollBar.flickable.contentHeight - scrollBar.flickable.height)
                        else
                            scrollBar.flickable.contentY = newPos
                    } else {
                        var newPos =((mouseX - pressedPos.x -1 ) / (scrollBar.width-2)) * scrollBar.flickable.contentWidth
                        if (newPos < 0)
                            scrollBar.flickable.contentX = 0
                        else if(newPos > (scrollBar.flickable.contentWidth - scrollBar.flickable.width))
                            scrollBar.flickable.contentX = (scrollBar.flickable.contentWidth - scrollBar.flickable.width)
                        else
                            scrollBar.flickable.contentX = newPos
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        grid.forceActiveFocus();
    }
}
