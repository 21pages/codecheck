import QtQuick 2.7
import QtQuick.Layouts 1.3

Rectangle {
    id:root
    property var d
    Layout.fillWidth: true
    Layout.fillHeight: true
    height:Global.listHeight + listView2.height

    Rectangle {
        id:column
        height: Global.listHeight
        width: parent.width
        border.width: 1;border.color: "green"
        RowLayout {
            height: parent.height
            Rectangle {
                id:rect
                width: 50;height: parent.height
                visible: modelData.array.length !== 0
                color: "blue"
            }
            ColumnLayout {
                Text {
                    y:0
                    text: modelData.file + "\t行号:" +modelData.line
                    height: Global.listHeight * 0.5
                    padding: 10
                    color: "blue"
                }
                Text {
                    y:50
                    text: modelData.id + ":" + modelData.summary
                    height: Global.listHeight * 0.5
                    padding: 10
                    color: "blue"
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            onClicked: {
                console.log(d);
                if(modelData.array.length !== 0) {
                    listView2.visible = !listView2.visible;
                    listView2.height = listView2.visible ? Global.subListHeight * modelData.array.length: 0;
                }
                provider.initHighlighter(modelData.file);
                mouse.accepted = false;
            }
        }
    }
    ListView {
        y:Global.listHeight
        id:listView2
        model: modelData.array
        delegate:LineRowDelegate{}
        visible: false
        highlightFollowsCurrentItem:true

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            enabled: true
            onClicked: {
                console.log("contentX:",listView2.contentX,"contentY:",listView2.contentY);
                console.log("x:",mouse.x,"y:",mouse.y);
                listView2.currentIndex = listView2.indexAt(mouse.x + listView2.contentX,mouse.y + listView2.contentY);
                console.log("ListRowDelegateCurrentIndex:",listView2.currentIndex);
                mouse.accepted = false;
            }
        }
    }
}

