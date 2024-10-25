//
// Volume Data Visualizer for Google Earth
//
// contourline_control.h
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

#ifndef CONTOURLINE_CONTROL_H
#define CONTOURLINE_CONTROL_H

#include <QWidget>
#include <QtGui>
#include "../src/define.h"
#include "../src/grads.h"
#include "../src/parameter.h"

extern Grads grads;
extern Parameter param;

class ContourLineControl : public QWidget
{
	Q_OBJECT

public:
	explicit ContourLineControl( QWidget *parent = 0 );
	~ContourLineControl();

	QGroupBox* getControlPanel() { return m_GroupBox; }
	void setInitialValue();
	void setDisabled( bool );
	void changeThreshold();

private:
	void makeWidget();

	QGroupBox   *m_GroupBox;
	QCheckBox   *m_CheckBox_ContourLineX;
	QCheckBox   *m_CheckBox_ContourLineY;
	QCheckBox   *m_CheckBox_ContourLineZ;
	QLineEdit   *m_LineEdit_XValue;
	QLineEdit   *m_LineEdit_YValue;
	QLineEdit   *m_LineEdit_ZValue;
	QSlider     *m_HSlider_ContourLineX;
	QSlider     *m_HSlider_ContourLineY;
	QSlider     *m_HSlider_ContourLineZ;
	QLineEdit   *m_LineEdit_Min;
	QLineEdit   *m_LineEdit_Max;
	QPushButton *m_Button_ResetMin;
	QPushButton *m_Button_ResetMax;
	QLineEdit   *m_LineEdit_Interval;
	QSpinBox    *m_SpinBox_LineWidth;

signals:
	void valueChanged( ValueType );

private slots:
	void setStateContourLineX( bool );
	void setStateContourLineY( bool );
	void setStateContourLineZ( bool );
	void setContourLineXAxisLabel( int );
	void setContourLineYAxisLabel( int );
	void setContourLineZAxisLabel( int );
	void setContourLineXValue();
	void setContourLineYValue();
	void setContourLineZValue();
	void setMin();
	void setMax();
	void resetMin();
	void resetMax();
	void setInterval();
	void setLineWidth( int );

protected:
	QString m_String_XValue_Backup;
	QString m_String_YValue_Backup;
	QString m_String_ZValue_Backup;
	QString m_String_Min_Backup;
	QString m_String_Max_Backup;
	QString m_String_Interval_Backup;
	bool eventFilter( QObject *obj, QEvent *event );
};

#endif // CONTOURLINE_CONTROL_H
