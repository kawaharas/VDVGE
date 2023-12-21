//
// Volume Data Visualizer for Google Earth
//
// gradsinfo.h
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

#ifndef GRADSINFO_H
#define GRADSINFO_H

#include <QWidget>
#include <QtGui>
#include "../src/grads.h"

extern Grads grads;

class GrADSInfo : public QWidget
{
	Q_OBJECT

public:
	explicit GrADSInfo( QWidget *parent = 0 );

	QGroupBox* getControlPanel() { return m_GroupBox; }
	void setInfomation();

private:
	void makeWidget();

	QGroupBox *m_GroupBox;
	QLabel    *m_Label_CtlFileString;
	QLabel    *m_Label_DataFileString;
	QLabel    *m_Label_GridXString;
	QLabel    *m_Label_GridYString;
	QLabel    *m_Label_GridZString;
	QLabel    *m_Label_GridTString;
};

#endif // GRADSINFO_H
