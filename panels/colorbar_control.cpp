//
// Volume Data Visualizer for Google Earth
//
// colorbar_control.cpp
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

#include "colorbar_control.h"

ColorBarControl::ColorBarControl( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

	connect( m_RadioButton_Horizontal, SIGNAL( toggled( bool ) ),
			 this, SLOT( setColorBarDirection() ) );
	connect( m_RadioButton_Vertical, SIGNAL( toggled( bool ) ),
			 this, SLOT( setColorBarDirection() ) );
	connect( m_RadioButton_RGB, SIGNAL( toggled( bool ) ),
			 this, SLOT( setColorBarType() ) );
	connect( m_RadioButton_RGBA, SIGNAL( toggled( bool ) ),
			 this, SLOT( setColorBarType() ) );
	connect( m_RadioButton_RGB_AND_RGBA, SIGNAL( toggled( bool ) ),
			 this, SLOT( setColorBarType() ) );
	connect( m_SpinBox_DecimalPlacesNum, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setDecimalPlacesNum( int ) ) );
	connect( m_SpinBox_MajorTickMarkNum, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setMajorTickMarkNum( int ) ) );
	connect( m_SpinBox_MinorTickMarkNum, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setMinorTickMarkNum( int ) ) );
	connect( m_ComboBox_MajorTickMarkType, SIGNAL( activated( int ) ),
			 this, SLOT( setMajorTickMarkType( int ) ) );
	connect( m_ComboBox_MinorTickMarkType, SIGNAL( activated( int ) ),
			 this, SLOT( setMinorTickMarkType( int ) ) );
}

void ColorBarControl::makeWidget()
{
	m_GroupBox = new QGroupBox();
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox->setTitle( "ColorBar" );

	QLabel *decimalPlacesNum  = new QLabel( tr( "Number of Decimal Places: " ) );
	QLabel *majorTickMarkNum  = new QLabel( tr( "Number of Major Tick Mark: " ) );
	QLabel *minorTickMarkNum  = new QLabel( tr( "Number of Minor Tick Mark: " ) );
	QLabel *majorTickMarkType = new QLabel( tr( "Major Tick Mark Type: " ) );
	QLabel *minorTickMarkType = new QLabel( tr( "Minor Tick Mark Type: " ) );

	m_RadioButton_Horizontal  = new QRadioButton( "Horizontal", 0 );
	m_RadioButton_Vertical    = new QRadioButton( "Vertical",   0 );
	m_RadioButton_Horizontal->setChecked( true );
	m_RadioButton_Vertical->setChecked( false );

	m_RadioButton_RGB          = new QRadioButton( "RGB",      0 );
	m_RadioButton_RGBA         = new QRadioButton( "RGBA",     0 );
	m_RadioButton_RGB_AND_RGBA = new QRadioButton( "RGB+RGBA", 0 );
	m_RadioButton_RGB->setChecked( true );
	m_RadioButton_RGBA->setChecked( false );
	m_RadioButton_RGB_AND_RGBA->setChecked( false );

	m_SpinBox_DecimalPlacesNum = new QSpinBox();
	m_SpinBox_DecimalPlacesNum->setAlignment( Qt::AlignRight );
	m_SpinBox_DecimalPlacesNum->setRange( 1, 5 );
	m_SpinBox_DecimalPlacesNum->setValue( 3 );

	m_SpinBox_MajorTickMarkNum = new QSpinBox();
	m_SpinBox_MajorTickMarkNum->setAlignment( Qt::AlignRight );
	m_SpinBox_MajorTickMarkNum->setMinimum( 2 );
	m_SpinBox_MajorTickMarkNum->setValue( 5 );

	m_SpinBox_MinorTickMarkNum = new QSpinBox();
	m_SpinBox_MinorTickMarkNum->setAlignment( Qt::AlignRight );
	m_SpinBox_MinorTickMarkNum->setMinimum( 0 );
	m_SpinBox_MinorTickMarkNum->setValue( 1 );

	m_ComboBox_MajorTickMarkType = new QComboBox();
	m_ComboBox_MajorTickMarkType->addItem( tr( "None" ) );
	m_ComboBox_MajorTickMarkType->addItem( tr( "OutSide" ) );
	m_ComboBox_MajorTickMarkType->addItem( tr( "InSide" ) );
	m_ComboBox_MajorTickMarkType->addItem( tr( "Cross" ) );
	m_ComboBox_MajorTickMarkType->setMinimumWidth( 100 );
	m_ComboBox_MajorTickMarkType->setCurrentIndex( 3 );

	m_ComboBox_MinorTickMarkType = new QComboBox();
	m_ComboBox_MinorTickMarkType->addItem( tr( "None" ) );
	m_ComboBox_MinorTickMarkType->addItem( tr( "OutSide" ) );
	m_ComboBox_MinorTickMarkType->addItem( tr( "InSide" ) );
	m_ComboBox_MinorTickMarkType->addItem( tr( "Cross" ) );
	m_ComboBox_MinorTickMarkType->setCurrentIndex( 1 );

	QFrame *line1 = new QFrame();
	line1->setFrameStyle( QFrame::Sunken | QFrame::HLine );
	QFrame *line2 = new QFrame();
	line2->setFrameStyle( QFrame::Sunken | QFrame::HLine );
	QFrame *line3 = new QFrame();
	line3->setFrameStyle( QFrame::Sunken | QFrame::HLine );
	QSpacerItem *spacer1 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );
	QSpacerItem *spacer2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );
	QSpacerItem *spacer3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );
	QSpacerItem *spacer4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );
	QSpacerItem *spacer5 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );
	QSpacerItem *spacer6 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );
	QSpacerItem *spacer7 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );

	QLabel *colorBarDirection  = new QLabel( tr( "Direction: " ) );
	QButtonGroup *buttonGroup1 = new QButtonGroup( m_GroupBox );
	buttonGroup1->addButton( m_RadioButton_Horizontal );
	buttonGroup1->addButton( m_RadioButton_Vertical );
	QHBoxLayout *hBoxLayout1 = new QHBoxLayout();
	hBoxLayout1->setSpacing( 15 );
	hBoxLayout1->addWidget( colorBarDirection );
	hBoxLayout1->addWidget( m_RadioButton_Horizontal );
	hBoxLayout1->addWidget( m_RadioButton_Vertical );
	hBoxLayout1->addItem( spacer1 );

	QLabel *colorBarType  = new QLabel( tr( "Type: " ) );
	QButtonGroup *buttonGroup2 = new QButtonGroup( m_GroupBox );
	buttonGroup2->addButton( m_RadioButton_RGB );
	buttonGroup2->addButton( m_RadioButton_RGBA );
	buttonGroup2->addButton( m_RadioButton_RGB_AND_RGBA );
	QHBoxLayout *hBoxLayout2 = new QHBoxLayout();
	hBoxLayout2->setSpacing( 15 );
	hBoxLayout2->addWidget( colorBarType );
	hBoxLayout2->addWidget( m_RadioButton_RGB );
	hBoxLayout2->addWidget( m_RadioButton_RGBA );
	hBoxLayout2->addWidget( m_RadioButton_RGB_AND_RGBA );
	hBoxLayout2->addItem( spacer2 );

	QGridLayout *gridLayout1 = new QGridLayout();
	gridLayout1->addWidget( decimalPlacesNum, 0, 0 );
	gridLayout1->addWidget( majorTickMarkNum, 1, 0 );
	gridLayout1->addWidget( minorTickMarkNum, 2, 0 );
	gridLayout1->addWidget( m_SpinBox_DecimalPlacesNum, 0, 1 );
	gridLayout1->addWidget( m_SpinBox_MajorTickMarkNum, 1, 1 );
	gridLayout1->addWidget( m_SpinBox_MinorTickMarkNum, 2, 1 );
	gridLayout1->addItem( spacer3, 1, 2 );
	gridLayout1->addItem( spacer4, 2, 2 );
	gridLayout1->addItem( spacer5, 0, 2 );

	QGridLayout *gridLayout2 = new QGridLayout();
	gridLayout2->addWidget( majorTickMarkType, 0, 0 );
	gridLayout2->addWidget( minorTickMarkType, 1, 0 );
	gridLayout2->addWidget( m_ComboBox_MajorTickMarkType, 0, 1 );
	gridLayout2->addWidget( m_ComboBox_MinorTickMarkType, 1, 1 );
	gridLayout2->addItem( spacer6, 0, 2 );
	gridLayout2->addItem( spacer7, 1, 2 );

	QVBoxLayout *vBoxLayout = new QVBoxLayout( m_GroupBox );
	vBoxLayout->addLayout( hBoxLayout1 );
//	vBoxLayout->addWidget( groupBox1 );
	vBoxLayout->addWidget( line1 );
	vBoxLayout->addLayout( hBoxLayout2 );
//	vBoxLayout->addWidget( groupBox2 );
	vBoxLayout->addWidget( line2 );
	vBoxLayout->addLayout( gridLayout1 );
	vBoxLayout->addWidget( line3 );
	vBoxLayout->addLayout( gridLayout2 );

	m_GroupBox->setDisabled( true );
}

void ColorBarControl::setInitialValue()
{
	if ( param.getColorBarOrientation() == HORIZONTAL ) {
		m_RadioButton_Horizontal->setChecked( true );
		m_RadioButton_Vertical->setChecked( false );
	} else {
		m_RadioButton_Horizontal->setChecked( false );
		m_RadioButton_Vertical->setChecked( true );
	}
	m_SpinBox_DecimalPlacesNum->setValue( param.getColorBarDecimalPlacesNum() );
	m_SpinBox_MajorTickMarkNum->setValue( param.getColorBarMajorTickMarkNum() );
	m_SpinBox_MinorTickMarkNum->setValue( param.getColorBarMinorTickMarkNum() );
	m_ComboBox_MajorTickMarkType->setCurrentIndex( param.getColorBarMajorTickMarkType() );
	m_ComboBox_MinorTickMarkType->setCurrentIndex( param.getColorBarMinorTickMarkType() );
}

void ColorBarControl::setDisabled( bool state )
{
	m_GroupBox->setDisabled( state );
}

void ColorBarControl::setColorBarType()
{
	if ( m_RadioButton_RGB->isChecked() ) {
		param.setColorBarType( COLORBAR_RGB );
	} else if ( m_RadioButton_RGBA->isChecked() ) {
		param.setColorBarType( COLORBAR_RGBA );
	} else if ( m_RadioButton_RGB_AND_RGBA->isChecked() ) {
		param.setColorBarType( COLORBAR_RGB_AND_RGBA );
	}
}

void ColorBarControl::setColorBarDirection()
{
	if ( m_RadioButton_Horizontal->isChecked() ) {
		param.setColorBarOrientation( HORIZONTAL );
	} else if ( m_RadioButton_Vertical->isChecked() ) {
		param.setColorBarOrientation( VERTICAL );
	}
}

void ColorBarControl::setDecimalPlacesNum( int value )
{
	param.setColorBarDecimalPlacesNum( value );
}

void ColorBarControl::setMajorTickMarkNum( int value )
{
	param.setColorBarMajorTickMarkNum( value );
}

void ColorBarControl::setMinorTickMarkNum( int value )
{
	param.setColorBarMinorTickMarkNum( value );
}

void ColorBarControl::setMajorTickMarkType( int value )
{
	ColorBarTickMarkType type = TICKMARK_NONE;
	if ( value == 0 ) {
		type = TICKMARK_NONE;
	} else if ( value == 1 ) {
		type = TICKMARK_OUTSIDE;
	} else if ( value == 2 ) {
		type = TICKMARK_INSIDE;
	} else {
		type = TICKMARK_CROSS;
	}
	param.setColorBarMajorTickMarkType( type );
}

void ColorBarControl::setMinorTickMarkType( int value )
{
	ColorBarTickMarkType type = TICKMARK_NONE;
	if ( value == 0 ) {
		type = TICKMARK_NONE;
	} else if ( value == 1 ) {
		type = TICKMARK_OUTSIDE;
	} else if ( value == 2 ) {
		type = TICKMARK_INSIDE;
	} else {
		type = TICKMARK_CROSS;
	}
	param.setColorBarMinorTickMarkType( type );
}
