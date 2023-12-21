//
// Volume Data Visualizer for Google Earth
//
// font_control.h
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

#ifndef FONT_CONTROL_H
#define FONT_CONTROL_H

#include <QDialog>
#include <QtGui>
#include "../src/define.h"
#include "../src/parameter.h"
#include "../src/app_parameter.h"

extern Parameter param;
extern AppParameter appParam;

class FontControl : public QDialog
{
	Q_OBJECT

public:
	explicit FontControl( QWidget *parent = 0 );
	~FontControl();

	void popup();

private:
	enum Type {
		ColorBar,
		TimeDisplay
	};

	void makeWidget();

	QFontComboBox *m_FontComboBox_ColorBarString;
	QFontComboBox *m_FontComboBox_ColorBarValue;
	QFontComboBox *m_FontComboBox_TimeDisplay;
	QSpinBox      *m_SpinBox_FontSize_ColorBarString;
	QSpinBox      *m_SpinBox_FontSize_ColorBarValue;
	QSpinBox      *m_SpinBox_FontSize_TimeDisplay;
	QPushButton   *m_Button_ColorBarString_Bold;
	QPushButton   *m_Button_ColorBarString_Italic;
	QPushButton   *m_Button_ColorBarValue_Bold;
	QPushButton   *m_Button_ColorBarValue_Italic;
	QPushButton   *m_Button_TimeDisplay_Bold;
	QPushButton   *m_Button_TimeDisplay_Italic;

signals:
	void valueChanged( ValueType );

private slots:
	void setFont();
};

#endif // FONT_CONTROL_H
