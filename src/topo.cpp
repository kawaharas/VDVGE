//
// Volume Data Visualizer for Google Earth
//
// topo.cpp
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

#include "topo.h"

Topo::Topo( QWidget *parent ) :
	QGLWidget( parent )
{
}

Topo::~Topo()
{
}

void Topo::initialize()
{
	m_North = grads.getGridYMax();
	m_South = grads.getGridYMin();
	m_West  = grads.getGridXMin();
	m_East  = grads.getGridXMax();

	if ( m_West < 0.0 ) {
		m_West += 360.0;
		m_East += 360.0;
	}
}

void Topo::setData()
{
	QString appPath = QCoreApplication::applicationDirPath();
	QString topoFileName;
	TopoType type = param.getTopoType();
	if ( type == TOPO_ETOPO5 ) {
		topoFileName = appPath + "/ETOPO5.DOS";
		LON = 4320;
		LAT = 2160;
	} else if ( type == TOPO_ETOPO2 ) {
		topoFileName = appPath + "/ETOPO2v2g_i2_LSB.bin";
		LON = 10801;
		LAT =  5401;
	} else if ( type == TOPO_ETOPO1 ) {
		topoFileName = appPath + "/etopo1_ice_g_i2.bin";
		LON = 21601;
		LAT = 10801;
	} else {
		return;
	}

	double dLON = static_cast< double >( LON );
	double dLAT = static_cast< double >( LAT );

	int idx0x = 0;
	int idx1x = 0;
	int idx0y = 0;
	int idx1y = 0;

	if ( type == TOPO_ETOPO5 ) {
		// for ETOPO5 ( cell centered )
		for ( int i = 0; i < LON * 2; i++ ) {
			double di = static_cast< double >( i );
			if ( m_West < 180.0 * ( 2.0 * di + 1.0 ) / dLON ) {
				idx0x = i - 1;
				break;
			}
		}
		for ( int i = 0; i < LON * 2; i++ ) {
			double di = static_cast< double >( i );
			if ( m_East < 180.0 * ( 2.0 * di + 1.0 ) / dLON ) {
				idx1x = i;
				break;
			}
		}
		for ( int i = 0; i < LAT; i++ ) {
			double di = static_cast< double >( i );
			if ( m_South < -90.0 + ( 180.0 * di + 90.0 ) / dLAT ) {
				idx0y = i;
				break;
			}
		}
		idx1y = LAT - 1; // for overflow
		for ( int i = 0; i < LAT; i++ ) {
			double di = static_cast< double >( i );
			if ( m_North < -90.0 + ( 180.0 * di + 90.0 ) / dLAT ) {
				idx1y = i;
				break;
			}
		}
	} else {
		// for ETOPO1/ETOPO2 ( grid centered )
		for ( int i = 0; i < LON * 2; i++ ) {
			double di = static_cast< double >( i );
			if ( m_West < 360.0 / ( dLON - 1.0 ) * di ) {
				idx0x = i - 1;
				break;
			}
		}
		for ( int i = 0; i < LON * 2; i++ ) {
			double di = static_cast< double >( i );
			if ( m_East < 360.0 / ( dLON - 1.0 ) * di ) {
				idx1x = i;
				break;
			}
		}
		for ( int i = 0; i < LAT; i++ ) {
			double di = static_cast< double >( i );
			if ( m_South <
					-90.0 + 180.0 / ( dLAT - 1.0 ) * di ) {
				idx0y = i - 1;
				break;
			}
		}
		for ( int i = 0; i < LAT; i++ ) {
			double di = static_cast< double >( i );
			if ( m_North <=
					-90.0 + 180.0 / ( dLAT - 1.0 ) * di ) {
				idx1y = i;
				break;
			}
		}
	}

	m_LON = idx1x - idx0x + 1;
	m_LAT = idx1y - idx0y + 1;

	QFile topoFile( topoFileName );
	topoFile.open( QIODevice::ReadOnly );
	QDataStream stream( &topoFile );
	stream.setByteOrder( QDataStream::LittleEndian );

	qint16 *m_AreaData = new qint16[ m_LON * m_LAT ]();
	if ( m_AreaData == NULL ) {
		QString msg;
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
		msg.asprintf( "Could not allocate memory to read data." );
#else
		msg.sprintf( "Could not allocate memory to read data." );
#endif
		QMessageBox::critical( 0, "Error", msg );
	}

	int n = 0;
	if ( type == TOPO_ETOPO5 ) {
		// for ETOPO5
		for ( int j = idx0y; j <= idx1y; j++ ) {
			for ( int i = idx0x; i <= idx1x; i++ ) {
				qint64 skipBytes = 0;

				if ( i == -1 ) {
					skipBytes = ( LON * ( LAT - ( j + 1 ) ) + ( LON - 1 ) ) * sizeof( short );
				} else if ( i >= LON ) {
					skipBytes = ( LON * ( LAT - ( j + 1 ) ) + ( i - LON ) ) * sizeof( short );
				} else {
					skipBytes = ( LON * ( LAT - ( j + 1 ) ) + i ) * sizeof( short );
				}

				topoFile.seek( skipBytes );
				stream >> m_AreaData[ n ];
				n++;
			}
		}
	} else {
		// for ETOPO1, ETOPO2
		for ( int j = idx0y; j <= idx1y; j++ ) {
			for ( int i = idx0x; i <= idx1x; i++ ) {
				qint64 skipBytes = 0;
				if ( i >= LON ) {
					qint64 lonPos = i - LON;
					lonPos += ( LON / 2 );
					if ( lonPos > LON ) {
						lonPos -= LON;
					}
					skipBytes = ( LON * ( LAT - ( j + 1 ) ) + lonPos ) * sizeof( short );
				} else {
					qint64 lonPos = i;
					lonPos += ( LON / 2 );
					if ( lonPos > LON ) {
						lonPos -= LON;
					}
					skipBytes = ( LON * ( LAT - ( j + 1 ) ) + lonPos ) * sizeof( short );
				}
				topoFile.seek( skipBytes );
				stream >> m_AreaData[ n ];
				n++;
			}
		}
	}
	topoFile.close();

	// Create Arrays
	//
	// 1 +-----+ 2
	//   |    /|
	//   |   / |
	//   |  /  |
	//   | /   |
	//   |/    |
	// 0 +-----+ 3
	//
	// idx = ( 1, 0, 2 ), ( 2, 0, 3 )

	// Make Coordinate
//	double coordX[ m_LON ];
//	double coordY[ m_LAT ];
	double *coordX = new double[ m_LON ]();
	double *coordY = new double[ m_LAT ]();
	if ( type == TOPO_ETOPO5 ) {
		// for ETOPO5
		for ( int i = 0; i < m_LON; i++ ) {
			double di = static_cast< double >( i + idx0x );
			coordX[ i ] = 180.0 * ( 2.0 * di + 1.0 ) / dLON;
			coordX[ i ] -= grads.getGridXMin();
			coordX[ i ] /= ( grads.getGridXMax() - grads.getGridXMin() );
		}
		for ( int i = 0; i < m_LAT; i++ ) {
			double di = static_cast< double >( i + idx0y );
			coordY[ i ] = -90.0 + ( 180.0 * di + 90.0 ) / dLAT;
			coordY[ i ] -= grads.getGridYMin();
			coordY[ i ] /= ( grads.getGridYMax() - grads.getGridYMin() );
		}
	} else {
		// for ETOPO1, ETOPO2
		for ( int i = 0; i < m_LON; i++ ) {
			double di = static_cast< double >( i + idx0x );
			coordX[ i ] = 360.0 / ( dLON - 1.0 ) * di;
			coordX[ i ] -= grads.getGridXMin();
			coordX[ i ] /= ( grads.getGridXMax() - grads.getGridXMin() );
		}
		for ( int i = 0; i < m_LAT; i++ ) {
			double di = static_cast< double >( i + idx0y );
			coordY[ i ] = -90.0 + 180.0 / ( dLAT - 1.0 ) * di;
			coordY[ i ] -= grads.getGridYMin();
			coordY[ i ] /= ( grads.getGridYMax() - grads.getGridYMin() );
		}
	}

	if ( !m_Vertex.isEmpty() ) {
		m_Vertex.clear();
	}
	if ( !m_Normal.isEmpty() ) {
		m_Normal.clear();
	}
	if ( !m_Color.isEmpty() ) {
		m_Color.clear();
	}
	if ( !m_Index.isEmpty() ) {
		m_Index.clear();
	}

	for ( int j = 0; j < m_LAT; j++ ) {
		for ( int i = 0; i < m_LON; i++ ) {
			float z = static_cast< float >( m_AreaData[ m_LON * j + i ] );
			addElement( coordX[ i ], coordY[ j ], z );
		}
	}

	m_Normal.fill( QVector3D( 0.0, 0.0, 0.0 ), m_Vertex.size() );
	for ( int j = 0; j < ( m_LAT - 1 ); j++ ) {
		for ( int i = 0; i < ( m_LON - 1 ); i++ ) {
			int idx0 = m_LON *   j       +   i;
			int idx1 = m_LON * ( j + 1 ) +   i;
			int idx2 = m_LON * ( j + 1 ) + ( i + 1 );
			int idx3 = m_LON *   j       + ( i + 1 );
			QVector3D norm0 = QVector3D::normal(
				m_Vertex[ idx0 ], m_Vertex[ idx1 ], m_Vertex[ idx2 ] );
			QVector3D norm1 = QVector3D::normal(
				m_Vertex[ idx0 ], m_Vertex[ idx2 ], m_Vertex[ idx3 ] );
			m_Normal[ idx0 ] += norm0;
			m_Normal[ idx1 ] += norm0;
			m_Normal[ idx2 ] += norm0;
			m_Normal[ idx0 ] += norm1;
			m_Normal[ idx2 ] += norm1;
			m_Normal[ idx3 ] += norm1;
		}
	}

	// Create Arrays
	//
	// 1 +-----+ 2
	//   |    /|
	//   |   / |
	//   |  /  |
	//   | /   |
	//   |/    |
	// 0 +-----+ 3
	//
	// idx = ( 1, 0, 2 ), ( 2, 0, 3 )

	for ( int j = 0; j < ( m_LAT - 1 ); j++ ) {
		for ( int i = 0; i < ( m_LON - 1 ); i++ ) {

			uint idx0 = m_LON *   j       +   i;
			uint idx1 = m_LON * ( j + 1 ) +   i;
			uint idx2 = m_LON * ( j + 1 ) + ( i + 1 );
			uint idx3 = m_LON *   j       + ( i + 1 );

			m_Index.push_back( idx1 );
			m_Index.push_back( idx0 );
			m_Index.push_back( idx2 );

			m_Index.push_back( idx2 );
			m_Index.push_back( idx0 );
			m_Index.push_back( idx3 );
		}
	}

	delete [] coordX;
	delete [] coordY;
	delete [] m_AreaData;
}

void Topo::draw()
{
	if ( param.getTopoType() == TOPO_NONE ) {
		return;
	}

	if ( !param.IsTopo() ) {
		return;
	}

	if ( m_Index.size() > 16777215 ) { // Maximum Number of GLsizei
		return;
	}

	const double clipPlane0[]={  1.0,  0.0, 0.0, 0.0 }; // west
	const double clipPlane1[]={  0.0,  1.0, 0.0, 0.0 }; // east
	const double clipPlane2[]={  0.0, -1.0, 0.0, 1.0 }; // north
	const double clipPlane3[]={ -1.0,  0.0, 0.0, 1.0 }; // south

	glPushMatrix();

	glClipPlane( GL_CLIP_PLANE0, clipPlane0 );
	glClipPlane( GL_CLIP_PLANE1, clipPlane1 );
	glClipPlane( GL_CLIP_PLANE2, clipPlane2 );
	glClipPlane( GL_CLIP_PLANE3, clipPlane3 );
	glEnable( GL_CLIP_PLANE0 );
	glEnable( GL_CLIP_PLANE1 );
	glEnable( GL_CLIP_PLANE2 );
	glEnable( GL_CLIP_PLANE3 );

	// Ground and Sea Floor

	glVertexPointer( 3, GL_FLOAT, 0, m_Vertex.constData() );
	glNormalPointer( GL_FLOAT, 0, m_Normal.constData() );
	glColorPointer( 4, GL_FLOAT, 0, m_Color.constData() );

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 18.0 );
	glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_NORMALIZE );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
	glPushMatrix();

	glDrawElements( GL_TRIANGLES, m_Index.size(),
		GL_UNSIGNED_INT, m_Index.constData() );

	glPopMatrix();
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );

	// Sea Surface
	if ( param.IsSeaSurface() ) {
		glDepthFunc( GL_LEQUAL );
		glDisable( GL_LIGHTING );
        glColor4f( 0.0, 0.0, 0.5f, 0.65f );
		glEnable( GL_BLEND );
		glEnable( GL_DEPTH_TEST );
		glBegin( GL_QUADS );
        glVertex3f( 0.0,  0.0,  0.000001f );
        glVertex3f( 1.0f, 0.0,  0.000001f );
        glVertex3f( 1.0f, 1.0f, 0.000001f );
        glVertex3f( 0.0,  1.0f, 0.000001f );
		glEnd();
		glDisable( GL_DEPTH_TEST );
		glDisable( GL_BLEND );
		glDepthFunc( GL_LESS );
	}

	glDisable( GL_CLIP_PLANE0 );
	glDisable( GL_CLIP_PLANE1 );
	glDisable( GL_CLIP_PLANE2 );
	glDisable( GL_CLIP_PLANE3 );

	glPopMatrix();
}

void Topo::addElement( float x, float y, float z )
{
	if ( z > 0.0 ) {
		qreal red = 0.17 + z / 1000.0 * 0.3;
		if ( red > 1.0 ) red = 1.0;
        m_Color.push_back(
            QVector4D( static_cast< float >( red ), 0.5f, 0.17f, 1.0f ) );
		z *= static_cast< float >( param.getGroundScale() );
	} else {
        m_Color.push_back( QVector4D( 0.6f, 0.6f, 0.6f, 1.0f ) );
		if ( param.IsSyncScale() ) {
			z *= static_cast< float >( param.getGroundScale() );
		} else {
			z *= static_cast< float >( param.getSeaFloorScale() );
		}
	}
    z /= ( grads.getGridXMax() - grads.getGridXMin() ) / 360.0 *
		2.0 * M_PI * EARTH_RADIUS_WE_WGS84;
	m_Vertex.push_back( QVector3D( x, y, z ) );
}
