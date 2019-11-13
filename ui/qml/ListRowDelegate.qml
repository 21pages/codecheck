import QtQuick 2.7
import QtQuick.Layouts 1.3

Rectangle {
    id:root
    property var d
    Layout.fillWidth: true
    Layout.fillHeight: true
    height:column.height + listView.height

    Rectangle {
        id:column
        height: 100
        width: parent.width
        border.width: 1;border.color: "green"
        ColumnLayout {
            Text {
                y:0
                text: modelData.file + "\t行号:" +modelData.line
                height: 50
                padding: 10
                color: "blue"
            }
            Text {
                y:50
                text: modelData.id + ":" + modelData.summary
                height: 50
                padding: 10
                color: "blue"
            }
        }

        MouseArea {
            z:1
            anchors.fill: parent
            onClicked: {
                console.log(d);
                if(modelData.array.length !== 0) {
                    listView.visible = !listView.visible;
                    listView.height = listView.visible ? 80 * modelData.array.length: 0;
                }
            }
        }
    }
    ListView {
        y:100
        id:listView
        model: modelData.array
        delegate:LineRowDelegate{}
        visible: false

    }
}

