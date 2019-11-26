import QtQuick 2.12
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import CC 1.0
import "qrc:/qml/"

ListView {
    id: listView
    anchors.bottomMargin: 10
    clip: true
    cacheBuffer: 9999
    signal listViewItemClicked(var obj)

    model: provider.items

    delegate: ListRowDelegate {
        d: provider.items.item(index)
        anchors.left: parent.left
        anchors.right: parent.right
        onListViewItemClicked: {
            listView.listViewItemClicked(obj);
        }
    }

    removeDisplaced: Transition {
        NumberAnimation { properties: "x,y"; duration: 100; easing.type: Easing.InOutQuad }
    }

    Component{
        id: sectionHeader
        Rectangle{
            width: 100
            height: 50
            color: "lightsteelblue"
            Text{
                text: section
                font.bold: true
                font.pointSize: 20
            }
        }
    }
//    section.property: modelData.file
    section.criteria: ViewSection.FullString
    section.delegate: sectionHeader

}

