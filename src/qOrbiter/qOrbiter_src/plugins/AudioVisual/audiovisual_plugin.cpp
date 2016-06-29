/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
    Author: Langston Ball
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "audiovisual_plugin.h"
#include "mediamanager.h"
#ifdef RPI
#include "mediamanager.h"
#else
#include "mediamanager.h"
#endif


void AudioVisualPlugin::registerTypes(const char *uri)
{
#ifndef QT5
    // @uri AudioVisual
#endif
    Q_ASSERT(uri==QLatin1String("AudioVisual"));
    qmlRegisterType<MediaManager>(uri, 1, 0, "MediaManager");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(AudioVisual, AudioVisualPlugin)
#endif

