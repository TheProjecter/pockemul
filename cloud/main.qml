import QtQuick 1.0
import "TabbedQuickApp"
import "content"
import "."

Rectangle {
    width: 360
    height: 640

    id: root

    property string serverURL: "http://ds409/cloud/"
    property string currentUserid: "pock emul"
    property string currentApiKey: cloud.getValueFor("apikey","0")

    VisualItemModel {
        id: tabsModel
        Tab {
            name: "Own Cloud"
            icon: "pics/tab0.png"

            color: "yellow"

            PmlView   {
                id: privateCloud
                anchors.fill: parent
                publicCloud: false

            }

        }
        Tab {
            name: "Public Cloud"
            icon: "pics/tab1.png"

            color: "green"
            PmlView   {
                id: publicCloud
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

    // this function is included locally, but you can also include separately via a header definition
    function request(url, callback) {
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = (function(myxhr) {
//            console.log(xhr.readyState);
//            if (xhr.readyState == 4 )
            {
                return function() {
                    callback(myxhr);
                }
            }
        })(xhr);
        xhr.open('GET', url, true);
        xhr.send('');
    }
}
