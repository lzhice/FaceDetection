#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>
#include "DetectionFilter.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<DetectionFilter>("com.facedetection.classes", 1, 0, "CvDetectFilter");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QQuickView view(url);

    view.show();

    return app.exec();
}
