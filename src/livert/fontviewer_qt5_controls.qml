import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.0

Rectangle {
    id: root
    width: livert.screenWidth
    height: livert.screenHeight

    property alias testText: textInput.text
    property alias pointSize: pointSizeSpinBox.value
    property var defaultWeights:[
        { name: "Light", weight: Font.Light },
        { name: "Regular", weight: Font.Normal },
        { name: "DemiBold", weight: Font.DemiBold },
        { name: "Bold", weight: Font.Bold },
        { name: "Black", weight: Font.Black },
    ]

    property alias showAllWeights: showAllWeightsCheckBox.checked

    Rectangle {
        id: header

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: 32

        z: 2

        color: "#E0E0E0"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8

            Label {
                width: 120
                height: 20

                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: "Font Size:"
            }

            SpinBox {
                id: pointSizeSpinBox
                suffix: "pt"
                stepSize: 2
                value: 12
                minimumValue: 0
                maximumValue: 50

                Layout.preferredWidth: 60
            }

            Label {
                width: 150
                height: 20

                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: "Example Text: "
            }

            TextField {
                id: textInput

                height: 20
                Layout.preferredWidth: 600

                readOnly: false

                text: "The quick brown fox jumps over the lazy dog"

                focus: true
            }

            Label {
                width: 250
                height: 20

                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: "Show all QML font styles: "
            }

            CheckBox {
                id: showAllWeightsCheckBox
                height: 20
                width: 20
            }

            Item {
                width: 16; height: 16
                Layout.fillWidth: true
            }

        }
    }

    ScrollView {
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

    ListView {
        id: list

        model: styles

        delegate: ListView {

            id: familyDelegate

            property var style: styles[index]

            interactive: false

            width: parent.width
            height: childrenRect.height;

            model: !showAllWeights ? style.weights : defaultWeights

            delegate: Column {

                width: root.width
                height: childrenRect.height + 20

                anchors.margins: 2

                spacing: 2

                property string name: showAllWeights ? modelData.name : familyDelegate.style.weights[index].name
                property int weight: showAllWeights ? modelData.weight : familyDelegate.style.weights[index].weight

                Text {
                    text: familyDelegate.style.family + ", " + name
                }

                Rectangle {
                    color: "black"
                    height: 1
                    width: root.width
                }

                Text {
                    id: exampleText
                    font.pointSize: root.pointSize
                    font.family: familyDelegate.style.family
                    font.weight: weight
                    text: "abcdefghijklmnopqrstuvwxyz"
                }

                Text {
                    font.pointSize: root.pointSize
                    font.family: familyDelegate.style.family
                    font.weight: weight
                    text: "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                }

                Text {
                    font.pointSize: root.pointSize
                    font.family: familyDelegate.style.family
                    font.weight: weight
                    text: "0123456789.:;(*!?'/\\\")%&-+@~#<>{}[]"
                }

                Rectangle {
                    color: "black"
                    height: 1
                    width: root.width
                }

                Text {
                    font.pointSize: 6
                    font.family: familyDelegate.style.family
                    font.weight: weight
                    text: "(" + font.pointSize + "pt)" + testText
                }

                Text {
                    font.pointSize: 8
                    font.family: familyDelegate.style.family
                    font.weight: weight
                    text: "(" + font.pointSize + "pt)" + testText
                }

                Text {
                    font.pointSize: 10
                    font.family: familyDelegate.style.family
                    font.weight: weight
                    text: "(" + font.pointSize + "pt)" + testText
                }

                Text {
                    font.pointSize: 18
                    font.family: familyDelegate.style.family
                    font.weight: weight
                    text: "(" + font.pointSize + "pt)" + testText
                }

                Text {
                    font.pointSize: 24
                    font.family: familyDelegate.style.family
                    font.weight: weight
                    text: "(" + font.pointSize + "pt)" + testText
                }

                Text {
                    font.pointSize: 30
                    font.family: familyDelegate.style.family
                    font.weight: weight
                    text: "(" + font.pointSize + "pt)" + testText
                }

                Text {
                    font.pointSize: 36
                    font.family: familyDelegate.style.family
                    font.weight: weight
                    text: "(" + font.pointSize + "pt)" + testText
                }

                Text {
                    font.pointSize: 42
                    font.family: familyDelegate.style.family
                    font.weight: weight
                    text: "(" + font.pointSize + "pt)" + testText
                }
            }
        }
    }

    }

//    Item {
//        id: scrollBar

//        width: 15; height: list.height
//        anchors.top: header.bottom
//        anchors.right: parent.right
//        anchors.rightMargin: 3
//        anchors.bottom: parent.bottom

//        // The properties that define the scrollbar's state.
//        // position and pageSize are in the range 0.0 - 1.0.  They are relative to the
//        // height of the page, i.e. a pageSize of 0.5 means that you can see 50%
//        // of the height of the view.
//        // orientation can be either Qt.Vertical or Qt.Horizontal
//        property Flickable flickable: list
//        property real position: scrollBar.orientation == Qt.Vertical ? flickable.visibleArea.yPosition : flickable.visibleArea.xPosition
//        property real pageSize: scrollBar.orientation == Qt.Vertical ? flickable.visibleArea.heightRatio : flickable.visibleArea.widthRatio
//        property int orientation : Qt.Vertical
//        property alias enableDrag: mouseArea.enabled

//        visible: pageSize != 1

//        // A light, semi-transparent background
//        Rectangle {
//            id: background
//            anchors.fill: parent
//            radius: scrollBar.orientation == Qt.Vertical ? (width/2 - 1) : (height/2 - 1)
//            color: "white"
//            opacity: 0.3
//        }

//        // Size the bar to the required size, depending upon the orientation.
//        Rectangle {
//            id: bar
//            x: scrollBar.orientation == Qt.Vertical ? 1 : (scrollBar.position * (scrollBar.width-2) + 1)
//            y: scrollBar.orientation == Qt.Vertical ? (scrollBar.position * (scrollBar.height-2) + 1) : 1
//            width: scrollBar.orientation == Qt.Vertical ? (parent.width-2) : (scrollBar.pageSize * (scrollBar.width-2))
//            height: scrollBar.orientation == Qt.Vertical ? (scrollBar.pageSize * (scrollBar.height-2)) : (parent.height-2)
//            radius: scrollBar.orientation == Qt.Vertical ? (width/2 - 1) : (height/2 - 1)
//            color: "black"
//            opacity: 0.7
//        }

//        MouseArea {
//            id: mouseArea
//            anchors.fill: parent

//            property variant pressedPos
//            property bool drag: true
//            onPressed: {
//                pressedPos = mapToItem(bar, mouseX, mouseY)
//                if (pressedPos.x >= 0 && pressedPos.x <= bar.width &&
//                    pressedPos.y >= 0 && pressedPos.y <= bar.height)
//                    drag = true
//            }
//            onReleased: drag = false

//            onPositionChanged: {
//                if (drag) {
//                    if (scrollBar.orientation == Qt.Vertical) {
//                        var newPos =((mouseY - pressedPos.y -1 ) / (scrollBar.height-2)) * scrollBar.flickable.contentHeight
//                        if (newPos < 0)
//                            scrollBar.flickable.contentY = 0
//                        else if(newPos > (scrollBar.flickable.contentHeight - scrollBar.flickable.height))
//                            scrollBar.flickable.contentY = (scrollBar.flickable.contentHeight - scrollBar.flickable.height)
//                        else
//                            scrollBar.flickable.contentY = newPos
//                    } else {
//                        var newPos =((mouseX - pressedPos.x -1 ) / (scrollBar.width-2)) * scrollBar.flickable.contentWidth
//                        if (newPos < 0)
//                            scrollBar.flickable.contentX = 0
//                        else if(newPos > (scrollBar.flickable.contentWidth - scrollBar.flickable.width))
//                            scrollBar.flickable.contentX = (scrollBar.flickable.contentWidth - scrollBar.flickable.width)
//                        else
//                            scrollBar.flickable.contentX = newPos
//                    }
//                }
//            }
//        }
//    }
}
