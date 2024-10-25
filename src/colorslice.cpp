//
// Volume Data Visualizer for Google Earth
//
// colorslice.cpp
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

#include "colorslice.h"

void ColorSlice::draw()
{
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	if ( param.IsColorSliceAlpha() || param.getColorSliceOpacity() != 1.0 ) {
		glAlphaFunc( GL_GREATER, 0.0 );
	} else {
		glAlphaFunc( GL_EQUAL, 1.0 );
	}
	glEnable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glEnable( GL_DEPTH_TEST ); // added from 1.1.6
	glDisable( GL_LIGHTING );

	if ( param.IsColorSliceAlpha() ) {
		glBindTexture( GL_TEXTURE_3D, texture3d.getTexture() );
	} else {
		glBindTexture( GL_TEXTURE_3D, texture3d.getTextureRGB() );
	}
	glEnable( GL_TEXTURE_3D );
	glColor4f( 1.0, 1.0, 1.0, param.getColorSliceOpacity() );
	if ( param.IsColorSliceX() == true ) {
		float dx = param.getColorSlicePosX();
		glBegin( GL_QUADS );
		glTexCoord3f( 0.0 + dx,  0.0,  0.0 );
		glVertex3f(  -0.5 + dx, -0.5, -0.5 );
		glTexCoord3f( 0.0 + dx,  0.0,  1.0 );
		glVertex3f(  -0.5 + dx, -0.5,  0.5 );
		glTexCoord3f( 0.0 + dx,  1.0,  1.0 );
		glVertex3f(  -0.5 + dx,  0.5,  0.5 );
		glTexCoord3f( 0.0 + dx,  1.0,  0.0 );
		glVertex3f(  -0.5 + dx,  0.5, -0.5 );
		glEnd();
	}
	if ( param.IsColorSliceY() == true ) {
		float dy = param.getColorSlicePosY();
		glBegin( GL_QUADS );
		glTexCoord3f( 0.0,  0.0 + dy,  0.0 );
		glVertex3f(  -0.5, -0.5 + dy, -0.5 );
		glTexCoord3f( 1.0,  0.0 + dy,  0.0 );
		glVertex3f(   0.5, -0.5 + dy, -0.5 );
		glTexCoord3f( 1.0,  0.0 + dy,  1.0 );
		glVertex3f(   0.5, -0.5 + dy,  0.5 );
		glTexCoord3f( 0.0,  0.0 + dy,  1.0 );
		glVertex3f(  -0.5, -0.5 + dy,  0.5 );
		glEnd();
	}
	if ( param.IsColorSliceZ() == true ) {
		float dz = param.getColorSlicePosZ();
		glBegin( GL_QUADS );
		glTexCoord3f( 0.0,  0.0,  0.0 + dz );
		glVertex3f(  -0.5, -0.5, -0.5 + dz );
		glTexCoord3f( 1.0,  0.0,  0.0 + dz );
		glVertex3f(   0.5, -0.5, -0.5 + dz );
		glTexCoord3f( 1.0,  1.0,  0.0 + dz );
		glVertex3f(   0.5,  0.5, -0.5 + dz );
		glTexCoord3f( 0.0,  1.0,  0.0 + dz );
		glVertex3f(  -0.5,  0.5, -0.5 + dz );
		glEnd();
	}
	glDisable( GL_TEXTURE_3D );
	glBindTexture( GL_TEXTURE_3D, 0 );

	glDisable( GL_ALPHA_TEST );
	glDisable( GL_BLEND );
	glEnable( GL_LIGHTING );
}

void ColorSlice::drawSphereData()
{
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	if ( param.IsColorSliceAlpha() || param.getColorSliceOpacity() != 1.0 ) {
		glAlphaFunc( GL_GREATER, 0.0 );
	} else {
		glAlphaFunc( GL_EQUAL, 1.0 );
	}
	glEnable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );
	glDisable( GL_LIGHTING );

	if ( param.IsColorSliceAlpha() ) {
		glBindTexture( GL_TEXTURE_3D, texture3d.getTexture() );
	} else {
		glBindTexture( GL_TEXTURE_3D, texture3d.getTextureRGB() );
	}
	glEnable( GL_TEXTURE_3D );
	glColor4f( 1.0, 1.0, 1.0, param.getColorSliceOpacity() );
	if ( param.IsColorSliceX() ) {
		drawSphereYZPlane( param.getColorSlicePosX() );
	}
	if ( param.IsColorSliceY() ) {
		drawSphereXZPlane( param.getColorSlicePosY() );
	}
	if ( param.IsColorSliceZ() ) {
		drawSphereXYPlane( param.getColorSlicePosZ() );
	}
	glDisable( GL_TEXTURE_3D );
	glBindTexture( GL_TEXTURE_3D, 0 );

	glDisable( GL_ALPHA_TEST );
	glDisable( GL_BLEND );
	glEnable( GL_LIGHTING );
}

void ColorSlice::drawSphereYZPlane( float slicePos )
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

	glPushMatrix();
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
	glPopMatrix();
}

void ColorSlice::drawSphereXZPlane( float slicePos )
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

	glPushMatrix();
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
	glPopMatrix();
}

void ColorSlice::drawSphereXYPlane( float slicePos )
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

	glPushMatrix();
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
	glPopMatrix();
}
