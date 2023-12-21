//
// Volume Data Visualizer for Google Earth
//
// interpolator.cpp
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

#include "interpolator.h"

void interpolator( int width, int height, int depth, uchar *data )
{
	int sx = grads.getGridSizeX();
	int sy = grads.getGridSizeY();
	int sz = grads.getGridSizeZ();

	float *orgdata = grads.getData();

	double *coord_x = new double[ width  ];
	double *coord_y = new double[ height ];
	double *coord_z = new double[ depth  ];
	int *idx_x   = new int[ width  ];
	int *idx_y   = new int[ height ];
	int *idx_z   = new int[ depth  ];

	double dx, dy, dz;

	if ( width != 1 ) {
		dx = ( grads.getGridXMax() - grads.getGridXMin() ) / static_cast< double >( width  - 1 );
	} else {
		dx = 0.0;
	}
	if ( height != 1 ) {
		dy = ( grads.getGridYMax() - grads.getGridYMin() ) / static_cast< double >( height - 1 );
	} else {
		dy = 0.0;
	}
	if ( depth != 1 ) {
		dz = ( grads.getGridZMax() - grads.getGridZMin() ) / static_cast< double >( depth  - 1 );
	} else {
		dz = 0.0;
	}

	for ( int i = 0; i < width; i++ ) {
		coord_x[ i ] = grads.getGridXMin() + dx * static_cast< double >( i );
	}
	for ( int i = 0; i < height; i++ ) {
		coord_y[ i ] = grads.getGridYMin() + dy * static_cast< double >( i );
	}
	for ( int i = 0; i < depth; i++ ) {
		coord_z[ i ] = grads.getGridZMin() + dz * static_cast< double >( i );
	}

	idx_x[0] = 0;
	for ( int i = 1; i < width; i++ ) {
		for ( int j = 0; j < sx; j++ ) {
			if ( coord_x[ i ] <= grads.getCoordX( j ) ) {
				idx_x[ i ] = j - 1;
				break;
			}
		}
	}
	idx_x[ width - 1 ] = sx - 2;

	idx_y[0] = 0;
	for ( int i = 1; i < height; i++ ) {
		for ( int j = 0; j < sy; j++ ) {
			if ( coord_y[ i ] <= grads.getCoordY( j ) ) {
				idx_y[ i ] = j - 1;
				break;
			}
		}
	}
	idx_y[ height - 1 ] = sy - 2;

	if ( depth == 1 ) {
		idx_z[ 0 ] = 0;
	} else {
		idx_z[0] = 0;
		for ( int i = 1; i < depth; i++ ) {
			for ( int j = 0; j < sz; j++ ) {
				if ( coord_z[ i ] <= grads.getCoordZ( j ) ) {
					idx_z[ i ] = j - 1;
					break;
				}
			}
		}
		idx_z[ depth - 1 ] = sz - 2;
	}

	int xa, xb;
	int ya, yb;
	int za, zb;
	double x0, x1, y0, y1, z0, z1;
	double v0, v1, v2, v3, v4, v5, v6, v7;
	double p, q, r;
	double ans;

	for ( int k = 0; k < depth; k++ ) {
		for ( int j = 0; j < height; j++ ) {
			for ( int i = 0; i < width; i++ ) {
				xa = idx_x[ i ];
				xb = xa + 1;
				ya = idx_y[ j ];
				yb = ya + 1;
				if ( depth == 1 ) {
					za = idx_z[ k ];
					zb = za;
				} else {
					za = idx_z[ k ];
					zb = za + 1;
				}
				x0 = grads.getCoordX( xa );
				x1 = grads.getCoordX( xb );
				y0 = grads.getCoordY( ya );
				y1 = grads.getCoordY( yb );
				z0 = grads.getCoordZ( za );
				z1 = grads.getCoordZ( zb );

				v0 = static_cast< double >( orgdata[ ( za * sx * sy ) + ( ya * sx ) + xa ] );
				v1 = static_cast< double >( orgdata[ ( za * sx * sy ) + ( ya * sx ) + xb ] );
				v2 = static_cast< double >( orgdata[ ( zb * sx * sy ) + ( ya * sx ) + xa ] );
				v3 = static_cast< double >( orgdata[ ( zb * sx * sy ) + ( ya * sx ) + xb ] );
				v4 = static_cast< double >( orgdata[ ( za * sx * sy ) + ( yb * sx ) + xa ] );
				v5 = static_cast< double >( orgdata[ ( za * sx * sy ) + ( yb * sx ) + xb ] );
				v6 = static_cast< double >( orgdata[ ( zb * sx * sy ) + ( yb * sx ) + xa ] );
				v7 = static_cast< double >( orgdata[ ( zb * sx * sy ) + ( yb * sx ) + xb ] );

				float min   = param.getVolumeThresholdMin();
				float max   = param.getVolumeThresholdMax();
				float undef = grads.getUndef();
				float ansf;

				if ( ( v0 == undef ) || ( v1 == undef ) ||
					 ( v2 == undef ) || ( v3 == undef ) ||
					 ( v4 == undef ) || ( v5 == undef ) ||
					 ( v6 == undef ) || ( v7 == undef ) ) {
					ansf = undef;
				} else {
					if ( x0 == x1 ) {
						p = 0.0;
					} else {
						p = ( coord_x[ i ] - x0 ) / ( x1 - x0 );
					}
					if ( y0 == y1 ) {
						q = 0.0;
					} else {
						q = ( coord_y[ j ] - y0 ) / ( y1 - y0 );
					}
					if ( z0 == z1 ) {
						r = 0.0;
					} else {
						r = ( coord_z[ k ] - z0 ) / ( z1 - z0 );
					}
					ans = 0.0;
					ans += v0 * ( 1 - p ) * ( 1 - q ) * ( 1 - r );
					ans += v1 *     p     * ( 1 - q ) * ( 1 - r );
					ans += v2 * ( 1 - p ) * ( 1 - q ) *     r    ;
					ans += v3 *     p     * ( 1 - q ) *     r    ;
					ans += v4 * ( 1 - p ) *     q     * ( 1 - r );
					ans += v5 *     p     *     q     * ( 1 - r );
					ans += v6 * ( 1 - p ) *     q     *     r    ;
					ans += v7 *     p     *     q     *     r    ;
					ansf  = static_cast< float >( ans );
				}

				if ( ansf == undef )  {
					data[ k * height * width + j * width + i ] = 255;
				} else {
					if ( ansf < min ) {
						data[ k * height * width + j * width + i ] = 0;
					} else if ( ansf > max ) {
						data[ k * height * width + j * width + i ] = 254;
					} else {
						if ( !param.IsLogScale() ) {
							data[ k * height * width + j * width + i ] =
								static_cast< uchar >( ( ansf - min ) / ( max - min ) * 254.0f );
						} else {
							data[ k * height * width + j * width + i ] =
								static_cast< uchar >( ( log10( ansf ) - log10( min ) ) /
													  ( log10( max ) - log10( min ) ) * 254.0f );
						}
					}
				}

			}
		}
	}

	delete [] coord_x;
	delete [] coord_y;
	delete [] coord_z;
	delete [] idx_x;
	delete [] idx_y;
	delete [] idx_z;
}
