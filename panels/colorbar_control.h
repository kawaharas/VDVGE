//
// Volume Data Visualizer for Google Earth
//
// colorbar_control.h
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

#ifndef COLORBAR_CONTROL_H
#define COLORBAR_CONTROL_H

#include <QWidget>
#include <QtGui>
#include "../src/define.h"
#include "../src/parameter.h"

extern Parameter param;

class ColorBarControl : public QWidget
{
	Q_OBJECT

public:
	explicit ColorBarControl( QWidget *parent = 0 );

	QGroupBox* getControlPanel() { return m_GroupBox; }
	void setInitialValue();

signals:
	void valueChanged( ValueType );

public slots:
	void setDisabled( bool );

private:
	void makeWidget();

	QGroupBox    *m_GroupBox;
	QRadioButton *m_RadioButton_Vertical;
	QRadioButton *m_RadioButton_Horizontal;
	QSpinBox     *m_SpinBox_DecimalPlacesNum;
	QSpinBox     *m_SpinBox_MajorTickMarkNum;
	QSpinBox     *m_SpinBox_MinorTickMarkNum;
	QComboBox    *m_ComboBox_MajorTickMarkType;
	QComboBox    *m_ComboBox_MinorTickMarkType;
	QRadioButton *m_RadioButton_RGB;
	QRadioButton *m_RadioButton_RGBA;
	QRadioButton *m_RadioButton_RGB_AND_RGBA;

private slots:
	void setColorBarType();
	void setColorBarDirection();
	void setDecimalPlacesNum( int );
	void setMajorTickMarkNum( int );
	void setMinorTickMarkNum( int );
	void setMajorTickMarkType( int );
	void setMinorTickMarkType( int );
};

#endif // COLORBAR_CONTROL_H
