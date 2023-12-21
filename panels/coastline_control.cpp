//
// Volume Data Visualizer for Google Earth
//
// coastline_control.cpp
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

#include "coastline_control.h"

CoastlineControl::CoastlineControl( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

	connect( m_CheckBox_Coastline, SIGNAL( toggled( bool ) ),
			 this, SLOT( setStateCoastline( bool ) ) );
	connect( m_Button_SelectCoastlineFile, SIGNAL( pressed() ),
			 this, SLOT( setCoastlineFile() ) );
}

void CoastlineControl::makeWidget()
{
	m_GroupBox = new QGroupBox();
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox->setTitle( "Coastline" );

	m_CheckBox_Coastline = new QCheckBox( "Enable", 0 );
	m_Button_SelectCoastlineFile = new QPushButton( tr( "Select File" ) );

	QSpacerItem *spacer = new QSpacerItem( 20, 20,
		QSizePolicy::Expanding, QSizePolicy::Expanding );

	QHBoxLayout *hBoxLayout1 = new QHBoxLayout( m_GroupBox );
	hBoxLayout1->setSpacing( 15 );
	hBoxLayout1->addWidget( m_CheckBox_Coastline );
	hBoxLayout1->addWidget( m_Button_SelectCoastlineFile );
	hBoxLayout1->addItem( spacer );
	m_CheckBox_Coastline->setCheckState( Qt::Unchecked );
	m_GroupBox->setDisabled( true );
}

void CoastlineControl::setInitialValue()
{
	if ( param.IsTopo() ) {
		m_CheckBox_Coastline->setCheckState( Qt::Checked );
	} else {
		m_CheckBox_Coastline->setCheckState( Qt::Unchecked );
	}
}

void CoastlineControl::setDisabled( bool state )
{
	m_GroupBox->setDisabled( state );
}

void CoastlineControl::setStateCoastline( bool state )
{
	if ( state ) {
		if ( param.getCoastline() == "" ) {
			setCoastlineFile();
		}
	}
	param.setStateCoastline( state );
}

void CoastlineControl::setCoastlineFile()
{
	QString filename;

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
	filename = QFileDialog::getOpenFileName(
			this, "Select Coastline File",
			QCoreApplication::applicationDirPath(),
			0, 0, QFileDialog::DontUseNativeDialog );
#else
	filename = QFileDialog::getOpenFileName(
			this, "Select Coastline File",
			QCoreApplication::applicationDirPath(), 0 );
#endif
#else
#ifdef Q_OS_WIN
	filename = QFileDialog::getOpenFileName(
			this, "Select Coastline File",
			QCoreApplication::applicationDirPath(),
			0, 0, QFileDialog::DontUseNativeDialog );
#else
	filename = QFileDialog::getOpenFileName(
			this, "Select Coastline File",
			QCoreApplication::applicationDirPath(), 0 );
#endif
#endif

	if ( !filename.isEmpty() ) {
		param.setCoastline( filename );
		emit valueChanged( CHANGE_COASTLINE_FILE );
	} else {
		m_CheckBox_Coastline->setCheckState( Qt::Unchecked );
	}
}
