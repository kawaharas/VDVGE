//
// Volume Data Visualizer for Google Earth
//
// control_panel.cpp
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

#include "control_panel.h"

ControlPanel::ControlPanel( QWidget *parent ) :
	QTabWidget( parent )
{
	makeWidget();
}

void ControlPanel::makeWidget()
{
	m_ColorTablePanel  = new GradientWidget();
	m_PreferencesPanel = new PreferencesPanel();
	m_FileInfoPanel    = new FileInfoPanel();
	m_ColorTablePanel->setMaximumHeight( 640 );
	m_ColorTablePanel->setContentsMargins( 9, 9, 9, 9 );
	m_PreferencesPanel->setContentsMargins( 9, 9, 9, 9 );
	m_FileInfoPanel->setContentsMargins( 9, 9, 9, 9 );

	QSizePolicy policy( QSizePolicy::Preferred, QSizePolicy::Expanding );
	policy.setHorizontalStretch( 0 );
	policy.setVerticalStretch( 0 );
	setSizePolicy( policy );
//	setMinimumSize( 464, 600 );
//	setMaximumSize( 464, 16777215 );
	setFixedWidth( 464 );
	addTab( m_ColorTablePanel, tr( "ColorTable Editor" ) );
	addTab( m_PreferencesPanel, tr( "Preferences" ) );
	addTab( m_FileInfoPanel, tr( "File Information" ) );
	setCurrentIndex( 0 );
/*
	connect( m_ColorTablePanel, SIGNAL( setColorTableToParamSucceeded() ),
			 ui->widget_Viewer, SLOT( setColorTable() ) );
*/
	connect( m_ColorTablePanel, SIGNAL( valueChanged( ValueType ) ),
			 this, SIGNAL( valueChanged( ValueType ) ) );
	connect( m_PreferencesPanel, SIGNAL( valueChanged( ValueType ) ),
			 this, SIGNAL( valueChanged( ValueType ) ) );
	connect( m_PreferencesPanel, SIGNAL( valueChanged( ValueType ) ),
			 m_ColorTablePanel, SLOT( setScaleMarkings() ) );
}

void ControlPanel::initialize()
{
	m_ColorTablePanel->setDefault01();
	m_PreferencesPanel->setGrads();
	m_FileInfoPanel->setGrads();
//	m_ColorTablePanel->getEditor()->getColorControlPointFromParam();
}
