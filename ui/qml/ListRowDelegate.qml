import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

Rectangle {
    id:listViewDelegate
    property var d
    Layout.fillWidth: true
    Layout.fillHeight: true
    height:Global.listHeight + listView2.height
    signal listViewItemClicked(var obj)

    Rectangle {
        id:column
        height: Global.listHeight
        width: parent.width
        border.width: 1;border.color: Global.severityColorMap[modelData.severity] !== undefined ? Global.severityColorMap[modelData.severity]:"#fefefe";
        RowLayout {
            height: parent.height
            Rectangle {
                id:rect
                width: 50;height: parent.height
                visible: modelData.array.length !== 0
            }
            ColumnLayout {
                Text {
                    y:0
                    text: modelData.file + "\t行号:" +modelData.line
                    height: Global.listHeight * 0.5
                    padding: 10
                }
                Text {
                    y:50
                    text: modelData.id + ":" + modelData.summary
                    height: Global.listHeight * 0.5
                    padding: 10
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if(modelData.array.length !== 0) {
                    var visible =  listView2.visible;
                    listView2.visible = !visible;
                    listView2.height = (!visible) ? Global.subListHeight * modelData.array.length: 0;
                }
                var obj = {"file":modelData.file,"line":modelData.line}
                listViewItemClicked(obj);
            }
        }
    }


    ListView {
        y:Global.listHeight
        id:listView2
        model: modelData.array
        delegate:LineRowDelegate{
            onListViewItemClicked:{
                listViewDelegate.listViewItemClicked(obj);
            }
        }
        visible: false
    }
}

