#ifndef MOTIONPICAMERA_H
#define MOTIONPICAMERA_H
#include "abstractpicamera.h"
#include "qnetworkaccessmanager.h"
class QVariant;

//picture save
class MotionPiCamera : public NvrCameraBase
{
public:
    MotionPiCamera(QString cameraName,
            QString userName,
            QString password,
            quint16 port,
            quint16 control_port,
            QUrl url,
            int dceId,
            CameraType t= PI_MOTION_STREAM, AudioType a =NO_AUDIO, QObject *parent=0);

    enum DetectionCommand{
        COMMAND_PAUSE,
        COMMAND_START,
        COMMAND_STATUS,
        COMMAND_CONNECTION
    };

    static const QString WRITE_SETTINGS;
    static const QString ALL_CAMERAS;
    static const QString CAMERA_DETECTION_STATUS;
    static const QString CAMERA_DETECTION_PAUSE;
    static const QString CAMERA_DETECTION_START;
    static const QString CAMERA_DETECTION_CONNECTION;

    static const QString CAMERA_CONFIG_WIDTH;
    static const QString CAMERA_CONFIG_HEIGHT;
    static const QString CAMERA_CONFIG_TARGET_DIR;
    static const QString CAMERA_CONFIG_SNAPSHOT_FILENAME;
    static const QString CAMERA_CONFIG_JPG_FILENAME;
    static const QString CAMERA_CONFIG_TIMELAPSE_FILENAME;
    static const QString CAMERA_CONFIG_LOCATE;
    static const QString CAMERA_CONFIG_ROTATE;

    static const QString ON_EVENT_START;
    static const QString ON_EVENT_END;
    static const QString ON_PICTURE_SAVE;
    static const QString ON_MOTION_DETECTED;
    static const QString ON_MOVIE_START;
    static const QString ON_MOVIE_END;
    static const QString ON_CAMERA_LOST;


    static const QString CURL_EVENT_START;
    static const QString CURL_EVENT_END;
    static const QString CURL_MOTION_DETECTED;
    static const QString CURL_SEND_PICTURE;

signals:
    void motionStarted();
    void motionEnded();
    void newPicture();
    void newMovie();


private slots:
    void setConnections();
    void testControlPort();
    void handleControlReply(QNetworkReply*p);
    void handleRequest(QNetworkRequest q);
    void parseStream(QByteArray st);
    void refreshImage();
public slots:
    void setCameraType(CameraType t);
    void setAudioType(AudioType a);
    void sendDetectionCommand(DetectionCommand d);
    void getMotionSetting(QString s);
    void setMotionSetting(QString Setting, QVariant val);
    void writeSettings();
    std::string getImage();

private:
    QNetworkAccessManager *httpManager;
    QByteArray d_array;


private:




};

#endif // MOTIONPICAMERA_H
