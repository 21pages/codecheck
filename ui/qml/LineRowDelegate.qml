import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4


MouseArea {
//    Layout.fillWidth: true
//    Layout.fillHeight: true
//    border.width: 1;border.color: "red"
    height: Global.subListHeight
    width: parent.width
    onClicked: {
        console.log("lineRowDelegate");
        provider.initHighlighter(modelData.file);
        mouse.accepted = false;
    }



    Column {
        id:column
        width: parent.width;height: parent.width

        Text {
            text: modelData.file + "\t行号:" +modelData.line
            height: Global.subListHeight * 0.5
            padding: 10
        }
        Text {
            text: modelData.column + ":" + modelData.info
            height: Global.subListHeight * 0.5
            padding: 10
        }

    }

//    MouseArea {
//        Layout.fillHeight:true;
//        Layout.fillWidth: true;
//        enabled: true
//        propagateComposedEvents: true
//        onClicked: {
//            console.log("lineRowDelegate");
//            provider.initHighlighter(modelData.file);
//            mouse.accepted = false;
//        }
//    }

//    Button {
//        text:"btn";
//        onClicked: console.log("btn");
//    }
}
