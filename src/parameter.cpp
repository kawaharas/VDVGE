//
// Volume Data Visualizer for Google Earth
//
// parameter.cpp
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

#include "parameter.h"

extern AppParameter appParam;

Parameter::Parameter()
{
}

void Parameter::initialize()
{
	m_ZUnit  = ZUNIT_METER;
	m_ZScale = 1.0;

	m_StateVolumeX  = false;
	m_StateVolumeY  = false;
	m_StateVolumeZ  = true;
	m_VolumeXNum    = grads.getGridSizeX();
	m_VolumeYNum    = grads.getGridSizeY();
	m_VolumeZNum    = grads.getGridSizeZ();
	m_VolumeThresholdMin = grads.getMin();
	m_VolumeThresholdMax = grads.getMax();

	m_StateColorSliceX = false;
	m_StateColorSliceY = false;
	m_StateColorSliceZ = false;
	m_StateColorSliceAlpha = false;
	m_ColorSlicePosX = 0.0;
	m_ColorSlicePosY = 0.0;
	m_ColorSlicePosZ = 0.0;
	m_ColorSliceOpacity = 1.0;

	m_StateContourLineX = false;
	m_StateContourLineY = false;
	m_StateContourLineZ = false;
	m_ContourLinePosX = 0.0;
	m_ContourLinePosY = 0.0;
	m_ContourLinePosZ = 0.0;
	m_ContourLineThresholdMin = grads.getMin();
	m_ContourLineThresholdMax = grads.getMax();
	m_ContourLineInterval = 0.0;
	m_ContourLineWidth = 1.0;

	m_StateColorBar      = true;
	m_StateBoundingBox   = true;
	m_StateTimeDisplay   = true;

	m_ColorBarType              = COLORBAR_RGB;
	m_ColorBarOrientation       = HORIZONTAL;
	m_ColorBarDecimalPlacesNum  = COLORBAR_DECIMAL_PLACES_NUM;
	m_ColorBarMajorTickMarkNum  = COLORBAR_MAJOR_TICKMARK_NUM;
	m_ColorBarMinorTickMarkNum  = COLORBAR_MINOR_TICKMARK_NUM;
	m_ColorBarMajorTickMarkType = TICKMARK_CROSS;
	m_ColorBarMinorTickMarkType = TICKMARK_OUTSIDE;

	m_ObjectPos[0]    = OBJECT_POS_X;
	m_ObjectPos[1]    = OBJECT_POS_Y;
	m_ObjectPos[2]    = OBJECT_POS_Z;
	m_ObjectAngle[0]  = OBJECT_ANGLE_PITCH;
	m_ObjectAngle[1]  = OBJECT_ANGLE_YAW;
	m_ObjectAngle[2]  = OBJECT_ANGLE_ROLL;
	m_ObjectPosSphere[0]   =  OBJECT_POS_SPHERE_X;
	m_ObjectPosSphere[1]   =  OBJECT_POS_SPHERE_Y;
	m_ObjectPosSphere[2]   =  OBJECT_POS_SPHERE_Z;
	m_ObjectAngleSphere[0] = grads.getGridYMin() + ( grads.getGridYMax() - grads.getGridYMin() ) / 2.0;
	m_ObjectAngleSphere[1] = -90.0 - ( grads.getGridXMin() + ( grads.getGridXMax() - grads.getGridXMin() ) / 2.0 );
	m_ObjectAngleSphere[2] =  0.0,

	setCurrentStep( 0 );
	setTimeBegin( 0 );
	setTimeEnd( grads.getGridSizeT() - 1 );
	setTimeSkip( 1 );
	setTimeConversionToUTC( "+00:00" );

	m_StateBoundingBoxAxisLabel = true;

	for ( int i = 0; i < 256; i++ ) {
		m_ColorTableR[ i ] = 0.0;
		m_ColorTableG[ i ] = 0.0;
		m_ColorTableB[ i ] = 0.0;
		m_ColorTableA[ i ] = 0.0;
	}

	m_BGColor.setRgbF( 0.0, 0.0, 0.0 );

	m_ControlPoint_R.clear();
	m_ControlPoint_G.clear();
	m_ControlPoint_B.clear();
	m_ControlPoint_A.clear();
	m_ColorTableConnectionType = CURVE_CONNECTION;

	m_StateLogo  = false;
	QString logofile = QCoreApplication::applicationDirPath() + "/logo.png";
	if ( QFileInfo( logofile ).exists() ) {
		m_LogoFileName = logofile;
	} else {
		m_LogoFileName = "";
	}
	m_LogoAspectRatio = 1.0;

	m_StateLogScale = false;

	m_KMLMaxImageSize[0] = KML_MAX_IMAGE_SIZE_X;
	m_KMLMaxImageSize[1] = KML_MAX_IMAGE_SIZE_Y;
	m_KMLMaxImageSize[2] = KML_MAX_IMAGE_SIZE_Z;

	m_Projection = PERSPECTIVE;
	m_ViewDirection = VIEW_DEFAULT;

	m_StateTopo = false;
	m_StateSeaSurface = false;
	m_StateSyncScale = true;
	m_GroundScale = 1.0;
	m_SeaFloorScale = 1.0;
	m_TopoType = TOPO_NONE;

	m_StateCoastline = false;
	QString coastlinefile = QCoreApplication::applicationDirPath() + "/coastline.dat";
	if ( QFileInfo( coastlinefile ).exists() ) {
		m_CoastlineFileName = coastlinefile;
	} else {
		m_CoastlineFileName = "";
	}

	m_BoundingBox = new BoundingBoxParam;
	boundingBox()->initialize();
}

bool Parameter::save( const QString &filename )
{
	QFile saveFile( filename );
	saveFile.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &saveFile );
	QString tmpStr;

	stream << "#\n";
	stream << "# Volume Data Visualizer for Google Earth\n";
	stream << "# Save File for\n";
	stream << "# \"" << grads.getCtlFileName() << "\"\n";
	stream << "#\n\n";

	if ( ( getTimeBegin() != 0 ) ||
		 ( getTimeEnd() != ( grads.getGridSizeT() - 1 ) ) ||
		 ( getTimeSkip() != 1 ) ||
		 ( getTimeConversionToUTC() != "+00:00" ) ) {
		stream << "# Time Range\n";
		if ( getTimeBegin() != 0 ) {
			stream << "TIME_BEGIN   " << getTimeBegin() << ENDL;
		}
		if ( getTimeEnd() != ( grads.getGridSizeT() - 1 ) ) {
			stream << "TIME_END     " << getTimeEnd() << ENDL;
		}
		if ( getTimeSkip() != 1 ) {
			stream << "TIME_SKIP    " << getTimeSkip() << ENDL;
		}
		if ( getTimeConversionToUTC() != "+00:00" ) {
			if ( IsTimeConversionToUTC() ) {
				stream << "TIME_CONVERSION_TO_UTC ON ";
			} else {
				stream << "TIME_CONVERSION_TO_UTC OFF ";
			}
			stream << getTimeConversionToUTC() << ENDL;
		}
		stream << ENDL;
	}

	if ( ( getZUnit() != ZUNIT_METER ) || ( getZScale() != 1.0 ) ) {
		stream << "# Z Axis\n";
		if ( getZUnit() != ZUNIT_METER ) {
			stream << "ZUNIT  " << "KILOMETER" << ENDL;
		}
		if ( getZScale() != 1.0 ) {
			stream << "ZSCALE " << getZScale() << ENDL;
		}
		stream << ENDL;
	}

	tmpStr.clear();

	if ( getObjectPosX() != OBJECT_POS_X ) {
		tmpStr += "OBJECT_X " +	QString::number( getObjectPosX(), 'g' ) + "\n";
	}
	if ( getObjectPosY() != OBJECT_POS_Y ) {
		tmpStr += "OBJECT_Y " + QString::number( getObjectPosY(), 'g' ) + "\n";
	}
	if ( getObjectPosZ() != OBJECT_POS_Z ) {
		tmpStr += "OBJECT_Z " + QString::number( getObjectPosZ(), 'g' ) + "\n";
	}
	if ( getObjectAnglePitch() != OBJECT_ANGLE_PITCH ) {
		tmpStr += "OBJECT_PITCH " + QString::number( getObjectAnglePitch(), 'g' ) + "\n";
	}
	if ( getObjectAngleYaw() != OBJECT_ANGLE_YAW ) {
		tmpStr += "OBJECT_YAW " + QString::number( getObjectAngleYaw(), 'g' ) + "\n";
	}
	if ( getObjectPosSphereX() != OBJECT_POS_SPHERE_X ) {
		tmpStr += "OBJECT_X_SPHERE " + QString::number( getObjectPosSphereX(), 'g' ) + "\n";
	}
	if ( getObjectPosSphereY() != OBJECT_POS_SPHERE_Y ) {
		tmpStr += "OBJECT_Y_SPHERE " + QString::number( getObjectPosSphereY(), 'g' ) + "\n";
	}
	if ( getObjectPosSphereZ() != OBJECT_POS_SPHERE_Z ) {
		tmpStr += "OBJECT_Z_SPHERE " + QString::number( getObjectPosSphereZ(), 'g' ) + "\n";
	}
	if ( getObjectAngleSpherePitch() !=
		static_cast< float >( grads.getGridYMin()
			+ ( grads.getGridYMax() - grads.getGridYMin() ) / 2.0 ) ) {
		tmpStr += "OBJECT_PITCH_SPHERE " + QString::number( getObjectAngleSpherePitch(), 'g' ) + "\n";
	}
	if ( getObjectAngleSphereYaw() !=
		static_cast< float >( -90.0 - ( grads.getGridXMin()
			+ ( grads.getGridXMax() - grads.getGridXMin() ) / 2.0 ) ) ) {
		tmpStr += "OBJECT_YAW_SPHERE " + QString::number( getObjectAngleSphereYaw(), 'g' ) + "\n";
	}
	if ( !tmpStr.isEmpty() ) {
		stream << "# View\n" << tmpStr << ENDL;
	}

	tmpStr.clear();
	tmpStr += "VOLUME_THRESHOLD_MIN " +
		QString::number( getVolumeThresholdMin(), 'g' ) + "\n";
	tmpStr += "VOLUME_THRESHOLD_MAX " +
		QString::number( getVolumeThresholdMax(), 'g' ) + "\n";
	if ( !tmpStr.isEmpty() ) {
		stream << "# Data Range\n" << tmpStr << ENDL;
	}

	if ( IsLogScale() ) {
		stream << "# Log Scale\n";
		stream << "LOGSCALE ON\n\n";
	}

	tmpStr.clear();
	if ( getVolumeXNum() != grads.getGridSizeX() ) {
		if ( IsVolumeX() ) {
			tmpStr += "VOLUME_X ON\n";
		} else {
			tmpStr += "VOLUME_X OFF\n";
		}
		tmpStr += "VOLUME_X_SLICES " +
				QString::number( getVolumeXNum(), 10 ) + "\n";
	} else {
		if ( IsVolumeX() ) {
			tmpStr += "VOLUME_X ON\n";
		}
	}
	if ( getVolumeYNum() != grads.getGridSizeY() ) {
		if ( IsVolumeY() ) {
			tmpStr += "VOLUME_Y ON\n";
		} else {
			tmpStr += "VOLUME_Y OFF\n";
		}
		tmpStr += "VOLUME_Y_SLICES " +
				QString::number( getVolumeYNum(), 10 ) + "\n";
	} else {
		if ( IsVolumeY() ) {
			tmpStr += "VOLUME_Y ON\n";
		}
	}
	if ( getVolumeZNum() != grads.getGridSizeZ() ) {
		if ( IsVolumeZ() ) {
			tmpStr += "VOLUME_Z ON\n";
		} else {
			tmpStr += "VOLUME_Z OFF\n";
		}
		tmpStr += "VOLUME_Z_SLICES " +
				QString::number( getVolumeZNum(), 10 ) + "\n";
	} else {
		if ( IsVolumeZ() ) {
			tmpStr += "VOLUME_Z ON\n";
		}
	}
	if ( !tmpStr.isEmpty() ) {
		stream << "# Volume Render\n" << tmpStr << ENDL;
	}

	tmpStr.clear();
	if ( getColorSlicePosX() != 0.0 ) {
		if ( IsColorSliceX() ) {
			tmpStr += "COLORSLICE_X ON\n";
		} else {
			tmpStr += "COLORSLICE_X OFF\n";
		}
		tmpStr += "COLORSLICE_X_POS " +
				QString::number( getColorSlicePosX(), 'g' ) + "\n";
	} else {
		if ( IsColorSliceX() ) {
			tmpStr += "COLORSLICE_X ON\n";
		}
	}
	if ( getColorSlicePosY() != 0.0 ) {
		if ( IsColorSliceY() ) {
			tmpStr += "COLORSLICE_Y ON\n";
		} else {
			tmpStr += "COLORSLICE_Y OFF\n";
		}
		tmpStr += "COLORSLICE_Y_POS " +
				QString::number( getColorSlicePosY(), 'g' ) + "\n";
	} else {
		if ( IsColorSliceY() ) {
			tmpStr += "COLORSLICE_Y ON\n";
		}
	}
	if ( getColorSlicePosZ() != 0.0 ) {
		if ( IsColorSliceZ() ) {
			tmpStr += "COLORSLICE_Z ON\n";
		} else {
			tmpStr += "COLORSLICE_Z OFF\n";
		}
		tmpStr += "COLORSLICE_Z_POS " +
			QString::number( getColorSlicePosZ(), 'g' ) + "\n";
	} else {
		if ( IsColorSliceZ() ) {
			tmpStr += "COLORSLICE_Z ON\n";
		}
	}
	if ( IsColorSliceClampToGround() ) {
		tmpStr += "COLORSLICE_CLAMP_TO_GROUND ON\n";
	}
	if ( IsColorSliceAlpha() ) {
		tmpStr += "COLORSLICE_ALPHA ON\n";
	}
	if ( getColorSliceOpacity() != 1.0 ) {
		tmpStr += "COLORSLICE_OPACITY " +
			QString::number( getColorSliceOpacity(), 'g' ) + "\n";
	}
	if ( !tmpStr.isEmpty() ) {
		stream << "# Color Slice\n" << tmpStr << ENDL;
	}

	tmpStr.clear();
	if ( getContourLinePosX() != 0.0 ) {
		if ( IsContourLineX() ) {
			tmpStr += "CONTOURLINE_X ON\n";
		} else {
			tmpStr += "CONTOURLINE_X OFF\n";
		}
		tmpStr += "CONTOURLINE_X_POS " +
			QString::number( getContourLinePosX(), 'g' ) + "\n";
	} else {
		if ( IsContourLineX() ) {
			tmpStr += "CONTOURLINE_X ON\n";
		}
	}
	if ( getContourLinePosY() != 0.0 ) {
		if ( IsContourLineY() ) {
			tmpStr += "CONTOURLINE_Y ON\n";
		} else {
			tmpStr += "CONTOURLINE_Y OFF\n";
		}
		tmpStr += "CONTOURLINE_Y_POS " +
			QString::number( getContourLinePosY(), 'g' ) + "\n";
	} else {
		if ( IsContourLineY() ) {
			tmpStr += "CONTOURLINE_Y ON\n";
		}
	}
	if ( getContourLinePosZ() != 0.0 ) {
		if ( IsContourLineZ() ) {
			tmpStr += "CONTOURLINE_Z ON\n";
		} else {
			tmpStr += "CONTOURLINE_Z OFF\n";
		}
		tmpStr += "CONTOURLINE_Z_POS " +
			QString::number( getContourLinePosZ(), 'g' ) + "\n";
	} else {
		if ( IsContourLineZ() ) {
			tmpStr += "CONTOURLINE_Z ON\n";
		}
	}
	if ( getContourLineThresholdMin() != getVolumeThresholdMin() ) {
		tmpStr += "CONTOURLINE_THRESHOLD_MIN " +
			QString::number( getContourLineThresholdMin(), 'g' ) + "\n";
	}
	if ( getContourLineThresholdMax() != getVolumeThresholdMax() ) {
		tmpStr += "CONTOURLINE_THRESHOLD_MAX " +
			QString::number( getContourLineThresholdMax(), 'g' ) + "\n";
	}
	if ( getContourLineInterval() != 0.0 ) {
		tmpStr += "CONTOURLINE_INTERVAL " +
			QString::number( getContourLineInterval(), 'g' ) + "\n";
	}
	if ( getContourLineWidth() != 1.0 ) {
		tmpStr += "CONTOURLINE_WIDTH " +
			QString::number( getContourLineWidth(), 'g' ) + "\n";
	}
	if ( !tmpStr.isEmpty() ) {
		stream << "# Contour Line\n" << tmpStr << ENDL;
	}

	if ( getBGColor() != Qt::black ) {
		stream << "# Background Color\n";
		stream << "BACKGROUND_COLOR "
			   << getBGColor().red() << " "
			   << getBGColor().green() << " "
			   << getBGColor().blue() << ENDL << ENDL;
	}

	tmpStr.clear();
	if ( !IsBoundingBox() ) {
		tmpStr += "BOUNDINGBOX OFF\n";
	}
	if ( !IsColorBar() ) {
		tmpStr += "COLORBAR    OFF\n";
	}
	if ( !IsTimeDisplay() ) {
		tmpStr += "TIMEDISPLAY OFF\n";
	}
	if ( !IsBoundingBoxAxisLabel() ) {
		tmpStr += "BOUNDINGBOX_AXISLABEL OFF\n";
	}
	if ( getColorBarOrientation() == VERTICAL ) {
		tmpStr += "COLORBAR_DIRECTION VERTICAL\n";
	}
	if ( getColorBarType() == COLORBAR_RGBA ) {
		tmpStr += "COLORBAR_TYPE RGBA\n";
	} else if ( getColorBarType() == COLORBAR_RGB_AND_RGBA ) {
		tmpStr += "COLORBAR_TYPE RGB_AND_RGBA\n";
	}
	if ( getColorBarDecimalPlacesNum() != COLORBAR_DECIMAL_PLACES_NUM ) {
		tmpStr += "COLORBAR_DECIMAL_PLACES ";
		tmpStr += QString::number( getColorBarDecimalPlacesNum(), 10 ) + "\n";
	}
	if ( getColorBarMajorTickMarkNum() != COLORBAR_MAJOR_TICKMARK_NUM ) {
		tmpStr += "COLORBAR_MAJOR_TICKMARKS ";
		tmpStr += QString::number( getColorBarMajorTickMarkNum(), 10 ) + "\n";
	}
	if ( getColorBarMinorTickMarkNum() != COLORBAR_MINOR_TICKMARK_NUM ) {
		tmpStr += "COLORBAR_MINOR_TICKMARKS ";
		tmpStr += QString::number( getColorBarMinorTickMarkNum(), 10 ) + "\n";
	}
	if ( getColorBarMajorTickMarkType() != TICKMARK_CROSS ) {
		if ( getColorBarMajorTickMarkType() == TICKMARK_INSIDE ) {
			tmpStr += "COLORBAR_MAJOR_TICKMARK_TYPE INSIDE\n";
		} else if ( getColorBarMajorTickMarkType() == TICKMARK_OUTSIDE ) {
			tmpStr += "COLORBAR_MAJOR_TICKMARK_TYPE OUTSIDE\n";
		} else {
			tmpStr += "COLORBAR_MAJOR_TICKMARK_TYPE NONE\n";
		}
	}
	if ( getColorBarMinorTickMarkType() != TICKMARK_OUTSIDE ) {
		if ( getColorBarMinorTickMarkType() == TICKMARK_INSIDE ) {
			tmpStr += "COLORBAR_MINOR_TICKMARK_TYPE INSIDE\n";
		} else if ( getColorBarMinorTickMarkType() == TICKMARK_CROSS ) {
			tmpStr += "COLORBAR_MINOR_TICKMARK_TYPE CROSS\n";
		} else {
			tmpStr += "COLORBAR_MINOR_TICKMARK_TYPE NONE\n";
		}
	}
	if ( !getLogo().isEmpty() ) {
		if ( IsLogo() ) {
			tmpStr += "LOGO ON\n";
		} else {
			tmpStr += "LOGO OFF\n";
		}
		tmpStr += "LOGO_FILE " + getLogo() + "\n";
	}
	if ( !tmpStr.isEmpty() ) {
		stream << "# Visual Effects\n" << tmpStr << ENDL;
	}

	stream << "# Color Table\n";
	QString path = QFileInfo( filename ).absolutePath();
	QString name = QFileInfo( filename ).baseName() + ".colortable";
	QString colortablefile = path + "/" + name;
	stream << "COLORTABLE_FILE " << name << ENDL;
	saveColorTable( colortablefile );

	tmpStr.clear();
	if ( ( getKMLMaxImageSizeX() != 600 )
	  || ( getKMLMaxImageSizeY() != 600 )
	  || ( getKMLMaxImageSizeZ() != 600 ) ) {
		tmpStr += "KML_MAX_IMAGE_SIZE_X ";
		tmpStr += QString::number( getKMLMaxImageSizeX(), 10 ) + "\n";
		tmpStr += "KML_MAX_IMAGE_SIZE_Y ";
		tmpStr += QString::number( getKMLMaxImageSizeY(), 10 ) + "\n";
		tmpStr += "KML_MAX_IMAGE_SIZE_Z ";
		tmpStr += QString::number( getKMLMaxImageSizeZ(), 10 ) + "\n";
	}
	if ( !tmpStr.isEmpty() ) {
		stream << "# KML Maximum Image Size\n" << tmpStr << ENDL;
	}

	saveFile.close();

	return true;
}

ErrorCode Parameter::load( const QString &filename )
{
	QFile loadFile( filename );
	if ( !( loadFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) ) {
		if ( appParam.getStateGUI() ) {
			QString msg = QString( "Could not load \"%1\"." ).arg( filename );
			QMessageBox::critical( 0, "File Open Error", msg );
		}
		return PARAM_ERROR_SAVEFILE_OPEN;
	}
	QTextStream inputStream( &loadFile );
	QString lineString;
	QString token1, token2;

	int     timeBegin        = 0;
	int     timeEnd          = grads.getGridSizeT() - 1;
	int     timeSkip         = 1;
	bool    stateTimeConvertionToUTC = false;
	QString timeConvertionToUTC = "+00:00";

	ZUnit  zUnit            = ZUNIT_METER;
	float  zScale           = 1.0;
	float  objX             = OBJECT_POS_X;
	float  objY             = OBJECT_POS_Y;
	float  objZ             = OBJECT_POS_Z;
	float  objPitch         = OBJECT_ANGLE_PITCH;
	float  objYaw           = OBJECT_ANGLE_YAW;
	float  objSphereX       = OBJECT_POS_SPHERE_X;
	float  objSphereY       = OBJECT_POS_SPHERE_Y;
	float  objSphereZ       = OBJECT_POS_SPHERE_Z;
	float  objSpherePitch   = grads.getGridYMin() + ( grads.getGridYMax() - grads.getGridYMin() ) / 2.0;
	float  objSphereYaw     = -90.0 - ( grads.getGridXMin() + ( grads.getGridXMax() - grads.getGridXMin() ) / 2.0 );

	double volumeThresMin   = grads.getMin();
	double volumeThresMax   = grads.getMax();
	bool   logScale         = false;
	bool   volumeSliceX     = false;
	bool   volumeSliceY     = false;
	bool   volumeSliceZ     = false;
	bool   colorSliceX      = false;
	bool   colorSliceY      = false;
	bool   colorSliceZ      = false;
	bool   colorSliceClampToGround = false;
	bool   colorSliceAlpha  = false;
	int    volumeSliceXNum  = grads.getGridSizeX();
	int    volumeSliceYNum  = grads.getGridSizeY();
	int    volumeSliceZNum  = grads.getGridSizeZ();
	float  colorSliceXPos = 0.0;
	float  colorSliceYPos = 0.0;
	float  colorSliceZPos = 0.0;
	float  colorSliceOpacity = 1.0;

	bool   contourLineX = false;
	bool   contourLineY = false;
	bool   contourLineZ = false;
	float  contourLineXPos = 0.0;
	float  contourLineYPos = 0.0;
	float  contourLineZPos = 0.0;
	float  contourLineThresMin = grads.getMin();
	float  contourLineThresMax = grads.getMax();
	float  contourLineInterval = 0.0;
	float  contourLineWidth = 1.0;

	int    backGroundColorR = 0;
	int    backGroundColorG = 0;
	int    backGroundColorB = 0;

	bool   stateBoundingBox  = true;
	bool   stateColorBar     = true;
	bool   stateTimeDisplay  = true;
	bool   stateBoundingBoxAxisLabel = true;
	ColorBarOrientation ColorBarOrientation = HORIZONTAL;
	ColorBarType colorBarType = COLORBAR_RGB;
	int    colorBarDecimalPlacesNum  = COLORBAR_DECIMAL_PLACES_NUM;
	int    colorBarMajorTickMarkNum  = COLORBAR_MAJOR_TICKMARK_NUM;
	int    colorBarMinorTickMarkNum  = COLORBAR_MINOR_TICKMARK_NUM;
	ColorBarTickMarkType colorBarMajorTickMarkType = TICKMARK_CROSS;
	ColorBarTickMarkType colorBarMinorTickMarkType = TICKMARK_OUTSIDE;
	bool   stateLogo = false;
	QString logoFileName;

	QVector< QPair< float, float > > colorControlPointRed;
	QVector< QPair< float, float > > colorControlPointGreen;
	QVector< QPair< float, float > > colorControlPointBlue;
	QVector< QPair< float, float > > colorControlPointAlpha;

	QString colorTableFileName;
	bool   existColorTableFile = false;
	bool   existColorTableR = false;
	bool   existColorTableG = false;
	bool   existColorTableB = false;
	bool   existColorTableA = false;

	int kmlMaxImageSizeX = 600;
	int kmlMaxImageSizeY = 600;
	int kmlMaxImageSizeZ = 600;

	QStringList stringList;

	bool ok;
	while ( !inputStream.atEnd() ) {
		lineString = inputStream.readLine();
		if ( lineString[0] != QChar( '#' ) ) {
			stringList = splitString( "\\s+", lineString );
			if ( stringList.size() >= 2 ) {
				token1 = ( stringList.at( 0 ) ).toUpper();
				token2 = ( stringList.at( 1 ) ).toUpper();
				if ( token1 == "TIME_BEGIN" ) {
					timeBegin = token2.toInt( &ok );
					if ( !ok ) {
						return PARAM_ERROR_TIME_BEGIN;
					}
				} else if ( token1 == "TIME_END" ) {
					timeEnd = token2.toInt( &ok );
					if ( !ok ) {
						return PARAM_ERROR_TIME_END;
					}
				} else if ( token1 == "TIME_SKIP" ) {
					timeSkip = token2.toInt( &ok );
					if ( !ok ) {
						return PARAM_ERROR_TIME_SKIP;
					}
				} else if ( token1 == "TIME_CONVERSION_TO_UTC" ) {
					if ( token2 == "ON" ) {
						stateTimeConvertionToUTC = true;
					} else if ( token2 == "OFF" ) {
						stateTimeConvertionToUTC = false;
					} else {
						return PARAM_ERROR_TIME_CONVERSION_TO_UTC;
					}
					QString str = ( stringList.at( 2 ) ).toUpper();
					bool plus = true;
					if ( str.at( 0 ) == '+' ) {
						str.remove( 0, 1 );
					} else if ( str.at( 0 ) == '-' ) {
						str.remove( 0, 1 );
						plus = false;
					}
					QStringList list = str.split( ":" );
					if ( list.size() != 2 ) {
						return PARAM_ERROR_TIME_CONVERSION_TO_UTC;
					} else {
						bool ok1, ok2;
						int hour   = list.at( 0 ).toInt( &ok1 );
						int minute = list.at( 1 ).toInt( &ok2 );
						if ( !ok1 || !ok2 ) {
							return PARAM_ERROR_TIME_CONVERSION_TO_UTC;
						} else if ( hour > 24 ) {
							return PARAM_ERROR_TIME_CONVERSION_TO_UTC;
						} else if ( minute > 60 ) {
							return PARAM_ERROR_TIME_CONVERSION_TO_UTC;
						} else if ( ( hour * 100 + minute ) > 2400 ) {
							return PARAM_ERROR_TIME_CONVERSION_TO_UTC;
						}
						timeConvertionToUTC = QString( "%1:%2" )
							.arg( hour,   2, 10, QChar( '0' ) )
							.arg( minute, 2, 10, QChar( '0' ) );
						if ( plus ) {
							timeConvertionToUTC.prepend( "+" );
						} else {
							timeConvertionToUTC.prepend( "-" );
						}
					}
				} else if ( token1 == "ZUNIT" ) {
					if ( token2 == "METER" ) {
						zUnit = ZUNIT_METER;
					} else if ( token2 == "KILOMETER" ) {
						zUnit = ZUNIT_KILOMETER;
					} else {
						return PARAM_ERROR_ZUNIT;
					}
				} else if ( token1 == "ZSCALE" ) {
					zScale = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_ZSCALE;
					}
				} else if ( token1 == "OBJECT_X" ) {
					objX = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_OBJECT_X;
					}
				} else if ( token1 == "OBJECT_Y" ) {
					objY = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_OBJECT_Y;
					}
				} else if ( token1 == "OBJECT_Z" ) {
					objZ = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_OBJECT_Z;
					}
				} else if ( token1 == "OBJECT_PITCH" ) {
					objPitch = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_OBJECT_PITCH;
					}
				} else if ( token1 == "OBJECT_YAW" ) {
					objYaw = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_OBJECT_YAW;
					}
				} else if ( token1 == "OBJECT_X_SPHERE" ) {
					objSphereX = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_OBJECT_X_SPHERE;
					}
				} else if ( token1 == "OBJECT_Y_SPHERE" ) {
					objSphereY = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_OBJECT_Y_SPHERE;
					}
				} else if ( token1 == "OBJECT_Z_SPHERE" ) {
					objSphereZ = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_OBJECT_Z_SPHERE;
					}
				} else if ( token1 == "OBJECT_PITCH_SPHERE" ) {
					objSpherePitch = token2.toFloat();
					if ( !ok ) {
						return PARAM_ERROR_OBJECT_PITCH_SPHERE;
					}
				} else if ( token1 == "OBJECT_YAW_SPHERE" ) {
					objSphereYaw = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_OBJECT_YAW_SPHERE;
					}
				} else if ( token1 == "VOLUME_THRESHOLD_MIN" ) {
					volumeThresMin = token2.toDouble( &ok );
					if ( !ok ) {
						return PARAM_ERROR_VOLUME_THRESHOLD_MIN;
					}
				} else if ( token1 == "VOLUME_THRESHOLD_MAX" ) {
					volumeThresMax = token2.toDouble( &ok );
					if ( !ok ) {
						return PARAM_ERROR_VOLUME_THRESHOLD_MAX;
					}
				} else if ( token1 == "LOGSCALE" ) {
					if ( token2 == "ON" ) {
						logScale = true;
					} else if ( token2 == "OFF" ) {
						logScale = false;
					} else {
						return PARAM_ERROR_LOGSCALE;
					}
				} else if ( token1 == "VOLUME_X" ) {
					if ( token2 == "ON" ) {
						volumeSliceX = true;
					} else if ( token2 == "OFF" ) {
						volumeSliceX = false;
					} else {
						return PARAM_ERROR_VOLUME_X;
					}
				} else if ( token1 == "VOLUME_Y" ) {
					if ( token2 == "ON" ) {
						volumeSliceY = true;
					} else if ( token2 == "OFF" ) {
						volumeSliceY = false;
					} else {
						return PARAM_ERROR_VOLUME_Y;
					}
				} else if ( token1 == "VOLUME_Z" ) {
					if ( token2 == "ON" ) {
						volumeSliceZ = true;
					} else if ( token2 == "OFF" ) {
						volumeSliceZ = false;
					} else {
						return PARAM_ERROR_VOLUME_Z;
					}
				} else if ( token1 == "VOLUME_X_SLICES" ) {
					volumeSliceXNum = token2.toInt( &ok );
					if ( !ok ) {
						return PARAM_ERROR_VOLUME_X_SLICES;
					}
				} else if ( token1 == "VOLUME_Y_SLICES" ) {
					volumeSliceYNum = token2.toInt( &ok );
					if ( !ok ) {
						return PARAM_ERROR_VOLUME_Y_SLICES;
					}
				} else if ( token1 == "VOLUME_Z_SLICES" ) {
					volumeSliceZNum = token2.toInt( &ok );
					if ( !ok ) {
						return PARAM_ERROR_VOLUME_Z_SLICES;
					}
				} else if ( token1 == "COLORSLICE_X" ) {
					if ( token2 == "ON" ) {
						colorSliceX = true;
					} else if ( token2 == "OFF" ) {
						colorSliceX = false;
					} else {
						return PARAM_ERROR_COLORSLICE_X;
					}
				} else if ( token1 == "COLORSLICE_Y" ) {
					if ( token2 == "ON" ) {
						colorSliceY = true;
					} else if ( token2 == "OFF" ) {
						colorSliceY = false;
					} else {
						return PARAM_ERROR_COLORSLICE_Y;
					}
				} else if ( token1 == "COLORSLICE_Z" ) {
					if ( token2 == "ON" ) {
						colorSliceZ = true;
					} else if ( token2 == "OFF" ) {
						colorSliceZ = false;
					} else {
						return PARAM_ERROR_COLORSLICE_Z;
					}
				} else if ( token1 == "COLORSLICE_X_POS" ) {
					colorSliceXPos = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_COLORSLICE_X_POS;
					}
				} else if ( token1 == "COLORSLICE_Y_POS" ) {
					colorSliceYPos = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_COLORSLICE_Y_POS;
					}
				} else if ( token1 == "COLORSLICE_Z_POS" ) {
					colorSliceZPos = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_COLORSLICE_Z_POS;
					}
				} else if ( token1 == "COLORSLICE_CLAMP_TO_GROUND" ) {
					if ( token2 == "ON" ) {
						colorSliceClampToGround = true;
					} else if ( token2 == "OFF" ) {
						colorSliceClampToGround = false;
					} else {
						return PARAM_ERROR_COLORSLICE_CLAMP_TO_GROUND;
					}
				} else if ( token1 == "COLORSLICE_ALPHA" ) {
					if ( token2 == "ON" ) {
						colorSliceAlpha = true;
					} else if ( token2 == "OFF" ) {
						colorSliceAlpha = false;
					} else {
						return PARAM_ERROR_COLORSLICE_ALPHA;
					}
				} else if ( token1 == "COLORSLICE_OPACITY" ) {
					colorSliceOpacity = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_COLORSLICE_OPACITY;
					}
				} else if ( token1 == "CONTOURLINE_X" ) {
					if ( token2 == "ON" ) {
						contourLineX = true;
					} else if ( token2 == "OFF" ) {
						contourLineX = false;
					} else {
						return PARAM_ERROR_CONTOURLINE_X;
					}
				} else if ( token1 == "CONTOURLINE_Y" ) {
					if ( token2 == "ON" ) {
						contourLineY = true;
					} else if ( token2 == "OFF" ) {
						contourLineY = false;
					} else {
						return PARAM_ERROR_CONTOURLINE_Y;
					}
				} else if ( token1 == "CONTOURLINE_Z" ) {
					if ( token2 == "ON" ) {
						contourLineZ = true;
					} else if ( token2 == "OFF" ) {
						contourLineZ = false;
					} else {
						return PARAM_ERROR_CONTOURLINE_Z;
					}
				} else if ( token1 == "CONTOURLINE_X_POS" ) {
					contourLineXPos = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_CONTOURLINE_X_POS;
					}
				} else if ( token1 == "CONTOURLINE_Y_POS" ) {
					contourLineYPos = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_CONTOURLINE_Y_POS;
					}
				} else if ( token1 == "CONTOURLINE_Z_POS" ) {
					contourLineZPos = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_CONTOURLINE_Z_POS;
					}
				} else if ( token1 == "CONTOURLINE_THRESHOLD_MIN" ) {
					contourLineThresMin = token2.toDouble( &ok );
					if ( !ok ) {
						return PARAM_ERROR_CONTOURLINE_THRESHOLD_MIN;
					}
				} else if ( token1 == "CONTOURLINE_THRESHOLD_MAX" ) {
					contourLineThresMax = token2.toDouble( &ok );
					if ( !ok ) {
						return PARAM_ERROR_CONTOURLINE_THRESHOLD_MAX;
					}
				} else if ( token1 == "CONTOURLINE_INTERVAL" ) {
					contourLineInterval = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_CONTOURLINE_INTERVAL;
					}
				} else if ( token1 == "CONTOURLINE_WIDTH" ) {
					contourLineWidth = token2.toFloat( &ok );
					if ( !ok ) {
						return PARAM_ERROR_CONTOURLINE_WIDTH;
					}
				} else if ( token1 == "BACKGROUND_COLOR" ) {
					if ( stringList.size() < 4 ) {
						return PARAM_ERROR_BACKGROUND_COLOR;
					}
					bool ok1, ok2, ok3;
					backGroundColorR = ( stringList.at( 1 ) ).toInt( &ok1 );
					backGroundColorG = ( stringList.at( 2 ) ).toInt( &ok2 );
					backGroundColorB = ( stringList.at( 3 ) ).toInt( &ok3 );
					if ( !ok1 || !ok2 || !ok3 ) {
						return PARAM_ERROR_BACKGROUND_COLOR;
					} else {
						if ( backGroundColorR < 0 ) {
							backGroundColorR = 0;
						}
						if ( backGroundColorR > 255 ) {
							backGroundColorR = 255;
						}
						if ( backGroundColorG < 0 ) {
							backGroundColorG = 0;
						}
						if ( backGroundColorG > 255 ) {
							backGroundColorG = 255;
						}
						if ( backGroundColorB < 0 ) {
							backGroundColorB = 0;
						}
						if ( backGroundColorB > 255 ) {
							backGroundColorB = 255;
						}
					}
				} else if ( token1 == "BOUNDINGBOX" ) {
					if ( token2 == "ON" ) {
						stateBoundingBox = true;
					} else if ( token2 == "OFF" ) {
						stateBoundingBox = false;
					} else {
						return PARAM_ERROR_BOUNDINGBOX;
					}
				} else if ( token1 == "COLORBAR" ) {
					if ( token2 == "ON" ) {
						stateColorBar = true;
					} else if ( token2 == "OFF" ) {
						stateColorBar = false;
					} else {
						return PARAM_ERROR_COLORBAR;
					}
				} else if ( token1 == "TIMEDISPLAY" ) {
					if ( token2 == "ON" ) {
						stateTimeDisplay = true;
					} else if ( token2 == "OFF" ) {
						stateTimeDisplay = false;
					} else {
						return PARAM_ERROR_TIMEDISPLAY;
					}
				} else if ( token1 == "BOUNDINGBOX_AXISLABEL" ) {
					if ( token2 == "ON" ) {
						stateBoundingBoxAxisLabel = true;
					} else if ( token2 == "OFF" ) {
						stateBoundingBoxAxisLabel = false;
					} else {
						return PARAM_ERROR_BOUNDINGBOX_AXISLABEL;
					}
				} else if ( token1 == "COLORBAR_DIRECTION" ) {
					if ( token2 == "HORIZONTAL" ) {
						ColorBarOrientation = HORIZONTAL;
					} else if ( token2 == "VERTICAL" ) {
						ColorBarOrientation = VERTICAL;
					} else {
						return PARAM_ERROR_COLORBAR_DIRECTION;
					}
				} else if ( token1 == "COLORBAR_TYPE" ) {
					if ( token2 == "RGB" ) {
						colorBarType = COLORBAR_RGB;
					} else if ( token2 == "RGBA" ) {
						colorBarType = COLORBAR_RGBA;
					} else if ( token2 == "RGB_AND_RGBA" ) {
						colorBarType = COLORBAR_RGB_AND_RGBA;
					} else {
						return PARAM_ERROR_COLORBAR_TYPE;
					}
				} else if ( token1 == "COLORBAR_DECIMAL_PLACES" ) {
					colorBarDecimalPlacesNum = token2.toInt( &ok );
					if ( !ok ) {
						return PARAM_ERROR_COLORBAR_DECIMAL_PLACES;
					}
				} else if ( token1 == "COLORBAR_MAJOR_TICKMARKS" ) {
					colorBarMajorTickMarkNum = token2.toInt( &ok );
					if ( !ok ) {
						return PARAM_ERROR_COLORBAR_MAJOR_TICKMARKS;
					}
				} else if ( token1 == "COLORBAR_MINOR_TICKMARKS" ) {
					colorBarMinorTickMarkNum = token2.toInt( &ok );
					if ( !ok ) {
						return PARAM_ERROR_COLORBAR_MINOR_TICKMARKS;
					}
				} else if ( token1 == "COLORBAR_MAJOR_TICKMARK_TYPE" ) {
					if ( token2 == "OUTSIDE" ) {
						colorBarMajorTickMarkType = TICKMARK_OUTSIDE;
					} else if ( token2 == "INSIDE" ) {
						colorBarMajorTickMarkType = TICKMARK_INSIDE;
					} else if ( token2 == "CROSS" ) {
						colorBarMajorTickMarkType = TICKMARK_CROSS;
					} else if ( token2 == "NONE" ) {
						colorBarMajorTickMarkType = TICKMARK_NONE;
					} else {
						return PARAM_ERROR_COLORBAR_MAJOR_TICKMARK_TYPE;
					}
				} else if ( token1 == "COLORBAR_MINOR_TICKMARK_TYPE" ) {
					if ( token2 == "OUTSIDE" ) {
						colorBarMinorTickMarkType = TICKMARK_OUTSIDE;
					} else if ( token2 == "INSIDE" ) {
						colorBarMinorTickMarkType = TICKMARK_INSIDE;
					} else if ( token2 == "CROSS" ) {
						colorBarMinorTickMarkType = TICKMARK_CROSS;
					} else if ( token2 == "NONE" ) {
						colorBarMinorTickMarkType = TICKMARK_NONE;
					} else {
						return PARAM_ERROR_COLORBAR_MINOR_TICKMARK_TYPE;
					}
				} else if ( token1 == "LOGO" ) {
					if ( token2 == "ON" ) {
						stateLogo = true;
					} else if ( token2 == "OFF" ) {
						stateLogo = false;
					} else {
						return PARAM_ERROR_LOGO;
					}
				} else if ( token1 == "LOGO_FILE" ) {
					logoFileName = token2;
				} else if ( token1 == "COLORTABLE_FILE" ) {
					colorTableFileName = token2;
					QString path = QFileInfo( filename ).absolutePath();
					QString name = QFileInfo( filename ).baseName() + ".colortable";
					QString colortablefile = path + "/" + name;
					ErrorCode err = loadColorTable( colortablefile );
					if ( err != PARAM_COLORTABLE_NO_ERROR ) {
						return err;
					}
					existColorTableFile = true;
				} else if ( ( token1 == "COLORTABLE_RED" ) ||
							( token1 == "COLORPALLET_RED" ) ) {
					int cp_num = token2.toInt();
					bool ok1, ok2;
					for ( int i = 0; i < cp_num; i++ ) {
						lineString = inputStream.readLine();
						stringList = splitString( "\\s+", lineString );
						float level = ( stringList.at( 0 ) ).toFloat( &ok1 );
						float value = ( stringList.at( 1 ) ).toFloat( &ok2 );
						if ( !ok1 || !ok2 ) {
							return PARAM_ERROR_COLORTABLE_RED;
						}
						colorControlPointRed.push_back( QPair< float, float >( level, value ) );
					}
					if ( colorControlPointRed.size() != cp_num ) {
						return PARAM_ERROR_COLORTABLE_RED;
					} else {
						existColorTableR = true;
					}
				} else if ( ( token1 == "COLORTABLE_GREEN" ) ||
							( token1 == "COLORPALLET_GREEN" ) ) {
					int cp_num = token2.toInt();
					bool ok1, ok2;
					for ( int i = 0; i < cp_num; i++ ) {
						lineString = inputStream.readLine();
						stringList = splitString( "\\s+", lineString );
						float level = ( stringList.at( 0 ) ).toFloat( &ok1 );
						float value = ( stringList.at( 1 ) ).toFloat( &ok2 );
						if ( !ok1 || !ok2 ) {
							return PARAM_ERROR_COLORTABLE_GREEN;
						}
						colorControlPointGreen.push_back( QPair< float, float >( level, value ) );
					}
					if ( colorControlPointGreen.size() != cp_num ) {
						return PARAM_ERROR_COLORTABLE_GREEN;
					} else {
						existColorTableG = true;
					}
				} else if ( ( token1 == "COLORTABLE_BLUE" ) ||
							( token1 == "COLORPALLET_BLUE" ) ) {
					int cp_num = token2.toInt();
					bool ok1, ok2;
					for ( int i = 0; i < cp_num; i++ ) {
						lineString = inputStream.readLine();
						stringList = splitString( "\\s+", lineString );
						float level = ( stringList.at( 0 ) ).toFloat( &ok1 );
						float value = ( stringList.at( 1 ) ).toFloat( &ok2 );
						if ( !ok1 || !ok2 ) {
							return PARAM_ERROR_COLORTABLE_BLUE;
						}
						colorControlPointBlue.push_back( QPair< float, float >( level, value ) );
					}
					if ( colorControlPointBlue.size() != cp_num ) {
						return PARAM_ERROR_COLORTABLE_BLUE;
					} else {
						existColorTableB = true;
					}
				} else if ( ( token1 == "COLORTABLE_ALPHA" ) ||
							( token1 == "COLORPALLET_ALPHA" ) ) {
					int cp_num = token2.toInt();
					bool ok1, ok2;
					for ( int i = 0; i < cp_num; i++ ) {
						lineString = inputStream.readLine();
						stringList = splitString( "\\s+", lineString );
						float level = ( stringList.at( 0 ) ).toFloat( &ok1 );
						float value = ( stringList.at( 1 ) ).toFloat( &ok2 );
						if ( !ok1 || !ok2 ) {
							return PARAM_ERROR_COLORTABLE_ALPHA;
						}
						colorControlPointAlpha.push_back( QPair< float, float >( level, value ) );
					}
					if ( colorControlPointAlpha.size() != cp_num ) {
						return PARAM_ERROR_COLORTABLE_ALPHA;
					} else {
						existColorTableA = true;
					}
				} else if ( token1 == "KML_MAX_IMAGE_SIZE_X" ) {
					kmlMaxImageSizeX = token2.toInt( &ok );
					if ( !ok ) {
						return PARAM_ERROR_KML_MAX_IMAGE_SIZE;
					}
				} else if ( token1 == "KML_MAX_IMAGE_SIZE_Y" ) {
					kmlMaxImageSizeY = token2.toInt( &ok );
					if ( !ok ) {
						return PARAM_ERROR_KML_MAX_IMAGE_SIZE;
					}
				} else if ( token1 == "KML_MAX_IMAGE_SIZE_Z" ) {
					kmlMaxImageSizeZ = token2.toInt( &ok );
					if ( !ok ) {
						return PARAM_ERROR_KML_MAX_IMAGE_SIZE;
					}
				}
			}
		}
	}

	setTimeBegin( timeBegin );
	setTimeEnd( timeEnd );
	setTimeSkip( timeSkip );
	setStateTimeConversionToUTC( stateTimeConvertionToUTC );
	setTimeConversionToUTC( timeConvertionToUTC );
	setCurrentStep( timeBegin );

	setZUnit( zUnit );
	setZScale( zScale );
	setObjectPos( objX, objY, objZ );
	setObjectAngle( objPitch, objYaw, 0.0 );
	setObjectPosSphere( objSphereX, objSphereY, objSphereZ );
	setObjectAngleSphere( objSpherePitch, objSphereYaw, 0.0 );

	setVolumeThresholdMin( volumeThresMin );
	setVolumeThresholdMax( volumeThresMax );
	setStateLogScale( logScale );
	setStateVolumeX( volumeSliceX );
	setStateVolumeY( volumeSliceY );
	setStateVolumeZ( volumeSliceZ );
	setVolumeXNum( volumeSliceXNum );
	setVolumeYNum( volumeSliceYNum );
	setVolumeZNum( volumeSliceZNum );
	setStateColorSliceX( colorSliceX );
	setStateColorSliceY( colorSliceY );
	setStateColorSliceZ( colorSliceZ );
	setColorSlicePosX( colorSliceXPos );
	setColorSlicePosY( colorSliceYPos );
	setColorSlicePosZ( colorSliceZPos );
	setStateColorSliceClampToGround( colorSliceClampToGround );
	setStateColorSliceAlpha( colorSliceAlpha );
	setColorSliceOpacity( colorSliceOpacity );

	setStateContourLineX( contourLineX );
	setStateContourLineY( contourLineY );
	setStateContourLineZ( contourLineZ );
	setContourLinePosX( contourLineXPos );
	setContourLinePosY( contourLineYPos );
	setContourLinePosZ( contourLineZPos );
	setContourLineThresholdMin( contourLineThresMin );
	setContourLineThresholdMax( contourLineThresMax );
	setContourLineInterval( contourLineInterval );
	setContourLineWidth( contourLineWidth );

	QColor backGroundColor;
	backGroundColor.setRed( backGroundColorR );
	backGroundColor.setGreen( backGroundColorG );
	backGroundColor.setBlue( backGroundColorB );

	setBGColor( backGroundColor );
	setStateBoundingBox( stateBoundingBox );
	setStateColorBar( stateColorBar );
	setStateTimeDisplay( stateTimeDisplay );
	setStateBoundingBoxAxisLabel( stateBoundingBoxAxisLabel );
	setColorBarOrientation( ColorBarOrientation );
	setColorBarType( colorBarType );
	setColorBarDecimalPlacesNum( colorBarDecimalPlacesNum );
	setColorBarMajorTickMarkNum( colorBarMajorTickMarkNum );
	setColorBarMinorTickMarkNum( colorBarMinorTickMarkNum );
	setColorBarMajorTickMarkType( colorBarMajorTickMarkType );
	setColorBarMinorTickMarkType( colorBarMinorTickMarkType );

	if ( QFile( logoFileName ).exists() ) {
		setStateLogo( stateLogo );
		setLogo( logoFileName );
	}

	if ( !existColorTableFile ) {
		if ( ( existColorTableR & existColorTableG &
			   existColorTableB & existColorTableA ) == true ) {
			setColorControlPoint(
				colorControlPointRed,
				colorControlPointGreen,
				colorControlPointBlue,
				colorControlPointAlpha );
		} else {
			return PARAM_ERROR_COLORTABLE;
		}
	}

	setKMLMaxImageSize( kmlMaxImageSizeX, kmlMaxImageSizeY, kmlMaxImageSizeZ );

	loadFile.close();

	return PARAM_NO_ERROR;
}

bool Parameter::saveColorTable( const QString &filename )
{
	QFile saveFile( filename );
	saveFile.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &saveFile );

	stream << "#\n";
	stream << "# Volume Data Visualizer for Google Earth\n";
	stream << "# ColorTable File\n";
	stream << "#\n\n";

	stream << "COLORTABLE_RED " << getColorControlPointNum( RED ) << ENDL;
	for ( int i = 0; i < getColorControlPointNum( RED ); i++ )
	{
		stream << "  " << getColorControlPoint( RED, i ).first << " "
					   << getColorControlPoint( RED, i ).second << ENDL;
	}
	stream << "COLORTABLE_GREEN " << getColorControlPointNum( GREEN ) << ENDL;
	for ( int i = 0; i < getColorControlPointNum( GREEN ); i++ )
	{
		stream << "  " << getColorControlPoint( GREEN, i ).first << " "
					   << getColorControlPoint( GREEN, i ).second << ENDL;
	}
	stream << "COLORTABLE_BLUE " << getColorControlPointNum( BLUE ) << ENDL;
	for ( int i = 0; i < getColorControlPointNum( BLUE ); i++ )
	{
		stream << "  " << getColorControlPoint( BLUE, i ).first << " "
					   << getColorControlPoint( BLUE, i ).second << ENDL;
	}
	stream << "COLORTABLE_ALPHA " << getColorControlPointNum( ALPHA ) << ENDL;
	for ( int i = 0; i < getColorControlPointNum( ALPHA ); i++ )
	{
		stream << "  " << getColorControlPoint( ALPHA, i ).first << " "
					   << getColorControlPoint( ALPHA, i ).second << ENDL;
	}
	stream << "CONNECTION_TYPE ";
	if ( m_ColorTableConnectionType == LINEAR_CONNECTION ) {
		stream << "LINEAR" << ENDL;
	} else {
		stream << "CURVE" << ENDL;
	}

	saveFile.close();

	return true;
}

ErrorCode Parameter::loadColorTable( const QString &filename )
{
	QFile loadFile( filename );
	if ( !( loadFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) ) {
		if ( appParam.getStateGUI() ) {
			QString msg = QString( "Could not load \"%1\"." ).arg( filename );
			QMessageBox::critical( 0, "File Open Error", msg );
		}
		return PARAM_ERROR_COLORTABLE_FILE_OPEN;
	}

	QTextStream inputStream( &loadFile );
	QString lineString;
	QString token1, token2;
	QStringList stringList;

	QVector< QPair< float, float > > colorControlPointRed;
	QVector< QPair< float, float > > colorControlPointGreen;
	QVector< QPair< float, float > > colorControlPointBlue;
	QVector< QPair< float, float > > colorControlPointAlpha;
	ColorTableConnectionType connectionType = CURVE_CONNECTION;

	while ( !inputStream.atEnd() ) {
		lineString = inputStream.readLine();
		if ( lineString[0] != QChar( '#' ) ) {
			stringList = splitString( "\\s+", lineString );
			if ( stringList.size() >= 2 ) {
				token1 = ( stringList.at( 0 ) ).toUpper();
				token2 = ( stringList.at( 1 ) ).toUpper();
				if ( token1 == "COLORTABLE_RED" ) {
					int cp_num = token2.toInt();
					bool ok1, ok2;
					for ( int i = 0; i < cp_num; i++ ) {
						lineString = inputStream.readLine();
						stringList = splitString( "\\s+", lineString );
						float level = ( stringList.at( 0 ) ).toFloat( &ok1 );
						float value = ( stringList.at( 1 ) ).toFloat( &ok2 );
						if ( !ok1 || !ok2 ) {
							return PARAM_ERROR_COLORTABLE_RED;
						}
						colorControlPointRed.push_back( QPair< float, float >( level, value ) );
					}
					if ( colorControlPointRed.size() != cp_num ) {
						return PARAM_ERROR_COLORTABLE_RED;
					}
				} else if ( token1 == "COLORTABLE_GREEN" ) {
					int cp_num = token2.toInt();
					bool ok1, ok2;
					for ( int i = 0; i < cp_num; i++ ) {
						lineString = inputStream.readLine();
						stringList = splitString( "\\s+", lineString );
						float level = ( stringList.at( 0 ) ).toFloat( &ok1 );
						float value = ( stringList.at( 1 ) ).toFloat( &ok2 );
						if ( !ok1 || !ok2 ) {
							return PARAM_ERROR_COLORTABLE_GREEN;
						}
						colorControlPointGreen.push_back( QPair< float, float >( level, value ) );
					}
					if ( colorControlPointGreen.size() != cp_num ) {
						return PARAM_ERROR_COLORTABLE_GREEN;
					}
				} else if ( token1 == "COLORTABLE_BLUE" ) {
					int cp_num = token2.toInt();
					bool ok1, ok2;
					for ( int i = 0; i < cp_num; i++ ) {
						lineString = inputStream.readLine();
						stringList = splitString( "\\s+", lineString );
						float level = ( stringList.at( 0 ) ).toFloat( &ok1 );
						float value = ( stringList.at( 1 ) ).toFloat( &ok2 );
						if ( !ok1 || !ok2 ) {
							return PARAM_ERROR_COLORTABLE_BLUE;
						}
						colorControlPointBlue.push_back( QPair< float, float >( level, value ) );
					}
					if ( colorControlPointBlue.size() != cp_num ) {
						return PARAM_ERROR_COLORTABLE_BLUE;
					}
				} else if ( token1 == "COLORTABLE_ALPHA" ) {
					int cp_num = token2.toInt();
					bool ok1, ok2;
					for ( int i = 0; i < cp_num; i++ ) {
						lineString = inputStream.readLine();
						stringList = splitString( "\\s+", lineString );
						float level = ( stringList.at( 0 ) ).toFloat( &ok1 );
						float value = ( stringList.at( 1 ) ).toFloat( &ok2 );
						if ( !ok1 || !ok2 ) {
							return PARAM_ERROR_COLORTABLE_ALPHA;
						}
						colorControlPointAlpha.push_back( QPair< float, float >( level, value ) );
					}
					if ( colorControlPointAlpha.size() != cp_num ) {
						return PARAM_ERROR_COLORTABLE_ALPHA;
					}
				} else if ( token1 == "CONNECTION_TYPE" ) {
					if ( token2 == "CURVE" ) {
						connectionType = CURVE_CONNECTION;
					} else if ( token2 == "LINEAR" ) {
						connectionType = LINEAR_CONNECTION;
					} else {
						return PARAM_ERROR_COLORTABLE_CONNECTION_TYPE;
					}
				}
			}
		}
	}

	setColorControlPoint(
		colorControlPointRed,
		colorControlPointGreen,
		colorControlPointBlue,
		colorControlPointAlpha );
	m_ColorTableConnectionType = connectionType;

	loadFile.close();

	return PARAM_COLORTABLE_NO_ERROR;
}

void Parameter::setObjectPos( float x, float y, float z )
{
	if ( z <= -350.0 ) z = -350.0;
	if ( z >=   25.0 ) z =   25.0;
	m_ObjectPos[ 0 ] = x;
	m_ObjectPos[ 1 ] = y;
	m_ObjectPos[ 2 ] = z;
}

void Parameter::setObjectPosSphere( float x, float y, float z )
{
	if ( z <= -350.0 ) z = -350.0;
	if ( z >=   25.0 ) z =   25.0;
	m_ObjectPosSphere[ 0 ] = x;
	m_ObjectPosSphere[ 1 ] = y;
	m_ObjectPosSphere[ 2 ] = z;
}

void Parameter::setObjectAngle( float pitch, float yaw, float roll )
{
	if ( yaw < 0.0 ) {
		yaw += 360.0;
	} else if ( yaw > 360.0 ) {
		yaw -= 360.0;
	}
	if ( pitch < -90.0 ) {
		pitch = -90.0;
	} else if ( pitch > 90.0 ) {
		pitch = 90.0;
	}
	m_ObjectAngle[ 0 ] = pitch;
	m_ObjectAngle[ 1 ] = yaw;
	m_ObjectAngle[ 2 ] = roll;
}

void Parameter::setObjectAngleSphere( float pitch, float yaw, float roll )
{
	if ( yaw < 0.0 ) {
		yaw += 360.0;
	} else if ( yaw > 360.0 ) {
		yaw -= 360.0;
	}
	if ( pitch < -90.0 ) {
		pitch = -90.0;
	} else if ( pitch > 90.0 ) {
		pitch = 90.0;
	}
	m_ObjectAngleSphere[ 0 ] = pitch;
	m_ObjectAngleSphere[ 1 ] = yaw;
	m_ObjectAngleSphere[ 2 ] = roll;
}

void Parameter::setObjectToCenter()
{
	m_ObjectPos[0] = OBJECT_POS_X;
	m_ObjectPos[1] = OBJECT_POS_Y;
	m_ObjectPos[2] = OBJECT_POS_Z;
	m_ObjectPosSphere[0] = OBJECT_POS_SPHERE_X;
	m_ObjectPosSphere[1] = OBJECT_POS_SPHERE_Y;
	m_ObjectPosSphere[2] = OBJECT_POS_SPHERE_Z;
}

void Parameter::resetColorControlPoint()
{
	m_ControlPoint_R.clear();
	m_ControlPoint_G.clear();
	m_ControlPoint_B.clear();
	m_ControlPoint_A.clear();
}

void Parameter::setColorControlPoint( ColorType colortype, QPair<float, float> controlpoint )
{
	if ( colortype == RED ) {
		m_ControlPoint_R.push_back( controlpoint );
	} else if ( colortype == GREEN ) {
		m_ControlPoint_G.push_back( controlpoint );
	} else if ( colortype == BLUE ) {
		m_ControlPoint_B.push_back( controlpoint );
	} else if ( colortype == ALPHA ) {
		m_ControlPoint_A.push_back( controlpoint );
	}
}

void Parameter::setColorControlPoint(
	QVector< QPair<float, float> > red,
	QVector< QPair<float, float> > green,
	QVector< QPair<float, float> > blue,
	QVector< QPair<float, float> > alpha )
{
	m_ControlPoint_R.clear();
	m_ControlPoint_G.clear();
	m_ControlPoint_B.clear();
	m_ControlPoint_A.clear();

	for ( int i = 0; i < red.size(); i++ ) {
		m_ControlPoint_R.push_back( red.at( i ) );
	}
	for ( int i = 0; i < green.size(); i++ ) {
		m_ControlPoint_G.push_back( green.at( i ) );
	}
	for ( int i = 0; i < blue.size(); i++ ) {
		m_ControlPoint_B.push_back( blue.at( i ) );
	}
	for ( int i = 0; i < alpha.size(); i++ ) {
		m_ControlPoint_A.push_back( alpha.at( i ) );
	}
}

int Parameter::getColorControlPointNum( ColorType colortype )
{
	if ( colortype == RED ) {
		return m_ControlPoint_R.size();
	} else if ( colortype == GREEN ) {
		return m_ControlPoint_G.size();
	} else if ( colortype == BLUE ) {
		return m_ControlPoint_B.size();
	} else if ( colortype == ALPHA ) {
		return m_ControlPoint_A.size();
	} else {
		return -1;
	}
}

QPair<float, float> Parameter::getColorControlPoint( ColorType colortype, int id )
{
	if ( colortype == RED ) {
		return m_ControlPoint_R.at( id );
	} else if ( colortype == GREEN ) {
		return m_ControlPoint_G.at( id );
	} else if ( colortype == BLUE ) {
		return m_ControlPoint_B.at( id );
	} else if ( colortype == ALPHA ) {
		return m_ControlPoint_A.at( id );
	} else {
		QPair< float, float > pair;
		pair.first = -1.0;
		pair.second = -1.0;
		return pair;
	}
}

void Parameter::setColorTable( ColorType type, int level, float value )
{
	if ( type == RED ) {
		m_ColorTableR[ level ] = value;
	} else if ( type == GREEN ) {
		m_ColorTableG[ level ] = value;
	} else if ( type == BLUE ) {
		m_ColorTableB[ level ] = value;
	} else if ( type == ALPHA ) {
		m_ColorTableA[ level ] = value;
	}
}

float Parameter::getColorTable( ColorType type, int level )
{
	if ( type == RED ) {
		return m_ColorTableR[ level ];
	} else if ( type == GREEN ) {
		return m_ColorTableG[ level ];
	} else if ( type == BLUE ) {
		return m_ColorTableB[ level ];
	} else if ( type == ALPHA ) {
		return m_ColorTableA[ level ];
	} else {
		return -1;
	}
}

float* Parameter::getColorTable( ColorType type )
{
	if ( type == RED ) {
		return m_ColorTableR;
	} else if ( type == GREEN ) {
		return m_ColorTableG;
	} else if ( type == BLUE ) {
		return m_ColorTableB;
	} else if ( type == ALPHA ) {
		return m_ColorTableA;
	} else {
		return NULL;
	}
}

void Parameter::set256Table()
{
	for ( int i = 0; i < 255; i++ ) {
		float level = static_cast<float>( i ) / 254.0;
		float value;
		value = 1.0 - interpolation( RED,   level );
		setColorTable( RED,   i, value );
		value = 1.0 - interpolation( GREEN, level );
		setColorTable( GREEN, i, value );
		value = 1.0 - interpolation( BLUE,  level );
		setColorTable( BLUE,  i, value );
		value = 1.0 - interpolation( ALPHA, level );
		setColorTable( ALPHA, i, value );
	}
	setColorTable( RED,   255, 0.0 );
	setColorTable( GREEN, 255, 0.0 );
	setColorTable( BLUE,  255, 0.0 );
	setColorTable( ALPHA, 255, 0.0 );
}

void Parameter::setKMLMaxImageSize( int x, int y, int z )
{
	m_KMLMaxImageSize[0] = x;
	m_KMLMaxImageSize[1] = y;
	m_KMLMaxImageSize[2] = z;
}

float Parameter::interpolation( ColorType type, float x )
{
	for ( int i = 1; i < getColorControlPointNum( type ); ++i ) {
		QPair<float, float> p1 = getColorControlPoint( type, i - 1 );
		QPair<float, float> p2 = getColorControlPoint( type, i );
		if( x <= p2.first ) {
			if ( m_ColorTableConnectionType == CURVE_CONNECTION ) {
				// Ferguson / Coons Curve
				double x1 = p1.first;
				double x2 = p2.first;
				double y1 = p1.second;
				double y2 = p2.second;
				double leftBoundAngle  = 0.0;
				double rightBoundAngle = 0.0;
				double dy1 = tan( leftBoundAngle );
				double dy2 = tan( rightBoundAngle );
				double a = ((dy1+dy2) - 2*(y1-y2)/(x1-x2))/pow(x1-x2, 2);
				double b = 0.5*(dy1-dy2)/(x1-x2) - 1.5*a*(x1+x2);
				double c = (y1-y2)/(x1-x2) - (x1*x1+x1*x2+x2*x2)*a - (x1+x2)*b;
				double d = y2 - (a*x2*x2*x2 + b*x2*x2 + c*x2);
				double val = a*x*x*x + b*x*x + c*x + d;
				if ( val < 0.0 ) val = 0.0;
				else if ( val > 1.0 ) val = 1.0;
				return static_cast<float>( val );
			} else {
				// Linear
				double x1 = p1.first;
				double x2 = p2.first;
				double y1 = p1.second;
				double y2 = p2.second;
				double val = ( y2 - y1 ) / ( x2 - x1 ) * ( static_cast< double >( x ) - x1 ) + y1;
				return static_cast<float>( val );
			}
		}
	}
	return 0;
}
