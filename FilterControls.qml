import QtQuick 2.12
import QtQuick.Window 2.2
import QtMultimedia 5.4

Rectangle {
    id: root

    color: "black"

    property bool isLandscape: Screen.orientation === Qt.LandscapeOrientation || Screen.orientation === Qt.InvertedLandscapeOrientation
    property var camera: null

    Column {
        id: controlColumn
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        property variant classifiers: ["Haar Cascade", "Match Template"]
        property variant qualities: ["Low", "Medium", "High"]

        Text {
            color: "white"
            text: "Cameras:"
        }

        Repeater {
            anchors.left: parent.left
            anchors.right: parent.right

            model: QtMultimedia.availableCameras

            delegate: SimpleButton  {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 100
                text: modelData.displayName
                color:  root.camera.deviceId === modelData.deviceId ? "green" : "gray"

                onClicked: {
                    root.camera.deviceId = modelData.deviceId
                    root.camera.focus.focusMode = CameraFocus.FocusContinuous
                }
            }
        }

        Text {
            color: "white"
            text: "Classifier type:"
        }

        Row {
            height: 100
            anchors.left: parent.left
            anchors.right: parent.right

            Repeater {
                model: 2

                SimpleButton {
                    height: 100
                    width: parent.width/2
                    color: testFilter.classifierType === index ? "green" : "gray"
                    text: controlColumn.classifiers[index]
                    onClicked: testFilter.setClassifierType(index)
                }
            }
        }

        Text {
            color: "white"
            text: "Quality:"
        }

        Row {
            height: 100
            anchors.left: parent.left
            anchors.right: parent.right

            Repeater {
                model: 3

                SimpleButton {
                    height: 100
                    width: parent.width/3
                    color: testFilter.quality === index ? "green" : "gray"
                    text: controlColumn.qualities[index]
                    onClicked: testFilter.setQuality(index)
                }
            }
        }

        Text {
            color: "white"
            text: "Options:"
        }

        SimpleButton {
            anchors.left: parent.left
            anchors.right: parent.right
            height: 100

            text: "Save image"
            onClicked: testFilter.saveLastImages(0,0,100,100)
        }
    }

}
