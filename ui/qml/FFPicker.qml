import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import Qt.labs.folderlistmodel 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Window 2.12
import "qrc:/js/utils.js" as Utils

Item {
    id:picker
    Layout.fillWidth: true
    Layout.fillHeight: true

    enum PickerType{
        File,
        Dir
    }
    signal ok(string reason,string path)
    signal cancel(string reason)
    readonly property real textmargin: Utils.dp(Screen.pixelDensity, 8)
    readonly property real textSize: Utils.dp(Screen.pixelDensity, 15)
    readonly property real headerTextSize: Utils.dp(Screen.pixelDensity, 12)
    readonly property real buttonHeight: Utils.dp(Screen.pixelDensity, 24)
    readonly property real rowHeight: Utils.dp(Screen.pixelDensity, 36)
    readonly property real toolbarHeight: Utils.dp(Screen.pixelDensity, 48)
    readonly property string prepath: "file:///"
    property int pickerType: FFPicker.PickerType.File
    property bool showHidden: true
    property bool showDirsFirst: true
    property string drive: "C"
    property string basefolder: prepath + drive + ":/"
    property string nameFilters: "*.*"
    property string reason: ""

    Component.onCompleted: {
        console.log("onCompleted")
    }
    Component.onDestruction: {
        console.log("onDestruction")
    }

    function currentFolder() {
        return folderListModel.folder;
    }

    function isFolder(fileName) {
        return folderListModel.isFolder(folderListModel.indexOf(folderListModel.folder + "/" + fileName));
    }
    function canMoveUp() {
        return folderListModel.folder.toString() !== basefolder;
    }

    function getLastName(folderName) {
        var index = folderName.lastIndexOf('/');
        if(index !== -1) {
            return folderName.substring(index+1);
        }
        return "";
    }

    function onItemClicked(fileName,index) {
        field.text = fileName;
        view.currentIndex = index
    }

    function onItemDoubleClicked(fileName,index) {
        field.text = fileName;
        if(!isFolder(fileName)) {
            view.currentIndex = index
            return;
        }
        if(folderListModel.folder.toString() === basefolder) {
            folderListModel.folder += fileName
        } else {
            folderListModel.folder += "/" + fileName
        }
        view.currentIndex = -1
    }

    function onBackBtnClicked()
    {
        if(canMoveUp) {
            folderListModel.folder = folderListModel.parentFolder
            field.text = ""
            view.currentIndex = -1
        }
    }


    /*contentItem: */ColumnLayout{
        width: parent.width
        height: parent.height
        Rectangle {
            width: Layout.fillWidth
            height: toolbarHeight
            RowLayout {
                    id: toolbar
                    width: parent.width
                    height: parent.height
                    spacing: 5
                    ComboBox {
                        id:driveCombo
                        model: driveModel
                        width: 100
                        height: parent.height
                        indicator: Canvas {
                                  id: canvas
                                  x: driveCombo.width - width - driveCombo.rightPadding
                                  y: driveCombo.topPadding + (driveCombo.availableHeight - height) / 2
                                  width: 12
                                  height: 8
                                  contextType: "2d"
                                  Connections {
                                      target: driveCombo
                                      onPressedChanged: canvas.requestPaint()
                                  }
                                  onPaint: {
                                      var context = getContext("2d")
                                      context.reset();
                                      context.moveTo(0, 0);
                                      context.lineTo(width, 0);
                                      context.lineTo(width / 2, height);
                                      context.closePath();
                                      context.fillStyle = driveCombo.pressed ? Material.accent : Material.foreground;
                                      context.fill();
                                  }
                              }
                        popup: Popup {
                                  y: driveCombo.height - 1
                                  width: driveCombo.width
                                  implicitHeight: contentItem.implicitHeight
                                  padding: 1
                                  contentItem: ListView {
                                      clip: true
                                      implicitHeight: contentHeight
                                      model: driveCombo.popup.visible ? driveCombo.delegateModel : null
                                      currentIndex: driveCombo.highlightedIndex

                                      ScrollIndicator.vertical: ScrollIndicator { }
                                  }
                                  background: Rectangle {
                                      border.color: Material.color(Material.Green)
                                      radius: 2
                                  }
                              }

                        onCurrentTextChanged: {
                            drive = currentText.charAt(0);
                            folderListModel.folder = picker.basefolder
                        }
                    }
                    Container {
                        contentItem: Text {
                            id: filePath
                            Layout.maximumWidth: 400
                            Layout.minimumWidth: 400
                            text: folderListModel.folder.toString().replace(prepath, "►").replace(new RegExp("/",'g'), "►")
                            renderType: Text.NativeRendering
                            elide: Text.ElideLeft
                            font.bold: true
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: textSize
                        }

                    }
                    Button {
                        id: button
                        width: 50
                        Layout.alignment: Qt.AlignRight
                        icon.source: "qrc:/icons/navigation/back2.svg"
                        enabled: canMoveUp()
                        flat: true
                        onClicked: {
                            onBackBtnClicked()
                        }
                    }
                }
        }


        ListView {
            id: view
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.topMargin: 20
            Layout.bottomMargin: 20
            model: folderListModel
            delegate: fileDelegate
            ScrollBar.vertical: ScrollBar {active: true}
            ScrollBar.horizontal: ScrollBar { }
            flickableDirection: Flickable.AutoFlickDirection
            z:1

            FolderListModel {
                id:  folderListModel
                showDotAndDotDot: false
                showHidden: picker.showHidden
                showDirsFirst: picker.showDirsFirst
                folder: picker.basefolder
                nameFilters: picker.nameFilters
            }

            Component {
                id: fileDelegate
                Rectangle {
                        id:fileDelegateItem
                        height: rowHeight
                        width: view.width
                        color:fileDelegateItem.ListView.isCurrentItem ? Material.accent : Material.background
                        Text {
                            id: fileNameText
                            height: width
                            anchors.left: image.right
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.right: parent.right
                            text: fileName !== undefined ? fileName : ""
                            verticalAlignment: Text.AlignVCenter
                        }
                        Image {
                            id: image
                            height: buttonHeight
                            width: height
                            anchors.left: parent.left
                            anchors.leftMargin: textmargin
                            anchors.verticalCenter: parent.verticalCenter
                            source: isFolder(fileNameText.text) ? "qrc:/images/ic_folder_open_black_48dp.png" : "qrc:/images/ic_insert_drive_file_black_48dp.png"
                        }
                        MouseArea {
                            id:mouseArea
                            width: picker.width
                            height: rowHeight
                            onClicked: {
                                onItemClicked(fileNameText.text,index)
                            }
                            onDoubleClicked: {
                                onItemDoubleClicked(fileNameText.text,index)
                            }
                        }
                    }
            }

            Component.onCompleted: {
                view.currentIndex = -1
            }
        }

        RowLayout {
            TextField {
                id: field
                Layout.fillWidth: true
                placeholderText: ""
            }
            Button {
                id:okBtn
                width: 50
                text: "选择"
                onClicked: {
                    var folder = new String(folderListModel.folder);
                    if(folder.charAt(folder.length - 1) !== "/") {
                        folder += '/';
                    }
                    var path = folder + field.text;
                    ok(reason,path)
                    stackView.pop()
                }
            }
            Button {
                id:cancelBtn;
                width: 50
                text: "取消"
                onClicked: {
                    cancel(reason)
                    stackView.pop()
                }
            }
        }



    }
}
