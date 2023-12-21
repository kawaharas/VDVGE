//
// Volume Data Visualizer for Google Earth
//
// boundingbox.h
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

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "common.h"
#include <string>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#else
#include <QGLWidget>
#endif

#include "../src/define.h"
#include "../src/grads.h"
#include "../src/parameter.h"

extern Grads grads;
extern Parameter param;

enum FaceType {
	FRONT_SIDE,
	BACK_SIDE
};

const GLdouble bbox_vert[][4][3] = {
	{ { -0.5, -0.5, -0.5 }, {  0.5, -0.5, -0.5 }, {  0.5, -0.5,  0.5 }, { -0.5, -0.5,  0.5 } },
	{ { -0.5, -0.5, -0.5 }, { -0.5,  0.5, -0.5 }, {  0.5,  0.5, -0.5 }, {  0.5, -0.5, -0.5 } },
	{ {  0.5, -0.5, -0.5 }, {  0.5,  0.5, -0.5 }, {  0.5,  0.5,  0.5 }, {  0.5, -0.5,  0.5 } },
	{ {  0.5, -0.5,  0.5 }, {  0.5,  0.5,  0.5 }, { -0.5,  0.5,  0.5 }, { -0.5, -0.5,  0.5 } },
	{ { -0.5, -0.5,  0.5 }, { -0.5,  0.5,  0.5 }, { -0.5,  0.5, -0.5 }, { -0.5, -0.5, -0.5 } },
	{ { -0.5,  0.5,  0.5 }, {  0.5,  0.5,  0.5 }, {  0.5,  0.5, -0.5 }, { -0.5,  0.5, -0.5 } },
};

static const GLdouble bbox_norm[][3] = {
	{  0.0, -1.0,  0.0 },
	{  0.0,  0.0, -1.0 },
	{  1.0,  0.0,  0.0 },
	{  0.0,  0.0,  1.0 },
	{ -1.0,  0.0,  0.0 },
	{  0.0,  1.0,  0.0 },
};

class BoundingBox
{
public:
	BoundingBox( QGLWidget *parent = 0 );

	enum AxisType {
		AXIS_LAT,
		AXIS_LON,
		AXIS_HEIGHT
	};

	void draw( FaceType facetype );
	void drawSphereBoundingBox();

protected:
	QGLWidget *m_QGLWidget;

	void drawText( GLfloat, GLfloat, GLfloat, double );
	void drawSphereParts( float );
	void drawAxisLabelAndLine();
	void drawAxisLabelAndLineParts( AxisType, ViewDirection );

	void renderTextViaQImage( int, int, QString, QFont, float = 0.0 );

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	void renderText(double, double, double, QString, QFont);
	inline GLint project(
		GLdouble objx, GLdouble objy, GLdouble objz,
		const GLdouble model[16], const GLdouble proj[16],
		const GLint viewport[4],
		GLdouble * winx, GLdouble * winy, GLdouble * winz);
	inline void transformPoint(GLdouble out[4], const GLdouble m[16], const GLdouble in[4]);
#endif
};

#endif // BOUNDINGBOX_H
