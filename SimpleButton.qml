import QtQuick 2.0

Rectangle {
    id: root

    signal clicked()

    property alias text: mainText.text

    color: "gray"
    border.color: "black"
    border.width: 2
    radius: 5

    Text {
        id: mainText
        anchors.fill: parent
        anchors.leftMargin: 10
        verticalAlignment: Text.AlignVCenter
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }

}
