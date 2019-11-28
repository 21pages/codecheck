pragma Singleton
import QtQml 2.0
import QtQuick.Controls.Material 2.12

QtObject {
    readonly property int screenHeight: 600
    readonly property int screenWidth: 800
     readonly property int toolBarHeight: 50
    readonly property int listHeight: 90
    readonly property int subListHeight: 40
    readonly property var severityColorMap: {
        "error":Material.color(Material.Red),
        "warning":Material.color(Material.Orange),
        "style":Material.color(Material.Purple),
        "performance":Material.color(Material.Blue),
        "portability":Material.color(Material.Green),
        "information":Material.color(Material.Cyan)
    }
    property var $listItemVar: ({})
}
