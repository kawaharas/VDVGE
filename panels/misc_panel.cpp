//
// Volume Data Visualizer for Google Earth
//
// misc_panel.cpp
//
// Copyright (c) 2012-2013 Shintaro KAWAHARA (kawahara@jamstec.go.jp)
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

#include "misc_panel.h"

MiscPanel::MiscPanel( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

	connect( m_EffectControl, SIGNAL( toggledStateBoundingBox( bool ) ),
			 m_BoundingBoxControl, SLOT( setDisabled( bool ) ) );
	connect( m_EffectControl, SIGNAL( toggledStateColorBar( bool ) ),
			 m_ColorBarControl, SLOT( setDisabled( bool ) ) );
//	connect( m_LogoControl, SIGNAL( logoChanged() ),
//			 this, SIGNAL( logoChanged() ) );
	connect( m_LogoControl, SIGNAL( valueChanged( ValueType ) ),
			 this, SIGNAL( valueChanged( ValueType ) ) );
	connect( m_ColorBarControl, SIGNAL( valueChanged( ValueType ) ),
			 this, SIGNAL( valueChanged( ValueType ) ) );
}

void MiscPanel::makeWidget()
{
	m_EffectControl      = new EffectControl();
	m_BoundingBoxControl = new BoundingBoxControl();
	m_ColorBarControl    = new ColorBarControl();
	m_LogoControl        = new LogoControl();

	QVBoxLayout *vBoxLayout = new QVBoxLayout();
	vBoxLayout->addWidget( m_EffectControl->getControlPanel() );
	vBoxLayout->addWidget( m_BoundingBoxControl->getControlPanel() );
	vBoxLayout->addWidget( m_ColorBarControl->getControlPanel() );
	vBoxLayout->addWidget( m_LogoControl->getControlPanel() );
	vBoxLayout->setContentsMargins( 0, 0, 0, 0 );
	vBoxLayout->addStretch( 1 );
	setLayout( vBoxLayout );
}

void MiscPanel::setGrads()
{
	m_EffectControl->setDisabled( false );
	m_BoundingBoxControl->setDisabled( false );
	m_ColorBarControl->setDisabled( false );
	m_LogoControl->setDisabled( false );

	m_EffectControl->setInitialValue();
	m_BoundingBoxControl->setInitialValue();
	m_ColorBarControl->setInitialValue();
	m_LogoControl->setInitialValue();
}
