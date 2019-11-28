import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.12
import "qrc:/qc/"
import "qrc:/MaterialUI/Interface/"

Rectangle {
    id:subListDelegate
    Layout.fillWidth: true
    height: Global.subListHeight
    width: listView.width
    signal listViewItemClicked(var obj)

    MaterialCard {
        anchors.margins: 5
        height: Global.subListHeight - anchors.margins * 2
        width: parent.width - anchors.margins * 2
        elevation:2
        Text {
            text: "行号:" +modelData.line+"\t"+modelData.info
            height: Global.subListHeight
            padding: 10
        }
        Text {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            color: Material.color(Material.Purple)
            text: index + 1
        }
    }

    MouseArea {
        anchors.fill: parent
        enabled: true
        onClicked: {
            var obj = {"file":modelData.file,"line":modelData.line}
            subListDelegate.listViewItemClicked(obj);
        }
    }
}
