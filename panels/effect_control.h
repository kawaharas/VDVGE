//
// Volume Data Visualizer for Google Earth
//
// effect_control.h
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

#ifndef EFFECT_CONTROL_H
#define EFFECT_CONTROL_H

#include <QWidget>
#include <QtGui>
#include "../src/parameter.h"

extern Parameter param;

class EffectControl : public QWidget
{
	Q_OBJECT

public:
	explicit EffectControl( QWidget *parent = 0 );

	QGroupBox* getControlPanel() { return m_GroupBox; }
	void setInitialValue();
	void setDisabled( bool );

private:
	void makeWidget();

	QGroupBox *m_GroupBox;
	QCheckBox *m_CheckBox_BoundingBox;
	QCheckBox *m_CheckBox_ColorBar;
	QCheckBox *m_CheckBox_TimeDisplay;

private slots:
	void setStateBoundingBox( bool );
	void setStateColorBar( bool );
	void setStateTimeDisplay( bool );

signals:
	void toggledStateBoundingBox( bool state );
	void toggledStateColorBar( bool state );
	void toggledStateTimeDisplay( bool state );
};

#endif // EFFECT_CONTROL_H
