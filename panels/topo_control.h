//
// Volume Data Visualizer for Google Earth
//
// topo_control.h
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

#ifndef TOPO_CONTROL_H
#define TOPO_CONTROL_H

#include <QWidget>
#include <QtGui>
#include "../src/define.h"
#include "../src/grads.h"
#include "../src/parameter.h"

extern Grads grads;
extern Parameter param;

class TopoControl : public QWidget
{
	Q_OBJECT

public:
	explicit TopoControl( QWidget *parent = 0 );

	QGroupBox* getControlPanel() { return m_GroupBox; }
	void setInitialValue();
	void setDisabled( bool );

private:
	enum DataType {
		CELL_CENTERED,
		GRID_CENTERED
	};
	void makeWidget();
	bool isUsableData( int, int, DataType );
	void setDisabledSettings( bool );

	double   m_North;
	double   m_South;
	double   m_West;
	double   m_East;

	QGroupBox    *m_GroupBox;
	QGroupBox    *m_GroupBox_Scale;
	QCheckBox    *m_CheckBox_Topo;
	QCheckBox    *m_CheckBox_SeaSurface;
	QCheckBox    *m_CheckBox_SyncScale;
	QLabel       *m_Label_GroundScale;
	QLabel       *m_Label_SeaFloorScale;
	QLabel       *m_Label_GroundScaleValue;
	QLabel       *m_Label_SeaFloorScaleValue;
	QSlider      *m_HSlider_GroundScale;
	QSlider      *m_HSlider_SeaFloorScale;
	QButtonGroup *m_ButtonGroup;
	QRadioButton *m_RadioButton_ETOPO5;
	QRadioButton *m_RadioButton_ETOPO2;
	QRadioButton *m_RadioButton_ETOPO1;
	QRadioButton *m_RadioButton_NONE;
	QPushButton  *m_Button_SyncScale;

	bool m_ExistETOPO5;
	bool m_ExistETOPO2;
	bool m_ExistETOPO1;
	bool m_ExistTopoData;

signals:
	void valueChanged( ValueType );
	void toggledStateTopo( bool state );

private slots:
	void setStateTopo( bool );
	void setStateSeaSurface( bool );
	void setStateSyncScale( bool );
	void setGroundScale( int );
	void setSeaFloorScale( int );
	void setTopoType( int );
};

#endif // TOPO_CONTROL_H
