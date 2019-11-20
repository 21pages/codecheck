import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4


Item {
    id:createProjectRoot
    Layout.fillWidth: true
    Layout.fillHeight: true
    property Component ffPicker: FFPicker {
        onOk: {
            if(reason === "btnSource") {
                textInput.text = path
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

    ColumnLayout {
        width: parent.width
        Layout.fillHeight: true
        ButtonGroup {
            buttons: buttons.children
        }
        ColumnLayout {
            id:buttons
          RadioButton {
              id:raidoButtonProj
              text: "vs项目文件(*.dsw,*.dsp,*.sln,*.vsxproj)"
              checked: true
          }
          RadioButton {
              id:radioButtonFolder
              text: "任意文件夹"
          }
        }
        RowLayout {
            width: parent.width
            height: 50
            TextField {
                id:textInput;
                Layout.fillWidth: true
            }
            Button {
                id:btnSource
                width: 100
                text: "源码路径"
                onClicked: {
                    ffPicker.reason = "btnSource"
                    stackView.push(ffPicker)
                }
            }
        }

    }


}
