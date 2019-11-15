import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.5
import QtQuick.Controls 1.4


Dialog {
    id:createProjectDialog
    width: 400;height: 300;
    anchors.centerIn: parent
    z:1

    TextInput {
        id:projectTextField
        width: parent.width - margins * 2;
        anchors.margins: 10;
//        placeholderText: "工程路径/工程目录";
        MouseArea {
            anchors.fill: parent;
            onDoubleClicked: {
                console.log("doubleClicked");
            }
        }
    }

    FileDialog {
        id:projectFileDialog
    }

}
