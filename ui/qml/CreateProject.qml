import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3


Item {
    id:createProjectRoot
    anchors.fill: parent

    GroupBox {
          title: ""
          height:50;
          width: parent.width
          RowLayout {
              ExclusiveGroup { id: tabPositionGroup }
              RadioButton {
                  text: "vs项目文件(*.dsw,*.dsp,*.sln,*.vsxproj)"
                  checked: true
                  exclusiveGroup: tabPositionGroup
              }
              RadioButton {
                  text: "任意文件夹"
                  exclusiveGroup: tabPositionGroup
              }
          }
      }

    TextInput {
        id:projectTextField
        width: parent.width - margins * 2;
        anchors.margins: 10;
        MouseArea {
            anchors.fill: parent;
            onDoubleClicked: {
                console.log("doubleClicked");
            }
        }
    }

}
