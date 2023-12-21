//
// Volume Data Visualizer for Google Earth
//
// earth.h
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

#ifndef EARTH_H
#define EARTH_H

#include "common.h"
#include "../src/define.h"
#include <QtGui>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#else
#include <QGLWidget>
#endif

class Earth : public QGLWidget
{
	Q_OBJECT

public:
	Earth( QWidget *parent = 0 );
	~Earth();

	void readTexture();
	void draw();

private:
	GLUquadricObj *m_Earth;
	GLuint m_TexEarth;
};

#endif // EARTH_H
