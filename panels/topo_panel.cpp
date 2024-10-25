//
// Volume Data Visualizer for Google Earth
//
// topo_panel.cpp
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

#include "topo_panel.h"

TopoPanel::TopoPanel( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

	connect( m_TopoControl, SIGNAL( valueChanged( ValueType ) ),
			 this, SIGNAL( valueChanged( ValueType ) ) );
	connect( m_CoastlineControl, SIGNAL( valueChanged( ValueType ) ),
			 this, SIGNAL( valueChanged( ValueType ) ) );
}

void TopoPanel::makeWidget()
{
	m_TopoControl = new TopoControl();
	m_CoastlineControl = new CoastlineControl();

	QVBoxLayout *vBoxLayout = new QVBoxLayout();
	vBoxLayout->addWidget( m_TopoControl->getControlPanel() );
	vBoxLayout->addWidget( m_CoastlineControl->getControlPanel() );
	vBoxLayout->setContentsMargins( 0, 0, 0, 0 );
	vBoxLayout->addStretch( 1 );
	setLayout( vBoxLayout );
}

void TopoPanel::setGrads()
{
	m_TopoControl->setDisabled( false );
	m_TopoControl->setInitialValue();
	m_CoastlineControl->setDisabled( false );
	m_CoastlineControl->setInitialValue();
}
