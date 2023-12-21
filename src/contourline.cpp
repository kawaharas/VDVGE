//
// Volume Data Visualizer for Google Earth
//
// contourline.cpp
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

#include "contourline.h"

void ContourLine::initialize()
{
	setLevels();
	recalc();
}

void ContourLine::setLevels()
{
	float interval = param.getContourLineInterval();
	m_ThresholdMin = param.getContourLineThresholdMin();
	m_ThresholdMax = param.getContourLineThresholdMax();

	if ( interval == 0.0 ) {
		return;
	}

	int i = 0;
	m_Level.clear();
	m_LineColor.clear();
	float minValue, maxValue;
	if ( m_ThresholdMin <= param.getVolumeThresholdMin() ) {
		minValue = param.getVolumeThresholdMin();
	} else {
		minValue = m_ThresholdMin;
	}
	if ( m_ThresholdMax > param.getVolumeThresholdMax() ) {
		maxValue = param.getVolumeThresholdMax();
	} else {
		maxValue = m_ThresholdMax;
	}
	while (1) {
		float level = m_ThresholdMin + interval * static_cast< float >( i );
		if ( level < minValue ) {
			i++;
		} else if ( level <= maxValue + interval * 0.1 ) {
			m_Level.push_back( level );
			int colorID = static_cast< int >( ( level - param.getVolumeThresholdMin() ) /
				(  param.getVolumeThresholdMax() - param.getVolumeThresholdMin() ) * 254.0 );
			int r = static_cast< int >( param.getColorTableR( colorID ) * 255.0 );
			int g = static_cast< int >( param.getColorTableG( colorID ) * 255.0 );
			int b = static_cast< int >( param.getColorTableB( colorID ) * 255.0 );
			m_LineColor.push_back( QColor( r, g, b, 255 ) );
			i++;
		} else {
			break;
		}
	}
}

void ContourLine::draw()
{
	if ( param.getContourLineInterval() == 0.0 ) {
		return;
	}

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );
	glLineWidth( param.getContourLineWidth() );
	if ( param.IsContourLineX() ) {
		drawMain( m_LineX );
	}
	if ( param.IsContourLineY() ) {
		drawMain( m_LineY );
	}
	if ( param.IsContourLineZ() ) {
		drawMain( m_LineZ );
	}
	glEnable( GL_LIGHTING );
	glDisableClientState( GL_VERTEX_ARRAY );
}

void ContourLine::drawSphereData()
{
	if ( param.getContourLineInterval() == 0.0 ) {
		return;
	}

	glEnableClientState( GL_VERTEX_ARRAY );
	glDisable( GL_LIGHTING );
	glLineWidth( param.getContourLineWidth() );
	if ( param.IsContourLineX() ) {
		m_LineXSpherical = cartToSpherical( m_LineX );
		drawMain( m_LineXSpherical );
	}
	if ( param.IsContourLineY() ) {
		m_LineYSpherical = cartToSpherical( m_LineY );
		drawMain( m_LineYSpherical );
	}
	if ( param.IsContourLineZ() ) {
		m_LineZSpherical = cartToSpherical( m_LineZ );
		drawMain( m_LineZSpherical );
	}
	glEnable( GL_LIGHTING );
	glDisableClientState( GL_VERTEX_ARRAY );
}

void ContourLine::recalc()
{
	if ( param.getContourLineInterval() == 0.0 ) {
		return;
	}

	m_Data  = grads.getData();
	m_SizeX = grads.getGridSizeX();
	m_SizeY = grads.getGridSizeY();
	m_SizeZ = grads.getGridSizeZ();
	m_MinX  = grads.getGridXMin();
	m_MaxX  = grads.getGridXMax();
	m_MinY  = grads.getGridYMin();
	m_MaxY  = grads.getGridYMax();
	m_MinZ  = grads.getGridZMin();
	m_MaxZ  = grads.getGridZMax();
	m_Undef = grads.getUndef();
	m_ThresholdMin = param.getContourLineThresholdMin();
	m_ThresholdMax = param.getContourLineThresholdMax();
	m_LineX.clear();
	m_LineY.clear();
	m_LineZ.clear();

	setLevels();

	recalcX();
	recalcY();
	recalcZ();
}

void ContourLine::recalcX()
{
	if ( !param.IsContourLineX() ) {
		return;
	}

	double xpos = m_MinX + ( m_MaxX - m_MinX ) *
		static_cast< double >( param.getContourLinePosX() );
	int xidx = 0;
	if ( m_SizeX != 1 ) {
		for ( int i = 0; i < m_SizeX - 1 ; i++ ) {
			if ( grads.getCoordX( i + 1 ) >= xpos ) {
				xidx = i;
				break;
			}
		}
	}

	float *slicedata = new float[ m_SizeY * m_SizeZ ];
	std::fill( slicedata, slicedata + m_SizeY * m_SizeZ, 0 );

	for ( int i = 0; i < ( m_SizeY * m_SizeZ ); i++ ) {
		ulong idx0 = m_SizeX * i + xidx;
		ulong idx1 = idx0 + 1;
		float ansf;
		if ( m_SizeX == 1 ) {
			ansf = m_Data[ idx0 ];
		} else {
			if ( ( m_Data[ idx0 ] ==  m_Undef ) || ( m_Data[ idx1 ] ==  m_Undef ) ) {
				ansf =  m_Undef;
			} else {
				ansf = m_Data[ idx0 ] + ( m_Data[ idx1 ] - m_Data[ idx0 ] ) *
					( xpos - grads.getCoordX( xidx ) ) /
					( grads.getCoordX( xidx + 1 ) - grads.getCoordX( xidx ) );
			}
			slicedata[ i ] = ansf;
		}
	}

	QVector< QVector< QVector3D > > lineData;
	lineData.clear();

	float  x = -0.5 + param.getContourLinePosX();

	for ( int level = 0; level < m_Level.size(); level++ ) {
		float value = m_Level.at( level );
		m_Vertex.clear();
		for ( int j = 0; j < ( m_SizeZ - 1 ); j++ ) {
			for ( int i = 0; i < ( m_SizeY - 1 ); i++ ) {
				float y0 = -0.5 + ( grads.getCoordY(i) - m_MinY ) / ( m_MaxY - m_MinY );
				float y1 = -0.5 + ( grads.getCoordY(i+1) - m_MinY ) / ( m_MaxY - m_MinY );
				float z0 = -0.5 + ( grads.getCoordZ(j) - m_MinZ ) / ( m_MaxZ - m_MinZ );
				float z1 = -0.5 + ( grads.getCoordZ(j+1) - m_MinZ ) / ( m_MaxZ - m_MinZ );
				float v0 = slicedata[ m_SizeY *   j       + i     ];
				float v1 = slicedata[ m_SizeY * ( j + 1 ) + i     ];
				float v2 = slicedata[ m_SizeY * ( j + 1 ) + i + 1 ];
				float v3 = slicedata[ m_SizeY *   j       + i + 1 ];
				QVector4D p0 = QVector4D( x, y0, z0, v0 );
				QVector4D p1 = QVector4D( x, y0, z1, v1 );
				QVector4D p2 = QVector4D( x, y1, z1, v2 );
				QVector4D p3 = QVector4D( x, y1, z0, v3 );

				makeLine( p0, p1, p2, value );
				makeLine( p0, p3, p2, value );
			}
		}
		lineData.push_back( m_Vertex );
	}

	delete [] slicedata;

	m_LineX = reduction( lineData );
}

void ContourLine::recalcY()
{
	if ( !param.IsContourLineY() ) {
		return;
	}

	double ypos = m_MinY + ( m_MaxY - m_MinY ) *
		static_cast< double >( param.getContourLinePosY() );
	int yidx = 0;
	if ( m_SizeY != 1 ) {
		for ( int i = 0; i < m_SizeY - 1 ; i++ ) {
			if ( grads.getCoordY( i + 1 ) >= ypos ) {
				yidx = i;
				break;
			}
		}
	}

	float *slicedata = new float[ m_SizeX * m_SizeZ ];
	std::fill( slicedata, slicedata + m_SizeX * m_SizeZ, 0 );

	for ( int j = 0; j < m_SizeZ; j++ ) {
		for ( int i = 0; i < m_SizeX; i++ ) {
			ulong idx0 = m_SizeX * m_SizeY * j + i + m_SizeX * yidx;
			ulong idx1 = m_SizeX * m_SizeY * j + i + m_SizeX * ( yidx + 1 );
			float ansf;
			if ( m_SizeY == 1 ) {
				ansf = m_Data[ idx0 ];
			} else {
				if ( ( m_Data[ idx0 ] ==  m_Undef ) || ( m_Data[ idx1 ] ==  m_Undef ) ) {
					ansf =  m_Undef;
				} else {
					ansf = m_Data[ idx0 ] + ( m_Data[ idx1 ] - m_Data[ idx0 ] ) *
						( ypos - grads.getCoordY( yidx ) ) /
						( grads.getCoordY( yidx + 1 ) - grads.getCoordY( yidx ) );
				}
				slicedata[ m_SizeX * j + i ] = ansf;
			}
		}
	}

	QVector< QVector< QVector3D > > lineData;
	lineData.clear();

	float  y = -0.5 + param.getContourLinePosY();

	for ( int level = 0; level < m_Level.size(); level++ ) {
		float value = m_Level.at( level );
		m_Vertex.clear();
		for ( int j = 0; j < ( m_SizeZ - 1 ); j++ ) {
			for ( int i = 0; i < ( m_SizeX - 1 ); i++ ) {
				float x0 = -0.5 + ( grads.getCoordX(i) - m_MinX ) / ( m_MaxX - m_MinX );
				float x1 = -0.5 + ( grads.getCoordX(i+1) - m_MinX ) / ( m_MaxX - m_MinX );
				float z0 = -0.5 + ( grads.getCoordZ(j) - m_MinZ ) / ( m_MaxZ - m_MinZ );
				float z1 = -0.5 + ( grads.getCoordZ(j+1) - m_MinZ ) / ( m_MaxZ - m_MinZ );
				float v0 = slicedata[ m_SizeX *   j       + i     ];
				float v1 = slicedata[ m_SizeX * ( j + 1 ) + i     ];
				float v2 = slicedata[ m_SizeX * ( j + 1 ) + i + 1 ];
				float v3 = slicedata[ m_SizeX *   j       + i + 1 ];
				QVector4D p0 = QVector4D( x0, y, z0, v0 );
				QVector4D p1 = QVector4D( x0, y, z1, v1 );
				QVector4D p2 = QVector4D( x1, y, z1, v2 );
				QVector4D p3 = QVector4D( x1, y, z0, v3 );

				makeLine( p0, p1, p2, value );
				makeLine( p0, p3, p2, value );
			}
		}
		lineData.push_back( m_Vertex );
	}

	delete [] slicedata;

	m_LineY = reduction( lineData );
}

void ContourLine::recalcZ()
{
	if ( !param.IsContourLineZ() ) {
		return;
	}

	double zpos = m_MinZ + ( m_MaxZ - m_MinZ ) *
		static_cast< double >( param.getContourLinePosZ() );
	int zidx = 0;
	if ( m_SizeZ != 1 ) {
		for ( int i = 0; i < m_SizeZ - 1 ; i++ ) {
			if ( grads.getCoordZ( i + 1 ) >= zpos ) {
				zidx = i;
				break;
			}
		}
	}

	float *slicedata = new float[ m_SizeX * m_SizeY ];
	std::fill( slicedata, slicedata + m_SizeX * m_SizeY, 0 );

	for ( int j = 0; j < m_SizeY; j++ ) {
		for ( int i = 0; i < m_SizeX; i++ ) {
			ulong idx0 = m_SizeX * m_SizeY * zidx + m_SizeX * j + i;
			ulong idx1 = m_SizeX * m_SizeY * ( zidx + 1 ) + m_SizeX * j + i;
			float ansf;
			if ( m_SizeZ == 1 ) {
				ansf = m_Data[ idx0 ];
			} else {
				if ( ( m_Data[ idx0 ] ==  m_Undef ) ||
					 ( m_Data[ idx1 ] ==  m_Undef ) ) {
					ansf =  m_Undef;
				} else {
					ansf = m_Data[ idx0 ] + ( m_Data[ idx1 ] - m_Data[ idx0 ] ) *
						( zpos - grads.getCoordZ( zidx ) ) /
						( grads.getCoordZ( zidx + 1 ) - grads.getCoordZ( zidx ) );
				}
			}
			slicedata[ m_SizeX * j + i ] = ansf;
		}
	}

	QVector< QVector< QVector3D > > lineData;
	lineData.clear();

	float  z = -0.5 + param.getContourLinePosZ();

	for ( int level = 0; level < m_Level.size(); level++ ) {
		float value = m_Level.at( level );
		m_Vertex.clear();
		for ( int j = 0; j < ( m_SizeY - 1 ); j++ ) {
			for ( int i = 0; i < ( m_SizeX - 1 ); i++ ) {
				float x0 = -0.5 + ( grads.getCoordX( i ) - m_MinX ) / ( m_MaxX - m_MinX );
				float x1 = -0.5 + ( grads.getCoordX( i + 1 ) - m_MinX ) / ( m_MaxX - m_MinX );
				float y0 = -0.5 + ( grads.getCoordY( j ) - m_MinY ) / ( m_MaxY - m_MinY );
				float y1 = -0.5 + ( grads.getCoordY( j + 1 ) - m_MinY ) / ( m_MaxY - m_MinY );
				float v0 = slicedata[ m_SizeX *   j       + i     ];
				float v1 = slicedata[ m_SizeX * ( j + 1 ) + i     ];
				float v2 = slicedata[ m_SizeX * ( j + 1 ) + i + 1 ];
				float v3 = slicedata[ m_SizeX *   j       + i + 1 ];
				QVector4D p0 = QVector4D( x0, y0, z, v0 );
				QVector4D p1 = QVector4D( x0, y1, z, v1 );
				QVector4D p2 = QVector4D( x1, y1, z, v2 );
				QVector4D p3 = QVector4D( x1, y0, z, v3 );

				makeLine( p0, p1, p2, value );
				makeLine( p0, p3, p2, value );
			}
		}
		lineData.push_back( m_Vertex );
	}

	delete [] slicedata;

	m_LineZ = reduction( lineData );
}

void ContourLine::makeLine(
	QVector4D p0, QVector4D p1, QVector4D p2, float value )
{
	uchar pattern = 0;

	if ( ( p0.w() ==  m_Undef ) ||
		 ( p1.w() ==  m_Undef ) ||
		 ( p2.w() ==  m_Undef ) ) {
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
		m_Vertex.push_back( ip0 );
		m_Vertex.push_back( ip1 );
	}
}

QVector3D ContourLine::crossPoint( QVector4D p0, QVector4D p1, float value )
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

QVector< QVector< QVector< QVector3D > > >
ContourLine::cartToSpherical( QVector< QVector< QVector<QVector3D> > >& lineData )
{
	QVector< QVector< QVector< QVector3D > > > sphericalData;
	QVector< QVector< QVector3D > > lineGroup;
	QVector< QVector3D > line;
	sphericalData.clear();

	for ( int l = 0; l < lineData.size(); l++ ) {
		lineGroup.clear();
		for ( int n = 0; n < lineData[l].size(); n++ ) {
			line.clear();
			for ( int i = 0; i < lineData[l][n].size(); i++ ) {
				QVector< QVector3D > vec3d = lineData[l].at(n);
				float nx = vec3d[i].x() + 0.5;
				float ny = vec3d[i].y() + 0.5;
				float nz = vec3d[i].z() + 0.5;
				float rx = m_MinX + ( m_MaxX - m_MinX ) * nx;
				float ry = m_MinY + ( m_MaxY - m_MinY ) * ny;
				float rz = static_cast< float >( ( EARTH_OBJECT_RADIUS
					* ( m_MinZ + ( m_MaxZ - m_MinZ ) * nz )
					/ EARTH_RADIUS_WE_WGS84 ) * param.getZScale() );
				if ( param.getZUnit() == ZUNIT_KILOMETER ) {
					rz *= 1000.0;
				}
				rz += EARTH_OBJECT_RADIUS;
				rx = rx * M_PI / 180.0;
				ry = ( 90.0 - ry ) * M_PI / 180.0;
				QVector3D newVec3D = QVector3D( rz * sinf( ry ) * cosf( rx ),
										rz * sinf( ry ) * sinf( rx ),
										rz * cosf( ry ) );
				line.push_back( newVec3D );
			}
			lineGroup.push_back( line );
		}
		sphericalData.push_back( lineGroup );
	}

	return sphericalData;
}

void ContourLine::drawMain( QVector< QVector< QVector<QVector3D> > >& lineData )
{
	for ( int l = 0; l < lineData.size(); l++ ) {
		glColor3f( m_LineColor.at(l).redF(),
				   m_LineColor.at(l).greenF(),
				   m_LineColor.at(l).blueF() );
		for ( int n = 0; n < lineData[l].size(); n++ ) {
			if ( lineData[l].at(n).first() == lineData[l][n].last() ) {
				glVertexPointer( 3, GL_FLOAT, 0, lineData[l][n].constData() );
				glDrawArrays( GL_LINE_LOOP, 0, lineData[l][n].size() - 1 );
			} else {
				glVertexPointer( 3, GL_FLOAT, 0, lineData[l][n].constData() );
				glDrawArrays( GL_LINE_STRIP, 0, lineData[l][n].size() );
			}
		}
	}
}

QVector< QVector< QVector< QVector3D > > >
ContourLine::reduction( QVector< QVector<QVector3D> >& lineData )
{
	QVector< QVector< QVector< QVector3D > > > reductedData;
	reductedData.clear();

	for ( int i = 0; i < m_Level.size(); i++ ) {
		QVector< QVector<QVector3D> > lineGroup;
		QVector<QVector3D> lineParts;
		lineGroup.clear();
		while ( 1 ) {
			if ( lineData[i].isEmpty() ) {
				break;
			}
			lineParts.clear();
			lineParts.push_back( lineData[i].at( 0 ) );
			lineParts.push_back( lineData[i].at( 1 ) );
			lineData[i].remove( 0, 2 );
			while ( 1 ) {
				int idx = lineData[i].indexOf( lineParts.last() );
				if ( idx == -1 ) {
					break;
				} else {
					if ( ( idx % 2 ) == 0 ) {
						lineParts.push_back( lineData[i].at( idx + 1 ) );
						lineData[i].remove( idx, 2 );
					} else {
						lineParts.push_back( lineData[i].at( idx - 1 ) );
						lineData[i].remove( idx - 1, 2 );
					}
				}
			}
			while ( 1 ) {
				int idx = lineData[i].indexOf( lineParts.first() );
				if ( idx == -1 ) {
					break;
				} else {
					if ( ( idx % 2 ) == 0 ) {
						lineParts.push_front( lineData[i].at( idx + 1 ) );
						lineData[i].remove( idx, 2 );
					} else {
						lineParts.push_front( lineData[i].at( idx - 1 ) );
						lineData[i].remove( idx - 1, 2 );
					}
				}
			}
			lineGroup.push_back( lineParts );
		}
		reductedData.push_back( lineGroup );
	}

	return reductedData;
}
