//
// Volume Data Visualizer for Google Earth
//
// timedisplay.cpp
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
// TimeDisplay::renderTextViaQImage()
//
// In Qt4 for Mac, QGLWidget::renderText() and QPainter::drawText()
// has a bug that is the misalienment of the character's vertical position
// when a text string was drawn directly to the frame buffer.
// In order to solve this problem, this program render the text strings
// via QPainter that was binded to QImage.
/////////////////////////////////////////////////////////////////////////

#include "timedisplay.h"

TimeDisplay::TimeDisplay( QGLWidget *parent )
{
	m_QGLWidget = parent;
}

void TimeDisplay::draw()
{
	if ( param.IsTimeDisplay() ) {
		int winWidth  = m_QGLWidget->width();
		int winHeight = m_QGLWidget->height();
		int contentsMargin = 35;

		QString TimeString = getTimeString( getActualTime( param.getCurrentStep() ) );
		QFont font = appParam.getFontTimeDisplay();
		QFontMetrics fontMetrics( font );
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
		int stringWidth  = fontMetrics.boundingRect(QString(TimeString)).width();
//		int stringWidth  = fontMetrics.horizontalAdvance(QString(TimeString));
#else
		int stringWidth  = fontMetrics.width( QString( TimeString ) );
#endif
		int stringHeight = fontMetrics.height();
		int textPosX = winWidth  - stringWidth  - contentsMargin;
		int textPosY = winHeight - stringHeight - contentsMargin;

		QColor complementaryColor;
		complementaryColor.setRgb(
			abs( param.getBGColor().red()   - 255 ),
			abs( param.getBGColor().green() - 255 ),
			abs( param.getBGColor().blue()  - 255 ) );

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		glColor3f(
			complementaryColor.redF(),
			complementaryColor.greenF(),
			complementaryColor.blueF());
#else
		m_QGLWidget->qglColor( complementaryColor );
#endif

		glPushMatrix();
		glLoadIdentity();
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
			glLoadIdentity();
			gluOrtho2D( 0, winWidth, 0, winHeight );
			glPushAttrib( GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT );
			glDisable( GL_DEPTH_TEST );
			glDisable( GL_LIGHTING );

#if QT_VERSION < 0x050000
#ifndef Q_WS_MAC
			m_QGLWidget->renderText(
				textPosX, winHeight - textPosY, TimeString, font );
#else
			renderTextViaQImage(
				textPosX, textPosY, TimeString, font, complementaryColor );
#endif
#else
			renderTextViaQImage(
				textPosX, textPosY, TimeString, font, complementaryColor );
#endif

			glEnable( GL_LIGHTING );
			glEnable( GL_DEPTH_TEST );
			glPopAttrib();
		glPopMatrix();
		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();
	}
}

void TimeDisplay::renderTextViaQImage(
	int x, int y, QString str, QFont font, QColor color )
{
	// create QImage to draw to GLContext
	QFontMetrics fontMetrics( font );
	QRect rect = fontMetrics.boundingRect( str );
	QImage image( rect.width(), rect.height(), QImage::Format_ARGB32 );
	image.fill( 0 );

	// setup QPen and bind QPainter to QImage
	QPen pen( color );
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

	// draw QImage to GLContext
	glPixelTransferi( GL_MAP_COLOR, GL_FALSE );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glRasterPos2i( x, y );
	glEnable( GL_BLEND );
	glDrawPixels( glImage.width(), glImage.height(),
		GL_RGBA, GL_UNSIGNED_BYTE, glImage.bits() );
	glDisable( GL_BLEND );
}
