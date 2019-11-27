import QtQuick 2.4
import "../Element"

/*!
   \qmltype Card
   \inqmlmodule Material

   \brief A card is a piece of paper with unique related data that serves as an entry point
   to more detailed information.
 */
View {
    width: 300
    height: 250
    elevation: flat ? 0 : 1

    property bool flat: false

    border.color: flat ? Qt.rgba(0,0,0,0.2) : "transparent"
    radius: fullWidth || fullHeight ? 0 : 2

}
