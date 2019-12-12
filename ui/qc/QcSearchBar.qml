import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import "qrc:/js/utils.js" as Util

FocusScope {
    signal search(var str)
    id:control

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
            color: field.focus ? "#88ffffff":"#88aaaaaa"
            border.color:"transparent"
            radius: height * 0.5
        }
        visible:false
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignBottom
        color:field.focus ?"#FF5722":"#FF9800"
        cursorVisible: field.visible
        Keys.onReturnPressed: {
            func()
            field.visible = false
        }
    }


    ToolButton {
        id:btn
        width: 30;height: 30
        anchors.right: field.right
        anchors.rightMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        icon.source: "qrc:/icons/navigation/search.svg"
        icon.color:  hovered ? "#FF5722":"white"
        onClicked:{
                field.visible = !field.visible
        }

    }

    function func() {
        var str = field.text;
        str = Util.trim(str);
        control.search(str);
    }
}
