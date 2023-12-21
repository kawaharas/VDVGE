//
// Volume Data Visualizer for Google Earth
//
// app_parameter.h
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

#ifndef APP_PARAMETER_H
#define APP_PARAMETER_H

#include <QWidget>
#include <QtGui>
#include "common.h"
#include "../src/define.h"
#include "errorcode.h"

class AppParameter : public QObject
{
	Q_OBJECT

public:
	AppParameter();

	void initialize();
	bool save();
	ErrorCode load();
	bool useNativeDialog() { return m_StateNativeDialog; }
	void setStateTopo( bool state ) { m_StateTopo = state; }
	bool getStateTopo() { return m_StateTopo; }
	void setStateCoastline( bool state ) { m_StateCoastline = state; }
	bool getStateCoastline() { return m_StateCoastline; }
	GLsizei getMaxTex3DWidth()  { return m_MaxTex3D_W; }
	GLsizei getMaxTex3DHeight() { return m_MaxTex3D_H; }
	GLsizei getMaxTex3DDepth()  { return m_MaxTex3D_D; }
	QFont getFontColorBarString() { return m_FontColorBarString; }
	QFont getFontColorBarValue() { return m_FontColorBarValue; }
	QFont getFontTimeDisplay() { return m_FontTimeDisplay; }
	void setFontColorBarString( QFont font ) { m_FontColorBarString = font; }
	void setFontColorBarValue( QFont font ) { m_FontColorBarValue = font; }
	void setFontTimeDisplay( QFont font ) { m_FontTimeDisplay = font; }
	void setStateGUI( bool value ) { m_StateGUI = value; }
	bool getStateGUI() { return m_StateGUI; }

private:
	bool      m_StateGUI;

	bool      m_StateNativeDialog;
	bool      m_StateTopo;
	bool      m_StateCoastline;
	GLsizei   m_MaxTex3D_W;
	GLsizei   m_MaxTex3D_H;
	GLsizei   m_MaxTex3D_D;
	QString   m_UsableFonts;
	QFont     m_FontColorBarString;
	QFont     m_FontColorBarValue;
	QFont     m_FontTimeDisplay;
};

#endif // APP_PARAMETER_H
