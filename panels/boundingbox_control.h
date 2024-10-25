//
// Volume Data Visualizer for Google Earth
//
// boundingbox_control.h
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

#ifndef BOUNDINGBOX_CONTROL_H
#define BOUNDINGBOX_CONTROL_H

#include <QWidget>
#include <QtGui>
#include "../src/parameter.h"

extern Parameter param;

class BoundingBoxControl : public QWidget
{
	Q_OBJECT

public:
	explicit BoundingBoxControl( QWidget *parent = 0 );

	QGroupBox* getControlPanel() { return m_GroupBox; }
	void setInitialValue();

public slots:
	void setDisabled( bool );

private:
	void makeWidget();

	QGroupBox   *m_GroupBox;
	QCheckBox   *m_CheckBox_AxisLabel;

private slots:
	void setStateAxisLabel( bool );
};

#endif // BOUNDINGBOX_CONTROL_H
