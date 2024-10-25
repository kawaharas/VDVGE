//
// Volume Data Visualizer for Google Earth
//
// misc_panel.h
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

#ifndef MISC_PANEL_H
#define MISC_PANEL_H

#include <QWidget>
#include <QtGui>
#include "effect_control.h"
#include "boundingbox_control.h"
#include "colorbar_control.h"
#include "logo_control.h"

class MiscPanel : public QWidget
{
	Q_OBJECT

public:
	explicit MiscPanel( QWidget *parent = 0 );

private:
	void makeWidget();

	EffectControl       *m_EffectControl;
	BoundingBoxControl  *m_BoundingBoxControl;
	ColorBarControl     *m_ColorBarControl;
	LogoControl         *m_LogoControl;

signals:
//	void logoChanged(); // This signal use to set logo texture data
	void valueChanged( ValueType );

public slots:
	void setGrads();
};

#endif // MISC_PANEL_H
