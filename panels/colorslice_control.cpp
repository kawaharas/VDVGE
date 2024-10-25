//
// Volume Data Visualizer for Google Earth
//
// colorslice_control.cpp
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

#include "colorslice_control.h"

ColorSliceControl::ColorSliceControl( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

	connect( m_LineEdit_XValue, SIGNAL( returnPressed() ),
			 this, SLOT( setColorSliceXValue() ) );
	connect( m_LineEdit_YValue, SIGNAL( returnPressed() ),
			 this, SLOT( setColorSliceYValue() ) );
	connect( m_LineEdit_ZValue, SIGNAL( returnPressed() ),
			 this, SLOT( setColorSliceZValue() ) );
	connect( m_CheckBox_ColorSliceX, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateColorSliceX( bool ) ) );
	connect( m_CheckBox_ColorSliceY, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateColorSliceY( bool ) ) );
	connect( m_CheckBox_ColorSliceZ, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateColorSliceZ( bool ) ) );
	connect( m_CheckBox_ClampToGround, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateClampToGround( bool ) ) );
	connect( m_CheckBox_ColorSliceAlpha, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateColorSliceAlpha( bool ) ) );
	connect( m_HSlider_ColorSliceX, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setColorSliceXAxisLabel( int ) ) );
	connect( m_HSlider_ColorSliceY, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setColorSliceYAxisLabel( int ) ) );
	connect( m_HSlider_ColorSliceZ, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setColorSliceZAxisLabel( int ) ) );
	connect( m_HSlider_ColorSliceOpacity, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setColorSliceOpacity( int ) ) );
}

void ColorSliceControl::makeWidget()
{
	m_GroupBox = new QGroupBox();
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox->setTitle( "Color Slice" );

	m_CheckBox_ColorSliceX = new QCheckBox( "X-Axis:", 0 );
	m_CheckBox_ColorSliceY = new QCheckBox( "Y-Axis:", 0 );
	m_CheckBox_ColorSliceZ = new QCheckBox( "Z-Axis:", 0 );
	m_CheckBox_ColorSliceX->setCheckState( Qt::Unchecked );
	m_CheckBox_ColorSliceY->setCheckState( Qt::Unchecked );
	m_CheckBox_ColorSliceZ->setCheckState( Qt::Unchecked );

	m_LineEdit_XValue = new QLineEdit( this );
	m_LineEdit_YValue = new QLineEdit( this );
	m_LineEdit_ZValue = new QLineEdit( this );
	m_LineEdit_XValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_LineEdit_YValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_LineEdit_ZValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_LineEdit_XValue->setMinimumWidth( 80 );
	m_LineEdit_YValue->setMinimumWidth( 80 );
	m_LineEdit_ZValue->setMinimumWidth( 80 );
	m_LineEdit_XValue->setMaximumWidth( 80 );
	m_LineEdit_YValue->setMaximumWidth( 80 );
	m_LineEdit_ZValue->setMaximumWidth( 80 );
	QDoubleValidator *validator = new QDoubleValidator( this );
	validator->setNotation( QDoubleValidator::ScientificNotation );
	m_LineEdit_XValue->setValidator( validator );
	m_LineEdit_YValue->setValidator( validator );
	m_LineEdit_ZValue->setValidator( validator );
	m_LineEdit_XValue->installEventFilter( this );
	m_LineEdit_YValue->installEventFilter( this );
	m_LineEdit_ZValue->installEventFilter( this );
	m_LineEdit_XValue->setDisabled( true );
	m_LineEdit_YValue->setDisabled( true );
	m_LineEdit_ZValue->setDisabled( true );

	m_HSlider_ColorSliceX = new QSlider( Qt::Horizontal, 0 );
	m_HSlider_ColorSliceY = new QSlider( Qt::Horizontal, 0 );
	m_HSlider_ColorSliceZ = new QSlider( Qt::Horizontal, 0 );
	m_HSlider_ColorSliceX->setMinimum(   0 );
	m_HSlider_ColorSliceY->setMinimum(   0 );
	m_HSlider_ColorSliceZ->setMinimum(   0 );
	m_HSlider_ColorSliceX->setMaximum( 100000 );
	m_HSlider_ColorSliceY->setMaximum( 100000 );
	m_HSlider_ColorSliceZ->setMaximum( 100000 );
	m_HSlider_ColorSliceX->setSingleStep( 1000 );
	m_HSlider_ColorSliceY->setSingleStep( 1000 );
	m_HSlider_ColorSliceZ->setSingleStep( 1000 );
	m_HSlider_ColorSliceX->setValue( 0 );
	m_HSlider_ColorSliceY->setValue( 0 );
	m_HSlider_ColorSliceZ->setValue( 0 );
	m_HSlider_ColorSliceX->setDisabled( true );
	m_HSlider_ColorSliceY->setDisabled( true );
	m_HSlider_ColorSliceZ->setDisabled( true );

	m_CheckBox_ColorSliceAlpha = new QCheckBox( "Use Alpha Channel", 0 );
	m_CheckBox_ColorSliceAlpha->setCheckState( Qt::Unchecked );
	QLabel *label_ColorSliceOpacity = new QLabel( tr( "Opacity: " ) );
	m_HSlider_ColorSliceOpacity = new QSlider( Qt::Horizontal, 0 );
	m_HSlider_ColorSliceOpacity->setMinimum(   0 );
	m_HSlider_ColorSliceOpacity->setMaximum( 100 );
	m_HSlider_ColorSliceOpacity->setValue( 100 );
//	m_HSlider_ColorSliceOpacity->setDisabled( true );

	m_CheckBox_ClampToGround = new QCheckBox( "ClampToGround (Z-Axis Only)", 0 );
	m_CheckBox_ClampToGround->setCheckState( Qt::Unchecked );

	QGridLayout *gridLayout = new QGridLayout();
//	gridLayout->setVerticalSpacing( 10 );
	gridLayout->addWidget( m_CheckBox_ColorSliceX, 0, 0 );
	gridLayout->addWidget( m_CheckBox_ColorSliceY, 1, 0 );
	gridLayout->addWidget( m_CheckBox_ColorSliceZ, 2, 0 );
	gridLayout->addWidget( m_LineEdit_XValue, 0, 1 );
	gridLayout->addWidget( m_LineEdit_YValue, 1, 1 );
	gridLayout->addWidget( m_LineEdit_ZValue, 2, 1 );
	gridLayout->addWidget( m_HSlider_ColorSliceX, 0, 2 );
	gridLayout->addWidget( m_HSlider_ColorSliceY, 1, 2 );
	gridLayout->addWidget( m_HSlider_ColorSliceZ, 2, 2 );

	QHBoxLayout *hBoxLayout1 = new QHBoxLayout();
	hBoxLayout1->addWidget( m_CheckBox_ColorSliceAlpha );
	hBoxLayout1->addWidget( m_CheckBox_ClampToGround );

	QHBoxLayout *hBoxLayout2 = new QHBoxLayout();
	hBoxLayout2->addWidget( label_ColorSliceOpacity );
	hBoxLayout2->addWidget( m_HSlider_ColorSliceOpacity );

	QFrame *line = new QFrame();
	line->setFrameStyle( QFrame::Sunken | QFrame::HLine );

	QVBoxLayout *vBoxLayout = new QVBoxLayout( m_GroupBox );
	vBoxLayout->addLayout( gridLayout );
	vBoxLayout->addWidget( line );
//	vBoxLayout->addWidget( m_CheckBox_ClampToGround );
//	vBoxLayout->addWidget( m_CheckBox_ColorSliceAlpha );
//	vBoxLayout->addLayout( hBoxLayout );
	vBoxLayout->addLayout( hBoxLayout1 );
	vBoxLayout->addLayout( hBoxLayout2 );

	m_GroupBox->setDisabled( true );
}

void ColorSliceControl::setInitialValue()
{
	int slicePosX = static_cast< int >( param.getColorSlicePosX() * 100000.0 );
	int slicePosY = static_cast< int >( param.getColorSlicePosY() * 100000.0 );
	int slicePosZ = static_cast< int >( param.getColorSlicePosZ() * 100000.0 );
	int slicePosOpacity = static_cast< int >( param.getColorSliceOpacity() * 100.0 );

	if ( grads.getGridSizeZ() == 1 ) {
		m_CheckBox_ColorSliceX->hide();
		m_LineEdit_XValue->hide();
		m_HSlider_ColorSliceX->hide();
		m_CheckBox_ColorSliceY->hide();
		m_LineEdit_YValue->hide();
		m_HSlider_ColorSliceY->hide();
	} else {
		m_CheckBox_ColorSliceX->show();
		m_LineEdit_XValue->show();
		m_HSlider_ColorSliceX->show();
		m_CheckBox_ColorSliceY->show();
		m_LineEdit_YValue->show();
		m_HSlider_ColorSliceY->show();
	}
	if ( param.IsColorSliceX() ) {
		m_CheckBox_ColorSliceX->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_ColorSliceX->setCheckState( Qt::Unchecked );
	}
	if ( param.IsColorSliceY() ) {
		m_CheckBox_ColorSliceY->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_ColorSliceY->setCheckState( Qt::Unchecked );
	}
	if ( param.IsColorSliceZ() ) {
		m_CheckBox_ColorSliceZ->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_ColorSliceZ->setCheckState( Qt::Unchecked );
	}
	if ( param.IsColorSliceAlpha() ) {
		m_CheckBox_ColorSliceAlpha->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_ColorSliceAlpha->setCheckState( Qt::Unchecked );
	}
	if ( param.IsColorSliceClampToGround() ) {
		m_CheckBox_ClampToGround->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_ClampToGround->setCheckState( Qt::Unchecked );
	}
	m_HSlider_ColorSliceX->setValue( slicePosX );
	m_HSlider_ColorSliceY->setValue( slicePosY );
	m_HSlider_ColorSliceZ->setValue( slicePosZ );
	m_HSlider_ColorSliceOpacity->setValue( slicePosOpacity );
	setColorSliceXAxisLabel( slicePosX );
	setColorSliceYAxisLabel( slicePosY );
	setColorSliceZAxisLabel( slicePosZ );
}

void ColorSliceControl::setDisabled( bool state )
{
	m_GroupBox->setDisabled( state );
}

void ColorSliceControl::setColorSliceXAxisLabel( int colorSliceX )
{
	float ColorSlice_Height = 0.0;
	if ( grads.isDataLoaded() == true ) {
		ColorSlice_Height = grads.getGridXMin()
		+ ( grads.getGridXMax() - grads.getGridXMin() ) * ( float )colorSliceX / 100000.0;
	}
	m_LineEdit_XValue->setText( QString("%1").arg( ColorSlice_Height, 0, 'f', 2 ) );
	param.setColorSlicePosX( ( float )colorSliceX / 100000.0 );
}

void ColorSliceControl::setColorSliceYAxisLabel( int colorSliceY )
{
	float ColorSlice_Height = 0.0;
	if ( grads.isDataLoaded() == true ) {
		ColorSlice_Height = grads.getGridYMin()
		+ ( grads.getGridYMax() - grads.getGridYMin() ) * ( float )colorSliceY / 100000.0;
	}
	m_LineEdit_YValue->setText( QString("%1").arg( ColorSlice_Height, 0, 'f', 2 ) );
	param.setColorSlicePosY( ( float )colorSliceY / 100000.0 );
}

void ColorSliceControl::setColorSliceZAxisLabel( int colorSliceZ )
{
	float ColorSlice_Height = 0.0;
	if ( grads.isDataLoaded() == true ) {
		ColorSlice_Height = grads.getGridZMin()
		+ ( grads.getGridZMax() - grads.getGridZMin() ) * ( float )colorSliceZ / 100000.0;
	}
	m_LineEdit_ZValue->setText( QString("%1").arg( ColorSlice_Height, 0, 'f', 2 ) );
	param.setColorSlicePosZ( ( float )colorSliceZ / 100000.0 );
}

void ColorSliceControl::setColorSliceOpacity( int value )
{
	param.setColorSliceOpacity( static_cast< float >( value ) / 100.0 );
}

void ColorSliceControl::setStateColorSliceX( bool state )
{
	m_LineEdit_XValue->setDisabled( !state );
	m_HSlider_ColorSliceX->setDisabled( !state );
	param.setStateColorSliceX( state );
}

void ColorSliceControl::setStateColorSliceY( bool state )
{
	m_LineEdit_YValue->setDisabled( !state );
	m_HSlider_ColorSliceY->setDisabled( !state );
	param.setStateColorSliceY( state );
}

void ColorSliceControl::setStateColorSliceZ( bool state )
{
	m_LineEdit_ZValue->setDisabled( !state );
	m_HSlider_ColorSliceZ->setDisabled( !state );
	param.setStateColorSliceZ( state );
}

void ColorSliceControl::setStateColorSliceAlpha( bool state )
{
	param.setStateColorSliceAlpha( state );
}

void ColorSliceControl::setStateClampToGround( bool state )
{
	param.setStateColorSliceClampToGround( state );
}

bool ColorSliceControl::eventFilter( QObject *obj, QEvent *event )
{
	if ( event->type() == QEvent::MouseButtonPress ) {
		if ( obj == m_LineEdit_XValue ) {
			m_String_XValue_Backup = m_LineEdit_XValue->text();
			return true;
		} else if ( obj == m_LineEdit_YValue ) {
			m_String_YValue_Backup = m_LineEdit_YValue->text();
			return true;
		} else if ( obj == m_LineEdit_ZValue ) {
			m_String_ZValue_Backup = m_LineEdit_ZValue->text();
			return true;
		}
	}
	return false;
}

void ColorSliceControl::setColorSliceXValue()
{
	double min = grads.getGridXMin();
	double max = grads.getGridXMax();
	double inputValue = ( m_LineEdit_XValue->text() ).toDouble();
	if ( ( inputValue >= min ) && ( inputValue <= max ) ) {
		m_String_XValue_Backup = m_LineEdit_XValue->text();
		float normalizedValue =
			static_cast< float >( ( inputValue - min ) / ( max - min ) );
		param.setColorSlicePosX( normalizedValue );
		m_HSlider_ColorSliceX->setValue( static_cast< int >( normalizedValue * 100000.0 ) );
	} else {
		QString msg;
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
		msg.asprintf( "The input value must be a number between %f and %f.",
			min, max );
#else
		msg.sprintf( "The input value must be a number between %f and %f.",
					min, max );
#endif
		QMessageBox::StandardButton mbox;
		mbox = QMessageBox::information( this, tr( "Warning" ), msg );
		m_LineEdit_XValue->setText( m_String_XValue_Backup );
	}
}

void ColorSliceControl::setColorSliceYValue()
{
	double min = grads.getGridYMin();
	double max = grads.getGridYMax();
	double inputValue = ( m_LineEdit_YValue->text() ).toDouble();
	if ( ( inputValue >= min ) && ( inputValue <= max ) ) {
		m_String_YValue_Backup = m_LineEdit_YValue->text();
		float normalizedValue =
			static_cast< float >( ( inputValue - min ) / ( max - min ) );
		param.setColorSlicePosY( normalizedValue );
		m_HSlider_ColorSliceY->setValue( static_cast< int >( normalizedValue * 100000.0 ) );
	} else {
		QString msg;
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
		msg.asprintf( "The input value must be a number between %f and %f.",
			min, max );
#else
		msg.sprintf( "The input value must be a number between %f and %f.",
					min, max );
#endif
		QMessageBox::StandardButton mbox;
		mbox = QMessageBox::information( this, tr( "Warning" ), msg );
		m_LineEdit_YValue->setText( m_String_YValue_Backup );
	}
}

void ColorSliceControl::setColorSliceZValue()
{
	double min = grads.getGridZMin();
	double max = grads.getGridZMax();
	double inputValue = ( m_LineEdit_ZValue->text() ).toDouble();
	if ( ( inputValue >= min ) && ( inputValue <= max ) ) {
		m_String_ZValue_Backup = m_LineEdit_ZValue->text();
		float normalizedValue =
			static_cast< float >( ( inputValue - min ) / ( max - min ) );
		param.setColorSlicePosZ( normalizedValue );
		m_HSlider_ColorSliceZ->setValue( static_cast< int >( normalizedValue * 100000.0 ) );
	} else {
		QString msg;
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
		msg.asprintf( "The input value must be a number between %f and %f.",
			min, max );
#else
		msg.sprintf( "The input value must be a number between %f and %f.",
					min, max );
#endif
		QMessageBox::StandardButton mbox;
		mbox = QMessageBox::information( this, tr( "Warning" ), msg );
		m_LineEdit_ZValue->setText( m_String_ZValue_Backup );
	}
}
