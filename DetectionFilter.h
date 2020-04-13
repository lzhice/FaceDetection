#ifndef DETECTIONFILTER_H
#define DETECTIONFILTER_H

#include <QAbstractVideoFilter>
#include <QObject>
#include <QtQuick>
#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"

class DetectionFilter : public QAbstractVideoFilter
{
    Q_OBJECT

    Q_PROPERTY(Qualities quality READ quality WRITE setQuality NOTIFY qualityChanged);
    Q_PROPERTY(Classifiers classifierType READ classifierType WRITE setClassifierType NOTIFY classifierTypeChanged);
    Q_PROPERTY(int orientation READ orientation WRITE setOrientation NOTIFY orientationChanged);
    Q_PROPERTY(QImage lastImage READ lastImage WRITE setLastImage NOTIFY lastImageChanged);
    Q_PROPERTY(cv::Mat lastMat READ lastMat WRITE setLastMat NOTIFY lastMatChanged)
    Q_PROPERTY(QImage templateImage READ templateImage WRITE setTemplateImage NOTIFY templateImageChanged);
    Q_PROPERTY(cv::Mat templateMat READ templateMat WRITE setTemplateMat NOTIFY templateMatChanged)
    Q_PROPERTY(cv::CascadeClassifier classifier READ classifier WRITE setClassifier NOTIFY classifierChanged)


public:
    enum Qualities {
            Quality_Low = 0,
            Quality_Medium = 1,
            Quality_High = 2,
    };

    enum Classifiers {
            Classifier_HaarCascade = 0,
            Classifier_MatchTemplate = 1
    };

    Q_ENUMS(Qualities)
    Q_ENUMS(Classifiers)

public:
    QVideoFilterRunnable* createFilterRunnable();

    Qualities quality() const;
    Classifiers classifierType() const;
    int orientation() const;    
    QImage lastImage() const;
    cv::Mat lastMat() const;
    QImage templateImage() const;
    cv::Mat templateMat() const;
    cv::CascadeClassifier classifier() const;

signals:
    void objectDetected(QVariantList list);
    void qualityChanged(int quality);
    void classifierTypeChanged(Classifiers classifierType);
    void orientationChanged(int orientation);    
    void lastImageChanged(QImage lastImage);
    void lastMatChanged(cv::Mat lastMat);
    void templateImageChanged(QImage templateImage);
    void templateMatChanged(cv::Mat templateMat);
    void classifierChanged(cv::CascadeClassifier classifier);

public slots:
    void saveLastImages();
    void saveImage(QImage img, QString name);
    void saveImage(cv::Mat img, QString name);

    void setQuality(Qualities quality);
    void setClassifierType(Classifiers classifierType);
    void setOrientation(int orientation);
    void setLastImage(QImage lastImage);
    void setLastMat(cv::Mat lastMat);
    void setTemplateImage(QImage templateImage);
    void setTemplateMat(cv::Mat templateMat);
    void setClassifier(cv::CascadeClassifier classifier);

private:
    QString m_subdir = "OpenCV";
    Qualities m_quality = Qualities::Quality_Low;
    Classifiers m_classifierType = Classifiers::Classifier_HaarCascade;
    int m_orientation;
    int m_count = 0;
    QImage m_lastImage;
    cv::Mat m_lastMat;
    QImage m_templateImage;
    cv::Mat m_templateMat;
    cv::CascadeClassifier m_classifier;

    void loadClassifiersData();
};

class DetectionFilterRunnable : public QVideoFilterRunnable
{
public:
    DetectionFilterRunnable(DetectionFilter* creator);
    QVideoFrame run(QVideoFrame* input, const QVideoSurfaceFormat& surfaceFormat, RunFlags flags);

private:
    void dft(cv::InputArray input, cv::OutputArray output);
    std::vector<cv::Rect> runHaarCascade(const cv::Mat &mat);
    std::vector<cv::Rect> runMatchTemplate(const cv::Mat &mat, const cv::Mat &templ);
    DetectionFilter* m_filter;
};
#endif // DETECTIONFILTER_H
