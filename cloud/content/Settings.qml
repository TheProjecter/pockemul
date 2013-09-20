import QtQuick 1.1


Rectangle {
    width: 800; height: 480

    VisualItemModel {
        id: visualSettingsModel
        SettingsDelegate { id: username; name: "username"; labelString: "User Name"; type: "input"; }
        SettingsDelegate { id: password; name: "password"; labelString: "Password"; type: "input"; }
        SettingsDelegate { name: "apikey"; labelString: "Get your APIKey"; type: "action";
            onButtonClicked: {
                buttonElementEnabled = false;

                var key = Qt.btoa(cloud.generateKey(username.inputText,password.inputText));
//                console.log(username.inputText+'/'+password.inputText+':key:'+key);
                var url = serverURL+'login?username='+encodeURIComponent(username.inputText)+'&key='+encodeURIComponent(key);
//                console.log('url:'+url);
                request(url, function (o) {

                    // log the json response
//                    console.log(o.responseText);
                    buttonElementEnabled = true;
                    cloud.saveValueFor(name,o.responseText);
                    currentApiKey = o.responseText;
                    apikey.inputText = o.responseText;
                    privateCloud.categoryModel.reload();
                });
            }
        }
        SettingsDelegate { id: apikey; name: "out"; labelString: "Retrieved Key"; type: "input"; saveInput:false; }
    }

    ListView {
        focus: true
        id: categories
        anchors.fill: parent
        model: visualSettingsModel
        //            footer: quitButtonDelegate
        //        highlight: Rectangle { color: "steelblue" }
        highlightMoveSpeed: 9999999
    }
    ScrollBar {
        scrollArea: categories; height: categories.height; width: 8
        anchors.right: categories.right
    }
    //        LineInput {
    //            id: username
    //            objectName: "UserName"
    //            height: 30
    //            label: "User Name";
    //            font.pixelSize: 16
    //            onInputAccepted: cloud.saveValueFor(key,value)
    //        }

    //        LineInput {
    //            id: password
    //            objectName: "Password"
    //            height: 30
    //            label: "Password";
    //            font.pointSize: 16
    //            onInputAccepted: cloud.saveValueFor(key,value)
    //        }
}


