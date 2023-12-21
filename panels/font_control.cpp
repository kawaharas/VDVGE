//
// Volume Data Visualizer for Google Earth
//
// font_control.cpp
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

#include "font_control.h"

FontControl::FontControl( QWidget *parent ) :
	QDialog( parent )
{
	makeWidget();

	connect( m_FontComboBox_ColorBarString, SIGNAL( currentFontChanged( QFont ) ),
			 this, SLOT( setFont() ) );
	connect( m_FontComboBox_ColorBarValue, SIGNAL( currentFontChanged( QFont ) ),
			 this, SLOT( setFont() ) );
	connect( m_FontComboBox_TimeDisplay, SIGNAL( currentFontChanged( QFont ) ),
			 this, SLOT( setFont() ) );
	connect( m_SpinBox_FontSize_ColorBarString, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setFont() ) );
	connect( m_SpinBox_FontSize_ColorBarValue, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setFont() ) );
	connect( m_SpinBox_FontSize_TimeDisplay, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setFont() ) );
	connect( m_Button_ColorBarString_Bold, SIGNAL( toggled( bool ) ),
			 this, SLOT( setFont() ) );
	connect( m_Button_ColorBarString_Italic, SIGNAL( toggled( bool ) ),
			 this, SLOT( setFont() ) );
	connect( m_Button_ColorBarValue_Bold, SIGNAL( toggled( bool ) ),
			 this, SLOT( setFont() ) );
	connect( m_Button_ColorBarValue_Italic, SIGNAL( toggled( bool ) ),
			 this, SLOT( setFont() ) );
	connect( m_Button_TimeDisplay_Bold, SIGNAL( toggled( bool ) ),
			 this, SLOT( setFont() ) );
	connect( m_Button_TimeDisplay_Italic, SIGNAL( toggled( bool ) ),
			 this, SLOT( setFont() ) );
}

FontControl::~FontControl()
{
}

void FontControl::makeWidget()
{
	setWindowTitle( tr( "Font Settings" ) );
	setWindowModality( Qt::ApplicationModal );

	QLabel *label_ColorBarString = new QLabel( tr( "ColorBar String: " ) );
	label_ColorBarString->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
	QLabel *label_ColorBarValue = new QLabel( tr( "ColorBar Value: " ) );
	label_ColorBarValue->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
	QLabel *label_TimeDisplay = new QLabel( tr( "TimeDisplay: " ) );
	label_TimeDisplay->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );

	m_FontComboBox_ColorBarString = new QFontComboBox;
	m_FontComboBox_ColorBarString->setCurrentFont( QFont( "helvetica" ) );
	m_FontComboBox_ColorBarString->setFontFilters( QFontComboBox::ScalableFonts );
	m_FontComboBox_ColorBarValue = new QFontComboBox;
	m_FontComboBox_ColorBarValue->setCurrentFont( QFont( "helvetica" ) );
	m_FontComboBox_ColorBarValue->setFontFilters( QFontComboBox::ScalableFonts );
	m_FontComboBox_TimeDisplay = new QFontComboBox;
	m_FontComboBox_TimeDisplay->setCurrentFont( QFont( "helvetica" ) );
	m_FontComboBox_TimeDisplay->setFontFilters( QFontComboBox::ScalableFonts );

	m_SpinBox_FontSize_ColorBarString = new QSpinBox();
	m_SpinBox_FontSize_ColorBarString->setAlignment( Qt::AlignRight );
	m_SpinBox_FontSize_ColorBarString->setMinimum( 6 );
	m_SpinBox_FontSize_ColorBarString->setValue( 12 );
	m_SpinBox_FontSize_ColorBarValue = new QSpinBox();
	m_SpinBox_FontSize_ColorBarValue->setAlignment( Qt::AlignRight );
	m_SpinBox_FontSize_ColorBarValue->setMinimum( 6 );
	m_SpinBox_FontSize_ColorBarValue->setValue( 12 );
	m_SpinBox_FontSize_TimeDisplay = new QSpinBox();
	m_SpinBox_FontSize_TimeDisplay->setAlignment( Qt::AlignRight );
	m_SpinBox_FontSize_TimeDisplay->setMinimum( 6 );
	m_SpinBox_FontSize_TimeDisplay->setValue( 12 );

	QFont font( "Courier" );
	font.setPixelSize( 12 );
	font.setBold( true );
	m_Button_ColorBarString_Bold   = new QPushButton( tr( "B" ) );
	m_Button_ColorBarValue_Bold    = new QPushButton( tr( "B" ) );
	m_Button_TimeDisplay_Bold      = new QPushButton( tr( "B" ) );
	m_Button_ColorBarString_Bold->setFont( font );
	m_Button_ColorBarValue_Bold->setFont( font );
	m_Button_TimeDisplay_Bold->setFont( font );
	font.setBold( false );
	font.setItalic( true );
	m_Button_ColorBarString_Italic = new QPushButton( tr( "I" ) );
	m_Button_ColorBarValue_Italic  = new QPushButton( tr( "I" ) );
	m_Button_TimeDisplay_Italic    = new QPushButton( tr( "I" ) );
	m_Button_ColorBarString_Italic->setFont( font );
	m_Button_ColorBarValue_Italic->setFont( font );
	m_Button_TimeDisplay_Italic->setFont( font );
	m_Button_ColorBarString_Bold->setMaximumWidth( 25 );
	m_Button_ColorBarValue_Bold->setMaximumWidth( 25 );
	m_Button_TimeDisplay_Bold->setMaximumWidth( 25 );
	m_Button_ColorBarString_Italic->setMaximumWidth( 25 );
	m_Button_ColorBarValue_Italic->setMaximumWidth( 25 );
	m_Button_TimeDisplay_Italic->setMaximumWidth( 25 );
	m_Button_ColorBarString_Bold->setCheckable( true );
	m_Button_ColorBarString_Italic->setCheckable( true );
	m_Button_ColorBarValue_Bold->setCheckable( true );
	m_Button_ColorBarValue_Italic->setCheckable( true );
	m_Button_TimeDisplay_Bold->setCheckable( true );
	m_Button_TimeDisplay_Italic->setCheckable( true );

	QGridLayout *gridLayout = new QGridLayout( this );
	gridLayout->addWidget( label_ColorBarString,              0, 0 );
	gridLayout->addWidget( m_FontComboBox_ColorBarString,     0, 1 );
	gridLayout->addWidget( m_SpinBox_FontSize_ColorBarString, 0, 2 );
	gridLayout->addWidget( m_Button_ColorBarString_Bold,      0, 3 );
	gridLayout->addWidget( m_Button_ColorBarString_Italic,    0, 4 );
	gridLayout->addWidget( label_ColorBarValue,               1, 0 );
	gridLayout->addWidget( m_FontComboBox_ColorBarValue,      1, 1 );
	gridLayout->addWidget( m_SpinBox_FontSize_ColorBarValue,  1, 2 );
	gridLayout->addWidget( m_Button_ColorBarValue_Bold,       1, 3 );
	gridLayout->addWidget( m_Button_ColorBarValue_Italic,     1, 4 );
	gridLayout->addWidget( label_TimeDisplay,                 2, 0 );
	gridLayout->addWidget( m_FontComboBox_TimeDisplay,        2, 1 );
	gridLayout->addWidget( m_SpinBox_FontSize_TimeDisplay,    2, 2 );
	gridLayout->addWidget( m_Button_TimeDisplay_Bold,         2, 3 );
	gridLayout->addWidget( m_Button_TimeDisplay_Italic,       2, 4 );
}

void FontControl::popup()
{
	show();
	setFocus();
}

void FontControl::setFont()
{
	// font settings for colorbar string
	QFont cbsfont;
	cbsfont.setFamily( m_FontComboBox_ColorBarString->currentFont().family() );
	cbsfont.setPixelSize( m_SpinBox_FontSize_ColorBarString->value() );
	if ( m_Button_ColorBarString_Bold->isChecked() ) {
		cbsfont.setBold( true );
	} else {
		cbsfont.setBold( false );
	}
	if ( m_Button_ColorBarString_Italic->isChecked() ) {
		cbsfont.setItalic( true );
	} else {
		cbsfont.setItalic( false );
	}
#if QT_VERSION >= 0x040800
	cbsfont.setHintingPreference( QFont::PreferVerticalHinting );
#endif
	appParam.setFontColorBarString( cbsfont );

	// font settings for colorbar value
	QFont cbvfont;
	cbvfont.setFamily( m_FontComboBox_ColorBarValue->currentFont().family() );
	cbvfont.setPixelSize( m_SpinBox_FontSize_ColorBarValue->value() );
	if ( m_Button_ColorBarValue_Bold->isChecked() ) {
		cbvfont.setBold( true );
	} else {
		cbvfont.setBold( false );
	}
	if ( m_Button_ColorBarValue_Italic->isChecked() ) {
		cbvfont.setItalic( true );
	} else {
		cbvfont.setItalic( false );
	}
	appParam.setFontColorBarValue( cbvfont );

	// font settings for time display
	QFont tdfont;
	tdfont.setFamily( m_FontComboBox_TimeDisplay->currentFont().family() );
	tdfont.setPixelSize( m_SpinBox_FontSize_TimeDisplay->value() );
	if ( m_Button_TimeDisplay_Bold->isChecked() ) {
		tdfont.setBold( true );
	} else {
		tdfont.setBold( false );
	}
	if ( m_Button_TimeDisplay_Italic->isChecked() ) {
		tdfont.setItalic( true );
	} else {
		tdfont.setItalic( false );
	}
	appParam.setFontTimeDisplay( tdfont );
}
