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


import "content"
Rectangle {

    id: pmlview

    width: 800; height: 480

    property string currentObjid: "0"
    property bool loading: xmlpmlModel.status == XmlListModel.Loading
    property bool publicCloud: true

    property alias categoryListView: categories
    property alias pmlListView: list
    property alias refpmlModel: refpmlModel


    property int objid: 0
    property int ispublic: publicCloud ? 1 : 0

    onObjidChanged: {populatePMLModel()}

    ListModel {
        id: categoryModel
    }
    SortListModel {
        id: tmpcategoryModel
        sortColumnName: "name"
    }

    XmlListModel {
        id: xmlpmlModel
        source: serverURL + "listPML/" + (publicCloud?"0":currentApiKey)+"/0"
        query: "/listPML/item"

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
//                    console.log("xmlpmlModel onStatusChanged: START");
                    refpmlModel.clear();
                    for (var i=0; i<count; i++) {
                        var item = get(i)
//                        console.log(item.objects)
                        refpmlModel.append({rowid : i,
                                            pmlid: item.pmlid,
                                           username: item.username,
                                            objects: item.objects,
                                            listobjects: item.listobjects,
                                            ispublic: item.ispublic,
                                            isdeleted: item.isdeleted,
                                            title: item.title,
                                            description: item.description})
                    }

                    populatePMLModel();
                    populateCategoryModel();
//                    console.log("xmlpmlModel onStatusChanged: END");
                }

            }
    }

    function insertorupdatecategoryModel(object_id,object_name) {
//        console.log("insertorupdatecategoryModel:"+object_id+"-"+object_name);
        for (var i=0; i<tmpcategoryModel.count;i++) {
            var item = tmpcategoryModel.get(i);
            if (item.objid == object_id) {
                // found, increment
                item.counter++;
                return;
            }
        }
        // not found, create a record
        tmpcategoryModel.append({objid: object_id,
                                 name: object_name,
                                 counter: 1});
    }

    function populateCategoryModel() {
//        console.log("populateCategoryModel");
        tmpcategoryModel.clear();
        var isdeletedCount = 0;
        for (var i=0; i<refpmlModel.count; i++) {
            var item = refpmlModel.get(i)
//            console.log("Read: "+item.pmlid+"-"+item.title);
            if (pmlview.publicCloud && (item.ispublic == 0)) continue;

            if (item.isdeleted) { isdeletedCount++; continue; }

            // fetch all item's objects
//            console.log("XML:"+item.objects);
            var x=item.objects


            var tableau=x.split(';');
            for (var j=0; j<tableau.length; j++) {
//                console.log("j="+j)
                var obj = tableau[j].split('|');
                insertorupdatecategoryModel(obj[0],obj[1]);
            }
//            console.log("populateCategoryModel : END");
        }

        // sort tmpcategoryModel
        console.log("***"+tmpcategoryModel.count);
        tmpcategoryModel.quick_sort();
        console.log("***"+tmpcategoryModel.count);
        categoryModel.clear();
        categoryModel.append({objid: 0,name: "All", counter: (refpmlModel.count-isdeletedCount)});
        // copy tmpcategoryModel to categoryModel with SORT
        for (var i=0; i<tmpcategoryModel.count;i++){
            categoryModel.append(tmpcategoryModel.get(i));
        }
        categoryModel.append({objid: -1,name: "Recycle Bin", counter: (isdeletedCount)});
        tmpcategoryModel.clear();
    }



    function populatePMLModel() {
//        console.log("REFRESH Model");
        pmlModel.clear();
        for (var i=0; i<refpmlModel.count; i++) {
            var item = refpmlModel.get(i)
//            console.log("Read: "+item.pmlid+"-"+item.title);
            if (pmlview.publicCloud && (item.ispublic == 0)) continue;
//            console.log("public OK");
            if ( (pmlview.objid >= 0) && (item.isdeleted == 1)) continue;

            if ( (pmlview.objid > 0) && !idInArray(pmlview.objid.toString(),item.listobjects)) continue;
//            console.log("object OK");
            if ( (pmlview.objid == -1) && (item.isdeleted != 1 )) continue;
//            console.log("Deleted OK");
            pmlModel.append({   rowid : i,
                                pmlid: item.pmlid,
                               username: item.username,
                                objects: item.objects,
                                ispublic: item.ispublic,
                                isdeleted: item.isdeleted,
                                title: item.title,
                                description: item.description})
//            console.log("Store: "+item.title);
        }

    }

    function idInArray(id, list) {
//        console.log("Search:"+id+" in:"+list+"  contains:"+String(list).indexOf(","+id+","));
        return (String(list).indexOf(","+id+",")>=0);
    }


    // Reference List Model. Contains all record
    // it might be valuable to dump it on disk when the application close
    // and load the cache at startup
    ListModel { id: refpmlModel; }

    // Filtered ListModel
    ListModel { id: pmlModel; }

    Row {
        Rectangle {
            id: categoriesView
            width: 220; height: pmlview.height
            color: "#efefef"


            ListView {
                id: categories
                focus: true
                anchors.fill: parent
                model: categoryModel //xmlcategoryModel
                header: refreshButtonDelegate
                delegate: CategoryDelegate {}
                highlight: Rectangle { color: "steelblue" }
                highlightMoveSpeed: 9999999
            }
            ScrollBar {
                scrollArea: categories; height: categories.height; width: 8
                anchors.right: categories.right
            }
        }
        ListView {
            id: list
            width: pmlview.width - categoriesView.width; height: pmlview.height
            model: pmlModel
            delegate: NewsDelegate {}
        }
    }
    Component {
        id: refreshButtonDelegate
        Item {
            width: categories.width; height: 60
            Text {
                id: expandcollapsebutton
                text: "<"
                font { family: "Helvetica"; pixelSize: 16; bold: true }
                anchors {
                    left: parent.left; leftMargin: 15
                    verticalCenter: parent.verticalCenter
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {

                   categoriesView.width = (expandcollapsebutton.text=="<")? 50 : 220;
                    expandcollapsebutton.text = (expandcollapsebutton.text=="<")? ">" : "<";
                 //   populateCategoryModel();//xmlcategoryModel.reload();
                }
            }
        }
    }
    ScrollBar { scrollArea: list; height: list.height; width: 8; anchors.right: pmlview.right }
    Rectangle { x: categoriesView.width; height: pmlview.height; width: 1; color: "#cccccc" }


    ListModel { id: pmlThumbModel; }

    function focusPml(pmlid) {
        for (var i=0; i<pmlModel.count;i++) {
            if (pmlModel.get(i).pmlid == pmlid) {
//                console.log("***found***");
                list.currentIndex = i;
            }
        }
    }

    function updThumbId(refpmlid) {
        // search pmlid and increment
//        console.log("update counter for :"+refpmlid);
        for (var i=0; i< pmlThumbModel.count;i++){
            var item = pmlThumbModel.get(i);
            if (item.pmlid == refpmlid) {
//                console.log("updThumbId - Found:"+item.counter);
                pmlThumbModel.setProperty(i,"counter",item.counter+1);
//                console.log("Found, updated:"+item.counter);
                return 0;
            }
        }
        pmlThumbModel.append({pmlid: refpmlid,counter:1});
//        console.log("updThumbId - NOT Found, created:" +pmlThumbModel.count);
    }
    function getThumbId(refpmlid) {
//        console.log("getThumbId:"+refpmlid+" count:"+pmlThumbModel.count);
        for (var i=0; i< pmlThumbModel.count;i++){
            var item = pmlThumbModel.get(i);
//            console.log("getThumbId - Fetch:"+item.pmlid);
            if (item.pmlid == refpmlid) {
//                console.log("getThumbId - Found:"+item.counter);
                return item.counter;
            }
        }
        return 0;
    }

    function refresh() {
        xmlpmlModel.reload();
    }
}
