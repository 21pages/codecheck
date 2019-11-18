import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4


Rectangle {
    id:subListDelegate
    Layout.fillWidth: true
    height: Global.subListHeight
    width: listView.width
    signal listViewItemClicked(var obj)

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
    MouseArea {
        anchors.fill: parent
        enabled: true
        onClicked: {
            var obj = {"file":modelData.file,"line":modelData.line}
//            root.onListViewClicked(obj);
            subListDelegate.listViewItemClicked(obj);
        }
    }
}
