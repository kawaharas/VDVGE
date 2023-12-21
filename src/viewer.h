//
// Volume Data Visualizer for Google Earth
//
// viewer.h
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

#ifndef VIEWER_H
#define VIEWER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "common.h"

#include <QtGui>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#else
#include <QGLWidget>
#endif

#include "../src/define.h"
#include "util.h"
#include "../src/grads.h"
#include "../src/parameter.h"

#include "boundingbox.h"
#include "colorbar.h"
#include "earth.h"
#include "logo.h"
#include "texture3d.h"
#include "colortable.h"
#include "colorslice.h"
#include "contourline.h"
#include "volumerender.h"
#include "kml.h"
#include "timedisplay.h"
#include "topo.h"
#include "coastline.h"

extern Grads grads;
extern Parameter param;

class Viewer : public QGLWidget {

	Q_OBJECT

public:
	Viewer( QWidget *parent = 0 );
	~Viewer();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	QOpenGLFunctions *m_GLFunc;
	QOpenGLFunctions* getGLFunc() { return m_GLFunc; }
#endif

public slots:
	void setGradsData();
	void setCurrentData();
	void setColorTable();

	void animate( int time );
	void toggleAutoRotation() { m_IsAutoRotation = !m_IsAutoRotation; }
	void setViewMode( bool state ) { m_IsSphereView = state; }
	void setViewDirection( ViewDirection );
	void setViewDirectionDefault();
	void setViewDirectionFront();
	void setViewDirectionRear();
	void setViewDirectionTop();
	void setViewDirectionBottom();
	void setViewDirectionLeft();
	void setViewDirectionRight();
	void toggleProjection();

	void setLogoTexture();

	bool exportKML();
	bool renderToPNG( QString dirPath, int time );

	void valueChanged( ValueType );

protected:
	void initializeGL();
	void resizeGL( int w, int h );
	void paintGL();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	void updateGL() { update(); }
#endif

	void setupViewport( int w, int h );
	void backupViewParameters();
	void restoreViewParameters();

	void mousePressEvent( QMouseEvent *event );
	void mouseReleaseEvent( QMouseEvent *event );
	void mouseMoveEvent( QMouseEvent *event );
	void mouseDoubleClickEvent( QMouseEvent *event );
	void wheelEvent( QWheelEvent *event );
	void showEvent( QShowEvent *event );

	float m_ObjectPosX_Backup;
	float m_ObjectPosY_Backup;
	float m_ObjectPosZ_Backup;
	float m_ObjectAnglePitch_Backup;
	float m_ObjectAngleYaw_Backup;

private:
	GLfloat m_LightPosition[4];

	GLfloat m_CurrentAngle;
	float   m_ScaleX;
	float   m_ScaleY;
	float   m_ScaleZ;
	float   m_ScaleZ_SingleLayer;
	float   m_OrthoScale;

	ColorSlice   *m_ColorSlice;
	ContourLine  *m_ContourLine;
	VolumeRender *m_VolumeRender;

	TimeDisplay  *m_TimeDisplay;
	BoundingBox  *m_BoundingBox;
	Earth        *m_Earth;
	Logo         *m_Logo;
	ColorBar     *m_ColorBar;

	Topo         *m_Topo;
	Coastline    *m_Coastline;

	QPoint  m_LastMousePosition;

	bool    m_IsDataLoaded;
	bool    m_IsAutoRotation;
	bool    m_IsSphereView;

	ViewDirection m_ViewDirection;
};

#endif // VIEWER_H
