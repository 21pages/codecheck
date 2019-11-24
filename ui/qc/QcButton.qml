
import QtQuick 2.12
import QtQuick.Controls 2.12
import "qrc:/MaterialUI/Element/"


Button {
  id: control
  background: Rectangle {
      radius:control.width / 2
      opacity: !hovered ? 0 : (pressed ? 0.2 : 0.1)
      border.color: control.down ? "#17a81a" : "#21be2b"
      border.width: 1
  }


}
