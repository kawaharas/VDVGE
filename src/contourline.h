//
// Volume Data Visualizer for Google Earth
//
// contourline.h
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

#ifndef CONTOURLINE_H
#define CONTOURLINE_H

#include "common.h"
#include <QWidget>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#else
#include <QGLWidget>
#endif

#include "../src/grads.h"
#include "../src/parameter.h"
#include "texture3d.h"
#include "interpolator.h"

extern Grads grads;
extern Parameter param;
extern Texture3D texture3d;

class ContourLine
{
public:
	void draw();
	void drawSphereData();
	void initialize();
	void recalc();
	void setLevels();

private:
	QVector< QVector< QVector<QVector3D> > > m_LineX;
	QVector< QVector< QVector<QVector3D> > > m_LineY;
	QVector< QVector< QVector<QVector3D> > > m_LineZ;
	QVector< QVector< QVector<QVector3D> > > m_LineXSpherical;
	QVector< QVector< QVector<QVector3D> > > m_LineYSpherical;
	QVector< QVector< QVector<QVector3D> > > m_LineZSpherical;
	QVector<float> m_Level;
	QVector<QColor> m_LineColor;
	float   m_ThresholdMin;
	float   m_ThresholdMax;
	QVector<QVector3D>  m_Vertex; // buffer to store vertex

	void addLine( float level );
	void deleteLine( float level );

protected:
	void recalcX();
	void recalcY();
	void recalcZ();
	QVector< QVector< QVector< QVector3D > > >
		cartToSpherical( QVector< QVector< QVector<QVector3D> > >& );
	QVector3D crossPoint( QVector4D, QVector4D, float );
	void makeLine( QVector4D, QVector4D, QVector4D, float );
	void drawMain( QVector< QVector< QVector<QVector3D> > >& lineData );
	QVector< QVector< QVector<QVector3D> > >
		reduction( QVector< QVector<QVector3D> >& lineData );

	float  *m_Data;
	double  m_MinX, m_MaxX, m_MinY, m_MaxY, m_MinZ, m_MaxZ;
	float   m_Undef;
	int     m_SizeX, m_SizeY, m_SizeZ;
};

#endif // CONTOURLINE_H
