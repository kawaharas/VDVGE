//
// Volume Data Visualizer for Google Earth
//
// preferences_panel.h
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

#ifndef PREFERENCES_PANEL_H
#define PREFERENCES_PANEL_H

#include <QWidget>
#include <QtGui>
#include "../src/app_parameter.h"
#include "../src/grads.h"

/*
#include "panels/visualization_panel.h"
#include "panels/data_panel.h"
#include "panels/topo_panel.h"
#include "panels/misc_panel.h"
*/
#include "visualization_panel.h"
#include "data_panel.h"
#include "topo_panel.h"
#include "misc_panel.h"

extern Grads grads;
extern AppParameter appParam;

class PreferencesPanel : public QWidget
{
	Q_OBJECT

public:
	PreferencesPanel( QWidget *parent = 0 );

private:
	void makeWidget();

	VisualizationPanel  *m_VisualizationPanel;
	DataPanel           *m_DataPanel;
	TopoPanel           *m_TopoPanel;
	MiscPanel           *m_MiscPanel;

	QComboBox           *m_ComboBox;
	QStackedLayout      *m_StackedLayout;

signals:
	void valueChanged( ValueType ); // This signal use to recalc 3-D texture data
//	void logoChanged();  // This signal use to set logo texture data

public slots:
	void setGrads();
};

#endif // PREFERENCES_PANEL_H
