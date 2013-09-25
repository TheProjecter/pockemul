import QtQuick 1.1

Rectangle {
    id: container

    property alias text: label.text
    property alias font: label.font

    property bool expand: true;

    signal clicked

    width: expand ? Math.max(label.width + 20 , parent.width) : label.width + 20 ;
    height: label.height + 6
    border.color: root.buttonBorderColor
    radius: 5
    //antialiasing: true
//    radius: 10

//    gradient: Gradient {
//        GradientStop { id: gradientStop; position: 0.0; color: "#666666" }
//        GradientStop { position: 1.0; color: "#000000" }
//    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: { container.clicked() }
    }

    Text {
        id: label
        color: root.textButtonColor
        anchors.centerIn: parent
    }

    states: State {
        name: "pressed"
        when: mouseArea.pressed
        PropertyChanges { target: container; color: "#333333" }
    }
}
