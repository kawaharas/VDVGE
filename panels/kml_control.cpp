//
// Volume Data Visualizer for Google Earth
//
// kml_control.cpp
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

#include "kml_control.h"

KMLControl::KMLControl( QWidget *parent ) :
	QDialog( parent )
{
	makeWidget();

	connect( m_Button_Accept, SIGNAL( pressed() ),
			 this, SLOT( setValue() ) );
	connect( m_Button_Cancel, SIGNAL( pressed() ),
			 this, SLOT( hide() ) );
}

KMLControl::~KMLControl()
{
}

void KMLControl::makeWidget()
{
	setWindowTitle( tr( "Maximum Image Size" ) );
	setWindowModality( Qt::ApplicationModal );
	setFixedWidth( 300 );

	QLabel *label_X = new QLabel( tr( "X: " ) );
	label_X->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
//	label_X->setMinimumWidth( 70 );
//	label_X->setMaximumWidth( 70 );
	QLabel *label_Y = new QLabel( tr( "Y: " ) );
	label_Y->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
//	label_Y->setMinimumWidth( 70 );
//	label_Y->setMaximumWidth( 70 );
	QLabel *label_Z = new QLabel( tr( "Z: " ) );
	label_Z->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
//	label_Z->setMinimumWidth( 70 );
//	label_Z->setMaximumWidth( 70 );

	m_LineEdit_X = new QLineEdit( this );
	m_LineEdit_X->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_LineEdit_Y = new QLineEdit( this );
	m_LineEdit_Y->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_LineEdit_Z = new QLineEdit( this );
	m_LineEdit_Z->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	QIntValidator *validator = new QIntValidator( this );
	validator->setBottom( 0 );
	m_LineEdit_X->setValidator( validator );
	m_LineEdit_Y->setValidator( validator );
	m_LineEdit_Z->setValidator( validator );
	m_LineEdit_X->setText( QString("%1").arg( param.getKMLMaxImageSizeX() ) );
	m_LineEdit_Y->setText( QString("%1").arg( param.getKMLMaxImageSizeY() ) );
	m_LineEdit_Z->setText( QString("%1").arg( param.getKMLMaxImageSizeZ() ) );

	QGridLayout *gridLayout = new QGridLayout();
	gridLayout->addWidget( label_X,      0, 0 );
	gridLayout->addWidget( m_LineEdit_X, 0, 1 );
	gridLayout->addWidget( label_Y,      1, 0 );
	gridLayout->addWidget( m_LineEdit_Y, 1, 1 );
	gridLayout->addWidget( label_Z,      2, 0 );
	gridLayout->addWidget( m_LineEdit_Z, 2, 1 );

	m_Button_Accept = new QPushButton( tr( "Accept" ) );
	m_Button_Cancel = new QPushButton( tr( "Cancel" ) );
	QHBoxLayout *hBoxLayout = new QHBoxLayout();
	hBoxLayout->addWidget( m_Button_Accept );
	hBoxLayout->addWidget( m_Button_Cancel );

	QVBoxLayout *vBoxLayout = new QVBoxLayout();
	vBoxLayout->addLayout( gridLayout );
	vBoxLayout->addLayout( hBoxLayout );
	setLayout( vBoxLayout );
}

void KMLControl::setValue()
{
	int x = ( m_LineEdit_X->text() ).toInt();
	int y = ( m_LineEdit_Y->text() ).toInt();
	int z = ( m_LineEdit_Z->text() ).toInt();

	if ( ( x < 50 ) || ( y < 50 ) || ( z < 50 ) ) {
		QString msg0, msg1, msg2;
		if ( x < 50 ) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
			msg0.asprintf( "X: Mimimum Size is 50.\n" );
#else
			msg0.sprintf( "X: Mimimum Size is 50.\n" );
#endif
			m_LineEdit_X->setText( QString("%1").arg( 50 ) );
		}
		if ( y < 50 ) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
			msg1.asprintf( "Y: Mimimum Size is 50.\n" );
#else
			msg1.sprintf( "Y: Mimimum Size is 50.\n" );
#endif
			m_LineEdit_Y->setText( QString("%1").arg( 50 ) );
		}
		if ( z < 50 ) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
			msg2.asprintf( "Z: Mimimum Size is 50.\n" );
#else
			msg2.sprintf( "Z: Mimimum Size is 50.\n" );
#endif
			m_LineEdit_Z->setText( QString("%1").arg( 50 ) );
		}
		msg0 += msg1;
		msg0 += msg2;
		QMessageBox::StandardButton mbox;
		mbox = QMessageBox::information( this, tr( "Error" ), msg0 );
	} else {
		param.setKMLMaxImageSize( x, y, z );
		hide();
	}
}

void KMLControl::popup()
{
	m_LineEdit_X->setText( QString("%1").arg( param.getKMLMaxImageSizeX() ) );
	m_LineEdit_Y->setText( QString("%1").arg( param.getKMLMaxImageSizeY() ) );
	m_LineEdit_Z->setText( QString("%1").arg( param.getKMLMaxImageSizeZ() ) );
	show();
	setFocus();
}
