import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import "qrc:/js/utils.js" as Util

FocusScope {
    signal search(var str)
    id:control
    implicitHeight: 50
    implicitWidth: parent.width * 0.9
    onFocusChanged:{
        console.log(focus)
    }
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
            color: field.focus ? "#33333333":"#88eeeeee"
            border.color:"transparent"
            radius: height * 0.5
        }
        visible:false
        Keys.onEnterPressed:{
            func()
        }
        color:"blue"
    }
    ToolButton {
        id:btn
        width: 30;height: 30
        anchors.right: field.right
        anchors.rightMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        icon.source: "qrc:/icons/navigation/search.svg"
        icon.color:  hovered ? "pink":"grey"
        onClicked:{
            if(field.visible) {
                func()
            }
            field.visible = !field.visible
        }
    }

    function func() {
        var str = field.text;
        str = Util.trim(str);
        if(str.length > 0) {
            control.search(str);
        }
    }
}
