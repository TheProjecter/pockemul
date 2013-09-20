import QtQuick 1.1


Rectangle {

    property alias objectName: input.objectName
    property alias text: input.text
    property alias label: label.text
    property alias maximumLength: input.maximumLength


    property alias font: fontstyle.font

    id: lineinput
    signal inputAccepted(string key,string value)



    width: 480; height: 100

    Text {
        id: fontstyle
        text: ""
        visible: false
        font.pixelSize: 16
    }
    Row {
        y: 10
        spacing: 10
        Text {
            id: label
            text: "Label"
            width: 100
            height: 30
            font: fontstyle.font
        }

        FocusScope {
            width: 200
            height: 30
            BorderImage {
                source: "images/lineedit-bg-focus.png"
                anchors.fill: parent
            }
            TextInput {
                width: parent.width - 20
                height: parent.height - 10
                anchors.centerIn: parent
                id: input
                objectName: "inputField"
                text: cloud.getValueFor(input.objectName, "")
                font: fontstyle.font

                onAccepted: {
                    lineinput.inputAccepted(input.objectName, input.text)
                    //Here is where we save the value or string
//                    cloud.saveValueFor(input.objectName, input.text)

                }
            }
        }
    }
}
