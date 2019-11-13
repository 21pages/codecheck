import QtQuick 2.7
import QtQuick.Layouts 1.3


Rectangle {
    Layout.fillWidth: true
    Layout.fillHeight: true
//    height:column.height
//    y: -1 *column.height
    border.width: 1;border.color: "red"
//    x:0;y:180
    height: 80

    Column {
        id:column
        Text {
            text: modelData.file + "\t行号:" +modelData.line
            height: 40
            padding: 10
        }
        Text {
            text: modelData.column + ":" + modelData.info
            height: 40
            padding: 10
        }
    }
}
