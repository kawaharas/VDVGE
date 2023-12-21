//
// Volume Data Visualizer for Google Earth
//
// preferences_panel.cpp
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

#include "preferences_panel.h"

PreferencesPanel::PreferencesPanel( QWidget *parent ) :
	QWidget( parent )
{
	setMinimumHeight( 600 );
	makeWidget();

	connect( m_ComboBox, SIGNAL( activated( int ) ),
			 m_StackedLayout, SLOT( setCurrentIndex( int ) ) );
	connect( m_VisualizationPanel, SIGNAL( valueChanged( ValueType ) ),
			 this, SIGNAL( valueChanged( ValueType ) ) );
	connect( m_TopoPanel, SIGNAL( valueChanged( ValueType ) ),
			 this, SIGNAL( valueChanged( ValueType ) ) );
	connect( m_DataPanel, SIGNAL( valueChanged( ValueType ) ),
			 this, SIGNAL( valueChanged( ValueType ) ) );
//	connect( m_MiscPanel, SIGNAL( logoChanged() ),
//			 this, SIGNAL( logoChanged() ) );
	connect( m_MiscPanel, SIGNAL( valueChanged( ValueType ) ),
			 this, SIGNAL( valueChanged( ValueType ) ) );
}

void PreferencesPanel::makeWidget()
{
	m_VisualizationPanel  = new VisualizationPanel();
	m_DataPanel           = new DataPanel();
	m_TopoPanel           = new TopoPanel();
	m_MiscPanel           = new MiscPanel();

	m_ComboBox = new QComboBox();
	m_ComboBox->addItem( tr( "Visualization Settings" ) );
	m_ComboBox->addItem( tr( "Data Settings" ) );
	m_ComboBox->addItem( tr( "Topo Settings" ) );
	m_ComboBox->addItem( tr( "Misc Settings" ) );
	m_ComboBox->setDisabled( true );

	m_StackedLayout = new QStackedLayout();
	m_StackedLayout->addWidget( m_VisualizationPanel );
	m_StackedLayout->addWidget( m_DataPanel );
	m_StackedLayout->addWidget( m_TopoPanel );
	m_StackedLayout->addWidget( m_MiscPanel );

	QVBoxLayout *vBoxLayout = new QVBoxLayout();
	vBoxLayout->addWidget( m_ComboBox );
	vBoxLayout->addLayout( m_StackedLayout );
	setLayout( vBoxLayout );
}

void PreferencesPanel::setGrads()
{
	m_ComboBox->setDisabled( false );

	m_VisualizationPanel->setGrads();
	m_DataPanel->setGrads();
	m_TopoPanel->setGrads();
	m_MiscPanel->setGrads();
}
