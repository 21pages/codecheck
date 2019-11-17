/**
MIT License

Copyright (c) 2017 Andrey Semenov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

import QtQuick 2.5
import QtQuick.Controls 1.4 as OldControls
import QtQuick.Controls 2.1
import Qt.labs.folderlistmodel 2.1
import QtQuick.Window 2.0
import QtQuick.Controls.Material 2.5
import "qrc:/js/utils.js" as Utils

Item {
	id:picker
    signal folderSelected(string folderName)
	readonly property real textmargin: Utils.dp(Screen.pixelDensity, 8)
    readonly property real textSize: Utils.dp(Screen.pixelDensity, 15)
	readonly property real headerTextSize: Utils.dp(Screen.pixelDensity, 12)
	readonly property real buttonHeight: Utils.dp(Screen.pixelDensity, 24)
	readonly property real rowHeight: Utils.dp(Screen.pixelDensity, 36)
	readonly property real toolbarHeight: Utils.dp(Screen.pixelDensity, 48)
    readonly property string prepath: "file:///"
	property bool showDotAndDotDot: false
	property bool showHidden: true
	property bool showDirsFirst: true
    property string drive: "C"
    property string basefolder: prepath + drive + ":/"
	property string nameFilters: "*.*"
    property string selectedFolderPath: ""

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

    function onItemDoubleClicked(fileName) {
		if(fileName === ".." && canMoveUp()) {
			folderListModel.folder = folderListModel.parentFolder
		} else if(fileName !== ".") {
            if(folderListModel.folder.toString() === basefolder) {
				folderListModel.folder += fileName
			} else {
				folderListModel.folder += "/" + fileName
			}
		}
        setSelectedFolderPath();
	}

    function onItemClick(fileName) {
        var foldname = folderListModel.folder;
        if(foldname[foldname.length - 1] !== '/') {
            foldname += '/';
        }
        foldname += fileName
        selectedFolderPath = foldname
    }

    function setSelectedFolderPath()
    {
        selectedFolderPath = folderListModel.folder;
    }
	Rectangle {
		id: toolbar
		anchors.right: parent.right
		anchors.left: parent.left
		anchors.top: parent.top
		height: toolbarHeight
		color: Utils.backgroundColor()

        ComboBox {
            id:driveCombo
            model: driveModel
            width: 100
            anchors.left: parent.left
            anchors.leftMargin: buttonHeight
            anchors.bottom: parent.bottom
            anchors.top: parent.top
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

            onCurrentTextChanged: {
                drive = currentText.charAt(0);
                folderListModel.folder = picker.basefolder
            }
        }

        Text {
            id: filePath
            text: folderListModel.folder.toString().replace(prepath, "►").replace(new RegExp("/",'g'), "►")
            renderType: Text.NativeRendering
            elide: Text.ElideMiddle
            anchors.right: button.left
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            anchors.left: driveCombo.right
            anchors.leftMargin: 0
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            font.pixelSize: textSize
        }

		Button {
			id: button
            icon.source: "qrc:/icons/navigation/back.svg"
			anchors.right: parent.right
			anchors.rightMargin: buttonHeight
			anchors.bottom: parent.bottom
			anchors.top: parent.top
			enabled: canMoveUp()
			flat: true
            onClicked: {
				if(canMoveUp) {
					folderListModel.folder = folderListModel.parentFolder
				}
                setSelectedFolderPath();
			}
		}

	}

	FolderListModel {
		id:  folderListModel
		showDotAndDotDot: picker.showDotAndDotDot
		showHidden: picker.showHidden
		showDirsFirst: picker.showDirsFirst
        folder: picker.basefolder
        nameFilters: picker.nameFilters
        showFiles: false
	}


    ListView {
		id: view
		anchors.top: toolbar.bottom
		anchors.right: parent.right
        anchors.bottom: rect.top
		anchors.left: parent.left
		model: folderListModel
        delegate: fileDelegate

        Component {
			id: fileDelegate
            Item {
                id:fileDelegateItem
                width: view.width;
                height: rowHeight
                Rectangle {
                    anchors.fill: parent
                    color:fileDelegateItem.ListView.isCurrentItem ? Material.accent : Material.background
                    MouseArea {
                        anchors.fill: parent
                        onDoubleClicked: {
                            onItemDoubleClicked(fileNameText.text)
                            fileDelegateItem.ListView.view.currentIndex = -1
                        }
                        onClicked: {
                            onItemClick(fileNameText.text)
                            fileDelegateItem.ListView.view.currentIndex = index
                        }
                    }
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
                }

            }
        }
	}

    Rectangle {
        id:rect
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        TextField {
            id:textField
            anchors.left: parent.left
            anchors.right: okBtn.left
            anchors.rightMargin: 10
            height: parent.height
            text: getLastName(selectedFolderPath)
            readOnly: true
        }
        Button {
            id:okBtn
            text: "确定"
            anchors.right: cancelBtn.left
            anchors.rightMargin: 10
            width: 50
            height: parent.height
            enabled: selectedFolderPath !== ""
            onClicked: {
                picker.folderSelected(selectedFolderPath)
            }
        }
        Button {
            id:cancelBtn
            anchors.right: parent.right
            text: "取消"
            width: 50
            height: parent.height
        }
    }
}
