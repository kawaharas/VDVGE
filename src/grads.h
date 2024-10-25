//
// Volume Data Visualizer for Google Earth
//
// grads.h
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

#ifndef GRADS_H
#define GRADS_H

//#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <string>
#include <QWidget>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtGui>
#else
#include <QtWidgets>
#endif

#include <src/app_parameter.h>
#include <src/errorcode.h>

#define GET_TOKEN QString::fromStdString( strtok( NULL, " " ) )

using namespace std;

typedef struct {
	QString name;
	int     level;
	QString units;
	QString description;
} VarInfomation;

typedef struct {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
} TimeDef;

typedef struct {
	struct tm stime;
	TimeDef   itime;
	int startYear;
	bool isAfter1900;
} TimeInfo;

class Grads
{
public:
	Grads();
	~Grads();

	ErrorCode openFile( const QString &filename );
	bool      setData( int time );

	QString   getCtlFileName() { return m_FileNameCtl; }
	QString   getDataFileName() { return m_FileNameData; }
	int       getGridSizeX() { return m_GridSizeX; }
	int       getGridSizeY() { return m_GridSizeY; }
	int       getGridSizeZ() { return m_GridSizeZ; }
	int       getGridSizeT() { return m_GridSizeT; }
	double    getGridXMin() { return m_CoordX[0]; }
	double    getGridXMax() { return m_CoordX[ m_GridSizeX - 1 ]; }
	double    getGridYMin() { return m_CoordY[0]; }
	double    getGridYMax() { return m_CoordY[ m_GridSizeY - 1 ]; }
	double    getGridZMin() { return m_CoordZ[0]; }
	double    getGridZMax() { return m_CoordZ[ m_GridSizeZ - 1 ]; }
	double    getCoordX( int n ) { return m_CoordX[ n ]; }
	double    getCoordY( int n ) { return m_CoordY[ n ]; }
	double    getCoordZ( int n ) { return m_CoordZ[ n ]; }
	float     getMin() { return m_Min; }
	float     getMax() { return m_Max; }
	float     getUndef() { return m_Undef; }
	float    *getData() { return m_Data; }
	TimeInfo  getTimeInfo() { return m_TimeInfo; }
	bool      isDataLoaded() { return m_IsDataLoaded; }
	QString   getVarDescription( int num ) { return m_VarInfo[ num ].description; }
	int       checkOption( QString option ) { return m_Option.indexOf( option ); }
	bool      isYRev() { return m_IsYRev; }
	bool      isZRev() { return m_IsZRev; }

private:
	void      checkMinMax();
	void      cleanupMemory();

	bool      m_IsDataLoaded;

	QString   m_FileNameCtl;
	QString   m_FileNameData;
	FILE     *m_DataFilePtr;
	float    *m_Data;

	int       m_GridSizeX;
	int       m_GridSizeY;
	int       m_GridSizeZ;
	int       m_GridSizeT;
	double   *m_CoordX;
	double   *m_CoordY;
	double   *m_CoordZ;
	double   *m_CoordT;
	QString   m_AxisFormatX;
	QString   m_AxisFormatY;
	QString   m_AxisFormatZ;
	QString   m_AxisFormatT;

	QVector<QString> m_Option;
	bool      m_IsYRev;
	bool      m_IsZRev;

	float     m_Min;
	float     m_Max;
	float     m_Undef;
	int       m_VarNum;
	QVector<VarInfomation> m_VarInfo;

	TimeInfo  m_TimeInfo;

	int       m_FileHeader;
	int       m_IsSequential;
};

#endif // GRADS_H
