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
    property alias sectionIsInline: raidoButtonSectionInline.checked

    property Component ffPickerOpenProject : FFPicker{
        onOk: {
            projectManager.open(path);
        }
    }

    property Component createProject: CreateProject {
        onOk:{
            materialUI.showLoading( "正在分析源码,请稍等,50% ");
            projectManager.create(obj)
        }
    }


    Connections {
        target:projectManager
        onOpenFinished:{

        }
        onCreateFinished:{
            materialUI.hideLoading()
        }
    }

    StackView {
        id:stackView
        anchors.fill:parent
        width: parent.width;
        height: parent.height - y
        initialItem: Page {
            header:MyToolBar {
                leftButton.icon.source:"qrc:/icons/navigation/menu.svg"
                rightButton.icon.source:"qrc:/icons/navigation/menu2.svg"
                onLeftClicked:{
                    drawer.open()
                }
                onRightClicked:{
                    optionsMenu.x = rightButton.x
                    optionsMenu.y = rightButton.y + rightButton.height
                    optionsMenu.open()
                }
                Menu {
                    id: optionsMenu
                    MenuItem {
                        text:"显示工具栏"
                        onTriggered: {
                            toolBarFooter.visible = true
                        }
                    }
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
                          obj.codec = radioButtonGBK.checked?"gbk":"utf8";
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
                             textFormat:TextEdit.PlainText
                             selectionColor: Material.color(Material.Yellow)
                             selectedTextColor: Qt.rgba(0,0,0,1)
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
                    cursorShape:entered?Qt.PointingHandCursor:Qt.ArrowCursor
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
                width: 200;height: 200
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
                        projectManager.close()
                        edit.text = ""
                    }
                }
                style:QcPieMenuStyle{}
            }

            Drawer {
                id:drawer
                width: 0.66 * root.width; height: root.height
                ColumnLayout{
                    width: parent.width
                    ButtonGroup {
                        buttons: [raidoButtonSectionInline,radioButtonSectionHead]
                    }
                    RowLayout {
                        Label {
                            text: "表头样式:"
                        }
                        RadioButton {
                            id:raidoButtonSectionInline
                            text: "inline"
                            checked: true
                        }
                        RadioButton {
                            id:radioButtonSectionHead
                            text:"head"
                        }
                    }
                    ButtonGroup {
                        buttons: [raidoButtonUTF8,radioButtonGBK]
                    }
                    RowLayout {
                        Label {
                            text: "浏览编码:"
                        }
                        RadioButton {
                            id:raidoButtonUTF8
                            text: "UTF8"
                            checked: true
                        }
                        RadioButton {
                            id:radioButtonGBK
                            text:"GBK"
                        }
                    }
                }
            }

            footer: ToolBar {
                id:toolBarFooter
                visible: false
                RowLayout {
                    ToolButton {
                        text: "统计"
                        id:toolButtonShowStatistics
                    }
                }
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
        edit.text = obj["content"]
        var start = obj["start"];
        var end = obj["end"];
        edit.select(start, end);
    }

}
