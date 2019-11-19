import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import Qt.labs.folderlistmodel 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Window 2.12
import "qrc:/js/utils.js" as Utils

Dialog {
    id:picker
    width: 600;height: 500;
    x: parent.width / 2 - width / 2
    y: parent.height / 2 - height / 2
    modal: true
    title: getTitle()
    standardButtons: Dialog.Ok | Dialog.Cancel

    enum PickerType{
        ChooseFile,
        ChooseDir,
        SaveFile
    }
    readonly property real textmargin: Utils.dp(Screen.pixelDensity, 8)
    readonly property real textSize: Utils.dp(Screen.pixelDensity, 15)
    readonly property real headerTextSize: Utils.dp(Screen.pixelDensity, 12)
    readonly property real buttonHeight: Utils.dp(Screen.pixelDensity, 24)
    readonly property real rowHeight: Utils.dp(Screen.pixelDensity, 36)
    readonly property real toolbarHeight: Utils.dp(Screen.pixelDensity, 48)
    readonly property string prepath: "file:///"
    property int pickerType: FilePickerDialog.PickerType.ChooseFile
    property bool showHidden: true
    property bool showDirsFirst: true
    property string drive: "C"
    property string basefolder: prepath + drive + ":/"
    property string nameFilters: "*.*"
    property string retPath: ""


    onAccepted: {

    }

    onRejected: {

    }

    function getTitle()
    {
        if(pickerType === 0) {
            return "选择文件"
        } else if(pickerType === 1) {
            return "选择目录"
        } else if(pickerType === 2) {
            return "保存文件"
        }
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

    function emitFile(fileName){
        var folder = new String(folderListModel.folder);
        if(folder.charAt(folder.length - 1) !== "/") {
            folder += '/';
        }
        folder += fileName;
        fileSelected(folder);
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


    contentItem: ColumnLayout{
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

                    Text {
                        id: filePath
                        width: parent.width - driveCombo.width - button.width - spacing * 4
                        text: folderListModel.folder.toString().replace(prepath, "►").replace(new RegExp("/",'g'), "►")
                        renderType: Text.NativeRendering
                        elide: Text.ElideMiddle
                        font.bold: true
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: textSize
                    }

                    Button {
                        id: button
                        width: 50
                        Layout.alignment: Qt.AlignRight
                        icon.source: "qrc:/icons/navigation/back2.svg"
                        enabled: canMoveUp()
                        flat: true
                        onClicked: {
                            if(canMoveUp) {
                                folderListModel.folder = folderListModel.parentFolder
                            }
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

        TextField {
            id: field
            Layout.fillWidth: true
            placeholderText: ""
        }

    }
}
