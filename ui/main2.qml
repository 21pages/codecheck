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
    width: 600; height: 800

    CC.OpenProjectManager{
        id:openProject;
    }
    Row {
        height: 100
        Button {
            width: 100;height: 100
            text: "open"
            onClicked:openProject.open()
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
//               baseUrl: Qt.resolvedUrl("qrc:/qml/ResultTree.qml")
               id: edit
               width: flick.width
               focus: true
               wrapMode: TextEdit.Wrap
               onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
           }
           Component.onCompleted: {
               provider.document = edit.textDocument;
           }
       }



}
