pragma Singleton
import QtQml 2.0

QtObject {
    readonly property int screenHeight: 600
    readonly property int screenWidth: 800
     readonly property int toolBarHeight: 50
    readonly property int listHeight: 100
    readonly property int subListHeight: 80
    readonly property var severityColorMap: {"error":"red","warning":"yellow","style":"blue","performance":"green","portability":"#fefe00","information":"#786543"}
    property var $listItemVar: ({})
}
