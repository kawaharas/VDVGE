//
// Volume Data Visualizer for Google Earth
//
// viewer_control.h
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

#ifndef VIEWER_CONTROL_H
#define VIEWER_CONTROL_H

#include <QDialog>
#include <QtGui>
#include "../src/define.h"
#include "../src/parameter.h"

extern Parameter param;

class ViewerControl : public QDialog
{
	Q_OBJECT

public:
	explicit ViewerControl( QWidget *parent = 0 );
	~ViewerControl();

	void popup();

private:
	void makeWidget();

	QLineEdit *m_LineEdit_Width;
	QLineEdit *m_LineEdit_Height;
	QPushButton *m_Button_Accept;
	QPushButton *m_Button_Cancel;

signals:
	void valueChanged( ValueType );

public slots:
	void setSize();
};

#endif // VIEWER_CONTROL_H
