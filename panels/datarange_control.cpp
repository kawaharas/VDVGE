//
// Volume Data Visualizer for Google Earth
//
// datarange_control.cpp
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

#include "datarange_control.h"

DataRangeControl::DataRangeControl( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

	connect( m_LineEdit_Min, SIGNAL( returnPressed() ),
			 this, SLOT( setMin() ) );
	connect( m_LineEdit_Max, SIGNAL( returnPressed() ),
			 this, SLOT( setMax() ) );
	connect( m_Button_ResetMin, SIGNAL( pressed() ),
			 this, SLOT( resetMin() ) );
	connect( m_Button_ResetMax, SIGNAL( pressed() ),
			 this, SLOT( resetMax() ) );
	connect( m_CheckBox_LogScale, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateLogScale( bool ) ) );
}

DataRangeControl::~DataRangeControl()
{
	m_LineEdit_Min->removeEventFilter( this );
	m_LineEdit_Max->removeEventFilter( this );
}

void DataRangeControl::makeWidget()
{
	m_GroupBox = new QGroupBox();
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox->setTitle( "Data Range" );

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
	QDoubleValidator *validator = new QDoubleValidator( this );
	validator->setNotation( QDoubleValidator::ScientificNotation );
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

	m_CheckBox_LogScale = new QCheckBox( "Log Scale", 0 );

	QSpacerItem *spacer = new QSpacerItem( 100, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );

	QGridLayout *gridLayout = new QGridLayout( m_GroupBox );
	gridLayout->addWidget( label_Min,         0, 0 );
	gridLayout->addWidget( m_LineEdit_Min,    0, 1 );
	gridLayout->addWidget( m_Button_ResetMin, 0, 2 );
	gridLayout->addItem( spacer, 0, 3 );
	gridLayout->addWidget( label_Max,         1, 0 );
	gridLayout->addWidget( m_LineEdit_Max,    1, 1 );
	gridLayout->addWidget( m_Button_ResetMax, 1, 2 );
	gridLayout->addWidget( m_CheckBox_LogScale, 1, 3 );

	m_GroupBox->setDisabled( true );
}

void DataRangeControl::setInitialValue()
{
	m_LineEdit_Min->setText( QString("%1").arg( param.getVolumeThresholdMin() ) );
	m_LineEdit_Max->setText( QString("%1").arg( param.getVolumeThresholdMax() ) );
	if ( param.IsLogScale() ) {
		m_CheckBox_LogScale->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_LogScale->setCheckState( Qt::Unchecked );
	}
}

void DataRangeControl::setDisabled( bool state )
{
	m_GroupBox->setDisabled( state );
}

bool DataRangeControl::eventFilter( QObject *obj, QEvent *event )
{
	if ( event->type() == QEvent::MouseButtonPress ) {
		if ( obj == m_LineEdit_Min ) {
			m_String_Min_Backup = m_LineEdit_Min->text();
			return true;
		} else if ( obj == m_LineEdit_Max ) {
			m_String_Max_Backup = m_LineEdit_Max->text();
			return true;
		}
	}
	return false;
}

void DataRangeControl::setMin()
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
//		QMessageBox::StandardButton mbox;
//		mbox = QMessageBox::information( this, tr( "Warning" ), msg );
		QMessageBox::information( this, tr( "Warning" ), msg );
		m_LineEdit_Min->setText( m_String_Min_Backup );
	} else {
		m_String_Min_Backup = m_LineEdit_Min->text();
		param.setVolumeThresholdMin( inputValue );
		emit valueChanged( CHANGE_DATARANGE );
	}
}

void DataRangeControl::setMax()
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
//		QMessageBox::StandardButton mbox;
//		mbox = QMessageBox::information( this, tr( "Warning" ), msg );
		QMessageBox::information( this, tr( "Warning" ), msg );
		m_LineEdit_Max->setText( m_String_Max_Backup );
	} else {
		m_String_Max_Backup = m_LineEdit_Max->text();
		param.setVolumeThresholdMax( inputValue );
		emit valueChanged( CHANGE_DATARANGE );
	}
}

void DataRangeControl::resetMin()
{
	m_LineEdit_Min->setText( QString("%1").arg( grads.getMin() ) );
	param.setVolumeThresholdMin( grads.getMin() );
	emit valueChanged( CHANGE_DATARANGE );
}

void DataRangeControl::resetMax()
{
	m_LineEdit_Max->setText( QString("%1").arg( grads.getMax() ) );
	param.setVolumeThresholdMax( grads.getMax() );
	emit valueChanged( CHANGE_DATARANGE );
}

void DataRangeControl::setStateLogScale( bool state )
{
	if ( ( state == true ) && ( param.getVolumeThresholdMin() <= 0.0 ) ) {
		QString msg;
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
		msg.asprintf( "The min value must be a number over zero." );
#else
		msg.sprintf( "The min value must be a number over zero." );
#endif
		QMessageBox::StandardButton mbox;
		mbox = QMessageBox::information( this, tr( "Warning" ), msg );
		m_CheckBox_LogScale->setCheckState( Qt::Unchecked );
	} else {
		param.setStateLogScale( state );
		emit valueChanged( CHANGE_DATARANGE );
	}
}
