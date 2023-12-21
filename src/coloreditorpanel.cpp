//
// Volume Data Visualizer for Google Earth
//
// coloreditorpanel.cpp
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

#include "coloreditorpanel.h"

ColorEditorPanel::ColorEditorPanel( QWidget *parent ) :
	QGLWidget( parent )
{
}

void ColorEditorPanel::initializeGL()
{
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
}

void ColorEditorPanel::resizeGL( int w, int h )
{
	glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glOrtho( -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f );
	glMatrixMode( GL_MODELVIEW );
}

void ColorEditorPanel::paintGL()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void ColorEditorPanel::mousePressEvent( QMouseEvent *event )
{
	emit mouseButtonPressed();
}
