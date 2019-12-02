import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import CC 1.0


Page {
    id:createProjectRoot
    signal ok(var obj)
    signal cancel()
    Layout.fillWidth: true
    Layout.fillHeight: true
    property string pickerReason: ""
    property Component ffPicker: FFPicker {
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
                    model:["","native","win32Ansi","win32Unicode","win64","unix32","unix64"]
                }
            }
            RowLayout {
                Layout.leftMargin: 50
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
                      id:raidoButtonProj
                      text: "vs项目文件(*.sln,*.vsxproj)"
                      checked: true
                    }
                    RadioButton {
                      id:radioButtonFolder
                      text: "任意文件夹"
                    }
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
                    stackView.push(ffPicker)
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
                    stackView.push(ffPicker)
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
                    "platform":comboBoxPlatform.currentIndex
                }
                ok(obj)
                root.stackView.pop()
            }
            onRejected: {
                cancel()
                root.stackView.pop()
            }
    }



}
