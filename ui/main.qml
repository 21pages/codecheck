import QtQuick 2.12
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import "qrc:/Material/" //as Material
import "qrc:/Material/ListItems/" as ListItem
import "qrc:/Material/Extras/" as Extras
import CC 1.0 as CC

/*Material.*/ApplicationWindow {
    id:root
    visible:  true
    width: dp(800); height: dp(600)
    //minimumWidth: 800; minimumHeight: 600
    flags: flags | Qt.FramelessWindowHint

    theme {
        primaryColor: "blue"
        accentColor: "red"
        tabHighlightColor: "white"
    }

    property var styles: [
            "a1", "a2", "a3"
    ]

    property var basicComponents: [
            "b1", "ResultTree", "b3", "b4",
            "b5", "b6", "b7"
    ]

    property var compoundComponents: [
            "c1", "c2", "c3", "c4", "c5", "c6", "c7"
    ]

    property var sections: [ basicComponents, styles, compoundComponents ]

    property var sectionTitles: [ "Basic Components", "Style", "Compound Components" ]

    property string selectedComponent: sections[0][0]

    CC.OpenProjectManager{
        id:openProject;
    }

    Component.onCompleted: {
        root.showMaximized()
    }

    initialPage: TabbedPage {
        id: page

        title: "Demo"

        actionBar.maxActionCount: navDrawer.enabled ? 3 : 4

        actions: [
            Action {
                iconName: "maps/place"
                name:"项目"
                onTriggered: projectSheet.open()
            },

            Action {
                iconName: "alert/warning"
                name: "Dummy error"
                onTriggered: root.showError("Something went wrong", "Do you want to retry?", "Close", true)
            },

            Action {
                iconName: "image/color_lens"
                name: "Colors"
                onTriggered: colorPicker.show()
            },

            Action {
                iconName: "action/settings"
                name: "Settings"
                hoverAnimation: true
            },

            Action {
                iconName: "alert/warning"
                name: "THIS SHOULD BE HIDDEN!"
                visible: false
            },

            Action {
                iconName: "action/language"
                name: "Language"
                enabled: false
            },

            Action {
                iconName: "action/account_circle"
                name: "Accounts"
            }
        ]

        backAction: navDrawer.action

        NavigationDrawer {
            id: navDrawer

            enabled: page.width < dp(500)

            onEnabledChanged: smallLoader.active = enabled

            Flickable {
                anchors.fill: parent

                contentHeight: Math.max(content.implicitHeight, height)

                Column {
                    id: content
                    anchors.fill: parent

                    Repeater {
                        model: sections

                        delegate: Column {
                            width: parent.width

                            ListItem.Subheader {
                                text: sectionTitles[index]
                            }

                            Repeater {
                                model: modelData
                                delegate: ListItem.Standard {
                                    text: modelData
                                    selected: modelData == root.selectedComponent
                                    onClicked: {
                                        root.selectedComponent = modelData
                                        navDrawer.close()
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        Repeater {
            model: !navDrawer.enabled ? sections : 0

            delegate: Tab {
                title: sectionTitles[index]

                property string selectedComponent: modelData[0]
                property var section: modelData

                sourceComponent: tabDelegate
            }
        }

        Loader {
            id: smallLoader
            anchors.fill: parent
            sourceComponent: tabDelegate

            property var section: []
            visible: active
            active: false
        }
    }

    Dialog {
        id: colorPicker
        title: "Pick color"

        positiveButtonText: "Done"

        MenuField {
            id: selection
            model: ["Primary color", "Accent color", "Background color"]
            width: dp(160)
        }

        Grid {
            columns: 7
            spacing: dp(8)

            Repeater {
                model: [
                    "red", "pink", "purple", "deepPurple", "indigo",
                    "blue", "lightBlue", "cyan", "teal", "green",
                    "lightGreen", "lime", "yellow", "amber", "orange",
                    "deepOrange", "grey", "blueGrey", "brown", "black",
                    "white"
                ]

                Rectangle {
                    width: dp(30)
                    height: dp(30)
                    radius: dp(2)
                    color: Palette.colors[modelData]["500"]
                    border.width: modelData === "white" ? dp(2) : 0
                    border.color: Theme.alpha("#000", 0.26)

                    Ink {
                        anchors.fill: parent

                        onPressed: {
                            switch(selection.selectedIndex) {
                                case 0:
                                    theme.primaryColor = parent.color
                                    break;
                                case 1:
                                    theme.accentColor = parent.color
                                    break;
                                case 2:
                                    theme.backgroundColor = parent.color
                                    break;
                            }
                        }
                    }
                }
            }
        }

        onRejected: {
            // TODO set default colors again but we currently don't know what that is
        }
    }

    Component {
        id: tabDelegate

        Item {

            Sidebar {
                id: sidebar

                expanded: !navDrawer.enabled

                Column {
                    width: parent.width

                    Repeater {
                        model: section
                        delegate: ListItem.Standard {
                            text: modelData
                            selected: modelData == selectedComponent
                            onClicked: selectedComponent = modelData
                        }
                    }
                }
            }
            Flickable {
                id: flickable
                anchors {
                    left: sidebar.right
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }
                clip: true
                contentHeight: Math.max(example.implicitHeight + 40, height)
                Loader {
                    id: example
                    anchors.fill: parent
                    asynchronous: true
                    visible: status == Loader.Ready
                    // selectedComponent will always be valid, as it defaults to the first component
                    source: {
                        if (navDrawer.enabled) {
                            return Qt.resolvedUrl("qrc:/qml/%.qml").arg(root.selectedComponent.replace(" ", ""))
                        } else {
                            return Qt.resolvedUrl("qrc:/qml/%.qml").arg(selectedComponent.replace(" ", ""))
                        }
                    }
                }

                ProgressCircle {
                    anchors.centerIn: parent
                    visible: example.status == Loader.Loading
                }
            }
            Scrollbar {
                flickableItem: flickable
            }
        }
    }

    BottomActionSheet {
        id: projectSheet

        title: "项目"

        actions: [
            Action {
                iconName: "social/share"
                name: "新建项目"
            },

            Action {
                iconName: "file/file_download"
                name: "打开项目"
                onTriggered: openProject.open()
            },

            Action {
                iconName: "action/autorenew"
                name: "编辑项目"
                hasDividerAfter: true
            },

            Action {
                iconName: "action/settings"
                name: "关闭项目"
            }
        ]
    }

}

