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

    header: ToolBar {
        height: Global.toolBarHeight
        Label{
            anchors.centerIn: parent
            text: "新建项目"
            font.pixelSize: 22
            color:"#ffffff"
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
                    text: "源文件类型:"
                    width: 100
                    height: buttons.height
                }
                ButtonGroup {
                    buttons: buttons.children
                }
                ColumnLayout {
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

            RowLayout {
                Label {
                    text: "程序平台"
                }
                ComboBox {
                    id:comboBoxPlatform
                    model:["","native","win32Ansi","win32Unicode","win64","unix32","unix64"]
                }
            }

        }

        RowLayout {
            width: parent.width
            height: 50
            TextField {
                id:textFieldSource;
                Layout.fillWidth: true
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

        RowLayout {
            width: parent.width
            height: 50
            TextField {
                id:textFieldDestination;
                Layout.fillWidth: true
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
//                /*obj.name*/obj["name"] = textFieldProjectName.text
//                /*obj.type*/obj["type"] = raidoButtonProj.checked ? Project.ProjTypeVS : Project.ProjTypeDIR
//                /*obj.source*/obj["source"] = textFieldSource.text
//                /*obj.destination*/obj["destination"] = textFieldDestination.text
                console.log(obj)
                ok(obj)
                root.stackView.pop()
            }
            onRejected: {
                cancel()
                root.stackView.pop()
            }
    }



}
