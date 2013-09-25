import QtQuick 1.1

Rectangle {
    id: container

    property alias text: label.text
    property alias font: label.font

    signal clicked

    width: label.width + 20;
    height: label.height + 6
    border.color: "orange"
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
//        color: "white"
        anchors.centerIn: parent
    }

    states: State {
        name: "pressed"
        when: mouseArea.pressed
        PropertyChanges { target: container; color: "#333333" }
    }
}
