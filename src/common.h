//
// Volume Data Visualizer for Google Earth
//
// common.h
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

#ifndef COMMON_H
#define COMMON_H

#if defined(__WIN32__) || defined(_WIN32)
	#include <windows.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
//	#include <GL/glext.h> // "qopenglext.h" is included instead of "glext.h"
#elif defined __APPLE__
	#include <glu.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glext.h>
#endif

#include "compatibility.h"

#endif // COMMON_H
