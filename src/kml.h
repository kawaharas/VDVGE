//
// Volume Data Visualizer for Google Earth
//
// kml.h
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

#ifndef KML_H
#define KML_H

#include <QtGui>
#include <QProgressDialog>

#include "define.h"
#include "util.h"
#include "grads.h"
#include "parameter.h"
#include "interpolator.h"

extern Grads grads;
extern Parameter param;

enum ObjectType {
	NO_OBJECT,
	COLORSLICE_X,
	COLORSLICE_Y,
	COLORSLICE_Z,
	CONTOURLINE_X,
	CONTOURLINE_Y,
	CONTOURLINE_Z,
	VOLUME_X,
	VOLUME_Y,
	VOLUME_Z
};

enum AltitudeMode {
	ALTITUDE_ABSOLUTE,
	ALTITUDE_CLAMP_TO_GROUND,
	ALTITUDE_RELATIVE_TO_GROUND
};

class KML : public QObject
{
	Q_OBJECT

public:
	KML();

	void enableGUI() { m_GUI = true; }
	void disableGUI() { m_GUI = false; }
	bool exportKML();

/*
public slots:
	bool exportKML();
*/

private:
	QString m_DirPath;
	QTextStream *m_Stream;

	QVector<QRgb> m_ColorTable;		// for Volume Rendering
	QVector<QRgb> m_ColorTableRGBA;	// for Color Slice with Opacity
	QVector<QRgb> m_ColorTableRGB;	// for Color Slice with Opacity

	QProgressDialog *m_Progress;

	bool m_GUI;

	void initialize();
	void exportVolume();
	void exportVolumeX();
	void exportVolumeY();
	void exportVolumeZ();
	void exportColorSlice();
	void exportColorSliceX();
	void exportColorSliceY();
	void exportColorSliceZ();
	void exportContourLine();
	void exportContourLineX();
	void exportContourLineY();
	void exportContourLineZ();
	void exportBoundingBox();
	void exportColorBar();
	void exportLogo();
	void setStateAltitudeModeOfColorSliceZ();

protected:
	uchar float2uchar( float );
	void clearDir( QString );

	void makeLine( QVector<QVector3D>& vertex,
				   QVector4D p0, QVector4D p1, QVector4D p2, float value );
	QVector3D crossPoint( QVector4D p0, QVector4D p1, float value );
	QVector< QVector<QVector3D> > reduction( QVector< QVector<QVector3D> >& lineData, int levelID );

	int m_SizeX;
	int m_SizeY;
	int m_SizeZ;
	double m_MinX;
	double m_MaxX;
	double m_MinY;
	double m_MaxY;
	double m_MinZ;
	double m_MaxZ;
	ObjectType m_MainObject;
	AltitudeMode m_AltitudeModeOfColorSliceZ;
//	AltitudeMode m_AltitudeModeOfVolumeZ;
};

class KMLSettingDialog : public QDialog
{
	Q_OBJECT

public:
	KMLSettingDialog( QWidget *parent = 0 );

private:
	QRadioButton *m_RadioButton_Absolute;
	QRadioButton *m_RadioButton_ClampToGround;
	QPushButton  *m_PushButton_Accept;

public slots:
	AltitudeMode getStateAltitudeMode();
};

#endif // KML_H
