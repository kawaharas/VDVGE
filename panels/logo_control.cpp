//
// Volume Data Visualizer for Google Earth
//
// logo_control.cpp
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

#include "logo_control.h"

LogoControl::LogoControl( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

	connect( m_CheckBox_Logo, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateLogo( bool ) ) );
	connect( m_Button_SelectLogoFile, SIGNAL( pressed() ),
			 this, SLOT( setLogoFile() ) );
}

void LogoControl::makeWidget()
{
	m_GroupBox = new QGroupBox();
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox->setTitle( "Logo" );

	m_CheckBox_Logo = new QCheckBox( "Enable", 0 );
	m_Button_SelectLogoFile = new QPushButton( tr( "Select File" ) );

	QSpacerItem *spacer = new QSpacerItem( 20, 20,
		QSizePolicy::Expanding, QSizePolicy::Expanding );

	QHBoxLayout *hBoxLayout = new QHBoxLayout( m_GroupBox );
	hBoxLayout->setSpacing( 15 );
	hBoxLayout->addWidget( m_CheckBox_Logo );
	hBoxLayout->addWidget( m_Button_SelectLogoFile );
	hBoxLayout->addItem( spacer );
//	m_CheckBox_Logo->setCheckState( Qt::Checked );
	m_CheckBox_Logo->setCheckState( Qt::Unchecked );
	m_GroupBox->setDisabled( true );
}

void LogoControl::setInitialValue()
{
	if ( param.IsLogo() ) {
		m_CheckBox_Logo->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_Logo->setCheckState( Qt::Unchecked );
	}
}

void LogoControl::setDisabled( bool state )
{
	m_GroupBox->setDisabled( state );
}

void LogoControl::setStateLogo( bool state )
{
/*
	if ( state == true ) {
		if ( param.getLogo() == "" ) {
			setLogoFile();
		}
	}
*/
	param.setStateLogo( state );
}

void LogoControl::setLogoFile()
{
	QString filename;
	filename.clear();

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
	filename = QFileDialog::getOpenFileName(
			this, "Select Logo File",
			QCoreApplication::applicationDirPath(),
			"PNG Image File (*.png)",
			0, QFileDialog::DontUseNativeDialog );
#else
	filename = QFileDialog::getOpenFileName(
			this, "Select Logo File",
			QCoreApplication::applicationDirPath(),
			"PNG Image File (*.png)" );
#endif
#else
#ifdef Q_OS_WIN
	filename = QFileDialog::getOpenFileName(
			this, "Select Logo File",
			QCoreApplication::applicationDirPath(),
			"PNG Image File (*.png)",
			0, QFileDialog::DontUseNativeDialog );
#else
	filename = QFileDialog::getOpenFileName(
			this, "Select Logo File",
			QCoreApplication::applicationDirPath(),
			"PNG Image File (*.png)" );
#endif
#endif

	qDebug() << filename;
	if ( filename.isNull() ) return;

	if ( QFileInfo( filename ).exists() ) {
//	if ( !filename.isEmpty() ) {
		param.setLogo( filename );
		m_CheckBox_Logo->setCheckState( Qt::Checked );
		param.setStateLogo( true );
//		emit logoChanged();
		emit valueChanged( CHANGE_LOGO_IMAGE );
	} else {
		m_CheckBox_Logo->setCheckState( Qt::Unchecked );
		param.setStateLogo( false );
	}
}
