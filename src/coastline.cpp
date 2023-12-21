//
// Volume Data Visualizer for Google Earth
//
// coastline.cpp
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

#include "coastline.h"

Coastline::Coastline( QGLWidget *parent )
{
	m_QGLWidget = parent;

	m_DisplayList = glGenLists(1);
}

Coastline::~Coastline()
{
}

ErrorCode Coastline::setData()
{
	QFile coastlineFile( param.getCoastline() );
	if ( !coastlineFile.exists() ) {
		return COASTLINE_ERROR_FILE_OPEN;
	}
	coastlineFile.open( QIODevice::ReadOnly | QIODevice::Text );
	QTextStream inputStream( &coastlineFile );
	QString lineString;
	QStringList stringList;
	float lat, lon;
	bool  ok1, ok2;

	QVector< QVector< QVector3D > > lineData;
	QVector< QVector2D > lineSegment;
	lineData.clear();
	lineSegment.clear();
	bool isLoop = false; // true: segment across 180.0 degrees

	double west  = grads.getGridXMin();
	double east  = grads.getGridXMax();
	double south = grads.getGridYMin();
	double north = grads.getGridYMax();
	if ( east > 360.0 ) {
		west -= 360.0;
		east -= 360.0;
	}

	qint64 segments = 0;
	while ( !inputStream.atEnd() ) {
		lineString = inputStream.readLine();
		if ( lineString[0] != QChar( '>' ) ) {
			stringList = splitString( "\\s+", lineString );
			if ( stringList.size() >= 2 ) {
				lon = ( stringList.at( 0 ) ).toFloat( &ok1 );
				lat = ( stringList.at( 1 ) ).toFloat( &ok2 );
				if ( ok1 && ok2 ) {
					if ( !isLoop && ( lon < 0.0 ) ) {
						isLoop = true;
					}
					lineSegment.push_back( QVector2D( lon, lat ) );
				} else {
					return COASTLINE_ERROR_SYNTAX;
				}
			}
		} else {
			if ( lineSegment.size() != 0 ) {
				if ( isLoop ) {
					int size = lineSegment.size();
					for ( int i = 0; i < size; i++ ) {
						lineSegment.push_back(
							QVector2D( lineSegment[i].x() + 360.0, lineSegment[i].y() ) );
					}
				}
				bool isValid = false;
				for ( int i = 0; i < lineSegment.size(); i++ ) {
					if ( ( lineSegment[i].x() >= west ) &&
						 ( lineSegment[i].x() <= east ) &&
						 ( lineSegment[i].y() >= south ) &&
						 ( lineSegment[i].y() <= north ) ) {
						isValid = true;
						break;
					}
				}
				if ( isValid ) {
					QVector< QVector3D > lineSegmentNormalized;
					int size = 0;
					if ( isLoop ) {
						size = lineSegment.size() / 2;
					} else {
						size = lineSegment.size();
					}
					for ( int i = 0; i < size; i++ ) {
						float normX = ( lineSegment[i].x() - west ) / ( east - west );
						float normY = ( lineSegment[i].y() - south ) / ( north - south );
						lineSegmentNormalized.push_back( QVector3D( normX, normY, 0.0 ) );
					}
					lineData.push_back( lineSegmentNormalized );
					lineSegmentNormalized.clear();
					if ( isLoop ) {
						for ( int i = size; i < lineSegment.size(); i++ ) {
							float normX = ( lineSegment[i].x() - west ) / ( east - west );
							float normY = ( lineSegment[i].y() - south ) / ( north - south );
							lineSegmentNormalized.push_back( QVector3D( normX, normY, 0.0 ) );
						}
						lineData.push_back( lineSegmentNormalized );
						lineSegmentNormalized.clear();
					}
				}
			}
			lineSegment.clear();
			isLoop = false;
			segments++;
		}
	}

	glNewList( m_DisplayList, GL_COMPILE );
	for ( int i = 0; i < lineData.size(); i++ ) {
		if ( lineData[i].first() == lineData[i].last() ) {
			glBegin( GL_LINE_LOOP );
		} else {
			glBegin( GL_LINE_STRIP );
		}
		for ( int j = 0; j < lineData[i].size(); j++ ) {
			glVertex3f( lineData[i][j].x(), lineData[i][j].y(), lineData[i][j].z() );
		}
		glEnd();
	}
	glEndList();

	lineData.clear();

	coastlineFile.close();

//	param.setStateCoastline( true );
	return COASTLINE_NO_ERROR;
}

void Coastline::draw()
{
	if ( !param.IsCoastline() ) {
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
	glDisable( GL_LIGHTING );
	if ( ( param.getBGColor().red()   == 255 ) &&
		 ( param.getBGColor().green() == 255 ) &&
		 ( param.getBGColor().blue()  == 255 ) ) {
        glColor3f(  0.0,  0.0,  0.0 );
	} else {
        glColor3f( 1.0f, 1.0f, 1.0f );
	}
    glLineWidth( 1.2f );
	glCallList( m_DisplayList );
	glEnable( GL_LIGHTING );
	glLineWidth( 1 );
	glDisable( GL_CLIP_PLANE0 );
	glDisable( GL_CLIP_PLANE1 );
	glDisable( GL_CLIP_PLANE2 );
	glDisable( GL_CLIP_PLANE3 );
	glPopMatrix();
}
