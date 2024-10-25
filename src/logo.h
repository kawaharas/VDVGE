//
// Volume Data Visualizer for Google Earth
//
// logo.h
//
// Copyright (c) 2012 Shintaro KAWAHARA (kawahara@jamstec.go.jp)
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

#ifndef LOGO_H
#define LOGO_H

#include "common.h"
#include "define.h"
#include "parameter.h"
#include <QtGui>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#else
#include <QGLWidget>
#endif

const float MIN_LOGO_WIDTH = 100.0;

extern Parameter param;

class Logo
{
public:
	Logo( QGLWidget *parent = 0 );
	~Logo();

	void initialize();
	void readTexture();
	void readTexture( QString );
	void draw();

private:
	QString m_LogoFileName;
	GLuint  m_TexLogo;
	float   m_TexLogoWidth;
	float   m_TexLogoHeight;
	float   m_VPHeight;	// height of GLWidget's Viewport
	float   m_VPWidth;	// width  of GLWidget's Viewport

protected:
	QGLWidget *m_QGLWidget;
};

#endif // LOGO_H
