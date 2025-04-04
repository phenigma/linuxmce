#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>

#include <mythdirs.h>
#include <mythdb.h>
#include <mythcontext.h>
#include <netgrabbermanager.h>

#include "netcommon.h"

QString GetThumbnailFilename(QString url, QString title)
{
    QString fileprefix = GetConfDir();

    QDir dir(fileprefix);
    if (!dir.exists())
        dir.mkdir(fileprefix);

    fileprefix += "/MythNetvision";

    dir = QDir(fileprefix);
    if (!dir.exists())
        dir.mkdir(fileprefix);

    fileprefix += "/thumbcache";

    dir = QDir(fileprefix);
    if (!dir.exists())
        dir.mkdir(fileprefix);

    QString sFilename = QString("%1/%2_%3")
        .arg(fileprefix)
        .arg(qChecksum(url.toLocal8Bit().constData(),
                       url.toLocal8Bit().size()))
        .arg(qChecksum(title.toLocal8Bit().constData(),
                       title.toLocal8Bit().size()));
    return sFilename;
}

QString GetMythXMLURL(void)
{
    QString MasterIP = gCoreContext->GetMasterServerIP();
    int MasterStatusPort = gCoreContext->GetMasterServerStatusPort();

    return QString("http://%1:%2/InternetContent/").arg(MasterIP).arg(MasterStatusPort);
}

QUrl GetMythXMLSearch(QString url, QString query, QString grabber,
                         QString pagenum)
{
    QString tmp = QString("%1GetInternetSearch?Query=%2&Grabber=%3&Page=%4")
        .arg(url).arg(query).arg(grabber).arg(pagenum);
    return QUrl(tmp);
}
