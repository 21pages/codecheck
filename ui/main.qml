import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls 1.4 as C14
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
    objectName: "root"
    visible: true
    width: Global.screenWidth; height: Global.screenHeight
    onFocusObjectChanged: console.log(activeFocusItem)

    property Component filePicker : FilePicker{
        onFileSelected: {
            stackView.pop();
//            projectManager.open(fileName);
        }
    }

    property Component folderPicker : FolderPicker{
        onFolderSelected: {
            stackView.pop();
//            projectManager.open(fileName);
        }
    }

    property Component createProject: CreateProject {

    }

    ToolBar {
            x:0; y:0;
            width: parent.width;
            height: Global.toolBarHeight
            Material.foreground: "white"

            RowLayout {
                spacing: 20
                anchors.fill: parent

                ToolButton {
                    icon.source: stackView.depth > 1 ? "qrc:/icons/navigation/back.svg" : "qrc:/icons/navigation/menu.svg"
                    onClicked: {
                        if (stackView.depth > 1) {
                            stackView.pop()
                        } else {
//                            drawer.open()
                        }
                    }
                }

                Label {
                    id: titleLabel
                    text: "CodeCheck"
                    font.pixelSize: 20
                    elide: Label.ElideRight
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    Layout.fillWidth: true
                }

                ToolButton {
                    icon.source: "qrc:/icons/navigation/menu2.svg"
                    onClicked: optionsMenu.open()

                    Menu {
                        id: optionsMenu
                    }
                }
            }
        }


    StackView {
        id:stackView
        x:0; y:Global.toolBarHeight
        width: parent.width;
        height: parent.height - y
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
                        stackView.push(createProject);
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
            C14.SplitView {
                  y:100
                  height: parent.height - 100
                  width: parent.width
                  orientation: Qt.Vertical

                  Item {
                      Layout.minimumHeight: 300
                      width:parent.width
                      implicitHeight: 300
                      ResultList{
                          id:resultList
                          onListViewItemClicked: {
                              obj.codec = "gbk";
                              provider.onListViewClicked(obj);
                          }
                      }
                  }
                  Flickable {
                         id: flick
                         Layout.maximumHeight: 400
                         width: parent.width;
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
                             property string fileName: ""
                             width: flick.width
                             focus: true
                             wrapMode: TextEdit.NoWrap
                             onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
                             readOnly: true
                             selectionColor: Material.color(Material.BlueGrey)
                             selectedTextColor: Material.color(Material.Pink)
                         }
                         Component.onCompleted: {
                             provider.document = edit.textDocument;
                             provider.initDocument();
                         }
                     }

            }
        }
    }

    function textSelect(obj) {
        edit.clear();
        edit.append(obj["content"]);
        var start = obj["start"];
        var end = obj["end"];
        edit.select(start>10?start-10:start, end>10?end-10:end);
    }



}
