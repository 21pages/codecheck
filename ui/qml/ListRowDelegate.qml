import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Material 2.12
import QtQuick.Controls 2.12
import "qrc:/qc/"
import "qrc:/MaterialUI/Interface/"

Rectangle {
    id:listViewDelegate
    property var d
    Layout.fillWidth: true
    Layout.fillHeight: true
    height:Global.listHeight + listView2.height
    signal listViewItemClicked(var obj)
    property color textColor:listViewDelegate.ListView.isCurrentItem ? Material.color(Material.Blue) : "#000000"



    MaterialCard{
        id:card
        anchors.margins: 5
        height: Global.listHeight - anchors.margins * 2
        width: parent.width - anchors.margins * 2
        elevation:1

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
                        height: parent.height * 0.8
                        width: height
                        Layout.margins: parent.height * 0.1
                        source:"qrc:/icons/alert/"+ modelData.severity+ ".svg"
                        overlayColor:Global.severityColorMap[modelData.severity] !== undefined ? Global.severityColorMap[modelData.severity]:"#fefefe";
                    }
                    Column {
                        width: parent.width - severityImage.width
                        height: parent.height
                        /*RowLayout*/ Row{
                            height: parent.height * 0.6
                            width: parent.width
                            Text {
                                id:textID
                                text: modelData.id
                                padding: 5
                                color: textColor
                            }
                            Text {
                                id:textLine
                                text:"行号:" + modelData.line + "  " + ((modelData.array.length === 0) ? "" : ("相关行数:" + modelData.array.length))
                                padding: 5
                                fontSizeMode: Text.Fit
                                color: textColor
                            }

                            Rectangle {
                                width: parent.width - textID.width - textLine.width
                                height: parent.height
                                QcColoredImage {
                                    anchors.right: parent.right
                                    anchors.rightMargin: 10
                                    id:testBtn
                                    height: 15
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: height
                                    source: "qrc:/icons/file/file.svg"
                                    overlayColor: Material.color(Material.Orange)
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            projectManager.openFile(textFieldOpenFileExe.text,modelData.file)
                                        }
                                    }
                                }
                            }
                        }
                        Text {
                            height: parent.height * 0.4
                            text: modelData.summary
                            padding: 5
                            font.weight: Font.Light
                            color: textColor
                        }
                    }
                }
                Rectangle {
                    height: parent.height * 0.2
                    width: parent.width
                    color:"transparent"
                    Text {
                        id:textIndex
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        height: parent.height
                        verticalAlignment: Text.AlignTop
                        text: index + 1
                        color: Material.color(Material.Teal)
                    }
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            onClicked: {
                if(modelData.array.length !== 0) {
                    var visible =  listView2.visible;
                    listView2.visible = !visible;
                    listView2.height = (!visible) ? Global.subListHeight * modelData.array.length: 0;
                }
                var obj = {"file":modelData.file,"line":modelData.line}
                listViewItemClicked(obj);
                listView.currentIndex = index;
                mouse.accepted = false
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
        Component.onCompleted:  {
            listView2.currentIndex = -1
        }
    }


}

