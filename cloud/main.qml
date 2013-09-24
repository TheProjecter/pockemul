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
    property bool   isPortrait: false
    property alias bigposter: bigposter
    property alias publicCloudTabName: publicCloudTab.name

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
                anchors.fill: parent
                publicCloud: false


            }

        }
        Tab {
            id: publicCloudTab
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
                        PmlView.populateCategoryModel();
                    }
                }
                TextButton {
                    id: saveCurrentSessionButton
                    text: "Save current session"
                    font.pointSize: 16
                    onClicked: {
                        var xml = cloud.save();
                        var url = cloud.getValueFor("serverURL","")+"savePML/"+ currentApiKey ;

//                        console.log("ok:"+url);
                        requestPost(url,xml, function(o) {
                            console.log(o.responseText);
                            tmpXmlListModel.xml = o.responseText;
                        });

//                        privateCloud.categoryModel.reload();
//                        publicCloud.categoryModel.reload();
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

                 }
                 privateCloud.populatePMLModel();
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
