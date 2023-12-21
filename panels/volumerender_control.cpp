//
// Volume Data Visualizer for Google Earth
//
// volumerender_control.cpp
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

#include "volumerender_control.h"

VolumeRenderControl::VolumeRenderControl( QWidget *parent ) :
	QWidget( parent )
{
	setMinimumHeight( 600 );
	makeWidget();

	connect( m_CheckBox_XSlice, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateXSlice( bool ) ) );
	connect( m_CheckBox_YSlice, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateYSlice( bool ) ) );
	connect( m_CheckBox_ZSlice, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateZSlice( bool ) ) );
	connect( m_HSlider_XSlice, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setSliceNumX( int ) ) );
	connect( m_HSlider_YSlice, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setSliceNumY( int ) ) );
	connect( m_HSlider_ZSlice, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setSliceNumZ( int ) ) );
}

void VolumeRenderControl::makeWidget()
{
	m_CheckBox_XSlice = new QCheckBox( "X Slice:", 0 );
	m_CheckBox_YSlice = new QCheckBox( "Y Slice:", 0 );
	m_CheckBox_ZSlice = new QCheckBox( "Z Slice:", 0 );
	m_Label_XSlice = new QLabel( tr("---") );
	m_Label_YSlice = new QLabel( tr("---") );
	m_Label_ZSlice = new QLabel( tr("---") );
	m_HSlider_XSlice = new QSlider( Qt::Horizontal, 0 );
	m_HSlider_YSlice = new QSlider( Qt::Horizontal, 0 );
	m_HSlider_ZSlice = new QSlider( Qt::Horizontal, 0 );
	m_CheckBox_XSlice->setCheckState( Qt::Unchecked );
	m_CheckBox_YSlice->setCheckState( Qt::Unchecked );
	m_CheckBox_ZSlice->setCheckState( Qt::Checked );

	m_GroupBox = new QGroupBox();
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox->setTitle( "Volume Render" );
//	m_GroupBox->setFixedWidth( 400 );
//	m_GroupBox->setMinimumHeight( 200 );
	m_GroupBox->setDisabled( true );

	QGridLayout *gridLayout = new QGridLayout( m_GroupBox );
//	gridLayout->setVerticalSpacing( 10 );
	gridLayout->addWidget( m_CheckBox_XSlice, 0, 0 );
	gridLayout->addWidget( m_CheckBox_YSlice, 1, 0 );
	gridLayout->addWidget( m_CheckBox_ZSlice, 2, 0 );
	gridLayout->addWidget( m_Label_XSlice, 0, 1 );
	gridLayout->addWidget( m_Label_YSlice, 1, 1 );
	gridLayout->addWidget( m_Label_ZSlice, 2, 1 );
	gridLayout->addWidget( m_HSlider_XSlice, 0, 2 );
	gridLayout->addWidget( m_HSlider_YSlice, 1, 2 );
	gridLayout->addWidget( m_HSlider_ZSlice, 2, 2 );
	m_Label_XSlice->setMinimumWidth( 20 );
	m_Label_YSlice->setMinimumWidth( 20 );
	m_Label_ZSlice->setMinimumWidth( 20 );
	m_Label_XSlice->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_Label_YSlice->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_Label_ZSlice->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
}

void VolumeRenderControl::setInitialValue()
{
	int sliceNumX = param.getVolumeXNum();
	int sliceNumY = param.getVolumeYNum();
	int sliceNumZ = param.getVolumeZNum();

	if ( grads.getGridSizeX() == 1 ) {
		m_HSlider_XSlice->setRange( 1, grads.getGridSizeX() );
	} else {
		m_HSlider_XSlice->setRange( 2, grads.getGridSizeX() );
	}
	m_HSlider_XSlice->setValue( sliceNumX );
	if ( param.IsVolumeX() ) {
		m_HSlider_XSlice->setDisabled( false );
		m_Label_XSlice->setDisabled( false );
		m_CheckBox_XSlice->setCheckState( Qt::Checked );
	} else {
		m_HSlider_XSlice->setDisabled( true );
		m_Label_XSlice->setDisabled( true );
		m_CheckBox_XSlice->setCheckState( Qt::Unchecked );
	}

	if ( grads.getGridSizeY() == 1 ) {
		m_HSlider_YSlice->setRange( 1, grads.getGridSizeY() );
	} else {
		m_HSlider_YSlice->setRange( 2, grads.getGridSizeY() );
	}
	m_HSlider_YSlice->setValue( sliceNumY );
	if ( param.IsVolumeY() ) {
		m_HSlider_YSlice->setDisabled( false );
		m_Label_YSlice->setDisabled( false );
		m_CheckBox_YSlice->setCheckState( Qt::Checked );
	} else {
		m_HSlider_YSlice->setDisabled( true );
		m_Label_YSlice->setDisabled( true );
		m_CheckBox_YSlice->setCheckState( Qt::Unchecked );
	}

	if ( grads.getGridSizeZ() == 1 ) {
		m_CheckBox_XSlice->hide();
		m_Label_XSlice->hide();
		m_HSlider_XSlice->hide();
		m_CheckBox_YSlice->hide();
		m_Label_YSlice->hide();
		m_HSlider_YSlice->hide();
		m_HSlider_ZSlice->setRange( 1, grads.getGridSizeZ() );
	} else {
		m_CheckBox_XSlice->show();
		m_Label_XSlice->show();
		m_HSlider_XSlice->show();
		m_CheckBox_YSlice->show();
		m_Label_YSlice->show();
		m_HSlider_YSlice->show();
		m_HSlider_ZSlice->setRange( 2, grads.getGridSizeZ() );
	}
	m_HSlider_ZSlice->setValue( sliceNumZ );
	if ( param.IsVolumeZ() ) {
		m_HSlider_ZSlice->setDisabled( false );
		m_Label_ZSlice->setDisabled( false );
		m_CheckBox_ZSlice->setCheckState( Qt::Checked );
	} else {
		m_HSlider_ZSlice->setDisabled( true );
		m_Label_ZSlice->setDisabled( true );
		m_CheckBox_ZSlice->setCheckState( Qt::Unchecked );
	}
}

void VolumeRenderControl::setDisabled( bool state )
{
	m_GroupBox->setDisabled( state );
}

void VolumeRenderControl::setStateXSlice( bool state )
{
	m_Label_XSlice->setDisabled( !state );
	m_HSlider_XSlice->setDisabled( !state );
	param.setStateVolumeX( state );
}

void VolumeRenderControl::setStateYSlice( bool state )
{
	m_Label_YSlice->setDisabled( !state );
	m_HSlider_YSlice->setDisabled( !state );
	param.setStateVolumeY( state );
}

void VolumeRenderControl::setStateZSlice( bool state )
{
	m_Label_ZSlice->setDisabled( !state );
	m_HSlider_ZSlice->setDisabled( !state );
	param.setStateVolumeZ( state );
}

void VolumeRenderControl::setSliceNumX( int num )
{
	m_Label_XSlice->setNum( num );
	param.setVolumeXNum( num );
}

void VolumeRenderControl::setSliceNumY( int num )
{
	m_Label_YSlice->setNum( num );
	param.setVolumeYNum( num );
}

void VolumeRenderControl::setSliceNumZ( int num )
{
	m_Label_ZSlice->setNum( num );
	param.setVolumeZNum( num );
}
