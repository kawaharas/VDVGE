//
// Volume Data Visualizer for Google Earth
//
// colorslice_control.h
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

#ifndef COLORSLICE_CONTROL_H
#define COLORSLICE_CONTROL_H

#include <QWidget>
#include <QtGui>
#include "../src/grads.h"
#include "../src/parameter.h"

extern Grads grads;
extern Parameter param;

class ColorSliceControl : public QWidget
{
    Q_OBJECT

public:
	explicit ColorSliceControl( QWidget *parent = 0 );

	QGroupBox* getControlPanel() { return m_GroupBox; }
	void setInitialValue();
	void setDisabled( bool );

private:
	void makeWidget();

	QGroupBox *m_GroupBox;
	QCheckBox *m_CheckBox_ColorSliceX;
	QCheckBox *m_CheckBox_ColorSliceY;
	QCheckBox *m_CheckBox_ColorSliceZ;
	QCheckBox *m_CheckBox_ColorSliceAlpha;
	QCheckBox *m_CheckBox_ClampToGround;
	QLineEdit *m_LineEdit_XValue;
	QLineEdit *m_LineEdit_YValue;
	QLineEdit *m_LineEdit_ZValue;
	QSlider   *m_HSlider_ColorSliceX;
	QSlider   *m_HSlider_ColorSliceY;
	QSlider   *m_HSlider_ColorSliceZ;
	QSlider   *m_HSlider_ColorSliceOpacity;

private slots:
	void setStateColorSliceX( bool );
	void setStateColorSliceY( bool );
	void setStateColorSliceZ( bool );
	void setStateColorSliceAlpha( bool );
	void setStateClampToGround( bool );
	void setColorSliceXAxisLabel( int );
	void setColorSliceYAxisLabel( int );
	void setColorSliceZAxisLabel( int );
	void setColorSliceOpacity( int );
	void setColorSliceXValue();
	void setColorSliceYValue();
	void setColorSliceZValue();

protected:
	QString m_String_XValue_Backup;
	QString m_String_YValue_Backup;
	QString m_String_ZValue_Backup;
	bool eventFilter( QObject *obj, QEvent *event );
};

#endif // COLORSLICE_CONTROL_H
