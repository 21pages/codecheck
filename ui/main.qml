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
import FileFolder 1.0
import "qrc:/qml/"
import "qrc:/qc/"
import "qrc:/MaterialUI/"
import "qrc:/MaterialUI/Interface/"
import "qrc:/filefolder/"
import "qrc:/js/utils.js" as Utils



ApplicationWindow {
    id:root
    objectName: "root"
    visible: true
    width: Global.screenWidth; height: Global.screenHeight
    property alias stackView: stackView
    property alias materialUI:materialUI
    property Component fileFolderOpenProject: FileFolder {
        onOk: {
            materialUI.showLoading( "正在打开项目,请稍等...",loadingClickCallBack);
            if(!Utils.isEndWith(path,".codecheck")) {
                materialUI.hideLoading();
                materialUI.showSnackbarMessage(path+"不是项目文件");
                return;
            }
            setting_main.setValue("fileFolderOpenProject_startDir",Utils.getDir(path))
            setting_main.setValue("fileFolderOpenProject_startName",Utils.getFile(path))
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
                materialUI.showLoading(str)
            }
        }

        onProjectInfoChanged:{
            var name = projectManager.projectInfo["name"];
            var dir = projectManager.projectInfo["dir"];
            if(dir.charAt(dir.length - 1) !== "/") {
                dir += "/";
            }
            textDir.text = dir + name + ".codecheck"
        }
    }

    Connections {
        target: provider
        onStatistics:{
            Global.$statistics = obj
        }
        onPrint_finished:{
            materialUI.hideLoading();
            materialUI.showSnackbarMessage("PDF报告生成到" + projectManager.projectInfo["dir"]);
        }
    }

    StackView {
        id:stackView
        anchors.fill:parent
        width: parent.width;
        height: parent.height - y
        initialItem: Page {
            header:ToolBar {
                    id:titleBar
                    width: parent.width;
                    height: Global.toolBarHeight
                    Material.foreground: "white"
                    QcButton{
                        id:leftButton
                        icon.source:"qrc:/icons/navigation/menu.svg"
                        anchors.left: parent.left
                        onClicked: {
                            drawer.open()
                        }
                    }
                    Label {
                        id:titleLable
                        text:"代码质量检测软件"
                        anchors.left: leftButton.right
                        height: parent.height
                        font.pixelSize: 20
                        elide: Label.ElideRight
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                    }
                    QcSearchBar{
                        height: parent.height
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left:titleLable.right
                        anchors.leftMargin: 50
                        anchors.right: rightButton.left
                        onSearch:{
                            provider.search = str;
                        }
                    }
                    QcButton{
                            id:rightButton
                            anchors.right: parent.right
                            icon.source:"qrc:/icons/navigation/menu2.svg"
                            onClicked:{
                                optionsMenu.x = rightButton.x
                                optionsMenu.y = rightButton.y + rightButton.height
                                optionsMenu.open()
                            }
                    }
                    Menu {
                        id: optionsMenu
                        MenuItem {
                            text:"显示统计(全部)"
                            enabled: textDir.text !== ""
                            onTriggered: {
                                stackView.push(statisticsPage)
                                provider.getStatistic(0)
                            }
                        }
                        MenuItem {
                            text:"显示统计(过滤)"
                            enabled: textDir.text !== ""
                            onTriggered: {
                                stackView.push(statisticsPage)
                                provider.getStatistic(1)
                            }
                        }
                        MenuItem {
                            text: "生成报告(全部)"
                            enabled: textDir.text !== ""
                            onTriggered:  {
                                optionsMenu.close()
                                provider.getStatistic(0)
                                var component = Qt.createComponent("qrc:/qml/Statistics.qml");
                                if(component.status === Component.Ready) {
                                    stackView.push(component)
                                }
                                provider.print(0)
                                materialUI.showLoading("正在生成PDF报告...")
                            }
                        }
                        MenuItem {
                            text: "生成报告(过滤)"
                            enabled: textDir.text !== ""
                            onTriggered:  {
                                optionsMenu.close()
                                provider.getStatistic(1)
                                var component = Qt.createComponent("qrc:/qml/Statistics.qml");
                                if(component.status === Component.Ready) {
                                    stackView.push(component)
                                }
                                provider.print(1)
                                materialUI.showLoading("正在生成PDF报告...")
                            }
                        }
                    }
            }


            C14.SplitView {
                  id:splitViewResult
                  y:100
                  anchors.fill:parent
                  orientation: Qt.Vertical
                  handleDelegate:Rectangle {
                    color:Material.foreground/*Material.color(Material.Green)*/
                    width: parent.width
                    height: 2
                  }
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
//                             else if (contentY+height <= r.y+r.height)
//                                 contentY = r.y+r.height-height;
                             else if (contentY+height/2 <= r.y+r.height)
                                 contentY = r.y + r.height - height /2
                         }

                         TextEdit {
                             id: edit
                             property string file: ""
                             width: flick.width
                             focus: true
                             wrapMode: TextEdit.NoWrap
                             onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
                             readOnly: true
                             textFormat:TextEdit.PlainText
                             selectionColor: Material.color(Material.Yellow)
                             selectedTextColor: Qt.rgba(0,0,0,1)
                             MouseArea {
                                anchors.fill: parent
                                onDoubleClicked: {
                                    projectManager.openFile(textFieldOpenFileExe.text,edit.file)
                                }
                             }
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
                        clearUI();
                        var startDir = setting_main.value("fileFolderOpenProject_startDir","C:/");
                        var startName = setting_main.value("fileFolderOpenProject_startName","");
                        fileFolderData.setObj({
                                              "nameFilter":["*.codecheck"],
                                               "startDir":startDir,
                                               "startName":startName,
                                                "type":FileFolder.FileFolderType_File
                                              });
                        stackView.push(fileFolderOpenProject);
                    }
                }
                C14.MenuItem {
                    text: "新建"
                    onTriggered: {
                        clearUI();
                        stackView.push(createProject);
                    }
                }
                C14.MenuItem {
                    text: "关闭"
                    onTriggered: {
                        clearUI();
                        projectManager.close()
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
                                color: checkBoxError.checked?Global.severityColorMap["error"]:"#88888888"
                        }
                        QcCheckBox {
                                id:checkBoxWarning
                                text: "警告"
                                checked: true
                                source:"qrc:/icons/alert/warning.svg"
                                color: checkBoxWarning.checked?Global.severityColorMap["warning"]:"#88888888"
                        }
                        QcCheckBox {
                                id:checkBoxStyle
                                text: "风格"
                                checked: true
                                source:"qrc:/icons/alert/style.svg"
                                color: checkBoxStyle.checked?Global.severityColorMap["style"]:"#88888888"
                        }
                        QcCheckBox {
                                id:checkBoxPerformance
                                text: "性能"
                                checked: true
                                source:"qrc:/icons/alert/performance.svg"
                                color: checkBoxPerformance.checked?Global.severityColorMap["performance"]:"#88888888"
                        }
                        QcCheckBox {
                                id:checkBoxPortability
                                text: "平台"
                                checked: true
                                source:"qrc:/icons/alert/portability.svg"
                                color: checkBoxPortability.checked?Global.severityColorMap["portability"]:"#88888888"
                        }
                        QcCheckBox {
                                id:checkBoxInformation
                                text: "提示"
                                checked: true
                                source:"qrc:/icons/alert/information.svg"
                                color: checkBoxInformation.checked?Global.severityColorMap["information"]:"#88888888"
                        }
                    }
                    RowLayout {
                        width: parent.width
                        Label {
                            text: "打开文本所用程序:"
                        }
                        TextField {
                            id:textFieldOpenFileExe
                            Layout.fillWidth: true
                            text: "C:/Windows/System32/notepad.exe"
                        }
                    }
                }
            }

            footer: ToolBar {
                id:toolBarFooter
                visible: true
                height: 30
                Row {
                    width: parent.width;height: parent.height
                    Text {
                        id:textDir
                        height: parent.height
                        elide: Text.ElideLeft
                        font.pixelSize: 14
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        color: "white"
                    }
                    Text {
                        text: (provider.search !== "") ?("  搜索:\"" + provider.search + "\""):""
                        height: parent.height
                        elide: Text.ElideRight
                        font.pixelSize: 14
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
        fileName: provider.exeDir + "/app.ini"
        category:"drawer"
        property alias setting_radioButtonGBK_checked: radioButtonGBK.checked
        property alias setting_radioButtonUTF8_checked: radioButtonUTF8.checked
        property alias setting_checkBoxError_checked: checkBoxError.checked
        property alias setting_checkBoxWarning_checked:checkBoxWarning.checked
        property alias setting_checkBoxStyle_checked: checkBoxStyle.checked
        property alias setting_checkBoxPerformance_checked: checkBoxPerformance.checked
        property alias setting_checkBoxPortability_checked: checkBoxPortability.checked
        property alias setting_checkBoxInformation_checked: checkBoxInformation.checked
        property alias setting_textFieldOpenFileExe_text: textFieldOpenFileExe.text
    }

    Settings {
        id:setting_main
        fileName: provider.exeDir + "/app.ini"
        category:"main"
    }

    function textSelect(obj) {
        edit.text = obj["content"]
        var start = obj["start"];
        var end = obj["end"];
        edit.select(start, end);
        edit.file = obj["file"];
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

    function clearUI()
    {
        edit.clear();
        edit.file = ""
        textDir.text = ""
        provider.search = "";
    }
}
