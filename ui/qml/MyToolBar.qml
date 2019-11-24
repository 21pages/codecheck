import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import "qrc:/qc/"
import "qrc:/MaterialUI/Interface/"

ToolBar {
    property alias leftButton:leftButton
    property alias rightButton:rightButton
    property string title:"CodeCheck"
    signal leftClicked()
    signal rightClicked()

    width: parent.width;
    height: Global.toolBarHeight
    Material.foreground: "white"

    RowLayout {
        spacing: 20
        anchors.fill: parent
        QcButton{
            id:leftButton
            onClicked:leftClicked()
        }
        Label {
            text:title
            Layout.fillWidth:true
            font.pixelSize: 20
            elide: Label.ElideRight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
        }
        QcButton{
            id:rightButton
            onClicked:rightClicked()
        }
    }


}
