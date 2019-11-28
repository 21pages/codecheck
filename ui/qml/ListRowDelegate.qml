import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Material 2.12
import QtQuick.Controls 2.12
import "qrc:/qc/"
import "qrc:/MaterialUI/Interface/"

Control {
    id:listViewDelegate
    property var d
    Layout.fillWidth: true
    Layout.fillHeight: true
    height:Global.listHeight + listView2.height
    signal listViewItemClicked(var obj)

    MaterialCard{
        id:card
        anchors.margins: 5
        height: Global.listHeight - anchors.margins * 2
        width: parent.width - anchors.margins * 2
        elevation:2

        Control {
            id:flickable
            width: parent.width
            height: parent.height
            Column {
                id:layout1
                height: parent.height
                width: parent.width
                Row {
                    id:layout2
                    height: parent.height * 0.8
                    width: parent.width
                    QcColoredImage {
                        id:severityImage
                        height: parent.height
                        width: height
                        source:"qrc:/icons/alert/"+ modelData.severity+ ".svg"
                        overlayColor:Global.severityColorMap[modelData.severity] !== undefined ? Global.severityColorMap[modelData.severity]:"#fefefe";
                    }
                    Column {
                        width: parent.width - severityImage.width
                        height: parent.height
                        RowLayout {
                            height: parent.height * 0.6
                            Text {
                                text: modelData.id
                                padding: 10
                            }
                            Text {
                                text:(modelData.array.length === 0) ? ("行号:" + modelData.line) : ("相关行数:" + modelData.array.length)
                                padding: 10
                                fontSizeMode: Text.Fit
                            }
                        }
                        Text {
                            height: parent.height * 0.4
                            text: modelData.summary
                            padding: 10
                            font.weight: Font.Light
                        }
                    }
                }
                Rectangle {
                    height: parent.height * 0.2
                    width: parent.width
                    Label {
                        anchors.right: parent.right
                        height: parent.height
                        text: index + 1
                    }
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if(modelData.array.length !== 0) {
                    var visible =  listView2.visible;
                    listView2.visible = !visible;
                    listView2.height = (!visible) ? Global.subListHeight * modelData.array.length: 0;
                }
                var obj = {"file":modelData.file,"line":modelData.line}
                listViewItemClicked(obj);
            }
        }
    }


    ListView {
        y:Global.listHeight
        id:listView2
        model: modelData.array
        delegate:LineRowDelegate{
            onListViewItemClicked:{
                listViewDelegate.listViewItemClicked(obj);
            }
        }
        visible: false
    }
}

