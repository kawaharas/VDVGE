//
// Volume Data Visualizer for Google Earth
//
// kml.cpp
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

#include "kml.h"

KML::KML()
{
	m_GUI = true;
}

void KML::clearDir( QString path )
{
	QDir kmlDir;
	kmlDir.cd( path );
	QStringList files;
	files = kmlDir.entryList( QDir::Files, QDir::Name );
	QList<QString>::iterator f;
	for ( f = files.begin(); f != files.end(); ++f ) {
		QFile tempFile( path + "/" + (*f) );
		kmlDir.remove( tempFile.fileName() );
	}
	QStringList dirs;
	dirs = kmlDir.entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
	for ( f = dirs.begin(); f != dirs.end(); ++f ) {
		this->clearDir( kmlDir.path() + "/" + (*f) );
		kmlDir.rmdir( *f );
	}
	kmlDir.cdUp();
}

bool KML::exportKML()
{
	if ( m_GUI ) {
		QString dirPath;

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
		dirPath = QFileDialog::getExistingDirectory(
			0, tr( "Choose Directory to Save" ), QDir::homePath(),
			QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog );
#else
		dirPath = QFileDialog::getExistingDirectory(
			0, tr( "Choose Directory to Save" ), QDir::homePath(),
			QFileDialog::ShowDirsOnly );
#endif
#else
#ifdef Q_OS_WIN
		dirPath = QFileDialog::getExistingDirectory(
			0, tr( "Choose Directory to Save" ), QDir::homePath(),
			QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog );
#else
		dirPath = QFileDialog::getExistingDirectory(
			0, tr( "Choose Directory to Save" ), QDir::homePath(),
			QFileDialog::ShowDirsOnly );
#endif
#endif

		if ( !dirPath.isEmpty() ) {
			m_DirPath = dirPath;
		} else {
			return false;
		}

		QDir kmlDir;
		kmlDir.cd( m_DirPath );
		if ( !kmlDir.exists( "files" ) ) {
			kmlDir.mkdir( "files" );
		} else {
			QMessageBox::StandardButton reply
				= QMessageBox::warning( 0, tr( "Warning" ),
					tr( "Directory is already exists.\nAre you sure you want to overwrite the existing file?" ),
					QMessageBox::Ok | QMessageBox::Cancel );
			if ( reply == QMessageBox::Ok ) {
				clearDir( m_DirPath + "/files" );
			} else {
				return false;
			}
		}
	} else {
		QDir kmlDir;
		m_DirPath = QDir::currentPath();
		kmlDir.cd( m_DirPath );
		if ( !kmlDir.exists( "files" ) ) {
			kmlDir.mkdir( "files" );
		} else {
			clearDir( m_DirPath + "/files" );
		}
	}

	m_AltitudeModeOfColorSliceZ = ALTITUDE_ABSOLUTE;
	if ( param.IsColorSliceClampToGround() ) {
		m_AltitudeModeOfColorSliceZ = ALTITUDE_CLAMP_TO_GROUND;
	}
/*
	m_AltitudeModeOfColorSliceZ = ALTITUDE_ABSOLUTE;
	if ( param.IsColorSliceZ() ) {
		if ( m_GUI ) {
			KMLSettingDialog *settingDialog = new KMLSettingDialog();
			settingDialog->setWindowTitle( tr( "Select Altitude Mode of ColorSlice" ) );
			settingDialog->show();
			if ( settingDialog->exec() == QDialog::Accepted ) {
				m_AltitudeModeOfColorSliceZ = settingDialog->getStateAltitudeMode();
			}
		} else {
			m_AltitudeModeOfColorSliceZ = ALTITUDE_CLAMP_TO_GROUND;
		}
	}

	m_AltitudeModeOfVolumeZ = ALTITUDE_ABSOLUTE;
	if ( ( param.IsVolumeZ() ) && ( param.getVolumeZNum() == 1 ) ) {
		if ( m_GUI ) {
			KMLSettingDialog *settingDialog = new KMLSettingDialog();
			settingDialog->setWindowTitle( tr( "Select Altitude Mode of Volume Layer" ) );
			settingDialog->setWindowModality( Qt::ApplicationModal );
			settingDialog->show();
			settingDialog->setFocus();
			if ( settingDialog->exec() == QDialog::Accepted ) {
				m_AltitudeModeOfVolumeZ = settingDialog->getStateAltitudeMode();
			}
		} else {
			m_AltitudeModeOfVolumeZ = ALTITUDE_CLAMP_TO_GROUND;
		}
	}
*/
	if ( m_GUI ) {
		m_Progress = new QProgressDialog();
		m_Progress->setWindowTitle( QString( "Export KML" ) );
		m_Progress->setFixedSize( 500, 150 );
		m_Progress->setRange( 0, 100 );
		m_Progress->setAutoClose( false );
		m_Progress->setWindowModality( Qt::ApplicationModal );
		m_Progress->show();
		m_Progress->setFocus();
	}

	initialize();

	QString kmlFileName = m_DirPath + "/doc.kml";
	QFile kmlFile( kmlFileName );
	kmlFile.open( QIODevice::WriteOnly | QIODevice::Text );
	m_Stream = new QTextStream( &kmlFile );
	m_Stream->setRealNumberNotation( QTextStream::ScientificNotation );

	*m_Stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	*m_Stream << "<kml\n";
	*m_Stream << "xmlns=\"http://earth.google.com/kml/2.2\"\n";
	*m_Stream << "xmlns:gx=\"http://www.google.com/kml/ext/2.2\"\n";
	*m_Stream << "xmlns:kml=\"http://www.opengis.net/kml/2.2\"\n";
	*m_Stream << "xmlns:atom=\"http://www.w3.org/2005/Atom\">\n";
	*m_Stream << "<Document>\n";
	QString description = grads.getVarDescription( 0 );
	*m_Stream << "\t<name>" << description << "</name>\n";
	*m_Stream << "\t<open>1</open>\n";

	*m_Stream << "\t<Region>\n";
	*m_Stream << "\t\t<LatLonAltBox>\n";
	*m_Stream << "\t\t\t<west>"  << QString::number( m_MinX, 'f', 6 ) << "</west>\n";
	*m_Stream << "\t\t\t<east>"  << QString::number( m_MaxX, 'f', 6 ) << "</east>\n";
	*m_Stream << "\t\t\t<south>" << QString::number( m_MinY, 'f', 6 ) << "</south>\n";
	*m_Stream << "\t\t\t<north>" << QString::number( m_MaxY, 'f', 6 ) << "</north>\n";
	*m_Stream << "\t\t</LatLonAltBox>\n";
	*m_Stream << "\t\t<Lod>\n";
	*m_Stream << "\t\t\t<minLodPixels>-1</minLodPixels>\n";
	*m_Stream << "\t\t\t<maxLodPixels>-1</maxLodPixels>\n";
	*m_Stream << "\t\t</Lod>\n";
	*m_Stream << "\t</Region>\n";

	exportVolume();
	exportColorSlice();
	exportContourLine();

	exportBoundingBox();
	exportColorBar();
	exportLogo();

	*m_Stream << "</Document>\n";
	*m_Stream << "</kml>\n";

	kmlFile.close();

	delete m_Stream;

	if ( m_GUI ) {
		m_Progress->close();
		delete m_Progress;
	}

	return true;
}

uchar KML::float2uchar( float value )
{
	float min = param.getVolumeThresholdMin();
	float max = param.getVolumeThresholdMax();
	if ( fabs( value ) > fabs( grads.getUndef() ) )  {
		return 255;
	} else {
		if ( value < min ) {
			return 0;
		} else if ( value > max ) {
			return 254;
		} else {
			if ( !param.IsLogScale() ) {
				return static_cast< uchar >( ( value - min ) / ( max - min ) * 254.0f );
			} else {
				return static_cast< uchar >( ( log10( value ) - log10( min ) )
					/ ( log10( max ) - log10( min ) ) * 254.0f );
			}
		}
	}
}

void KML::initialize()
{
	m_SizeX = grads.getGridSizeX();
	m_SizeY = grads.getGridSizeY();
	m_SizeZ = grads.getGridSizeZ();

	if ( m_SizeX > param.getKMLMaxImageSizeX() ) {
		m_SizeX = param.getKMLMaxImageSizeX();
	}
	if ( m_SizeY > param.getKMLMaxImageSizeY() ) {
		m_SizeY = param.getKMLMaxImageSizeY();
	}
	if ( m_SizeZ > param.getKMLMaxImageSizeZ() ) {
		m_SizeZ = param.getKMLMaxImageSizeZ();
	}

	m_MinX  = grads.getGridXMin();
	m_MaxX  = grads.getGridXMax();
	m_MinY  = grads.getGridYMin();
	m_MaxY  = grads.getGridYMax();
	m_MinZ  = grads.getGridZMin();
	m_MaxZ  = grads.getGridZMax();

	m_ColorTable.clear();
	m_ColorTableRGBA.clear();
	m_ColorTableRGB.clear();
	for ( int i = 0; i < 256; i++ ) {
		int r = static_cast< int >( param.getColorTableR( i ) * 255.0 );
		int g = static_cast< int >( param.getColorTableG( i ) * 255.0 );
		int b = static_cast< int >( param.getColorTableB( i ) * 255.0 );
		int a = static_cast< int >( param.getColorTableA( i ) * 255.0 );
		m_ColorTable.push_back( qRgba( r, g, b, a ) );
		float ao = static_cast< float >( a ) * param.getColorSliceOpacity();
		m_ColorTableRGBA.push_back( qRgba( r, g, b, static_cast< uchar >( ao ) ) );
		if ( i == 255 ) {
			m_ColorTableRGB.push_back( qRgba( r, g, b, 0 ) );
		} else {
			ao = 255.0 * param.getColorSliceOpacity();
			m_ColorTableRGB.push_back( qRgba( r, g, b, static_cast< uchar >( ao ) ) );
		}
	}

	m_MainObject = NO_OBJECT;
	if ( param.IsContourLineX() ) m_MainObject = CONTOURLINE_X;
	if ( param.IsContourLineY() ) m_MainObject = CONTOURLINE_Y;
	if ( param.IsContourLineZ() ) m_MainObject = CONTOURLINE_Z;
	if ( param.IsColorSliceX() ) m_MainObject = COLORSLICE_X;
	if ( param.IsColorSliceY() ) m_MainObject = COLORSLICE_Y;
	if ( param.IsColorSliceZ() ) m_MainObject = COLORSLICE_Z;
	if ( param.IsVolumeX() ) m_MainObject = VOLUME_X;
	if ( param.IsVolumeY() ) m_MainObject = VOLUME_Y;
	if ( param.IsVolumeZ() ) m_MainObject = VOLUME_Z;
}

void KML::exportVolume()
{
	if ( !param.IsVolumeX() &&
		 !param.IsVolumeY() &&
		 !param.IsVolumeZ() ) return;

	QDir kmlDir;
	kmlDir.cd( m_DirPath + "/files" );
	kmlDir.mkdir( "volume" );

	*m_Stream << "\t<Folder>\n";
	*m_Stream << "\t\t<name>Volume</name>\n";
	*m_Stream << "\t\t<open>0</open>\n";

	exportVolumeX();
	exportVolumeY();
	exportVolumeZ();

	*m_Stream << "\t</Folder>\n";
}

void KML::exportColorSlice()
{
	if ( !param.IsColorSliceX() &&
		 !param.IsColorSliceY() &&
		 !param.IsColorSliceZ() ) return;

	QDir kmlDir;
	kmlDir.cd( m_DirPath + "/files" );
	kmlDir.mkdir( "colorslice" );

	*m_Stream << "\t<Folder>\n";
	*m_Stream << "\t\t<name>ColorSlice</name>\n";
	*m_Stream << "\t\t<open>0</open>\n";

	exportColorSliceX();
	exportColorSliceY();
	exportColorSliceZ();

	*m_Stream << "\t</Folder>\n";
}

void KML::exportColorSliceX()
{
	if ( !param.IsColorSliceX() ) return;

	QDir dir;
	dir.cd( m_DirPath + "/files/colorslice" );
	dir.mkdir( "x" );
	dir.cd( "x" );

	// Generate Texture Images
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating image files for ColorSliceX..." ) );
	}

	double xposn = ( double )( param.getColorSlicePosX() );
	double xpos = m_MinX + ( m_MaxX - m_MinX ) * xposn;
//	double xcoord[ m_SizeX ];
	double *xcoord = new double[ m_SizeX ];
	int xidx = 0;
	if ( m_SizeX == 1 ) {
		xcoord[0] = m_MinX;
	} else {
		for ( int i = 0; i < m_SizeX ; i++ ) {
			xcoord[i] = m_MinX + ( m_MaxX - m_MinX ) / ( m_SizeX - 1 ) * i;
		}
		for ( int i = 0; i < m_SizeX - 1 ; i++ ) {
			if ( xcoord[ i + 1 ] >= xpos ) {
				xidx = i;
				break;
			}
		}
	}

	QImage image( m_SizeY, m_SizeZ, QImage::Format_Indexed8 );
	if ( param.IsColorSliceAlpha() ) {
		image.setColorTable( m_ColorTableRGBA );
	} else {
		image.setColorTable( m_ColorTableRGB );
	}
	ulong size = m_SizeX * m_SizeY * m_SizeZ;
	uchar *data = new uchar[ size ];
	std::fill( data, data + size, 0 );
	uchar *slicedata = new uchar[ m_SizeY * m_SizeZ ];
	std::fill( slicedata, slicedata + m_SizeY * m_SizeZ, 0 );

	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) {
					// cleanup memories
					delete [] xcoord;
					delete [] slicedata;
					delete [] data;
					return;
				}
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			grads.setData( time );
			interpolator( m_SizeX, m_SizeY, m_SizeZ, data );

			for ( int i = 0; i < ( m_SizeY * m_SizeZ ); i++ ) {
				ulong idx0 = m_SizeX * i + xidx;
				ulong idx1 = idx0 + 1;
				float ansf;
				if ( m_SizeX == 1 ) {
					ansf = static_cast< float >( data[ idx0 ] );
				} else {
					ansf = static_cast< float >( data[ idx0 ] )
						+ static_cast< float >( data[ idx1 ] - data[ idx0 ] ) *
						( xpos - xcoord[ xidx ] ) / ( xcoord[ xidx + 1 ] - xcoord[ xidx ] );
				}
				slicedata[ i ] = static_cast< uchar >( ansf );
			}

			for ( int i = 0; i < m_SizeZ; ++i ) {
				memcpy( image.scanLine( i ), slicedata + m_SizeY * i, m_SizeY );
			}

			QString filename = QString( m_DirPath + "/files/colorslice/x/%1.png" )
								.arg( time, 4, 10, QChar( '0' ) );
			image.mirrored( false, true ).save( filename );
		}
	}
	delete [] xcoord;
	delete [] slicedata;
	delete [] data;

	// Generate KML
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating KML for ColorSliceX..." ) );
	}

	*m_Stream << "\t\t<NetworkLink id=\"colorslicex\">\n";
	*m_Stream << "\t\t\t<name>X</name>\n";
	*m_Stream << "\t\t\t<open>0</open>\n";
	if ( m_MainObject == COLORSLICE_X ) {
		*m_Stream << "\t\t\t<visibility>1</visibility>\n";
	} else {
		*m_Stream << "\t\t\t<visibility>0</visibility>\n";
	}
	*m_Stream << "\t\t\t<Link>\n";
	*m_Stream << "\t\t\t\t<href>files/colorslice/x/x.kml</href>\n";
	*m_Stream << "\t\t\t</Link>\n";
	*m_Stream << "\t\t</NetworkLink>\n";

	double centerPosX = m_MinX + ( m_MaxX - m_MinX ) / 2.0;
	double centerPosY = m_MinY + ( m_MaxY - m_MinY ) / 2.0;

	QString filename = m_DirPath + "/files/colorslice/x/x.kml";
	QFile file( filename );
	file.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &file );
	stream.setRealNumberNotation( QTextStream::ScientificNotation );

	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
	stream << "<Document>\n";
	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) return;
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			QString TS_Begin = getTimeStringKML( getActualTime( time ) );
			QString TS_End   = getTimeStringKML( getActualTime( time + param.getTimeSkip() ) );
			stream << "\t<Placemark>\n";
			stream << "\t\t<name>T=" << TS_Begin << "</name>\n";
			stream << "\t\t<TimeSpan>\n";
			stream << "\t\t\t<begin>" << TS_Begin << "</begin>\n";
			stream << "\t\t\t<end>" << TS_End << "</end>\n";
			stream << "\t\t</TimeSpan>\n";
			stream << "\t\t<Model id=\"ModelID_ColorSliceX\">\n";
			stream << "\t\t\t<altitudeMode>absolute</altitudeMode>\n";
			stream << "\t\t\t<Location>\n";
			stream << "\t\t\t\t<longitude>" << QString::number( centerPosX, 'f', 6 ) << "</longitude>\n";
			stream << "\t\t\t\t<latitude>"  << QString::number( centerPosY, 'f', 6 ) << "</latitude>\n";
			stream << "\t\t\t\t<altitude>0.0</altitude>\n";
			stream << "\t\t\t</Location>\n";
			stream << "\t\t\t<Scale>\n";
			stream << "\t\t\t\t<x>1.0</x>\n";
			stream << "\t\t\t\t<y>1.0</y>\n";
			stream << "\t\t\t\t<z>1.0</z>\n";
			stream << "\t\t\t</Scale>\n";
			stream << "\t\t\t<Link>\n";
			QString filename = QString( "%1.dae" )
								.arg( time, 4, 10, QChar( '0' ) );
			stream << "\t\t\t\t<href>" << filename << "</href>\n";
			stream << "\t\t\t</Link>\n";
			stream << "\t\t</Model>\n";
			stream << "\t</Placemark>\n";
		}
	}
	stream << "</Document>\n";
	stream << "</kml>\n";

	file.close();

	// Genarate COLLADA
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating COLLADA files for ColorSliceX..." ) );
	}

	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ ) {
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) return;
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			QString filename = QString( m_DirPath + "/files/colorslice/x/%1.dae" )
								.arg( time, 4, 10, QChar( '0' ) );
			QFile file( filename );
			file.open( QIODevice::WriteOnly | QIODevice::Text );
			QTextStream stream( &file );
			stream.setRealNumberNotation( QTextStream::ScientificNotation );

			stream << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n";
			stream << "<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">\n";
			stream << "\t<asset>\n";
			stream << "\t\t<unit meter=\"1.000000\" name=\"meter\" />\n";
			stream << "\t\t<up_axis>Z_UP</up_axis>\n";
			stream << "\t</asset>\n";
			stream << "\t<library_geometries>\n";

			double height0 = static_cast< double >( m_MinZ * param.getZScale() );
			double height1 = static_cast< double >( m_MaxZ * param.getZScale() );
			if ( param.getZUnit() == ZUNIT_KILOMETER ) {
				height0 *= 1000.0;
				height1 *= 1000.0;
			}
			height0 += EARTH_RADIUS_WE_WGS84;
			height1 += EARTH_RADIUS_WE_WGS84;
			double rangeDegLat = m_MaxY - m_MinY;
			double rangeDegLon = m_MaxX - m_MinX;
			double rangeRadLat = rangeDegLat / 180.0f * M_PI;
			double rangeRadLon = rangeDegLon / 180.0f * M_PI;
			int    divLat = ( int )( rangeDegLat / 1.5f );
			if ( divLat == 0 ) {
				divLat = 1;
			}
			double dRadLat = rangeRadLat / ( double )divLat;
			double dTexLat = 1.0f / ( double )divLat;
			double radLatitude = ( 90.0 - m_MaxY ) * M_PI / 180.0;
			double radLongitude = m_MinX * M_PI / 180.0;
			double lon = ( xpos * M_PI / 180.0 )
					   - ( radLongitude + rangeRadLon / 2.0 ) - M_PI / 2.0;

			stream << "\t\t<geometry id=\"ID_Geometry_1\">\n";
			stream << "\t\t\t<mesh>\n";
			stream << "\t\t\t\t<source id=\"ID_Geometry_Position_1\">\n";
			stream << "\t\t\t\t\t<float_array id=\"ID_Geometry_Position_Array_1\" count=\"" << 12 * divLat << "\">\n";

			for ( int ui = 0; ui < divLat; ui++ )
			{
				double lat0 = radLatitude + dRadLat *   ui;
				double lat1 = radLatitude + dRadLat * ( ui + 1 );
				double vert00[3] = { height0 * sin( lat0 ) * cos( lon ),
									 height0 * sin( lat0 ) * sin( lon ),
									 height0 * cos( lat0 ) };
				double vert10[3] = { height0 * sin( lat1 ) * cos( lon ),
									 height0 * sin( lat1 ) * sin( lon ),
									 height0 * cos( lat1 ) };
				double vert11[3] = { height1 * sin( lat1 ) * cos( lon ),
									 height1 * sin( lat1 ) * sin( lon ),
									 height1 * cos( lat1 ) };
				double vert01[3] = { height1 * sin( lat0 ) * cos( lon ),
									 height1 * sin( lat0 ) * sin( lon ),
									 height1 * cos( lat0 ) };

				double v00[3], v10[3], v11[3], v01[3];
				for ( int i = 0; i < 3; i++ ) {
					v00[i] = vert00[i];
					v10[i] = vert10[i];
					v11[i] = vert11[i];
					v01[i] = vert01[i];
				}
				double lotation = - ( radLatitude + rangeRadLat / 2.0 );
				vert00[0] = v00[0];
				vert00[1] = v00[1] * cos( lotation ) - v00[2] * sin( lotation );
				vert00[2] = v00[1] * sin( lotation ) + v00[2] * cos( lotation );
				vert10[0] = v10[0];
				vert10[1] = v10[1] * cos( lotation ) - v10[2] * sin( lotation );
				vert10[2] = v10[1] * sin( lotation ) + v10[2] * cos( lotation );
				vert11[0] = v11[0];
				vert11[1] = v11[1] * cos( lotation ) - v11[2] * sin( lotation );
				vert11[2] = v11[1] * sin( lotation ) + v11[2] * cos( lotation );
				vert01[0] = v01[0];
				vert01[1] = v01[1] * cos( lotation ) - v01[2] * sin( lotation );
				vert01[2] = v01[1] * sin( lotation ) + v01[2] * cos( lotation );

				vert00[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );
				vert10[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );
				vert11[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );
				vert01[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );

				stream << "\t\t\t\t\t\t"
					   << QString::number( vert00[0], 'f', 6 ) << " "
					   << QString::number( vert00[1], 'f', 6 ) << " "
					   << QString::number( vert00[2], 'f', 6 ) << "\n";
				stream << "\t\t\t\t\t\t"
					   << QString::number( vert10[0], 'f', 6 ) << " "
					   << QString::number( vert10[1], 'f', 6 ) << " "
					   << QString::number( vert10[2], 'f', 6 ) << "\n";
				stream << "\t\t\t\t\t\t"
					   << QString::number( vert11[0], 'f', 6 ) << " "
					   << QString::number( vert11[1], 'f', 6 ) << " "
					   << QString::number( vert11[2], 'f', 6 ) << "\n";
				stream << "\t\t\t\t\t\t"
					   << QString::number( vert01[0], 'f', 6 ) << " "
					   << QString::number( vert01[1], 'f', 6 ) << " "
					   << QString::number( vert01[2], 'f', 6 ) << "\n";
			}
			stream << "\t\t\t\t\t</float_array>\n";
			stream << "\t\t\t\t\t<technique_common>\n";
			stream << "\t\t\t\t\t\t<accessor count=\"" << 4 * divLat << "\" source=\"#ID_Geometry_Position_Array_1\" stride=\"3\">\n";
			stream << "\t\t\t\t\t\t\t<param name=\"X\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t\t<param name=\"Y\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t\t<param name=\"Z\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t</accessor>\n";
			stream << "\t\t\t\t\t</technique_common>\n";
			stream << "\t\t\t\t</source>\n";

			stream << "\t\t\t\t<source id=\"ID_Geometry_Normal_1\">\n";
			stream << "\t\t\t\t\t<float_array id=\"ID_Geometry_Normal_Array_1\" count=\"12\">\n";
			stream << "\t\t\t\t\t\t 1.0 0.0 0.0\n";
			stream << "\t\t\t\t\t\t 1.0 0.0 0.0\n";
			stream << "\t\t\t\t\t\t 1.0 0.0 0.0\n";
			stream << "\t\t\t\t\t\t 1.0 0.0 0.0\n";
			stream << "\t\t\t\t\t</float_array>\n";
			stream << "\t\t\t\t\t<technique_common>\n";
			stream << "\t\t\t\t\t\t<accessor count=\"4\" source=\"#ID_Geometry_Normal_Array_1\" stride=\"3\">\n";
			stream << "\t\t\t\t\t\t\t<param name=\"X\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t\t<param name=\"Y\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t\t<param name=\"Z\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t</accessor>\n";
			stream << "\t\t\t\t\t</technique_common>\n";
			stream << "\t\t\t\t</source>\n";

			stream << "\t\t\t\t<source id=\"ID_Geometry_Texcoord_1\">\n";
			stream << "\t\t\t\t\t<float_array id=\"ID_Geometry_Texcoord_Array_1\" count=\"" << 8 * divLat << "\">\n";
			for ( int ui = 0; ui < divLat; ui++ )
			{
				double texLat0 = 1.0 - ( dTexLat *   ui );
				double texLat1 = 1.0 - ( dTexLat * ( ui + 1 ) );

				if ( texLat0 == 0.00 ) texLat0 = 0.01;
				if ( texLat0 == 1.00 ) texLat0 = 0.99;
				if ( texLat1 == 0.00 ) texLat1 = 0.01;
				if ( texLat1 == 1.00 ) texLat1 = 0.99;

				double TexCoord00[2] = { texLat0, 0.01 };
				double TexCoord10[2] = { texLat1, 0.01 };
				double TexCoord11[2] = { texLat1, 0.99 };
				double TexCoord01[2] = { texLat0, 0.99 };

				stream << "\t\t\t\t\t\t"
					   << QString::number( TexCoord00[0], 'f', 6 ) << " "
					   << QString::number( TexCoord00[1], 'f', 6 ) << "\n";
				stream << "\t\t\t\t\t\t"
					   << QString::number( TexCoord10[0], 'f', 6 ) << " "
					   << QString::number( TexCoord10[1], 'f', 6 ) << "\n";
				stream << "\t\t\t\t\t\t"
					   << QString::number( TexCoord11[0], 'f', 6 ) << " "
					   << QString::number( TexCoord11[1], 'f', 6 ) << "\n";
				stream << "\t\t\t\t\t\t"
					   << QString::number( TexCoord01[0], 'f', 6 ) << " "
					   << QString::number( TexCoord01[1], 'f', 6 ) << "\n";
			}
			stream << "\t\t\t\t\t</float_array>\n";
			stream << "\t\t\t\t\t<technique_common>\n";
			stream << "\t\t\t\t\t\t<accessor count=\"" << 4 * divLat << "\" source=\"#ID_Geometry_Texcoord_Array_1\" stride=\"2\">\n";
			stream << "\t\t\t\t\t\t\t<param name=\"S\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t\t<param name=\"T\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t</accessor>\n";
			stream << "\t\t\t\t\t</technique_common>\n";
			stream << "\t\t\t\t</source>\n";

			stream << "\t\t\t\t<vertices id=\"ID_Geometry_Vertex_1\">\n";
			stream << "\t\t\t\t\t<input semantic=\"POSITION\" source=\"#ID_Geometry_Position_1\" />\n";
			stream << "\t\t\t\t\t<input semantic=\"NORMAL\" source=\"#ID_Geometry_Normal_1\" />\n";
			stream << "\t\t\t\t</vertices>\n";
			stream << "\t\t\t\t<triangles count=\"" << 4 * divLat << "\" material=\"Material2\">\n";
			stream << "\t\t\t\t\t<input offset=\"0\" semantic=\"VERTEX\" source=\"#ID_Geometry_Vertex_1\" />\n";
			stream << "\t\t\t\t\t<input offset=\"1\" semantic=\"TEXCOORD\" source=\"#ID_Geometry_Texcoord_1\" />\n";
			stream << "\t\t\t\t\t<p>\n";
			for ( int ui = 0; ui < divLat; ui++ )
			{
				int increment = 4 * ui;
				stream << "\t\t\t\t\t" << 0 + increment << " " << 0 + increment << " "
									   << 1 + increment << " " << 1 + increment << " "
									   << 2 + increment << " " << 2 + increment << " "
									   << 2 + increment << " " << 2 + increment << " "
									   << 3 + increment << " " << 3 + increment << " "
									   << 0 + increment << " " << 0 + increment << "\n";
				stream << "\t\t\t\t\t" << 2 + increment << " " << 2 + increment << " "
									   << 1 + increment << " " << 1 + increment << " "
									   << 0 + increment << " " << 0 + increment << " "
									   << 0 + increment << " " << 0 + increment << " "
									   << 3 + increment << " " << 3 + increment << " "
									   << 2 + increment << " " << 2 + increment << "\n";
			}
			stream << "\t\t\t\t\t</p>\n";
			stream << "\t\t\t\t</triangles>\n";
			stream << "\t\t\t</mesh>\n";
			stream << "\t\t</geometry>\n";

			stream << "\t</library_geometries>\n";
			stream << "\t<library_materials>\n";

			stream << "\t\t<material id=\"ID_Material_1\">\n";
			stream << "\t\t\t<instance_effect url=\"#ID_Effect_1\" />\n";
			stream << "\t\t</material>\n";

			stream << "\t</library_materials>\n";
			stream << "\t<library_effects>\n";

			stream << "\t\t<effect id=\"ID_Effect_1\">\n";
			stream << "\t\t\t<profile_COMMON>\n";
			stream << "\t\t\t\t<newparam sid=\"ID_Effect_Side_1\">\n";
			stream << "\t\t\t\t\t<surface type=\"2D\">\n";
			stream << "\t\t\t\t\t\t<init_from>ID_Image_1</init_from>\n";
			stream << "\t\t\t\t\t</surface>\n";
			stream << "\t\t\t\t</newparam>\n";
			stream << "\t\t\t\t<newparam sid=\"ID_Image_Side_1\">\n";
			stream << "\t\t\t\t\t<sampler2D>\n";
			stream << "\t\t\t\t\t\t<source>ID_Effect_Side_1</source>\n";
			stream << "\t\t\t\t\t\t<wrap_s>BORDER</wrap_s>\n";
			stream << "\t\t\t\t\t\t<wrap_t>BORDER</wrap_t>\n";
			stream << "\t\t\t\t\t\t<minfilter>LINEAR</minfilter>\n";
			stream << "\t\t\t\t\t\t<magfilter>LINEAR</magfilter>\n";
			stream << "\t\t\t\t\t</sampler2D>\n";
			stream << "\t\t\t\t</newparam>\n";
			stream << "\t\t\t\t<technique sid=\"COMMON\">\n";
			stream << "\t\t\t\t\t<lambert>\n";
			stream << "\t\t\t\t\t\t<emission><color>1 1 1 1</color></emission>\n";
			stream << "\t\t\t\t\t\t<ambient><color>1 1 1 1</color></ambient>\n";
			stream << "\t\t\t\t\t\t<diffuse>\n";
			stream << "\t\t\t\t\t\t\t<texture texture=\"ID_Image_Side_1\" texcoord=\"UVSET0\" />\n";
			stream << "\t\t\t\t\t\t</diffuse>\n";
			stream << "\t\t\t\t\t</lambert>\n";
			stream << "\t\t\t\t</technique>\n";
			stream << "\t\t\t</profile_COMMON>\n";
			stream << "\t\t</effect>\n";

			stream << "\t</library_effects>\n";
			stream << "\t<library_images>\n";

			stream << "\t\t<image id=\"ID_Image_1\">\n";
			QString imageFileName = QString( "%1.png" )
									.arg( time, 4, 10, QChar( '0' ) );
			stream << "\t\t\t<init_from>" << imageFileName << "</init_from>\n";
			stream << "\t\t</image>\n";

			stream << "\t</library_images>\n";
			stream << "\t<library_visual_scenes>\n";
			stream << "\t\t<visual_scene id=\"ID_Scene_1\">\n";
			stream << "\t\t\t<node>\n";

			stream << "\t\t\t\t<instance_geometry url=\"#ID_Geometry_1\">\n";
			stream << "\t\t\t\t\t<bind_material>\n";
			stream << "\t\t\t\t\t\t<technique_common>\n";
			stream << "\t\t\t\t\t\t\t<instance_material symbol=\"Material2\" target=\"#ID_Material_1\">\n";
			stream << "\t\t\t\t\t\t\t\t<bind_vertex_input semantic=\"UVSET0\" input_semantic=\"TEXCOORD\" input_set=\"0\" />\n";
			stream << "\t\t\t\t\t\t\t</instance_material>\n";
			stream << "\t\t\t\t\t\t</technique_common>\n";
			stream << "\t\t\t\t\t</bind_material>\n";
			stream << "\t\t\t\t</instance_geometry>\n";

			stream << "\t\t\t</node>\n";
			stream << "\t\t</visual_scene>\n";
			stream << "\t</library_visual_scenes>\n";
			stream << "\t<scene>\n";
			stream << "\t\t<instance_visual_scene url=\"#ID_Scene_1\" />\n";
			stream << "\t</scene>\n";
			stream << "</COLLADA>\n";

			file.close();
		}
	}
}

void KML::exportColorSliceY()
{
	if ( !param.IsColorSliceY() ) return;

	QDir dir;
	dir.cd( m_DirPath + "/files/colorslice" );
	dir.mkdir( "y" );
	dir.cd( "y" );

	// Generate Texture Images
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating image files for ColorSliceY..." ) );
	}

	double yposn = ( double )( param.getColorSlicePosY() );
	double ypos = m_MinY + ( m_MaxY - m_MinY ) * yposn;
//	double ycoord[ m_SizeY ];
	double *ycoord = new double[ m_SizeY ];
	int yidx = 0;
	if ( m_SizeY == 1 ) {
		ycoord[0] = m_MinY;
	} else {
		for ( int i = 0; i < m_SizeY ; i++ ) {
			ycoord[i] = m_MinY + ( m_MaxY - m_MinY ) / ( m_SizeY - 1 ) * i;
		}
		for ( int i = 0; i < m_SizeY - 1 ; i++ ) {
			if ( ycoord[ i + 1 ] >= ypos ) {
				yidx = i;
				break;
			}
		}
	}

	QImage image( m_SizeX, m_SizeZ, QImage::Format_Indexed8 );
	if ( param.IsColorSliceAlpha() ) {
		image.setColorTable( m_ColorTableRGBA );
	} else {
		image.setColorTable( m_ColorTableRGB );
	}
	ulong size = m_SizeX * m_SizeY * m_SizeZ;
	uchar *data = new uchar[ size ];
	std::fill( data, data + size, 0 );
	uchar *slicedata = new uchar[ m_SizeX * m_SizeZ ];
	std::fill( slicedata, slicedata + m_SizeX * m_SizeZ, 0 );
	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) {
					// cleanup memories
					delete [] ycoord;
					delete [] slicedata;
					delete [] data;
					return;
				}
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			grads.setData( time );
			interpolator( m_SizeX, m_SizeY, m_SizeZ, data );

			for ( int j = 0; j < m_SizeZ; j++ ) {
				for ( int i = 0; i < m_SizeX; i++ ) {
					ulong idx0 = m_SizeX * m_SizeY * j + i + m_SizeX * yidx;
					ulong idx1 = m_SizeX * m_SizeY * j + i + m_SizeX * ( yidx + 1 );
					float ansf;
					if ( m_SizeY == 1 ) {
						ansf = static_cast< float >( data[ idx0 ] );
					} else {
						ansf = static_cast< float >( data[ idx0 ] )
							+ static_cast< float >( data[ idx1 ] - data[ idx0 ] ) *
							( ypos - ycoord[ yidx ] ) / ( ycoord[ yidx + 1 ] - ycoord[ yidx ] );
					}
					slicedata[ m_SizeX * j + i ] = static_cast< uchar >( ansf );
				}
			}

			for ( int i = 0; i < m_SizeZ; ++i ) {
				memcpy( image.scanLine( i ), slicedata + m_SizeX * i, m_SizeX );
			}

			QString filename = QString( m_DirPath + "/files/colorslice/y/%1.png" )
								.arg( time, 4, 10, QChar( '0' ) );
			image.mirrored( false, true ).save( filename );
		}
	}
	delete [] ycoord;
	delete [] slicedata;
	delete [] data;

	// Generate KML
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating KML for ColorSliceY..." ) );
	}

	*m_Stream << "\t\t<NetworkLink id=\"colorslicey\">\n";
	*m_Stream << "\t\t\t<name>Y</name>\n";
	*m_Stream << "\t\t\t<open>0</open>\n";
	if ( m_MainObject == COLORSLICE_Y ) {
		*m_Stream << "\t\t\t<visibility>1</visibility>\n";
	} else {
		*m_Stream << "\t\t\t<visibility>0</visibility>\n";
	}
	*m_Stream << "\t\t\t<Link>\n";
	*m_Stream << "\t\t\t\t<href>files/colorslice/y/y.kml</href>\n";
	*m_Stream << "\t\t\t</Link>\n";
	*m_Stream << "\t\t</NetworkLink>\n";

	double centerPosX = m_MinX + ( m_MaxX - m_MinX ) / 2.0;
	double centerPosY = m_MinY + ( m_MaxY - m_MinY ) / 2.0;

	QString filename = m_DirPath + "/files/colorslice/y/y.kml";
	QFile file( filename );
	file.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &file );
	stream.setRealNumberNotation( QTextStream::ScientificNotation );

	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
	stream << "<Document>\n";
	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) return;
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			QString TS_Begin = getTimeStringKML( getActualTime( time ) );
			QString TS_End   = getTimeStringKML( getActualTime( time + param.getTimeSkip() ) );
			stream << "\t<Placemark>\n";
			stream << "\t\t<name>T=" << TS_Begin << "</name>\n";
			stream << "\t\t<TimeSpan>\n";
			stream << "\t\t\t<begin>" << TS_Begin << "</begin>\n";
			stream << "\t\t\t<end>" << TS_End << "</end>\n";
			stream << "\t\t</TimeSpan>\n";
			stream << "\t\t<Model id=\"ModelID_ColorSliceY\">\n";
			stream << "\t\t\t<altitudeMode>absolute</altitudeMode>\n";
			stream << "\t\t\t<Location>\n";
			stream << "\t\t\t\t<longitude>" << QString::number( centerPosX, 'f', 6 ) << "</longitude>\n";
			stream << "\t\t\t\t<latitude>"  << QString::number( centerPosY, 'f', 6 ) << "</latitude>\n";
			stream << "\t\t\t\t<altitude>0.0</altitude>\n";
			stream << "\t\t\t</Location>\n";
			stream << "\t\t\t<Scale>\n";
			stream << "\t\t\t\t<x>1.0</x>\n";
			stream << "\t\t\t\t<y>1.0</y>\n";
			stream << "\t\t\t\t<z>1.0</z>\n";
			stream << "\t\t\t</Scale>\n";
			stream << "\t\t\t<Link>\n";
			QString filename = QString( "%1.dae" )
								.arg( time, 4, 10, QChar( '0' ) );
			stream << "\t\t\t\t<href>" << filename << "</href>\n";
			stream << "\t\t\t</Link>\n";
			stream << "\t\t</Model>\n";
			stream << "\t</Placemark>\n";
		}
	}
	stream << "</Document>\n";
	stream << "</kml>\n";

	file.close();

	// Generate COLLADA
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating COLLADA files for ColorSliceY..." ) );
	}

	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ ) {
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) return;
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			QString filename = QString( m_DirPath + "/files/colorslice/y/%1.dae" )
								.arg( time, 4, 10, QChar( '0' ) );
			QFile file( filename );
			file.open( QIODevice::WriteOnly | QIODevice::Text );
			QTextStream stream( &file );
			stream.setRealNumberNotation( QTextStream::ScientificNotation );

			stream << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n";
			stream << "<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">\n";
			stream << "\t<asset>\n";
			stream << "\t\t<unit meter=\"1.000000\" name=\"meter\" />\n";
			stream << "\t\t<up_axis>Z_UP</up_axis>\n";
			stream << "\t</asset>\n";
			stream << "\t<library_geometries>\n";

			double height0 = static_cast< double >( m_MinZ * param.getZScale() );
			double height1 = static_cast< double >( m_MaxZ * param.getZScale() );
			if ( param.getZUnit() == ZUNIT_KILOMETER ) {
				height0 *= 1000.0;
				height1 *= 1000.0;
			}
			height0 += EARTH_RADIUS_WE_WGS84;
			height1 += EARTH_RADIUS_WE_WGS84;
			double rangeDegLat = m_MaxY - m_MinY;
			double rangeDegLon = m_MaxX - m_MinX;
			double rangeRadLat = rangeDegLat / 180.0f * M_PI;
			double rangeRadLon = rangeDegLon / 180.0f * M_PI;
			int    divLon = ( int )( rangeDegLon / 1.5f );
			if ( divLon == 0 ) {
				divLon = 1;
			}
			double dRadLon = rangeRadLon / ( double )divLon;
			double dTexLon = 1.0f / ( double )divLon;
			double radLatitude = ( 90.0 - m_MaxY ) * M_PI / 180.0;
			double lat = ( 90.0 - ypos ) * M_PI / 180.0;

			stream << "\t\t<geometry id=\"ID_Geometry_1\">\n";
			stream << "\t\t\t<mesh>\n";
			stream << "\t\t\t\t<source id=\"ID_Geometry_Position_1\">\n";
			stream << "\t\t\t\t\t<float_array id=\"ID_Geometry_Position_Array_1\" count=\"" << 12 * divLon << "\">\n";

			for ( int ui = 0; ui < divLon; ui++ )
			{
				double lon0 = - rangeRadLon / 2.0 + dRadLon *   ui       - M_PI / 2.0;
				double lon1 = - rangeRadLon / 2.0 + dRadLon * ( ui + 1 ) - M_PI / 2.0;
				double vert00[3] = { height0 * sin( lat ) * cos( lon0 ),
									 height0 * sin( lat ) * sin( lon0 ),
									 height0 * cos( lat ) };
				double vert10[3] = { height0 * sin( lat ) * cos( lon1 ),
									 height0 * sin( lat ) * sin( lon1 ),
									 height0 * cos( lat ) };
				double vert11[3] = { height1 * sin( lat ) * cos( lon1 ),
									 height1 * sin( lat ) * sin( lon1 ),
									 height1 * cos( lat ) };
				double vert01[3] = { height1 * sin( lat ) * cos( lon0 ),
									 height1 * sin( lat ) * sin( lon0 ),
									 height1 * cos( lat ) };

				double v00[3], v10[3], v11[3], v01[3];
				for ( int i = 0; i < 3; i++ ) {
					v00[i] = vert00[i];
					v10[i] = vert10[i];
					v11[i] = vert11[i];
					v01[i] = vert01[i];
				}
				double lotation = - ( radLatitude + rangeRadLat / 2.0 );
				vert00[0] = v00[0];
				vert00[1] = v00[1] * cos( lotation ) - v00[2] * sin( lotation );
				vert00[2] = v00[1] * sin( lotation ) + v00[2] * cos( lotation );
				vert10[0] = v10[0];
				vert10[1] = v10[1] * cos( lotation ) - v10[2] * sin( lotation );
				vert10[2] = v10[1] * sin( lotation ) + v10[2] * cos( lotation );
				vert11[0] = v11[0];
				vert11[1] = v11[1] * cos( lotation ) - v11[2] * sin( lotation );
				vert11[2] = v11[1] * sin( lotation ) + v11[2] * cos( lotation );
				vert01[0] = v01[0];
				vert01[1] = v01[1] * cos( lotation ) - v01[2] * sin( lotation );
				vert01[2] = v01[1] * sin( lotation ) + v01[2] * cos( lotation );

				vert00[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );
				vert10[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );
				vert11[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );
				vert01[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );

				stream << "\t\t\t\t\t\t" << vert00[0] << " " << vert00[1] << " " << vert00[2] << "\n";
				stream << "\t\t\t\t\t\t" << vert10[0] << " " << vert10[1] << " " << vert10[2] << "\n";
				stream << "\t\t\t\t\t\t" << vert11[0] << " " << vert11[1] << " " << vert11[2] << "\n";
				stream << "\t\t\t\t\t\t" << vert01[0] << " " << vert01[1] << " " << vert01[2] << "\n";
			}
			stream << "\t\t\t\t\t</float_array>\n";
			stream << "\t\t\t\t\t<technique_common>\n";
			stream << "\t\t\t\t\t\t<accessor count=\"" << 4 * divLon << "\" source=\"#ID_Geometry_Position_Array_1\" stride=\"3\">\n";
			stream << "\t\t\t\t\t\t\t<param name=\"X\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t\t<param name=\"Y\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t\t<param name=\"Z\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t</accessor>\n";
			stream << "\t\t\t\t\t</technique_common>\n";
			stream << "\t\t\t\t</source>\n";

			stream << "\t\t\t\t<source id=\"ID_Geometry_Normal_1\">\n";
			stream << "\t\t\t\t\t<float_array id=\"ID_Geometry_Normal_Array_1\" count=\"12\">\n";
			stream << "\t\t\t\t\t\t 0.0 -1.0 0.0\n";
			stream << "\t\t\t\t\t\t 0.0 -1.0 0.0\n";
			stream << "\t\t\t\t\t\t 0.0 -1.0 0.0\n";
			stream << "\t\t\t\t\t\t 0.0 -1.0 0.0\n";
			stream << "\t\t\t\t\t</float_array>\n";
			stream << "\t\t\t\t\t<technique_common>\n";
			stream << "\t\t\t\t\t\t<accessor count=\"4\" source=\"#ID_Geometry_Normal_Array_1\" stride=\"3\">\n";
			stream << "\t\t\t\t\t\t\t<param name=\"X\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t\t<param name=\"Y\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t\t<param name=\"Z\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t</accessor>\n";
			stream << "\t\t\t\t\t</technique_common>\n";
			stream << "\t\t\t\t</source>\n";

			stream << "\t\t\t\t<source id=\"ID_Geometry_Texcoord_1\">\n";
			stream << "\t\t\t\t\t<float_array id=\"ID_Geometry_Texcoord_Array_1\" count=\"" << 8 * divLon << "\">\n";
			for ( int ui = 0; ui < divLon; ui++ )
			{
				double texLon0 = dTexLon *   ui;
				double texLon1 = dTexLon * ( ui + 1 );

				if ( texLon0 == 0.00 ) texLon0 = 0.01;
				if ( texLon0 == 1.00 ) texLon0 = 0.99;
				if ( texLon1 == 0.00 ) texLon1 = 0.01;
				if ( texLon1 == 1.00 ) texLon1 = 0.99;

				double TexCoord00[2] = { texLon0, 0.01 };
				double TexCoord10[2] = { texLon1, 0.01 };
				double TexCoord11[2] = { texLon1, 0.99 };
				double TexCoord01[2] = { texLon0, 0.99 };

				stream << "\t\t\t\t\t\t"
					   << QString::number( TexCoord00[0], 'f', 6 ) << " "
					   << QString::number( TexCoord00[1], 'f', 6 ) << "\n";
				stream << "\t\t\t\t\t\t"
					   << QString::number( TexCoord10[0], 'f', 6 ) << " "
					   << QString::number( TexCoord10[1], 'f', 6 ) << "\n";
				stream << "\t\t\t\t\t\t"
					   << QString::number( TexCoord11[0], 'f', 6 ) << " "
					   << QString::number( TexCoord11[1], 'f', 6 ) << "\n";
				stream << "\t\t\t\t\t\t"
					   << QString::number( TexCoord01[0], 'f', 6 ) << " "
					   << QString::number( TexCoord01[1], 'f', 6 ) << "\n";
			}
			stream << "\t\t\t\t\t</float_array>\n";
			stream << "\t\t\t\t\t<technique_common>\n";
			stream << "\t\t\t\t\t\t<accessor count=\"" << 4 * divLon << "\" source=\"#ID_Geometry_Texcoord_Array_1\" stride=\"2\">\n";
			stream << "\t\t\t\t\t\t\t<param name=\"S\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t\t<param name=\"T\" type=\"float\" />\n";
			stream << "\t\t\t\t\t\t</accessor>\n";
			stream << "\t\t\t\t\t</technique_common>\n";
			stream << "\t\t\t\t</source>\n";

			stream << "\t\t\t\t<vertices id=\"ID_Geometry_Vertex_1\">\n";
			stream << "\t\t\t\t\t<input semantic=\"POSITION\" source=\"#ID_Geometry_Position_1\" />\n";
			stream << "\t\t\t\t\t<input semantic=\"NORMAL\" source=\"#ID_Geometry_Normal_1\" />\n";
			stream << "\t\t\t\t</vertices>\n";
			stream << "\t\t\t\t<triangles count=\"" << 4 * divLon << "\" material=\"Material2\">\n";
			stream << "\t\t\t\t\t<input offset=\"0\" semantic=\"VERTEX\" source=\"#ID_Geometry_Vertex_1\" />\n";
			stream << "\t\t\t\t\t<input offset=\"1\" semantic=\"TEXCOORD\" source=\"#ID_Geometry_Texcoord_1\" />\n";
			stream << "\t\t\t\t\t<p>\n";
			for ( int ui = 0; ui < divLon; ui++ )
			{
				int increment = 4 * ui;
				stream << "\t\t\t\t\t" << 0 + increment << " " << 0 + increment << " "
									   << 1 + increment << " " << 1 + increment << " "
									   << 2 + increment << " " << 2 + increment << " "
									   << 2 + increment << " " << 2 + increment << " "
									   << 3 + increment << " " << 3 + increment << " "
									   << 0 + increment << " " << 0 + increment << "\n";
				stream << "\t\t\t\t\t" << 2 + increment << " " << 2 + increment << " "
									   << 1 + increment << " " << 1 + increment << " "
									   << 0 + increment << " " << 0 + increment << " "
									   << 0 + increment << " " << 0 + increment << " "
									   << 3 + increment << " " << 3 + increment << " "
									   << 2 + increment << " " << 2 + increment << "\n";
			}
			stream << "\t\t\t\t\t</p>\n";
			stream << "\t\t\t\t</triangles>\n";
			stream << "\t\t\t</mesh>\n";
			stream << "\t\t</geometry>\n";

			stream << "\t</library_geometries>\n";
			stream << "\t<library_materials>\n";

			stream << "\t\t<material id=\"ID_Material_1\">\n";
			stream << "\t\t\t<instance_effect url=\"#ID_Effect_1\" />\n";
			stream << "\t\t</material>\n";

			stream << "\t</library_materials>\n";
			stream << "\t<library_effects>\n";

			stream << "\t\t<effect id=\"ID_Effect_1\">\n";
			stream << "\t\t\t<profile_COMMON>\n";
			stream << "\t\t\t\t<newparam sid=\"ID_Effect_Side_1\">\n";
			stream << "\t\t\t\t\t<surface type=\"2D\">\n";
			stream << "\t\t\t\t\t\t<init_from>ID_Image_1</init_from>\n";
			stream << "\t\t\t\t\t</surface>\n";
			stream << "\t\t\t\t</newparam>\n";
			stream << "\t\t\t\t<newparam sid=\"ID_Image_Side_1\">\n";
			stream << "\t\t\t\t\t<sampler2D>\n";
			stream << "\t\t\t\t\t\t<source>ID_Effect_Side_1</source>\n";
			stream << "\t\t\t\t\t\t<wrap_s>BORDER</wrap_s>\n";
			stream << "\t\t\t\t\t\t<wrap_t>BORDER</wrap_t>\n";
			stream << "\t\t\t\t\t\t<minfilter>LINEAR</minfilter>\n";
			stream << "\t\t\t\t\t\t<magfilter>LINEAR</magfilter>\n";
			stream << "\t\t\t\t\t</sampler2D>\n";
			stream << "\t\t\t\t</newparam>\n";
			stream << "\t\t\t\t<technique sid=\"COMMON\">\n";
			stream << "\t\t\t\t\t<lambert>\n";
			stream << "\t\t\t\t\t\t<emission><color>1 1 1 1</color></emission>\n";
			stream << "\t\t\t\t\t\t<ambient><color>1 1 1 1</color></ambient>\n";
			stream << "\t\t\t\t\t\t<diffuse>\n";
			stream << "\t\t\t\t\t\t\t<texture texture=\"ID_Image_Side_1\" texcoord=\"UVSET0\" />\n";
			stream << "\t\t\t\t\t\t</diffuse>\n";
			stream << "\t\t\t\t\t</lambert>\n";
			stream << "\t\t\t\t</technique>\n";
			stream << "\t\t\t</profile_COMMON>\n";
			stream << "\t\t</effect>\n";

			stream << "\t</library_effects>\n";
			stream << "\t<library_images>\n";

			stream << "\t\t<image id=\"ID_Image_1\">\n";
			QString imageFileName = QString( "%1.png" )
									.arg( time, 4, 10, QChar( '0' ) );
			stream << "\t\t\t<init_from>" << imageFileName << "</init_from>\n";
			stream << "\t\t</image>\n";

			stream << "\t</library_images>\n";
			stream << "\t<library_visual_scenes>\n";
			stream << "\t\t<visual_scene id=\"ID_Scene_1\">\n";
			stream << "\t\t\t<node>\n";

			stream << "\t\t\t\t<instance_geometry url=\"#ID_Geometry_1\">\n";
			stream << "\t\t\t\t\t<bind_material>\n";
			stream << "\t\t\t\t\t\t<technique_common>\n";
			stream << "\t\t\t\t\t\t\t<instance_material symbol=\"Material2\" target=\"#ID_Material_1\">\n";
			stream << "\t\t\t\t\t\t\t\t<bind_vertex_input semantic=\"UVSET0\" input_semantic=\"TEXCOORD\" input_set=\"0\" />\n";
			stream << "\t\t\t\t\t\t\t</instance_material>\n";
			stream << "\t\t\t\t\t\t</technique_common>\n";
			stream << "\t\t\t\t\t</bind_material>\n";
			stream << "\t\t\t\t</instance_geometry>\n";

			stream << "\t\t\t</node>\n";
			stream << "\t\t</visual_scene>\n";
			stream << "\t</library_visual_scenes>\n";
			stream << "\t<scene>\n";
			stream << "\t\t<instance_visual_scene url=\"#ID_Scene_1\" />\n";
			stream << "\t</scene>\n";
			stream << "</COLLADA>\n";

			file.close();
		}
	}
}

void KML::exportColorSliceZ()
{
	if ( !param.IsColorSliceZ() ) return;

	QDir dir;
	dir.cd( m_DirPath + "/files/colorslice" );
	dir.mkdir( "z" );
	dir.cd( "z" );

	// Generate Texture Images
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating image files for ColorSliceZ..." ) );
	}

	double zposn = ( double )( param.getColorSlicePosZ() );
	double zpos = m_MinZ + ( m_MaxZ - m_MinZ ) * zposn;
//	double zcoord[ m_SizeZ ];
	double *zcoord = new double[ m_SizeZ ];
	int zidx = 0;
	if ( m_SizeZ == 1 ) {
		zcoord[0] = m_MinZ;
	} else {
		for ( int i = 0; i < m_SizeZ ; i++ ) {
			zcoord[i] = m_MinZ + ( m_MaxZ - m_MinZ ) / ( m_SizeZ - 1 ) * i;
		}
		for ( int i = 0; i < m_SizeZ - 1 ; i++ ) {
			if ( zcoord[ i + 1 ] >= zpos ) {
				zidx = i;
				break;
			}
		}
	}

	QImage image( m_SizeX, m_SizeY, QImage::Format_Indexed8 );
	if ( param.IsColorSliceAlpha() ) {
		image.setColorTable( m_ColorTableRGBA );
	} else {
		image.setColorTable( m_ColorTableRGB );
	}
	ulong size = m_SizeX * m_SizeY * m_SizeZ;
	uchar *data = new uchar[ size ];
	std::fill( data, data + size, 0 );
	uchar *slicedata = new uchar[ m_SizeX * m_SizeY ];
	std::fill( slicedata, slicedata + m_SizeX * m_SizeY, 0 );

	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) {
					// cleanup memories
					delete [] zcoord;
					delete [] slicedata;
					delete [] data;
					return;
				}
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			grads.setData( time );
			interpolator( m_SizeX, m_SizeY, m_SizeZ, data );

			for ( int j = 0; j < m_SizeY ; j++ ) {
				for ( int i = 0; i < m_SizeX ; i++ ) {
					ulong idx0 = m_SizeX * m_SizeY * zidx + m_SizeX * j + i;
					ulong idx1 = m_SizeX * m_SizeY * ( zidx + 1 ) + m_SizeX * j + i;
					float ansf;
					if ( m_SizeZ == 1 ) {
						ansf = static_cast< float >( data[ idx0 ] );
					} else {
						ansf = static_cast< float >( data[ idx0 ] )
							+ static_cast< float >( data[ idx1 ] - data[ idx0 ] ) *
							( zpos - zcoord[ zidx ] ) / ( zcoord[ zidx + 1 ] - zcoord[ zidx ] );
					}
					slicedata[ m_SizeX * j + i ] = static_cast< uchar >( ansf );
				}
			}

			for ( int i = 0; i < m_SizeY; ++i ) {
				memcpy( image.scanLine( i ), slicedata + m_SizeX * i, m_SizeX );
			}

			if ( ( m_MinX < 180.0 ) && ( m_MaxX > 180.0 ) ) {
				QString imageA = QString( m_DirPath + "/files/colorslice/z/%1_A.png" )
									.arg( time, 4, 10, QChar( '0' ) );
				QString imageB = QString( m_DirPath + "/files/colorslice/z/%1_B.png" )
									.arg( time, 4, 10, QChar( '0' ) );
				int imageA_Width = static_cast< int >( static_cast< float >( image.width() )
								* ( 180.0 - m_MinX ) / ( m_MaxX - m_MinX ) );
				int imageB_Width = image.width() - imageA_Width;
				int imageHeight  = image.height();
				image.mirrored( false, true ).copy( 0, 0, imageA_Width, imageHeight ).save( imageA );
				image.mirrored( false, true ).copy( imageA_Width, 0, imageB_Width, imageHeight ).save( imageB );
			} else {
				QString filename = QString( m_DirPath + "/files/colorslice/z/%1.png" )
									.arg( time, 4, 10, QChar( '0' ) );
				image.mirrored( false, true ).save( filename );
			}
		}
	}
	delete [] zcoord;
	delete [] slicedata;
	delete [] data;

	// Generate KML
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating KML for ColorSliceZ..." ) );
	}

	*m_Stream << "\t\t<NetworkLink id=\"colorslicez\">\n";
	*m_Stream << "\t\t\t<name>Z</name>\n";
	*m_Stream << "\t\t\t<open>0</open>\n";
	if ( m_MainObject == COLORSLICE_Z ) {
		*m_Stream << "\t\t\t<visibility>1</visibility>\n";
	} else {
		*m_Stream << "\t\t\t<visibility>0</visibility>\n";
	}
	*m_Stream << "\t\t\t<Link>\n";
	*m_Stream << "\t\t\t\t<href>files/colorslice/z/z.kml</href>\n";
	*m_Stream << "\t\t\t</Link>\n";
	*m_Stream << "\t\t</NetworkLink>\n";

	QString filename = m_DirPath + "/files/colorslice/z/z.kml";
	QFile file( filename );
	file.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &file );
	stream.setRealNumberNotation( QTextStream::ScientificNotation );

	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
	stream << "<Document>\n";

	double realAltitude = zpos;
	double altitude = realAltitude * param.getZScale();
	if ( param.getZUnit() == ZUNIT_KILOMETER ) {
		altitude *= 1000.0;
	}
//	if ( altitude <= 0.0 ) {
//		altitude = 0.00000001;
//	}
	stream << "\t<Folder>\n";
	QString altitudeStr = QString::number( realAltitude, 'f', 2 );
	if ( param.getZUnit() == ZUNIT_KILOMETER ) {
		altitudeStr += "km";
	} else {
		altitudeStr += "m";
	}
	stream << "\t\t<name>" << altitudeStr << "</name>\n";
	stream << "\t\t<Style>\n";
	stream << "\t\t\t<ListStyle>\n";
	stream << "\t\t\t\t<listItemType>checkHideChildren</listItemType>\n";
	stream << "\t\t\t</ListStyle>\n";
	stream << "\t\t</Style>\n";

	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) return;
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			QString TS_Begin = getTimeStringKML( getActualTime( time ) );
			QString TS_End   = getTimeStringKML( getActualTime( time + param.getTimeSkip() ) );

			if ( ( m_MinX < 180.0 ) && ( m_MaxX > 180.0 ) ) {
				stream << "\t\t<Folder>\n";
				stream << "\t\t\t<name>T=" << TS_Begin << "</name>\n";
				stream << "\t\t\t<Style>\n";
				stream << "\t\t\t\t<ListStyle>\n";
				stream << "\t\t\t\t\t<listItemType>checkHideChildren</listItemType>\n";
				stream << "\t\t\t\t</ListStyle>\n";
				stream << "\t\t\t</Style>\n";
				stream << "\t\t\t<GroundOverlay>\n";
				stream << "\t\t\t\t<name>A</name>\n";
				stream << "\t\t\t\t<TimeSpan>\n";
				stream << "\t\t\t\t\t<begin>" << TS_Begin << "</begin>\n";
				stream << "\t\t\t\t\t<end>" << TS_End << "</end>\n";
				stream << "\t\t\t\t</TimeSpan>\n";
				stream << "\t\t\t\t<Icon>\n";
				QString imageFileName_A = QString( "%1_A.png" )
											.arg( time, 4, 10, QChar( '0' ) );
				stream << "\t\t\t\t\t<href>" << imageFileName_A << "</href>\n";
				stream << "\t\t\t\t</Icon>\n";
				stream << "\t\t\t\t<LatLonBox>\n";
				stream << "\t\t\t\t\t<west>"  << QString::number( m_MinX, 'f', 6 ) << "</west>\n";
				stream << "\t\t\t\t\t<east>180.0</east>\n";
				stream << "\t\t\t\t\t<south>" << QString::number( m_MinY, 'f', 6 ) << "</south>\n";
				stream << "\t\t\t\t\t<north>" << QString::number( m_MaxY, 'f', 6 ) << "</north>\n";
				stream << "\t\t\t\t</LatLonBox>\n";
				stream << "\t\t\t\t<altitude>" << altitude << "</altitude>\n";
				if ( m_AltitudeModeOfColorSliceZ == ALTITUDE_ABSOLUTE ) {
					stream << "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
				} else {
					stream << "\t\t\t\t<altitudeMode>clampToGround</altitudeMode>\n";
				}
				stream << "\t\t\t</GroundOverlay>\n";

				stream << "\t\t\t<GroundOverlay>\n";
				stream << "\t\t\t\t<name>B</name>\n";
				stream << "\t\t\t\t<TimeSpan>\n";
				stream << "\t\t\t\t\t<begin>" << TS_Begin << "</begin>\n";
				stream << "\t\t\t\t\t<end>" << TS_End << "</end>\n";
				stream << "\t\t\t\t</TimeSpan>\n";
				stream << "\t\t\t\t<Icon>\n";
				QString imageFileName_B = QString("%1_B.png")
											.arg( time, 4, 10, QChar( '0' ) );
				stream << "\t\t\t\t\t<href>" << imageFileName_B << "</href>\n";
				stream << "\t\t\t\t</Icon>\n";
				stream << "\t\t\t\t<LatLonBox>\n";
				stream << "\t\t\t\t\t<west>-180.0</west>\n";
				stream << "\t\t\t\t\t<east>"  << QString::number( m_MaxX - 360.0, 'f', 6 ) << "</east>\n";
				stream << "\t\t\t\t\t<south>" << QString::number( m_MinY, 'f', 6 ) << "</south>\n";
				stream << "\t\t\t\t\t<north>" << QString::number( m_MaxY, 'f', 6 ) << "</north>\n";
				stream << "\t\t\t\t</LatLonBox>\n";
				stream << "\t\t\t\t<altitude>" << altitude << "</altitude>\n";
				if ( m_AltitudeModeOfColorSliceZ == ALTITUDE_ABSOLUTE ) {
					stream << "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
				} else {
					stream << "\t\t\t\t<altitudeMode>clampToGround</altitudeMode>\n";
				}
				stream << "\t\t\t</GroundOverlay>\n";
				stream << "\t\t</Folder>\n";
			} else {
				stream << "\t\t<GroundOverlay>\n";
				stream << "\t\t\t<name>T=" << TS_Begin << "</name>\n";
				stream << "\t\t\t<TimeSpan>\n";
				stream << "\t\t\t\t<begin>" << TS_Begin << "</begin>\n";
				stream << "\t\t\t\t<end>" << TS_End << "</end>\n";
				stream << "\t\t\t</TimeSpan>\n";
				stream << "\t\t\t<Icon>\n";
				QString imageFileName = QString("%1.png")
											.arg( time, 4, 10, QChar( '0' ) );
				stream << "\t\t\t\t<href>" << imageFileName << "</href>\n";
				stream << "\t\t\t</Icon>\n";
				stream << "\t\t\t<LatLonBox>\n";
				stream << "\t\t\t\t<west>"  << QString::number( m_MinX, 'f', 6 ) << "</west>\n";
				stream << "\t\t\t\t<east>"  << QString::number( m_MaxX, 'f', 6 ) << "</east>\n";
				stream << "\t\t\t\t<south>" << QString::number( m_MinY, 'f', 6 ) << "</south>\n";
				stream << "\t\t\t\t<north>" << QString::number( m_MaxY, 'f', 6 ) << "</north>\n";
				stream << "\t\t\t</LatLonBox>\n";
				stream << "\t\t\t<altitude>" << altitude << "</altitude>\n";
				if ( m_AltitudeModeOfColorSliceZ == ALTITUDE_ABSOLUTE ) {
					stream << "\t\t\t<altitudeMode>absolute</altitudeMode>\n";
				} else {
					stream << "\t\t\t<altitudeMode>clampToGround</altitudeMode>\n";
				}
				stream << "\t\t</GroundOverlay>\n";
			}
		}
	}
	stream << "\t</Folder>\n";
	stream << "</Document>\n";
	stream << "</kml>\n";

	file.close();
}

void KML::exportVolumeX()
{
	if ( !param.IsVolumeX() ) return;

	QDir dir;
	dir.cd( m_DirPath + "/files/volume" );
	dir.mkdir( "x" );
	dir.cd( "x" );

	// Generate Texture Images
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating image files for VolumeX..." ) );
	}

	QImage image( m_SizeY, m_SizeZ, QImage::Format_Indexed8 );
	image.setColorTable( m_ColorTable );
	ulong size = param.getVolumeXNum() * m_SizeY * m_SizeZ;
	uchar *data = new uchar[ size ];
	std::fill( data, data + size, 0 );
	uchar *slicedata = new uchar[ m_SizeY * m_SizeZ ];
	std::fill( slicedata, slicedata + m_SizeY * m_SizeZ, 0 );
	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			dir.mkdir( QString( "%1" ).arg( time, 4, 10, QChar( '0' ) ) );
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) {
					// cleanup memories
					delete [] slicedata;
					delete [] data;
					return;
				}
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			grads.setData( time );
			interpolator( param.getVolumeXNum(), m_SizeY, m_SizeZ, data );
			for ( int n = 0; n < param.getVolumeXNum(); n++ ) {
				for ( int i = 0; i < ( m_SizeY * m_SizeZ ); i++ ) {
					slicedata[ i ] = data[ param.getVolumeXNum() * i + n ];
				}
				for ( int i = 0; i < m_SizeZ; ++i ) {
					memcpy( image.scanLine( i ), slicedata + m_SizeY * i, m_SizeY );
				}
				QString filename = QString( m_DirPath + "/files/volume/x/%1/%2.png" )
									.arg( time, 4, 10, QChar( '0' ) )
									.arg( n,    4, 10, QChar( '0' ) );
				image.mirrored( false, true ).save( filename );
			}
		}
	}
	delete [] slicedata;
	delete [] data;

	// Generate KML
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating KML for VolumeX..." ) );
	}

	*m_Stream << "\t\t<NetworkLink id=\"volumex\">\n";
	*m_Stream << "\t\t\t<name>X</name>\n";
	*m_Stream << "\t\t\t<open>0</open>\n";
	if ( m_MainObject == VOLUME_X ) {
		*m_Stream << "\t\t\t<visibility>1</visibility>\n";
	} else {
		*m_Stream << "\t\t\t<visibility>0</visibility>\n";
	}
	*m_Stream << "\t\t\t<Link>\n";
	*m_Stream << "\t\t\t\t<href>files/volume/x/x.kml</href>\n";
	*m_Stream << "\t\t\t</Link>\n";
	*m_Stream << "\t\t</NetworkLink>\n";

	double centerPosX = m_MinX + ( m_MaxX - m_MinX ) / 2.0;
	double centerPosY = m_MinY + ( m_MaxY - m_MinY ) / 2.0;
	if ( centerPosX >= 180.0 ) {
		centerPosX -= 360.0;
	}

	QString filename = m_DirPath + "/files/volume/x/x.kml";
	QFile file( filename );
	file.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &file );
	stream.setRealNumberNotation( QTextStream::ScientificNotation );

	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
	stream << "<Document>\n";
	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) return;
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			QString TS_Begin = getTimeStringKML( getActualTime( time ) );
			QString TS_End   = getTimeStringKML( getActualTime( time + param.getTimeSkip() ) );
			stream << "\t<Placemark>\n";
			stream << "\t\t<name>T=" << TS_Begin << "</name>\n";
			stream << "\t\t<TimeSpan>\n";
			stream << "\t\t\t<begin>" << TS_Begin << "</begin>\n";
			stream << "\t\t\t<end>" << TS_End << "</end>\n";
			stream << "\t\t</TimeSpan>\n";
			stream << "\t\t<Model id=\"ModelID_VolumeX\">\n";
			stream << "\t\t\t<altitudeMode>absolute</altitudeMode>\n";
			stream << "\t\t\t<Location>\n";
			stream << "\t\t\t\t<longitude>" << QString::number( centerPosX, 'f', 6 ) << "</longitude>\n";
			stream << "\t\t\t\t<latitude>"  << QString::number( centerPosY, 'f', 6 ) << "</latitude>\n";
			stream << "\t\t\t\t<altitude>0.0</altitude>\n";
			stream << "\t\t\t</Location>\n";
			stream << "\t\t\t<Scale>\n";
			stream << "\t\t\t\t<x>1.0</x>\n";
			stream << "\t\t\t\t<y>1.0</y>\n";
			stream << "\t\t\t\t<z>1.0</z>\n";
			stream << "\t\t\t</Scale>\n";
			stream << "\t\t\t<Link>\n";
			QString filename = QString( "%1.dae" )
									.arg( time, 4, 10, QChar( '0' ) );
			stream << "\t\t\t\t<href>" << filename << "</href>\n";
			stream << "\t\t\t</Link>\n";
			stream << "\t\t</Model>\n";
			stream << "\t</Placemark>\n";
		}
	}
	stream << "</Document>\n";
	stream << "</kml>\n";
	file.close();

	// Genarate COLLADA
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating COLLADA files for VolumeX..." ) );
	}

	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ ) {
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) return;
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			QString filename = QString( m_DirPath + "/files/volume/x/%1.dae" )
								.arg( time, 4, 10, QChar( '0' ) );
			QFile file( filename );
			file.open( QIODevice::WriteOnly | QIODevice::Text );
			QTextStream stream( &file );
			stream.setRealNumberNotation( QTextStream::ScientificNotation );

			stream << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n";
			stream << "<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">\n";
			stream << "\t<asset>\n";
			stream << "\t\t<unit meter=\"1.000000\" name=\"meter\" />\n";
			stream << "\t\t<up_axis>Z_UP</up_axis>\n";
			stream << "\t</asset>\n";
			stream << "\t<library_geometries>\n";
			for ( int n = 0; n < param.getVolumeXNum(); n++ ) {
				double height0 = static_cast< double >( m_MinZ * param.getZScale() );
				double height1 = static_cast< double >( m_MaxZ * param.getZScale() );
				if ( param.getZUnit() == ZUNIT_KILOMETER ) {
					height0 *= 1000.0;
					height1 *= 1000.0;
				}
				height0 += EARTH_RADIUS_WE_WGS84;
				height1 += EARTH_RADIUS_WE_WGS84;
				double rangeDegLat = m_MaxY - m_MinY;
				double rangeDegLon = m_MaxX - m_MinX;
				double rangeRadLat = rangeDegLat / 180.0f * M_PI;
				double rangeRadLon = rangeDegLon / 180.0f * M_PI;
				int    divLat = ( int )( rangeDegLat / 1.5f );
				if ( divLat == 0 ) {
					divLat = 1;
				}
				double dRadLat = rangeRadLat / ( double )divLat;
				double dTexLat = 1.0f / ( double )divLat;
				double radLatitude = ( 90.0 - m_MaxY ) * M_PI / 180.0;
				double radLongitude = m_MinX * M_PI / 180.0;
				double lon = ( radLongitude + rangeRadLon / ( param.getVolumeXNum() - 1 ) * n )
						   - ( radLongitude + rangeRadLon / 2.0 ) - M_PI / 2.0;

				stream << "\t\t<geometry id=\"ID_Geometry_" << n << "\">\n";
				stream << "\t\t\t<mesh>\n";
				stream << "\t\t\t\t<source id=\"ID_Geometry_Position_" << n << "\">\n";
				stream << "\t\t\t\t\t<float_array id=\"ID_Geometry_Position_Array_" << n << "\" count=\"" << 12 * divLat << "\">\n";

				for ( int ui = 0; ui < divLat; ui++ )
				{
					double lat0 = radLatitude + dRadLat *   ui;
					double lat1 = radLatitude + dRadLat * ( ui + 1 );
					double vert00[3] = { height0 * sin( lat0 ) * cos( lon ),
										 height0 * sin( lat0 ) * sin( lon ),
										 height0 * cos( lat0 ) };
					double vert10[3] = { height0 * sin( lat1 ) * cos( lon ),
										 height0 * sin( lat1 ) * sin( lon ),
										 height0 * cos( lat1 ) };
					double vert11[3] = { height1 * sin( lat1 ) * cos( lon ),
										 height1 * sin( lat1 ) * sin( lon ),
										 height1 * cos( lat1 ) };
					double vert01[3] = { height1 * sin( lat0 ) * cos( lon ),
										 height1 * sin( lat0 ) * sin( lon ),
										 height1 * cos( lat0 ) };

					double v00[3], v10[3], v11[3], v01[3];
					for ( int i = 0; i < 3; i++ ) {
						v00[i] = vert00[i];
						v10[i] = vert10[i];
						v11[i] = vert11[i];
						v01[i] = vert01[i];
					}
					double lotation = - ( radLatitude + rangeRadLat / 2.0 );
					vert00[0] = v00[0];
					vert00[1] = v00[1] * cos( lotation ) - v00[2] * sin( lotation );
					vert00[2] = v00[1] * sin( lotation ) + v00[2] * cos( lotation );
					vert10[0] = v10[0];
					vert10[1] = v10[1] * cos( lotation ) - v10[2] * sin( lotation );
					vert10[2] = v10[1] * sin( lotation ) + v10[2] * cos( lotation );
					vert11[0] = v11[0];
					vert11[1] = v11[1] * cos( lotation ) - v11[2] * sin( lotation );
					vert11[2] = v11[1] * sin( lotation ) + v11[2] * cos( lotation );
					vert01[0] = v01[0];
					vert01[1] = v01[1] * cos( lotation ) - v01[2] * sin( lotation );
					vert01[2] = v01[1] * sin( lotation ) + v01[2] * cos( lotation );

					vert00[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );
					vert10[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );
					vert11[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );
					vert01[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );


					stream << "\t\t\t\t\t\t"
						   << QString::number( vert00[0], 'f', 6 ) << " "
						   << QString::number( vert00[1], 'f', 6 ) << " "
						   << QString::number( vert00[2], 'f', 6 ) << "\n";
					stream << "\t\t\t\t\t\t"
						   << QString::number( vert10[0], 'f', 6 ) << " "
						   << QString::number( vert10[1], 'f', 6 ) << " "
						   << QString::number( vert10[2], 'f', 6 ) << "\n";
					stream << "\t\t\t\t\t\t"
						   << QString::number( vert11[0], 'f', 6 ) << " "
						   << QString::number( vert11[1], 'f', 6 ) << " "
						   << QString::number( vert11[2], 'f', 6 ) << "\n";
					stream << "\t\t\t\t\t\t"
						   << QString::number( vert01[0], 'f', 6 ) << " "
						   << QString::number( vert01[1], 'f', 6 ) << " "
						   << QString::number( vert01[2], 'f', 6 ) << "\n";
				}
				stream << "\t\t\t\t\t</float_array>\n";
				stream << "\t\t\t\t\t<technique_common>\n";
				stream << "\t\t\t\t\t\t<accessor count=\"" << 4 * divLat << "\" source=\"#ID_Geometry_Position_Array_" << n << "\" stride=\"3\">\n";
				stream << "\t\t\t\t\t\t\t<param name=\"X\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t\t<param name=\"Y\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t\t<param name=\"Z\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t</accessor>\n";
				stream << "\t\t\t\t\t</technique_common>\n";
				stream << "\t\t\t\t</source>\n";

				stream << "\t\t\t\t<source id=\"ID_Geometry_Normal_" << n << "\">\n";
				stream << "\t\t\t\t\t<float_array id=\"ID_Geometry_Normal_Array_" << n << "\" count=\"12\">\n";
				stream << "\t\t\t\t\t\t 1.0 0.0 0.0\n";
				stream << "\t\t\t\t\t\t 1.0 0.0 0.0\n";
				stream << "\t\t\t\t\t\t 1.0 0.0 0.0\n";
				stream << "\t\t\t\t\t\t 1.0 0.0 0.0\n";
				stream << "\t\t\t\t\t</float_array>\n";
				stream << "\t\t\t\t\t<technique_common>\n";
				stream << "\t\t\t\t\t\t<accessor count=\"4\" source=\"#ID_Geometry_Normal_Array_" << n << "\" stride=\"3\">\n";
				stream << "\t\t\t\t\t\t\t<param name=\"X\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t\t<param name=\"Y\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t\t<param name=\"Z\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t</accessor>\n";
				stream << "\t\t\t\t\t</technique_common>\n";
				stream << "\t\t\t\t</source>\n";

				stream << "\t\t\t\t<source id=\"ID_Geometry_Texcoord_" << n << "\">\n";
				stream << "\t\t\t\t\t<float_array id=\"ID_Geometry_Texcoord_Array_"
					   << n << "\" count=\"" << 8 * divLat << "\">\n";
				for ( int ui = 0; ui < divLat; ui++ )
				{
					double texLat0 = 1.0 - ( dTexLat *   ui );
					double texLat1 = 1.0 - ( dTexLat * ( ui + 1 ) );

					if ( texLat0 == 0.00 ) texLat0 = 0.01;
					if ( texLat0 == 1.00 ) texLat0 = 0.99;
					if ( texLat1 == 0.00 ) texLat1 = 0.01;
					if ( texLat1 == 1.00 ) texLat1 = 0.99;

					double TexCoord00[2] = { texLat0, 0.01 };
					double TexCoord10[2] = { texLat1, 0.01 };
					double TexCoord11[2] = { texLat1, 0.99 };
					double TexCoord01[2] = { texLat0, 0.99 };

					stream << "\t\t\t\t\t\t"
						   << QString::number( TexCoord00[0], 'f', 6 ) << " "
						   << QString::number( TexCoord00[1], 'f', 6 ) << "\n";
					stream << "\t\t\t\t\t\t"
						   << QString::number( TexCoord10[0], 'f', 6 ) << " "
						   << QString::number( TexCoord10[1], 'f', 6 ) << "\n";
					stream << "\t\t\t\t\t\t"
						   << QString::number( TexCoord11[0], 'f', 6 ) << " "
						   << QString::number( TexCoord11[1], 'f', 6 ) << "\n";
					stream << "\t\t\t\t\t\t"
						   << QString::number( TexCoord01[0], 'f', 6 ) << " "
						   << QString::number( TexCoord01[1], 'f', 6 ) << "\n";
				}
				stream << "\t\t\t\t\t</float_array>\n";
				stream << "\t\t\t\t\t<technique_common>\n";
				stream << "\t\t\t\t\t\t<accessor count=\"" << 4 * divLat << "\" source=\"#ID_Geometry_Texcoord_Array_" << n << "\" stride=\"2\">\n";
				stream << "\t\t\t\t\t\t\t<param name=\"S\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t\t<param name=\"T\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t</accessor>\n";
				stream << "\t\t\t\t\t</technique_common>\n";
				stream << "\t\t\t\t</source>\n";

				stream << "\t\t\t\t<vertices id=\"ID_Geometry_Vertex_" << n << "\">\n";
				stream << "\t\t\t\t\t<input semantic=\"POSITION\" source=\"#ID_Geometry_Position_" << n << "\" />\n";
				stream << "\t\t\t\t\t<input semantic=\"NORMAL\" source=\"#ID_Geometry_Normal_" << n << "\" />\n";
				stream << "\t\t\t\t</vertices>\n";
				stream << "\t\t\t\t<triangles count=\"" << 4 * divLat << "\" material=\"Material2\">\n";
				stream << "\t\t\t\t\t<input offset=\"0\" semantic=\"VERTEX\" source=\"#ID_Geometry_Vertex_" << n << "\" />\n";
				stream << "\t\t\t\t\t<input offset=\"1\" semantic=\"TEXCOORD\" source=\"#ID_Geometry_Texcoord_" << n << "\" />\n";
				stream << "\t\t\t\t\t<p>\n";
				for ( int ui = 0; ui < divLat; ui++ )
				{
					int increment = 4 * ui;
					stream << "\t\t\t\t\t" << 0 + increment << " " << 0 + increment << " "
										   << 1 + increment << " " << 1 + increment << " "
										   << 2 + increment << " " << 2 + increment << " "
										   << 2 + increment << " " << 2 + increment << " "
										   << 3 + increment << " " << 3 + increment << " "
										   << 0 + increment << " " << 0 + increment << "\n";
					stream << "\t\t\t\t\t" << 2 + increment << " " << 2 + increment << " "
										   << 1 + increment << " " << 1 + increment << " "
										   << 0 + increment << " " << 0 + increment << " "
										   << 0 + increment << " " << 0 + increment << " "
										   << 3 + increment << " " << 3 + increment << " "
										   << 2 + increment << " " << 2 + increment << "\n";

				}
				stream << "\t\t\t\t\t</p>\n";
				stream << "\t\t\t\t</triangles>\n";
				stream << "\t\t\t</mesh>\n";
				stream << "\t\t</geometry>\n";
			}
			stream << "\t</library_geometries>\n";
			stream << "\t<library_materials>\n";
			for ( int n = 0; n < param.getVolumeXNum(); n++ ) {
				stream << "\t\t<material id=\"ID_Material_" << n << "\">\n";
				stream << "\t\t\t<instance_effect url=\"#ID_Effect_" << n << "\" />\n";
				stream << "\t\t</material>\n";
			}
			stream << "\t</library_materials>\n";
			stream << "\t<library_effects>\n";
			for ( int n = 0; n < param.getVolumeXNum(); n++ ) {
				stream << "\t\t<effect id=\"ID_Effect_" << n << "\">\n";
				stream << "\t\t\t<profile_COMMON>\n";
				stream << "\t\t\t\t<newparam sid=\"ID_Effect_Side_" << n << "\">\n";
				stream << "\t\t\t\t\t<surface type=\"2D\">\n";
				stream << "\t\t\t\t\t\t<init_from>ID_Image_" << n << "</init_from>\n";
				stream << "\t\t\t\t\t</surface>\n";
				stream << "\t\t\t\t</newparam>\n";
				stream << "\t\t\t\t<newparam sid=\"ID_Image_Side_" << n << "\">\n";
				stream << "\t\t\t\t\t<sampler2D>\n";
				stream << "\t\t\t\t\t\t<source>ID_Effect_Side_" << n << "</source>\n";
				stream << "\t\t\t\t\t\t<wrap_s>BORDER</wrap_s>\n";
				stream << "\t\t\t\t\t\t<wrap_t>BORDER</wrap_t>\n";
				stream << "\t\t\t\t\t\t<minfilter>LINEAR</minfilter>\n";
				stream << "\t\t\t\t\t\t<magfilter>LINEAR</magfilter>\n";
				stream << "\t\t\t\t\t</sampler2D>\n";
				stream << "\t\t\t\t</newparam>\n";
				stream << "\t\t\t\t<technique sid=\"COMMON\">\n";
				stream << "\t\t\t\t\t<lambert>\n";
				stream << "\t\t\t\t\t\t<emission><color>1 1 1 1</color></emission>\n";
				stream << "\t\t\t\t\t\t<ambient><color>1 1 1 1</color></ambient>\n";
				stream << "\t\t\t\t\t\t<diffuse>\n";
				stream << "\t\t\t\t\t\t\t<texture texture=\"ID_Image_Side_" << n << "\" texcoord=\"UVSET0\" />\n";
				stream << "\t\t\t\t\t\t</diffuse>\n";
				stream << "\t\t\t\t\t</lambert>\n";
				stream << "\t\t\t\t</technique>\n";
				stream << "\t\t\t</profile_COMMON>\n";
				stream << "\t\t</effect>\n";
			}
			stream << "\t</library_effects>\n";
			stream << "\t<library_images>\n";
			for ( int n = 0; n < param.getVolumeXNum(); n++ ) {
				stream << "\t\t<image id=\"ID_Image_" << n << "\">\n";
				QString imageFileName = QString( "%1/%2.png" )
										.arg( time, 4, 10, QChar( '0' ) )
										.arg( n,    4, 10, QChar( '0' ) );
				stream << "\t\t\t<init_from>" << imageFileName << "</init_from>\n";
				stream << "\t\t</image>\n";
			}
			stream << "\t</library_images>\n";
			stream << "\t<library_visual_scenes>\n";
			stream << "\t\t<visual_scene id=\"ID_Scene_1\">\n";
			stream << "\t\t\t<node>\n";
			for ( int n = 0; n < param.getVolumeXNum(); n++ ) {
				stream << "\t\t\t\t<instance_geometry url=\"#ID_Geometry_" << n << "\">\n";
				stream << "\t\t\t\t\t<bind_material>\n";
				stream << "\t\t\t\t\t\t<technique_common>\n";
				stream << "\t\t\t\t\t\t\t<instance_material symbol=\"Material2\" target=\"#ID_Material_" << n << "\">\n";
				stream << "\t\t\t\t\t\t\t\t<bind_vertex_input semantic=\"UVSET0\" input_semantic=\"TEXCOORD\" input_set=\"0\" />\n";
				stream << "\t\t\t\t\t\t\t</instance_material>\n";
				stream << "\t\t\t\t\t\t</technique_common>\n";
				stream << "\t\t\t\t\t</bind_material>\n";
				stream << "\t\t\t\t</instance_geometry>\n";
			}
			stream << "\t\t\t</node>\n";
			stream << "\t\t</visual_scene>\n";
			stream << "\t</library_visual_scenes>\n";
			stream << "\t<scene>\n";
			stream << "\t\t<instance_visual_scene url=\"#ID_Scene_1\" />\n";
			stream << "\t</scene>\n";
			stream << "</COLLADA>\n";

			file.close();
		}
		dir.cdUp();
	}
}

void KML::exportVolumeY()
{
	if ( !param.IsVolumeY() ) return;

	QDir dir;
	dir.cd( m_DirPath + "/files/volume" );
	dir.mkdir( "y" );
	dir.cd( "y" );

	// Generate Texture Images
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating image files for VolumeY..." ) );
	}

	QImage image( m_SizeX, m_SizeZ, QImage::Format_Indexed8 );
	image.setColorTable( m_ColorTable );
	ulong size = m_SizeX * param.getVolumeYNum() * m_SizeZ;
	uchar *data = new uchar[ size ];
	std::fill( data, data + size, 0 );
	uchar *slicedata = new uchar[ m_SizeX * m_SizeZ ];
	std::fill( slicedata, slicedata + m_SizeX * m_SizeZ, 0 );
	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			dir.mkdir( QString( "%1" ).arg( time, 4, 10, QChar( '0' ) ) );
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) {
					// cleanup memories
					delete [] slicedata;
					delete [] data;
					return;
				}
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			grads.setData( time );
			interpolator( m_SizeX, param.getVolumeYNum(), m_SizeZ, data );
			for ( int n = 0; n < param.getVolumeYNum(); n++ ) {
				ulong offset = m_SizeX * n;
				for ( int j = 0; j < m_SizeZ; j++ ) {
					for ( int i = 0; i < m_SizeX; i++ ) {
						slicedata[ m_SizeX * j + i ] =
							data[ m_SizeX * param.getVolumeYNum() * j + i + offset ];
					}
				}
				for ( int i = 0; i < m_SizeZ; ++i ) {
					memcpy( image.scanLine( i ), slicedata + m_SizeX * i, m_SizeX );
				}
				QString filename = QString( m_DirPath + "/files/volume/y/%1/%2.png" )
									.arg( time, 4, 10, QChar( '0' ) )
									.arg( n,    4, 10, QChar( '0' ) );
				image.mirrored( false, true ).save( filename );
			}
		}
	}
	delete [] slicedata;
	delete [] data;

	// Generate KML
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating KML for VolumeY..." ) );
	}

	*m_Stream << "\t\t<NetworkLink id=\"volumey\">\n";
	*m_Stream << "\t\t\t<name>Y</name>\n";
	*m_Stream << "\t\t\t<open>0</open>\n";
	if ( m_MainObject == VOLUME_Y ) {
		*m_Stream << "\t\t\t<visibility>1</visibility>\n";
	} else {
		*m_Stream << "\t\t\t<visibility>0</visibility>\n";
	}
	*m_Stream << "\t\t\t<Link>\n";
	*m_Stream << "\t\t\t\t<href>files/volume/y/y.kml</href>\n";
	*m_Stream << "\t\t\t</Link>\n";
	*m_Stream << "\t\t</NetworkLink>\n";

	double centerPosX = m_MinX + ( m_MaxX - m_MinX ) / 2.0;
	double centerPosY = m_MinY + ( m_MaxY - m_MinY ) / 2.0;
	if ( centerPosX >= 180.0 ) {
		centerPosX -= 360.0;
	}

	QString filename = m_DirPath + "/files/volume/y/y.kml";
	QFile file( filename );
	file.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &file );
	stream.setRealNumberNotation( QTextStream::ScientificNotation );

	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
	stream << "<Document>\n";

	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) return;
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			QString TS_Begin = getTimeStringKML( getActualTime( time ) );
			QString TS_End   = getTimeStringKML( getActualTime( time + param.getTimeSkip() ) );
			stream << "\t<Placemark>\n";
			stream << "\t\t<name>T=" << TS_Begin << "</name>\n";
			stream << "\t\t<TimeSpan>\n";
			stream << "\t\t\t<begin>" << TS_Begin << "</begin>\n";
			stream << "\t\t\t<end>" << TS_End << "</end>\n";
			stream << "\t\t</TimeSpan>\n";
			stream << "\t\t<Model id=\"ModelID_VolumeY\">\n";
			stream << "\t\t\t<altitudeMode>absolute</altitudeMode>\n";
			stream << "\t\t\t<Location>\n";
			stream << "\t\t\t\t<longitude>" << QString::number( centerPosX, 'f', 6 ) << "</longitude>\n";
			stream << "\t\t\t\t<latitude>"  << QString::number( centerPosY, 'f', 6 ) << "</latitude>\n";
			stream << "\t\t\t\t<altitude>0.0</altitude>\n";
			stream << "\t\t\t</Location>\n";
			stream << "\t\t\t<Scale>\n";
			stream << "\t\t\t\t<x>1.0</x>\n";
			stream << "\t\t\t\t<y>1.0</y>\n";
			stream << "\t\t\t\t<z>1.0</z>\n";
			stream << "\t\t\t</Scale>\n";
			stream << "\t\t\t<Link>\n";
			QString filename = QString( "%1.dae" )
								.arg( time, 4, 10, QChar( '0' ) );
			stream << "\t\t\t\t<href>" << filename << "</href>\n";
			stream << "\t\t\t</Link>\n";
			stream << "\t\t</Model>\n";
			stream << "\t</Placemark>\n";
		}
	}
	stream << "</Document>\n";
	stream << "</kml>\n";
	file.close();

	// Generate COLLADA
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating COLLADA files for VolumeY..." ) );
	}

	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ ) {
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) return;
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			QString filename = QString( m_DirPath + "/files/volume/y/%1.dae" )
								.arg( time, 4, 10, QChar( '0' ) );
			QFile file( filename );
			file.open( QIODevice::WriteOnly | QIODevice::Text );
			QTextStream stream( &file );
			stream.setRealNumberNotation( QTextStream::ScientificNotation );

			stream << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n";
			stream << "<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">\n";
			stream << "\t<asset>\n";
			stream << "\t\t<unit meter=\"1.000000\" name=\"meter\" />\n";
			stream << "\t\t<up_axis>Z_UP</up_axis>\n";
			stream << "\t</asset>\n";
			stream << "\t<library_geometries>\n";
			for ( int n = 0; n < param.getVolumeYNum(); n++ ) {
				double height0 = static_cast< double >( m_MinZ * param.getZScale() );
				double height1 = static_cast< double >( m_MaxZ * param.getZScale() );
				if ( param.getZUnit() == ZUNIT_KILOMETER ) {
					height0 *= 1000.0;
					height1 *= 1000.0;
				}
				height0 += EARTH_RADIUS_WE_WGS84;
				height1 += EARTH_RADIUS_WE_WGS84;
				double rangeDegLat = m_MaxY - m_MinY;
				double rangeDegLon = m_MaxX - m_MinX;
				double rangeRadLat = rangeDegLat / 180.0f * M_PI;
				double rangeRadLon = rangeDegLon / 180.0f * M_PI;
				int    divLon = ( int )( rangeDegLon / 1.5f );
				if ( divLon == 0 ) {
					divLon = 1;
				}
				double dRadLon = rangeRadLon / ( double )divLon;
				double dTexLon = 1.0f / ( double )divLon;
				double radLatitude = ( 90.0 - m_MaxY ) * M_PI / 180.0;
				double lat = radLatitude + rangeRadLat / ( param.getVolumeYNum() - 1 ) * n;

				stream << "\t\t<geometry id=\"ID_Geometry_" << n << "\">\n";
				stream << "\t\t\t<mesh>\n";
				stream << "\t\t\t\t<source id=\"ID_Geometry_Position_" << n << "\">\n";
				stream << "\t\t\t\t\t<float_array id=\"ID_Geometry_Position_Array_" << n << "\" count=\"" << 12 * divLon << "\">\n";

				for ( int ui = 0; ui < divLon; ui++ )
				{
					double lon0 = - rangeRadLon / 2.0 + dRadLon *   ui       - M_PI / 2.0;
					double lon1 = - rangeRadLon / 2.0 + dRadLon * ( ui + 1 ) - M_PI / 2.0;
					double vert00[3] = { height0 * sin( lat ) * cos( lon0 ),
										 height0 * sin( lat ) * sin( lon0 ),
										 height0 * cos( lat ) };
					double vert10[3] = { height0 * sin( lat ) * cos( lon1 ),
										 height0 * sin( lat ) * sin( lon1 ),
										 height0 * cos( lat ) };
					double vert11[3] = { height1 * sin( lat ) * cos( lon1 ),
										 height1 * sin( lat ) * sin( lon1 ),
										 height1 * cos( lat ) };
					double vert01[3] = { height1 * sin( lat ) * cos( lon0 ),
										 height1 * sin( lat ) * sin( lon0 ),
										 height1 * cos( lat ) };

					double v00[3], v10[3], v11[3], v01[3];
					for ( int i = 0; i < 3; i++ ) {
						v00[i] = vert00[i];
						v10[i] = vert10[i];
						v11[i] = vert11[i];
						v01[i] = vert01[i];
					}
					double lotation = - ( radLatitude + rangeRadLat / 2.0 );
					vert00[0] = v00[0];
					vert00[1] = v00[1] * cos( lotation ) - v00[2] * sin( lotation );
					vert00[2] = v00[1] * sin( lotation ) + v00[2] * cos( lotation );
					vert10[0] = v10[0];
					vert10[1] = v10[1] * cos( lotation ) - v10[2] * sin( lotation );
					vert10[2] = v10[1] * sin( lotation ) + v10[2] * cos( lotation );
					vert11[0] = v11[0];
					vert11[1] = v11[1] * cos( lotation ) - v11[2] * sin( lotation );
					vert11[2] = v11[1] * sin( lotation ) + v11[2] * cos( lotation );
					vert01[0] = v01[0];
					vert01[1] = v01[1] * cos( lotation ) - v01[2] * sin( lotation );
					vert01[2] = v01[1] * sin( lotation ) + v01[2] * cos( lotation );

					vert00[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );
					vert10[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );
					vert11[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );
					vert01[2] -= static_cast< double >( EARTH_RADIUS_WE_WGS84 );

					stream << "\t\t\t\t\t\t"
						   << QString::number( vert00[0], 'f', 6 ) << " "
						   << QString::number( vert00[1], 'f', 6 ) << " "
						   << QString::number( vert00[2], 'f', 6 ) << "\n";
					stream << "\t\t\t\t\t\t"
						   << QString::number( vert10[0], 'f', 6 ) << " "
						   << QString::number( vert10[1], 'f', 6 ) << " "
						   << QString::number( vert10[2], 'f', 6 ) << "\n";
					stream << "\t\t\t\t\t\t"
						   << QString::number( vert11[0], 'f', 6 ) << " "
						   << QString::number( vert11[1], 'f', 6 ) << " "
						   << QString::number( vert11[2], 'f', 6 ) << "\n";
					stream << "\t\t\t\t\t\t"
						   << QString::number( vert01[0], 'f', 6 ) << " "
						   << QString::number( vert01[1], 'f', 6 ) << " "
						   << QString::number( vert01[2], 'f', 6 ) << "\n";
				}
				stream << "\t\t\t\t\t</float_array>\n";
				stream << "\t\t\t\t\t<technique_common>\n";
				stream << "\t\t\t\t\t\t<accessor count=\"" << 4 * divLon << "\" source=\"#ID_Geometry_Position_Array_" << n << "\" stride=\"3\">\n";
				stream << "\t\t\t\t\t\t\t<param name=\"X\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t\t<param name=\"Y\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t\t<param name=\"Z\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t</accessor>\n";
				stream << "\t\t\t\t\t</technique_common>\n";
				stream << "\t\t\t\t</source>\n";

				stream << "\t\t\t\t<source id=\"ID_Geometry_Normal_" << n << "\">\n";
				stream << "\t\t\t\t\t<float_array id=\"ID_Geometry_Normal_Array_" << n << "\" count=\"12\">\n";
				stream << "\t\t\t\t\t\t 0.0 -1.0 0.0\n";
				stream << "\t\t\t\t\t\t 0.0 -1.0 0.0\n";
				stream << "\t\t\t\t\t\t 0.0 -1.0 0.0\n";
				stream << "\t\t\t\t\t\t 0.0 -1.0 0.0\n";
				stream << "\t\t\t\t\t</float_array>\n";
				stream << "\t\t\t\t\t<technique_common>\n";
				stream << "\t\t\t\t\t\t<accessor count=\"4\" source=\"#ID_Geometry_Normal_Array_" << n << "\" stride=\"3\">\n";
				stream << "\t\t\t\t\t\t\t<param name=\"X\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t\t<param name=\"Y\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t\t<param name=\"Z\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t</accessor>\n";
				stream << "\t\t\t\t\t</technique_common>\n";
				stream << "\t\t\t\t</source>\n";

				stream << "\t\t\t\t<source id=\"ID_Geometry_Texcoord_" << n << "\">\n";
				stream << "\t\t\t\t\t<float_array id=\"ID_Geometry_Texcoord_Array_"
					   << n << "\" count=\"" << 8 * divLon << "\">\n";
				for ( int ui = 0; ui < divLon; ui++ )
				{
					double texLon0 = dTexLon *   ui;
					double texLon1 = dTexLon * ( ui + 1 );

					if ( texLon0 == 0.00 ) texLon0 = 0.01;
					if ( texLon0 == 1.00 ) texLon0 = 0.99;
					if ( texLon1 == 0.00 ) texLon1 = 0.01;
					if ( texLon1 == 1.00 ) texLon1 = 0.99;

					double TexCoord00[2] = { texLon0, 0.01 };
					double TexCoord10[2] = { texLon1, 0.01 };
					double TexCoord11[2] = { texLon1, 0.99 };
					double TexCoord01[2] = { texLon0, 0.99 };

					stream << "\t\t\t\t\t\t"
						   << QString::number( TexCoord00[0], 'f', 6 ) << " "
						   << QString::number( TexCoord00[1], 'f', 6 ) << "\n";
					stream << "\t\t\t\t\t\t"
						   << QString::number( TexCoord10[0], 'f', 6 ) << " "
						   << QString::number( TexCoord10[1], 'f', 6 ) << "\n";
					stream << "\t\t\t\t\t\t"
						   << QString::number( TexCoord11[0], 'f', 6 ) << " "
						   << QString::number( TexCoord11[1], 'f', 6 ) << "\n";
					stream << "\t\t\t\t\t\t"
						   << QString::number( TexCoord01[0], 'f', 6 ) << " "
						   << QString::number( TexCoord01[1], 'f', 6 ) << "\n";
				}
				stream << "\t\t\t\t\t</float_array>\n";
				stream << "\t\t\t\t\t<technique_common>\n";
				stream << "\t\t\t\t\t\t<accessor count=\"" << 4 * divLon << "\" source=\"#ID_Geometry_Texcoord_Array_" << n << "\" stride=\"2\">\n";
				stream << "\t\t\t\t\t\t\t<param name=\"S\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t\t<param name=\"T\" type=\"float\" />\n";
				stream << "\t\t\t\t\t\t</accessor>\n";
				stream << "\t\t\t\t\t</technique_common>\n";
				stream << "\t\t\t\t</source>\n";

				stream << "\t\t\t\t<vertices id=\"ID_Geometry_Vertex_" << n << "\">\n";
				stream << "\t\t\t\t\t<input semantic=\"POSITION\" source=\"#ID_Geometry_Position_" << n << "\" />\n";
				stream << "\t\t\t\t\t<input semantic=\"NORMAL\" source=\"#ID_Geometry_Normal_" << n << "\" />\n";
				stream << "\t\t\t\t</vertices>\n";
				stream << "\t\t\t\t<triangles count=\"" << 4 * divLon << "\" material=\"Material2\">\n";
				stream << "\t\t\t\t\t<input offset=\"0\" semantic=\"VERTEX\" source=\"#ID_Geometry_Vertex_" << n << "\" />\n";
				stream << "\t\t\t\t\t<input offset=\"1\" semantic=\"TEXCOORD\" source=\"#ID_Geometry_Texcoord_" << n << "\" />\n";
				stream << "\t\t\t\t\t<p>\n";
				for ( int ui = 0; ui < divLon; ui++ )
				{
					int increment = 4 * ui;
					stream << "\t\t\t\t\t" << 0 + increment << " " << 0 + increment << " "
										   << 1 + increment << " " << 1 + increment << " "
										   << 2 + increment << " " << 2 + increment << " "
										   << 2 + increment << " " << 2 + increment << " "
										   << 3 + increment << " " << 3 + increment << " "
										   << 0 + increment << " " << 0 + increment << "\n";
					stream << "\t\t\t\t\t" << 2 + increment << " " << 2 + increment << " "
										   << 1 + increment << " " << 1 + increment << " "
										   << 0 + increment << " " << 0 + increment << " "
										   << 0 + increment << " " << 0 + increment << " "
										   << 3 + increment << " " << 3 + increment << " "
										   << 2 + increment << " " << 2 + increment << "\n";
				}
				stream << "\t\t\t\t\t</p>\n";
				stream << "\t\t\t\t</triangles>\n";
				stream << "\t\t\t</mesh>\n";
				stream << "\t\t</geometry>\n";
			}
			stream << "\t</library_geometries>\n";
			stream << "\t<library_materials>\n";
			for ( int n = 0; n < param.getVolumeYNum(); n++ ) {
				stream << "\t\t<material id=\"ID_Material_" << n << "\">\n";
				stream << "\t\t\t<instance_effect url=\"#ID_Effect_" << n << "\" />\n";
				stream << "\t\t</material>\n";
			}
			stream << "\t</library_materials>\n";
			stream << "\t<library_effects>\n";
			for ( int n = 0; n < param.getVolumeYNum(); n++ ) {
				stream << "\t\t<effect id=\"ID_Effect_" << n << "\">\n";
				stream << "\t\t\t<profile_COMMON>\n";
				stream << "\t\t\t\t<newparam sid=\"ID_Effect_Side_" << n << "\">\n";
				stream << "\t\t\t\t\t<surface type=\"2D\">\n";
				stream << "\t\t\t\t\t\t<init_from>ID_Image_" << n << "</init_from>\n";
				stream << "\t\t\t\t\t</surface>\n";
				stream << "\t\t\t\t</newparam>\n";
				stream << "\t\t\t\t<newparam sid=\"ID_Image_Side_" << n << "\">\n";
				stream << "\t\t\t\t\t<sampler2D>\n";
				stream << "\t\t\t\t\t\t<source>ID_Effect_Side_" << n << "</source>\n";
				stream << "\t\t\t\t\t\t<wrap_s>BORDER</wrap_s>\n";
				stream << "\t\t\t\t\t\t<wrap_t>BORDER</wrap_t>\n";
				stream << "\t\t\t\t\t\t<minfilter>LINEAR</minfilter>\n";
				stream << "\t\t\t\t\t\t<magfilter>LINEAR</magfilter>\n";
				stream << "\t\t\t\t\t</sampler2D>\n";
				stream << "\t\t\t\t</newparam>\n";
				stream << "\t\t\t\t<technique sid=\"COMMON\">\n";
				stream << "\t\t\t\t\t<lambert>\n";
				stream << "\t\t\t\t\t\t<emission><color>1 1 1 1</color></emission>\n";
				stream << "\t\t\t\t\t\t<ambient><color>1 1 1 1</color></ambient>\n";
				stream << "\t\t\t\t\t\t<diffuse>\n";
				stream << "\t\t\t\t\t\t\t<texture texture=\"ID_Image_Side_" << n << "\" texcoord=\"UVSET0\" />\n";
				stream << "\t\t\t\t\t\t</diffuse>\n";
				stream << "\t\t\t\t\t</lambert>\n";
				stream << "\t\t\t\t</technique>\n";
				stream << "\t\t\t</profile_COMMON>\n";
				stream << "\t\t</effect>\n";
			}
			stream << "\t</library_effects>\n";
			stream << "\t<library_images>\n";
			for ( int n = 0; n < param.getVolumeYNum(); n++ ) {
				stream << "\t\t<image id=\"ID_Image_" << n << "\">\n";
				QString imageFileName = QString( "%1/%2.png" )
										.arg( time, 4, 10, QChar( '0' ) )
										.arg( ( param.getVolumeYNum() - 1 ) - n,
													4, 10, QChar( '0' ) );
				stream << "\t\t\t<init_from>" << imageFileName << "</init_from>\n";
				stream << "\t\t</image>\n";
			}
			stream << "\t</library_images>\n";
			stream << "\t<library_visual_scenes>\n";
			stream << "\t\t<visual_scene id=\"ID_Scene_1\">\n";
			stream << "\t\t\t<node>\n";
			for ( int n = 0; n < param.getVolumeYNum(); n++ ) {
				stream << "\t\t\t\t<instance_geometry url=\"#ID_Geometry_" << n << "\">\n";
				stream << "\t\t\t\t\t<bind_material>\n";
				stream << "\t\t\t\t\t\t<technique_common>\n";
				stream << "\t\t\t\t\t\t\t<instance_material symbol=\"Material2\" target=\"#ID_Material_" << n << "\">\n";
				stream << "\t\t\t\t\t\t\t\t<bind_vertex_input semantic=\"UVSET0\" input_semantic=\"TEXCOORD\" input_set=\"0\" />\n";
				stream << "\t\t\t\t\t\t\t</instance_material>\n";
				stream << "\t\t\t\t\t\t</technique_common>\n";
				stream << "\t\t\t\t\t</bind_material>\n";
				stream << "\t\t\t\t</instance_geometry>\n";
			}
			stream << "\t\t\t</node>\n";
			stream << "\t\t</visual_scene>\n";
			stream << "\t</library_visual_scenes>\n";
			stream << "\t<scene>\n";
			stream << "\t\t<instance_visual_scene url=\"#ID_Scene_1\" />\n";
			stream << "\t</scene>\n";
			stream << "</COLLADA>\n";

			file.close();
		}
		dir.cdUp();
	}
}

void KML::exportVolumeZ()
{
	if ( !param.IsVolumeZ() ) return;

	QDir dir;
	dir.cd( m_DirPath + "/files/volume" );
	dir.mkdir( "z" );
	dir.cd( "z" );

	// Generate Texture Images
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating image files for VolumeZ..." ) );
	}

	QImage image( m_SizeX, m_SizeY, QImage::Format_Indexed8 );
	image.setColorTable( m_ColorTable );
	ulong size = m_SizeX * m_SizeY * param.getVolumeZNum();
	uchar *data = new uchar[ size ];
	std::fill( data, data + size, 0 );
	uchar *slicedata = new uchar[ m_SizeX * m_SizeY ];
	std::fill( slicedata, slicedata + m_SizeX * m_SizeY, 0 );
	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			dir.mkdir( QString( "%1" ).arg( time, 4, 10, QChar( '0' ) ) );
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) {
					// cleanup memories
					delete [] slicedata;
					delete [] data;
					return;
				}
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			grads.setData( time );
			interpolator( m_SizeX, m_SizeY, param.getVolumeZNum(), data );
			for ( int n = 0; n < param.getVolumeZNum(); n++ ) {
				memcpy( slicedata,
						data + m_SizeX * m_SizeY * n,
						m_SizeX * m_SizeY );
				for ( int i = 0; i < m_SizeY; ++i ) {
					memcpy( image.scanLine( i ), slicedata + m_SizeX * i, m_SizeX );
				}
				if ( ( m_MinX < 180.0 ) && ( m_MaxX > 180.0 ) ) {
					QString imageA = QString( m_DirPath + "/files/volume/z/%1/%2_A.png" )
										.arg( time, 4, 10, QChar( '0' ) )
										.arg( n,    4, 10, QChar( '0' ) );
					QString imageB = QString( m_DirPath + "/files/volume/z/%1/%2_B.png" )
										.arg( time, 4, 10, QChar( '0' ) )
										.arg( n,    4, 10, QChar( '0' ) );
					int imageA_Width = static_cast< int >( static_cast< float >( image.width() )
									* ( 180.0 - m_MinX ) / ( m_MaxX - m_MinX ) );
					int imageB_Width = image.width() - imageA_Width;
					int imageHeight  = image.height();
					image.mirrored( false, true ).copy( 0, 0, imageA_Width, imageHeight ).save( imageA );
					image.mirrored( false, true ).copy( imageA_Width, 0, imageB_Width, imageHeight ).save( imageB );
				} else {
					QString filename = QString( m_DirPath + "/files/volume/z/%1/%2.png" )
										.arg( time, 4, 10, QChar( '0' ) )
										.arg( n,    4, 10, QChar( '0' ) );
					image.mirrored( false, true ).save( filename );
				}
			}
		}
	}
	delete [] slicedata;
	delete [] data;

	// Generate KML
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating KML for VolumeZ..." ) );
	}

	*m_Stream << "\t\t<NetworkLink id=\"volumez\">\n";
	*m_Stream << "\t\t\t<name>Z</name>\n";
	*m_Stream << "\t\t\t<open>0</open>\n";
	*m_Stream << "\t\t\t<visibility>1</visibility>\n";
	*m_Stream << "\t\t\t<Link>\n";
	*m_Stream << "\t\t\t\t<href>files/volume/z/z.kml</href>\n";
	*m_Stream << "\t\t\t</Link>\n";
	*m_Stream << "\t\t</NetworkLink>\n";

	QString filename = m_DirPath + "/files/volume/z/z.kml";
	QFile file( filename );
	file.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &file );
	stream.setRealNumberNotation( QTextStream::ScientificNotation );

	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
	stream << "<Document>\n";

	double dz = 0.0;
	if ( param.getVolumeZNum() != 1 ) {
		dz = ( m_MaxZ - m_MinZ ) /
			   static_cast< double >( param.getVolumeZNum() - 1 );
	}

	QString *tmpStr = new QString[ param.getVolumeZNum() ];
	for ( int n = 0; n < param.getVolumeZNum(); n++ ) {
		double realAltitude = m_MinZ + dz * n;
		tmpStr[n]  = "\t<Folder>\n";
		tmpStr[n] += "\t\t<name>";
		tmpStr[n] += QString::number( realAltitude, 'f', 2 );
		if ( param.getZUnit() == ZUNIT_KILOMETER ) {
			tmpStr[n] += "km";
		} else {
			tmpStr[n] += "m";
		}
		tmpStr[n] += "</name>\n";
		tmpStr[n] += "\t\t<Style>\n";
		tmpStr[n] += "\t\t\t<ListStyle>\n";
		tmpStr[n] += "\t\t\t\t<listItemType>checkHideChildren</listItemType>\n";
		tmpStr[n] += "\t\t\t</ListStyle>\n";
		tmpStr[n] += "\t\t</Style>\n";
	}

	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) return;
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			QString TS_Begin = getTimeStringKML( getActualTime( time ) );
			QString TS_End   = getTimeStringKML( getActualTime( time + param.getTimeSkip() ) );

			for ( int n = 0; n < param.getVolumeZNum(); n++ ) {
				double realAltitude = m_MinZ + dz * n;
				double altitude = realAltitude * param.getZScale();
				if ( param.getZUnit() == ZUNIT_KILOMETER ) {
					altitude = ( m_MinZ + dz * n ) * param.getZScale() * 1000.0;
				}
//				if ( altitude <= 0.0 ) {
//					altitude = 0.00000001;
//				}

				if ( ( m_MinX < 180.0 ) && ( m_MaxX > 180.0 ) ) {
					tmpStr[n] += "\t\t<Folder>\n";
					tmpStr[n] += "\t\t\t<name>T=" + TS_Begin + "</name>\n";
					tmpStr[n] += "\t\t\t<Style>\n";
					tmpStr[n] += "\t\t\t\t<ListStyle>\n";
					tmpStr[n] += "\t\t\t\t\t<listItemType>checkHideChildren</listItemType>\n";
					tmpStr[n] += "\t\t\t\t</ListStyle>\n";
					tmpStr[n] += "\t\t\t</Style>\n";
					tmpStr[n] += "\t\t\t<GroundOverlay>\n";
					tmpStr[n] += "\t\t\t\t<name>A</name>\n";
					tmpStr[n] += "\t\t\t\t<TimeSpan>\n";
					tmpStr[n] += "\t\t\t\t\t<begin>" + TS_Begin + "</begin>\n";
					tmpStr[n] += "\t\t\t\t\t<end>" + TS_End + "</end>\n";
					tmpStr[n] += "\t\t\t\t</TimeSpan>\n";
					tmpStr[n] += "\t\t\t\t<Icon>\n";
					QString imageFileName_A = QString( "%1/%2_A.png" )
											.arg( time, 4, 10, QChar( '0' ) )
											.arg( n,    4, 10, QChar( '0' ) );
					tmpStr[n] += "\t\t\t\t\t<href>" + imageFileName_A + "</href>\n";
					tmpStr[n] += "\t\t\t\t</Icon>\n";
					tmpStr[n] += "\t\t\t\t<LatLonBox>\n";
					tmpStr[n] += "\t\t\t\t\t<west>"  + QString::number( m_MinX, 'f', 6 ) + "</west>\n";
					tmpStr[n] += "\t\t\t\t\t<east>180.0</east>\n";
					tmpStr[n] += "\t\t\t\t\t<south>" + QString::number( m_MinY, 'f', 6 ) + "</south>\n";
					tmpStr[n] += "\t\t\t\t\t<north>" + QString::number( m_MaxY, 'f', 6 ) + "</north>\n";
					tmpStr[n] += "\t\t\t\t</LatLonBox>\n";
					tmpStr[n] += "\t\t\t\t<altitude>";
					tmpStr[n] += QString::number( altitude, 'f', 2 );
					tmpStr[n] += "</altitude>\n";
					tmpStr[n] += "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
/*
					if ( m_AltitudeModeOfVolumeZ == ALTITUDE_ABSOLUTE ) {
						tmpStr[n] += "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
					} else {
						tmpStr[n] += "\t\t\t\t<altitudeMode>clampToGround</altitudeMode>\n";
					}
*/
					tmpStr[n] += "\t\t\t</GroundOverlay>\n";

					tmpStr[n] += "\t\t\t<GroundOverlay>\n";
					tmpStr[n] += "\t\t\t\t<name>B</name>\n";
					tmpStr[n] += "\t\t\t\t<TimeSpan>\n";
					tmpStr[n] += "\t\t\t\t\t<begin>" + TS_Begin + "</begin>\n";
					tmpStr[n] += "\t\t\t\t\t<end>" + TS_End + "</end>\n";
					tmpStr[n] += "\t\t\t\t</TimeSpan>\n";
					tmpStr[n] += "\t\t\t\t<Icon>\n";
					QString imageFileName_B = QString("%1/%2_B.png")
											.arg( time, 4, 10, QChar( '0' ) )
											.arg( n,    4, 10, QChar( '0' ) );
					tmpStr[n] += "\t\t\t\t\t<href>" + imageFileName_B + "</href>\n";
					tmpStr[n] += "\t\t\t\t</Icon>\n";
					tmpStr[n] += "\t\t\t\t<LatLonBox>\n";
					tmpStr[n] += "\t\t\t\t\t<west>-180.0</west>\n";
					tmpStr[n] += "\t\t\t\t\t<east>"  + QString::number( m_MaxX - 360.0, 'f', 6 ) + "</east>\n";
					tmpStr[n] += "\t\t\t\t\t<south>" + QString::number( m_MinY, 'f', 6 ) + "</south>\n";
					tmpStr[n] += "\t\t\t\t\t<north>" + QString::number( m_MaxY, 'f', 6 ) + "</north>\n";
					tmpStr[n] += "\t\t\t\t</LatLonBox>\n";
					tmpStr[n] += "\t\t\t\t<altitude>";
					tmpStr[n] += QString::number( altitude, 'f', 2 );
					tmpStr[n] += "</altitude>\n";
					tmpStr[n] += "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
/*
					if ( m_AltitudeModeOfVolumeZ == ALTITUDE_ABSOLUTE ) {
						tmpStr[n] += "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
					} else {
						tmpStr[n] += "\t\t\t\t<altitudeMode>clampToGround</altitudeMode>\n";
					}
*/
					tmpStr[n] += "\t\t\t</GroundOverlay>\n";
					tmpStr[n] += "\t\t</Folder>\n";
				} else {
					tmpStr[n] += "\t\t<GroundOverlay>\n";
					tmpStr[n] += "\t\t\t<name>T=" + TS_Begin + "</name>\n";
					tmpStr[n] += "\t\t\t<TimeSpan>\n";
					tmpStr[n] += "\t\t\t\t<begin>" + TS_Begin + "</begin>\n";
					tmpStr[n] += "\t\t\t\t<end>" + TS_End + "</end>\n";
					tmpStr[n] += "\t\t\t</TimeSpan>\n";
					tmpStr[n] += "\t\t\t<Icon>\n";
					QString imageFileName = QString("%1/%2.png")
											.arg( time, 4, 10, QChar( '0' ) )
											.arg( n,    4, 10, QChar( '0' ) );
					tmpStr[n] += "\t\t\t\t<href>" + imageFileName + "</href>\n";
					tmpStr[n] += "\t\t\t</Icon>\n";
					tmpStr[n] += "\t\t\t<LatLonBox>\n";
					tmpStr[n] += "\t\t\t\t<west>"  + QString::number( m_MinX, 'f', 6 ) + "</west>\n";
					tmpStr[n] += "\t\t\t\t<east>"  + QString::number( m_MaxX, 'f', 6 ) + "</east>\n";
					tmpStr[n] += "\t\t\t\t<south>" + QString::number( m_MinY, 'f', 6 ) + "</south>\n";
					tmpStr[n] += "\t\t\t\t<north>" + QString::number( m_MaxY, 'f', 6 ) + "</north>\n";
					tmpStr[n] += "\t\t\t</LatLonBox>\n";
					tmpStr[n] += "\t\t\t<altitude>";
					tmpStr[n] += QString::number( altitude, 'f', 2 );
					tmpStr[n] += "</altitude>\n";
					tmpStr[n] += "\t\t\t<altitudeMode>absolute</altitudeMode>\n";
/*
					if ( m_AltitudeModeOfVolumeZ == ALTITUDE_ABSOLUTE ) {
						tmpStr[n] += "\t\t\t<altitudeMode>absolute</altitudeMode>\n";
					} else {
						tmpStr[n] += "\t\t\t<altitudeMode>clampToGround</altitudeMode>\n";
					}
*/
					tmpStr[n] += "\t\t</GroundOverlay>\n";
				}
			}
		}
		dir.cdUp();
	}

	for ( int n = 0; n < param.getVolumeZNum(); n++ ) {
		tmpStr[n] += "\t</Folder>\n";
		stream << tmpStr[n];
	}

	stream << "</Document>\n";
	stream << "</kml>\n";

	delete[] tmpStr;
}

void KML::exportContourLine()
{
	if ( param.getContourLineInterval() == 0.0 ) {
		return;
	}

	if ( !param.IsContourLineX() &&
		 !param.IsContourLineY() &&
		 !param.IsContourLineZ() ) return;

	QDir kmlDir;
	kmlDir.cd( m_DirPath + "/files" );
	kmlDir.mkdir( "contourline" );

	*m_Stream << "\t<Folder>\n";
	*m_Stream << "\t\t<name>ContourLine</name>\n";
	*m_Stream << "\t\t<open>0</open>\n";

	exportContourLineX();
	exportContourLineY();
	exportContourLineZ();

	*m_Stream << "\t</Folder>\n";
}

void KML::exportContourLineX()
{
	if ( !param.IsContourLineX() ) {
		return;
	}

	QDir dir;
	dir.cd( m_DirPath + "/files/contourline" );
	dir.mkdir( "x" );
	dir.cd( "x" );

	// Generate KML
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating KML for ContourLineX..." ) );
	}

	*m_Stream << "\t\t<NetworkLink id=\"contourlinex\">\n";
	*m_Stream << "\t\t\t<name>X</name>\n";
	*m_Stream << "\t\t\t<open>0</open>\n";
	if ( m_MainObject == CONTOURLINE_X ) {
		*m_Stream << "\t\t\t<visibility>1</visibility>\n";
	} else {
		*m_Stream << "\t\t\t<visibility>0</visibility>\n";
	}
	*m_Stream << "\t\t\t<Link>\n";
	*m_Stream << "\t\t\t\t<href>files/contourline/x/x.kml</href>\n";
	*m_Stream << "\t\t\t</Link>\n";
	*m_Stream << "\t\t</NetworkLink>\n";

	QString filename = m_DirPath + "/files/contourline/x/x.kml";
	QFile file( filename );
	file.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &file );
	stream.setRealNumberNotation( QTextStream::ScientificNotation );

	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
	stream << "<Document>\n";
	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) return;
				QApplication::processEvents();
			}

			QString TS_Begin = getTimeStringKML( getActualTime( time ) );
			QString TS_End   = getTimeStringKML( getActualTime( time + param.getTimeSkip() ) );
			stream << "\t<NetworkLink>\n";
			stream << "\t\t<name>T=" << TS_Begin << "</name>\n";
			stream << "\t\t<TimeSpan>\n";
			stream << "\t\t\t<begin>" << TS_Begin << "</begin>\n";
			stream << "\t\t\t<end>" << TS_End << "</end>\n";
			stream << "\t\t</TimeSpan>\n";
			stream << "\t\t<Link>\n";
			QString filename = QString( "%1.kml" )
									.arg( time, 4, 10, QChar( '0' ) );
			stream << "\t\t\t<href>" << filename << "</href>\n";
			stream << "\t\t</Link>\n";
			stream << "\t</NetworkLink>\n";
		}
	}
	stream << "</Document>\n";
	stream << "</kml>\n";
	file.close();

	double xpos = grads.getGridXMin() + ( grads.getGridXMax() - grads.getGridXMin() ) *
		static_cast< double >( param.getContourLinePosX() );
	int xidx = 0;
	if ( grads.getGridSizeX() != 1 ) {
		for ( int i = 0; i < grads.getGridSizeX() - 1 ; i++ ) {
			if ( grads.getCoordX( i + 1 ) >= xpos ) {
				xidx = i;
				break;
			}
		}
	}

	// Set Threshold Value and Line Color of Contourline
	QVector<float> lineLevel;
	QVector<QColor> lineColor;
	lineLevel.clear();
	lineColor.clear();
	float interval = param.getContourLineInterval();
	float thresMin = param.getContourLineThresholdMin();
	float thresMax = param.getContourLineThresholdMax();

	if ( thresMin < param.getVolumeThresholdMin() ) {
		thresMin = param.getVolumeThresholdMin();
	}
	if ( thresMax > param.getVolumeThresholdMax() ) {
		thresMax = param.getVolumeThresholdMax();
	}
	int i = 0;
	while (1) {
		float level = param.getContourLineThresholdMin() + interval * i;
		if ( level < thresMin ) {
			i++;
		} else if ( level <= thresMax ) {
			lineLevel.push_back( level );
			int colorID = static_cast< int >( ( level - param.getVolumeThresholdMin() ) /
				(  param.getVolumeThresholdMax() - param.getVolumeThresholdMin() ) * 254.0 );
			int r = static_cast< int >( param.getColorTableR( colorID ) * 255.0 );
			int g = static_cast< int >( param.getColorTableG( colorID ) * 255.0 );
			int b = static_cast< int >( param.getColorTableB( colorID ) * 255.0 );
			lineColor.push_back( QColor( r, g, b, 255 ) );
			i++;
		} else {
			break;
		}
	}

	float *slicedata = new float[ grads.getGridSizeY() * grads.getGridSizeZ() ];

	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ ) {
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) {
					// cleanup memories
					delete [] slicedata;
					return;
				}
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			QString filename = m_DirPath + "/files/contourline/x/" + QString( "%1" ).arg( time, 4, 10, QChar( '0' ) ) + ".kml";
			QFile file( filename );
			file.open( QIODevice::WriteOnly | QIODevice::Text );
			QTextStream stream( &file );
			stream.setRealNumberNotation( QTextStream::ScientificNotation );

			stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
			stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
			stream << "<Document>\n";
			stream << "\t<Folder>\n";
			QString longitudeStr = QString::number( xpos, 'f', 6 );
			stream << "\t\t<name>" << longitudeStr << "</name>\n";
			stream << "\t\t<Style>\n";
			stream << "\t\t\t<ListStyle>\n";
			stream << "\t\t\t\t<listItemType>checkHideChildren</listItemType>\n";
			stream << "\t\t\t</ListStyle>\n";
			stream << "\t\t</Style>\n";

			for ( int l = 0; l < lineLevel.size(); l++ ) {
				QString styleID = QString( "linecolor%1" ).arg( l, 4, 10, QChar( '0' ) );
				stream << "\t\t<Style id=\"" << styleID << "\">\n";
				stream << "\t\t\t<LineStyle>\n";
				int r = lineColor.at(l).red();
				int g = lineColor.at(l).green();
				int b = lineColor.at(l).blue();
				QString colorString = QString( "ff%1%2%3" )
					.arg( b, 2, 16, QChar( '0' ) )
					.arg( g, 2, 16, QChar( '0' ) )
					.arg( r, 2, 16, QChar( '0' ) );
				stream << "\t\t\t\t<color>" << colorString.toLower() << "</color>\n";
				stream << "\t\t\t\t<width>" << param.getContourLineWidth() << "</width>\n";
				stream << "\t\t\t</LineStyle>\n";
				stream << "\t\t</Style>\n";
			}

			grads.setData( time );
			float *data = grads.getData();
			std::fill( slicedata, slicedata + grads.getGridSizeY() * grads.getGridSizeZ(), 0 );

			// Generate Vertex to Draw Contourline
			for ( int i = 0; i < ( grads.getGridSizeY() * grads.getGridSizeZ() ); i++ ) {
				ulong idx0 = grads.getGridSizeX() * i + xidx;
				ulong idx1 = idx0 + 1;
				float ansf;
				if ( grads.getGridSizeX() == 1 ) {
					ansf = data[ idx0 ];
				} else {
					if ( ( data[ idx0 ] == grads.getUndef() ) || ( data[ idx1 ] == grads.getUndef() ) ) {
						ansf = grads.getUndef();
					} else {
						ansf = data[ idx0 ] + ( data[ idx1 ] - data[ idx0 ] ) *
							( xpos - grads.getCoordX( xidx ) ) / ( grads.getCoordX( xidx + 1 ) - grads.getCoordX( xidx ) );
					}
					slicedata[ i ] = ansf;
				}
			}

			QVector< QVector<QVector3D> > lineData;
			for ( int level = 0; level < lineLevel.size(); level++ ) {
				float value = lineLevel.at( level );
				QVector<QVector3D> vertex;
				vertex.clear();
				for ( int j = 0; j < ( grads.getGridSizeZ() - 1 ); j++ ) {
					for ( int i = 0; i < ( grads.getGridSizeY() - 1 ); i++ ) {
						float y0 = grads.getCoordY( i );
						float y1 = grads.getCoordY( i + 1 );
						float z0 = grads.getCoordZ( j ) * param.getZScale();
						float z1 = grads.getCoordZ( j + 1 ) * param.getZScale();
						if ( param.getZUnit() == ZUNIT_KILOMETER ) {
							z0 *= 1000.0;
							z1 *= 1000.0;
						}
						float v0 = slicedata[ grads.getGridSizeY() *   j       + i     ];
						float v1 = slicedata[ grads.getGridSizeY() * ( j + 1 ) + i     ];
						float v2 = slicedata[ grads.getGridSizeY() * ( j + 1 ) + i + 1 ];
						float v3 = slicedata[ grads.getGridSizeY() *   j       + i + 1 ];
						QVector4D p0 = QVector4D( xpos, y0, z0, v0 );
						QVector4D p1 = QVector4D( xpos, y0, z1, v1 );
						QVector4D p2 = QVector4D( xpos, y1, z1, v2 );
						QVector4D p3 = QVector4D( xpos, y1, z0, v3 );

						makeLine( vertex, p0, p1, p2, value );
						makeLine( vertex, p0, p3, p2, value );
					}
				}
				lineData.push_back( vertex );
			}

			for ( int l = 0; l < lineLevel.size(); l++ ) {
				QVector<QVector3D> line = lineData.at(l);
				if ( line.size() != 0 ) {
					QVector< QVector<QVector3D> > reductedData;
					reductedData = reduction( lineData, l );
					for ( int i = 0; i < reductedData.size(); i++ ) {
						stream << "\t\t<Placemark>\n";
						QString styleID = QString( "linecolor%1" ).arg( l, 4, 10, QChar( '0' ) );
						stream << "\t\t\t<styleUrl>#" << styleID << "</styleUrl>\n";
						stream << "\t\t\t<LineString>\n";
						stream << "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
						stream << "\t\t\t\t<coordinates>\n";
						QVector< QVector3D > lineGroup = reductedData.at(i);
						for ( int j = 0; j < lineGroup.size(); j++ ) {
							QVector3D vec3d = lineGroup.at(j);
							stream << "\t\t\t\t\t"
								   << QString::number( vec3d.x(), 'f', 6 ) << ","
								   << QString::number( vec3d.y(), 'f', 6 ) << ","
								   << QString::number( vec3d.z(), 'f', 6 ) << "\n";
						}
						stream << "\t\t\t\t</coordinates>\n";
						stream << "\t\t\t</LineString>\n";
						stream << "\t\t</Placemark>\n";
					}
				}
			}

			stream << "\t</Folder>\n";
			stream << "</Document>\n";
			stream << "</kml>\n";

			file.close();

		}
	}

	delete [] slicedata;
}

void KML::exportContourLineY()
{
	if ( !param.IsContourLineY() ) {
		return;
	}

	QDir dir;
	dir.cd( m_DirPath + "/files/contourline" );
	dir.mkdir( "y" );
	dir.cd( "y" );

	// Generate KML
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating KML for ContourLineY..." ) );
	}

	*m_Stream << "\t\t<NetworkLink id=\"contourliney\">\n";
	*m_Stream << "\t\t\t<name>Y</name>\n";
	*m_Stream << "\t\t\t<open>0</open>\n";
	if ( m_MainObject == CONTOURLINE_Y ) {
		*m_Stream << "\t\t\t<visibility>1</visibility>\n";
	} else {
		*m_Stream << "\t\t\t<visibility>0</visibility>\n";
	}
	*m_Stream << "\t\t\t<Link>\n";
	*m_Stream << "\t\t\t\t<href>files/contourline/y/y.kml</href>\n";
	*m_Stream << "\t\t\t</Link>\n";
	*m_Stream << "\t\t</NetworkLink>\n";

	QString filename = m_DirPath + "/files/contourline/y/y.kml";
	QFile file( filename );
	file.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &file );
	stream.setRealNumberNotation( QTextStream::ScientificNotation );

	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
	stream << "<Document>\n";
	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) return;
				QApplication::processEvents();
			}

			QString TS_Begin = getTimeStringKML( getActualTime( time ) );
			QString TS_End   = getTimeStringKML( getActualTime( time + param.getTimeSkip() ) );
			stream << "\t<NetworkLink>\n";
			stream << "\t\t<name>T=" << TS_Begin << "</name>\n";
			stream << "\t\t<TimeSpan>\n";
			stream << "\t\t\t<begin>" << TS_Begin << "</begin>\n";
			stream << "\t\t\t<end>" << TS_End << "</end>\n";
			stream << "\t\t</TimeSpan>\n";
			stream << "\t\t<Link>\n";
			QString filename = QString( "%1.kml" )
									.arg( time, 4, 10, QChar( '0' ) );
			stream << "\t\t\t<href>" << filename << "</href>\n";
			stream << "\t\t</Link>\n";
			stream << "\t</NetworkLink>\n";
		}
	}
	stream << "</Document>\n";
	stream << "</kml>\n";
	file.close();

	double ypos = grads.getGridYMin() + ( grads.getGridYMax() - grads.getGridYMin() ) *
		static_cast< double >( param.getContourLinePosY() );
	int yidx = 0;
	if ( grads.getGridSizeY() != 1 ) {
		for ( int i = 0; i < grads.getGridSizeY() - 1 ; i++ ) {
			if ( grads.getCoordY( i + 1 ) >= ypos ) {
				yidx = i;
				break;
			}
		}
	}

	// Set Threshold Value and Line Color of Contourline
	QVector<float> lineLevel;
	QVector<QColor> lineColor;
	lineLevel.clear();
	lineColor.clear();
	float interval = param.getContourLineInterval();
	float thresMin = param.getContourLineThresholdMin();
	float thresMax = param.getContourLineThresholdMax();

	if ( thresMin < param.getVolumeThresholdMin() ) {
		thresMin = param.getVolumeThresholdMin();
	}
	if ( thresMax > param.getVolumeThresholdMax() ) {
		thresMax = param.getVolumeThresholdMax();
	}

	int i = 0;
	while (1) {
		float level = param.getContourLineThresholdMin() + interval * i;
		if ( level < thresMin ) {
			i++;
		} else if ( level <= thresMax ) {
			lineLevel.push_back( level );
			int colorID = static_cast< int >( ( level - param.getVolumeThresholdMin() ) /
				(  param.getVolumeThresholdMax() - param.getVolumeThresholdMin() ) * 254.0 );
			int r = static_cast< int >( param.getColorTableR( colorID ) * 255.0 );
			int g = static_cast< int >( param.getColorTableG( colorID ) * 255.0 );
			int b = static_cast< int >( param.getColorTableB( colorID ) * 255.0 );
			lineColor.push_back( QColor( r, g, b, 255 ) );
			i++;
		} else {
			break;
		}
	}

	float *slicedata = new float[ grads.getGridSizeX() * grads.getGridSizeZ() ];

	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ ) {
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) {
					// cleanup memories
					delete [] slicedata;
					return;
				}
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			QString filename = m_DirPath + "/files/contourline/y/" + QString( "%1" ).arg( time, 4, 10, QChar( '0' ) ) + ".kml";
			QFile file( filename );
			file.open( QIODevice::WriteOnly | QIODevice::Text );
			QTextStream stream( &file );
			stream.setRealNumberNotation( QTextStream::ScientificNotation );

			stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
			stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
			stream << "<Document>\n";
			stream << "\t<Folder>\n";
			QString latitudeStr = QString::number( ypos, 'f', 6 );
			stream << "\t\t<name>" << latitudeStr << "</name>\n";
			stream << "\t\t<Style>\n";
			stream << "\t\t\t<ListStyle>\n";
			stream << "\t\t\t\t<listItemType>checkHideChildren</listItemType>\n";
			stream << "\t\t\t</ListStyle>\n";
			stream << "\t\t</Style>\n";

			for ( int l = 0; l < lineLevel.size(); l++ ) {
				QString styleID = QString( "linecolor%1" ).arg( l, 4, 10, QChar( '0' ) );
				stream << "\t\t<Style id=\"" << styleID << "\">\n";
				stream << "\t\t\t<LineStyle>\n";
				int r = lineColor.at(l).red();
				int g = lineColor.at(l).green();
				int b = lineColor.at(l).blue();
				QString colorString = QString( "ff%1%2%3" )
					.arg( b, 2, 16, QChar( '0' ) )
					.arg( g, 2, 16, QChar( '0' ) )
					.arg( r, 2, 16, QChar( '0' ) );
				stream << "\t\t\t\t<color>" << colorString.toLower() << "</color>\n";
				stream << "\t\t\t\t<width>" << param.getContourLineWidth() << "</width>\n";
				stream << "\t\t\t</LineStyle>\n";
				stream << "\t\t</Style>\n";
			}

			grads.setData( time );
			float *data = grads.getData();
			std::fill( slicedata, slicedata + grads.getGridSizeX() * grads.getGridSizeZ(), 0 );

			// Generate Vertex to Draw Contourline
			for ( int j = 0; j < grads.getGridSizeZ(); j++ ) {
				for ( int i = 0; i < grads.getGridSizeX(); i++ ) {
					ulong idx0 = grads.getGridSizeX() * grads.getGridSizeY() * j + i + grads.getGridSizeX() * yidx;
					ulong idx1 = grads.getGridSizeX() * grads.getGridSizeY() * j + i + grads.getGridSizeX() * ( yidx + 1 );
					float ansf;
					if ( grads.getGridSizeY() == 1 ) {
						ansf = data[ idx0 ];
					} else {
						if ( ( data[ idx0 ] == grads.getUndef() ) || ( data[ idx1 ] == grads.getUndef() ) ) {
							ansf = grads.getUndef();
						} else {
							ansf = data[ idx0 ] + ( data[ idx1 ] - data[ idx0 ] ) *
								( ypos - grads.getCoordY( yidx ) ) / ( grads.getCoordY( yidx + 1 ) - grads.getCoordY( yidx ) );
						}
						slicedata[ grads.getGridSizeX() * j + i ] = ansf;
					}
				}
			}

			QVector< QVector<QVector3D> > lineData;
			for ( int level = 0; level < lineLevel.size(); level++ ) {
				float value = lineLevel.at( level );
				QVector<QVector3D> vertex;
				vertex.clear();
				for ( int j = 0; j < ( m_SizeZ - 1 ); j++ ) {
					for ( int i = 0; i < ( m_SizeX - 1 ); i++ ) {
						float x0 = grads.getCoordX( i );
						float x1 = grads.getCoordX( i + 1 );
						float z0 = grads.getCoordZ( j ) * param.getZScale();
						float z1 = grads.getCoordZ( j + 1 ) * param.getZScale();
						if ( param.getZUnit() == ZUNIT_KILOMETER ) {
							z0 *= 1000.0;
							z1 *= 1000.0;
						}
						float v0 = slicedata[ grads.getGridSizeX() *   j       + i     ];
						float v1 = slicedata[ grads.getGridSizeX() * ( j + 1 ) + i     ];
						float v2 = slicedata[ grads.getGridSizeX() * ( j + 1 ) + i + 1 ];
						float v3 = slicedata[ grads.getGridSizeX() *   j       + i + 1 ];
						QVector4D p0 = QVector4D( x0, ypos, z0, v0 );
						QVector4D p1 = QVector4D( x0, ypos, z1, v1 );
						QVector4D p2 = QVector4D( x1, ypos, z1, v2 );
						QVector4D p3 = QVector4D( x1, ypos, z0, v3 );

						makeLine( vertex, p0, p1, p2, value );
						makeLine( vertex, p0, p3, p2, value );
					}
				}
				lineData.push_back( vertex );
			}

			for ( int l = 0; l < lineLevel.size(); l++ ) {
				QVector<QVector3D> line = lineData.at(l);
				if ( line.size() != 0 ) {
					QVector< QVector<QVector3D> > reductedData;
					reductedData = reduction( lineData, l );
					for ( int i = 0; i < reductedData.size(); i++ ) {
						stream << "\t\t<Placemark>\n";
						QString styleID = QString( "linecolor%1" ).arg( l, 4, 10, QChar( '0' ) );
						stream << "\t\t\t<styleUrl>#" << styleID << "</styleUrl>\n";
						stream << "\t\t\t<LineString>\n";
						stream << "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
						stream << "\t\t\t\t<coordinates>\n";
						QVector< QVector3D > lineGroup = reductedData.at(i);
						for ( int j = 0; j < lineGroup.size(); j++ ) {
							QVector3D vec3d = lineGroup.at(j);
							stream << "\t\t\t\t\t"
								   << QString::number( vec3d.x(), 'f', 6 ) << ","
								   << QString::number( vec3d.y(), 'f', 6 ) << ","
								   << QString::number( vec3d.z(), 'f', 6 ) << "\n";
						}
						stream << "\t\t\t\t</coordinates>\n";
						stream << "\t\t\t</LineString>\n";
						stream << "\t\t</Placemark>\n";
					}
				}
			}

			stream << "\t</Folder>\n";
			stream << "</Document>\n";
			stream << "</kml>\n";

			file.close();

		}
	}

	delete [] slicedata;
}

void KML::exportContourLineZ()
{
	if ( !param.IsContourLineZ() ) {
		return;
	}

	QDir dir;
	dir.cd( m_DirPath + "/files/contourline" );
	dir.mkdir( "z" );
	dir.cd( "z" );

	// Generate KML
	if ( m_GUI ) {
		m_Progress->setLabelText(
			QString( "Generating KML for ContourLineZ..." ) );
	}

	*m_Stream << "\t\t<NetworkLink id=\"contourlinez\">\n";
	*m_Stream << "\t\t\t<name>Z</name>\n";
	*m_Stream << "\t\t\t<open>0</open>\n";
	if ( m_MainObject == CONTOURLINE_Z ) {
		*m_Stream << "\t\t\t<visibility>1</visibility>\n";
	} else {
		*m_Stream << "\t\t\t<visibility>0</visibility>\n";
	}
	*m_Stream << "\t\t\t<Link>\n";
	*m_Stream << "\t\t\t\t<href>files/contourline/z/z.kml</href>\n";
	*m_Stream << "\t\t\t</Link>\n";
	*m_Stream << "\t\t</NetworkLink>\n";

	QString filename = m_DirPath + "/files/contourline/z/z.kml";
	QFile file( filename );
	file.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &file );
	stream.setRealNumberNotation( QTextStream::ScientificNotation );

	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
	stream << "<Document>\n";
	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ )
	{
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) return;
				QApplication::processEvents();
			}

			QString TS_Begin = getTimeStringKML( getActualTime( time ) );
			QString TS_End   = getTimeStringKML( getActualTime( time + param.getTimeSkip() ) );
			stream << "\t<NetworkLink>\n";
			stream << "\t\t<name>T=" << TS_Begin << "</name>\n";
			stream << "\t\t<TimeSpan>\n";
			stream << "\t\t\t<begin>" << TS_Begin << "</begin>\n";
			stream << "\t\t\t<end>" << TS_End << "</end>\n";
			stream << "\t\t</TimeSpan>\n";
			stream << "\t\t<Link>\n";
			QString filename = QString( "%1.kml" )
									.arg( time, 4, 10, QChar( '0' ) );
			stream << "\t\t\t<href>" << filename << "</href>\n";
			stream << "\t\t</Link>\n";
			stream << "\t</NetworkLink>\n";
		}
	}
	stream << "</Document>\n";
	stream << "</kml>\n";
	file.close();

	double zpos = grads.getGridZMin() + ( grads.getGridZMax() - grads.getGridZMin() ) *
		static_cast< double >( param.getContourLinePosZ() );
	int zidx = 0;
	if ( grads.getGridSizeZ() != 1 ) {
		for ( int i = 0; i < grads.getGridSizeZ() - 1 ; i++ ) {
			if ( grads.getCoordZ( i + 1 ) >= zpos ) {
				zidx = i;
				break;
			}
		}
	}

	// Set Threshold Value and Line Color of Contourline
	QVector<float> lineLevel;
	QVector<QColor> lineColor;
	lineLevel.clear();
	lineColor.clear();
	float interval = param.getContourLineInterval();
	float thresMin = param.getContourLineThresholdMin();
	float thresMax = param.getContourLineThresholdMax();

	if ( thresMin < param.getVolumeThresholdMin() ) {
		thresMin = param.getVolumeThresholdMin();
	}
	if ( thresMax > param.getVolumeThresholdMax() ) {
		thresMax = param.getVolumeThresholdMax();
	}
	int i = 0;
	while (1) {
		float level = param.getContourLineThresholdMin() + interval * static_cast< float >( i );
		if ( level < thresMin ) {
			i++;
		} else if ( level <= thresMax ) {
			lineLevel.push_back( level );
			int colorID = static_cast< int >( ( level - param.getVolumeThresholdMin() ) /
				(  param.getVolumeThresholdMax() - param.getVolumeThresholdMin() ) * 254.0 );
			int r = static_cast< int >( param.getColorTableR( colorID ) * 255.0 );
			int g = static_cast< int >( param.getColorTableG( colorID ) * 255.0 );
			int b = static_cast< int >( param.getColorTableB( colorID ) * 255.0 );
			lineColor.push_back( QColor( r, g, b, 255 ) );
			i++;
		} else {
			break;
		}
	}

	float *slicedata = new float[ grads.getGridSizeX() * grads.getGridSizeY() ];

	for ( int time = param.getTimeBegin(); time <= param.getTimeEnd(); time++ ) {
		if ( ( ( time - param.getTimeBegin() ) % param.getTimeSkip() ) == 0 ) {
			if ( m_GUI ) {
				if ( m_Progress->wasCanceled() ) {
					// cleanup memories
					delete [] slicedata;
					return;
				}
				float percent;
				if ( param.getTimeBegin() == param.getTimeEnd() ) {
					percent = 100.0;
				} else {
					percent = static_cast< float >( time - param.getTimeBegin() )
						/ static_cast< float >( param.getTimeEnd() - param.getTimeBegin() ) * 100.0;
				}
				m_Progress->setValue( static_cast< int >( percent ) );
				QApplication::processEvents();
			}

			QString filename = m_DirPath + "/files/contourline/z/" + QString( "%1" ).arg( time, 4, 10, QChar( '0' ) ) + ".kml";
			QFile file( filename );
			file.open( QIODevice::WriteOnly | QIODevice::Text );
			QTextStream stream( &file );
			stream.setRealNumberNotation( QTextStream::ScientificNotation );

			stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
			stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
			stream << "<Document>\n";
			double realAltitude = zpos;
			double altitude = realAltitude * param.getZScale();
			if ( param.getZUnit() == ZUNIT_KILOMETER ) {
				altitude *= 1000.0;
			}
//			if ( altitude <= 0.0 ) {
//				altitude = 0.00000001;
//			}
			stream << "\t<Folder>\n";
			QString altitudeStr = QString::number( realAltitude, 'f', 2 );
			if ( param.getZUnit() == ZUNIT_KILOMETER ) {
				altitudeStr += "km";
			} else {
				altitudeStr += "m";
			}
			stream << "\t\t<name>" << altitudeStr << "</name>\n";
			stream << "\t\t<Style>\n";
			stream << "\t\t\t<ListStyle>\n";
			stream << "\t\t\t\t<listItemType>checkHideChildren</listItemType>\n";
			stream << "\t\t\t</ListStyle>\n";
			stream << "\t\t</Style>\n";

			for ( int l = 0; l < lineLevel.size(); l++ ) {
				QString styleID = QString( "linecolor%1" ).arg( l, 4, 10, QChar( '0' ) );
				stream << "\t\t<Style id=\"" << styleID << "\">\n";
				stream << "\t\t\t<LineStyle>\n";
				int r = lineColor.at(l).red();
				int g = lineColor.at(l).green();
				int b = lineColor.at(l).blue();
				QString colorString = QString( "ff%1%2%3" )
					.arg( b, 2, 16, QChar( '0' ) )
					.arg( g, 2, 16, QChar( '0' ) )
					.arg( r, 2, 16, QChar( '0' ) );
				stream << "\t\t\t\t<color>" << colorString.toLower() << "</color>\n";
				stream << "\t\t\t\t<width>" << param.getContourLineWidth() << "</width>\n";
				stream << "\t\t\t</LineStyle>\n";
				stream << "\t\t</Style>\n";
			}

			grads.setData( time );
			float *data = grads.getData();
			std::fill( slicedata, slicedata + grads.getGridSizeX() * grads.getGridSizeY(), 0 );

			// Generate Vertex to Draw Contourline
			for ( int j = 0; j < grads.getGridSizeY() ; j++ ) {
				for ( int i = 0; i < grads.getGridSizeX() ; i++ ) {
					ulong idx0 = grads.getGridSizeX() * grads.getGridSizeY() * zidx + grads.getGridSizeX() * j + i;
					ulong idx1 = grads.getGridSizeX() * grads.getGridSizeY() * ( zidx + 1 ) + grads.getGridSizeX() * j + i;
					float ansf;
					if ( grads.getGridSizeZ() == 1 ) {
						ansf = data[ idx0 ];
					} else {
						if ( ( data[ idx0 ] == grads.getUndef() ) || ( data[ idx1 ] == grads.getUndef() ) ) {
							ansf = grads.getUndef();
						} else {
							ansf = data[ idx0 ] + ( data[ idx1 ] - data[ idx0 ] ) *
								( zpos - grads.getCoordZ( zidx ) ) / ( grads.getCoordZ( zidx + 1 ) - grads.getCoordZ( zidx ) );
						}
					}
					slicedata[ grads.getGridSizeX() * j + i ] = ansf;

				}
			}

			QVector< QVector<QVector3D> > lineData;
			for ( int level = 0; level < lineLevel.size(); level++ ) {
				float value = lineLevel.at( level );
				QVector<QVector3D> vertex;
				vertex.clear();
				for ( int j = 0; j < ( grads.getGridSizeY() - 1 ); j++ ) {
					for ( int i = 0; i < ( grads.getGridSizeX() - 1 ); i++ ) {
						float x0 = grads.getCoordX( i );
						float x1 = grads.getCoordX( i + 1 );
						float y0 = grads.getCoordY( j );
						float y1 = grads.getCoordY( j + 1 );
						float v0 = slicedata[ grads.getGridSizeX() *   j       + i     ];
						float v1 = slicedata[ grads.getGridSizeX() * ( j + 1 ) + i     ];
						float v2 = slicedata[ grads.getGridSizeX() * ( j + 1 ) + i + 1 ];
						float v3 = slicedata[ grads.getGridSizeX() *   j       + i + 1 ];
						float z  = zpos * param.getZScale();
						if ( param.getZUnit() == ZUNIT_KILOMETER ) {
							z *= 1000.0;
						}
						QVector4D p0 = QVector4D( x0, y0, z, v0 );
						QVector4D p1 = QVector4D( x0, y1, z, v1 );
						QVector4D p2 = QVector4D( x1, y1, z, v2 );
						QVector4D p3 = QVector4D( x1, y0, z, v3 );

						makeLine( vertex, p0, p1, p2, value );
						makeLine( vertex, p0, p3, p2, value );
					}
				}
				lineData.push_back( vertex );
			}

			for ( int l = 0; l < lineLevel.size(); l++ ) {
				QVector<QVector3D> line = lineData.at(l);
				if ( line.size() != 0 ) {
					QVector< QVector<QVector3D> > reductedData;
					reductedData = reduction( lineData, l );
					for ( int i = 0; i < reductedData.size(); i++ ) {
						stream << "\t\t<Placemark>\n";
						QString styleID = QString( "linecolor%1" ).arg( l, 4, 10, QChar( '0' ) );
						stream << "\t\t\t<styleUrl>#" << styleID << "</styleUrl>\n";
						stream << "\t\t\t<LineString>\n";
						stream << "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
						stream << "\t\t\t\t<coordinates>\n";
						QVector< QVector3D > lineGroup = reductedData.at(i);
						for ( int j = 0; j < lineGroup.size(); j++ ) {
							QVector3D vec3d = lineGroup.at(j);
							stream << "\t\t\t\t\t"
								   << QString::number( vec3d.x(), 'f', 6 ) << ","
								   << QString::number( vec3d.y(), 'f', 6 ) << ","
								   << QString::number( vec3d.z(), 'f', 6 ) << "\n";
						}
						stream << "\t\t\t\t</coordinates>\n";
						stream << "\t\t\t</LineString>\n";
						stream << "\t\t</Placemark>\n";
					}
				}
			}

			stream << "\t</Folder>\n";
			stream << "</Document>\n";
			stream << "</kml>\n";

			file.close();

		}
	}

	delete [] slicedata;
}

void KML::makeLine( QVector<QVector3D>& vertex,
	QVector4D p0, QVector4D p1, QVector4D p2, float value )
{
	uchar pattern = 0;
	if ( ( p0.w() == 255.0 ) ||
		 ( p1.w() == 255.0 ) ||
		 ( p2.w() == 255.0 ) ) {
		return;
	}

	if ( p0.w() >= value ) {
		pattern |= 0x04;
	}
	if ( p1.w() >= value ) {
		pattern |= 0x02;
	}
	if ( p2.w() >= value ) {
		pattern |= 0x01;
	}

	QVector3D ip0, ip1;
	if ( ( pattern == 1 ) || ( pattern == 6 ) ) {
		ip0 = crossPoint( p0, p2, value );
		ip1 = crossPoint( p1, p2, value );
	} else if ( ( pattern == 2 ) || ( pattern == 5 ) ) {
		ip0 = crossPoint( p0, p1, value );
		ip1 = crossPoint( p1, p2, value );
	} else if ( ( pattern == 3 ) || ( pattern == 4 ) ) {
		ip0 = crossPoint( p0, p1, value );
		ip1 = crossPoint( p0, p2, value );
	} else {
		return;
	}

	if ( ip0 != ip1 ) {
		vertex.push_back( ip0 );
		vertex.push_back( ip1 );
	}
}

QVector3D KML::crossPoint( QVector4D p0, QVector4D p1, float value )
{
	float ratio;
	if ( p0.w() == p1.w() ) {
		ratio = 0;
	} else {
		ratio = ( value - p0.w() ) / ( p1.w() - p0.w() );
	}

	return QVector3D( p0.x() + ( p1.x() - p0.x() ) * ratio,
					  p0.y() + ( p1.y() - p0.y() ) * ratio,
					  p0.z() + ( p1.z() - p0.z() ) * ratio );
}

QVector< QVector<QVector3D> >
KML::reduction( QVector< QVector<QVector3D> >& lineData, int levelID )
{
	QVector<QVector3D> line = lineData.at( levelID );
	QVector< QVector<QVector3D> > lineGroup;
	QVector<QVector3D> lineParts;
	lineGroup.clear();
	while ( 1 ) {
		if ( line.isEmpty() ) {
			break;
		}
		lineParts.clear();
		lineParts.push_back( line.at( 0 ) );
		lineParts.push_back( line.at( 1 ) );
		line.remove( 0, 2 );
		while ( 1 ) {
			int idx = line.indexOf( lineParts.last() );
			if ( idx == -1 ) {
				break;
			} else {
				if ( ( idx % 2 ) == 0 ) {
					lineParts.push_back( line.at( idx + 1 ) );
					line.remove( idx, 2 );
				} else {
					lineParts.push_back( line.at( idx - 1 ) );
					line.remove( idx - 1, 2 );
				}
			}
		}
		while ( 1 ) {
			int idx = line.indexOf( lineParts.first() );
			if ( idx == -1 ) {
				break;
			} else {
				if ( ( idx % 2 ) == 0 ) {
					lineParts.push_front( line.at( idx + 1 ) );
					line.remove( idx, 2 );
				} else {
					lineParts.push_front( line.at( idx - 1 ) );
					line.remove( idx - 1, 2 );
				}
			}
		}
		if ( !lineParts.isEmpty() ) {
			lineGroup.push_back( lineParts );
		}
	}

	return lineGroup;
}

void KML::exportBoundingBox()
{
	if ( !param.IsBoundingBox() ) return;

	*m_Stream << "\t<NetworkLink id=\"boundingbox\">\n";
	*m_Stream << "\t\t<name>Bound</name>\n";
	*m_Stream << "\t\t<open>0</open>\n";
	*m_Stream << "\t\t<visibility>1</visibility>\n";
	*m_Stream << "\t\t<Style>\n";
	*m_Stream << "\t\t\t<ListStyle>\n";
	*m_Stream << "\t\t\t\t<listItemType>checkHideChildren</listItemType>\n";
	*m_Stream << "\t\t\t</ListStyle>\n";
	*m_Stream << "\t\t</Style>\n";
	*m_Stream << "\t\t<Link>\n";
	*m_Stream << "\t\t\t<href>files/bound.kml</href>\n";
	*m_Stream << "\t\t</Link>\n";
	*m_Stream << "\t</NetworkLink>\n";

	QString boundFileName = m_DirPath + "/files/bound.kml";
	QFile boundFile( boundFileName );
	boundFile.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &boundFile );
	stream.setRealNumberNotation( QTextStream::ScientificNotation );

	float height0 = 0.0;
	float height1 = 0.0;
	if ( param.getZUnit() == ZUNIT_METER ) {
		height0 = m_MinZ * param.getZScale(); // km -> m
		height1 = m_MaxZ * param.getZScale(); // km -> m
	} else if ( param.getZUnit() == ZUNIT_KILOMETER ) {
		height0 = m_MinZ * 1000.0 * param.getZScale(); // km -> m
		height1 = m_MaxZ * 1000.0 * param.getZScale(); // km -> m
	}

	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	stream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n";
	stream << "<Document>\n";
	stream << "\t<Folder>\n";
	stream << "\t\t<name>Bounding Box</name>\n";
	stream << "\t\t<open>0</open>\n";
	stream << "\t\t<Style>\n";
	stream << "\t\t\t<ListStyle>\n";
	stream << "\t\t\t\t<listItemType>checkHideChildren</listItemType>\n";
	stream << "\t\t\t</ListStyle>\n";
	stream << "\t\t</Style>\n";
	stream << "\t\t<Placemark>\n";
	stream << "\t\t\t<name>Bounding Box A</name>\n";
	stream << "\t\t\t<LineString>\n";
	stream << "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
	stream << "\t\t\t\t<coordinates>\n";

	if ( static_cast< float >( m_MaxX - m_MinX ) != 360.0 ) {
		stream << "\t\t\t\t\t"
			   << QString::number( m_MinX, 'f', 6 ) << ","
			   << QString::number( m_MinY, 'f', 6 ) << ","
			   << QString::number( height1, 'f', 6 ) << "\n";
	}
	stream << "\t\t\t\t\t"
		   << QString::number( m_MinX, 'f', 6 ) << ","
		   << QString::number( m_MinY, 'f', 6 ) << ","
		   << QString::number( height0, 'f', 6 ) << "\n";
	for ( int i = 0; i < static_cast< int >( m_MaxX - m_MinX ); i++ ) {
		stream << "\t\t\t\t\t"
			   << QString::number( m_MinX + ( i + 1 ), 'f', 6 ) << ","
			   << QString::number( m_MinY, 'f', 6 ) << ","
			   << QString::number( height0, 'f', 6 ) << "\n";
	}
	stream << "\t\t\t\t\t"
		   << QString::number( m_MaxX, 'f', 6 ) << ","
		   << QString::number( m_MinY, 'f', 6 ) << ","
		   << QString::number( height0, 'f', 6 ) << "\n";
	if ( static_cast< float >( m_MaxX - m_MinX ) != 360.0 ) {
		stream << "\t\t\t\t\t"
			   << QString::number( m_MaxX, 'f', 6 ) << ","
			   << QString::number( m_MinY, 'f', 6 ) << ","
			   << QString::number( height1, 'f', 6 ) << "\n";
	}

	stream << "\t\t\t\t</coordinates>\n";
	stream << "\t\t\t</LineString>\n";
	stream << "\t\t</Placemark>\n";

	stream << "\t\t<Placemark>\n";
	stream << "\t\t\t<name>Bounding Box B</name>\n";
	stream << "\t\t\t<LineString>\n";
	stream << "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
	stream << "\t\t\t\t<coordinates>\n";
	if ( static_cast< float >( m_MaxX - m_MinX ) != 360.0 ) {
		stream << "\t\t\t\t\t"
			   << QString::number( m_MinX, 'f', 6 ) << ","
			   << QString::number( m_MaxY, 'f', 6 ) << ","
			   << QString::number( height1, 'f', 6 ) << "\n";
	}
	stream << "\t\t\t\t\t"
		   << QString::number( m_MinX, 'f', 6 ) << ","
		   << QString::number( m_MaxY, 'f', 6 ) << ","
		   << QString::number( height0, 'f', 6 ) << "\n";
	for ( int i = 0; i < static_cast< int >( m_MaxX - m_MinX ); i++ ) {
		stream << "\t\t\t\t\t"
			   << QString::number( m_MinX + ( i + 1 ), 'f', 6 ) << ","
			   << QString::number( m_MaxY, 'f', 6 ) << ","
			   << QString::number( height0, 'f', 6 ) << "\n";
	}
	stream << "\t\t\t\t\t"
		   << QString::number( m_MaxX, 'f', 6 ) << ","
		   << QString::number( m_MaxY, 'f', 6 ) << ","
		   << QString::number( height0, 'f', 6 ) << "\n";
	if ( static_cast< float >( m_MaxX - m_MinX ) != 360.0 ) {
		stream << "\t\t\t\t\t"
			   << QString::number( m_MaxX, 'f', 6 ) << ","
			   << QString::number( m_MaxY, 'f', 6 ) << ","
			   << QString::number( height1, 'f', 6 ) << "\n";
	}
	stream << "\t\t\t\t</coordinates>\n";
	stream << "\t\t\t</LineString>\n";
	stream << "\t\t</Placemark>\n";

	stream << "\t\t<Placemark>\n";
	stream << "\t\t\t<name>Bounding Box C</name>\n";
	stream << "\t\t\t<LineString>\n";
	stream << "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
	stream << "\t\t\t\t<coordinates>\n";
	stream << "\t\t\t\t\t"
		   << QString::number( m_MinX, 'f', 6 ) << ","
		   << QString::number( m_MinY, 'f', 6 ) << ","
		   << QString::number( height1, 'f', 6 ) << "\n";
	for ( int i = 0; i < static_cast< int >( m_MaxX - m_MinX ); i++ ) {
		stream << "\t\t\t\t\t"
			   << QString::number( m_MinX + ( i + 1 ), 'f', 6 ) << ","
			   << QString::number( m_MinY, 'f', 6 ) << ","
			   << QString::number( height1, 'f', 6 ) << "\n";
	}
	stream << "\t\t\t\t\t"
		   << QString::number( m_MaxX, 'f', 6 ) << ","
		   << QString::number( m_MinY, 'f', 6 ) << ","
		   << QString::number( height1, 'f', 6 ) << "\n";
	stream << "\t\t\t\t</coordinates>\n";
	stream << "\t\t\t</LineString>\n";
	stream << "\t\t</Placemark>\n";

	stream << "\t\t<Placemark>\n";
	stream << "\t\t\t<name>Bounding Box D</name>\n";
	stream << "\t\t\t<LineString>\n";
	stream << "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
	stream << "\t\t\t\t<coordinates>\n";
	stream << "\t\t\t\t\t"
		   << QString::number( m_MinX, 'f', 6 ) << ","
		   << QString::number( m_MaxY, 'f', 6 ) << ","
		   << QString::number( height1, 'f', 6 ) << "\n";
	for ( int i = 0; i < static_cast< int >( m_MaxX - m_MinX ); i++ ) {
		stream << "\t\t\t\t\t"
			   << QString::number( m_MinX + ( i + 1 ), 'f', 6 ) << ","
			   << QString::number( m_MaxY, 'f', 6 ) << ","
			   << QString::number( height1, 'f', 6 ) << "\n";
	}
	stream << "\t\t\t\t\t"
		   << QString::number( m_MaxX, 'f', 6 ) << ","
		   << QString::number( m_MaxY, 'f', 6 ) << ","
		   << QString::number( height1, 'f', 6 ) << "\n";
	stream << "\t\t\t\t</coordinates>\n";
	stream << "\t\t\t</LineString>\n";
	stream << "\t\t</Placemark>\n";

	if ( static_cast< float >( m_MaxX - m_MinX ) != 360.0 ) {
		stream << "\t\t<Placemark>\n";
		stream << "\t\t\t<name>Bounding Box E</name>\n";
		stream << "\t\t\t<LineString>\n";
		stream << "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
		stream << "\t\t\t\t<coordinates>\n";
		stream << "\t\t\t\t\t"
			   << QString::number( m_MinX, 'f', 6 ) << ","
			   << QString::number( m_MinY, 'f', 6 ) << ","
			   << QString::number( height0, 'f', 6 ) << "\n";
		for ( int i = 0; i < static_cast< int >( m_MaxY - m_MinY ); i++ ) {
			stream << "\t\t\t\t\t"
				   << QString::number( m_MinX, 'f', 6 ) << ","
				   << QString::number( m_MinY + ( i + 1 ), 'f', 6 ) << ","
				   << QString::number( height0, 'f', 6 ) << "\n";
		}
		stream << "\t\t\t\t\t"
			   << QString::number( m_MinX, 'f', 6 ) << ","
			   << QString::number( m_MaxY, 'f', 6 ) << ","
			   << QString::number( height0, 'f', 6 ) << "\n";
		stream << "\t\t\t\t</coordinates>\n";
		stream << "\t\t\t</LineString>\n";
		stream << "\t\t</Placemark>\n";

		stream << "\t\t<Placemark>\n";
		stream << "\t\t\t<name>Bounding Box F</name>\n";
		stream << "\t\t\t<LineString>\n";
		stream << "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
		stream << "\t\t\t\t<coordinates>\n";
		stream << "\t\t\t\t\t"
			   << QString::number( m_MaxX, 'f', 6 ) << ","
			   << QString::number( m_MinY, 'f', 6 ) << ","
			   << QString::number( height0, 'f', 6 ) << "\n";
		for ( int i = 0; i < static_cast< int >( m_MaxY - m_MinY ); i++ ) {
			stream << "\t\t\t\t\t"
				   << QString::number( m_MaxX, 'f', 6 ) << ","
				   << QString::number( m_MinY + ( i + 1 ), 'f', 6 ) << ","
				   << QString::number( height0, 'f', 6 ) << "\n";
		}
		stream << "\t\t\t\t\t"
			   << QString::number( m_MaxX, 'f', 6 ) << ","
			   << QString::number( m_MaxY, 'f', 6 ) << ","
			   << QString::number( height0, 'f', 6 ) << "\n";
		stream << "\t\t\t\t</coordinates>\n";
		stream << "\t\t\t</LineString>\n";
		stream << "\t\t</Placemark>\n";

		stream << "\t\t<Placemark>\n";
		stream << "\t\t\t<name>Bounding Box G</name>\n";
		stream << "\t\t\t<LineString>\n";
		stream << "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
		stream << "\t\t\t\t<coordinates>\n";
		stream << "\t\t\t\t\t"
			   << QString::number( m_MinX, 'f', 6 ) << ","
			   << QString::number( m_MinY, 'f', 6 ) << ","
			   << QString::number( height1, 'f', 6 ) << "\n";
		for ( int i = 0; i < static_cast< int >( m_MaxY - m_MinY ); i++ ) {
			stream << "\t\t\t\t\t"
				   << QString::number( m_MinX, 'f', 6 ) << ","
				   << QString::number( m_MinY + ( i + 1 ), 'f', 6 ) << ","
				   << QString::number( height1, 'f', 6 ) << "\n";
		}
		stream << "\t\t\t\t\t"
			   << QString::number( m_MinX, 'f', 6 ) << ","
			   << QString::number( m_MaxY, 'f', 6 ) << ","
			   << QString::number( height1, 'f', 6 ) << "\n";
		stream << "\t\t\t\t</coordinates>\n";
		stream << "\t\t\t</LineString>\n";
		stream << "\t\t</Placemark>\n";

		stream << "\t\t<Placemark>\n";
		stream << "\t\t\t<name>Bounding Box H</name>\n";
		stream << "\t\t\t<LineString>\n";
		stream << "\t\t\t\t<altitudeMode>absolute</altitudeMode>\n";
		stream << "\t\t\t\t<coordinates>\n";
		stream << "\t\t\t\t\t"
			   << QString::number( m_MaxX, 'f', 6 ) << ","
			   << QString::number( m_MinY, 'f', 6 ) << ","
			   << QString::number( height1, 'f', 6 ) << "\n";
		for ( int i = 0; i < static_cast< int >( m_MaxY - m_MinY ); i++ ) {
			stream << "\t\t\t\t\t"
				   << QString::number( m_MaxX, 'f', 6 ) << ","
				   << QString::number( m_MinY + ( i + 1 ), 'f', 6 ) << ","
				   << QString::number( height1, 'f', 6 ) << "\n";
		}
		stream << "\t\t\t\t\t"
			   << QString::number( m_MaxX, 'f', 6 ) << ","
			   << QString::number( m_MaxY, 'f', 6 ) << ","
			   << QString::number( height1, 'f', 6 ) << "\n";
		stream << "\t\t\t\t</coordinates>\n";
		stream << "\t\t\t</LineString>\n";
		stream << "\t\t</Placemark>\n";
	}

	stream << "\t</Folder>\n";
	stream << "</Document>\n";
	stream << "</kml>\n";

	boundFile.close();
}

void KML::exportColorBar()
{
	if ( !param.IsColorBar() ) return;

	*m_Stream << "\t<NetworkLink id=\"colorbar\">\n";
	*m_Stream << "\t\t<name>ColorBar</name>\n";
	*m_Stream << "\t\t<open>0</open>\n";
	*m_Stream << "\t\t<visibility>1</visibility>\n";
	*m_Stream << "\t\t<Style>\n";
	*m_Stream << "\t\t\t<ListStyle>\n";
	*m_Stream << "\t\t\t\t<listItemType>checkHideChildren</listItemType>\n";
	*m_Stream << "\t\t\t</ListStyle>\n";
	*m_Stream << "\t\t</Style>\n";
	*m_Stream << "\t\t<Link>\n";
	*m_Stream << "\t\t\t<href>files/colorbar.kml</href>\n";
	*m_Stream << "\t\t</Link>\n";
	*m_Stream << "\t</NetworkLink>\n";

	ColorBarType type = param.getColorBarType();

	QFont font( "Arial" );
	font.setPointSize( 12 );
	font.setBold( true );
	QFontMetrics fontMetrics( font );

	int FONT_HEIGHT = fontMetrics.height();
	int LINE_SPACE = 5;
	int COLORBAR_WIDTH  = 510;
	int COLORBAR_HEIGHT = 20;
	int IMAGE_WIDTH  = 600;
	int IMAGE_HEIGHT;
	int MAJOR_TICKMARK_LENGTH = 4;
	int MINOR_TICKMARK_LENGTH = 2;

	if ( param.getColorBarOrientation() == VERTICAL ) {
		IMAGE_HEIGHT = COLORBAR_HEIGHT + FONT_HEIGHT + LINE_SPACE * 4 + 100;
	} else {
		IMAGE_HEIGHT = COLORBAR_HEIGHT + FONT_HEIGHT * 2 + LINE_SPACE * 4;
	}
	if ( type == COLORBAR_RGB_AND_RGBA ) {
		IMAGE_HEIGHT += COLORBAR_HEIGHT;
	}

	// set QString of varieble description
	QString description = grads.getVarDescription( 0 );
	if ( param.IsLogScale() ) {
		description.append( " (log scale)" );
	}

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	int string_width = fontMetrics.boundingRect(description).width();
//	int string_width = fontMetrics.horizontalAdvance(description);
#else
	int string_width = fontMetrics.width( description );
#endif
	if ( string_width > IMAGE_WIDTH ) {
		IMAGE_WIDTH = string_width + LINE_SPACE * 2;
	}
	int string_half_width = static_cast< int >(
		static_cast< float >( string_width ) / 2.0 );
	int offset_x = IMAGE_WIDTH / 2 - string_half_width;
	int offset_y = LINE_SPACE;

	QImage imageCB( COLORBAR_WIDTH, COLORBAR_HEIGHT, QImage::Format_Indexed8 );
	uchar *slicedata = new uchar[ COLORBAR_WIDTH ];
	for ( int i = 0; i < COLORBAR_WIDTH; i++ ) {
		slicedata[ i ] = static_cast< int >( static_cast< float >( i ) / 2.0 );
	}

	for ( int i = 0; i < COLORBAR_HEIGHT; i++ ) {
		memcpy( imageCB.scanLine( i ), slicedata, COLORBAR_WIDTH );
	}
	imageCB.setColorTable( m_ColorTableRGB );
	QImage imageCB_RGB = imageCB.convertToFormat( QImage::Format_ARGB32 );
	imageCB.setColorTable( m_ColorTable );
	QImage imageCB_RGBA = imageCB.convertToFormat( QImage::Format_ARGB32 );

	// create QImage for output and set QPainter
	QImage image( IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_ARGB32 );
	image.fill( 0 );
	QPainter painter( &image );
	QPen pen( Qt::white );
	pen.setWidth( 3 );
	painter.setPen( pen );
	painter.setFont( font );

	// draw description of varieble
	painter.drawText( QRect( offset_x, offset_y,
		string_width, FONT_HEIGHT ), Qt::AlignLeft, description );

	// make pixmap of checker pattern
	QImage pixmap( 20, 20, QImage::Format_ARGB32 );
	QPainter checkerPattern( &pixmap );
	checkerPattern.fillRect(  0,  0, 10, 10, Qt::lightGray );
	checkerPattern.fillRect( 10, 10, 10, 10, Qt::lightGray );
	checkerPattern.fillRect(  0, 10, 10, 10, Qt::darkGray  );
	checkerPattern.fillRect( 10,  0, 10, 10, Qt::darkGray  );
	checkerPattern.end();

	// draw body of colorbar
	offset_x = static_cast< int >(
		static_cast< float >( IMAGE_WIDTH - COLORBAR_WIDTH ) / 2.0 );
	offset_y += FONT_HEIGHT + LINE_SPACE;
	if ( ( type == COLORBAR_RGB ) || ( type == COLORBAR_RGBA )) {
		if ( type == COLORBAR_RGB ) {
			painter.drawImage( offset_x, offset_y, imageCB_RGB );
		} else {
			painter.fillRect( offset_x, offset_y,
				COLORBAR_WIDTH, COLORBAR_HEIGHT, QBrush( pixmap ));
			painter.drawImage( offset_x, offset_y, imageCB_RGBA );
		}
		painter.drawRect( offset_x - 1, offset_y - 1,
						  COLORBAR_WIDTH + 1, COLORBAR_HEIGHT + 1 );
	} else {
		painter.drawImage( offset_x, offset_y, imageCB_RGB );
		painter.fillRect( offset_x, offset_y + COLORBAR_HEIGHT,
			COLORBAR_WIDTH, COLORBAR_HEIGHT, QBrush( pixmap ));
		painter.drawImage( offset_x, offset_y + COLORBAR_HEIGHT, imageCB_RGBA );
		painter.drawRect( offset_x - 1, offset_y - 1,
						  COLORBAR_WIDTH + 1, COLORBAR_HEIGHT * 2 + 1 );
	}

	// draw minor tickmark
	int minorTickMarkNum = param.getColorBarMinorTickMarkNum();
	int majorTickMarkNum = param.getColorBarMajorTickMarkNum();
	ColorBarTickMarkType tickMarkType;
	tickMarkType = param.getColorBarMinorTickMarkType();
	if ( tickMarkType != TICKMARK_NONE ) {
		int marknum = majorTickMarkNum
			+ ( minorTickMarkNum * ( majorTickMarkNum - 1 ) );
		for ( int i = 0; i < marknum; i++ ) {
			int dx = static_cast< int >(
				static_cast< float >( COLORBAR_WIDTH + 1 ) *
				static_cast< float >( i ) /
				static_cast< float >( marknum - 1 ) );
			int x0 = offset_x - 1 + dx;
			int y0, y1;
			if ( tickMarkType == TICKMARK_OUTSIDE ) {
				y0 = offset_y + COLORBAR_HEIGHT;
				if ( type == COLORBAR_RGB_AND_RGBA ) {
					y0 += COLORBAR_HEIGHT;
				}
				y1 = y0 + MINOR_TICKMARK_LENGTH;
			} else if ( tickMarkType == TICKMARK_INSIDE ) {
				y0 = offset_y + COLORBAR_HEIGHT;
				if ( type == COLORBAR_RGB_AND_RGBA ) {
					y0 += COLORBAR_HEIGHT;
				}
				y1 = y0 - MINOR_TICKMARK_LENGTH;
			} else {
				y0 = offset_y - 1;
				y1 = offset_y + COLORBAR_HEIGHT;
				if ( type == COLORBAR_RGB_AND_RGBA ) {
					y1 += COLORBAR_HEIGHT;
				}
			}
			painter.drawLine( x0, y0, x0, y1 );
		}
	}

	// draw major tickmark
	tickMarkType = param.getColorBarMajorTickMarkType();
	if ( tickMarkType != TICKMARK_NONE ) {
		for ( int i = 0; i < majorTickMarkNum; i++ ) {
			int dx = static_cast< int >(
				static_cast< float >( COLORBAR_WIDTH + 1 ) *
				static_cast< float >( i ) /
				static_cast< float >( majorTickMarkNum - 1 ) );
			int x0 = offset_x - 1 + dx;
			int y0, y1;
			if ( tickMarkType == TICKMARK_OUTSIDE ) {
				y0 = offset_y + COLORBAR_HEIGHT;
				if ( type == COLORBAR_RGB_AND_RGBA ) {
					y0 += COLORBAR_HEIGHT;
				}
				y1 = y0 + MAJOR_TICKMARK_LENGTH;
			} else if ( tickMarkType == TICKMARK_INSIDE ) {
				y0 = offset_y + COLORBAR_HEIGHT;
				if ( type == COLORBAR_RGB_AND_RGBA ) {
					y0 += COLORBAR_HEIGHT;
				}
				y1 = y0 - MAJOR_TICKMARK_LENGTH;
			} else {
				y0 = offset_y - 1;
				y1 = offset_y + COLORBAR_HEIGHT;
				if ( type == COLORBAR_RGB_AND_RGBA ) {
					y1 += COLORBAR_HEIGHT;
				}
			}
			painter.drawLine( x0, y0, x0, y1 );
		}
	}

	QTransform transform;
	transform.rotate( -90.0 );
	QImage vImage = image.transformed( transform );
	QPainter vPainter( &vImage );
	vPainter.setPen( Qt::white );
	vPainter.setFont( font );

	// draw scales
	font.setPointSize( 8 );
	FONT_HEIGHT = fontMetrics.height();
	int marknum = param.getColorBarMajorTickMarkNum();
	float dx = static_cast< float >( COLORBAR_WIDTH ) / static_cast< float >( marknum - 1 );
	int start_x = static_cast< int >( static_cast< float >( IMAGE_WIDTH - COLORBAR_WIDTH ) / 2.0 );
	offset_y += COLORBAR_HEIGHT + LINE_SPACE;
	if ( type == COLORBAR_RGB_AND_RGBA ) {
		offset_y += COLORBAR_HEIGHT;
	}
	char format[ 32 ];
	char str[ 32 ];
	char strtmp1[ 32 ];
	char strtmp2[ 32 ];
	for ( int i = 0; i < marknum; i++ ) {
		float value;
		if ( !param.IsLogScale() ) {
			value = param.getVolumeThresholdMin()
				+ ( param.getVolumeThresholdMax() - param.getVolumeThresholdMin() )
				/ static_cast< float >( marknum - 1 ) * static_cast< float >( i );
		} else {
			float logvalue = log10( param.getVolumeThresholdMin() )
				+ ( log10( param.getVolumeThresholdMax() ) - log10( param.getVolumeThresholdMin() ) )
				/ static_cast< float >( marknum - 1 ) * static_cast< float >( i );
			value = pow( 10.0, logvalue );
		}
		if ( ( value == 0.0 ) || ( ( fabs( value ) > 0.0001 ) && ( fabs( value ) < 10000.0 ) ) ) {
			sprintf( format, "%%.%df", param.getColorBarDecimalPlacesNum() );
			sprintf( str, format, value );
		} else {
			sprintf( format, "%%.%de", param.getColorBarDecimalPlacesNum() );
			sprintf( strtmp1, format, value );
			int eposition = 0;
            int length = static_cast< int >( strlen( strtmp1 ) );
			for ( int p = 0; p < length; p++ ) {
				if ( strtmp1[ p ] == 'e' ) {
					eposition = p;
					break;
				}
			}
			int q = 0;
			for ( int p = ( eposition + 2 ); p < length; p++ ) {
				strtmp2[ q ] = strtmp1[ p ];
				q++;
			}
			strtmp1[ eposition + 2 ] = '\0';
			strtmp2[ q ] = '\0';
			int exp = atoi( strtmp2 );
			sprintf( str, "%s%d", strtmp1, exp );
		}
		QString string = QString( str );
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
		string_width = fontMetrics.boundingRect(string).width();
//		string_width = fontMetrics.horizontalAdvance(string);
#else
		string_width = fontMetrics.width( string );
#endif
		if ( param.getColorBarOrientation() == VERTICAL ) {
			int font_half_height = static_cast< int >( static_cast< float >( FONT_HEIGHT ) / 2.0 );
			offset_x = IMAGE_WIDTH - ( start_x + static_cast< int >( dx * i ) ) - font_half_height;
			vPainter.drawText( QRect( offset_y, offset_x,
				string_width, FONT_HEIGHT ), Qt::AlignLeft, string );
		} else {
			string_half_width = static_cast< int >( static_cast< float >( string_width ) / 2.0 );
			offset_x = start_x + static_cast< int >( dx * i ) - string_half_width;
			painter.drawText( QRect( offset_x, offset_y,
				string_width, FONT_HEIGHT ), Qt::AlignLeft, string );
		}
	}

	QString filename = QString( m_DirPath + "/files/colorbar.png" );
	if ( param.getColorBarOrientation() == VERTICAL ) {
		vImage.save( filename );
	} else {
		image.save( filename );
	}

	QString colorbarFileName = m_DirPath + "/files/colorbar.kml";
	QFile colorbarFile( colorbarFileName );
	colorbarFile.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &colorbarFile );
	stream.setRealNumberNotation( QTextStream::ScientificNotation );

	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	stream << "<kml\n";
	stream << "xmlns=\"http://earth.google.com/kml/2.2\"\n";
	stream << "xmlns:gx=\"http://www.google.com/kml/ext/2.2\"\n";
	stream << "xmlns:kml=\"http://www.opengis.net/kml/2.2\"\n";
	stream << "xmlns:atom=\"http://www.w3.org/2005/Atom\">\n";
	stream << "<Document>\n";
	stream << "\t<Folder>\n";
	stream << "\t\t<name>ColorBar</name>\n";
	stream << "\t\t<open>0</open>\n";
	stream << "\t\t<Style>\n";
	stream << "\t\t\t<ListStyle>\n";
	stream << "\t\t\t\t<listItemType>checkHideChildren</listItemType>\n";
	stream << "\t\t\t</ListStyle>\n";
	stream << "\t\t</Style>\n";
	stream << "\t\t<ScreenOverlay>\n";
	stream << "\t\t\t<name>ColorBar</name>\n";
	stream << "\t\t\t<Icon>\n";
	stream << "\t\t\t\t<href>colorbar.png</href>\n";
	stream << "\t\t\t</Icon>\n";
	if ( param.getColorBarOrientation() == VERTICAL ) {
		stream << "\t\t\t<overlayXY x=\"0\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n";
		stream << "\t\t\t<screenXY x=\"0.05\" y=\"0.275\" xunits=\"fraction\" yunits=\"fraction\"/>\n";
		stream << "\t\t\t<size x=\"0\" y=\"0.45\" xunits=\"fraction\" yunits=\"fraction\"/>\n";
	} else {
		stream << "\t\t\t<overlayXY x=\"0\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n";
		stream << "\t\t\t<screenXY x=\"0.7\" y=\"0.1\" xunits=\"fraction\" yunits=\"fraction\"/>\n";
		stream << "\t\t\t<size x=\"0.3\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n";
	}
	stream << "\t\t</ScreenOverlay>\n";
	stream << "\t</Folder>\n";
	stream << "</Document>\n";
	stream << "</kml>\n";

	colorbarFile.close();
}

void KML::exportLogo()
{
	if ( !param.IsLogo() || ( param.getLogo() == "" ) ) return;

	QFile logoImage( param.getLogo() );
	logoImage.copy( m_DirPath + "/files/" + QFileInfo( logoImage ).fileName() );

	*m_Stream << "\t<NetworkLink id=\"logo\">\n";
	*m_Stream << "\t\t<name>Logo</name>\n";
	*m_Stream << "\t\t<open>0</open>\n";
	*m_Stream << "\t\t<visibility>1</visibility>\n";
	*m_Stream << "\t\t<Style>\n";
	*m_Stream << "\t\t\t<ListStyle>\n";
	*m_Stream << "\t\t\t\t<listItemType>checkHideChildren</listItemType>\n";
	*m_Stream << "\t\t\t</ListStyle>\n";
	*m_Stream << "\t\t</Style>\n";
	*m_Stream << "\t\t<Link>\n";
	*m_Stream << "\t\t\t<href>files/logo.kml</href>\n";
	*m_Stream << "\t\t</Link>\n";
	*m_Stream << "\t</NetworkLink>\n";

	QString logoFileName = m_DirPath + "/files/logo.kml";
	QFile logoFile( logoFileName );
	logoFile.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &logoFile );
	stream.setRealNumberNotation( QTextStream::ScientificNotation );

	stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	stream << "<kml\n";
	stream << "xmlns=\"http://earth.google.com/kml/2.2\"\n";
	stream << "xmlns:gx=\"http://www.google.com/kml/ext/2.2\"\n";
	stream << "xmlns:kml=\"http://www.opengis.net/kml/2.2\"\n";
	stream << "xmlns:atom=\"http://www.w3.org/2005/Atom\">\n";
	stream << "<Document>\n";
	stream << "\t<Folder>\n";
	stream << "\t\t<name>Logo</name>\n";
	stream << "\t\t<open>0</open>\n";
	stream << "\t\t<Style>\n";
	stream << "\t\t\t<ListStyle>\n";
	stream << "\t\t\t\t<listItemType>checkHideChildren</listItemType>\n";
	stream << "\t\t\t</ListStyle>\n";
	stream << "\t\t</Style>\n";
	stream << "\t\t<ScreenOverlay>\n";
	stream << "\t\t\t<name>Logo</name>\n";
	stream << "\t\t\t<Icon>\n";
	stream << "\t\t\t\t<href>" << QFileInfo( logoImage ).fileName() << "</href>\n";
	stream << "\t\t\t</Icon>\n";
	stream << "\t\t\t<overlayXY x=\"0\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n";
	stream << "\t\t\t<screenXY x=\"0.05\" y=\"0.05\" xunits=\"fraction\" yunits=\"fraction\"/>\n";
	stream << "\t\t\t<size x=\"0.1\" y=\"0\" xunits=\"fraction\" yunits=\"fraction\"/>\n";
	stream << "\t\t</ScreenOverlay>\n";
	stream << "\t</Folder>\n";
	stream << "</Document>\n";
	stream << "</kml>\n";

	logoFile.close();
}

void KML::setStateAltitudeModeOfColorSliceZ()
{

}

KMLSettingDialog::KMLSettingDialog( QWidget *parent )
	: QDialog( parent )
{
	m_RadioButton_Absolute = new QRadioButton( "absolute", 0 );
	m_RadioButton_ClampToGround = new QRadioButton( "clampToGround", 0 );
	m_RadioButton_Absolute->setChecked( true );
	m_RadioButton_ClampToGround->setChecked( false );

	m_PushButton_Accept = new QPushButton( tr( "&Accept" ) );

	QVBoxLayout *vBoxLayout = new QVBoxLayout;
	vBoxLayout->addWidget( m_RadioButton_Absolute );
	vBoxLayout->addWidget( m_RadioButton_ClampToGround );
	vBoxLayout->addWidget( m_PushButton_Accept );
	setLayout( vBoxLayout );

	setWindowTitle( tr( "Select Altitude Mode of ColorSlice" ) );
	setMinimumWidth( 500 );

	connect( m_PushButton_Accept, SIGNAL( clicked() ),
			 this, SLOT( getStateAltitudeMode() ) );
	connect( m_PushButton_Accept, SIGNAL( clicked() ),
			 this, SLOT( accept() ) );
}

AltitudeMode KMLSettingDialog::getStateAltitudeMode()
{
	if ( m_RadioButton_Absolute->isChecked() ) {
		return ALTITUDE_ABSOLUTE;
	} else {
		return ALTITUDE_CLAMP_TO_GROUND;
	}
}
