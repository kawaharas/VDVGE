//
// Volume Data Visualizer for Google Earth
//
// logo_control.h
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

#ifndef LOGO_CONTROL_H
#define LOGO_CONTROL_H

#include <QWidget>
#include <QtGui>
#include "../src/define.h"
#include "../src/parameter.h"

extern Parameter param;

class LogoControl : public QWidget
{
	Q_OBJECT

public:
	explicit LogoControl( QWidget *parent = 0 );

	QGroupBox* getControlPanel() { return m_GroupBox; }
	void setInitialValue();

public slots:
	void setDisabled( bool );
	void setLogoFile();

signals:
//	void logoChanged(); // This signal use to set logo texture data
	void valueChanged( ValueType );

private:
	void makeWidget();

	QGroupBox   *m_GroupBox;
	QCheckBox   *m_CheckBox_Logo;
	QPushButton *m_Button_SelectLogoFile;

private slots:
	void setStateLogo( bool );
};

#endif // LOGO_CONTROL_H
