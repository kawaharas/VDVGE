//
// Volume Data Visualizer for Google Earth
//
// define.h
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

#ifndef DEFINE_H
#define DEFINE_H

#define EARTH_RADIUS_NS_WGS84 6356752.3142
#define EARTH_RADIUS_WE_WGS84 6378137.0

#define EARTH_OBJECT_RADIUS 0.5

const int TEXT_MARGIN = 5;

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

enum ValueType {
	CHANGE_VIEWER_SIZE,
	CHANGE_COLORTABLE,
	CHANGE_DATARANGE,
	CHANGE_COLORBAR,
	CHANGE_CONTOUR_LINE_PARAM,
	CHANGE_LOGO_IMAGE,
	CHANGE_TOPO_STATE,
	CHANGE_TOPO_DATASET,
	CHANGE_TOPO_SCALE,
	CHANGE_COASTLINE_FILE
};

#endif // DEFINE_H
