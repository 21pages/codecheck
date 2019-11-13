import QtQuick 2.7
import QtQuick.Layouts 1.3

Item {
    property var d

    Layout.fillWidth: true
    Layout.fillHeight: true
    height:column.height

    Column {
        id:column
        Text {
            text: modelData.file + "\t行号:" +modelData.line
            height: 50
            padding: 10
        }
        Text {
            text: modelData.id + ":" + modelData.summary
            height: 50
            padding: 10
        }
    }
}

