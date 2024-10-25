//
// Volume Data Visualizer for Google Earth
//
// parameter.h
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

#ifndef PARAMETER_H
#define PARAMETER_H

#include <QWidget>
#include <QtGui>

#include "grads.h"

const float OBJECT_POS_X = 0.0;
const float OBJECT_POS_Y = 0.0;
const float OBJECT_POS_Z = 0.0;
const float OBJECT_ANGLE_PITCH = 30.0f;
const float OBJECT_ANGLE_YAW = 330.0f;
const float OBJECT_ANGLE_ROLL = 0.0;
const float OBJECT_POS_SPHERE_X = 0.0;
const float OBJECT_POS_SPHERE_Y = 0.0;
const float OBJECT_POS_SPHERE_Z = 1.2f;
const int COLORBAR_DECIMAL_PLACES_NUM = 3;
const int COLORBAR_MAJOR_TICKMARK_NUM = 5;
const int COLORBAR_MINOR_TICKMARK_NUM = 1;

const int KML_MAX_IMAGE_SIZE_X = 600;
const int KML_MAX_IMAGE_SIZE_Y = 600;
const int KML_MAX_IMAGE_SIZE_Z = 600;

class BoundingBoxParam
{
public:
	enum LType { // Line Type
		Solid,
		Dotted,
		Dashed
	};

	enum TMType { // TickMark Type
		InSide,
		OutSide
	};

	void initialize()
	{
		m_Base = 0.0;
		m_Interval = 10.0;
		m_Precision = 0;
		m_MinorTickMarks = 0;
		m_MajorTickMarkType = OutSide;
		m_MinorTickMarkType = OutSide;
		m_MajorLineType = Dotted;
		m_MinorLineType = Dotted;
		m_MajorLine = true;
		m_MinorLine = true;
	}

	void   save();
	void   load();

	// getter and setter
	double base() { return m_Base; }
	void   base( float value ) { m_Base = value; }
	double interval() { return m_Interval; }
	void   interval( float value ) { m_Interval = value; }
	int    precision() { return m_Precision; }
	void   precision( int value ) { m_Precision = value; }
	int    minorTickMarks() { return m_MinorTickMarks; }
	void   minorTickMarks( int value ) { m_MinorTickMarks = value; }
	int    majorTickMarkType() { return m_MajorTickMarkType; }
	void   majorTickMarkType( TMType type ) { m_MajorTickMarkType = type; }
	int    minorTickMarkType() { return m_MinorTickMarkType; }
	void   minorTickMarkType( TMType type ) { m_MinorTickMarkType = type; }
	int    majorLineType() { return m_MajorLineType; }
	void   majorLineType( LType type ) { m_MajorLineType = type; }
	int    minorLineType() { return m_MinorLineType; }
	void   minorLineType( LType type ) { m_MinorLineType = type; }
	bool   majorLine() { return m_MajorLine; }
	void   majorLine( bool state ) { m_MajorLine = state; }
	bool   minorLine() { return m_MinorLine; }
	void   minorLine( bool state ) { m_MinorLine = state; }

private:
	double m_Base;
	double m_Interval;
	int    m_Precision;
	int    m_MinorTickMarks;
	TMType m_MajorTickMarkType;
	TMType m_MinorTickMarkType;
	LType  m_MajorLineType;
	LType  m_MinorLineType;
	bool   m_MajorLine;
	bool   m_MinorLine;
};


enum Projection
{
	PERSPECTIVE,
	ORTHO
};

enum ViewDirection
{
	VIEW_DEFAULT,
	VIEW_FRONT,
	VIEW_REAR,
	VIEW_TOP,
	VIEW_BOTTOM,
	VIEW_LEFT,
	VIEW_RIGHT
};

enum ZUnit
{
	ZUNIT_METER,
	ZUNIT_KILOMETER
};

enum ColorBarOrientation
{
	HORIZONTAL,
	VERTICAL
};

enum ColorBarType
{
	COLORBAR_RGB,
	COLORBAR_RGBA,
	COLORBAR_RGB_AND_RGBA
};

enum ColorBarTickMarkType
{
	TICKMARK_NONE,
	TICKMARK_OUTSIDE,
	TICKMARK_INSIDE,
	TICKMARK_CROSS
};

enum ColorType
{
	RED,
	GREEN,
	BLUE,
	ALPHA
};

enum ColorTableConnectionType {
	LINEAR_CONNECTION,
	CURVE_CONNECTION
};

enum TopoType
{
	TOPO_NONE,
	TOPO_ETOPO5,
	TOPO_ETOPO2,
	TOPO_ETOPO1
};

extern Grads grads;

class Parameter : public QObject
{
	Q_OBJECT

public:
	Parameter();

	void  initialize();
	bool  save( const QString &filename );
	ErrorCode load( const QString &filename );

	bool  saveColorTable( const QString &filename );
	ErrorCode loadColorTable( const QString &filename );

	int   getCurrentStep() { return m_CurrentStep; }
	int   getTimeBegin() { return m_TimeBegin; }
	int   getTimeEnd() { return m_TimeEnd; }
	int   getTimeSkip() { return m_TimeSkip; }
	void  setCurrentStep( int value ) { m_CurrentStep = value; }
	void  setTimeBegin( int value ) { m_TimeBegin = value; }
	void  setTimeEnd( int value ) { m_TimeEnd = value; }
	void  setTimeSkip( int value ) { m_TimeSkip = value; }
	bool  IsTimeConversionToUTC() { return m_StateTimeConversionToUTC; }
	void  setStateTimeConversionToUTC( bool state ) { m_StateTimeConversionToUTC = state; }
	QString getTimeConversionToUTC() { return m_TimeConversionToUTC; }
	void setTimeConversionToUTC( QString str ) { m_TimeConversionToUTC = str; }

	void  setZUnit( ZUnit kind ) { m_ZUnit = kind; }
	ZUnit getZUnit() { return m_ZUnit; }
	void  setZScale( int value ) { m_ZScale = value; }
	int   getZScale() { return m_ZScale; }

	bool  IsColorSliceX() { return m_StateColorSliceX; }
	bool  IsColorSliceY() { return m_StateColorSliceY; }
	bool  IsColorSliceZ() { return m_StateColorSliceZ; }
	void  setStateColorSliceX( bool state ) { m_StateColorSliceX = state; }
	void  setStateColorSliceY( bool state ) { m_StateColorSliceY = state; }
	void  setStateColorSliceZ( bool state ) { m_StateColorSliceZ = state; }
	void  setColorSlicePosX( float value ) { m_ColorSlicePosX = value; }
	void  setColorSlicePosY( float value ) { m_ColorSlicePosY = value; }
	void  setColorSlicePosZ( float value ) { m_ColorSlicePosZ = value; }
	float getColorSlicePosX() { return m_ColorSlicePosX; }
	float getColorSlicePosY() { return m_ColorSlicePosY; }
	float getColorSlicePosZ() { return m_ColorSlicePosZ; }
	bool  IsColorSliceAlpha() { return m_StateColorSliceAlpha; }
	void  setStateColorSliceAlpha( bool state ) { m_StateColorSliceAlpha = state; }
	void  setColorSliceOpacity( float value ) { m_ColorSliceOpacity = value; }
	float getColorSliceOpacity() { return m_ColorSliceOpacity; }
	bool  IsColorSliceClampToGround() { return m_StateColorSliceClampToGround; }
	void  setStateColorSliceClampToGround( bool state ) { m_StateColorSliceClampToGround = state; }

	bool  IsContourLineX() { return m_StateContourLineX; }
	bool  IsContourLineY() { return m_StateContourLineY; }
	bool  IsContourLineZ() { return m_StateContourLineZ; }
	void  setStateContourLineX( bool state ) { m_StateContourLineX = state; }
	void  setStateContourLineY( bool state ) { m_StateContourLineY = state; }
	void  setStateContourLineZ( bool state ) { m_StateContourLineZ = state; }
	void  setContourLinePosX( float value ) { m_ContourLinePosX = value; }
	void  setContourLinePosY( float value ) { m_ContourLinePosY = value; }
	void  setContourLinePosZ( float value ) { m_ContourLinePosZ = value; }
	float getContourLinePosX() { return m_ContourLinePosX; }
	float getContourLinePosY() { return m_ContourLinePosY; }
	float getContourLinePosZ() { return m_ContourLinePosZ; }
	void  setContourLineThresholdMin( float value ) { m_ContourLineThresholdMin = value; }
	void  setContourLineThresholdMax( float value ) { m_ContourLineThresholdMax = value; }
	float getContourLineThresholdMin() { return m_ContourLineThresholdMin; }
	float getContourLineThresholdMax() { return m_ContourLineThresholdMax; }
	void  setContourLineInterval( float value ) { m_ContourLineInterval = value; }
	float getContourLineInterval() { return m_ContourLineInterval; }
	void  setContourLineWidth( float value ) { m_ContourLineWidth = value; }
	float getContourLineWidth() { return m_ContourLineWidth; }

	bool  IsVolumeX() { return m_StateVolumeX; }
	bool  IsVolumeY() { return m_StateVolumeY; }
	bool  IsVolumeZ() { return m_StateVolumeZ; }
	void  setStateVolumeX( bool state ) { m_StateVolumeX = state; }
	void  setStateVolumeY( bool state ) { m_StateVolumeY = state; }
	void  setStateVolumeZ( bool state ) { m_StateVolumeZ = state; }
	void  setVolumeXNum( int num ) { m_VolumeXNum = num; }
	void  setVolumeYNum( int num ) { m_VolumeYNum = num; }
	void  setVolumeZNum( int num ) { m_VolumeZNum = num; }
	int   getVolumeXNum() { return m_VolumeXNum; }
	int   getVolumeYNum() { return m_VolumeYNum; }
	int   getVolumeZNum() { return m_VolumeZNum; }
	void  setVolumeThresholdMin( float value ) { m_VolumeThresholdMin = value; }
	void  setVolumeThresholdMax( float value ) { m_VolumeThresholdMax = value; }
	float getVolumeThresholdMin() { return m_VolumeThresholdMin; }
	float getVolumeThresholdMax() { return m_VolumeThresholdMax; }

	bool  IsColorBar() { return m_StateColorBar; }
	void  setStateColorBar( bool state ) { m_StateColorBar = state; }
	bool  IsBoundingBox() { return m_StateBoundingBox; }
	void  setStateBoundingBox( bool state ) { m_StateBoundingBox = state; }
	bool  IsTimeDisplay() { return m_StateTimeDisplay; }
	void  setStateTimeDisplay( bool state ) { m_StateTimeDisplay = state; }

	void  setColorBarType( ColorBarType type ) { m_ColorBarType = type; }
	void  setColorBarOrientation( ColorBarOrientation type ) { m_ColorBarOrientation = type; }
	void  setColorBarDecimalPlacesNum( int value ) { m_ColorBarDecimalPlacesNum = value; }
	void  setColorBarMajorTickMarkNum( int value ) { m_ColorBarMajorTickMarkNum = value; }
	void  setColorBarMinorTickMarkNum( int value ) { m_ColorBarMinorTickMarkNum = value; }
	void  setColorBarMajorTickMarkType( ColorBarTickMarkType type )
			{ m_ColorBarMajorTickMarkType = type; }
	void  setColorBarMinorTickMarkType( ColorBarTickMarkType type )
			{ m_ColorBarMinorTickMarkType = type; }
	ColorBarType getColorBarType() { return m_ColorBarType; }
	ColorBarOrientation getColorBarOrientation() { return m_ColorBarOrientation; }
	int   getColorBarDecimalPlacesNum() { return m_ColorBarDecimalPlacesNum; }
	int   getColorBarMajorTickMarkNum() { return m_ColorBarMajorTickMarkNum; }
	int   getColorBarMinorTickMarkNum() { return m_ColorBarMinorTickMarkNum; }
	ColorBarTickMarkType getColorBarMajorTickMarkType() { return m_ColorBarMajorTickMarkType; }
	ColorBarTickMarkType getColorBarMinorTickMarkType() { return m_ColorBarMinorTickMarkType; }

	void   setColorTable( ColorType type, int level, float value );
	float  getColorTable( ColorType type, int level );
	void   setColorTableR( int level, float value ) { m_ColorTableR[ level ] = value; }
	void   setColorTableG( int level, float value ) { m_ColorTableG[ level ] = value; }
	void   setColorTableB( int level, float value ) { m_ColorTableB[ level ] = value; }
	void   setColorTableA( int level, float value ) { m_ColorTableA[ level ] = value; }
	float  getColorTableR( int level ) { return m_ColorTableR[ level ]; }
	float  getColorTableG( int level ) { return m_ColorTableG[ level ]; }
	float  getColorTableB( int level ) { return m_ColorTableB[ level ]; }
	float  getColorTableA( int level ) { return m_ColorTableA[ level ]; }
	float* getColorTableR() { return m_ColorTableR; }
	float* getColorTableG() { return m_ColorTableG; }
	float* getColorTableB() { return m_ColorTableB; }
	float* getColorTableA() { return m_ColorTableA; }
	float* getColorTable( ColorType type );
	void   setColorTableConnectionType( ColorTableConnectionType type )
			{ m_ColorTableConnectionType = type; }
	ColorTableConnectionType getColorTableConnectionType() { return m_ColorTableConnectionType; }

	void   setObjectPos( float, float, float );
	void   setObjectPosSphere( float, float, float );
	void   setObjectAngle( float, float, float );
	void   setObjectAngleSphere( float, float, float );
	void   setObjectPos( int i, float value ) { m_ObjectPos[i] = value; }
	void   setObjectPosSphere( int i, float value ) { m_ObjectPosSphere[i] = value; }
	void   setObjectAngle( int i, float value ) { m_ObjectAngle[i] = value; }
	void   setObjectAngleSphere( int i, float value ) { m_ObjectAngleSphere[i] = value; }
	float  getObjectPosX() { return m_ObjectPos[0]; }
	float  getObjectPosY() { return m_ObjectPos[1]; }
	float  getObjectPosZ() { return m_ObjectPos[2]; }
	float  getObjectPosSphereX() { return m_ObjectPosSphere[0]; }
	float  getObjectPosSphereY() { return m_ObjectPosSphere[1]; }
	float  getObjectPosSphereZ() { return m_ObjectPosSphere[2]; }
	float  getObjectAnglePitch() { return m_ObjectAngle[0]; }
	float  getObjectAngleYaw() { return m_ObjectAngle[1]; }
	float  getObjectAngleSpherePitch() { return m_ObjectAngleSphere[0]; }
	float  getObjectAngleSphereYaw() { return m_ObjectAngleSphere[1]; }
	void   setObjectToCenter();

	void   setBGColor( QColor color ) { m_BGColor = color; }
	QColor getBGColor() { return m_BGColor; }

	void   setStateBoundingBoxAxisLabel( bool state ) { m_StateBoundingBoxAxisLabel = state; }
	bool   IsBoundingBoxAxisLabel() { return m_StateBoundingBoxAxisLabel; }

	void   setStateLogo( bool state ) { m_StateLogo = state; }
	bool   IsLogo() { return m_StateLogo; }
	void   setLogo( QString filename ) { m_LogoFileName = filename; }
	QString getLogo() { return m_LogoFileName; }
	void   setLogoAspectRatio( float value ) { m_LogoAspectRatio = value; }
	float  getLogoAspectRatio() { return m_LogoAspectRatio; }


	void   setStateLogScale( bool state ) { m_StateLogScale = state; }
	bool   IsLogScale() { return m_StateLogScale; }
	void   resetColorControlPoint();
	int    getColorControlPointNum( ColorType colortype );
	QPair<float, float> getColorControlPoint( ColorType colortype, int id );

	void   setColorControlPoint( ColorType colortype, QPair<float, float> controlpoint );
	void   setColorControlPoint(
		QVector< QPair<float, float> > red,
		QVector< QPair<float, float> > green,
		QVector< QPair<float, float> > blue,
		QVector< QPair<float, float> > alpha );

	void set256Table();

	void   setKMLMaxImageSize( int, int, int );
	int    getKMLMaxImageSizeX() { return m_KMLMaxImageSize[0]; }
	int    getKMLMaxImageSizeY() { return m_KMLMaxImageSize[1]; }
	int    getKMLMaxImageSizeZ() { return m_KMLMaxImageSize[2]; }

	void   setViewerSize( QSize size ) { m_ViewerSize = size; }
	QSize  getViewerSize() { return m_ViewerSize; }
	void   setMaximumViewerSize( QSize size ) { m_MaximumViewerSize = size; }
	QSize  getMaximumViewerSize() { return m_MaximumViewerSize; }
	void   setProjection( Projection type ) { m_Projection = type; }
	Projection getProjection() { return m_Projection; }
	void   setViewDirection( ViewDirection type ) { m_ViewDirection = type; }
	ViewDirection getViewDirection() { return m_ViewDirection; }

	bool     IsTopo() { return m_StateTopo; }
	bool     IsSeaSurface() { return m_StateSeaSurface; }
	bool     IsSyncScale() { return m_StateSyncScale; }
	void     setStateTopo( bool state ) { m_StateTopo = state; }
	void     setStateSeaSurface( bool state ) { m_StateSeaSurface = state; }
	void     setStateSyncScale( bool state ) { m_StateSyncScale = state; }
	void     setGroundScale( int value ) { m_GroundScale = value; }
	int      getGroundScale() { return m_GroundScale; }
	void     setSeaFloorScale( int value ) { m_SeaFloorScale = value; }
	int      getSeaFloorScale() { return m_SeaFloorScale; }
	TopoType getTopoType() { return m_TopoType; }
	void     setTopoType( TopoType type ) { m_TopoType = type; }

	void    setStateCoastline( bool state ) { m_StateCoastline = state; }
	bool    IsCoastline() { return m_StateCoastline; }
	void    setCoastline( QString filename ) { m_CoastlineFileName = filename; }
	QString getCoastline() { return m_CoastlineFileName; }

	BoundingBoxParam* boundingBox() { return m_BoundingBox; }

protected:
	float interpolation( ColorType type, float x );

private:
	// Z-Scale
	ZUnit m_ZUnit;
	int   m_ZScale;

	// Time Range
	int   m_CurrentStep;
	int   m_TimeBegin;
	int   m_TimeEnd;
	int   m_TimeSkip;
	bool  m_StateTimeConversionToUTC;
	QString m_TimeConversionToUTC;

	// Color Slice
	bool  m_StateColorSliceX;
	bool  m_StateColorSliceY;
	bool  m_StateColorSliceZ;
	bool  m_StateColorSliceAlpha;
	bool  m_StateColorSliceClampToGround;
	float m_ColorSlicePosX;
	float m_ColorSlicePosY;
	float m_ColorSlicePosZ;
	float m_ColorSliceOpacity;

	// Contour Line
	bool  m_StateContourLineX;
	bool  m_StateContourLineY;
	bool  m_StateContourLineZ;
	float m_ContourLinePosX;
	float m_ContourLinePosY;
	float m_ContourLinePosZ;
	float m_ContourLineThresholdMin;
	float m_ContourLineThresholdMax;
	float m_ContourLineInterval;
	float m_ContourLineWidth;

	// Volume Rendering
	bool  m_StateVolumeX;
	bool  m_StateVolumeY;
	bool  m_StateVolumeZ;
	int   m_VolumeXNum;
	int   m_VolumeYNum;
	int   m_VolumeZNum;
	float m_VolumeThresholdMin;
	float m_VolumeThresholdMax;

	QString m_DataFileName;

	bool  m_StateColorBar;
	bool  m_StateBoundingBox;
	bool  m_StateTimeDisplay;

	bool  m_StateBoundingBoxAxisLabel;

	// Back Ground Color
	QColor m_BGColor;

	// ColorTable
	float m_ColorTableR[256];
	float m_ColorTableG[256];
	float m_ColorTableB[256];
	float m_ColorTableA[256];
	QVector< QPair<float, float> > m_ControlPoint_R;
	QVector< QPair<float, float> > m_ControlPoint_G;
	QVector< QPair<float, float> > m_ControlPoint_B;
	QVector< QPair<float, float> > m_ControlPoint_A;
	ColorTableConnectionType m_ColorTableConnectionType;

	// ColorBar
	ColorBarType m_ColorBarType;
	ColorBarOrientation m_ColorBarOrientation;
	int   m_ColorBarDecimalPlacesNum;
	int   m_ColorBarMajorTickMarkNum;
	int   m_ColorBarMinorTickMarkNum;
	ColorBarTickMarkType m_ColorBarMajorTickMarkType;
	ColorBarTickMarkType m_ColorBarMinorTickMarkType;

	// Logo
	bool    m_StateLogo;
	QString m_LogoFileName;
	float   m_LogoAspectRatio;

	// Camera ( Object )
	float   m_ObjectPos[3];	// X, Y, Z
	float   m_ObjectAngle[3];	// Pitch, Yaw. Roll
	float   m_ObjectPosSphere[3];
	float   m_ObjectAngleSphere[3];

	// Log Scale
	bool    m_StateLogScale;

	// KML
	int     m_KMLMaxImageSize[3];

	// Viewer Settings
	QSize   m_ViewerSize;
	QSize   m_MaximumViewerSize;
	Projection m_Projection;
	ViewDirection m_ViewDirection;

	// Topo
	bool     m_StateTopo;
	bool     m_StateSeaSurface;
	bool     m_StateSyncScale;
	int      m_GroundScale;
	int      m_SeaFloorScale;
	TopoType m_TopoType;

	// Coastline
	bool     m_StateCoastline;
	QString  m_CoastlineFileName;

	// BoundingBox
	BoundingBoxParam *m_BoundingBox;
};

#endif // PARAMETER_H
