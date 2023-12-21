//
// Volume Data Visualizer for Google Earth
//
// topo.h
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

#ifndef TOPO_H
#define TOPO_H

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

//static const int LON = 4320; // ETOPO5
//static const int LAT = 2160; // ETOPO5
//static const int LON = 10801; // ETOPO2
//static const int LAT =  5401; // ETOPO2
//static const int LON = 21601; // ETOPO1
//static const int LAT = 10801; // ETOPO1

extern Parameter param;
extern Grads grads;

class Topo : public QGLWidget
{
	Q_OBJECT

public:
	Topo( QWidget *parent = 0 );
	~Topo();

	void initialize();
	void setData();
	void draw();

private:
	int      m_LON;
	int      m_LAT;

	QVector<QVector3D> m_Vertex;
	QVector<QVector3D> m_Normal;
	QVector<QVector4D> m_Color;
	QVector<uint>      m_Index;

	double   m_North;
	double   m_South;
	double   m_West;
	double   m_East;

	int LON, LAT;


protected:
	void addElement( float, float, float );
};

#endif // TOPO_H
