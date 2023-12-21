//
// Volume Data Visualizer for Google Earth
//
// contourline_control.cpp
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

#include "contourline_control.h"

ContourLineControl::ContourLineControl( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

	connect( m_CheckBox_ContourLineX, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateContourLineX( bool ) ) );
	connect( m_CheckBox_ContourLineY, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateContourLineY( bool ) ) );
	connect( m_CheckBox_ContourLineZ, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateContourLineZ( bool ) ) );
	connect( m_HSlider_ContourLineX, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setContourLineXAxisLabel( int ) ) );
	connect( m_HSlider_ContourLineY, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setContourLineYAxisLabel( int ) ) );
	connect( m_HSlider_ContourLineZ, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setContourLineZAxisLabel( int ) ) );
	connect( m_LineEdit_XValue, SIGNAL( returnPressed() ),
			 this, SLOT( setContourLineXValue() ) );
	connect( m_LineEdit_YValue, SIGNAL( returnPressed() ),
			 this, SLOT( setContourLineYValue() ) );
	connect( m_LineEdit_ZValue, SIGNAL( returnPressed() ),
			 this, SLOT( setContourLineZValue() ) );
	connect( m_LineEdit_Min, SIGNAL( returnPressed() ),
			 this, SLOT( setMin() ) );
	connect( m_LineEdit_Max, SIGNAL( returnPressed() ),
			 this, SLOT( setMax() ) );
	connect( m_Button_ResetMin, SIGNAL( pressed() ),
			 this, SLOT( resetMin() ) );
	connect( m_Button_ResetMax, SIGNAL( pressed() ),
			 this, SLOT( resetMax() ) );
	connect( m_LineEdit_Interval, SIGNAL( returnPressed() ),
			 this, SLOT( setInterval() ) );
	connect( m_SpinBox_LineWidth, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setLineWidth( int ) ) );
}

ContourLineControl::~ContourLineControl()
{
	m_LineEdit_Min->removeEventFilter( this );
	m_LineEdit_Max->removeEventFilter( this );
	m_LineEdit_Interval->removeEventFilter( this );
}

void ContourLineControl::makeWidget()
{
	m_GroupBox = new QGroupBox();
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox->setTitle( "Contour Line" );

	m_CheckBox_ContourLineX = new QCheckBox( "X-Axis:", 0 );
	m_CheckBox_ContourLineY = new QCheckBox( "Y-Axis:", 0 );
	m_CheckBox_ContourLineZ = new QCheckBox( "Z-Axis:", 0 );
	m_CheckBox_ContourLineX->setCheckState( Qt::Unchecked );
	m_CheckBox_ContourLineY->setCheckState( Qt::Unchecked );
	m_CheckBox_ContourLineZ->setCheckState( Qt::Unchecked );

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

	m_HSlider_ContourLineX = new QSlider( Qt::Horizontal, 0 );
	m_HSlider_ContourLineY = new QSlider( Qt::Horizontal, 0 );
	m_HSlider_ContourLineZ = new QSlider( Qt::Horizontal, 0 );
	m_HSlider_ContourLineX->setMinimum(   0 );
	m_HSlider_ContourLineY->setMinimum(   0 );
	m_HSlider_ContourLineZ->setMinimum(   0 );
	m_HSlider_ContourLineX->setMaximum( 100000 );
	m_HSlider_ContourLineY->setMaximum( 100000 );
	m_HSlider_ContourLineZ->setMaximum( 100000 );
	m_HSlider_ContourLineX->setSingleStep( 1000 );
	m_HSlider_ContourLineY->setSingleStep( 1000 );
	m_HSlider_ContourLineZ->setSingleStep( 1000 );
	m_HSlider_ContourLineX->setValue( 0 );
	m_HSlider_ContourLineY->setValue( 0 );
	m_HSlider_ContourLineZ->setValue( 0 );
	m_HSlider_ContourLineX->setDisabled( true );
	m_HSlider_ContourLineY->setDisabled( true );
	m_HSlider_ContourLineZ->setDisabled( true );

	QGridLayout *gridLayout = new QGridLayout();
//	gridLayout->setVerticalSpacing( 10 );
	gridLayout->addWidget( m_CheckBox_ContourLineX, 0, 0 );
	gridLayout->addWidget( m_CheckBox_ContourLineY, 1, 0 );
	gridLayout->addWidget( m_CheckBox_ContourLineZ, 2, 0 );
	gridLayout->addWidget( m_LineEdit_XValue, 0, 1 );
	gridLayout->addWidget( m_LineEdit_YValue, 1, 1 );
	gridLayout->addWidget( m_LineEdit_ZValue, 2, 1 );
	gridLayout->addWidget( m_HSlider_ContourLineX, 0, 2 );
	gridLayout->addWidget( m_HSlider_ContourLineY, 1, 2 );
	gridLayout->addWidget( m_HSlider_ContourLineZ, 2, 2 );

	QFrame *line = new QFrame();
	line->setFrameStyle( QFrame::Sunken | QFrame::HLine );

	QLabel *label_Min = new QLabel( tr( "Min Value: " ) );
	label_Min->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
//	label_Min->setMinimumWidth( 70 );
//	label_Min->setMaximumWidth( 70 );
	QLabel *label_Max = new QLabel( tr( "Max Value: " ) );
	label_Max->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
//	label_Max->setMinimumWidth( 70 );
//	label_Max->setMaximumWidth( 70 );
	m_LineEdit_Min = new QLineEdit( this );
	m_LineEdit_Min->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_LineEdit_Max = new QLineEdit( this );
	m_LineEdit_Max->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_LineEdit_Min->setValidator( validator );
	m_LineEdit_Max->setValidator( validator );
	m_LineEdit_Min->installEventFilter( this );
	m_LineEdit_Max->installEventFilter( this );
	m_Button_ResetMin = new QPushButton( tr( "Reset" ) );
	m_Button_ResetMax = new QPushButton( tr( "Reset" ) );
	m_Button_ResetMin->setMaximumWidth( 50 );
	m_Button_ResetMax->setMaximumWidth( 50 );
	m_Button_ResetMin->setMaximumHeight( 22 );
	m_Button_ResetMax->setMaximumHeight( 22 );
	m_Button_ResetMin->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	m_Button_ResetMax->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

	QLabel *label_Interval = new QLabel( tr( "Interval: " ) );
	label_Interval->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
//	label_Interval->setMinimumWidth( 70 );
//	label_Interval->setMaximumWidth( 70 );
	m_LineEdit_Interval = new QLineEdit( this );
	m_LineEdit_Interval->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
//	m_LineEdit_Interval->setMinimumWidth( 45 );
//	m_LineEdit_Interval->setMaximumWidth( 45 );
	m_LineEdit_Interval->setValidator( validator );
	m_LineEdit_Interval->installEventFilter( this );

	QLabel *label_LineWidth = new QLabel( tr( "Line Width: " ) );
	label_LineWidth->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
//	label_LineWidth->setMinimumWidth( 70 );
//	label_LineWidth->setMaximumWidth( 70 );
	m_SpinBox_LineWidth = new QSpinBox();
	m_SpinBox_LineWidth->setAlignment( Qt::AlignRight );
	m_SpinBox_LineWidth->setRange( 1, 3 );
	m_SpinBox_LineWidth->setValue( 1 );

	QSpacerItem *spacer = new QSpacerItem( 30, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );

	QGridLayout *gridLayout2 = new QGridLayout();
	gridLayout2->addWidget( label_Min,           0, 0 );
	gridLayout2->addWidget( m_LineEdit_Min,      0, 1 );
	gridLayout2->addWidget( m_Button_ResetMin,   0, 2 );
	gridLayout2->addItem(   spacer,              0, 3 );
	gridLayout2->addWidget( label_LineWidth,     0, 4 );
	gridLayout2->addWidget( m_SpinBox_LineWidth, 0, 5 );
	gridLayout2->addWidget( label_Max,           1, 0 );
	gridLayout2->addWidget( m_LineEdit_Max,      1, 1 );
	gridLayout2->addWidget( m_Button_ResetMax,   1, 2 );
	gridLayout2->addWidget( label_Interval,      2, 0 );
	gridLayout2->addWidget( m_LineEdit_Interval, 2, 1 );

	QVBoxLayout *vBoxLayout = new QVBoxLayout( m_GroupBox );
	vBoxLayout->addLayout( gridLayout );
	vBoxLayout->addWidget( line );
	vBoxLayout->addLayout( gridLayout2 );

	m_GroupBox->setDisabled( true );
}

void ContourLineControl::setInitialValue()
{
	int slicePosX = static_cast< int >( param.getContourLinePosX() * 100000.0 );
	int slicePosY = static_cast< int >( param.getContourLinePosY() * 100000.0 );
	int slicePosZ = static_cast< int >( param.getContourLinePosZ() * 100000.0 );

	if ( grads.getGridSizeZ() == 1 ) {
		m_CheckBox_ContourLineX->hide();
		m_LineEdit_XValue->hide();
		m_HSlider_ContourLineX->hide();
		m_CheckBox_ContourLineY->hide();
		m_LineEdit_YValue->hide();
		m_HSlider_ContourLineY->hide();
	} else {
		m_CheckBox_ContourLineX->show();
		m_LineEdit_XValue->show();
		m_HSlider_ContourLineX->show();
		m_CheckBox_ContourLineY->show();
		m_LineEdit_YValue->show();
		m_HSlider_ContourLineY->show();
	}
	if ( param.IsContourLineX() ) {
		m_CheckBox_ContourLineX->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_ContourLineX->setCheckState( Qt::Unchecked );
	}
	if ( param.IsContourLineY() ) {
		m_CheckBox_ContourLineY->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_ContourLineY->setCheckState( Qt::Unchecked );
	}
	if ( param.IsContourLineZ() ) {
		m_CheckBox_ContourLineZ->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_ContourLineZ->setCheckState( Qt::Unchecked );
	}
	m_HSlider_ContourLineX->setValue( slicePosX );
	m_HSlider_ContourLineY->setValue( slicePosY );
	m_HSlider_ContourLineZ->setValue( slicePosZ );
	setContourLineXAxisLabel( slicePosX );
	setContourLineYAxisLabel( slicePosY );
	setContourLineZAxisLabel( slicePosZ );
	m_SpinBox_LineWidth->setValue( 1 );

	m_LineEdit_Min->setText( QString("%1").arg( param.getContourLineThresholdMin() ) );
	m_LineEdit_Max->setText( QString("%1").arg( param.getContourLineThresholdMax() ) );
	m_LineEdit_Interval->setText( QString("%1").arg( param.getContourLineInterval() ) );
}

void ContourLineControl::setDisabled( bool state )
{
	m_GroupBox->setDisabled( state );
}

void ContourLineControl::setContourLineXAxisLabel( int colorSliceX )
{
	float ContourLine_Height = 0.0;
	if ( grads.isDataLoaded() == true ) {
		ContourLine_Height = grads.getGridXMin()
		+ ( grads.getGridXMax() - grads.getGridXMin() ) * ( float )colorSliceX / 100000.0;
	}
	m_LineEdit_XValue->setText( QString("%1").arg( ContourLine_Height, 0, 'f', 2 ) );
	param.setContourLinePosX( ( float )colorSliceX / 100000.0 );
	emit valueChanged( CHANGE_CONTOUR_LINE_PARAM );
}

void ContourLineControl::setContourLineYAxisLabel( int colorSliceY )
{
	float ContourLine_Height = 0.0;
	if ( grads.isDataLoaded() == true ) {
		ContourLine_Height = grads.getGridYMin()
		+ ( grads.getGridYMax() - grads.getGridYMin() ) * ( float )colorSliceY / 100000.0;
	}
	m_LineEdit_YValue->setText( QString("%1").arg( ContourLine_Height, 0, 'f', 2 ) );
	param.setContourLinePosY( ( float )colorSliceY / 100000.0 );
	emit valueChanged( CHANGE_CONTOUR_LINE_PARAM );
}

void ContourLineControl::setContourLineZAxisLabel( int colorSliceZ )
{
	float ContourLine_Height = 0.0;
	if ( grads.isDataLoaded() == true ) {
		ContourLine_Height = grads.getGridZMin()
		+ ( grads.getGridZMax() - grads.getGridZMin() ) * ( float )colorSliceZ / 100000.0;
	}
	m_LineEdit_ZValue->setText( QString("%1").arg( ContourLine_Height, 0, 'f', 2 ) );
	param.setContourLinePosZ( ( float )colorSliceZ / 100000.0 );
	emit valueChanged( CHANGE_CONTOUR_LINE_PARAM );
}

void ContourLineControl::setStateContourLineX( bool state )
{
	m_LineEdit_XValue->setDisabled( !state );
	m_HSlider_ContourLineX->setDisabled( !state );
	param.setStateContourLineX( state );
}

void ContourLineControl::setStateContourLineY( bool state )
{
	m_LineEdit_YValue->setDisabled( !state );
	m_HSlider_ContourLineY->setDisabled( !state );
	param.setStateContourLineY( state );
}

void ContourLineControl::setStateContourLineZ( bool state )
{
	m_LineEdit_ZValue->setDisabled( !state );
	m_HSlider_ContourLineZ->setDisabled( !state );
	param.setStateContourLineZ( state );
}

bool ContourLineControl::eventFilter( QObject *obj, QEvent *event )
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
		} else if ( obj == m_LineEdit_Min ) {
			m_String_Min_Backup = m_LineEdit_Min->text();
			return true;
		} else if ( obj == m_LineEdit_Max ) {
			m_String_Max_Backup = m_LineEdit_Max->text();
			return true;
		} else {
			return false;
		}
	}
	return false;
}

void ContourLineControl::setMin()
{
	float inputValue = ( m_LineEdit_Min->text() ).toFloat();
	float maxValue   = ( m_LineEdit_Max->text() ).toFloat();
	if ( inputValue > maxValue ) {
		QString msg;
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
		msg.asprintf( "The input value must be a number below %f.", maxValue );
#else
		msg.sprintf( "The input value must be a number below %f.", maxValue );
#endif
		QMessageBox::StandardButton mbox;
		mbox = QMessageBox::information( this, tr( "Warning" ), msg );
		m_LineEdit_Min->setText( m_String_Min_Backup );
	} else {
		m_String_Min_Backup = m_LineEdit_Min->text();
		param.setContourLineThresholdMin( inputValue );
		emit valueChanged( CHANGE_CONTOUR_LINE_PARAM );
	}
}

void ContourLineControl::setMax()
{
	float inputValue = ( m_LineEdit_Max->text() ).toFloat();
	float minValue   = ( m_LineEdit_Min->text() ).toFloat();
	if ( inputValue < minValue ) {
		QString msg;
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
		msg.asprintf( "The input value must be a number over %f.", minValue );
#else
		msg.sprintf( "The input value must be a number over %f.", minValue );
#endif
		QMessageBox::StandardButton mbox;
		mbox = QMessageBox::information( this, tr( "Warning" ), msg );
		m_LineEdit_Max->setText( m_String_Max_Backup );
	} else {
		m_String_Max_Backup = m_LineEdit_Max->text();
		param.setContourLineThresholdMax( inputValue );
		emit valueChanged( CHANGE_CONTOUR_LINE_PARAM );
	}
}

void ContourLineControl::resetMin()
{
	m_LineEdit_Min->setText( QString("%1").arg( param.getVolumeThresholdMin() ) );
	param.setContourLineThresholdMin( param.getVolumeThresholdMin() );
	emit valueChanged( CHANGE_CONTOUR_LINE_PARAM );
}

void ContourLineControl::resetMax()
{
	m_LineEdit_Max->setText( QString("%1").arg( param.getVolumeThresholdMax() ) );
	param.setContourLineThresholdMax( param.getVolumeThresholdMax() );
	emit valueChanged( CHANGE_CONTOUR_LINE_PARAM );
}

void ContourLineControl::setInterval()
{
	float inputValue = ( m_LineEdit_Interval->text() ).toFloat();
	if ( inputValue < 0.0 ) {
		QString msg;
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
		msg.asprintf( "Number of lines must be a number over 0." );
#else
		msg.sprintf( "Number of lines must be a number over 0." );
#endif
		QMessageBox::StandardButton mbox;
		mbox = QMessageBox::information( this, tr( "Warning" ), msg );
		m_LineEdit_Interval->setText( m_String_Interval_Backup );
	} else {
		m_String_Interval_Backup = m_LineEdit_Interval->text();
		param.setContourLineInterval( inputValue );
		emit valueChanged( CHANGE_CONTOUR_LINE_PARAM );
	}
}

void ContourLineControl::setLineWidth( int value )
{
	param.setContourLineWidth( static_cast< float >( value ) );
}

void ContourLineControl::changeThreshold()
{
	if ( param.getVolumeThresholdMin() > param.getContourLineThresholdMin() ) {
		param.setContourLineThresholdMin( param.getVolumeThresholdMin() );
		m_LineEdit_Min->setText( QString("%1").arg( param.getVolumeThresholdMin() ) );
	}
	if ( param.getVolumeThresholdMax() < param.getContourLineThresholdMax() ) {
		param.setContourLineThresholdMax( param.getVolumeThresholdMax() );
		m_LineEdit_Max->setText( QString("%1").arg( param.getVolumeThresholdMax() ) );
	}
	emit valueChanged( CHANGE_CONTOUR_LINE_PARAM );
}

void ContourLineControl::setContourLineXValue()
{
	double min = grads.getGridXMin();
	double max = grads.getGridXMax();
	double inputValue = ( m_LineEdit_XValue->text() ).toDouble();
	if ( ( inputValue >= min ) && ( inputValue <= max ) ) {
		m_String_XValue_Backup = m_LineEdit_XValue->text();
		float normalizedValue =
			static_cast< float >( ( inputValue - min ) / ( max - min ) );
		param.setContourLinePosX( normalizedValue );
		m_HSlider_ContourLineX->setValue( static_cast< int >( normalizedValue * 100000.0 ) );
		emit valueChanged( CHANGE_CONTOUR_LINE_PARAM );
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

void ContourLineControl::setContourLineYValue()
{
	double min = grads.getGridYMin();
	double max = grads.getGridYMax();
	double inputValue = ( m_LineEdit_YValue->text() ).toDouble();
	if ( ( inputValue >= min ) && ( inputValue <= max ) ) {
		m_String_YValue_Backup = m_LineEdit_YValue->text();
		float normalizedValue =
			static_cast< float >( ( inputValue - min ) / ( max - min ) );
		param.setContourLinePosY( normalizedValue );
		m_HSlider_ContourLineY->setValue( static_cast< int >( normalizedValue * 100000.0 ) );
		emit valueChanged( CHANGE_CONTOUR_LINE_PARAM );
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

void ContourLineControl::setContourLineZValue()
{
	double min = grads.getGridZMin();
	double max = grads.getGridZMax();
	double inputValue = ( m_LineEdit_ZValue->text() ).toDouble();
	if ( ( inputValue >= min ) && ( inputValue <= max ) ) {
		m_String_ZValue_Backup = m_LineEdit_ZValue->text();
		float normalizedValue =
			static_cast< float >( ( inputValue - min ) / ( max - min ) );
		param.setContourLinePosZ( normalizedValue );
		m_HSlider_ContourLineZ->setValue( static_cast< int >( normalizedValue * 100000.0 ) );
		emit valueChanged( CHANGE_CONTOUR_LINE_PARAM );
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
