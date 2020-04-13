#include "DetectionFilter.h"
#include <QDebug>
#include <QFile>
#include <QTemporaryFile>
#include <QtAndroid>


bool requestAndroidPermissions()
{
    //Request requiered permissions at runtime
    const QVector<QString> permissions({
                                        "android.permission.WRITE_EXTERNAL_STORAGE",
                                        "android.permission.READ_EXTERNAL_STORAGE"});

    for (const QString &permission : permissions){
        auto result = QtAndroid::checkPermission(permission);
        if (result == QtAndroid::PermissionResult::Denied){
            auto resultHash = QtAndroid::requestPermissionsSync(QStringList({permission}));
            if (resultHash[permission] == QtAndroid::PermissionResult::Denied)
                return false;
        }
    }

    return true;
}

QVideoFilterRunnable* DetectionFilter::createFilterRunnable()
{
    loadClassifiersData();
    DetectionFilterRunnable *res = new DetectionFilterRunnable(this);
    return res;
}

DetectionFilter::Qualities DetectionFilter::quality() const
{
    return m_quality;
}

void DetectionFilter::setQuality(Qualities quality)
{
    if (m_quality == quality)
        return;

    m_quality = quality;
    emit qualityChanged(m_quality);
}

DetectionFilter::Classifiers DetectionFilter::classifierType() const
{
    return m_classifierType;
}

void DetectionFilter::setClassifierType(Classifiers classifierType)
{
    if (m_classifierType == classifierType)
        return;

    m_classifierType = classifierType;
    emit classifierTypeChanged(m_classifierType);
}

int  DetectionFilter::orientation() const
{
    return m_orientation;
}

QImage DetectionFilter::lastImage() const
{
    return m_lastImage;
}

void DetectionFilter::setLastImage(QImage lastImage)
{
    if (m_lastImage == lastImage)
        return;

    m_lastImage = lastImage;
    m_count++;
    emit lastImageChanged(m_lastImage);
}

cv::Mat  DetectionFilter::lastMat() const
{
    return m_lastMat;
}

void  DetectionFilter::setLastMat(cv::Mat lastMat)
{
    m_lastMat = lastMat.clone();;
    emit lastMatChanged(m_lastMat);
}

void DetectionFilter::saveImage(QImage img, QString name)
{
    requestAndroidPermissions();

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    dir.mkdir(m_subdir);

    QString filename = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)
                  + QDir::separator() + m_subdir +  QDir::separator() + name;

    img.save(filename);
}

void DetectionFilter::saveImage(cv::Mat img, QString name)
{
    requestAndroidPermissions();

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    dir.mkdir(m_subdir);

    QString filename = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)
                  + QDir::separator() + m_subdir +  QDir::separator() + name;

    cv::imwrite(filename.toStdString(), img);
}

void DetectionFilter::saveLastImages()
{    
    QString str = QDateTime::currentDateTime().toString("hh-mm-ss") + "-" + QString::number(m_count) + ".jpg";
    saveImage(m_lastImage, "camera_" + str);
    saveImage(m_lastMat, "mat_" + str);
}

void DetectionFilter::loadClassifiersData()
{
    //load classifier
    QFile xml(":/data/haarcascade_frontalface_default.xml");
    if (xml.open(QFile::ReadOnly | QFile::Text)) {
        QTemporaryFile temp;
        if (temp.open()) {
            temp.write(xml.readAll());
            temp.close();
            if(m_classifier.load(temp.fileName().toStdString()))
            {
                qDebug() << "Successfully loaded classifier!";
            }
            else {
                qDebug() << "Could not load classifier.";
            }
        }
        else {
            qDebug() << "Can't open temp file.";
        }
    }
    else {
        qDebug() << "Can't open XML.";
    }

    //load template
    m_templateImage.load(":/data/template.jpg");
    qDebug() << "loadTemplateImage, image loaded" << m_templateImage.size();

    if (!m_templateImage.isNull()) {
        cv::Mat mat(m_templateImage.height(),
            m_templateImage.width(),
            CV_8UC4,
            m_templateImage.bits(),
            m_templateImage.bytesPerLine());

        cv::resize(mat, mat, cv::Size(64, 64), cv::INTER_AREA);

        setTemplateMat(mat);

        //check the real template after resizing
//        saveImage(m_templateMat, "test.jpg");
    }
    else {
        qDebug() << "Can't open template image.";
    }

}

QImage DetectionFilter::templateImage() const
{
    return m_templateImage;
}

void DetectionFilter::setTemplateImage(QImage templateImage)
{
    if (m_templateImage == templateImage)
        return;

    m_templateImage = templateImage;
    emit templateImageChanged(m_templateImage);
}

cv::Mat DetectionFilter::templateMat() const
{
    return m_templateMat;
}

void DetectionFilter::setTemplateMat(cv::Mat templateMat)
{
    m_templateMat = templateMat.clone();
    emit templateMatChanged(m_templateMat);
}

void  DetectionFilter::setOrientation(int orientation)
{
    if (m_orientation == orientation)
        return;

    m_orientation = orientation;
    emit orientationChanged(m_orientation);
}

cv::CascadeClassifier DetectionFilter::classifier() const
{
    return m_classifier;
}

void  DetectionFilter::setClassifier(cv::CascadeClassifier classifier)
{
    m_classifier = classifier;
    emit classifierChanged(m_classifier);
}

DetectionFilterRunnable::DetectionFilterRunnable(DetectionFilter *creator)
{
    m_filter = creator;
}

QVideoFrame DetectionFilterRunnable::run(QVideoFrame* input, const QVideoSurfaceFormat& surfaceFormat, RunFlags flags)
{
    Q_UNUSED(flags);

    input->map(QAbstractVideoBuffer::ReadOnly);

    if (m_filter->classifier().empty()) {
        qDebug() << "classifier not loaded";
    }

    if (true || surfaceFormat.handleType() == QAbstractVideoBuffer::NoHandle)
    {        
        QImage _image;

        if (input->pixelFormat() == QVideoFrame::Format_BGR32) {

            _image =  QImage(input->bits(),
                             input->width(),
                             input->height(), QImage::Format_RGB32);

            _image = _image.rgbSwapped();
        }
        else {
            _image = QImage(input->bits(),
                            input->width(),
                            input->height(),
                            QVideoFrame::imageFormatFromPixelFormat(input->pixelFormat()));
        }

        QImage image = _image;
        m_filter->setLastImage(image);

        cv::Mat mat(image.height(),
            image.width(),
            CV_8UC4,
            image.bits(),
            image.bytesPerLine());

        cv::flip(mat, mat, 0);

        int baseX = 160;
        int baseY = 120;

        QSize resized = image.size().scaled(baseX*(m_filter->quality()+1), baseY*(m_filter->quality()+1), Qt::KeepAspectRatio);
        cv::resize(mat, mat, cv::Size(resized.width(), resized.height()), cv::INTER_AREA);

        if (m_filter->orientation() == Qt::PortraitOrientation) {
            cv::rotate(mat, mat, cv::ROTATE_90_CLOCKWISE);
        }
        else if (m_filter->orientation() == Qt::InvertedLandscapeOrientation) {
            cv::rotate(mat, mat, cv::ROTATE_180);
        }
        else if (m_filter->orientation() == Qt::InvertedPortraitOrientation) {
            cv::rotate(mat, mat, cv::ROTATE_90_COUNTERCLOCKWISE);
        }

        m_filter->setLastMat(mat);

        std::vector<cv::Rect> detected;
        if (m_filter->classifierType() == DetectionFilter::Classifier_HaarCascade) {
            detected = runHaarCascade(mat);
        } else if (m_filter->classifierType() == DetectionFilter::Classifier_MatchTemplate) {
            detected = runMatchTemplate(mat, m_filter->templateMat().clone());
        }

        //working with result
        QVariantList rectlist;
        if (detected.size() > 0) {
            // Normalize x,y,w,h to values between 0..1 and send them to UI
            for (cv::Rect item: detected) {
                rectlist << QRectF{
                          float(item.x) / float(mat.cols),
                          float(item.y) / float(mat.rows),
                          float(item.width) / float(mat.cols),
                          float(item.height) / float(mat.rows)};
                qDebug() << "detected " <<  item.x << item.y << item.width << item.height << mat.cols << mat.rows;
            }
            qDebug() << "detected " <<  rectlist;
            emit m_filter->objectDetected(rectlist);
        }
        else {
            emit m_filter->objectDetected(rectlist);
        }
    }

    input->unmap();
    return *input;
}

std::vector<cv::Rect> DetectionFilterRunnable::runHaarCascade(const cv::Mat &mat)
{
    std::vector<cv::Rect> detected;

    m_filter->classifier().detectMultiScale(mat, detected, 1.1, 3);

    return detected;
}

std::vector<cv::Rect> DetectionFilterRunnable::runMatchTemplate(const cv::Mat &mat, const cv::Mat &templ)
{
    std::vector<cv::Rect> detected;

    if (templ.cols == 0 || templ.rows == 0) {
        return detected;
    }

    cv::Mat result;

    int result_cols =  mat.cols - templ.cols + 1;
    int result_rows = mat.rows - templ.rows + 1;

    int match_method = cv::TM_CCOEFF;

    result.create( result_rows, result_cols, CV_32FC1 );
    cv::matchTemplate(mat, templ, result, match_method);

    normalize( result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );

    // Localizing the best match with minMaxLoc
    double minVal;
    double maxVal;
    cv::Point minLoc;
    cv::Point maxLoc;
    cv::Point matchLoc;

    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

    // For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if( match_method  == cv::TM_SQDIFF || match_method == cv::TM_SQDIFF_NORMED ) {
        matchLoc = minLoc;
    }
    else {
        matchLoc = maxLoc;
    }

    cv::Rect res;
    res.x = matchLoc.x;
    res.y = matchLoc.y;
    res.width = templ.cols;
    res.height = templ.rows;

    detected.push_back(res);

    return detected;
}
