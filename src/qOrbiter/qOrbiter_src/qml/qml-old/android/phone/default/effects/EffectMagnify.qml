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

import QtQuick 1.0

Effect {
    id: root
    divider: false
    parameters: ListModel {
        ListElement {
            name: "radius"
            value: 0.5
        }
        ListElement {
            name: "diffraction"
            value: 0.5
        }
    }

    property real posX: -1
    property real posY: -1

    QtObject {
        id: d
        property real oldTargetWidth: root.targetWidth
        property real oldTargetHeight: root.targetHeight
    }

    // Transform slider values, and bind result to shader uniforms
    property real radius: parameters.get(0).value * 100
    property real diffractionIndex: parameters.get(1).value

    onTargetWidthChanged: {
        if (posX == -1)
            posX = targetWidth / 2
        else if (d.oldTargetWidth != 0)
            posX *= (targetWidth / d.oldTargetWidth)
        d.oldTargetWidth = targetWidth
    }

    onTargetHeightChanged: {
        if (posY == -1)
            posY = targetHeight / 2
        else if (d.oldTargetHeight != 0)
            posY *= (targetHeight / d.oldTargetHeight)
        d.oldTargetHeight = targetHeight
    }

    fragmentShaderFilename: "shaders/magnify.fsh"

    MouseArea {
        anchors.fill: parent
        onPositionChanged: { root.posX = mouse.x; root.posY = root.targetHeight - mouse.y }
    }
}
