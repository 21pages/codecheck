import QtQuick 2.12
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import CC 1.0
import "qrc:/qml/"

ListView {
    id: listView
    anchors.fill: parent
    anchors.bottomMargin: 10
    boundsBehavior: Flickable.DragOverBounds
    clip: true
    cacheBuffer: 2
    highlightFollowsCurrentItem:true

    model: provider.items

    delegate: ListRowDelegate {
        d: provider.items.item(index)
        anchors.left: parent.left
        anchors.right: parent.right
    }

    removeDisplaced: Transition {
        NumberAnimation { properties: "x,y"; duration: 100; easing.type: Easing.InOutQuad }
    }

//    MouseArea {
//        anchors.fill: parent
//        propagateComposedEvents: true
//        onClicked: {
//            listView.currentIndex = listView.indexAt(mouse.x + listView.contentX,mouse.y + listView.contentY);
//            console.log("resultListCurrentIndex:",listView.currentIndex);
//            mouse.accepted = false;
//        }
//    }

}

