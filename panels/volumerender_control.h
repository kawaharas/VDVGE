//
// Volume Data Visualizer for Google Earth
//
// volumerender_control.h
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

#ifndef VOLUMERENDER_CONTROL_H
#define VOLUMERENDER_CONTROL_H

#include <QWidget>
#include <QtGui>
#include "../src/grads.h"
#include "../src/parameter.h"

extern Grads grads;
extern Parameter param;

class VolumeRenderControl : public QWidget
{
	Q_OBJECT

public:
	explicit VolumeRenderControl( QWidget *parent = 0 );

	QGroupBox* getControlPanel() { return m_GroupBox; }
	void setInitialValue();
	void setDisabled( bool );

private:
	void makeWidget();

	QGroupBox *m_GroupBox;
	QCheckBox *m_CheckBox_XSlice;
	QCheckBox *m_CheckBox_YSlice;
	QCheckBox *m_CheckBox_ZSlice;
	QLabel    *m_Label_XSlice;
	QLabel    *m_Label_YSlice;
	QLabel    *m_Label_ZSlice;
	QSlider   *m_HSlider_XSlice;
	QSlider   *m_HSlider_YSlice;
	QSlider   *m_HSlider_ZSlice;

private slots:
	void setStateXSlice( bool );
	void setStateYSlice( bool );
	void setStateZSlice( bool );
	void setSliceNumX( int );
	void setSliceNumY( int );
	void setSliceNumZ( int );
};

#endif // VOLUMERENDER_CONTROL_H
