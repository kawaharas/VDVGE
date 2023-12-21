//
// Volume Data Visualizer for Google Earth
//
// data_panel.cpp
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

#include "data_panel.h"

DataPanel::DataPanel( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

	connect( m_DataRangeControl, SIGNAL( valueChanged( ValueType ) ),
			 this, SIGNAL( valueChanged( ValueType ) ) );
}

void DataPanel::makeWidget()
{
	m_DataRangeControl    = new DataRangeControl();
	m_TimeRangeControl    = new TimeRangeControl();
	m_ZScaleControl       = new ZScaleControl();

	QVBoxLayout *vBoxLayout = new QVBoxLayout();
	vBoxLayout->addWidget( m_DataRangeControl->getControlPanel() );
	vBoxLayout->addWidget( m_TimeRangeControl->getControlPanel() );
	vBoxLayout->addWidget( m_ZScaleControl->getControlPanel() );
	vBoxLayout->setContentsMargins( 0, 0, 0, 0 );
	vBoxLayout->addStretch( 1 );
	setLayout( vBoxLayout );
}

void DataPanel::setGrads()
{
	m_DataRangeControl->setDisabled( false );
	m_TimeRangeControl->setDisabled( false );
	m_ZScaleControl->setDisabled( false );

	m_DataRangeControl->setInitialValue();
	m_TimeRangeControl->setInitialValue();
	m_ZScaleControl->setInitialValue();
}
