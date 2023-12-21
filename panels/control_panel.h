//
// Volume Data Visualizer for Google Earth
//
// control_panel.h
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

#ifndef CONTROL_PANEL_H
#define CONTROL_PANEL_H

#include <QWidget>
#include <QtGui>
#include "../src/gradients.h"
#include "preferences_panel.h"
#include "fileinfo_panel.h"

class ControlPanel : public QTabWidget
{
	Q_OBJECT

public:
	ControlPanel( QWidget *parent = 0 );

	void initialize();
	GradientWidget   *colorTablePanel() { return m_ColorTablePanel; }
	PreferencesPanel *preferencesPanel() { return m_PreferencesPanel; }
	FileInfoPanel    *fileInfoPanel() { return m_FileInfoPanel; }

signals:
	void valueChanged( ValueType );

//public slots:
//	void fileLoaded();

private:
	void makeWidget();

	GradientWidget   *m_ColorTablePanel;
	PreferencesPanel *m_PreferencesPanel;
	FileInfoPanel    *m_FileInfoPanel;
};

#endif // CONTROL_PANEL_H
