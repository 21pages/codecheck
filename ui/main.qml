import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
//import QtQuick.Dialogs 1.3
import Qt.labs.platform 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Material 2.0
import CC 1.0 as CC
import "qrc:/qml/"



Window {
    id:root
    visible: true
    width: Global.screenWidth; height: Global.screenHeight

    property Component filePicker : FilePicker{
        onFileSelected: {
            stackView.pop();
            projectManager.open(fileName);
        }
    }

    StackView {
        id:stackView
        anchors.fill: parent
        initialItem: Item {
            Row {
                height: 100
                Button {
                    width: 100;height: 100
                    text: "open"
                    onClicked: {
                        stackView.push(filePicker);
                    }
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

            CreateProjectDialog{
                id:createProjectDialog;
            }

            Component.onCompleted: {
                provider.sigSelectionPos.connect(textSelect);
            }

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


}
