//
// Volume Data Visualizer for Google Earth
//
// coastline.h
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

#ifndef COASTLINE_H
#define COASTLINE_H

#include "common.h"
#include "../src/define.h"
#include "../src/grads.h"
#include "../src/parameter.h"
#include <QtGui>
#include <QWidget>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#else
#include <QGLWidget>
#endif

extern Parameter param;
extern Grads grads;

class Coastline
{
public:
	Coastline( QGLWidget *parent = 0 );
	~Coastline();

	ErrorCode setData();
	void draw();

private:
	QGLWidget *m_QGLWidget;

	GLuint m_DisplayList;
};

#endif // COASTLINE_H
