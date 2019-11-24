import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls 1.4 as C14
import QtQuick.Extras 1.4 as E14
import QtQuick.Window 2.12
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
//import Qt.labs.platform 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Material 2.12
import CC 1.0 as CC
import "qrc:/qml/"
import "qrc:/qc/"
import "qrc:/MaterialUI/"
import "qrc:/MaterialUI/Interface/"



ApplicationWindow {
    id:root
    objectName: "root"
    visible: true
    width: Global.screenWidth; height: Global.screenHeight
    property alias stackView: stackView
    property alias materialUI:materialUI

    property Component ffPickerOpenProject : FFPicker{
        onOk: {
            projectManager.open(path);
        }
    }

    property Component createProject: CreateProject {
        onOk:{
            projectManager.create(obj)
        }
    }

    StackView {
        id:stackView
        anchors.fill:parent
        width: parent.width;
        height: parent.height - y
        initialItem: Page {
//            header:ToolBar {
//                    width: parent.width;
//                    height: Global.toolBarHeight
//                    Material.foreground: "white"

//                    RowLayout {
//                        spacing: 20
//                        anchors.fill: parent

//                        ToolButton {
//                            icon.source: stackView.depth > 1 ? "qrc:/icons/navigation/back.svg" : "qrc:/icons/navigation/menu.svg"
//                            onClicked: {
//                                if (stackView.depth > 1) {
//                                    stackView.pop()
//                                } else {
//        //                            drawer.open()
//                                }
//                            }
//                        }

//                        Label {
//                            id: titleLabel
//                            text: "CodeCheck"
//                            font.pixelSize: 20
//                            elide: Label.ElideRight
//                            horizontalAlignment: Qt.AlignHCenter
//                            verticalAlignment: Qt.AlignVCenter
//                            Layout.fillWidth: true
//                        }

//                        ToolButton {
//                            icon.source: "qrc:/icons/navigation/menu2.svg"
//                            onClicked: optionsMenu.open()

//                            Menu {
//                                id: optionsMenu
//                            }
//                        }
//                    }
//                }
            header:MyToolBar {
                leftButton.icon.source:"qrc:/icons/navigation/menu.svg"
                rightButton.icon.source:"qrc:/icons/navigation/menu2.svg"
                onLeftClicked:{
                   // drawer.open()
                }
                onRightClicked:{
                    optionsMenu.open()
                }
                Menu {
                    id: optionsMenu
                }
            }


            C14.SplitView {
                  id:splitViewResult
                  y:100
                  anchors.fill:parent
                  orientation: Qt.Vertical
                  ResultList{
                      id:resultList
                      width:parent.width
                      implicitHeight: 300
                      Layout.minimumHeight: 100
                      onListViewItemClicked: {
                          obj.codec = "gbk";
                          provider.onListViewClicked(obj);
                      }
                  }
                  Flickable {
                         id: flick
                         implicitHeight: 300
                         Layout.minimumHeight: 100
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


            DropArea {
              id:dropArea
              anchors.fill:parent
              onEntered:{
                  drag.accepted = true
              }
            }

            RoundButton{
                id:buttonFloating
                anchors.bottom:parent.bottom
                anchors.bottomMargin:pieMenu.height / 2
                anchors.right:parent.right
                anchors.rightMargin:pieMenu.width / 2
                icon.source:"qrc:/icons/action/edit.svg"
                Drag.active: dragArea.drag.active
                states:State{
                    name:"noAnchored"
                    AnchorChanges{
                        target:buttonFloating
                        anchors.bottom:undefined
                        anchors.right:undefined
                    }
                    PropertyChanges {
                        target:buttonFloating
                        anchors.bottomMargin:undefined
                        anchors.rightMargin:undefined
                    }
                }
                MouseArea{
                    id: dragArea
                    anchors.fill: parent
                    drag.target: parent
                    drag.minimumX:pieMenu.width/2
                    drag.maximumX:dropArea.width - width - pieMenu.width/2
                    drag.minimumY:pieMenu.height/2
                    drag.maximumY:splitViewResult.height - buttonFloating.height
                    cursorShape:entered?Qt.DragMoveCursor:Qt.ArrowCursor
                    onPressed:{
                        if(buttonFloating.state !== "noAnchored") {
                            buttonFloating.state = "noAnchored"
                        }
                    }
                    onClicked: {
                        pieMenu.popup(buttonFloating.x + buttonFloating.width /2, buttonFloating.y + buttonFloating.height / 2)
                    }
                }

            }

            E14.PieMenu {
                id:pieMenu
                C14.MenuItem {
                    text: "打开"
                    onTriggered: {
                        stackView.push(ffPickerOpenProject);
                    }
                }
                C14.MenuItem {
                    text: "新建"
                    onTriggered: {
                        stackView.push(createProject);
                    }
                }
                C14.MenuItem {
                    text: "关闭"
                    onTriggered: {
                    }
                }
                style:QcPieMenuStyle{}
            }


        }
    }

    MaterialUI {
        id:materialUI
        z: 2
        anchors.fill: parent
        dialogCancelText: "取消"
        dialogOKText: "确定"
    }

    function textSelect(obj) {
        edit.clear();
        edit.append(obj["content"]);
        var start = obj["start"];
        var end = obj["end"];
        edit.select(start>10?start-10:start, end>10?end-10:end);
    }



}
