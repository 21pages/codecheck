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

import QtQuick 2.0
import QtQuick.Controls 1.4 as OldControls
import QtQuick.Controls 2.1
import Qt.labs.folderlistmodel 2.1
import QtQuick.Window 2.0
import QtQuick.Controls.Material 2.5
import "qrc:/js/utils.js" as Utils

Item {
	id:picker
	signal fileSelected(string fileName)
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

    Component.onCompleted: {
        retPath = "";
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

    function emitFile(fileName){
        var folder = new String(folderListModel.folder);
        if(folder.charAt(folder.length - 1) !== "/") {
            folder += '/';
        }
        folder += fileName;
        fileSelected(folder);
    }

    function onItemDoubleClicked(fileName) {
		if(!isFolder(fileName)) {
            emitFile(fileName);
			return;
		}
		if(fileName === ".." && canMoveUp()) {
			folderListModel.folder = folderListModel.parentFolder
		} else if(fileName !== ".") {
            if(folderListModel.folder.toString() === basefolder) {
				folderListModel.folder += fileName
			} else {
				folderListModel.folder += "/" + fileName
			}
		}
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
                          border.color: "#21be2b"
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
            icon.source: "qrc:/icons/navigation/back2.svg"
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
	}

    ListView {
		id: view
		anchors.top: toolbar.bottom
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		model: folderListModel
        delegate: fileDelegate

		Component {
			id: fileDelegate
			Item {
                id:fileDelegateItem
				height: rowHeight
                width: view.width
				Rectangle {
					anchors.fill: parent
                    color:fileDelegateItem.ListView.isCurrentItem ? Material.accent : Material.background
					MouseArea {
						anchors.fill: parent
                        onDoubleClicked: {
                            onItemDoubleClicked(fileNameText.text)
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
}
