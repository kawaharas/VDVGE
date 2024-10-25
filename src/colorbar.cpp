//
// Volume Data Visualizer for Google Earth
//
// colorbar.cpp
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

/////////////////////////////////////////////////////////////////////////
// ColorBar::renderTextViaQImage()
//
// In Qt4 for Mac, QGLWidget::renderText() and QPainter::drawText()
// has a bug that is the misalienment of the character's vertical position
// when a text string was drawn directly to the frame buffer.
// In order to solve this problem, this program render the text strings
// via QPainter that was binded to QImage.
/////////////////////////////////////////////////////////////////////////

#include "colorbar.h"

ColorBar::ColorBar( QGLWidget *parent )
{
	m_QGLWidget = parent;

	m_Texture = 0;
	m_FrameBuffer = 0;
	m_RenderBuffer = 0;
	m_Texture_Org = 0;
	m_TextureCreated = false;
	m_BitmapWidth = 0;

	m_Margin = 35.0;
	m_ColorBarBodyHeight = 15.0;
	m_MinorTickMarkLength = 3.0;
	m_MajorTickMarkLength = 6.0;
	m_LineSpace = 20.0;
}

void ColorBar::initialize()
{
	m_Description = grads.getVarDescription( 0 );
	if ( param.IsLogScale() ) {
		m_Description.append( " (log scale)" );
	}
}

void ColorBar::draw()
{
	if ( !param.IsColorBar() ) return;

	m_Orientation = param.getColorBarOrientation();
	m_Type = param.getColorBarType();
	m_Offset_X  = 0;
	m_Offset_Y  = 0;
	m_VPWidth   = static_cast< float >( m_QGLWidget->width() );
	m_VPHeight  = static_cast< float >( m_QGLWidget->height() );
	m_OVPWidth  = m_VPWidth;
	m_OVPHeight = m_VPHeight;

	// set text color
	m_Color.setRgb(
		abs( param.getBGColor().red()   - 255 ),
		abs( param.getBGColor().green() - 255 ),
		abs( param.getBGColor().blue()  - 255 ) );

	if ( param.IsLogo() ) {
		if ( param.getColorBarOrientation() == HORIZONTAL ) {
			float logoWidth = m_VPWidth * 0.1;
			m_Offset_X  = logoWidth + m_Margin * 2.5;
			m_VPWidth  -= m_Offset_X;
		} else {
			float logoHeight = m_VPWidth * 0.1 * param.getLogoAspectRatio();
			m_Offset_Y  = logoHeight + m_Margin;
			m_VPHeight -= m_Offset_Y;
		}
	}

	glPushMatrix();
		glLoadIdentity();
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
			glLoadIdentity();
			gluOrtho2D( 0, static_cast< int >( m_OVPWidth )  - 1,
						0, static_cast< int >( m_OVPHeight ) - 1 );
			glPushAttrib( GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT );
			glDisable( GL_DEPTH_TEST );
			glDisable( GL_LIGHTING );

			glPointSize( 1.0 );
			glLineWidth( 1.0 );
			drawColorBarBody();

			glColor3f( m_Color.redF(), m_Color.greenF(), m_Color.blueF() );

			drawTitle();
			drawScaleMarkings();
			drawMajorTickMarkSet();
			drawMinorTickMarkSet();
			glLineWidth( 1.0 );
			glPointSize( 1.0 );

			glEnable( GL_LIGHTING );
			glEnable( GL_DEPTH_TEST );
			glPopAttrib();
		glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

void ColorBar::drawColorBarBody()
{
	if ( param.getColorBarOrientation() == HORIZONTAL ) {
		drawColorBarBodyHorizontal();
	} else {
		drawColorBarBodyVertical();
	}
}

void ColorBar::drawColorBarBodyHorizontal()
{
	float left  = 0.5;
	if ( ( m_VPWidth * left ) <= 400.0 ) {
		left = m_Margin / m_VPWidth;
	}
	float right = ( m_VPWidth - m_Margin ) / m_VPWidth;

	float lx = m_VPWidth * left;
	float dx, x0, x1, y0, y1;
	dx = m_VPWidth * ( right - left ) / 255.0;

	// fill background by checker pattern
	float loop_x = ( m_VPWidth * ( right - left ) ) / 7.5;
	float loop_y = m_ColorBarBodyHeight / 7.5;
	for ( int j = 0; j < loop_y; j++ ) {
		for ( int i = 0; i < loop_x; i++ ) {
			if ( ( j % 2 ) == 0 ) {
				if ( ( i % 2 ) == 0 ) {
					glColor3f( 0.6f, 0.6f, 0.6f );
				} else {
					glColor3f( 0.4f, 0.4f, 0.4f );
				}
			} else {
				if ( ( i % 2 ) == 0 ) {
					glColor3f( 0.4f, 0.4f, 0.4f );
				} else {
					glColor3f( 0.6f, 0.6f, 0.6f );
				}
			}
			glBegin( GL_QUADS );
				x0 = lx + 7.5 * i;
				x1 = lx + 7.5 * ( i + 1 );
				float y0 = m_Margin + m_LineSpace + 7.5 * j;
				float y1 = m_Margin + m_LineSpace + 7.5 * ( j + 1 );
				if ( x1 > m_VPWidth * ( right - left ) ) {
					x1 = lx + m_VPWidth * ( right - left );
				}
				if ( y1 > m_Margin + m_ColorBarBodyHeight + m_LineSpace ) {
					y1 = m_Margin + m_ColorBarBodyHeight + m_LineSpace;
				}
				x0 += m_Offset_X;
				x1 += m_Offset_X;
				glVertex2f( x0, y0 );
				glVertex2f( x1, y0 );
				glVertex2f( x1, y1 );
				glVertex2f( x0, y1 );
			glEnd();
		}
	}

	glEnable( GL_BLEND );
	for ( int i = 0; i <= 254; i++ ) {
		x0 = lx + dx * static_cast< float >( i );
		x1 = lx + dx * static_cast< float >( i + 1 );
		x0 += m_Offset_X;
		x1 += m_Offset_X;
		if ( ( m_Type == COLORBAR_RGB ) || ( m_Type == COLORBAR_RGBA ) ) {
			if ( m_Type == COLORBAR_RGB ) {
				glColor3f( param.getColorTableR( i ),
						   param.getColorTableG( i ),
						   param.getColorTableB( i ) );
			} else {
				glColor4f( param.getColorTableR( i ),
						   param.getColorTableG( i ),
						   param.getColorTableB( i ),
						   param.getColorTableA( i ) );
			}
			y0 = m_Margin + m_LineSpace;
			y1 = y0 + m_ColorBarBodyHeight;
			glBegin( GL_QUADS );
				glVertex2f( x0, y0 );
				glVertex2f( x1, y0 );
				glVertex2f( x1, y1 );
				glVertex2f( x0, y1 );
			glEnd();
		} else {
			y0 = m_Margin + m_LineSpace;
			y1 = y0 + m_ColorBarBodyHeight;
			glColor4f( param.getColorTableR( i ),
					   param.getColorTableG( i ),
					   param.getColorTableB( i ),
					   param.getColorTableA( i ) );
			glBegin( GL_QUADS );
				glVertex2f( x0, y0 );
				glVertex2f( x1, y0 );
				glVertex2f( x1, y1 );
				glVertex2f( x0, y1 );
			glEnd();
			y0 = m_Margin + m_LineSpace + m_ColorBarBodyHeight;
			y1 = y0 + m_ColorBarBodyHeight;
			glColor3f( param.getColorTableR( i ),
					   param.getColorTableG( i ),
					   param.getColorTableB( i ) );
			glBegin( GL_QUADS );
				glVertex2f( x0, y0 );
				glVertex2f( x1, y0 );
				glVertex2f( x1, y1 );
				glVertex2f( x0, y1 );
			glEnd();
		}
	}
	glDisable( GL_BLEND );

	float px0 = m_Offset_X + m_VPWidth * left;
	float px1 = m_Offset_X + m_VPWidth * right;
	float py0 = m_Margin + m_LineSpace;
	float py1 = py0 + m_ColorBarBodyHeight;
	if ( m_Type == COLORBAR_RGB_AND_RGBA ) {
		py1 += m_ColorBarBodyHeight;
	}
	glColor3f( m_Color.redF(), m_Color.greenF(), m_Color.blueF() );
	glBegin( GL_LINE_LOOP );
		glVertex2f( px0, py0 );
		glVertex2f( px1, py0 );
		glVertex2f( px1, py1 );
		glVertex2f( px0, py1 );
	glEnd();
}

void ColorBar::drawColorBarBodyVertical()
{
	m_CBHeight = m_VPHeight * 0.8;
	float dy, x0, x1, y0, y1;
	dy = m_CBHeight / 255.0;

	// fill background by the checker pattern
	float loop_x = m_ColorBarBodyHeight / 7.5;
	float loop_y = m_CBHeight / 7.5;
	for ( int j = 0; j < loop_y; j++ ) {
		for ( int i = 0; i < loop_x; i++ ) {
			if ( ( j % 2 ) == 0 ) {
				if ( ( i % 2 ) == 0 ) {
					glColor3f( 0.6f, 0.6f, 0.6f );
				} else {
					glColor3f( 0.4f, 0.4f, 0.4f );
				}
			} else {
				if ( ( i % 2 ) == 0 ) {
					glColor3f( 0.4f, 0.4f, 0.4f );
				} else {
					glColor3f( 0.6f, 0.6f, 0.6f );
				}
			}
			glBegin( GL_QUADS );
				y0 = m_Offset_Y + m_VPHeight * 0.1 + 7.5 * j;
				y1 = m_Offset_Y + m_VPHeight * 0.1 + 7.5 * ( j + 1 );
				float x0 = m_Margin + m_LineSpace + 7.5 * i;
				float x1 = m_Margin + m_LineSpace + 7.5 * ( i + 1 );
				if ( x1 > m_Margin + m_ColorBarBodyHeight + m_LineSpace ) {
					x1 = m_Margin + m_ColorBarBodyHeight + m_LineSpace;
				}
				if ( y1 > m_Offset_Y + m_VPHeight * 0.9 ) {
					y1 = m_Offset_Y + m_VPHeight * 0.9;
				}
				if ( m_Type == COLORBAR_RGB_AND_RGBA ) {
					x0 += m_ColorBarBodyHeight;
					x1 += m_ColorBarBodyHeight;
				}
				glVertex2f( x0, y0 );
				glVertex2f( x1, y0 );
				glVertex2f( x1, y1 );
				glVertex2f( x0, y1 );
			glEnd();
		}
	}

	glEnable( GL_BLEND );
	for ( int i = 0; i <= 254; i++ ) {
		y0 = m_Offset_Y + m_VPHeight * 0.1 + dy * static_cast< float >( i );
		y1 = m_Offset_Y + m_VPHeight * 0.1 + dy * static_cast< float >( i + 1 );

		if ( ( m_Type == COLORBAR_RGB ) || ( m_Type == COLORBAR_RGBA ) ) {
			if ( m_Type == COLORBAR_RGB ) {
				glColor3f( param.getColorTableR( i ),
						   param.getColorTableG( i ),
						   param.getColorTableB( i ) );
			} else {
				glColor4f( param.getColorTableR( i ),
						   param.getColorTableG( i ),
						   param.getColorTableB( i ),
						   param.getColorTableA( i ) );
			}
			x0 = m_Margin + m_LineSpace;
			x1 = x0 + m_ColorBarBodyHeight;
			glBegin( GL_QUADS );
				glVertex2f( x0, y0 );
				glVertex2f( x1, y0 );
				glVertex2f( x1, y1 );
				glVertex2f( x0, y1 );
			glEnd();
		} else {
			glColor3f( param.getColorTableR( i ),
					   param.getColorTableG( i ),
					   param.getColorTableB( i ) );
			x0 = m_Margin + m_LineSpace;
			x1 = x0 + m_ColorBarBodyHeight;
			glBegin( GL_QUADS );
				glVertex2f( x0, y0 );
				glVertex2f( x1, y0 );
				glVertex2f( x1, y1 );
				glVertex2f( x0, y1 );
			glEnd();
			glColor4f( param.getColorTableR( i ),
					   param.getColorTableG( i ),
					   param.getColorTableB( i ),
					   param.getColorTableA( i ) );
			x0 = m_Margin + m_LineSpace + m_ColorBarBodyHeight;
			x1 = x0 + m_ColorBarBodyHeight;
			glBegin( GL_QUADS );
				glVertex2f( x0, y0 );
				glVertex2f( x1, y0 );
				glVertex2f( x1, y1 );
				glVertex2f( x0, y1 );
			glEnd();
		}
	}
	glDisable( GL_BLEND );

	float px0 = m_Margin + m_LineSpace;
	float px1 = px0 + m_ColorBarBodyHeight;
	float py0 = m_VPHeight * 0.1 + m_Offset_Y;
	float py1 = m_VPHeight * 0.9 + m_Offset_Y;
	if ( m_Type == COLORBAR_RGB_AND_RGBA ) {
		px1 += m_ColorBarBodyHeight;
	}
	glColor3f( m_Color.redF(), m_Color.greenF(), m_Color.blueF() );
	glBegin( GL_LINE_LOOP );
		glVertex2f( px0, py0 );
		glVertex2f( px1, py0 );
		glVertex2f( px1, py1 );
		glVertex2f( px0, py1 );
	glEnd();
}

void ColorBar::drawScaleMarkings()
{
	int marknum = param.getColorBarMajorTickMarkNum();
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
		if ( param.getColorBarOrientation() == HORIZONTAL ) {
			drawScaleHorizontal( str,
				static_cast< float >( i ) / static_cast< float >( marknum - 1 ) );
		} else {
			drawScaleVertical( str,
				static_cast< float >( i ) / static_cast< float >( marknum - 1 ) );
		}
	}
}

void ColorBar::drawScaleHorizontal( char *str, float level )
{
	QFont font = appParam.getFontColorBarValue();
	QFontMetrics fontMetrics( font );

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	int stringWidth = fontMetrics.boundingRect(QString(str)).width();
//	int stringWidth = fontMetrics.horizontalAdvance(QString(str));
#else
	int stringWidth = fontMetrics.width( QString( str ) );
#endif
	int stringHeight = fontMetrics.height();

	float left  = 0.5;
	if ( ( m_VPWidth * left ) <= 400.0 ) {
		left = m_Margin / m_VPWidth;
	}
	float right = ( m_VPWidth - m_Margin ) / m_VPWidth;
	float markerPosX = m_VPWidth * ( left + ( right - left ) * level );
	float textPosX = markerPosX
		- static_cast< float >( stringWidth ) / 2.0 + m_Offset_X;
	float textPosY = m_Margin + m_LineSpace - 7.5 - stringHeight;

#if QT_VERSION < 0x050000
#ifndef Q_WS_MAC
	m_QGLWidget->renderText(
		textPosX, m_VPHeight - textPosY, QString( str ), font );
#else
	renderTextViaQImage( textPosX, textPosY, str, font );
#endif
#else
	renderTextViaQImage( textPosX, textPosY, str, font );
#endif
}

void ColorBar::drawScaleVertical( char *str, float level )
{
	QFont font = appParam.getFontColorBarValue();
	QFontMetrics fontMetrics( font );
//	int stringWidth  = fontMetrics.width( str );
	int stringHeight = fontMetrics.height();

	float minlevel = 0.1f;
	float maxlevel = 0.9f;
	float markerPosY = m_VPHeight * ( minlevel + ( maxlevel - minlevel ) * level );
	float textPosY   = markerPosY
		- static_cast< float >( stringHeight ) / 2.0;
//	textPosY += m_Offset_Y;
	float textPosX   = m_Margin + m_LineSpace + m_ColorBarBodyHeight + 8.0;
	if ( m_Type == COLORBAR_RGB_AND_RGBA ) {
		textPosX += m_ColorBarBodyHeight;
	}

#if QT_VERSION < 0x050000
#ifndef Q_WS_MAC
	m_QGLWidget->renderText(
		textPosX, m_VPHeight - textPosY, QString( str ), font );
#else
	renderTextViaQImage( textPosX, textPosY + m_Offset_Y, str, font );
#endif
#else
	renderTextViaQImage( textPosX, textPosY + m_Offset_Y, str, font );
#endif
}

void ColorBar::drawTitle()
{
	if ( param.getColorBarOrientation() == HORIZONTAL ) {
		drawTitleHorizontal();
	} else {
		drawTitleVertical();
	}
}

void ColorBar::drawTitleHorizontal()
{
	QFont font = appParam.getFontColorBarString();
	QFontMetrics fontMetrics( font );
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	int stringWidth  = fontMetrics.boundingRect(m_Description).width();
//	int stringWidth  = fontMetrics.horizontalAdvance(m_Description);
#else
	int stringWidth  = fontMetrics.width( m_Description );
#endif
	float left  = 0.5;
	if ( ( m_VPWidth * left ) <= 400.0 ) {
		left = m_Margin / m_VPWidth;
	}
	float right = ( m_VPWidth - m_Margin ) / m_VPWidth;
	float centerPosX = m_VPWidth
		* ( left + ( right - left ) * 0.5 ) + m_Offset_X;
	float textPosX   = centerPosX
		- static_cast< float >( stringWidth ) / 2.0;
	float textPosY   = m_Margin + m_LineSpace * 2.0 + 9.0;
	if ( m_Type == COLORBAR_RGB_AND_RGBA ) {
		textPosY += m_ColorBarBodyHeight;
	}

#if QT_VERSION < 0x050000
#ifndef Q_WS_MAC
	m_QGLWidget->renderText(
		textPosX, m_QGLWidget->height() - textPosY,
		m_Description, font );
#else
	renderTextViaQImage( textPosX, textPosY, m_Description, font );
#endif
#else
	renderTextViaQImage( textPosX, textPosY, m_Description, font );
#endif
}

void ColorBar::drawTitleVertical()
{
	QString str = m_Description;
	QFont font = appParam.getFontColorBarString();
	QFontMetrics fontMetrics( font );
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	int stringWidth  = fontMetrics.boundingRect(str).width();
//	int stringWidth  = fontMetrics.horizontalAdvance(str);
#else
	int stringWidth  = fontMetrics.width( str );
#endif
//	int stringHeight = fontMetrics.height();
	float centerPosY = m_VPHeight / 2.0 + m_Offset_Y;
	GLint bottomPosY = static_cast< GLint >( centerPosY
		- static_cast< float >( stringWidth + TEXT_MARGIN * 2 ) / 2.0 );
	GLint textPosX = 37 - font.pixelSize();
	GLint textPosY = bottomPosY;

	renderTextViaQImage( textPosX, textPosY, m_Description, font, 90.0 );
}

void ColorBar::drawMajorTickMarkSet()
{
	int marknum = param.getColorBarMajorTickMarkNum();
	for ( int i = 0; i < marknum; i++ ) {
		if ( param.getColorBarOrientation() == HORIZONTAL ) {
			drawMajorTickMarkHorizontal(
				static_cast< float >( i ) / static_cast< float >( marknum - 1 ) );
		} else {
			drawMajorTickMarkVertical(
				static_cast< float >( i ) / static_cast< float >( marknum - 1 ) );
		}
	}
}

void ColorBar::drawMinorTickMarkSet()
{
	int marknum = param.getColorBarMajorTickMarkNum()
			   + ( param.getColorBarMinorTickMarkNum() * ( param.getColorBarMajorTickMarkNum() - 1 ) );
	for ( int i = 0; i < marknum; i++ ) {
		if ( param.getColorBarOrientation() == HORIZONTAL ) {
			drawMinorTickMarkHorizontal(
				static_cast< float >( i ) / static_cast< float >( marknum - 1 ) );
		} else {
			drawMinorTickMarkVertical(
				static_cast< float >( i ) / static_cast< float >( marknum - 1 ) );
		}
	}
}

void ColorBar::drawMajorTickMarkHorizontal( float level )
{
	ColorBarTickMarkType type;
	type = param.getColorBarMajorTickMarkType();
	if ( type == TICKMARK_NONE ) {
		return;
	}

	float left  = 0.5;
	if ( ( m_VPWidth * left ) <= 400.0 ) {
		left = m_Margin / m_VPWidth;
	}
	float right = ( m_VPWidth - m_Margin ) / m_VPWidth;
	float markerPosX = m_VPWidth * ( left + ( right - left ) * level );
	markerPosX += m_Offset_X;
	float markerPosY = m_Margin + m_LineSpace;

	glBegin( GL_LINES );
	if ( type == TICKMARK_OUTSIDE ) {
		glVertex2f( markerPosX, markerPosY - m_MajorTickMarkLength );
		glVertex2f( markerPosX, markerPosY );
	} else if ( type == TICKMARK_INSIDE ) {
		glVertex2f( markerPosX, markerPosY );
		glVertex2f( markerPosX, markerPosY + m_MajorTickMarkLength );
	} else if ( type == TICKMARK_CROSS ) {
		glVertex2f( markerPosX, markerPosY );
		if ( m_Type == COLORBAR_RGB_AND_RGBA ) {
			glVertex2f( markerPosX, markerPosY + m_ColorBarBodyHeight * 2.0 );
		} else {
			glVertex2f( markerPosX, markerPosY + m_ColorBarBodyHeight );
		}
	}
	glEnd();
}

void ColorBar::drawMajorTickMarkVertical( float level )
{
	ColorBarTickMarkType type;
	type = param.getColorBarMajorTickMarkType();
	if ( type == TICKMARK_NONE ) {
		return;
	}

	float minlevel = 0.1f;
	float maxlevel = 0.9f;
	float markerPosX = m_Margin + m_LineSpace + m_ColorBarBodyHeight;
	float markerPosY = m_VPHeight * ( minlevel + ( maxlevel - minlevel ) * level );
	markerPosY += m_Offset_Y;

	glBegin( GL_LINES );
	if ( type == TICKMARK_OUTSIDE ) {
		if ( m_Type == COLORBAR_RGB_AND_RGBA ) {
			markerPosX += m_ColorBarBodyHeight;
		}
		glVertex2f( markerPosX, markerPosY );
		glVertex2f( markerPosX + m_MajorTickMarkLength, markerPosY );
	} else if ( type == TICKMARK_INSIDE ) {
		if ( m_Type == COLORBAR_RGB_AND_RGBA ) {
			markerPosX += m_ColorBarBodyHeight;
		}
		glVertex2f( markerPosX - m_MajorTickMarkLength, markerPosY );
		glVertex2f( markerPosX, markerPosY );
	} else if ( type == TICKMARK_CROSS ) {
		glVertex2f( markerPosX - m_ColorBarBodyHeight, markerPosY );
		if ( m_Type == COLORBAR_RGB_AND_RGBA ) {
			markerPosX += m_ColorBarBodyHeight;
		}
		glVertex2f( markerPosX, markerPosY );
	}
	glEnd();
}

void ColorBar::drawMinorTickMarkHorizontal( float level )
{
	ColorBarTickMarkType type;
	type = param.getColorBarMinorTickMarkType();
	if ( type == TICKMARK_NONE ) return;

	float left  = 0.5;
	if ( ( m_VPWidth * left ) <= 400.0 ) {
		left = m_Margin / m_VPWidth;
	}
	float right = ( m_VPWidth - m_Margin ) / m_VPWidth;
	float markerPosX = m_VPWidth * ( left + ( right - left ) * level );
	markerPosX += m_Offset_X;
	float markerPosY = m_Margin + m_LineSpace;

	glBegin( GL_LINES );
	if ( type == TICKMARK_OUTSIDE ) {
		glVertex2f( markerPosX, markerPosY - m_MinorTickMarkLength );
		glVertex2f( markerPosX, markerPosY );
	} else if ( type == TICKMARK_INSIDE ) {
		glVertex2f( markerPosX, markerPosY );
		glVertex2f( markerPosX, markerPosY + m_MinorTickMarkLength );
	} else if ( type == TICKMARK_CROSS ) {
		glVertex2f( markerPosX, markerPosY );
		if ( m_Type == COLORBAR_RGB_AND_RGBA ) {
			glVertex2f( markerPosX, markerPosY + m_ColorBarBodyHeight * 2.0 );
		} else {
			glVertex2f( markerPosX, markerPosY + m_ColorBarBodyHeight );
		}
	}
	glEnd();
}

void ColorBar::drawMinorTickMarkVertical( float level )
{
	ColorBarTickMarkType type;
	type = param.getColorBarMinorTickMarkType();
	if ( type == TICKMARK_NONE ) return;

	float minlevel = 0.1f;
	float maxlevel = 0.9f;
	float markerPosX = m_Margin + m_LineSpace + m_ColorBarBodyHeight;
	float markerPosY = m_VPHeight * ( minlevel + ( maxlevel - minlevel ) * level );
	markerPosY += m_Offset_Y;

	glBegin( GL_LINES );
	if ( type == TICKMARK_OUTSIDE ) {
		if ( m_Type == COLORBAR_RGB_AND_RGBA ) {
			markerPosX += m_ColorBarBodyHeight;
		}
		glVertex2f( markerPosX, markerPosY );
		glVertex2f( markerPosX + m_MinorTickMarkLength, markerPosY );
	} else if ( type == TICKMARK_INSIDE ) {
		if ( m_Type == COLORBAR_RGB_AND_RGBA ) {
			markerPosX += m_ColorBarBodyHeight;
		}
		glVertex2f( markerPosX - m_MinorTickMarkLength, markerPosY );
		glVertex2f( markerPosX, markerPosY );
	} else if ( type == TICKMARK_CROSS ) {
		glVertex2f( markerPosX - m_ColorBarBodyHeight, markerPosY );
		if ( m_Type == COLORBAR_RGB_AND_RGBA ) {
			markerPosX += m_ColorBarBodyHeight;
		}
		glVertex2f( markerPosX, markerPosY );
	}
	glEnd();
}

void ColorBar::renderTextViaQImage
( int x, int y, QString str, QFont font, float deg )
{
	// create QImage to draw to GLContext
	QFontMetrics fontMetrics( font );
	QRect rect = fontMetrics.boundingRect( str );
	QImage image( rect.width(), rect.height(), QImage::Format_ARGB32 );
	image.fill( 0 );

	// setup QPen and bind QPainter to QImage
	QPen pen( m_Color );
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
