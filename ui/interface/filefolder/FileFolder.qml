import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import Qt.labs.folderlistmodel 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Window 2.12
import Qt.labs.settings 1.0
import "qrc:/js/utils.js" as Utils
import "qrc:/qc/"
import "qrc:/qml/"
import FileFolder 1.0

FileFolderInterface {
    id:control

    enum FileFolderType{
        FileFolderType_File,
        FileFolderType_Dir
    }

    signal ok(string path)
    signal cancel()
    readonly property real textmargin: Utils.dp(Screen.pixelDensity, 8)
    readonly property real textSize: Utils.dp(Screen.pixelDensity, 15)
    readonly property real headerTextSize: Utils.dp(Screen.pixelDensity, 12)
    readonly property real buttonHeight: Utils.dp(Screen.pixelDensity, 24)
    readonly property real rowHeight: Utils.dp(Screen.pixelDensity, 36)
    readonly property real toolbarHeight: Utils.dp(Screen.pixelDensity, 48)
    readonly property string prepath: "file:///"
    property bool showHidden: true
    property bool showDirsFirst: true
    property string drive: control.startDir.charAt(0)
    property string basefolder: prepath + drive + ":/"

    function currentFolder() {
        return folderListModel.folder;
    }

    function isFolder(fileName) {
        return folderListModel.isFolder(folderListModel.indexOf(folderListModel.folder + "/" + fileName));
    }
    function canMoveUp() {
        return folderListModel.folder.toString() !== basefolder;
    }

    function onItemClicked(fileName,index) {
        field.text = fileName;
        view.currentIndex = index
    }

    function onItemDoubleClicked(fileName,index) {
        if(!isFolder(fileName)) {
            view.currentIndex = index
            field.text = fileName;
            return;
        }
        if(folderListModel.folder.toString() === basefolder) {
            folderListModel.folder += fileName
        } else {
            folderListModel.folder += "/" + fileName
        }
        field.text = ""
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

    Page {
        id:picker
        anchors.fill:parent

        header:MyToolBar {
            title:(control.type === FileFolder.FileFolderType_File)?"选择文件":"选择文件夹"
            leftButton.icon.source:"qrc:/icons/navigation/back.svg"
            onLeftClicked:{
                cancel()
                stackView.pop()
            }
        }

        Rectangle{
            width: parent.width
            height: parent.height
            Rectangle {
                id:topWrapper
                width: picker.width
                height: toolbarHeight
                QcComboBox {
                    id:driveCombo
                    model: driveModel
                    width: 100
                    height: parent.height
                    anchors.left:parent.left
                    anchors.leftMargin:10

                    onActivated: {
                        drive = driveCombo.currentText.charAt(0);
                        folderListModel.folder = control.basefolder
                    }
                }
                Label {
                    id: filePath
                    anchors.left:driveCombo.right
                    anchors.leftMargin:10
                    anchors.right:button.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    text: folderListModel.folder.toString().replace(prepath, "►").replace(new RegExp("/",'g'), "►")
                    renderType: Text.NativeRendering
                    elide: Text.ElideLeft
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: textSize
                }
                Button {
                    id: button
                    width: 50
                    height: toolbarHeight
                    anchors.right:parent.right
                    anchors.rightMargin:10
                    icon.source: "qrc:/icons/navigation/back2.svg"
                    enabled: canMoveUp()
                    flat: true
                    onClicked: {
                        onBackBtnClicked()
                    }
                }
            }


            ListView {
                id: view
                anchors{
                    top: topWrapper.bottom
                    topMargin: 20
                    bottom: parent.bottom
                    bottomMargin: 20
                }
                width: parent.width
                model: folderListModel
                delegate: fileDelegate
                ScrollBar.vertical: ScrollBar {active: true}
                ScrollBar.horizontal: ScrollBar { }
                flickableDirection: Flickable.AutoFlickDirection
                z:1

                FolderListModel {
                    id:  folderListModel
                    showDotAndDotDot: false
                    showHidden: control.showHidden
                    showDirsFirst: control.showDirsFirst
                    folder: Utils.addSlash(control.prepath) +  control.startDir
                    nameFilters: control.nameFilters
                    showFiles: control.type === FileFolder.FileFolderType_File
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
                                width: control.width
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
                    console.log(folderListModel.nameFilters)
                }
            }
        }

        footer: Rectangle {
            id:footWrapper
            width: parent.width
            height: 50
            TextField {
                id: field
                anchors {
                    left: parent.left
                    leftMargin:10
                    right: okBtn.left
                    rightMargin: 10
                }
                placeholderText: (control.type === FileFolder.FileFolderType_File) ? "新文件名":"新文件夹名"
                text: control.startName
            }
            Button {
                id:okBtn
                width: 50
                anchors {
                    right: cancelBtn.left
                    rightMargin: 10
                }
                text: "选择"
                onClicked: {
                    var folder = new String(folderListModel.folder);
                    var path = Utils.addSlash(folder) + field.text;
                    var index = path.indexOf(prepath);
                    if(index === 0) {
                        path = path.substring(prepath.length)
                    }
                    ok(path)
                    stackView.pop()
                }
            }
            Button {
                id:cancelBtn;
                anchors{
                    right: parent.right
                    rightMargin: 10
                }
                width: 50
                text: "取消"
                onClicked: {
                    cancel()
                    stackView.pop()
                }
            }
        }

    }
}

