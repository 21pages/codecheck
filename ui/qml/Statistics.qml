import QtCharts 2.3
import QtQuick 2.12
import QtQuick.Controls 2.12

Page {
    id: page

    header: MyToolBar {
        title:"统计"
        leftButton.icon.source:"qrc:/icons/navigation/back.svg"
        onLeftClicked:{
            stackView.pop()
        }
    }
    SwipeView {
        id: view
        currentIndex: 0
        anchors.fill: parent

        ChartView {
                id:chartSeverity;
                legend.visible: true
                legend.alignment: Qt.AlignRight
                antialiasing: true
                animationOptions:ChartView.GridAxisAnimations
                localizeNumbers:true
                title: "问题"
                PieSeries {
                    id:severityPie;
                }
        }
        ChartView {
                id:chartID;
                legend.visible: true
                legend.alignment: Qt.AlignRight
                antialiasing: true
                animationOptions:ChartView.GridAxisAnimations
                localizeNumbers:true
                title: "错误ID"
                PieSeries {
                    id:idPie;
                }
        }

    }

    PageIndicator {
        count: view.count
        currentIndex: view.currentIndex
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Timer {
        id:timer
        repeat: true
        interval: 100
        onTriggered: {
            if(root.testvar !== undefined) {
                timer.stop()
                 setStatistics(root.testvar)
            }

        }
    }

    Component.onCompleted: {
        timer.start()
    }

    function setStatistics(obj) {
        severityPie.clear()
        var obj_severity = obj["severity"];
        for(var key1 in obj_severity) {
            severityPie.append(key1,obj_severity[key1])
        }
         for (var i = 0; i < severityPie.count; i++) {
             var slice = severityPie.at(i);
            severityPie.at(i).lable = severityPie.at(i).percentage * 100 + "%(" + slice.value + ")";
            severityPie.at(i).labelVisible = true
            severityPie.at(i).labelPosition = PieSlice.LabelOutside
         }
        idPie.clear()
        var obj_id = obj["id"];
        for(var key2 in obj_id) {
            idPie.append(key2,obj_id[key2])
        }
        for (var i = 0; i < idPie.count; i++) {
            var slice = idPie.at(i);
           slice.lable = slice.percentage * 100 + "%(" + slice.value + ")";
           slice.labelVisible = true
           slice.labelPosition = PieSlice.LabelOutside
        }
    }
}

