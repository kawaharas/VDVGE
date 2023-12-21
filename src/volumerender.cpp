//
// Volume Data Visualizer for Google Earth
//
// volumerender.cpp
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

#include "volumerender.h"

int drawOrderX[ 2000 ];
int drawOrderY[ 2000 ];
int drawOrderZ[ 2000 ];

void VolumeRender::draw()
{
	float dx, dy, dz;
	if ( param.getVolumeXNum() != 1 ) {
		dx = 1.0 / ( float )( param.getVolumeXNum() - 1 );
	} else {
		dx = 0.0;
	}
	if ( param.getVolumeYNum() != 1 ) {
		dy = 1.0 / ( float )( param.getVolumeYNum() - 1 );
	} else {
		dy = 0.0;
	}
	if ( param.getVolumeZNum() != 1 ) {
		dz = 1.0 / ( float )( param.getVolumeZNum() - 1 );
	} else {
		dz = 0.0;
	}

	glPushMatrix();
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glBindTexture( GL_TEXTURE_3D, texture3d.getTexture() );
//	glDisable( GL_DEPTH_TEST );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glEnable( GL_TEXTURE_3D );
	if ( param.IsVolumeX() ) {
		if ( ( param.getObjectAngleYaw() < 360.0 ) &&
			 ( param.getObjectAngleYaw() > 180.0 ) ) {
			for ( int i = 0; i < param.getVolumeXNum(); i++ ) {
				drawOrderX[i] = i;
			}
		} else {
			for ( int i = 0; i < param.getVolumeXNum(); i++ ) {
				drawOrderX[i] = ( param.getVolumeXNum() - 1 ) - i;
			}
		}
		for ( int i = 0; i < param.getVolumeXNum(); i++ ) {
			glBegin( GL_QUADS );
			glTexCoord3f( 0.0 + dx * drawOrderX[i],  0.0,  0.0 );
			glVertex3f(  -0.5 + dx * drawOrderX[i], -0.5, -0.5 );
			glTexCoord3f( 0.0 + dx * drawOrderX[i],  0.0,  1.0 );
			glVertex3f(  -0.5 + dx * drawOrderX[i], -0.5,  0.5 );
			glTexCoord3f( 0.0 + dx * drawOrderX[i],  1.0,  1.0 );
			glVertex3f(  -0.5 + dx * drawOrderX[i],  0.5,  0.5 );
			glTexCoord3f( 0.0 + dx * drawOrderX[i],  1.0,  0.0 );
			glVertex3f(  -0.5 + dx * drawOrderX[i],  0.5, -0.5 );
			glEnd();
		}
	}
	if ( param.IsVolumeY() ) {
		if ( ( param.getObjectAngleYaw() < 270.0 ) &&
			 ( param.getObjectAngleYaw() >  90.0 ) ) {
			for ( int i = 0; i < param.getVolumeYNum(); i++ ) {
				drawOrderY[i] = i;
			}
		} else {
			for ( int i = 0; i < param.getVolumeYNum(); i++ ) {
				drawOrderY[i] = ( param.getVolumeYNum() - 1 ) - i;
			}
		}
		for ( int i = 0; i < param.getVolumeYNum(); i++ ) {
			glBegin( GL_QUADS );
			glTexCoord3f( 0.0,  0.0 + dy * drawOrderY[i],  0.0 );
			glVertex3f(  -0.5, -0.5 + dy * drawOrderY[i], -0.5 );
			glTexCoord3f( 1.0,  0.0 + dy * drawOrderY[i],  0.0 );
			glVertex3f(   0.5, -0.5 + dy * drawOrderY[i], -0.5 );
			glTexCoord3f( 1.0,  0.0 + dy * drawOrderY[i],  1.0 );
			glVertex3f(   0.5, -0.5 + dy * drawOrderY[i],  0.5 );
			glTexCoord3f( 0.0,  0.0 + dy * drawOrderY[i],  1.0 );
			glVertex3f(  -0.5, -0.5 + dy * drawOrderY[i],  0.5 );
			glEnd();
		}
	}
	if ( param.IsVolumeZ() ) {
		if ( param.getObjectAnglePitch() >= 0.0 ) {
			for ( int i = 0; i < param.getVolumeZNum(); i++ ) {
				drawOrderZ[i] = i;
			}
		} else {
			for ( int i = 0; i < param.getVolumeZNum(); i++ ) {
				drawOrderZ[i] = ( param.getVolumeZNum() - 1 ) - i;
			}
		}
		for ( int i = 0; i < param.getVolumeZNum(); i++ ) {
			glBegin( GL_QUADS );
			glTexCoord3f( 0.0,  0.0,  0.0 + dz * drawOrderZ[i] );
			glVertex3f(  -0.5, -0.5, -0.5 + dz * drawOrderZ[i] );
			glTexCoord3f( 1.0,  0.0,  0.0 + dz * drawOrderZ[i] );
			glVertex3f(   0.5, -0.5, -0.5 + dz * drawOrderZ[i] );
			glTexCoord3f( 1.0,  1.0,  0.0 + dz * drawOrderZ[i] );
			glVertex3f(   0.5,  0.5, -0.5 + dz * drawOrderZ[i] );
			glTexCoord3f( 0.0,  1.0,  0.0 + dz * drawOrderZ[i] );
			glVertex3f(  -0.5,  0.5, -0.5 + dz * drawOrderZ[i] );
			glEnd();
		}
	}
	glDisable( GL_TEXTURE_3D );
	glDisable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );
	glPopMatrix();
}

void VolumeRender::drawSphereData()
{
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glPushAttrib( GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );

	glPushMatrix();
	if ( param.IsVolumeZ() ) {
		for( int i = 0; i < param.getVolumeZNum(); i++ ) {
			float slicePos;
			if ( param.getVolumeZNum() != 1 ) {
				slicePos = 1.0 / static_cast< float >( param.getVolumeZNum() - 1 )
				* static_cast< float >( i );
			} else {
				slicePos = 0.0;
			}
			drawSphereXYPlane( slicePos );
		}
	}

	glEnable( GL_CLIP_PLANE0 );
	if ( param.IsVolumeX() ) {
		GLfloat orgYaw = -90.0 - ( grads.getGridXMin() + ( grads.getGridXMax() - grads.getGridXMin() ) / 2.0 );
		if ( orgYaw < 0.0 ) {
			orgYaw += 360.0;
		}
		if ( ( param.getObjectAngleSphereYaw() < orgYaw ) &&
			 ( param.getObjectAngleSphereYaw() > ( orgYaw - 180.0 ) ) ) {
			for ( int i = 0; i < param.getVolumeXNum(); i++ ) {
				drawOrderX[i] = i;
			}
		} else {
			for ( int i = 0; i < param.getVolumeXNum(); i++ ) {
				drawOrderX[i] = param.getVolumeXNum() - i;
			}
		}
		for ( int i = 0; i < param.getVolumeXNum(); i++ ) {
			float slicePos =
				1.0 / static_cast< float >( param.getVolumeXNum() - 1 )
				* static_cast< float >( drawOrderX[i] );
			drawSphereYZPlane( slicePos );
		}
	}

	if ( param.IsVolumeY() ) {
		GLfloat orgPitch = grads.getGridYMin() + ( grads.getGridYMax() - grads.getGridYMin() ) / 2.0;
		if ( ( param.getObjectAngleSpherePitch() < orgPitch ) &&
			 ( param.getObjectAngleSpherePitch() > ( orgPitch - 180.0 ) ) ) {
			for ( int i = 0; i < param.getVolumeYNum(); i++ ) {
				drawOrderY[i] = param.getVolumeYNum() - i;
			}
		} else {
			for ( int i = 0; i < param.getVolumeYNum(); i++ ) {
				drawOrderY[i] = i;
			}
		}

		for ( int i = 0; i < param.getVolumeYNum(); i++ ) {
			float slicePos =
				1.0 / static_cast< float >( param.getVolumeYNum() - 1 )
				* static_cast< float >( drawOrderY[i] );
			drawSphereXZPlane( slicePos );
		}
	}
	glDisable( GL_CLIP_PLANE0 );
	glPopMatrix();

	glDisable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );
	glPopAttrib();
}

void VolumeRender::drawSphereYZPlane( float slicePos )
{
	if ( slicePos < 0.0 ) {
		slicePos = 0.0;
	} else if ( slicePos > 1.0 ) {
		slicePos = 1.0;
	}
	float height0 = static_cast< float >(
		( EARTH_OBJECT_RADIUS * grads.getGridZMin() / EARTH_RADIUS_WE_WGS84 ) * param.getZScale() );
	float height1 = static_cast< float >(
		( EARTH_OBJECT_RADIUS * grads.getGridZMax() / EARTH_RADIUS_WE_WGS84 ) * param.getZScale() );
	if ( param.getZUnit() == ZUNIT_KILOMETER ) {
		height0 *= 1000.0;
		height1 *= 1000.0;
	}
	height0 += EARTH_OBJECT_RADIUS;
	height1 += EARTH_OBJECT_RADIUS;

	double rangeDegLat = grads.getGridYMax() - grads.getGridYMin();
	double rangeDegLon = grads.getGridXMax() - grads.getGridXMin();
	double rangeRadLat = rangeDegLat / 180.0f * M_PI;
	double rangeRadLon = rangeDegLon / 180.0f * M_PI;
	int    divLat = ( int )( rangeDegLat / 1.5f );
	double dRadLat = rangeRadLat / ( double )divLat;
	float  dTexLat = 1.0f / ( float )divLat;

	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	glPushMatrix();
	glEnable( GL_TEXTURE_3D );
	glBindTexture( GL_TEXTURE_3D, texture3d.getTexture() );
	glBegin( GL_QUADS );
	float radLatitude = ( 90.0 - grads.getGridYMax() ) * M_PI / 180.0;
	float radLongitude = grads.getGridXMin() * M_PI / 180.0;
	const float lon = radLongitude + rangeRadLon * slicePos;

	for ( int ui = 0; ui < divLat; ui++ )
	{
		const float lat0 = radLatitude + dRadLat *   ui;
		const float lat1 = radLatitude + dRadLat * ( ui + 1 );
		const float texLat0 = 1.0 - ( dTexLat *   ui );
		const float texLat1 = 1.0 - ( dTexLat * ( ui + 1 ) );

		float TexCoord00[3] = { slicePos, texLat0, 0.0 };
		float TexCoord10[3] = { slicePos, texLat1, 0.0 };
		float TexCoord11[3] = { slicePos, texLat1, 1.0 };
		float TexCoord01[3] = { slicePos, texLat0, 1.0 };
		float Vertex00[3] = { height0 * sinf( lat0 ) * cosf( lon ),
							  height0 * sinf( lat0 ) * sinf( lon ),
							  height0 * cosf( lat0 ) };
		float Vertex10[3] = { height0 * sinf( lat1 ) * cosf( lon ),
							  height0 * sinf( lat1 ) * sinf( lon ),
							  height0 * cosf( lat1 ) };
		float Vertex11[3] = { height1 * sinf( lat1 ) * cosf( lon ),
							  height1 * sinf( lat1 ) * sinf( lon ),
							  height1 * cosf( lat1 ) };
		float Vertex01[3] = { height1 * sinf( lat0 ) * cosf( lon ),
							  height1 * sinf( lat0 ) * sinf( lon ),
							  height1 * cosf( lat0 ) };

		glTexCoord3fv( TexCoord00 );
		glVertex3fv( Vertex00 );
		glTexCoord3fv( TexCoord10 );
		glVertex3fv( Vertex10 );
		glTexCoord3fv( TexCoord11 );
		glVertex3fv( Vertex11 );
		glTexCoord3fv( TexCoord01 );
		glVertex3fv( Vertex01 );
	}
	glEnd();
	glDisable( GL_TEXTURE_3D );
	glPopMatrix();
}

void VolumeRender::drawSphereXZPlane( float slicePos )
{
	if ( slicePos < 0.0 ) {
		slicePos = 0.0;
	} else if ( slicePos > 1.0 ) {
		slicePos = 1.0;
	}
	float height0 = static_cast< float >(
		( EARTH_OBJECT_RADIUS * grads.getGridZMin() / EARTH_RADIUS_WE_WGS84 ) * param.getZScale() );
	float height1 = static_cast< float >(
		( EARTH_OBJECT_RADIUS * grads.getGridZMax() / EARTH_RADIUS_WE_WGS84 ) * param.getZScale() );
	if ( param.getZUnit() == ZUNIT_KILOMETER ) {
		height0 *= 1000.0;
		height1 *= 1000.0;
	}
	height0 += EARTH_OBJECT_RADIUS;
	height1 += EARTH_OBJECT_RADIUS;

	double rangeDegLat = grads.getGridYMax() - grads.getGridYMin();
	double rangeDegLon = grads.getGridXMax() - grads.getGridXMin();
	double rangeRadLat = rangeDegLat / 180.0f * M_PI;
	double rangeRadLon = rangeDegLon / 180.0f * M_PI;
	int    divLon = ( int )( rangeDegLon / 1.5f );
	double dRadLon = rangeRadLon / ( double )divLon;
	float  dTexLon = 1.0f / ( float )divLon;

	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	glPushMatrix();
	glEnable( GL_TEXTURE_3D );
	glBindTexture( GL_TEXTURE_3D, texture3d.getTexture() );
	glBegin( GL_QUADS );
	float radLatitude = ( 90.0 - grads.getGridYMax() ) * M_PI / 180.0;
	float radLongitude = grads.getGridXMin() * M_PI / 180.0;
	const float lat = radLatitude + rangeRadLat * ( 1.0 - slicePos );
	for ( int ui = 0; ui < divLon; ui++ )
	{
		const float lon0 = radLongitude + dRadLon *   ui;
		const float lon1 = radLongitude + dRadLon * ( ui + 1 );
		const float texLon0 = dTexLon *   ui;
		const float texLon1 = dTexLon * ( ui + 1 );

		float TexCoord00[3] = { texLon0, slicePos, 0.0 };
		float TexCoord10[3] = { texLon1, slicePos, 0.0 };
		float TexCoord11[3] = { texLon1, slicePos, 1.0 };
		float TexCoord01[3] = { texLon0, slicePos, 1.0 };
		float Vertex00[3] = { height0 * sinf( lat ) * cosf( lon0 ),
							  height0 * sinf( lat ) * sinf( lon0 ),
							  height0 * cosf( lat ) };
		float Vertex10[3] = { height0 * sinf( lat ) * cosf( lon1 ),
							  height0 * sinf( lat ) * sinf( lon1 ),
							  height0 * cosf( lat ) };
		float Vertex11[3] = { height1 * sinf( lat ) * cosf( lon1 ),
							  height1 * sinf( lat ) * sinf( lon1 ),
							  height1 * cosf( lat ) };
		float Vertex01[3] = { height1 * sinf( lat ) * cosf( lon0 ),
							  height1 * sinf( lat ) * sinf( lon0 ),
							  height1 * cosf( lat ) };

		glTexCoord3fv( TexCoord00 );
		glVertex3fv( Vertex00 );
		glTexCoord3fv( TexCoord10 );
		glVertex3fv( Vertex10 );
		glTexCoord3fv( TexCoord11 );
		glVertex3fv( Vertex11 );
		glTexCoord3fv( TexCoord01 );
		glVertex3fv( Vertex01 );
	}
	glEnd();
	glDisable( GL_TEXTURE_3D );
	glPopMatrix();
}

void VolumeRender::drawSphereXYPlane( float slicePos )
{
	if ( slicePos < 0.0 ) {
		slicePos = 0.0;
	} else if ( slicePos > 1.0 ) {
		slicePos = 1.0;
	}
	float height = static_cast< float >( ( EARTH_OBJECT_RADIUS
		* ( grads.getGridZMin() + ( grads.getGridZMax() - grads.getGridZMin() ) * slicePos )
		/ EARTH_RADIUS_WE_WGS84 ) * param.getZScale() );
	if ( param.getZUnit() == ZUNIT_KILOMETER ) {
		height *= 1000.0;
	}
	height += EARTH_OBJECT_RADIUS;

	double rangeDegLat = grads.getGridYMax() - grads.getGridYMin();
	double rangeDegLon = grads.getGridXMax() - grads.getGridXMin();
	double rangeRadLat = rangeDegLat / 180.0f * M_PI;
	double rangeRadLon = rangeDegLon / 180.0f * M_PI;
	int    divLat = ( int )( rangeDegLat / 1.5f );
	int    divLon = ( int )( rangeDegLon / 1.5f );
	double dRadLat = rangeRadLat / ( double )divLat;
	double dRadLon = rangeRadLon / ( double )divLon;
	float  dTexLat = 1.0f / ( float )divLat;
	float  dTexLon = 1.0f / ( float )divLon;

	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	glPushMatrix();
	glEnable( GL_TEXTURE_3D );
	glBindTexture( GL_TEXTURE_3D, texture3d.getTexture() );
	glBegin( GL_QUADS );
	float radLatitude = ( 90.0 - grads.getGridYMax() ) * M_PI / 180.0;
	for ( int vi = 0; vi < divLat; vi++ )
	{
		const float lat0 = radLatitude + dRadLat *   vi;
		const float lat1 = radLatitude + dRadLat * ( vi + 1 );
		const float texLat0 = 1.0 - dTexLat *   vi;
		const float texLat1 = 1.0 - dTexLat * ( vi + 1 );
		float radLongitude = grads.getGridXMin() * M_PI / 180.0;
		for ( int ui = 0; ui < divLon; ui++ )
		{
			const float lon0 = radLongitude + dRadLon *   ui;
			const float lon1 = radLongitude + dRadLon * ( ui + 1 );
			const float texLon0 = dTexLon *   ui;
			const float texLon1 = dTexLon * ( ui + 1 );

			float TexCoord00[3] = { texLon0, texLat0, slicePos };
			float TexCoord10[3] = { texLon1, texLat0, slicePos };
			float TexCoord11[3] = { texLon1, texLat1, slicePos };
			float TexCoord01[3] = { texLon0, texLat1, slicePos };
			float Vertex00[3] = { height * sinf( lat0 ) * cosf( lon0 ),
								  height * sinf( lat0 ) * sinf( lon0 ),
								  height * cosf( lat0 ) };
			float Vertex10[3] = { height * sinf( lat0 ) * cosf( lon1 ),
								  height * sinf( lat0 ) * sinf( lon1 ),
								  height * cosf( lat0 ) };
			float Vertex11[3] = { height * sinf( lat1 ) * cosf( lon1 ),
								  height * sinf( lat1 ) * sinf( lon1 ),
								  height * cosf( lat1 ) };
			float Vertex01[3] = { height * sinf( lat1 ) * cosf( lon0 ),
								  height * sinf( lat1 ) * sinf( lon0 ),
								  height * cosf( lat1 ) };

			glTexCoord3fv( TexCoord00 );
			glVertex3fv( Vertex00 );
			glTexCoord3fv( TexCoord10 );
			glVertex3fv( Vertex10 );
			glTexCoord3fv( TexCoord11 );
			glVertex3fv( Vertex11 );
			glTexCoord3fv( TexCoord01 );
			glVertex3fv( Vertex01 );
		}
	}
	glEnd();
	glDisable( GL_TEXTURE_3D );
	glPopMatrix();
}
