//
// Volume Data Visualizer for Google Earth
//
// visualization_panel.cpp
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

#include "visualization_panel.h"

VisualizationPanel::VisualizationPanel( QWidget *parent ) :
	QWidget( parent )
{
	makeWidget();

	connect( m_ContourLineControl, SIGNAL( valueChanged( ValueType ) ),
			 this, SIGNAL( valueChanged( ValueType ) ) );
}

void VisualizationPanel::makeWidget()
{
	m_VolumeRenderControl = new VolumeRenderControl();
	m_ColorSliceControl   = new ColorSliceControl();
	m_ContourLineControl   = new ContourLineControl();

	QVBoxLayout *vBoxLayout = new QVBoxLayout();
	vBoxLayout->addWidget( m_VolumeRenderControl->getControlPanel() );
	vBoxLayout->addWidget( m_ColorSliceControl->getControlPanel() );
	vBoxLayout->addWidget( m_ContourLineControl->getControlPanel() );
	vBoxLayout->setContentsMargins( 0, 0, 0, 0 );
	vBoxLayout->addStretch( 1 );
	setLayout( vBoxLayout );
}

void VisualizationPanel::setGrads()
{
	m_VolumeRenderControl->setDisabled( false );
	m_ColorSliceControl->setDisabled( false );
	m_ContourLineControl->setDisabled( false );

	m_VolumeRenderControl->setInitialValue();
	m_ColorSliceControl->setInitialValue();
	m_ContourLineControl->setInitialValue();
}
