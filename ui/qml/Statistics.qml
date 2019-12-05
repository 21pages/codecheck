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
        objectName: "statistics_view"
        currentIndex: 0
        anchors.fill: parent
        property var statisticsLoaded: false

        ChartView {
                id:chartSeverity;
                objectName: "chartSeverity"
                legend.visible: true
                legend.alignment: Qt.AlignRight
                antialiasing: true
                animationOptions:ChartView.GridAxisAnimations
                localizeNumbers:true
                title: "等级"
                PieSeries {
                    id:severityPie;
                }
        }
        ChartView {
                id:chartID;
                objectName: "chartID"
                legend.visible: true
                legend.alignment: Qt.AlignRight
                antialiasing: true
                animationOptions:ChartView.GridAxisAnimations
                localizeNumbers:true
                title: "问题"
                PieSeries {
                    id:idPie;
                }
        }
        ChartView {
                id:chartErrorBar;
                objectName: "chartErrorBar"
                legend.visible: true
                legend.alignment: Qt.AlignRight
                antialiasing: true
                animationOptions:ChartView.GridAxisAnimations
                localizeNumbers:true
                title: "各种错误类型"
                BarSeries {
                    id:barSeriesError;
                    labelsVisible: true
                    labelsPosition: AbstractBarSeries.LabelsInsideEnd
                    labelsFormat: "@value"
                    axisX: BarCategoryAxis {
                        id:barSeriesError_axisx
                        categories: [" "];
                        titleText: "各种类型错误"
                        labelsVisible: false
                    }
                    axisY: ValueAxis{
                        id:barSeriesError_axisY
                        min: 0
                        titleText:"数量"
                    }
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
            if(Global.$statistics !== undefined) {
                timer.stop()
                 setStatistics(Global.$statistics)
            }

        }
    }

    Component.onCompleted: {
        timer.start()
    }

    function setStatistics(obj) {
        severityPie.clear()
        var obj_severity = obj["severity"];
        for(var key in obj_severity) {
            severityPie.append(key,obj_severity[key])
        }
         for (var i = 0; i < severityPie.count; i++) {
             var slice = severityPie.at(i);
            severityPie.at(i).lable = severityPie.at(i).percentage * 100 + "%(" + slice.value + ")";
            severityPie.at(i).labelVisible = true
            severityPie.at(i).labelPosition = PieSlice.LabelOutside
         }
        idPie.clear()
        var obj_id = obj["id"];
        for(var key in obj_id) {
            idPie.append(key,obj_id[key])
        }
        for (var i = 0; i < idPie.count; i++) {
            var slice = idPie.at(i);
           slice.lable = slice.percentage * 100 + "%(" + slice.value + ")";
           slice.labelVisible = true
           slice.labelPosition = PieSlice.LabelOutside
        }
        barSeriesError.clear();
        var obj_error = obj["error"];
        barSeriesError_axisY.max = obj_error["max"] + 1;
        for(var key in obj_error) {
            if(key !== "max") {
                var arr = [obj_error[key]];
                barSeriesError.append(key,arr);
            }

        }

        view.statisticsLoaded = true;
    }
}

