//
// Volume Data Visualizer for Google Earth
//
// coastline_control.h
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

#ifndef COASTLLINE_CONTROL_H
#define COASTLLINE_CONTROL_H

#include <QWidget>
#include <QtGui>
#include "../src/define.h"
#include "../src/grads.h"
#include "../src/parameter.h"

extern Grads grads;
extern Parameter param;

class CoastlineControl : public QWidget
{
	Q_OBJECT

public:
	explicit CoastlineControl( QWidget *parent = 0 );

	QGroupBox* getControlPanel() { return m_GroupBox; }
	void setInitialValue();

public slots:
	void setDisabled( bool );
	void setCoastlineFile();

signals:
	void valueChanged( ValueType );

private:
	void makeWidget();

	QGroupBox    *m_GroupBox;
	QCheckBox    *m_CheckBox_Coastline;
	QPushButton  *m_Button_SelectCoastlineFile;

private slots:
	void setStateCoastline( bool );
};

#endif // COASTLLINE_CONTROL_H
