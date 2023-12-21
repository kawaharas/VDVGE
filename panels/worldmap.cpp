//
// Volume Data Visualizer for Google Earth
//
// worldmap.cpp
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

#include "worldmap.h"

WorldMap::WorldMap( QWidget *parent )
	: QWidget( parent )
{
	makeWidget();
}

void WorldMap::makeWidget()
{
	m_GroupBox = new QGroupBox();
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	m_GroupBox->setAttribute( Qt::WA_ContentsPropagated );
#endif
	m_GroupBox->setTitle( "World Map" );

	m_WorldMapMain = new WorldMapMain();

	QVBoxLayout *vBoxLayout = new QVBoxLayout( m_GroupBox );
	vBoxLayout->addWidget( m_WorldMapMain );
}

void WorldMap::setDataState( bool state )
{
	m_WorldMapMain->setDataState( state );
	m_WorldMapMain->update();
}


WorldMapMain::WorldMapMain( QWidget *parent )
	: QWidget( parent )
{
	m_DataIsLoaded = false;
	makeWidget();
}

void WorldMapMain::makeWidget()
{
	QFrame *frame = new QFrame( this );
	frame->setFrameStyle( QFrame::Panel | QFrame::Sunken );
	frame->setGeometry( this->geometry() );

	setMinimumWidth( 360 );
	setMinimumHeight( 180 );

	QPalette blackPalette;
	blackPalette.setColor( QPalette::Window, QColor( Qt::black ) );
	setPalette( blackPalette );
	setAutoFillBackground( true );

	m_QPixmapEarth = QPixmap();
	QString path = QCoreApplication::applicationDirPath();
	m_QPixmapEarth.load( ":/resources/worldmap.png" );
}

void WorldMapMain::paintEvent( QPaintEvent * )
{
	QPainter painter( this );
	painter.setRenderHint( QPainter::Antialiasing );
	QPen pen( Qt::red );
	pen.setWidth( 2 );
	painter.setPen( pen );
	painter.drawPixmap( 0, 0, size().width(), size().height(), m_QPixmapEarth );

	if ( m_DataIsLoaded == true ) {
		float rectX0 = ( int )( ( float )( size().width()  ) / 360.0 * grads.getGridXMin() );
		float rectX1 = ( int )( ( float )( size().width()  ) / 360.0 * grads.getGridXMax() );
		float rectY0 = ( int )( ( float )( size().height() ) / 180.0 * grads.getGridYMin() );
		float rectY1 = ( int )( ( float )( size().height() ) / 180.0 * grads.getGridYMax() );
		painter.drawRect( rectX0, ( int )( float )( size().height() / 2.0 ) - rectY1,
						  rectX1 - rectX0, rectY1 - rectY0 );
	}
	painter.end();
}
