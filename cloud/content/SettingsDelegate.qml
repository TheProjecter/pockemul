
import QtQuick 1.1

Item {
    id: delegate

    property string name
    property string labelString
    property string type
    property bool saveInput: true
    property alias buttonElementEnabled: buttonElement.enabled
    property alias inputText: inputElement.text
    property alias defaultText: inputElement.defaultText

    signal buttonClicked

    width: delegate.ListView.view.width; height: 60

    LineInput {
        id: inputElement
        objectName: name
        width: parent.width
//        height: 40
        label: labelString
        font.pointSize: 16
        onInputAccepted: if (saveInput) cloud.saveValueFor(key,value)
        visible: (type == "input")
    }
    ComboBox {

        visible: (type=="combo")
    }

    Row {
        y: 10
        spacing: 10
        Text {
            id: labelElement
            text: labelString
            color: delegate.ListView.isCurrentItem ? "white" : "black"
            font { family: "Helvetica"; pointSize: 16; bold: false }
//            anchors {
//                left: parent.left; leftMargin: 15
//                verticalCenter: parent.verticalCenter
//            }
            anchors.verticalCenter: parent.verticalCenter
            visible: ((type == "text") | (type == "action") )
        }
        TextButton {
            id: buttonElement
            text: labelString
            objectName: name
            font.pointSize: 16
            visible: (type=="action")
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                buttonClicked();
            }
        }
    }
    BusyIndicator {
        scale: 0.6
        on: delegate.ListView.isCurrentItem && window.loading
        anchors { right: parent.right; rightMargin: 10; verticalCenter: parent.verticalCenter }
    }

    Rectangle {
        width: delegate.width; height: 1; color: "#cccccc"
        anchors.bottom: delegate.bottom
        visible: delegate.ListView.isCurrentItem ? false : true
    }
    Rectangle {
        width: delegate.width; height: 1; color: "white"
        visible: delegate.ListView.isCurrentItem ? false : true
    }

    //    MouseArea {
    //        anchors.fill: delegate
    //        onClicked: {
    //            delegate.ListView.view.currentIndex = index
    //            window.currentObjid = objid
    //        }
    //    }


}
