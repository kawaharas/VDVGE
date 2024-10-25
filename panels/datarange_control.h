//
// Volume Data Visualizer for Google Earth
//
// datarange_control.h
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

#ifndef DATARANGE_CONTROL_H
#define DATARANGE_CONTROL_H

#include <QWidget>
#include <QtGui>
#include "../src/define.h"
#include "../src/grads.h"
#include "../src/parameter.h"
#include "../src/util.h"

extern Grads grads;
extern Parameter param;

class DataRangeControl : public QWidget
{
	Q_OBJECT

public:
	explicit DataRangeControl( QWidget *parent = 0 );
	~DataRangeControl();

	QGroupBox* getControlPanel() { return m_GroupBox; }
	void setInitialValue();
	void setDisabled( bool );

private:
	void makeWidget();

	QGroupBox   *m_GroupBox;
	QLineEdit   *m_LineEdit_Min;
	QLineEdit   *m_LineEdit_Max;
	QPushButton *m_Button_ResetMin;
	QPushButton *m_Button_ResetMax;
	QCheckBox   *m_CheckBox_LogScale;

signals:
	void valueChanged( ValueType );

public slots:
	void setMin();
	void setMax();
	void resetMin();
	void resetMax();
	void setStateLogScale( bool state );

protected:
	QString m_String_Min_Backup;
	QString m_String_Max_Backup;
	bool eventFilter( QObject *obj, QEvent *event );
};

#endif // DATARANGE_CONTROL_H
