/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

import QtQuick 1.1
import Qt.labs.shaders 1.0
Item{

    property bool divider: true
    property real dividerValue: 0.5

    property ListModel parameters: ListModel {
        ListElement {
            name: "radius"
            value: 0.5
        }
    }

    property alias targetWidth: verticalShader.targetWidth
    property alias targetHeight: verticalShader.targetHeight
    property alias source: verticalShader.source

    ShaderEffectItem {
        id: verticalShader
        anchors.fill:  parent
        dividerValue: parent.dividerValue
        property real blurSize: 1.0 * parent.parameters.get(0).value / targetHeight
        fragmentShader: "
uniform float dividerValue;
uniform float blurSize;
uniform sampler2D source;
uniform lowp float qt_Opacity;
varying vec2 qt_TexCoord0;

void main()
{
vec2 uv = qt_TexCoord0.xy;
vec4 c = vec4(0.0);
if (uv.x < dividerValue) {
    c += texture2D(source, uv - vec2(0.0, 4.0*blurSize)) * 0.05;
    c += texture2D(source, uv - vec2(0.0, 3.0*blurSize)) * 0.09;
    c += texture2D(source, uv - vec2(0.0, 2.0*blurSize)) * 0.12;
    c += texture2D(source, uv - vec2(0.0, 1.0*blurSize)) * 0.15;
    c += texture2D(source, uv) * 0.18;
    c += texture2D(source, uv + vec2(0.0, 1.0*blurSize)) * 0.15;
    c += texture2D(source, uv + vec2(0.0, 2.0*blurSize)) * 0.12;
    c += texture2D(source, uv + vec2(0.0, 3.0*blurSize)) * 0.09;
    c += texture2D(source, uv + vec2(0.0, 4.0*blurSize)) * 0.05;
} else {
    c = texture2D(source, qt_TexCoord0);
}
// First pass we don't apply opacity
gl_FragColor = c;
}
"
    }

    ShaderEffectItem{
        id: horizontalShader
        anchors.fill: parent
        dividerValue: parent.dividerValue
        property real blurSize: 4.0 * parent.parameters.get(0).value / parent.targetWidth
        fragmentShader: "
uniform float dividerValue;
uniform float blurSize;

uniform sampler2D source;
uniform lowp float qt_Opacity;
varying vec2 qt_TexCoord0;

void main()
{
    vec2 uv = qt_TexCoord0.xy;
    vec4 c = vec4(0.0);
    if (uv.x < dividerValue) {
        c += texture2D(source, uv - vec2(4.0*blurSize, 0.0)) * 0.05;
        c += texture2D(source, uv - vec2(3.0*blurSize, 0.0)) * 0.09;
        c += texture2D(source, uv - vec2(2.0*blurSize, 0.0)) * 0.12;
        c += texture2D(source, uv - vec2(1.0*blurSize, 0.0)) * 0.15;
        c += texture2D(source, uv) * 0.18;
        c += texture2D(source, uv + vec2(1.0*blurSize, 0.0)) * 0.15;
        c += texture2D(source, uv + vec2(2.0*blurSize, 0.0)) * 0.12;
        c += texture2D(source, uv + vec2(3.0*blurSize, 0.0)) * 0.09;
        c += texture2D(source, uv + vec2(4.0*blurSize, 0.0)) * 0.05;
    } else {
        c = texture2D(source, qt_TexCoord0);
    }
    gl_FragColor = qt_Opacity * c;
}
"
        source: horizontalShaderSource

        ShaderEffectSource {
            id: horizontalShaderSource
            sourceItem: verticalShader
            smooth: true
            hideSource: true
        }
    }
}

