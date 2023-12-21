//
// Volume Data Visualizer for Google Earth
//
// viewer_control.cpp
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

#include "viewer_control.h"

ViewerControl::ViewerControl( QWidget *parent ) :
	QDialog( parent )
{
	makeWidget();

	connect( m_Button_Accept, SIGNAL( pressed() ),
			 this, SLOT( setSize() ) );
	connect( m_Button_Cancel, SIGNAL( pressed() ),
			 this, SLOT( hide() ) );
}

ViewerControl::~ViewerControl()
{
}

void ViewerControl::makeWidget()
{
	setWindowTitle( tr( "Viewer Size" ) );
	setWindowModality( Qt::ApplicationModal );
	setFixedWidth( 300 );

	QLabel *label_Width = new QLabel( tr( "Width: " ) );
	label_Width->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
//	label_Width->setMinimumWidth( 70 );
//	label_Width->setMaximumWidth( 70 );
	QLabel *label_Height = new QLabel( tr( "Height: " ) );
	label_Height->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
//	label_Height->setMinimumWidth( 70 );
//	label_Height->setMaximumWidth( 70 );

	m_LineEdit_Width = new QLineEdit( this );
	m_LineEdit_Width->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	m_LineEdit_Height = new QLineEdit( this );
	m_LineEdit_Height->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
	QIntValidator *validator = new QIntValidator( this );
	validator->setBottom( 0 );
	m_LineEdit_Width->setValidator( validator );
	m_LineEdit_Height->setValidator( validator );
	m_LineEdit_Width->setText( QString("%1").arg( param.getViewerSize().width() ) );
	m_LineEdit_Height->setText( QString("%1").arg( param.getViewerSize().height() ) );

	QGridLayout *gridLayout = new QGridLayout();
	gridLayout->addWidget( label_Width,          0, 0 );
	gridLayout->addWidget( m_LineEdit_Width,     0, 1 );
	gridLayout->addWidget( label_Height,         1, 0 );
	gridLayout->addWidget( m_LineEdit_Height,    1, 1 );

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

void ViewerControl::setSize()
{
	int width  = ( m_LineEdit_Width->text() ).toInt();
	int height = ( m_LineEdit_Height->text() ).toInt();
	QSize maxSize = param.getMaximumViewerSize();

	if ( ( width > maxSize.width() ) || ( height > maxSize.height() ) ) {
		QString msg0, msg1;
		if ( width > maxSize.width() ) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
			msg0.asprintf( "Maximum Width is %d.\n", maxSize.width() );
#else
			msg0.sprintf( "Maximum Width is %d.\n", maxSize.width() );
#endif
			m_LineEdit_Width->setText( QString("%1").arg( param.getViewerSize().width() ) );
		}
		if ( height > maxSize.height() ) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
			msg1.asprintf( "Maximum Height is %d.", maxSize.height() );
#else
			msg1.sprintf( "Maximum Height is %d.", maxSize.height() );
#endif
			m_LineEdit_Height->setText( QString("%1").arg( param.getViewerSize().height() ) );
		}
		msg0 += msg1;
		QMessageBox::StandardButton mbox;
		mbox = QMessageBox::information( this, tr( "Error" ), msg0 );
	} else {
		param.setViewerSize( QSize( width, height ) );
		hide();
		emit valueChanged( CHANGE_VIEWER_SIZE );
	}
}

void ViewerControl::popup()
{
	m_LineEdit_Width->setText( QString("%1").arg( param.getViewerSize().width() ) );
	m_LineEdit_Height->setText( QString("%1").arg( param.getViewerSize().height() ) );
	show();
	setFocus();
}
