//
// Volume Data Visualizer for Google Earth
//
// effect_control.cpp
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

#include "effect_control.h"

EffectControl::EffectControl( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

	connect( m_CheckBox_BoundingBox, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateBoundingBox( bool ) ) );
	connect( m_CheckBox_ColorBar, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateColorBar( bool ) ) );
	connect( m_CheckBox_TimeDisplay, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateTimeDisplay( bool ) ) );
}

void EffectControl::makeWidget()
{
	m_GroupBox = new QGroupBox();
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox->setTitle( "Visual Effects" );
	m_CheckBox_BoundingBox = new QCheckBox( "Bounding Box", 0 );
	m_CheckBox_ColorBar    = new QCheckBox( "ColorBar", 0 );
	m_CheckBox_TimeDisplay = new QCheckBox( "Time Display", 0 );

	QHBoxLayout *hBoxLayout = new QHBoxLayout( m_GroupBox );
	hBoxLayout->setSpacing( 15 );
	hBoxLayout->addWidget( m_CheckBox_BoundingBox );
	hBoxLayout->addWidget( m_CheckBox_ColorBar );
	hBoxLayout->addWidget( m_CheckBox_TimeDisplay );
	QSpacerItem *spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding );
	hBoxLayout->addItem( spacer );
	m_CheckBox_BoundingBox->setCheckState( Qt::Checked );
	m_CheckBox_ColorBar->setCheckState( Qt::Checked );
	m_CheckBox_TimeDisplay->setCheckState( Qt::Checked );
	m_GroupBox->setDisabled( true );
}

void EffectControl::setInitialValue()
{
	if ( param.IsBoundingBox() ) {
		m_CheckBox_BoundingBox->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_BoundingBox->setCheckState( Qt::Unchecked );
	}
	if ( param.IsColorBar() ) {
		m_CheckBox_ColorBar->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_ColorBar->setCheckState( Qt::Unchecked );
	}
	if ( param.IsTimeDisplay() ) {
		m_CheckBox_TimeDisplay->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_TimeDisplay->setCheckState( Qt::Unchecked );
	}
}

void EffectControl::setDisabled( bool state )
{
	m_GroupBox->setDisabled( state );
}

void EffectControl::setStateBoundingBox( bool state )
{
	param.setStateBoundingBox( state );
	emit toggledStateBoundingBox( !state );
}

void EffectControl::setStateColorBar( bool state )
{
	param.setStateColorBar( state );
	emit toggledStateColorBar( !state );
}

void EffectControl::setStateTimeDisplay( bool state )
{
	param.setStateTimeDisplay( state );
	emit toggledStateTimeDisplay( !state );
}
