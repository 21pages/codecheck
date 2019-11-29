import QtQuick 2.9
import QtCharts 2.0
ChartView {
        id:chart;
        width: 400
        height: 300
        theme: ChartView.ChartThemeBrownSand
        antialiasing: true
        animationOptions:ChartView.GridAxisAnimations
        LineSeries {
            id:myLine;
            name: "LineSeries"
            XYPoint { x: 0; y: 0 }
            XYPoint { x: 1.1; y: 2.1 }
            XYPoint { x: 1.9; y: 3.3 }
            XYPoint { x: 2.1; y: 2.1 }
            XYPoint { x: 9.9; y: 4.9 }
            XYPoint { x: 3.4; y: 3.0 }
            XYPoint { x: 4.1; y: 3.3 }
        }
    }

//import QtQuick 2.0

//Item {}
