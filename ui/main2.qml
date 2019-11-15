import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import CC 1.0 as CC
import "qrc:/qml/"
import QtGraphicalEffects 1.0



Window {
    id:root
    visible: true
//    minimumWidth: 600; minimumHeight: 800
    width: Global.screenWidth; height: Global.screenHeight

    CC.ProjectManager{
        id:projectManager;
    }
    Row {
        height: 100
        Button {
            width: 100;height: 100
            text: "open"
            onClicked:projectManager.open()
        }
        Button {
            width: 100;height: 100
            text: "create"
            onClicked: {
                createProjectDialog.open();
                projectManager.create();
            }
        }
        Button {
            width: 100;height: 100
            text: "remove"
            onClicked: {
//                provider.removeItem(provider.items.item(0)); //ok
                console.log(resultList.currentIndex);
                console.log(resultList.currentItem.d);
                provider.removeItem(resultList.currentItem.d);
            }
        }
        Button {
            width: 100;height: 100
            text: "text"
            onClicked: {
                textSelect(30,32);
            }
        }
    }

    Item {
        x:0;y:100
        width:parent.width
        height: 300
        ResultList{
            id:resultList
        }
    }
    Flickable {
           id: flick
           x:0;y:400
           width: 300; height: 200;
           contentWidth: edit.paintedWidth
           contentHeight: edit.paintedHeight
           clip: true

           function ensureVisible(r)
           {
               if (contentX >= r.x)
                   contentX = r.x;
               else if (contentX+width <= r.x+r.width)
                   contentX = r.x+r.width-width;
               if (contentY >= r.y)
                   contentY = r.y;
               else if (contentY+height <= r.y+r.height)
                   contentY = r.y+r.height-height;
           }

           TextEdit {
               id: edit
               width: flick.width
               focus: true
               wrapMode: TextEdit.Wrap
               onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
               readOnly: true
           }
           Component.onCompleted: {
               provider.document = edit.textDocument;
           }
       }

    function textSelect(obj) {
        var start = obj["start"];
        var end = obj["end"];
        edit.deselect();
        edit.select(start, end);
    }

    function onListViewClicked(obj) {
        obj.codec = "utf8";
        provider.onListViewClicked(obj);
    }

    CreateProjectDialog{
        id:createProjectDialog;
    }

    Component.onCompleted: {
        provider.sigSelectionPos.connect(textSelect);
    }
}
