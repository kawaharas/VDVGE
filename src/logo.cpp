//
// Volume Data Visualizer for Google Logo
//
// Logo.cpp
//
// Copyright (c) 2012-2015 Shintaro KAWAHARA (kawahara@jamstec.go.jp)
// Japan Agency for Marine-Logo Science and Technology (JAMSTEC)
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

#include "logo.h"

Logo::Logo( QGLWidget *parent )
{
	m_QGLWidget = parent;

	initialize();
}

Logo::~Logo()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	QOpenGLFunctions glFuncs(m_QGLWidget->context());
	glFuncs.glDeleteTextures(1, &m_TexLogo);
#else
	m_QGLWidget->deleteTexture( m_TexLogo );
#endif
}

void Logo::initialize()
{
/*
	QString logofile = QCoreApplication::applicationDirPath() + "/logo.png";
	if ( QFileInfo( logofile ).exists() ) {
		m_LogoFileName = logofile;
		readTexture( m_LogoFileName );
	}
*/
	readTexture();
}

void Logo::readTexture()
{
	QImage image;
//	image.load( m_LogoFileName );
	if ( m_LogoFileName.isNull() ) {
		image.load( ":/resources/logo.png" );
	} else {
		image.load( m_LogoFileName );
	}
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	QImage glImage = image.mirrored(false, true).convertToFormat(QImage::Format_ARGB32);
#else
	QImage glImage = QGLWidget::convertToGLFormat( image );
#endif
	glPixelTransferi( GL_MAP_COLOR, GL_FALSE );
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	glGenTextures(1, &m_TexLogo);
	glBindTexture(GL_TEXTURE_2D, m_TexLogo);
#else
	m_TexLogo = m_QGLWidget->bindTexture( glImage, GL_TEXTURE_2D, GL_RGBA );
#endif
	m_TexLogoWidth  = static_cast< float >( glImage.width() );
	m_TexLogoHeight = static_cast< float >( glImage.height() );
	gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA,
					   glImage.width(), glImage.height(),
					   GL_RGBA, GL_UNSIGNED_BYTE, glImage.bits() );
	param.setLogoAspectRatio( m_TexLogoHeight / m_TexLogoWidth );
}

void Logo::readTexture( QString filename )
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	QOpenGLFunctions* glFunc = m_QGLWidget->context()->functions();
	glFunc->glDeleteTextures( 1, &m_TexLogo );
#else
	m_QGLWidget->deleteTexture( m_TexLogo );
#endif

	QImage image;
//	image.load( filename );
	if ( filename.isNull() ) {
		image.load( ":/resources/logo.png" );
	} else {
		image.load( filename );
	}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	QImage glImage = image.mirrored(false, true).convertToFormat(QImage::Format_ARGB32);
#else
	QImage glImage = QGLWidget::convertToGLFormat( image );
#endif
	glPixelTransferi( GL_MAP_COLOR, GL_FALSE );
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	glGenTextures(1, &m_TexLogo);
	glBindTexture(GL_TEXTURE_2D, m_TexLogo);
#else
	m_TexLogo = m_QGLWidget->bindTexture( glImage, GL_TEXTURE_2D, GL_RGBA );
#endif
	m_TexLogoWidth  = static_cast< float >( glImage.width() );
	m_TexLogoHeight = static_cast< float >( glImage.height() );
	gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA,
					   glImage.width(), glImage.height(),
					   GL_RGBA, GL_UNSIGNED_BYTE, glImage.bits() );
	param.setLogoAspectRatio( m_TexLogoHeight / m_TexLogoWidth );
}

void Logo::draw()
{
	if ( !param.IsLogo() ) return;

	m_VPWidth  = static_cast< float >( m_QGLWidget->width() );
	m_VPHeight = static_cast< float >( m_QGLWidget->height() );

	glPushMatrix();
	glLoadIdentity();
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
			glLoadIdentity();
			gluOrtho2D( 0, static_cast< int >( m_VPWidth ),
						0, static_cast< int >( m_VPHeight ) );
			glPushAttrib( GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT );
			glDisable( GL_DEPTH_TEST );
			glDisable( GL_LIGHTING );

			glEnable( GL_BLEND );
			glEnable( GL_TEXTURE_2D );
			glPushMatrix();
			glBindTexture( GL_TEXTURE_2D, m_TexLogo );
			glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
			glColor4f( 1.0, 1.0, 1.0, 1.0 );
			glBegin( GL_QUADS );
				GLfloat x0, x1, y0, y1, logoWidth;
				logoWidth = m_VPWidth * 0.1;
				if ( logoWidth < MIN_LOGO_WIDTH ) {
					logoWidth = MIN_LOGO_WIDTH;
				}
				x0 = 35.0;
				x1 = 35.0 + logoWidth;
				y0 = 35.0;
				y1 = 35.0 + logoWidth * m_TexLogoHeight / m_TexLogoWidth;
				glTexCoord2f( 0.0, 0.0 );
				glVertex2f( x0, y0 );
				glTexCoord2f(  1.0,  0.0 );
				glVertex2f( x1, y0 );
				glTexCoord2f(  1.0,  1.0 );
				glVertex2f( x1, y1 );
				glTexCoord2f(  0.0, 1.0 );
				glVertex2f( x0, y1 );
				glEnd();
			glPopMatrix();
			glDisable( GL_TEXTURE_2D );
			glDisable( GL_BLEND );
			glEnable( GL_DEPTH_TEST );

			glEnable( GL_LIGHTING );
			glEnable( GL_DEPTH_TEST );
			glPopAttrib();
		glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}
