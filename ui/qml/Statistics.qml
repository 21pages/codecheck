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
        objectName: "SwipeViewStatistics"
        currentIndex: 0
        anchors.fill: parent
        property var statisticsLoaded: false
        property font bigTitleFont: Qt.font({family: "Times", pixelSize: 18, bold:true, capitalization: Font.AllUppercase})
        property font labelFont: Qt.font({family: "Times", bold:true})
        property font legentFont: Qt.font({family: "Times", bold:true})

        ChartView {
                id:chartSeverity;
                objectName: "chartViewSeverity"
                legend.visible: true
                legend.alignment: Qt.AlignRight
                legend.font:view.legentFont
                antialiasing: true
                animationOptions:ChartView.GridAxisAnimations
                title: "各\"等级\"占比"
                titleFont: view.bigTitleFont
                PieSeries {
                    id:severityPie;
                }
        }
        ChartView {
                id:chartID;
                objectName: "chartViewID"
                legend.visible: true
                legend.alignment: Qt.AlignRight
                legend.font:view.legentFont
                antialiasing: true
                animationOptions:ChartView.GridAxisAnimations
                title: "各\"问题\"占比"
                titleFont: view.bigTitleFont
                PieSeries {
                    id:idPie;
                }
        }
        ChartView {
                id:charViewError;
                objectName: "chartViewError"
                legend.visible: true
                legend.alignment: Qt.AlignRight
                legend.font:view.legentFont
                antialiasing: true
                animationOptions:ChartView.GridAxisAnimations
                title: "各\"错误\"的数量"
                titleFont: view.bigTitleFont
                BarSeries {
                    id:barSeriesError;
                    labelsVisible: true
                    labelsPosition: AbstractBarSeries.LabelsCenter
                    labelsFormat: "@value"
                    axisX: BarCategoryAxis {
                        id:barSeriesError_axisx
                        categories: [" "];
                        visible: false
                    }
                    axisY: ValueAxis{
                        id:barSeriesError_axisY
                        min: 0
                        titleText:"数量"
                    }
                }
        }

        ChartView {
                id:charViewWarning;
                objectName: "chartViewWarning"
                legend.visible: true
                legend.alignment: Qt.AlignRight
                legend.font:view.legentFont
                antialiasing: true
                animationOptions:ChartView.GridAxisAnimations
                title: "各\"警告\"的数量"
                titleFont: view.bigTitleFont
                BarSeries {
                    id:barSeriesWarning;
                    labelsVisible: true
                    labelsPosition: AbstractBarSeries.LabelsCenter
                    labelsFormat: "@value"
                    axisX: BarCategoryAxis {
                        id:barSeriesWarning_axisx
                        categories: [" "];
                        visible: false
                    }
                    axisY: ValueAxis{
                        id:barSeriesWarning_axisY
                        min: 0
                        titleText:"数量"
                    }
                }
        }


        ChartView {
                id:charViewStyle;
                objectName: "chartViewStyle"
                legend.visible: true
                legend.alignment: Qt.AlignRight
                legend.font:view.legentFont
                antialiasing: true
                animationOptions:ChartView.GridAxisAnimations
                title: "各\"风格\"的数量"
                titleFont: view.bigTitleFont
                BarSeries {
                    id:barSeriesStyle;
                    labelsVisible: true
                    labelsPosition: AbstractBarSeries.LabelsCenter
                    labelsFormat: "@value"
                    axisX: BarCategoryAxis {
                        id:barSeriesStyle_axisx
                        categories: [" "];
                        visible: false
                    }
                    axisY: ValueAxis{
                        id:barSeriesStyle_axisY
                        min: 0
                        titleText:"数量"
                    }
                }
        }

        ChartView {
                id:charViewPerformance;
                objectName: "chartViewPerformance"
                legend.visible: true
                legend.alignment: Qt.AlignRight
                legend.font:view.legentFont
                antialiasing: true
                animationOptions:ChartView.GridAxisAnimations
                title: "各\"性能\"的数量"
                titleFont: view.bigTitleFont
                BarSeries {
                    id:barSeriesPerformance;
                    labelsVisible: true
                    labelsPosition: AbstractBarSeries.LabelsCenter
                    labelsFormat: "@value"
                    axisX: BarCategoryAxis {
                        id:barSeriesPerformance_axisx
                        categories: [" "];
                        visible: false
                    }
                    axisY: ValueAxis{
                        id:barSeriesPerformance_axisY
                        min: 0
                        titleText:"数量"
                    }
                }
        }

        ChartView {
                id:charViewPortability;
                objectName: "chartViewPortability"
                legend.visible: true
                legend.alignment: Qt.AlignRight
                legend.font:view.legentFont
                antialiasing: true
                animationOptions:ChartView.GridAxisAnimations
                title: "各\"平台\"的数量"
                titleFont: view.bigTitleFont
                BarSeries {
                    id:barSeriesPortability;
                    labelsVisible: true
                    labelsPosition: AbstractBarSeries.LabelsCenter
                    labelsFormat: "@value"
                    axisX: BarCategoryAxis {
                        id:barSeriesPortability_axisx
                        categories: [" "];
                        visible: false
                    }
                    axisY: ValueAxis{
                        id:barSeriesPortability_axisY
                        min: 0
                        titleText:"数量"
                    }
                }
        }

        ChartView {
                id:charViewInformation;
                objectName: "chartViewInformation"
                legend.visible: true
                legend.alignment: Qt.AlignRight
                legend.font:view.legentFont
                antialiasing: true
                animationOptions:ChartView.GridAxisAnimations
                title: "各\"提示\"的数量"
                titleFont: view.bigTitleFont
                BarSeries {
                    id:barSeriesInformation;
                    labelsVisible: true
                    labelsPosition: AbstractBarSeries.LabelsCenter
                    labelsFormat: "@value"
                    axisX: BarCategoryAxis {
                        id:barSeriesInformation_axisx
                        categories: [" "];
                        visible: false
                    }
                    axisY: ValueAxis{
                        id:barSeriesInformation_axisY
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

        gen_pieSeries(severityPie,obj["severity"]);
        gen_pieSeries(idPie,obj["id"]);
        gen_barSeries(barSeriesError,barSeriesError_axisY,obj["error"]);
        gen_barSeries(barSeriesWarning,barSeriesWarning_axisY,obj["warning"]);
        gen_barSeries(barSeriesStyle,barSeriesStyle_axisY,obj["style"]);
        gen_barSeries(barSeriesPerformance,barSeriesPerformance_axisY,obj["performance"]);
        gen_barSeries(barSeriesPortability,barSeriesPortability_axisY,obj["portability"]);
        gen_barSeries(barSeriesInformation,barSeriesInformation_axisY,obj["information"]);

        view.statisticsLoaded = true;
    }

    function gen_pieSeries(pie,obj) {
        pie.clear()
        for(var key in obj) {
            pie.append(key,obj[key])
        }
        for (var i = 0; i < pie.count; i++) {
            var slice = pie.at(i);
           slice.lable = slice.percentage * 100 + "%(" + slice.value + ")";
           slice.labelVisible = true
           slice.labelPosition = PieSlice.LabelOutside
        }
    }

    function gen_barSeries(bar,axisY,obj) {
        bar.clear();
        axisY.max = obj["max"] + 1;
        for(var key in obj) {
            if(key !== "max") {
                var arr = [obj[key]];
                bar.append(key,arr);
            }
        }
    }
}

