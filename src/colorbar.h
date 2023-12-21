//
// Volume Data Visualizer for Google Earth
//
// colorbar.h
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

#ifndef COLORBAR_H
#define COLORBAR_H

#include "common.h"
#include "../src/define.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#else
#include <QGLWidget>
#include <QGLFramebufferObject>
#include <QGLFramebufferObjectFormat>
#endif

#include <QtGui>

#include "../src/grads.h"
#include "../src/parameter.h"
#include "app_parameter.h"

extern Grads grads;
extern Parameter param;
extern AppParameter appParam;

class ColorBar
{
public:
	ColorBar( QGLWidget *parent = 0 );

	void  initialize();
	void  draw();

private:
	float m_VPWidth;	// Width  of Viewport for ColorBar
	float m_VPHeight;	// Height of Viewport for ColorBar
	float m_OVPWidth;	// Width  of GLWidget's Viewport (Original Size)
	float m_OVPHeight;	// Height of GLWidget's Viewport (Original Size)
	float m_CBWidth;	// Width  of Colorbar
	float m_CBHeight;	// Height of Colorbar
	float m_Offset_X;
	float m_Offset_Y;

	ColorBarType m_Type;
	ColorBarOrientation m_Orientation;
	float m_Margin;
	float m_ColorBarBodyHeight;
	float m_MinorTickMarkLength;
	float m_MajorTickMarkLength;
	float m_LineSpace;

	void  drawTitle();
	void  drawColorBarBody();
	void  drawScaleMarkings();
	void  drawMajorTickMarkSet();
	void  drawMinorTickMarkSet();

protected:

	QGLWidget *m_QGLWidget;

	GLubyte *m_VTitle;
	GLuint m_Texture;
	GLuint m_FrameBuffer;
	GLuint m_RenderBuffer;
	bool   m_TextureCreated;
	GLint  m_Texture_Org;
	int    m_BitmapWidth;
	char   m_Str[128];

	QColor  m_Color;

	QString m_Description;

	void  drawTitleVertical();
	void  drawTitleHorizontal();
	void  drawColorBarBodyHorizontal();
	void  drawColorBarBodyVertical();
	void  drawScaleHorizontal( char *, float );
	void  drawScaleVertical( char *, float );
	void  drawMajorTickMarkHorizontal( float );
	void  drawMajorTickMarkVertical( float );
	void  drawMinorTickMarkHorizontal( float );
	void  drawMinorTickMarkVertical( float );

	void  drawHorizontalTitle();

	void  renderTextViaQImage( int, int, QString, QFont, float = 0.0 );
};

#endif // COLORBAR_H
