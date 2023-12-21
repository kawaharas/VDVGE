//
// Volume Data Visualizer for Google Earth
//
// fileinfo_panel.cpp
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

#include "fileinfo_panel.h"

FileInfoPanel::FileInfoPanel( QWidget *parent ) :
	QWidget( parent )
{
	setMinimumHeight( 600 );
	makeWidget();
}

void FileInfoPanel::makeWidget()
{
	m_GrADSInfo = new GrADSInfo();
	m_WorldMap  = new WorldMap();

	QVBoxLayout *vBoxLayout = new QVBoxLayout();
	vBoxLayout->addWidget( m_GrADSInfo->getControlPanel() );
	vBoxLayout->addWidget( m_WorldMap->getControlPanel() );
	vBoxLayout->addStretch( 1 );
	setLayout( vBoxLayout );
}

void FileInfoPanel::setGrads()
{
	m_GrADSInfo->setInfomation();
	m_WorldMap->setDataState( true );
}
