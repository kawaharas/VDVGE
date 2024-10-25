//
// Volume Data Visualizer for Google Earth
//
// topo_control.cpp
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

#include "topo_control.h"

TopoControl::TopoControl( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	connect( m_ButtonGroup, SIGNAL( idClicked( int ) ),
			this, SLOT( setTopoType( int ) ) );
#else
	connect( m_ButtonGroup, SIGNAL( buttonClicked( int ) ),
			 this, SLOT( setTopoType( int ) ) );
#endif
	connect( m_CheckBox_Topo, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateTopo( bool ) ) );
	connect( m_CheckBox_SeaSurface, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateSeaSurface( bool ) ) );
	connect( m_HSlider_GroundScale, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setGroundScale( int ) ) );
	connect( m_HSlider_GroundScale, SIGNAL( valueChanged( int ) ),
			 m_Label_GroundScaleValue, SLOT( setNum( int ) ) );
	connect( m_HSlider_SeaFloorScale, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setSeaFloorScale( int ) ) );
	connect( m_HSlider_SeaFloorScale, SIGNAL( valueChanged( int ) ),
			 m_Label_SeaFloorScaleValue, SLOT( setNum( int ) ) );

	connect( m_CheckBox_SyncScale, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateSyncScale( bool ) ) );
}

void TopoControl::makeWidget()
{
	m_GroupBox = new QGroupBox( this );
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox->setTitle( "Topography" );

	m_RadioButton_ETOPO5 = new QRadioButton( "ETOPO5", 0 );
	m_RadioButton_ETOPO2 = new QRadioButton( "ETOPO2", 0 );
	m_RadioButton_ETOPO1 = new QRadioButton( "ETOPO1", 0 );
	m_RadioButton_NONE   = new QRadioButton( "NONE", 0 );
	m_RadioButton_ETOPO5->setChecked( false );
	m_RadioButton_ETOPO2->setChecked( false );
	m_RadioButton_ETOPO1->setChecked( false );
	m_RadioButton_NONE->setChecked( true );

	m_CheckBox_Topo = new QCheckBox( "Enable Topo", 0 );
	m_CheckBox_SeaSurface = new QCheckBox( "Enable Seasurface", 0 );
	m_CheckBox_SyncScale = new QCheckBox( "Synchronize ground scale and seafloor scale", 0 );

	QSpacerItem *spacer1 = new QSpacerItem( 20, 20,
		QSizePolicy::Expanding, QSizePolicy::Expanding );
	QSpacerItem *spacer2 = new QSpacerItem( 20, 20,
		QSizePolicy::Expanding, QSizePolicy::Expanding );
	QSpacerItem *spacer3 = new QSpacerItem( 20, 20,
		QSizePolicy::Expanding, QSizePolicy::Expanding );
	QSpacerItem *spacer4 = new QSpacerItem( 20, 20,
		QSizePolicy::Expanding, QSizePolicy::Expanding );
	QFrame *line1 = new QFrame();
	line1->setFrameStyle( QFrame::Sunken | QFrame::HLine );
	QFrame *line2 = new QFrame();
	line2->setFrameStyle( QFrame::Sunken | QFrame::HLine );

	m_ButtonGroup = new QButtonGroup( m_GroupBox );
	m_ButtonGroup->addButton( m_RadioButton_ETOPO5 );
	m_ButtonGroup->addButton( m_RadioButton_ETOPO2 );
	m_ButtonGroup->addButton( m_RadioButton_ETOPO1 );
	m_ButtonGroup->addButton( m_RadioButton_NONE );
	m_ButtonGroup->setId( m_RadioButton_ETOPO5, TOPO_ETOPO5 );
	m_ButtonGroup->setId( m_RadioButton_ETOPO2, TOPO_ETOPO2 );
	m_ButtonGroup->setId( m_RadioButton_ETOPO1, TOPO_ETOPO1 );
	m_ButtonGroup->setId( m_RadioButton_NONE,   TOPO_NONE   );
	QHBoxLayout *hBoxLayout1 = new QHBoxLayout();
	hBoxLayout1->setSpacing( 15 );
	hBoxLayout1->addWidget( m_RadioButton_ETOPO5 );
	hBoxLayout1->addWidget( m_RadioButton_ETOPO2 );
	hBoxLayout1->addWidget( m_RadioButton_ETOPO1 );
	hBoxLayout1->addWidget( m_RadioButton_NONE );
	hBoxLayout1->addItem( spacer1 );

	QHBoxLayout *hBoxLayout2 = new QHBoxLayout();
	hBoxLayout2->setSpacing( 15 );
	hBoxLayout2->addWidget( m_CheckBox_Topo );
	hBoxLayout2->addItem( spacer2 );
	m_CheckBox_Topo->setCheckState( Qt::Unchecked );

	QHBoxLayout *hBoxLayout3 = new QHBoxLayout();
	hBoxLayout3->setSpacing( 15 );
	hBoxLayout3->addWidget( m_CheckBox_SeaSurface );
	hBoxLayout3->addItem( spacer3 );
	m_CheckBox_SeaSurface->setCheckState( Qt::Unchecked );

	m_Label_GroundScale = new QLabel( tr( "Ground Scale:" ) );
	m_Label_SeaFloorScale = new QLabel( tr( "Seafloor Scale:" ) );
	m_Label_GroundScaleValue = new QLabel( tr("") );
	m_Label_GroundScaleValue->setMinimumWidth( 25 );
	m_Label_GroundScaleValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_Label_GroundScaleValue->setNum( 1 );
	m_Label_SeaFloorScaleValue = new QLabel( tr("") );
	m_Label_SeaFloorScaleValue->setMinimumWidth( 25 );
	m_Label_SeaFloorScaleValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_Label_SeaFloorScaleValue->setNum( 1 );
	m_HSlider_GroundScale = new QSlider( Qt::Horizontal, 0 );
	m_HSlider_GroundScale->setRange( 1, 100 );
	m_HSlider_GroundScale->setValue( 1 );
	m_HSlider_SeaFloorScale = new QSlider( Qt::Horizontal, 0 );
	m_HSlider_SeaFloorScale->setRange( 1, 100 );
	m_HSlider_SeaFloorScale->setValue( 1 );
	m_Label_SeaFloorScale->setDisabled( true );
	m_Label_SeaFloorScaleValue->setDisabled( true );
	m_HSlider_SeaFloorScale->setDisabled( true );
	m_Button_SyncScale = new QPushButton( tr( "Sync" ) );
	m_Button_SyncScale->setMaximumSize( 20, 20 );
	m_Button_SyncScale->setCheckable( true );
	m_Button_SyncScale->setContentsMargins( 10, 10, 10, 10 );

	QGridLayout *gridLayout = new QGridLayout();
	gridLayout->setSpacing( 10 );
	gridLayout->addWidget( m_Label_GroundScale, 0, 0 );
	gridLayout->addWidget( m_Label_SeaFloorScale, 1, 0 );
	gridLayout->addWidget( m_Label_GroundScaleValue, 0, 1 );
	gridLayout->addWidget( m_Label_SeaFloorScaleValue, 1, 1 );
	gridLayout->addWidget( m_HSlider_GroundScale, 0, 2 );
	gridLayout->addWidget( m_HSlider_SeaFloorScale, 1, 2 );

	QHBoxLayout *hBoxLayout4 = new QHBoxLayout();
	hBoxLayout4->setSpacing( 0 );
	hBoxLayout4->addItem( spacer4 );
	hBoxLayout4->addWidget( m_CheckBox_SyncScale );
	m_CheckBox_SyncScale->setCheckState( Qt::Checked );

	QVBoxLayout *vBoxLayout1 = new QVBoxLayout();
	vBoxLayout1->addLayout( gridLayout );
	vBoxLayout1->addLayout( hBoxLayout4 );

	m_GroupBox_Scale = new QGroupBox();
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox_Scale->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox_Scale->setTitle( "Scale" );
	m_GroupBox_Scale->setLayout( vBoxLayout1 );

	QVBoxLayout *vBoxLayout = new QVBoxLayout( m_GroupBox );
	vBoxLayout->addLayout( hBoxLayout1 );
	vBoxLayout->addWidget( line1 );
	vBoxLayout->addLayout( hBoxLayout2 );
	vBoxLayout->addLayout( hBoxLayout3 );
	vBoxLayout->addWidget( m_GroupBox_Scale );

	QString topoFileName;
	topoFileName = QCoreApplication::applicationDirPath() + "/etopo1_ice_g_i2.bin";
	if ( QFileInfo( topoFileName ).exists() ) {
		m_ExistETOPO1 = true;
	} else {
		m_ExistETOPO1 = false;
		m_RadioButton_ETOPO1->setDisabled( true );
	}
	topoFileName = QCoreApplication::applicationDirPath() + "/ETOPO2v2g_i2_LSB.bin";
	if ( QFileInfo( topoFileName ).exists() ) {
		m_ExistETOPO2 = true;
	} else {
		m_ExistETOPO2 = false;
		m_RadioButton_ETOPO2->setDisabled( true );
	}
	topoFileName = QCoreApplication::applicationDirPath() + "/ETOPO5.DOS";
	if ( QFileInfo( topoFileName ).exists() ) {
		m_ExistETOPO5 = true;
	} else {
		m_ExistETOPO5 = false;
		m_RadioButton_ETOPO5->setDisabled( true );
	}
	m_ExistTopoData = m_ExistETOPO5 || m_ExistETOPO2 || m_ExistETOPO1;
	if ( !m_ExistTopoData ) {
		m_GroupBox->setDisabled( true );
	}
}

void TopoControl::setInitialValue()
{
	m_North = grads.getGridYMax();
	m_South = grads.getGridYMin();
	m_West  = grads.getGridXMin();
	m_East  = grads.getGridXMax();

	if ( m_West < 0.0 ) {
		m_West += 360.0;
		m_East += 360.0;
	}

	int GroundScale   = static_cast< int >( param.getGroundScale() );
	int SeaFloorScale = static_cast< int >( param.getSeaFloorScale() );

	bool usableETOPO5 = isUsableData(  4320,  2160, CELL_CENTERED );
	bool usableETOPO2 = isUsableData( 10801,  5401, GRID_CENTERED );
	bool usableETOPO1 = isUsableData( 21601, 10801, GRID_CENTERED );
	if ( m_ExistETOPO5 && usableETOPO5 ) {
		m_RadioButton_ETOPO5->setEnabled( true );
	} else {
		m_RadioButton_ETOPO5->setEnabled( false );
	}
	if ( m_ExistETOPO2 && usableETOPO2 ) {
		m_RadioButton_ETOPO2->setEnabled( true );
	} else {
		m_RadioButton_ETOPO2->setEnabled( false );
	}
	if ( m_ExistETOPO1 && usableETOPO1 ) {
		m_RadioButton_ETOPO1->setEnabled( true );
	} else {
		m_RadioButton_ETOPO1->setEnabled( false );
	}
	setDisabledSettings( true );

	m_CheckBox_Topo->setCheckState( Qt::Unchecked );
	m_CheckBox_SeaSurface->setCheckState( Qt::Unchecked );
	m_RadioButton_NONE->setChecked( true );

	m_HSlider_GroundScale->setValue( GroundScale );
	m_Label_GroundScaleValue->setNum( GroundScale );
	m_HSlider_SeaFloorScale->setValue( SeaFloorScale );
	m_Label_SeaFloorScaleValue->setNum( SeaFloorScale );

	if ( !m_ExistTopoData ) {
		m_GroupBox->setDisabled( true );
	}
}

void TopoControl::setDisabled( bool state )
{
	m_GroupBox->setDisabled( state );
}

void TopoControl::setDisabledSettings( bool state )
{
	m_CheckBox_Topo->setDisabled( state );
	m_CheckBox_SeaSurface->setDisabled( state );
	m_GroupBox_Scale->setDisabled( state );
}

void TopoControl::setStateTopo( bool state )
{
	if ( state ) {
		setDisabledSettings( false );
	} else {
		setDisabledSettings( true );
		m_CheckBox_Topo->setDisabled( false );
	}

	param.setStateTopo( state );
	emit valueChanged( CHANGE_TOPO_STATE );
}

void TopoControl::setStateSeaSurface( bool state )
{
	param.setStateSeaSurface( state );
}

void TopoControl::setStateSyncScale( bool state )
{
	if ( state ) {
		m_Label_SeaFloorScale->setDisabled( true );
		m_Label_SeaFloorScaleValue->setDisabled( true );
		m_HSlider_SeaFloorScale->setDisabled( true );
	} else {
		m_Label_SeaFloorScale->setDisabled( false );
		m_Label_SeaFloorScaleValue->setDisabled( false );
		m_HSlider_SeaFloorScale->setDisabled( false );
	}

	param.setStateSyncScale( state );
	emit valueChanged( CHANGE_TOPO_SCALE );
}

void TopoControl::setGroundScale( int scale )
{
	m_Label_GroundScaleValue->setNum( scale );
	param.setGroundScale( scale );
	emit valueChanged( CHANGE_TOPO_SCALE );
}

void TopoControl::setSeaFloorScale( int scale )
{
	m_Label_SeaFloorScaleValue->setNum( scale );
	param.setSeaFloorScale( scale );
	emit valueChanged( CHANGE_TOPO_SCALE );
}

void TopoControl::setTopoType( int id )
{
	if ( id == TOPO_NONE ) {
		param.setTopoType( TOPO_NONE );
		setDisabledSettings( true );
	} else {
		if ( id == TOPO_ETOPO5 ) {
			param.setTopoType( TOPO_ETOPO5 );
		} else if ( id == TOPO_ETOPO2 ) {
			param.setTopoType( TOPO_ETOPO2 );
		} else if ( id == TOPO_ETOPO1 ) {
			param.setTopoType( TOPO_ETOPO1 );
		}
		if ( m_CheckBox_Topo->isChecked() ) {
			setDisabledSettings( false );
		} else {
			setDisabledSettings( true );
			m_CheckBox_Topo->setDisabled( false );
		}
	}
	emit valueChanged( CHANGE_TOPO_DATASET );
}

bool TopoControl::isUsableData( int lon, int lat, DataType type )
{
	double dLON = static_cast< double >( lon );
	double dLAT = static_cast< double >( lat );

	int idx0x = 0;
	int idx1x = 0;
	int idx0y = 0;
	int idx1y = 0;

	if ( type == CELL_CENTERED ) {
		// for ETOPO5 ( cell centered )
		for ( int i = 0; i < lon * 2; i++ ) {
			double di = static_cast< double >( i );
			if ( m_West < 180.0 * ( 2.0 * di + 1.0 ) / dLON ) {
				idx0x = i - 1;
				break;
			}
		}
		for ( int i = 0; i < lon * 2; i++ ) {
			double di = static_cast< double >( i );
			if ( m_East < 180.0 * ( 2.0 * di + 1.0 ) / dLON ) {
				idx1x = i;
				break;
			}
		}
		for ( int i = 0; i < lat; i++ ) {
			double di = static_cast< double >( i );
			if ( m_South < -90.0 + ( 180.0 * di + 90.0 ) / dLAT ) {
				idx0y = i;
				break;
			}
		}
		idx1y = lat - 1; // for overflow
		for ( int i = 0; i < lat; i++ ) {
			double di = static_cast< double >( i );
			if ( m_North < -90.0 + ( 180.0 * di + 90.0 ) / dLAT ) {
				idx1y = i;
				break;
			}
		}
	} else {
		// for ETOPO1/ETOPO2 ( grid centered )
		for ( int i = 0; i < lon * 2; i++ ) {
			double di = static_cast< double >( i );
			if ( m_West < 360.0 / ( dLON - 1.0 ) * di ) {
				idx0x = i - 1;
				break;
			}
		}
		for ( int i = 0; i < lon * 2; i++ ) {
			double di = static_cast< double >( i );
			if ( m_East < 360.0 / ( dLON - 1.0 ) * di ) {
				idx1x = i;
				break;
			}
		}
		for ( int i = 0; i < lat; i++ ) {
			double di = static_cast< double >( i );
			if ( m_South <
					-90.0 + 180.0 / ( dLAT - 1.0 ) * di ) {
				idx0y = i - 1;
				break;
			}
		}
		for ( int i = 0; i < lat; i++ ) {
			double di = static_cast< double >( i );
			if ( m_North <=
					-90.0 + 180.0 / ( dLAT - 1.0 ) * di ) {
				idx1y = i;
				break;
			}
		}
	}

	// Maximum Size of Indexes is 0xFFFFFF
	//  ( 0xFFFFFF: Maximum Number of "GLsizei" that is used in glDrawElements() )
	if ( ( idx1x - idx0x - 1 ) * ( idx1y - idx0y - 1 ) * 6 > 0xFFFFFF ) {
		return false;
	}

	return true;
}
