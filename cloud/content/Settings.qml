import QtQuick 1.1


Rectangle {
    width: 800; height: 480
    Column { /* outer column */
        spacing: 10


        LineInput {
            id: username
            objectName: "UserName"
            height: 30
            label: "User Name";
            font.pixelSize: 16
            onInputAccepted: cloud.saveValueFor(key,value)
        }

        LineInput {
            id: password
            objectName: "Password"
            height: 30
            label: "Password";
            font.pixelSize: 16
            onInputAccepted: cloud.saveValueFor(key,value)
        }
    }

}
