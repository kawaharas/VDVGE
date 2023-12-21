//
// Volume Data Visualizer for Google Earth
//
// timerange_control.h
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

#ifndef TIMERANGE_CONTROL_H
#define TIMERANGE_CONTROL_H

#include <QWidget>
#include <QtGui>
#include "../src/grads.h"
#include "../src/parameter.h"
#include "../src/util.h"

extern Grads grads;
extern Parameter param;

class TimeRangeControl : public QWidget
{
	Q_OBJECT

public:
	explicit TimeRangeControl( QWidget *parent = 0 );
	~TimeRangeControl();

	QGroupBox* getControlPanel() { return m_GroupBox; }
	void setInitialValue();
	void setDisabled( bool );

private:
	void makeWidget();

	QGroupBox     *m_GroupBox;
	QLineEdit     *m_LineEdit_Begin;
	QLineEdit     *m_LineEdit_End;
	QLabel        *m_Label_BeginStrings;
	QLabel        *m_Label_EndStrings;
	QLineEdit     *m_LineEdit_Skip;
	QCheckBox     *m_CheckBox_TimeConversionToUTC;
	QLineEdit     *m_LineEdit_TimeConversionToUTC;

public slots:
	void setBegin();
	void setEnd();
	void setSkip();
	void setTimeConversionToUTC();

private slots:
	void setStateTimeConversionToUTC( bool );

protected:
	QString m_String_Begin_Backup;
	QString m_String_End_Backup;
	QString m_String_Skip_Backup;
	QString m_String_TimeConversionToUTC_Backup;
	bool eventFilter( QObject *obj, QEvent *event );
};

#endif // TIMERANGE_CONTROL_H
