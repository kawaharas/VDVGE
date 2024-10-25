//
// Volume Data Visualizer for Google Earth
//
// texture3d.cpp
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

#include "texture3d.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#ifdef Q_WS_WIN
extern PFNGLTEXIMAGE3DPROC glTexImage3D;
#endif
#else
#ifdef Q_OS_WIN
extern PFNGLTEXIMAGE3DPROC glTexImage3D;
#endif
#endif

Texture3D::Texture3D()
{
	m_Data = NULL;
}

Texture3D::~Texture3D()
{
	if ( m_Data != NULL ) {
		delete [] m_Data;
	}
}

void Texture3D::initialize()
{
	m_MaxTex3D_W = appParam.getMaxTex3DWidth();
	m_MaxTex3D_H = appParam.getMaxTex3DHeight();
	m_MaxTex3D_D = appParam.getMaxTex3DDepth();

	int maxTexUnits;
	int maxTexSize;
	glGetIntegerv( GL_MAX_TEXTURE_UNITS, &maxTexUnits );
	glGetIntegerv( GL_MAX_3D_TEXTURE_SIZE, &maxTexSize );
	int max3DTexSize = maxTexSize / maxTexUnits;
	if ( m_MaxTex3D_W > max3DTexSize ) {
		m_MaxTex3D_W = max3DTexSize;
	}
	if ( m_MaxTex3D_H > max3DTexSize ) {
		m_MaxTex3D_H = max3DTexSize;
	}
	if ( m_MaxTex3D_D > max3DTexSize ) {
		m_MaxTex3D_D = max3DTexSize;
	}

	if ( m_Data != NULL ) {
		delete [] m_Data;
		glDeleteTextures( 1, &m_Texture );
		glDeleteTextures( 1, &m_TextureRGB );
	} else {
		glGenTextures( 1, &m_Texture );
		glGenTextures( 1, &m_TextureRGB );
	}
	glBindTexture( GL_TEXTURE_3D, m_Texture );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelTransferi( GL_MAP_COLOR, GL_TRUE );
//	glTexParameteri( GL_TEXTURE_3D, GL_GENERATE_MIPMAP, GL_TRUE );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	glBindTexture( GL_TEXTURE_3D, m_TextureRGB );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelTransferi( GL_MAP_COLOR, GL_TRUE );
//	glTexParameteri( GL_TEXTURE_3D, GL_GENERATE_MIPMAP, GL_TRUE );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	m_Width  = grads.getGridSizeX();
	m_Height = grads.getGridSizeY();
	m_Depth  = grads.getGridSizeZ();

	if ( m_Width  > m_MaxTex3D_W ) {
		m_Width  = m_MaxTex3D_W;
	} else {
		for ( int i = 0; i < 10; i++ ) {
			int power2 = static_cast< int >( pow( 2.0, static_cast< double >( i ) ) );
			if ( m_Width <= power2 ) {
				m_Width = power2;
				break;
			}
		}
	}
	if ( m_Height > m_MaxTex3D_H ) {
		m_Height = m_MaxTex3D_H;
	} else {
		for ( int i = 0; i < 10; i++ ) {
			int power2 = static_cast< int >( pow( 2.0, static_cast< double >( i ) ) );
			if ( m_Height <= power2 ) {
				m_Height = power2;
				break;
			}
		}
	}
	if ( m_Depth  > m_MaxTex3D_D ) {
		m_Depth  = m_MaxTex3D_D;
	} else {
		for ( int i = 0; i < 6; i++ ) {
			int power2 = static_cast< int >( pow( 2.0, static_cast< double >( i ) ) );
			if ( m_Depth <= power2 ) {
				m_Depth = power2;
				break;
			}
		}
	}
	m_Size = m_Width * m_Height * m_Depth;
	m_Data = new GLubyte[ m_Size ];
	std::fill( m_Data, m_Data + m_Size, 0 );
}

void Texture3D::setData()
{
	interpolator( m_Width, m_Height, m_Depth, m_Data );
}

void Texture3D::setColorTable()
{
	glPixelMapfv( GL_PIXEL_MAP_I_TO_R, 256, param.getColorTableR() );
	glPixelMapfv( GL_PIXEL_MAP_I_TO_G, 256, param.getColorTableG() );
	glPixelMapfv( GL_PIXEL_MAP_I_TO_B, 256, param.getColorTableB() );
	glPixelMapfv( GL_PIXEL_MAP_I_TO_A, 256, param.getColorTableA() );
	glPixelTransferi( GL_MAP_COLOR, GL_TRUE );

	glBindTexture( GL_TEXTURE_3D, m_Texture );
	glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA,
				  m_Width, m_Height, m_Depth, 0,
				  GL_COLOR_INDEX, GL_UNSIGNED_BYTE, m_Data );

	float alpha[256];
	for ( int i = 0; i < 255; i++ ) {
		alpha[i] = 1.0;
	}
	alpha[255] = 0.0;
	glPixelMapfv( GL_PIXEL_MAP_I_TO_A, 256, alpha );
	glPixelTransferi( GL_MAP_COLOR, GL_TRUE );

	glBindTexture( GL_TEXTURE_3D, m_TextureRGB );
	glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA,
				  m_Width, m_Height, m_Depth, 0,
				  GL_COLOR_INDEX, GL_UNSIGNED_BYTE, m_Data );

	// added for Qt6 (2023/06/01)
	// If the pixel transfer parameter GL_MAP_COLOR is still enabled,
	// the area outside the preview window will also be filled with black.
	glPixelTransferi( GL_MAP_COLOR, GL_FALSE );
}
