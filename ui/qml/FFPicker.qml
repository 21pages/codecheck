import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import Qt.labs.folderlistmodel 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Window 2.12
import Qt.labs.settings 1.0
import "qrc:/js/utils.js" as Utils
import "qrc:/qc/"

Page {
    id:picker
    Layout.fillWidth: true
    Layout.fillHeight: true

    enum PickerType{
        File,
        Dir
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
    property int pickerType: FFPicker.PickerType.File
    property bool showHidden: true
    property bool showDirsFirst: true
    property string drive: "C"
    property string basefolder: prepath + drive + ":/"
    property string nameFilters: "*.*"

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

    header:MyToolBar {
        title:"选择文件"
        leftButton.icon.source:"qrc:/icons/navigation/back.svg"
        onLeftClicked:{
            cancel()
            stackView.pop()
        }
    }

    ColumnLayout{
        width: parent.width
        height: parent.height
        Rectangle {
            width: parent.width
            height: toolbarHeight
            QcComboBox {
                id:driveCombo
                model: driveModel
                width: 100
                height: parent.height
                anchors.left:parent.left
                anchors.leftMargin:10

                onCurrentTextChanged: {
                    drive = currentText.charAt(0);
                    folderListModel.folder = picker.basefolder
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
    }

    footer: RowLayout {
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
            width: 50
            text: "取消"
            onClicked: {
                cancel()
                stackView.pop()
            }
        }
    }

//    Settings{
//        fileName: provider.exeDir + "/app.ini"
//        category:"ffpicker"

//        id:settingFFPicker
//    }

//    Component.onCompleted: {

//    }

}
