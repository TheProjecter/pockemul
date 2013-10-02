import QtQuick 1.0
import "TabbedQuickApp"
import "content"
import "."
//import QtSensors 5.0 as Sensors

Rectangle {
    width: 360
    height: 640

    id: root
    signal sendWarning(string test)

    property string serverURL: cloud.getValueFor("serverURL","http://rrouvin.dyndns.org/cloud/")
    property string currentUserid: "pock emul"
    property string currentApiKey: cloud.getValueFor("apikey","0")
    property bool   isPortrait: false
    property alias bigposter: bigposter
    property alias publicCloudTabName: publicCloudTab.name

    property color backGroundColor: "white"
    property color textColor : "black"
    property color inputBorderColor: "black"
    property color buttonBorderColor: "orange"
    property color textButtonColor: "black"


//    TiltSensor {
//        id: tilt
//        active: false
//    }

    onWidthChanged: {
        isPortrait = cloud.isPortraitOrientation();
        //        console.log(isPortrait)
    }

    VisualItemModel {
        id: tabsModel
        Tab {
            name: "Private Cloud"
            icon: "pics/private-cloud-white.png"

            color: "yellow"

            PmlView   {
                id: privateCloud
                anchors.top: privateSearchItem.bottom
                anchors.bottom: parent.bottom
                width: parent.width
                publicCloud: false
                searchText: privateSearchItem.text

            }
            LineInput {
                id: privateSearchItem
                width: parent.width
                height: 50
                label: "Search :"
                objectName: "searchFld"
                onInputAccepted: {
                    privateCloud.populate(text)

                }

            }
        }
        Tab {
            id: publicCloudTab
            name: "Public Cloud"
            icon: "pics/public-cloud-white.png"

            color: "green"


                anchors.fill: parent

                PmlView   {
                    id: publicCloud
                    anchors.top: searchItem.bottom
                    anchors.bottom: parent.bottom
                    width: parent.width
                    publicCloud: true
                    searchText: searchItem.text

                }


                LineInput {
                    id: searchItem
                    width: parent.width
                    height: 50
                    label: "Search :"
                    objectName: "searchFld"
                    onInputAccepted: {
                        publicCloud.populate(text)

                    }

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
                    expand: false
                    onClicked: {
                        PmlView.populateCategoryModel();
                    }
                }
                TextButton {
                    id: saveCurrentSessionButton
                    text: "Save current session"
                    expand: false
                    font.pointSize: 16
                    onClicked: {
                        var xml = cloud.save();
                        var url = cloud.getValueFor("serverURL","")+"savePML/"+ currentApiKey ;

//                        console.log("ok:"+url);
                        requestPost(url,xml, function(o) {
                            if (o.readyState == 4) {
                                if (o.status==200) {
                                    tmpXmlListModel.xml = o.responseText;
                                }
                            }
                        });
                    }
                }
                TextButton {
                    text: "upload Session File"
                    expand: false
                    font.pointSize: 16
                    onClicked: {
                        cloud.showFileDialog();
                    }
                }
                TextButton {
                    text: "analyse sensors"
                    expand: true
                    font.pointSize: 16
                    onClicked: {
                        var types = Sensors.sensorTypes();
                        text = types.join(", ");

                    }
                }
                TextButton {
                    text: "Create shortcut"
                    onClicked: cloud.addShortcut("-r PC-1475");
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
        id: tabbedui
        tabsHeight: 72
        tabIndex: 0
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

    XmlListModel {
         id: tmpXmlListModel
         query: "/item"

         XmlRole { name: "insert"; query: "insert/string()"; }
         XmlRole { name: "pmlid"; query: "pmlid/string()"; }
         XmlRole { name: "username"; query: "username/string()" }
         XmlRole { name: "objects"; query: "objects/string()" }
         XmlRole { name: "listobjects"; query: "listobjects/string()" }
         XmlRole { name: "ispublic"; query: "ispublic/number()" }
         XmlRole { name: "isdeleted"; query: "deleted/number()" }
         XmlRole { name: "title"; query: "title/string()" }
         XmlRole { name: "description"; query: "description/string()" }
         onStatusChanged: {

             if (status == XmlListModel.Ready) {

                 var ind = privateCloud.refpmlModel.count;
                 for (var i=0; i<count; i++) {
                     var item = get(i)
//                     console.log(item.listobjects)
                     if (item.insert == 1) {
//                         console.log("INSERT MODE");
                         privateCloud.refpmlModel.append({rowid : ind+i,
                                                             pmlid: item.pmlid,
                                                             username: item.username,
                                                             objects: item.objects,
                                                             listobjects: item.listobjects,
                                                             ispublic: item.ispublic,
                                                             isdeleted: item.isdeleted,
                                                             title: item.title,
                                                             description: item.description})
                     }
                     if (item.insert == 0) {
//                         console.log("UPDATE MODE");
                         // fetch refpmlModel and find pmlid
                         for (var j=0; j<privateCloud.refpmlModel.count; j++) {
                             var existingitem = privateCloud.refpmlModel.get(j);
                             if (existingitem.pmlid == item.pmlid) {
                                 // update record
                                 privateCloud.refpmlModel.set(j,
                                                              {objects: item.objects,listobjects: item.listobjects})

                             }
                         }


                     }
                     tabbedui.tabClicked(0);
                     privateCloud.populateCategoryModel();
                     privateCloud.objid = 0;
                     privateCloud.populatePMLModel();
                     privateCloud.focusPml(item.pmlid);
                 }

             }
         }
     }

    // this function is included locally, but you can also include separately via a header definition
    function requestGet(url, callback) {
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function() { callback(xhr);}

        xhr.open('GET', url, true);
        xhr.send('');
    }

    function requestPost(url, data, callback) {

        var xhr = new XMLHttpRequest();

        xhr.onreadystatechange = function() { callback(xhr);}

        xhr.open('POST', url);
        xhr.send(data);

    }

    function addRefPmlModel(_pmlid,
                            _username,
                            _objects,
                            _listobjects,
                            _ispublic,
                            _isdeleted,
                            _title,
                            _description) {
        console.log("count before:"+refpmlModel.count());
        refpmlModel.append({rowid: refpmlModel.count(),
                               pmlid: _pmlid,
                               username: _username,
                               objects: _objects,
                               listobjects: _listobjects,
                               ispublic: _ispublic,
                               isdeleted: _isdeleted,
                               title: _title,
                               description: _description});
        console.log("count after:"+refpmlModel.count());

    }
}
