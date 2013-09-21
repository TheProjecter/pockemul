import QtQuick 1.0
import "TabbedQuickApp"
import "content"
import "."

Rectangle {
    width: 360
    height: 640

    id: root
    signal sendWarning(string test)

    property string serverURL: cloud.getValueFor("serverURL","http://rrouvin.dyndns.org/cloud/")
    property string currentUserid: "pock emul"
    property string currentApiKey: cloud.getValueFor("apikey","0")

    property alias bigposter: bigposter


    VisualItemModel {
        id: tabsModel
        Tab {
            name: "Private Cloud"
            icon: "pics/private-cloud-white.png"

            color: "yellow"

            PmlView   {
                id: privateCloud
                anchors.fill: parent
                publicCloud: false

            }

        }
        Tab {
            name: "Public Cloud"
            icon: "pics/public-cloud-white.png"

            color: "green"
            PmlView   {
                id: publicCloud
                anchors.fill: parent
                publicCloud: true
            }
        }
        Tab {
            name: "Action"
            icon: "pics/action-white.png"
            Column {
                spacing: 20
                anchors.fill: parent


                TextButton {
                    text: "Refresh all"
                    font.pointSize: 16
                    onClicked: {
                        privateCloud.categoryModel.reload();
                        publicCloud.categoryModel.reload();
                    }
                }
                TextButton {
                    text: "Save current session"
                    font.pointSize: 16
                    onClicked: {
                        cloud.save();
                    }
                }
                TextButton {
                    text: "upload Session File"
                    font.pointSize: 16
                    onClicked: {
                        cloud.showFileDialog();
                    }
                }
            }
        }
        Tab {
            name: "Settings"
            icon: "pics/settings-white.png"

            color: "red"

            Settings {
                anchors.fill: parent
            }
        }
        Tab {
            name: ""
            icon: "pics/back-white.png"
            MouseArea {
                anchors.fill: parent
                onClicked: Qt.quit()
            }

        }
    }

    TabbedUI {
        tabsHeight: 72
        tabIndex: 1
        tabsModel: tabsModel
        quitIndex: 4
    }

    Image {
        id: bigposter
        visible: false
        anchors.fill: parent
        MouseArea {
            anchors.fill: parent
            onClicked: visible=false;
        }
    }

    // this function is included locally, but you can also include separately via a header definition
    function requestGet(url, callback) {
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

    function requestPost(url, data, callback) {

        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = (function(myxhr) {
                        console.log(xhr.readyState);
//            if (xhr.readyState == 4 )
            {
                return function() {
                    callback(myxhr);
                }
            }
        })(xhr);
        xhr.open('POST', url);
        xhr.send(data);

        }


}
