import QtQuick 2.7
import QtQuick.Controls 2.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0

TreeView {
    id:treeView
    model: treeModel

//    TableViewColumn {
//        role: "iconFile"
//        title: "文件名"
//        delegate: iconTextDelegate
//        width: treeView.width * 0.3
//    }
    TableViewColumn {
        role: "severityStr"
        title: "错误级别"
        width: treeView.width * 0.4
    }
    TableViewColumn {
        role: "line"
        title: "行号"
        width: treeView.width * 0.1
    }
    TableViewColumn {
        role: "id"
        title: "错误号"
        width: treeView.width * 0.2
    }
    TableViewColumn {
        role: "summary";
        title: "概要";
        width: treeView.width * 0.3
    }

    Component {
        id:iconTextDelegate
        Item {
            Image {
                id:image
                source: styleData.value["icon"]
                height:parent.height * 0.9; width: height
                asynchronous:true
                cache:true
            }
            Text {
                text: styleData.value["file"]
                anchors.left: image.right
                anchors.leftMargin: 10
            }
        }
    }
}
