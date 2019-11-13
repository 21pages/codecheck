import QtQuick 2.12
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import CC 1.0
import "qrc:/qml/"

ListView {
    id: listView
    anchors.fill: parent
//    anchors.topMargin: buttons.implicitHeight + 10
    anchors.bottomMargin: 10
    boundsBehavior: Flickable.DragOverBounds
    clip: true
    cacheBuffer: 2

    // model of "itemsIndex" provides indices only.
    // also it dispatches all necessary signals
    // that allow view to react on indices changes
    model: provider.items

    delegate: ListRowDelegate {
        d: provider.items.item(index)
        anchors.left: parent.left
        anchors.right: parent.right
    }

    removeDisplaced: Transition {
        NumberAnimation { properties: "x,y"; duration: 100; easing.type: Easing.InOutQuad }
    }

}

