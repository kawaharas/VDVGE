//
// Volume Data Visualizer for Google Earth
//
// gradsinfo.cpp
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

#include "gradsinfo.h"

GrADSInfo::GrADSInfo( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();
}

void GrADSInfo::makeWidget()
{
	QLabel *label_CtlFile  = new QLabel( tr( "Control File:" ) );
	QLabel *label_DataFile = new QLabel( tr( "Data File:" ) );
	QLabel *label_GridX    = new QLabel( tr( "GridSize X:" ) );
	QLabel *label_GridY    = new QLabel( tr( "GridSize Y:" ) );
	QLabel *label_GridZ    = new QLabel( tr( "GridSize Z:" ) );
	QLabel *label_GridT    = new QLabel( tr( "TimeSteps:" ) );
	m_Label_CtlFileString  = new QLabel( tr( "" ) );
	m_Label_DataFileString = new QLabel( tr( "" ) );
	m_Label_GridXString    = new QLabel( tr( "" ) );
	m_Label_GridYString    = new QLabel( tr( "" ) );
	m_Label_GridZString    = new QLabel( tr( "" ) );
	m_Label_GridTString    = new QLabel( tr( "" ) );

	m_GroupBox = new QGroupBox();
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox->setTitle( "GrADS Information" );
//	m_GroupBox->setFixedWidth( 400 );
//	m_GroupBox->setMinimumHeight( 200 );

	QGridLayout *gridLayout = new QGridLayout( m_GroupBox );
	gridLayout->setVerticalSpacing( 10 );
	gridLayout->addWidget( label_CtlFile, 0, 0 );
	gridLayout->addWidget( label_DataFile, 1, 0 );
	gridLayout->addWidget( label_GridX, 2, 0 );
	gridLayout->addWidget( label_GridY, 3, 0 );
	gridLayout->addWidget( label_GridZ, 4, 0 );
	gridLayout->addWidget( label_GridT, 5, 0 );
	gridLayout->addWidget( m_Label_CtlFileString, 0, 1 );
	gridLayout->addWidget( m_Label_DataFileString, 1, 1 );
	gridLayout->addWidget( m_Label_GridXString, 2, 1 );
	gridLayout->addWidget( m_Label_GridYString, 3, 1 );
	gridLayout->addWidget( m_Label_GridZString, 4, 1 );
	gridLayout->addWidget( m_Label_GridTString, 5, 1 );

	QSpacerItem *spacer = new QSpacerItem( 20, 20,
		QSizePolicy::Expanding, QSizePolicy::Expanding );

	if ( m_Label_CtlFileString->width() > m_Label_DataFileString->width() ) {
		gridLayout->addItem( spacer, 0, 2 );
	} else {
		gridLayout->addItem( spacer, 1, 2 );
	}
}

void GrADSInfo::setInfomation()
{
	m_Label_CtlFileString->setText( QFileInfo( grads.getCtlFileName() ).fileName() );
	m_Label_CtlFileString->setToolTip( grads.getCtlFileName() );
	m_Label_DataFileString->setText( QFileInfo( grads.getDataFileName() ).fileName() );
	m_Label_DataFileString->setToolTip( grads.getDataFileName() );
	m_Label_GridXString->setNum( grads.getGridSizeX() );
	m_Label_GridYString->setNum( grads.getGridSizeY() );
	m_Label_GridZString->setNum( grads.getGridSizeZ() );
	m_Label_GridTString->setNum( grads.getGridSizeT() );
}
