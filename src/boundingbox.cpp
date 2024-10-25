//
// Volume Data Visualizer for Google Earth
//
// boundingbox.cpp
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

#include "boundingbox.h"

BoundingBox::BoundingBox( QGLWidget *parent )
{
	m_QGLWidget = parent;
}

void BoundingBox::draw( FaceType facetype )
{
	if ( param.IsBoundingBox() == true ) {

		if ( ( param.getBGColor().red() == 255 ) &&
			 ( param.getBGColor().green() == 255 ) &&
			 ( param.getBGColor().blue() == 255 ) ) {
			glColor3f( 0.0, 0.0, 0.0 );
		} else {
			glColor3f( 1.0, 1.0, 1.0 );
		}
		glLineWidth( 1.0 );
		glEnable( GL_DEPTH_TEST );
		glDisable( GL_LIGHTING );
		if ( facetype == FRONT_SIDE ) {
			glCullFace( GL_BACK );
		} else {
			glCullFace( GL_FRONT );
		}
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glEnable( GL_CULL_FACE );
		glBegin( GL_QUADS );
		for ( int j = 0; j < 6; ++j ) {
			glNormal3dv( bbox_norm[j] );
			for ( int i = 0; i < 4; ++i ) {
				glVertex3dv( bbox_vert[j][i] );
			}
		}
		glEnd();
		glDisable( GL_CULL_FACE );
		glEnable( GL_LIGHTING );
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

		// Line and ScaleMarkings
//		drawAxisLabelAndLine();

		if ( param.IsBoundingBoxAxisLabel() == true ) {
			if ( param.getViewDirection() == VIEW_DEFAULT ) {
				if ( facetype == FRONT_SIDE ) {
					glDisable( GL_LIGHTING );
					glColor3f( 1.0f, 0.0, 0.0 );
					drawText( -0.54f, -0.54f, -0.54f, grads.getGridXMin() );
					drawText(  0.54f, -0.54f, -0.54f, grads.getGridXMax() );
					glColor3f( 0.0, 1.0f, 0.0 );
					drawText(  0.54f, -0.54f,  0.54f, grads.getGridYMin() );
					drawText(  0.54f,  0.54f,  0.54f, grads.getGridYMax() );
					glColor3f( 0.0, 0.0, 1.0f );
					if ( param.getZUnit() == ZUNIT_METER ) {
						drawText( -0.54f,  0.54f, -0.54f, grads.getGridZMin() / 1000.0 );
						drawText( -0.54f,  0.54f,  0.54f, grads.getGridZMax() / 1000.0 );
					} else {
						drawText( -0.54f,  0.54f, -0.54f, grads.getGridZMin() );
						drawText( -0.54f,  0.54f,  0.54f, grads.getGridZMax() );
					}
					glEnable( GL_LIGHTING );
				}
			}
		}
	}
}

void BoundingBox::drawAxisLabelAndLine()
{
	if ( param.getProjection() == PERSPECTIVE ) {
		return;
	}

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_BLEND );
	glDisable( GL_LIGHTING );

	ViewDirection view = param.getViewDirection();
	if ( view == VIEW_FRONT ) {
		drawAxisLabelAndLineParts( AXIS_LON, VIEW_FRONT );
		drawAxisLabelAndLineParts( AXIS_LAT, VIEW_FRONT );
	} else if ( view == VIEW_LEFT ) {
		drawAxisLabelAndLineParts( AXIS_LAT, VIEW_LEFT );
		drawAxisLabelAndLineParts( AXIS_HEIGHT, VIEW_LEFT );
	} else if ( view == VIEW_BOTTOM ) {
		drawAxisLabelAndLineParts( AXIS_LON, VIEW_BOTTOM );
		drawAxisLabelAndLineParts( AXIS_HEIGHT, VIEW_BOTTOM );
	}

	glEnable( GL_LIGHTING );
	glDisable( GL_BLEND );
	glDisable( GL_LINE_SMOOTH );
}

void BoundingBox::drawAxisLabelAndLineParts( AxisType type, ViewDirection view )
{
	QFont font;
	font.setFamily( "Arial" );
	font.setPixelSize( 18 );
//	font.setBold( true );
//	font.setItalic( true );
	font.setStyleStrategy( QFont::PreferAntialias );
	QFontMetricsF fontMetrics( font );

	double modelview[16], projection[16];
	int viewport[4];
	glGetDoublev( GL_MODELVIEW_MATRIX,  modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	// create coordinate information
	double min, max;
	double base = 0.0;
	double interval = 10.0;
	int    precision = 0;
//	double base = param.boundingBox().base();
//	double interval = param.boundingBox().interval();
//	int    precision =  param.boundingBox().precision();
	if ( type == AXIS_LON ) {
		min = grads.getGridXMin();
		max = grads.getGridXMax();
		base -= 360.0; // for safety execution
	} else if ( type == AXIS_LAT ) {
		min = grads.getGridYMin();
		max = grads.getGridYMax();
		base = -90.0;
	} else {
		min = grads.getGridZMin();
		max = grads.getGridZMax();
		base = min;
		interval = ( max - min ) / 5.0;
	}
	QVector< QPair< double, double > > coord;
	int i = 0;
	while ( 1 ) {
		double value = base + interval * i;
		if ( value > max ) {
			break;
		} else if ( value >= min ) {
			QPair< double, double > pair;
			pair.first  = value;
			pair.second = ( value - min ) / ( max - min ) - 0.5;
			coord.push_back( pair );
		}
		i++;
	}

	for ( int i = 0; i < coord.size(); i++ ) {

		double winX, winY, winZ;
		double objX0, objY0, objZ0, objX1, objY1, objZ1;

		// set object position to render text
		if ( type == AXIS_LON ) {
			objX0 = coord[ i ].second;
			objY0 = -0.5;
			if ( view == VIEW_FRONT ) {
				objZ0 =  0.5;
			} else {
				objZ0 = -0.5;
			}
		} else if ( type == AXIS_LAT ) {
			objX0 = -0.5;
			objY0 = coord[ i ].second;
			if ( view == VIEW_FRONT ) {
				objZ0 =  0.5;
			} else {
				objZ0 = -0.5;
			}
		} else {
			objX0 = -0.5;
			objY0 =  0.5;
			objZ0 = coord[ i ].second;
		}

		// get window position to render text
		gluProject( objX0, objY0, objZ0,
			modelview, projection, viewport, &winX, &winY, &winZ );

		// draw tick mark
		double TICKMARK_LENGTH = 8.0;
		if ( type == AXIS_LON ) {
			winY -= TICKMARK_LENGTH;
		} else if ( type == AXIS_LAT ) {
			if ( view == VIEW_FRONT ) {
				winX -= TICKMARK_LENGTH;
			} else {
				winY -= TICKMARK_LENGTH;
			}
		} else {
			winX -= TICKMARK_LENGTH;
		}
		gluUnProject( winX, winY, winZ,
			modelview, projection, viewport, &objX1, &objY1, &objZ1 );
		glLineWidth( 2.0 );
		glBegin( GL_LINES );
		glVertex3d( objX0, objY0, objZ0 );
		glVertex3d( objX1, objY1, objZ1 );
		glEnd();
		glLineWidth( 1.0 );

		// draw line
		glEnable( GL_LINE_STIPPLE );
		glLineStipple( 1 , 0xF0F0 );
		glLineWidth( 2.0 );
		glBegin( GL_LINES );
		if ( type == AXIS_LON ) {
			if ( view == VIEW_FRONT ) {
				glVertex3f( coord[ i ].second, -0.5,  0.5 );
				glVertex3f( coord[ i ].second,  0.5,  0.5 );
			} else {
				glVertex3f( coord[ i ].second, -0.5, -0.5 );
				glVertex3f( coord[ i ].second, -0.5,  0.5 );
			}
		} else if ( type == AXIS_LAT ) {
			if ( view == VIEW_FRONT ) {
				glVertex3f( -0.5, coord[ i ].second,  0.5 );
				glVertex3f(  0.5, coord[ i ].second,  0.5 );
			} else {
				glVertex3f( -0.5, coord[ i ].second, -0.5 );
				glVertex3f( -0.5, coord[ i ].second,  0.5 );
			}
		} else {
			if ( view == VIEW_BOTTOM ) {
				glVertex3f( -0.5, -0.5, coord[ i ].second );
				glVertex3f(  0.5, -0.5, coord[ i ].second );
			} else {
				glVertex3f( -0.5, -0.5, coord[ i ].second );
				glVertex3f( -0.5,  0.5, coord[ i ].second );
			}
		}
		glEnd();
		glLineWidth( 1.0 );
		glDisable( GL_LINE_STIPPLE );

		// render text
		double label = coord[ i ].first;
		QString str;
		if ( type == AXIS_LON ) {
			if ( label > 180.0 ) {
				label -= 360.0;
			} else if ( label < -180.0 ) {
				label += 180.0;
			}
			if ( fmod( label, 180.0 ) == 0.0 ) {
				str = '0';
			} else if ( ( label > 0.0 ) && ( label < 180.0 ) ) {
				str = QString::number(  label, 'f', precision ) + 'E';
			} else {
				str = QString::number( -label, 'f', precision ) + 'W';
			}
		} else if ( type == AXIS_LAT ) {
			if ( label == 0.0 ) {
				str = "EQ";
			} else if ( label > 0.0 ) {
				str = QString::number(  label, 'f', precision ) + 'N';
			} else {
				str = QString::number( -label, 'f', precision ) + 'S';
			}
		} else {
			str = QString::number(  label, 'f', precision );
/*
			if ( param.getZUnit() == ZUNIT_KILOMETER ) {
				str += "km";
			} else {
				str += "m";
			}
*/
		}
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
		float strWidth  = fontMetrics.boundingRect(QString(str)).width();
//		float strWidth  = fontMetrics.horizontalAdvance(QString(str));
#else
		float strWidth  = fontMetrics.width( QString( str ) );
#endif
		float strHeight = fontMetrics.ascent();
		int strX, strY;
		if ( type == AXIS_LON ) {
			strX = static_cast< int >( winX - strWidth / 2.0 );
			strY = m_QGLWidget->height()
				- static_cast< int >( winY - strHeight - TEXT_MARGIN );
		} else if ( type == AXIS_LAT ) {
			if ( param.getViewDirection() == VIEW_FRONT ) {
				strX = static_cast< int >( winX - strWidth - TEXT_MARGIN * 2 );
				strY = m_QGLWidget->height()
					- static_cast< int >( winY - strHeight / 2.0 + 3.0 );
			} else {
				strX = static_cast< int >( winX - strWidth / 2.0 );
				strY = m_QGLWidget->height()
					- static_cast< int >( winY - strHeight - TEXT_MARGIN );
			}
		} else {
			strX = static_cast< int >( winX - strWidth - TEXT_MARGIN * 2 );
			strY = m_QGLWidget->height()
				- static_cast< int >( winY - strHeight / 2.0 + 3.0 );
		}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#else
		m_QGLWidget->renderText( strX, strY, str, font );
#endif
	}
}

void BoundingBox::drawText( GLfloat x, GLfloat y, GLfloat z, double num )
{
	QFont font( "Helvetica" );
//	font.setPointSize( 8 );
	font.setPixelSize( 12 );
	font.setBold( false );
	QString numStr = QString::number( num, 'f', 2 );
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	font.setFamily("Courier New");
	font.setHintingPreference(QFont::HintingPreference::PreferNoHinting);
	renderText( x, y, z, numStr, font );
#else
	m_QGLWidget->renderText( x, y, z, numStr, font );
#endif
}

void BoundingBox::drawSphereParts( float height )
{
	float normalized_height = 0.5 * static_cast< float >( height / EARTH_RADIUS_WE_WGS84 );
	if ( param.getZUnit() == ZUNIT_KILOMETER ) {
		normalized_height *= 1000.0;
	}
	float radius = 0.5 + normalized_height * param.getZScale();

	double rangeDegLat = grads.getGridYMax() - grads.getGridYMin();
	double rangeDegLon = grads.getGridXMax() - grads.getGridXMin();
	double rangeRadLat = rangeDegLat * M_PI / 180.0;
	double rangeRadLon = rangeDegLon * M_PI / 180.0;
	int    divLat = ( int )( rangeDegLat / 1.0 ) - 1;
	int    divLon = ( int )( rangeDegLon / 1.0 ) - 1;
	double dRadLat = rangeRadLat / ( double )divLat;
	double dRadLon = rangeRadLon / ( double )divLon;

	QList<QVector3D> vert0, vert1, vert2, vert3;
	glPushMatrix();
	glDisable( GL_LIGHTING );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glLineWidth( 1.0 );
	glColor3f( 1.0, 1.0, 1.0 );
	float radLatitude = ( 90.0 - grads.getGridYMax() ) * M_PI / 180.0;
	for ( int vi = 0; vi < divLat; vi++ )
	{
		const float lat0 = radLatitude;
		const float lat1 = radLatitude + dRadLat;
		float radLongitude = grads.getGridXMin() * M_PI / 180.0;
		for ( int ui = 0; ui < divLon; ui++ )
		{
			const float lon0 = radLongitude;
			const float lon1 = radLongitude + dRadLon;

			float Vertex00[3] = { radius*sinf(lat0)*cosf(lon0), radius*sinf(lat0)*sinf(lon0), radius*cosf(lat0) };
			float Vertex10[3] = { radius*sinf(lat0)*cosf(lon1), radius*sinf(lat0)*sinf(lon1), radius*cosf(lat0) };
			float Vertex11[3] = { radius*sinf(lat1)*cosf(lon1), radius*sinf(lat1)*sinf(lon1), radius*cosf(lat1) };
			float Vertex01[3] = { radius*sinf(lat1)*cosf(lon0), radius*sinf(lat1)*sinf(lon0), radius*cosf(lat1) };
			if ( ui == 0 ) {
				vert0.push_back(  QVector3D( Vertex00[0], Vertex00[1], Vertex00[2] ) );
			}
			if ( ui == divLon - 1 ) {
				vert1.push_front( QVector3D( Vertex11[0], Vertex11[1], Vertex11[2] ) );
			}
			if ( vi == 0 ) {
				vert2.push_front( QVector3D( Vertex10[0], Vertex10[1], Vertex10[2] ) );
			}
			if ( vi == divLat - 1 ) {
				vert3.push_back(  QVector3D( Vertex01[0], Vertex01[1], Vertex01[2] ) );
			}
			radLongitude = lon1;
		}
		radLatitude = lat1;
	}
	vert0.append( vert3 );
	vert0.append( vert1 );
	vert0.append( vert2 );
	glBegin( GL_LINE_LOOP );
	for ( int i = 0; i < vert0.size(); i++ ) {
		glVertex3f( vert0[i].x(), vert0[i].y(), vert0[i].z() );
	}
	glEnd();
	glLineWidth( 1.0 );
	glDisable( GL_BLEND );
	glDisable( GL_LINE_SMOOTH );
	glEnable( GL_LIGHTING );
	glPopMatrix();
}

void BoundingBox::drawSphereBoundingBox()
{
	if ( param.IsBoundingBox() == true ) {
		drawSphereParts( grads.getGridZMin() );
		drawSphereParts( grads.getGridZMax() );
	}
}

void BoundingBox::renderTextViaQImage
( int x, int y, QString str, QFont font, float deg )
{
	// create QImage to draw to GLContext
	QFontMetrics fontMetrics( font );
	QRect rect = fontMetrics.boundingRect( str );
	QImage image( rect.width(), rect.height(), QImage::Format_ARGB32 );
	image.fill( 0 );

	// setup QPen and bind QPainter to QImage
//	QPen pen( m_Color );
	QPen pen( Qt::white );
	QPainter painter( &image );
	painter.setPen( pen );
	painter.setFont( font );

	// draw text string to QImage via QPainter
	painter.drawText( QRect( 0, 0, image.width(), image.height() ),
		Qt::AlignLeft, str, &rect );
	painter.end();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	QImage glImage = image.mirrored(false, true).convertToFormat(QImage::Format_ARGB32);
#else
	QImage glImage = QGLWidget::convertToGLFormat( image );
#endif

	// transform image
	QTransform transform;
	transform.rotate( deg );
	QImage transformedImage =
		glImage.transformed( transform );

	// draw QImage to GLContext
	glPixelTransferi( GL_MAP_COLOR, GL_FALSE );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glRasterPos2i( x, y );
	glEnable( GL_BLEND );
	glDrawPixels( transformedImage.width(), transformedImage.height(),
		GL_RGBA, GL_UNSIGNED_BYTE, transformedImage.bits() );
	glDisable( GL_BLEND );
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void BoundingBox::renderText(double x, double y, double z, QString str, QFont font)
{
	double modelview[16], projection[16];
	int viewport[4];
	glGetDoublev( GL_MODELVIEW_MATRIX,  modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	// get window position to render text
	double winX, winY, winZ;
	gluProject( x, y, z,
		modelview, projection, viewport, &winX, &winY, &winZ );

//	winY = m_QGLWidget->height() - winY; // y is inverted

	QFontMetrics fontMetrics( font );
	QRect rect = fontMetrics.boundingRect( str );
	QImage image( rect.width(), rect.height(), QImage::Format_ARGB32 );
	image.fill( 0 );

	float currentColor[4];
	glGetFloatv( GL_CURRENT_COLOR, currentColor );
	QColor color;
	color.setRgbF(currentColor[2], currentColor[1], currentColor[0], currentColor[3]); // BGRA
	QPen pen( color );
//	QPen pen( Qt::white );
	QPainter painter( &image );
	painter.setPen( pen );
	painter.setFont( font );

	// draw text string to QImage via QPainter
	painter.drawText( QRect( 0, 0, image.width(), image.height() ),
					 Qt::AlignLeft, str, &rect );
	painter.end();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	QImage glImage = image.mirrored(false, true).convertToFormat(QImage::Format_ARGB32);
#else
	QImage glImage = QGLWidget::convertToGLFormat( image );
#endif

	float m_VPWidth   = static_cast< float >( m_QGLWidget->width() );
	float m_VPHeight  = static_cast< float >( m_QGLWidget->height() );

	glPushMatrix();
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0, static_cast< int >( m_VPWidth )  - 1,
			   0, static_cast< int >( m_VPHeight ) - 1 );
	glPushAttrib( GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );

   // draw QImage to GLContext
	glPixelTransferi( GL_MAP_COLOR, GL_FALSE );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glRasterPos2i( winX, winY );
//	qDebug() << "(winX, winY) = (" << winX << ", " << winY << ")" << ENDL;
	glEnable( GL_BLEND );
	glDrawPixels( glImage.width(), glImage.height(),
				 GL_RGBA, GL_UNSIGNED_BYTE, glImage.bits() );
	glDisable( GL_BLEND );

	glEnable( GL_LIGHTING );
	glEnable( GL_DEPTH_TEST );
	glPopAttrib();
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

/*
	int height = m_QGLWidget->height();

	GLdouble model[4][4], proj[4][4];
	GLint view[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, &model[0][0]);
	glGetDoublev(GL_PROJECTION_MATRIX, &proj[0][0]);
	glGetIntegerv(GL_VIEWPORT, &view[0]);
	GLdouble textPosX = 0, textPosY = 0, textPosZ = 0;

	project(x, y, z,
			&model[0][0], &proj[0][0], &view[0],
			&textPosX, &textPosY, &textPosZ);

	textPosY = height - textPosY; // y is inverted

	// Retrieve last OpenGL color to use as a font color
	GLdouble glColor[4];
	glGetDoublev(GL_CURRENT_COLOR, glColor);
	QColor fontColor = QColor(glColor[0], glColor[1], glColor[2], glColor[3]);

	// Render text
//	painter.setPen(fontColor);
//	painter.setFont(font);
//	painter.drawText(textPosX, textPosY, str);
//	painter.end();

	// create QImage to draw to GLContext
	font = QFont("Helvetica", 24);
	QFontMetrics fontMetrics( font );
	QRect rect = fontMetrics.boundingRect( str );
	QImage image( rect.width(), rect.height(), QImage::Format_ARGB32 );
	image.fill( 0 );

	str = "TEST";
//	QPainter painter(m_QGLWidget);
	QPainter painter(&image);
	painter.setPen(Qt::yellow);
//	painter.setFont(QFont("Helvetica", 8));
//	painter.setFont(QFont("Courier New", 24));
	painter.setFont(font);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
//	painter.drawText(textPosX, textPosY, str); // z = pointT4.z + distOverOp / 4
	// draw text string to QImage via QPainter
	painter.drawText( QRect( textPosX, textPosY, image.width(), image.height() ),
					 Qt::AlignLeft, str, &rect );
	painter.end();
*/
}

inline GLint BoundingBox::project(
	GLdouble objx, GLdouble objy, GLdouble objz,
	const GLdouble model[16], const GLdouble proj[16],
	const GLint viewport[4],
	GLdouble * winx, GLdouble * winy, GLdouble * winz)
{
	GLdouble in[4], out[4];

	in[0] = objx;
	in[1] = objy;
	in[2] = objz;
	in[3] = 1.0;
	transformPoint(out, model, in);
	transformPoint(in, proj, out);

	if (in[3] == 0.0)
		return GL_FALSE;

	in[0] /= in[3];
	in[1] /= in[3];
	in[2] /= in[3];

	*winx = viewport[0] + (1 + in[0]) * viewport[2] / 2;
	*winy = viewport[1] + (1 + in[1]) * viewport[3] / 2;

	*winz = (1 + in[2]) / 2;
	return GL_TRUE;
}

inline void BoundingBox::transformPoint(GLdouble out[4], const GLdouble m[16], const GLdouble in[4])
{
#define M(row,col)  m[col*4+row]
	out[0] =
		M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
	out[1] =
		M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
	out[2] =
		M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
	out[3] =
		M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
#undef M
}
#endif
