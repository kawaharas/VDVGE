//
// Volume Data Visualizer for Google Earth
//
// earth.cpp
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

#include "earth.h"

Earth::Earth( QWidget *parent ) :
	QGLWidget( parent )
{
	m_Earth = gluNewQuadric();

	gluQuadricDrawStyle  ( m_Earth, GLU_FILL    );
	gluQuadricNormals    ( m_Earth, GLU_SMOOTH  );
	gluQuadricOrientation( m_Earth, GLU_OUTSIDE );
	gluQuadricTexture    ( m_Earth, GL_TRUE     );
//	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

#if QT_VERSION < 0x050000
#ifndef Q_WS_WIN
	glGenTextures( 1, &m_TexEarth );
#endif
#else
#ifndef Q_OS_WIN
	glGenTextures( 1, &m_TexEarth );
#endif
#endif

	readTexture();
}

Earth::~Earth()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	glDeleteTextures( 1, &m_TexEarth );
#else
	QGLWidget::deleteTexture( m_TexEarth );
#endif
}

void Earth::readTexture()
{
	QImage image_glformat;
	QImage image_input;
	image_input.load( ":/resources/earthLarge.png" );
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	image_glformat = image_input.mirrored(false, true).convertToFormat(QImage::Format_ARGB32);
#else
	image_glformat = QGLWidget::convertToGLFormat( image_input );
#endif
	glPixelTransferi( GL_MAP_COLOR, GL_FALSE );

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
	m_TexEarth = QGLWidget::bindTexture( image_glformat, GL_TEXTURE_2D, GL_RGBA );
#else
	glBindTexture( GL_TEXTURE_2D, m_TexEarth );
#endif
#else
#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	glBindTexture( GL_TEXTURE_2D, m_TexEarth );
#else
	m_TexEarth = QGLWidget::bindTexture( image_glformat, GL_TEXTURE_2D, GL_RGBA );
#endif
#else
	glBindTexture( GL_TEXTURE_2D, m_TexEarth );
#endif
#endif

	gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA,
					   image_glformat.width(), image_glformat.height(),
					   GL_RGBA, GL_UNSIGNED_BYTE, image_glformat.bits() );

}

void Earth::draw()
{
	glEnable( GL_DEPTH_TEST );
	glCullFace( GL_BACK );
	glEnable( GL_CULL_FACE );
	glBindTexture( GL_TEXTURE_2D, m_TexEarth );
	glEnable( GL_TEXTURE_2D );
	glPushMatrix();
		static GLfloat amb[]  = { 1.0f, 1.0f, 1.0f, 0.1f };
		static GLfloat diff[] = { 1.0f, 1.0f, 1.0f, 0.9f };
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, amb );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diff );
		glRotatef( -90.0f, 0.0, 0.0, 1.0f );
		gluSphere( m_Earth, EARTH_OBJECT_RADIUS, 64, 64 );
	glPopMatrix();
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_CULL_FACE );
}
