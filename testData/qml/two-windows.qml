import QtQuick 2.0
import QtQuick.Window 2.1

Rectangle {
    width: 400
    height: 400
    color: "red"

    Text {
        anchors.centerIn: parent
	text: "Window 1"
    }

    Window {
	    width: 400
	    height: 400
	    visible: true
	Rectangle {
	    anchors.fill: parent
	    color: "blue"

	    Text {
		anchors.centerIn: parent
		text: "Window 2"
	    }
	}
    }
}
