import QtQuick 1.0
import "TabbedQuickApp"
import "content"
import "."

Rectangle {
    width: 360
    height: 640

    id: root

    VisualItemModel {
        id: tabsModel
        Tab {
            name: "Own Cloud"
            icon: "pics/tab0.png"

            color: "yellow"

            Rssnews   {
                anchors.fill: parent
                publicCloud: false
                currentUserid: "pock emul"
            }

        }
        Tab {
            name: "Public Cloud"
            icon: "pics/tab1.png"

            color: "green"
            Rssnews   {
                anchors.fill: parent
                publicCloud: true
            }
        }
        Tab {
            name: "Settings"
            icon: "pics/tab2.png"

            color: "red"

            Settings {
                anchors.fill: parent
            }
        }
    }

    TabbedUI {
        tabsHeight: 72
        tabIndex: 1
        tabsModel: tabsModel
    }

    Image {
        source: "pics/close.png"
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10

        MouseArea {
            anchors.fill: parent
            onClicked: Qt.quit()
        }
    }
}
