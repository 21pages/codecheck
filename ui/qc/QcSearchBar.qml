import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

Control {
    id:control
    implicitHeight: 50
    implicitWidth: parent.width * 0.9
    TextField {
        id:field
        leftPadding: 10;rightPadding: btn.width
        width: parent.width - 20
        height: parent.height - 10
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        background: Rectangle{
            width: parent.width
            height: parent.height
            color: field.focus ? "transparent" : "#353637"
            border.color: field.focus ? "#21be2b" : "transparent"
            radius: height * 0.5
        }
    }
    ToolButton {
        id:btn
        width: 30;height: 30
        anchors.right: field.right
        anchors.rightMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        icon.source: "qrc:/icons/navigation/search.svg"
        icon.color:  hovered ? Material.color(Material.blue):Material.color(Material.Grey)
    }
}
