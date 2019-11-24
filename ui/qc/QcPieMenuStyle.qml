import QtQuick.Controls.Styles 1.4
import QtQuick 2.12
import QtQuick.Controls.Material 2.12

PieMenuStyle {
      shadowRadius: 0

      menuItem: Item {
          id: item
          rotation: -90 + sectionCenterAngle(styleData.index)

          Rectangle {
              width: parent.height * 0.2
              height: width
              color: "darkorange"
              radius: width / 2
              anchors.right: parent.right
              anchors.verticalCenter: parent.verticalCenter

              Text {
                  id: textItem
                  text: control.menuItems[styleData.index].text
                  anchors.centerIn: parent
                  color: control.currentIndex === styleData.index ? "red" : "white"
                  rotation: -item.rotation
              }
          }
      }
  }
