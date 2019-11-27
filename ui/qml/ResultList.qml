import QtQuick 2.12
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.12
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
//    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
//    highlightFollowsCurrentItem: false

    removeDisplaced: Transition {
        NumberAnimation { properties: "x,y"; duration: 100; easing.type: Easing.InOutQuad }
    }

    Component{
        id: sectionHeader
        Rectangle{
            width: listView.width
            height: childrenRect.height
            color: Material.color(Material.Grey)
            Label{
                width: parent.width
                text: section
                font.bold: true
                font.pointSize: 20
                elide: Text.ElideLeft
                color: "white"
            }
        }
    }
    section.property: "modelData.file"
    section.criteria: ViewSection.FullString
    section.delegate: sectionHeader
    section.labelPositioning:root.sectionIsInline? ViewSection.InlineLabels:ViewSection.CurrentLabelAtStart

}

