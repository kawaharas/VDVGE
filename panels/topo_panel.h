//
// Volume Data Visualizer for Google Earth
//
// topo_panel.h
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

#ifndef TOPO_PANEL_H
#define TOPO_PANEL_H

#include <QWidget>
#include <QtGui>
#include "topo_control.h"
#include "coastline_control.h"

class TopoPanel : public QWidget
{
	Q_OBJECT

public:
	explicit TopoPanel( QWidget *parent = 0 );

private:
	void makeWidget();

	TopoControl       *m_TopoControl;
	CoastlineControl  *m_CoastlineControl;

signals:
	void valueChanged( ValueType );

public slots:
	void setGrads();
};

#endif // TOPO_PANEL_H
