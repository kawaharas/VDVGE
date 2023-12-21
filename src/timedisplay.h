//
// Volume Data Visualizer for Google Earth
//
// timedisplay.h
//
// Copyright (c) 2012-2015 Shintaro KAWAHARA (kawahara@jamstec.go.jp)
// Japan Agency for Marine-Earth Science and Technology (JAMSTEC)
// http://www.jamstec.go.jp
//
// This program is free software.
// You can redistribute it and/or modify it under the terms of the
// GNU General Public License version 3.0 as published by the Free
// Software Foundation and appearing in the file LICENSE.GPL included
// in the packaging of this file.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//

#ifndef TIMEDISPLAY_H
#define TIMEDISPLAY_H

#include "common.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#else
#include <QGLWidget>
#endif

#include "util.h"
#include "../src/parameter.h"
#include "app_parameter.h"

extern Parameter param;
extern AppParameter appParam;

class TimeDisplay
{
public:
	TimeDisplay( QGLWidget *parent = 0 );

	void draw();

private:
	QGLWidget *m_QGLWidget;

protected:
	void renderTextViaQImage( int, int, QString, QFont, QColor );
};

#endif // TIMEDISPLAY_H
