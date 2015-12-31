/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "filereader.h"
#include "trace.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QDir>
#include <QDebug>

QString adjustPath(const QString &path)
{
#ifdef ANDROID

    return path;

#endif

#ifdef Q_OS_UNIX
#ifdef Q_OS_MAC
    if (!QDir::isAbsolutePath(path))
        return QCoreApplication::applicationDirPath()
                + QLatin1String("/../Resources/") + path;
#else
    QString pathInInstallDir;
    const QString applicationDirPath = QCoreApplication::applicationDirPath();
    pathInInstallDir = QString::fromLocal8Bit("%1/../%2").arg(applicationDirPath, path);

    if (QFileInfo(pathInInstallDir).exists())
        return pathInInstallDir;
#endif
#endif
    return path;
}

QString FileReader::readFile(const QString &fileName)
{
#ifndef ANDROID
    qDebug() << "FileReader::readFile" << "fileName" << fileName << "adjusted" << adjustPath(fileName);
    QString content;
    QFile file(QCoreApplication::applicationDirPath()+"/qml/desktop/default/effects/"+fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        content = stream.readAll();
        qDebug("Opened file");
    }
    else
    {
        qDebug() << "Couldnt open"  << file.fileName();
    }
    return content;
#else

    qDebug() << "FileReader::readFile" << "fileName" << fileName << "adjusted" << adjustPath(fileName);
    QFile file("assets:/qml/android/phone/default/effects/"+fileName);
//     QFile file(QCoreApplication::applicationDirPath()+"/qml/android/phone/default/effects/"+fileName);
     QString content;
     if (file.open(QIODevice::ReadOnly)) {
         QTextStream stream(&file);
         content = stream.readAll();
         qDebug("Opened file");
         return content;
     }
     else
     {
         qDebug() << "Couldnt open"  << file.fileName();
         return content.append("fileErr");
     }
#endif
}


