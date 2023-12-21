//
// Volume Data Visualizer for Google Earth
//
// timerange_control.cpp
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

#include "timerange_control.h"

TimeRangeControl::TimeRangeControl( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

	connect( m_LineEdit_Begin, SIGNAL( returnPressed() ),
			 this, SLOT( setBegin() ) );
	connect( m_LineEdit_End, SIGNAL( returnPressed() ),
			 this, SLOT( setEnd() ) );
	connect( m_LineEdit_Skip, SIGNAL( returnPressed() ),
			 this, SLOT( setSkip() ) );
	connect( m_CheckBox_TimeConversionToUTC, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateTimeConversionToUTC( bool ) ) );
	connect( m_LineEdit_TimeConversionToUTC, SIGNAL( returnPressed() ),
			 this, SLOT( setTimeConversionToUTC() ) );
}

TimeRangeControl::~TimeRangeControl()
{
	m_LineEdit_Begin->removeEventFilter( this );
	m_LineEdit_End->removeEventFilter( this );
	m_LineEdit_Skip->removeEventFilter( this );
	m_LineEdit_TimeConversionToUTC->removeEventFilter( this );
}

void TimeRangeControl::makeWidget()
{
	m_GroupBox = new QGroupBox();
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox->setTitle( "Time Range" );

	QLabel *label_Begin = new QLabel( tr( "Begin: " ) );
	label_Begin->setMinimumWidth( 40 );
	QLabel *label_End = new QLabel( tr( "End: " ) );
	label_End->setMinimumWidth( 40 );
	QLabel *label_Skip = new QLabel( tr( "   TimeSkip: " ) );
	label_Skip->setMinimumWidth( 40 );

	m_LineEdit_Begin = new QLineEdit( this );
	m_LineEdit_Begin->setAlignment( Qt::AlignRight );
	m_LineEdit_Begin->setMinimumWidth( 60 );
	m_LineEdit_Begin->setMaximumWidth( 60 );
	m_LineEdit_End = new QLineEdit( this );
	m_LineEdit_End->setAlignment( Qt::AlignRight );
	m_LineEdit_End->setMinimumWidth( 60 );
	m_LineEdit_End->setMaximumWidth( 60 );
	m_LineEdit_Skip = new QLineEdit( this );
	m_LineEdit_Skip->setAlignment( Qt::AlignRight );
	m_LineEdit_Skip->setMinimumWidth( 60 );
	m_LineEdit_Skip->setMaximumWidth( 60 );
	m_LineEdit_TimeConversionToUTC = new QLineEdit( this );
	m_LineEdit_TimeConversionToUTC->setAlignment( Qt::AlignRight );
	m_LineEdit_TimeConversionToUTC->setMinimumWidth( 60 );
	m_LineEdit_TimeConversionToUTC->setMaximumWidth( 60 );
	QValidator *validator = new QIntValidator( this );
	m_LineEdit_Begin->setValidator( validator );
	m_LineEdit_End->setValidator( validator );
	m_LineEdit_Skip->setValidator( validator );
//	m_LineEdit_TimeConversionToUTC->setValidator( validator );
	m_LineEdit_Begin->installEventFilter( this );
	m_LineEdit_End->installEventFilter( this );
	m_LineEdit_Skip->installEventFilter( this );
	m_LineEdit_TimeConversionToUTC->installEventFilter( this );

	m_Label_BeginStrings = new QLabel( tr("") );
	m_Label_BeginStrings->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_Label_BeginStrings->setMinimumWidth( 130 );
	m_Label_BeginStrings->setMaximumWidth( 130 );
	m_Label_EndStrings = new QLabel( tr("") );
	m_Label_EndStrings->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_Label_EndStrings->setMinimumWidth( 130 );
	m_Label_EndStrings->setMaximumWidth( 130 );

	QSpacerItem *spacer1 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );
	QSpacerItem *spacer2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );

	m_CheckBox_TimeConversionToUTC = new QCheckBox( "Time Conversion To UTC:", 0 );
	m_CheckBox_TimeConversionToUTC->setCheckState( Qt::Unchecked );

//	QGridLayout *gridLayout = new QGridLayout( m_GroupBox );
	QGridLayout *gridLayout = new QGridLayout();
	gridLayout->addWidget( label_Begin, 0, 0 );
	gridLayout->addWidget( m_LineEdit_Begin, 0, 1 );
	gridLayout->addWidget( m_Label_BeginStrings, 0, 2 );
	gridLayout->addItem( spacer1, 0, 3 );
	gridLayout->addWidget( label_End, 1, 0 );
	gridLayout->addWidget( m_LineEdit_End, 1, 1 );
	gridLayout->addWidget( m_Label_EndStrings, 1, 2 );
	gridLayout->addWidget( label_Skip, 1, 3 );
	gridLayout->addWidget( m_LineEdit_Skip, 1, 4 );

	QHBoxLayout *hBoxLayout = new QHBoxLayout();
	hBoxLayout->addWidget( m_CheckBox_TimeConversionToUTC );
	hBoxLayout->addWidget( m_LineEdit_TimeConversionToUTC );
	hBoxLayout->addItem( spacer2 );

	QFrame *line = new QFrame();
	line->setFrameStyle( QFrame::Sunken | QFrame::HLine );

	QVBoxLayout *vBoxLayout = new QVBoxLayout( m_GroupBox );
	vBoxLayout->addLayout( gridLayout );
	vBoxLayout->addWidget( line );
	vBoxLayout->addLayout( hBoxLayout );

	m_GroupBox->setDisabled( true );
}

void TimeRangeControl::setInitialValue()
{
	m_LineEdit_Begin->setText( QString("%1").arg( param.getTimeBegin() ) );
	m_LineEdit_End->setText( QString("%1").arg( param.getTimeEnd() ) );
	m_LineEdit_Skip->setText( QString("%1").arg( param.getTimeSkip() ) );
	m_LineEdit_TimeConversionToUTC->setText( param.getTimeConversionToUTC() );
	m_Label_BeginStrings->setText( getTimeString( getActualTime( param.getTimeBegin() ) ) );
	m_Label_EndStrings->setText( getTimeString( getActualTime( param.getTimeEnd() ) ) );
	m_CheckBox_TimeConversionToUTC->setChecked( param.IsTimeConversionToUTC() );
}

void TimeRangeControl::setDisabled( bool state )
{
	m_GroupBox->setDisabled( state );
}

bool TimeRangeControl::eventFilter( QObject *obj, QEvent *event )
{
	if ( event->type() == QEvent::MouseButtonPress ) {
		if ( obj == m_LineEdit_Begin ) {
			m_String_Begin_Backup = m_LineEdit_Begin->text();
			return true;
		} else if ( obj == m_LineEdit_End ) {
			m_String_End_Backup = m_LineEdit_End->text();
			return true;
		} else if ( obj == m_LineEdit_Skip ) {
			m_String_Skip_Backup = m_LineEdit_Skip->text();
			return true;
		} else if ( obj == m_LineEdit_TimeConversionToUTC ) {
			m_String_TimeConversionToUTC_Backup
				= m_LineEdit_TimeConversionToUTC->text();
			return true;
		}
	}
	return false;
}

void TimeRangeControl::setBegin()
{
	int inputValue = ( m_LineEdit_Begin->text() ).toInt();
	int maxValue   = ( m_LineEdit_End->text() ).toInt();
	if ( ( inputValue < 0 ) || ( inputValue > maxValue ) ) {
		QString msg;
#if QT_VERSION >= 0x050600 // Qt 5.6 or over
		msg.asprintf( "StartStep must be a number between 0 and %d.", maxValue );
#else
		msg.sprintf( "StartStep must be a number between 0 and %d.", maxValue );
#endif
		QMessageBox::StandardButton mbox;
		mbox = QMessageBox::information( this, tr( "Warning" ), msg );
		m_LineEdit_Begin->setText( m_String_Begin_Backup );
	} else {
		m_String_Begin_Backup = m_LineEdit_Begin->text();
		param.setTimeBegin( inputValue );
		m_Label_BeginStrings->setText( getTimeString( getActualTime( inputValue ) ) );
	}
}

void TimeRangeControl::setEnd()
{
	int inputValue = ( m_LineEdit_End->text() ).toInt();
	int minValue   = ( m_LineEdit_Begin->text() ).toInt();
	if ( ( inputValue < minValue ) || ( inputValue >= grads.getGridSizeT() ) ) {
		QString msg;
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
		msg.asprintf( "EndStep must be a number between %d and %d.",
					 minValue, grads.getGridSizeT() - 1 );
#else
		msg.sprintf( "EndStep must be a number between %d and %d.",
					minValue, grads.getGridSizeT() - 1 );
#endif
		QMessageBox::StandardButton mbox;
		mbox = QMessageBox::information( this, tr( "Warning" ), msg );
		m_LineEdit_End->setText( m_String_End_Backup );
	} else {
		m_String_End_Backup = m_LineEdit_End->text();
		param.setTimeEnd( inputValue );
		m_Label_EndStrings->setText( getTimeString( getActualTime( inputValue ) ) );
	}
}

void TimeRangeControl::setSkip()
{
	int inputValue = ( m_LineEdit_Skip->text() ).toInt();
	if ( ( inputValue < 1 ) || ( inputValue >= grads.getGridSizeT() ) ) {
		QString msg;
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
		msg.asprintf( "TimeSkip must be a number between 1 and %d.",
					 grads.getGridSizeT() - 1 );
#else
		msg.sprintf( "TimeSkip must be a number between 1 and %d.",
					grads.getGridSizeT() - 1 );
#endif
		QMessageBox::StandardButton mbox;
		mbox = QMessageBox::information( this, tr( "Warning" ), msg );
		m_LineEdit_Skip->setText( m_String_Skip_Backup );
	} else {
		m_String_Skip_Backup = m_LineEdit_Skip->text();
		param.setTimeSkip( inputValue );
	}
}

void TimeRangeControl::setStateTimeConversionToUTC( bool state )
{
	param.setStateTimeConversionToUTC( state );
}

void TimeRangeControl::setTimeConversionToUTC()
{
	bool err = false;
	bool plus = true;
	int  hour = 0;
	int  minute = 0;
	QString inputString
		= m_LineEdit_TimeConversionToUTC->text();
	if ( inputString.at(0) == '+' ) {
		inputString.remove( 0, 1 );
	} else if ( inputString.at(0) == '-' ) {
		inputString.remove( 0, 1 );
		plus = false;
	}
	QStringList list = inputString.split( ":" );
	if ( list.size() == 2 ) {
		bool ok1, ok2;
		hour   = list.at( 0 ).toInt( &ok1 );
		minute = list.at( 1 ).toInt( &ok2 );
		if ( !ok1 || !ok2 ) {
			err = true;
		} else if ( hour > 24 ) {
			err = true;
		} else if ( minute > 60 ) {
			err = true;
		} else if ( ( hour * 100 + minute ) > 2400 ) {
			err = true;
		}
	} else {
		err = true;
	}
	QString fmtStr = QString( "%1:%2" )
		.arg( hour,   2, 10, QChar( '0' ) )
		.arg( minute, 2, 10, QChar( '0' ) );
	if ( plus ) {
		fmtStr.prepend( "+" );
	} else {
		fmtStr.prepend( "-" );
	}

	if ( err ) {
		QString msg;
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
		msg.asprintf( "Format Error: Time Conversion To UTC" );
#else
		msg.sprintf( "Format Error: Time Conversion To UTC" );
#endif
		QMessageBox::StandardButton mbox;
		mbox = QMessageBox::information( this, tr( "Warning" ), msg );
		m_LineEdit_TimeConversionToUTC->setText(
			m_String_TimeConversionToUTC_Backup );
	} else {
		m_String_TimeConversionToUTC_Backup = fmtStr;
		m_LineEdit_TimeConversionToUTC->setText( fmtStr );
		param.setTimeConversionToUTC( fmtStr );
	}
}
