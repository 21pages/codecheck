import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls 1.4 as C14
import QtQuick.Extras 1.4 as E14
import QtQuick.Window 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Material 2.12
import Qt.labs.settings 1.0
import QtQuick.Layouts 1.12
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
            materialUI.showLoading( "正在打开项目,请稍等...",loadingClickCallBack);
            projectManager.open(path);
        }
    }

    property Component createProject: CreateProject {
        onOk:{
            materialUI.showLoading( "正在分析源码,请稍等...");
            projectManager.create(obj)
        }
    }

    property Component statisticsPage: Statistics {

    }

    Component.onCompleted: {
        provider.document = edit.textDocument;
        var obj = {
            "typeShow":getTypeShow()
        };
        provider.initProviderFromUI(obj);
    }

    Connections {
        target:projectManager
        onOpenFinished:{
            materialUI.hideLoading()
        }
        onCreateFinished:{
            materialUI.hideLoading()
        }

        onProgress:{
            if(materialUI.loadingVisible) {
                var str = Math.round(value * 1.0  / 1024 * 100).toFixed(0) + "%\t" + description;
                console.log(str);
                materialUI.showLoading(str)
            }
        }

        onProjectInfoChanged:{
            var name = projectManager.projectInfo["name"];
            if(name === "") {
                titleBar.title = "CodeCheck"
            } else {
                titleBar.title = name
            }
            var dir = projectManager.projectInfo["dir"]
            textDir.text = dir
        }
    }

    Connections {
        target: provider
        onStatistics:{

        }
    }

    StackView {
        id:stackView
        anchors.fill:parent
        width: parent.width;
        height: parent.height - y
        initialItem: Page {
            header:MyToolBar {
                id:titleBar
                title:"CodeCheck"
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
                        text:"显示统计"
                        enabled: true/*titleBar.title != "CodeCheck"*/
                        onTriggered: {
                            stackView.push(statisticsPage)
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
                onAboutToHide: {
                    provider.typeShow = getTypeShow();
                }
                Column{
                    width: parent.width
                    ButtonGroup {
                        buttons: [radioButtonUTF8,radioButtonGBK]
                    }
                    Row {
                        Label {
                            text: "浏览编码:"
                            height:parent.height
                            verticalAlignment:Text.AlignVCenter
                        }
                        RadioButton {
                            id:radioButtonGBK
                            text:"GBK"
                            checked: true
                        }
                        RadioButton {
                            id:radioButtonUTF8
                            text: "UTF8"
                        }
                    }
                    ButtonGroup {
                        buttons: [checkBoxError,checkBoxWarning,checkBoxStyle,checkBoxPerformance,checkBoxPortability,checkBoxInformation]
                        exclusive:false
                    }
                    Row {
                        Label {
                            text: "问题显示:"
                            height:parent.height
                            verticalAlignment:Text.AlignVCenter
                        }
                        QcCheckBox {
                                id:checkBoxError
                                text: "错误"
                                checked: true
                                source:"qrc:/icons/alert/error.svg"
                                color: checkBoxError.checked?Global.severityColorMap["error"]:"#33000000"
                        }
                        QcCheckBox {
                                id:checkBoxWarning
                                text: "警告"
                                checked: true
                                source:"qrc:/icons/alert/warning.svg"
                                color: checkBoxWarning.checked?Global.severityColorMap["warning"]:"#33000000"
                        }
                        QcCheckBox {
                                id:checkBoxStyle
                                text: "风格"
                                checked: true
                                source:"qrc:/icons/alert/style.svg"
                                color: checkBoxStyle.checked?Global.severityColorMap["style"]:"#33000000"
                        }
                        QcCheckBox {
                                id:checkBoxPerformance
                                text: "性能"
                                checked: true
                                source:"qrc:/icons/alert/performance.svg"
                                color: checkBoxPerformance.checked?Global.severityColorMap["performance"]:"#33000000"
                        }
                        QcCheckBox {
                                id:checkBoxPortability
                                text: "移植"
                                checked: true
                                source:"qrc:/icons/alert/portability.svg"
                                color: checkBoxPortability.checked?Global.severityColorMap["portability"]:"#33000000"
                        }
                        QcCheckBox {
                                id:checkBoxInformation
                                text: "提示"
                                checked: true
                                source:"qrc:/icons/alert/information.svg"
                                color: checkBoxInformation.checked?Global.severityColorMap["information"]:"#33000000"
                        }
                    }
                }
            }

            QcSearchBar{
                anchors.top:splitViewResult.top
                anchors.topMargin:10
                anchors.horizontalCenter: splitViewResult.horizontalCenter
                width:splitViewResult.width * 0.9
                onSearch:{
                    provider.search = str;
                }
            }

            footer: ToolBar {
                id:toolBarFooter
                visible: true
                height: 30
                Row {
                    Text {
                        id:textDir
                        elide: Text.ElideLeft
                        font.pixelSize: 14
                        anchors.horizontalCenter: parent.horizontalCenter
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        color: "white"
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

    Settings{
        fileName: "app.ini"
        category:"drawer"
        property alias setting_radioButtonGBK_checked: radioButtonGBK.checked
        property alias setting_radioButtonUTF8_checked: radioButtonUTF8.checked
        property alias setting_checkBoxError_checked: checkBoxError.checked
        property alias setting_checkBoxWarning_checked:checkBoxWarning.checked
        property alias setting_checkBoxStyle_checked: checkBoxStyle.checked
        property alias setting_checkBoxPerformance_checked: checkBoxPerformance.checked
        property alias setting_checkBoxPortability_checked: checkBoxPortability.checked
        property alias setting_checkBoxInformation_checked: checkBoxInformation.checked
    }

    function textSelect(obj) {
        edit.text = obj["content"]
        var start = obj["start"];
        var end = obj["end"];
        edit.select(start, end);
    }

    function getTypeShow() {
        var typeShow = 0;
        if(checkBoxError.checked)typeShow+=1;
        if(checkBoxWarning.checked)typeShow+=2;
        if(checkBoxStyle.checked)typeShow+=4;
        if(checkBoxPerformance.checked)typeShow+=8;
        if(checkBoxPortability.checked)typeShow+=16;
        if(checkBoxInformation.checked)typeShow+=32;
        return typeShow;
    }

    function loadingClickCallBack() {
        materialUI.hideLoading()
    }
}
