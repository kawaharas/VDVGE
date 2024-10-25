//
// Volume Data Visualizer for Google Earth
//
// zscale_control.cpp
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

#include "zscale_control.h"

ZScaleControl::ZScaleControl( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

	connect( m_RadioButton_ZUnit_Meter, SIGNAL( toggled( bool ) ),
			 this, SLOT( setZUnit() ) );
	connect( m_RadioButton_ZUnit_KiloMeter, SIGNAL( toggled( bool ) ),
			 this, SLOT( setZUnit() ) );
	connect( m_HSlider_ZScale, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setZScale( int ) ) );
	connect( m_HSlider_ZScale, SIGNAL( valueChanged( int ) ),
			 m_Label_ZScaleValue, SLOT( setNum( int ) ) );
}

void ZScaleControl::makeWidget()
{
	m_GroupBox = new QGroupBox();
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox->setTitle( "Z Scale" );

	m_RadioButton_ZUnit_Meter     = new QRadioButton( "Meter", 0 );
	m_RadioButton_ZUnit_KiloMeter = new QRadioButton( "KiloMeter", 0 );
	QSpacerItem *spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding );

	QHBoxLayout *hBoxLayout1 = new QHBoxLayout();
	hBoxLayout1->setSpacing( 15 );
	hBoxLayout1->addWidget( m_RadioButton_ZUnit_Meter );
	hBoxLayout1->addWidget( m_RadioButton_ZUnit_KiloMeter );
	hBoxLayout1->addItem( spacer );
	m_RadioButton_ZUnit_Meter->setChecked( true );
	m_RadioButton_ZUnit_KiloMeter->setChecked( false );

	QLabel *label_ZScale = new QLabel( tr( "Scale:" ) );
	m_Label_ZScaleValue = new QLabel( tr("") );
	m_Label_ZScaleValue->setMinimumWidth( 25 );
	m_Label_ZScaleValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_Label_ZScaleValue->setNum( 1 );
	m_HSlider_ZScale = new QSlider( Qt::Horizontal, 0 );
	m_HSlider_ZScale->setRange( 1, 100 );
	m_HSlider_ZScale->setValue( 1 );

	QHBoxLayout *hBoxLayout2 = new QHBoxLayout();
	hBoxLayout2->addWidget( label_ZScale );
	hBoxLayout2->addWidget( m_Label_ZScaleValue );
	hBoxLayout2->addWidget( m_HSlider_ZScale );

	QVBoxLayout *vBoxLayout = new QVBoxLayout( m_GroupBox );
	vBoxLayout->addLayout( hBoxLayout1 );
	vBoxLayout->addLayout( hBoxLayout2 );

	m_GroupBox->setDisabled( true );
}

void ZScaleControl::setInitialValue()
{
	int zScale = static_cast< int >( param.getZScale() );

	if ( param.getZUnit() == ZUNIT_METER ) {
		m_RadioButton_ZUnit_Meter->setChecked( true );
		m_RadioButton_ZUnit_KiloMeter->setChecked( false );
	} else {
		m_RadioButton_ZUnit_Meter->setChecked( false );
		m_RadioButton_ZUnit_KiloMeter->setChecked( true );
	}
	m_HSlider_ZScale->setValue( zScale );
	m_Label_ZScaleValue->setNum( zScale );
}

void ZScaleControl::setDisabled( bool state )
{
	m_GroupBox->setDisabled( state );
}

void ZScaleControl::setZUnit()
{
	if ( m_RadioButton_ZUnit_Meter->isChecked() ) {
		param.setZUnit( ZUNIT_METER );
	} else if ( m_RadioButton_ZUnit_KiloMeter->isChecked() ) {
		param.setZUnit( ZUNIT_KILOMETER );
	}
}

void ZScaleControl::setZScale( int zscale )
{
	m_Label_ZScaleValue->setNum( zscale );
	param.setZScale( zscale );
}
