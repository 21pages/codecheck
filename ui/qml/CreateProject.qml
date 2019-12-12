import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import Qt.labs.settings 1.0
import CC 1.0
import FileFolder 1.0
import "qrc:/filefolder/"


Page {
    id:createProjectRoot
    signal ok(var obj)
    signal cancel()
    Layout.fillWidth: true
    Layout.fillHeight: true
    property string pickerReason: ""

    property Component fileFolder: FileFolder {
        onOk: {
            if(pickerReason === "source") {
                textFieldSource.text = path
            } else if(pickerReason == "destination") {
                textFieldDestination.text = path
            }
        }
    }

    function currentProjectType()
    {
        if(raidoButtonProj.checked) {
            return "vs";
        } else {
            return "dir";
        }
    }

    header: MyToolBar {
        title:"新建项目"
        leftButton.icon.source:"qrc:/icons/navigation/back.svg"
        rightButton.enabled:false
        onLeftClicked:{
            cancel()
            root.stackView.pop()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Label {
                text: "项目名称"
            }
            TextField {
                id:textFieldProjectName
                Layout.fillWidth: true
                height: 50
                placeholderText: "请输入项目名称"
            }
        }

        RowLayout {
            height: 80
            RowLayout {
                Label {
                    text: "程序平台"
                }
                ComboBox {
                    id:comboBoxPlatform
                    model:["通用","win32","win64","unix32","unix64"]
                }
            }
            RowLayout {
                Label {
                    text: "语言"
                }
                ComboBox {
                    id:comboBoxLang
                    model:["通用","C","Cpp"]
                }
            }

            RowLayout {
                Label {
                    text:"C标准"
                }
                ComboBox {
                    id:comboBoxC
                    model:["C89","C99","C11"]
                }
            }
            RowLayout {
                Label {
                    text:"C++标准"
                }
                ComboBox {
                    id:comboBoxCpp
                    model:["C++03","C++11","C++14","C++17","C++20"]
                }
            }
        }

        RowLayout {
            height:80
            Label {
                text: "源文件类型:"
                width: 100
                height: buttons.height
            }
            ButtonGroup {
                buttons: [raidoButtonProj,radioButtonFolder]
            }
            Row{
                 id:buttons
                 RadioButton {
                   id:radioButtonFolder
                   text: "任意文件夹"
                   checked: true
                 }
                RadioButton {
                  id:raidoButtonProj
                  text: "vs项目文件(*.sln,*.vcxproj)"
                }

            }


        }

        Row {
            width: parent.width
            height: 50
            TextField {
                id:textFieldSource;
                width: parent.width - btnSource.width
            }
            Button {
                id:btnSource
                width: 100
                text: "源码路径"
                onClicked: {
                    pickerReason = "source"
                    var isFolder = radioButtonFolder.checked;
                    var type =  isFolder ?FileFolder.FileFolderType_Dir:FileFolder.FileFolderType_File
                    var nameFilters = isFolder ? ["*.*"]:["*.sln","*.vcxproj"]
                    fileFolderData.setObj({
                        "nameFilters":nameFilters,
                        "startDir":textFieldSource.text,
                        "startName":"",
                        "type":type
                    });
                    stackView.push(fileFolder)
                }
            }
        }

        Row {
            width: parent.width
            height: 50
            TextField {
                id:textFieldDestination;
                width: parent.width - buttonDestination.width
            }
            Button {
                id:buttonDestination
                width: 100
                text: "输出目录"
                onClicked: {
                    pickerReason = "destination"
                    fileFolderData.obj = {
                        "nameFilters":["*.*"],
                        "startDir":textFieldDestination.text,
                        "startName":"",
                        "type":FileFolder.FileFolderType_Dir
                    }
                    stackView.push(fileFolder)
                }
            }
        }

    }
    footer: DialogButtonBox{
            standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
            onAccepted:{
                var obj = {
                    "name": textFieldProjectName.text,
                    "type":raidoButtonProj.checked ? Project.ProjTypeVS : Project.ProjTypeDIR,
                    "source":textFieldSource.text,
                    "destination":textFieldDestination.text,
                    "platform":comboBoxPlatform.currentIndex,
                    "lang":comboBoxLang.currentIndex,
                    "c":comboBoxC.currentText,
                    "cpp":comboBoxCpp.currentText
                }
                ok(obj)
                root.stackView.pop()
            }
            onRejected: {
                cancel()
                root.stackView.pop()
            }
    }

    Settings {
        fileName: provider.exeDir + "/app.ini"
        category:"createProject"
        property alias setting_textFieldProjectName_text: textFieldProjectName.text
        property alias setting_comboBoxPlatform_currentIndex: comboBoxPlatform.currentIndex
        property alias setting_radioButtonFolder_checked: radioButtonFolder.checked
        property alias setting_radioButtonProj_checked: raidoButtonProj.checked
        property alias setting_textFieldSource_text: textFieldSource.text
        property alias setting_textFieldDestination_text: textFieldDestination.text
        property alias setting_comboBoxLang_currentIndex: comboBoxLang.currentIndex
        property alias setting_comboBoxC_currentIndex: comboBoxC.currentIndex
        property alias setting_comboBoxCpp_currentIndex: comboBoxCpp.currentIndex
    }

}
