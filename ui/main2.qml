import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import CC 1.0 as CC
import "qrc:/qml/"
import QtGraphicalEffects 1.0



Window {
    id:root
    visible: true
//    minimumWidth: 600; minimumHeight: 800
    width: 600; height: 800

    CC.OpenProjectManager{
        id:openProject;
    }
    Button {
        x:0;y:0
        width: 200;height: 100
        text: "open"
        id:btn
        onClicked:openProject.open()
    }
    Item {
        x:200;y:0
        width:parent.width-200
        height: parent.height
        ResultList{

        }
    }



}
