//
// Volume Data Visualizer for Google Earth
//
// texture3d.h
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

#ifndef TEXTURE3D_H
#define TEXTURE3D_H

/*
#if QT_VERSION < 0x050000
#include <QOpenGLFunctions_1_4>
#endif
*/

#include "common.h"
#include "../src/define.h"
#include "../src/grads.h"
#include "../src/parameter.h"
#include "app_parameter.h"
#include "interpolator.h"

extern Grads grads;
extern Parameter param;
extern AppParameter appParam;

const GLsizei MAX_TEX3D_W = 128;
const GLsizei MAX_TEX3D_H = 128;
const GLsizei MAX_TEX3D_D =  64;

enum TextureType {
	RGBA,
	RGB
};

class Texture3D
{
public:
	Texture3D();
	~Texture3D();

	void     initialize();
	void     setData();
	void     setColorTable();
	GLuint   getTexture() { return m_Texture; }
	GLuint   getTextureRGB() { return m_TextureRGB; }
	GLubyte *getData() { return m_Data; }
	GLsizei  width()   { return m_Width; }
	GLsizei  height()  { return m_Height; }
	GLsizei  depth()   { return m_Depth; }

private:
	GLuint   m_Texture;
	GLuint   m_TextureRGB;
	GLubyte *m_Data;
	GLsizei  m_Width;
	GLsizei  m_Height;
	GLsizei  m_Depth;
	ulong    m_Size;

	GLsizei  m_MaxTex3D_W;
	GLsizei  m_MaxTex3D_H;
	GLsizei  m_MaxTex3D_D;
};

#endif // TEXTURE3D_H
