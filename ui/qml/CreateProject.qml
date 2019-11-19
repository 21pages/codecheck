import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4


Item {
    id:createProjectRoot
    Layout.fillWidth: true
    Layout.fillHeight: true

    function currentProjectType()
    {
        if(raidoButtonProj.checked) {
            return "vs";
        } else {
            return "dir";
        }
    }

    Column {
        width: parent.width
        Layout.fillHeight: true
        GroupBox {
            id:gourpBox
            title: ""
            height:50;
            Layout.fillWidth: true
            RowLayout {
              ExclusiveGroup { id: tabPositionGroup }
              RadioButton {
                  id:raidoButtonProj
                  text: "vs项目文件(*.dsw,*.dsp,*.sln,*.vsxproj)"
                  checked: true
                  exclusiveGroup: tabPositionGroup
              }
              RadioButton {
                  id:radioButtonFolder
                  text: "任意文件夹"
                  exclusiveGroup: tabPositionGroup
              }
            }
          }
        RowLayout {
            width: parent.width
            height: 50
            TextField {
                id:textInput;
                Layout.fillWidth: true
                style: TextFieldStyle{
                    background: Rectangle{
                        border.color: Material.primary
                    }
                }
            }
            Button {
                id:btn
                width: 100
                text: "选择路径"
                onClicked: {
                    if(currentProjectType() === "vs") {
//                        stackView.push(root.filePicker)
                        dialog.open()
                    } else {
                        stackView.push(root.folderPicker)
                    }
                }
            }
        }

    }

    FilePickerDialog {
        id:dialog
    }


}
