import QtQuick 2.12
import QtMultimedia 5.4
import QtQuick.Window 2.2
import com.facedetection.classes 1.0

Rectangle {
    id: root

    width: Screen.desktopAvailableWidth
    height: Screen.desktopAvailableHeight
    color: "black"

    property bool isLandscape: Screen.orientation === Qt.LandscapeOrientation || Screen.orientation === Qt.InvertedLandscapeOrientation
    property var result: []
    signal cameraClicked()


    Camera {
        id: mainCamera
        captureMode: Camera.CaptureStillImage
        focus.focusMode: CameraFocus.FocusContinuous
    }

    Rectangle {
        id: imageItem

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.rightMargin: root.isLandscape ? root.width*0.3 : 0
        anchors.bottomMargin: root.isLandscape ? 0 : root.height*0.45

        color: "transparent"

        VideoOutput {
            id: viewfinder
            anchors.fill: parent
            source: mainCamera
            autoOrientation: true

            filters : [testFilter]

            Rectangle{
                x: viewfinder.contentRect.x
                y: viewfinder.contentRect.y
                width: viewfinder.contentRect.width
                height: viewfinder.contentRect.height

                color: "transparent"
                border.color: "white"
                border.width: 1

                Repeater {
                    id: roundRepeater
                    model: root.result

                    delegate: Rectangle {
                        x: modelData.x * parent.width
                        y: modelData.y * parent.height
                        width: 30
                        height: 30
                        border.color: "green"
                        border.width: 5
                        opacity: 0.5
                        radius: 30
                    }
                }

                Repeater {
                    id: rectangleRepeater
                    model: root.result

                    delegate: Rectangle {
                        x: root.isLandscape ? modelData.x * parent.width : modelData.x * parent.width
                        y: root.isLandscape ? modelData.y * parent.height : modelData.y * parent.height
                        width: modelData.width * parent.width
                        height: modelData.height * parent.height

                        border.color: "red"
                        border.width: 3
                        opacity: 0.3
                    }
                }
            }
        }
    }

    DetectionFilter
    {
        id: testFilter
        orientation: Screen.orientation
        onObjectDetected :
        {
            root.result = list
        }
    }

    FilterControls {
        anchors.top: root.isLandscape ? parent.top : imageItem.bottom
        anchors.left: root.isLandscape ? imageItem.right : parent.left
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        camera: mainCamera
    }

}
