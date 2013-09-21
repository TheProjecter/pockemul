/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtQml module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.1
import "../RatingIndicatorQML"

Item {
    id: delegate
    height: column.height + 40
    width: delegate.ListView.view.width

    // Create a property to contain the visibility of the details.
    // We can bind multiple element's opacity to this one property,
    // rather than having a "PropertyChanges" line for each element we
    // want to fade.
    property real detailsOpacity : 0

    property bool ismine: (username == cloud.getValueFor("username",""))

    // A simple rounded rectangle for the background
    Rectangle {
        id: background
        x: 2; y: 2; width: parent.width - x*2; height: parent.height - y*2
        color: "white"
        border.color: "orange"
        radius: 5
    }

    MouseArea {
        anchors.fill: parent
        onClicked: delegate.state = 'Details';
    }

    Column {
        id: column
        x: 20; y: 20
        width: parent.width - 40
//        height: background.height
        Row {
            width: parent.width;
            spacing: 5

            Image {
                source: "images/public.png"
                width: 30
                 height: 30
                visible: (ispublic==1)
            }

            Edit {
                id: titleText
                text: (title=="")?"No title":title
                readOnly: !ismine
                wrapMode: Text.WordWrap
                width: parent.width - 40 - closeButton.width - 5
                height: 30
                font { bold: true; family: "Helvetica"; pointSize: 16 }
            }
        }

        Text {
            id: userText
            text: username; width: parent.width; wrapMode: Text.WordWrap
            font { bold: false; family: "Helvetica"; pointSize: 14 }
        }
        Row {
            Image {
            id: delegateImage
            source: serverURL+"getPMLthumb/"+pmlid
            fillMode: Image.PreserveAspectFit;
            }
            Edit {
                text: objects
                readOnly: true
            }
        }

        Edit {
            id: descriptionText
            width: parent.width;
            height:background.height - y - 25
            text: (description=="")?"No description":description
            readOnly: !ismine
            wrapMode: Text.WordWrap;
            font.family: "Helvetica"; font.pointSize: 14
            opacity: delegate.detailsOpacity
        }
    }


    Item {
        id: details
        opacity: delegate.detailsOpacity




    }
    TextButton {
        y: 50
        anchors { right: background.right; rightMargin: 10 }
        text: "Download File"
        font.pointSize: 16
        onClicked: cloud.getPML(pmlid);
    }
    // A button to close the detailed view, i.e. set the state back to default ('').
    TextButton {
        id: closeButton
        y: 10
        anchors { right: background.right; rightMargin: 10 }
        opacity: delegate.detailsOpacity
        text: "Close"

        onClicked: delegate.state = '';
    }
    states: State {
        name: "Details"

        PropertyChanges { target: background; color: "white" }
        PropertyChanges { target: delegateImage; width: 200; height: 200;} // Make picture bigger
        PropertyChanges { target: delegate; detailsOpacity: 1; x: 0 } // Make details visible
        PropertyChanges { target: delegate; height: list.height } // Fill the entire list area with the detailed view

        // Move the list so that this item is at the top.
        PropertyChanges { target: delegate.ListView.view; explicit: true; contentY: delegate.y }

        // Disallow flicking while we're in detailed view
        PropertyChanges { target: delegate.ListView.view; interactive: false }
    }

    transitions: Transition {
        // Make the state changes smooth
        ParallelAnimation {
            ColorAnimation { property: "color"; duration: 500 }
            NumberAnimation { duration: 300; properties: "detailsOpacity,x,contentY,height,width" }
        }
    }
    //    MouseArea {
    //        anchors.fill: parent
    //        onDoubleClicked: cloud.getPML(pmlid)
    //    }

    Rectangle {
        width: parent.width; height: 1; color: "#cccccc"
        anchors.bottom: parent.bottom
    }
}
