import QtQuick 2.12
import QtQuick.Controls 2.12

  CheckBox {
      property alias source: image.source
      property alias color: image.overlayColor
      id: control
      indicator: QcColoredImage {
          id:image
          implicitWidth: 18
          implicitHeight: 18
          x: control.leftPadding
          y: parent.height / 2 - height / 2
        }

      contentItem: Text {
          id:textControl
          text: control.text
          font: control.font
          opacity: enabled ? 1.0 : 0.3
          color:control.checked ? "#17a81a" : "#88888888"
          verticalAlignment: Text.AlignVCenter
          leftPadding: control.indicator.width + control.spacing
      }
  }
