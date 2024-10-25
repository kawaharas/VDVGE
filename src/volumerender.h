//
// Volume Data Visualizer for Google Earth
//
// volumerender.h
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

#ifndef VOLUMERENDER_H
#define VOLUMERENDER_H

#include <QWidget>
#include <QtGui>
#include "define.h"
#include "grads.h"
#include "parameter.h"
#include "texture3d.h"
#include "colorslice.h"

extern Grads grads;
extern Parameter param;
extern Texture3D texture3d;

class VolumeRender
{
public:
	void draw();
	void drawSphereData();

private:
	void drawSphereYZPlane( float );
	void drawSphereXZPlane( float );
	void drawSphereXYPlane( float );
};

#endif // VOLUMERENDER_H
