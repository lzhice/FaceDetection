QT += quick
QT += multimedia
QT += multimediawidgets
QT += multimedia-private
QT += androidextras

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    DetectionFilter.h

SOURCES += \
    DetectionFilter.cpp \
    main.cpp

RESOURCES += qml.qrc

android {
    #opencv
    OPENCVLIBS = D:/Work/Video/opencv/opencv-master/build/install/sdk/native
    INCLUDEPATH = $$OPENCVLIBS/jni/include

    contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
        # might need libs to be copied into app's library folder and loaded on start-up, in case android ships older libs??!
        ANDROID_EXTRA_LIBS = \
        $$OPENCVLIBS/libs/armeabi-v7a/libopencv_core.so \
        $$OPENCVLIBS/libs/armeabi-v7a/libopencv_imgproc.so \
        $$OPENCVLIBS/libs/armeabi-v7a/libtbb.so \
        $$OPENCVLIBS/libs/armeabi-v7a/libopencv_imgcodecs.so \
        $$OPENCVLIBS/libs/armeabi-v7a/libopencv_objdetect.so \
        $$OPENCVLIBS/libs/armeabi-v7a/libopencv_features2d.so \
        $$OPENCVLIBS/libs/armeabi-v7a/libopencv_flann.so \
        $$OPENCVLIBS/libs/armeabi-v7a/libopencv_calib3d.so

        LIBS += -L"$$OPENCVLIBS/libs/armeabi-v7a"\
            -lopencv_core -lopencv_imgproc -ltbb -lopencv_imgcodecs -lopencv_objdetect -lopencv_calib3d -lopencv_features2d\
            -lopencv_flann
    }

    #application folder path
    ANDROID_PACKAGE_SOURCE_DIR = D:/Work/Video/FaceDetection
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    AndroidManifest.xml \
    build.gradle \
    gradle/wrapper/gradle-wrapper.jar \
    gradle/wrapper/gradle-wrapper.properties \
    gradlew \
    gradlew.bat \
    res/values/libs.xml

