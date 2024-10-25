//
// Volume Data Visualizer for Google Earth
//
// fileinfo_panel.h
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

#ifndef FILEINFO_PANEL_H
#define FILEINFO_PANEL_H

#include <QWidget>
#include <QtGui>
#include "gradsinfo.h"
#include "worldmap.h"

class FileInfoPanel : public QWidget
{
	Q_OBJECT

public:
	FileInfoPanel( QWidget *parent = 0 );

private:
	void makeWidget();

	QGroupBox *m_GroupBox;
	GrADSInfo *m_GrADSInfo;
	WorldMap  *m_WorldMap;

public slots:
	void setGrads();
};

#endif // FILEINFO_PANEL_H
