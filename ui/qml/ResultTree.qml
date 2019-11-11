import QtQuick 2.7
import QtQuick.Controls 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import "qrc:/Material/" as Material

TreeView {
    anchors.fill: parent
    model: treeModel

    TableViewColumn {
        role: "iconFile"
        title: "文件名"
        delegate: iconTextDelegate
    }
    TableViewColumn {
        role: "severityStr"
        title: "错误级别"
    }
    TableViewColumn {
        role: "line"
        title: "行号"
    }
    TableViewColumn {
        role: "id"
        title: "错误号"
    }
    TableViewColumn {
        role: "summary"
        title: "概要"
    }

    Component {
        id:iconTextDelegate
        Item {
            Image {
                id:image
                source: styleData.value["icon"]
                height:parent.height * 0.9; width: height
            }
            Text {
                text: styleData.value["file"]
                anchors.left: image.right
                anchors.leftMargin: 10
                Material.Ink {
                    anchors.fill: parent
                }
            }
        }
    }
}
