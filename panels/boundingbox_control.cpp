//
// Volume Data Visualizer for Google Earth
//
// boundingbox_control.cpp
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

#include "boundingbox_control.h"

BoundingBoxControl::BoundingBoxControl( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

	connect( m_CheckBox_AxisLabel, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateAxisLabel( bool ) ) );
}

void BoundingBoxControl::makeWidget()
{
	m_GroupBox = new QGroupBox();
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox->setTitle( "Bounding Box" );

	m_CheckBox_AxisLabel = new QCheckBox( "Axis Label", 0 );

	QSpacerItem *spacer = new QSpacerItem( 20, 20,
		QSizePolicy::Expanding, QSizePolicy::Expanding );

	QHBoxLayout *hBoxLayout = new QHBoxLayout( m_GroupBox );
	hBoxLayout->setSpacing( 15 );
	hBoxLayout->addWidget( m_CheckBox_AxisLabel );
	hBoxLayout->addItem( spacer );
	m_CheckBox_AxisLabel->setCheckState( Qt::Checked );
	m_GroupBox->setDisabled( true );
}

void BoundingBoxControl::setInitialValue()
{
	if ( param.IsBoundingBoxAxisLabel() ) {
		m_CheckBox_AxisLabel->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_AxisLabel->setCheckState( Qt::Unchecked );
	}
}

void BoundingBoxControl::setDisabled( bool state )
{
	m_GroupBox->setDisabled( state );
}

void BoundingBoxControl::setStateAxisLabel( bool state )
{
	param.setStateBoundingBoxAxisLabel( state );
}
