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

import QtQuick 2.0
import Qt.labs.shaders 1.0

ShaderEffectItem {
    property variant source
    property ListModel parameters: ListModel { }
    property bool divider: false
    property real dividerValue: 1
    property real targetWidth: source.sourceItem.width
    property real targetHeight: source.sourceItem.height
    property string fragmentShaderFilename
    property string vertexShaderFilename


    QtObject {
        id: d
        property string fragmentShaderCommon: "
            #ifdef GL_ES
                precision mediump float;
            #else
            #   define lowp
            #   define mediump
            #   define highp
            #endif // GL_ES
        "
    }

    // The following is a workaround for the fact that ShaderEffectItem
    // doesn't provide a way for shader programs to be read from a file,
    // rather than being inline in the QML file

    onFragmentShaderFilenameChanged:
    {   console.log(fragmentShaderFilename)
        fragmentShader =  d.fragmentShaderCommon+fileReader.readFile(fragmentShaderFilename)
    }
    onVertexShaderFilenameChanged:
        vertexShader =d+fileReader.readFile(vertexShaderFilename)
}
