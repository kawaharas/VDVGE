//
// Volume Data Visualizer for Google Earth
//
// kml_control.h
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

#ifndef KML_CONTROL_H
#define KML_CONTROL_H

#include <QDialog>
#include <QtGui>
#include "../src/parameter.h"

extern Parameter param;

class KMLControl : public QDialog
{
	Q_OBJECT

public:
	explicit KMLControl( QWidget *parent = 0 );
	~KMLControl();

	void popup();

private:
	void makeWidget();

	QLineEdit *m_LineEdit_X;
	QLineEdit *m_LineEdit_Y;
	QLineEdit *m_LineEdit_Z;
	QPushButton *m_Button_Accept;
	QPushButton *m_Button_Cancel;

public slots:
	void setValue();
};

#endif // KML_CONTROL_H
