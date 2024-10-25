//
// Volume Data Visualizer for Google Earth
//
// gradients.cpp
//
// Original Code: gradients.cpp
// (Part of the demonstration applications of the Qt Toolkit)
// Modified by Shintaro KAWAHARA (kawahara@jamstec.go.jp)
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

#include "gradients.h"
#include "colortable.h"

ShadeWidget::ShadeWidget( ShadeType type, QWidget *parent )
	: QWidget( parent ), m_ShadeType( type ),
	  m_alpha_gradient( QLinearGradient( 0, 0, 0, 0 ) )
{
	// Checkers background
	if ( m_ShadeType == ARGBShade )
	{
		QPixmap pm( 20, 20 );
		QPainter pmp( &pm );
		pmp.fillRect(  0,  0, 10, 10, Qt::lightGray );
		pmp.fillRect( 10, 10, 10, 10, Qt::lightGray );
		pmp.fillRect(  0, 10, 10, 10, Qt::darkGray  );
		pmp.fillRect( 10,  0, 10, 10, Qt::darkGray  );
		pmp.end();
		QPalette pal = palette();
		pal.setBrush( backgroundRole(), QBrush( pm ) );
		setAutoFillBackground( true );
		setPalette( pal );
	} else {

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		setAttribute( Qt::WA_OpaquePaintEvent );
#else
		setAttribute( Qt::WA_NoBackground );
#endif
	}

	QPolygonF points;
	points << QPointF( 0, sizeHint().height() )
		   << QPointF( sizeHint().width(), 0 );

	m_colorTable = new ColorTable( this, ColorTable::CircleShape );
//	m_colorTable->setConnectionType( ColorTable::LineConnection );
	m_colorTable->setConnectionType( ColorTable::CurveConnection );
	m_colorTable->setPoints( points );
	m_colorTable->setPointLock( 0, ColorTable::LockToLeft );
	m_colorTable->setPointLock( 1, ColorTable::LockToRight );
	m_colorTable->setSortType( ColorTable::XSort );

	setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

	setMouseTracking( true );
	m_Label = new QLabel;

	m_IsEnter = false;
	m_TextMargin = 5;

	connect( m_colorTable, SIGNAL( pointsChanged( const QPolygonF & ) ),
			 this, SIGNAL( colorsChanged() ) );
}

uint ShadeWidget::colorAt( int x )
{
	generateShade();

	QPolygonF pts = m_colorTable->points();
	for (int i = 1; i < pts.size(); ++i ) {
		if ( pts.at( i - 1 ).x() <= x && pts.at( i ).x() >= x) {
			QLineF l( pts.at( i - 1 ), pts.at( i ) );
			l.setLength( l.length() * ( ( x - l.x1() ) / l.dx() ) );
			return m_shade.pixel( qRound( qMin( l.x2(), ( qreal( m_shade.width()  - 1 ) ) ) ),
								  qRound( qMin( l.y2(),   qreal( m_shade.height() - 1 ) ) ) );
		}
	}
	return 0;
}

void ShadeWidget::setGradientStops( const QGradientStops &stops )
{
	if ( m_ShadeType == ARGBShade ) {
		 m_alpha_gradient = QLinearGradient( 0, 0, width(), 0 );

		for (int i = 0; i < stops.size(); ++i ) {
			QColor c = stops.at( i ).second;
			m_alpha_gradient.setColorAt( stops.at( i ).first,
										 QColor( c.red(), c.green(), c.blue() ) );
		}

		m_shade = QImage();
		generateShade();
		update();
	}
}

void ShadeWidget::generateShade()
{
	if ( m_shade.isNull() || m_shade.size() != size() ) {

		if ( m_ShadeType == ARGBShade ) {
			m_shade = QImage( size(), QImage::Format_ARGB32_Premultiplied );
			m_shade.fill( 0 );

			QPainter p( &m_shade );
			p.fillRect( rect(), m_alpha_gradient );

			p.setCompositionMode( QPainter::CompositionMode_DestinationIn );
			QLinearGradient fade( 0, 0, 0, height() );
			fade.setColorAt( 0.0, QColor( 0, 0, 0, 255 ) );
			fade.setColorAt( 0.5, QColor( 0, 0, 0, 150 ) );
			fade.setColorAt( 1.0, QColor( 0, 0, 0,   0 ) );
			p.fillRect( rect(), fade );

		} else {
			m_shade = QImage( size(), QImage::Format_RGB32 );
			QLinearGradient shade( 0, 0, 0, height() );
			shade.setColorAt( 1, Qt::black );

			if ( m_ShadeType == RedShade )
				shade.setColorAt( 0, Qt::red );
			else if ( m_ShadeType == GreenShade )
				shade.setColorAt( 0, Qt::green );
			else
				shade.setColorAt( 0, Qt::blue );

			QPainter p( &m_shade );
			p.fillRect( rect(), shade );
		}
	}
}

void ShadeWidget::paintEvent( QPaintEvent * )
{
	generateShade();

	QPainter p( this );
	p.drawImage( 0, 0, m_shade );

	p.setPen( QColor( 146, 146, 146 ) );
	p.drawRect( 0, 0, width() - 1, height() - 1 );

	// draw vertical grid lines
	QPen pen( QColor( 0, 0, 0, 128 ) );
	pen.setWidthF( 1.5 );
	p.setPen( pen );
	float dx = static_cast< float >( width() - 1 ) /
		static_cast< float >( param.getColorBarMajorTickMarkNum() - 1 );
	for ( int i = 1; i < param.getColorBarMajorTickMarkNum() - 1; i++ ) {
		p.drawLine( dx * i, 0, dx * i, height() - 1 );
	}

	// Update Information of Cursor's Position
	if ( m_IsEnter ) {
		emit updateInfo( m_ShadeType, m_Value, m_Level );
	}
}

void ShadeWidget::mouseReleaseEvent( QMouseEvent * event )
{
	if ( ( event->button() == Qt::LeftButton ) ||
		 ( event->button() == Qt::RightButton ) ) {
		emit mouseReleased();
	}
}

void ShadeWidget::mouseMoveEvent( QMouseEvent * event )
{
	float fx = static_cast< float >( event->pos().x() );
	float fy = static_cast< float >( event->pos().y() );
	float fw = static_cast< float >( width() );
	float fh = static_cast< float >( height() );
	if ( fx < 0.0 ) {
		fx = 0.0;
	} else if ( fx > fw ) {
		fx = fw;
	}
	if ( fy < 0.0 ) {
		fy = 0.0;
	} else if ( fy > fh ) {
		fy = fh;
	}
	float min = param.getVolumeThresholdMin();
	float max = param.getVolumeThresholdMax();
	if ( param.IsLogScale() ) {
		m_Value = log10( min ) + ( log10( max ) - log10( min ) ) * fx / fw;
		m_Value = pow( 10.0, m_Value );
	} else {
		m_Value = min + ( max - min ) * fx / fw;
	}
	m_Level = 1.0 - fy / fh;
	m_MousePos = event->pos();
	update();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void ShadeWidget::enterEvent( QEnterEvent * event )
#else
void ShadeWidget::enterEvent( QEvent * event )
#endif
{
	m_IsEnter = true;
//	setCursor( Qt::BlankCursor );
	setCursor( Qt::CrossCursor );

	Q_UNUSED ( event );
}

void ShadeWidget::leaveEvent( QEvent * event )
{
	m_IsEnter = false;
	update();	// to clear text on widget

	Q_UNUSED ( event );
}

GradientEditor::GradientEditor( QWidget *parent )
	: QWidget( parent )
{
	QFont font( "Helvetica" );
	font.setPixelSize( 12 );
	QFontMetrics fontMetrics( font );

	m_ShadeR = new ShadeWidget( ShadeWidget::RedShade,   this );
	m_ShadeG = new ShadeWidget( ShadeWidget::GreenShade, this );
	m_ShadeB = new ShadeWidget( ShadeWidget::BlueShade,  this );
	m_ShadeA = new ShadeWidget( ShadeWidget::ARGBShade,  this );

	m_ConnecctionType = CurveConnection;

	for ( int i = 0; i < MAX_SCALE_MARKINGS; i++ ) {
		m_Label_ScaleMarking[i] = new QLabel( this );
		m_Label_ScaleMarking[i]->setText( tr( "--" ) );
		m_Label_ScaleMarking[i]->setFont( font );
		m_Label_ScaleMarking[i]->setMargin( 0 );
		m_Label_ScaleMarking[i]->setContentsMargins( 0, 0, 0, 0 );
	}
	m_Label_Value = new QLabel( tr( "Value: " ) );
	m_Label_Level = new QLabel( tr( "R: " ) );
	m_Label_CurrentValue = new QLabel( tr( " " ) );
	m_Label_CurrentLevel = new QLabel( tr( " " ) );
	m_Label_Value->setFont( font );
	m_Label_Level->setFont( font );
	m_Label_CurrentValue->setFont( font );
	m_Label_CurrentLevel->setFont( font );
	m_Label_Value->setAlignment( Qt::AlignRight );
	m_Label_Level->setAlignment( Qt::AlignRight );
	m_Label_CurrentValue->setAlignment( Qt::AlignLeft );
	m_Label_CurrentLevel->setAlignment( Qt::AlignLeft );

	QLabel *m_Label_ShadeR0 = new QLabel( tr("0") );
	QLabel *m_Label_ShadeR1 = new QLabel( tr("1") );
	QLabel *m_Label_ShadeG0 = new QLabel( tr("0") );
	QLabel *m_Label_ShadeG1 = new QLabel( tr("1") );
	QLabel *m_Label_ShadeB0 = new QLabel( tr("0") );
	QLabel *m_Label_ShadeB1 = new QLabel( tr("1") );
	QLabel *m_Label_ShadeA0 = new QLabel( tr("0") );
	QLabel *m_Label_ShadeA1 = new QLabel( tr("1") );
	m_Label_ShadeR0->setFont( font );
	m_Label_ShadeR1->setFont( font );
	m_Label_ShadeG0->setFont( font );
	m_Label_ShadeG1->setFont( font );
	m_Label_ShadeB0->setFont( font );
	m_Label_ShadeB1->setFont( font );
	m_Label_ShadeA0->setFont( font );
	m_Label_ShadeA1->setFont( font );
	m_Label_ShadeR0->setMargin( 0 );
	m_Label_ShadeR1->setMargin( 0 );
	m_Label_ShadeG0->setMargin( 0 );
	m_Label_ShadeG1->setMargin( 0 );
	m_Label_ShadeB0->setMargin( 0 );
	m_Label_ShadeB1->setMargin( 0 );
	m_Label_ShadeA0->setMargin( 0 );
	m_Label_ShadeA1->setMargin( 0 );
	m_Label_ShadeR0->setContentsMargins( 0, 0, 0, 0 );
	m_Label_ShadeR1->setContentsMargins( 0, 0, 0, 0 );
	m_Label_ShadeG0->setContentsMargins( 0, 0, 0, 0 );
	m_Label_ShadeG1->setContentsMargins( 0, 0, 0, 0 );
	m_Label_ShadeB0->setContentsMargins( 0, 0, 0, 0 );
	m_Label_ShadeB1->setContentsMargins( 0, 0, 0, 0 );
	m_Label_ShadeA0->setContentsMargins( 0, 0, 0, 0 );
	m_Label_ShadeA1->setContentsMargins( 0, 0, 0, 0 );
	m_Label_ShadeR0->setMaximumWidth( fontMetrics.maxWidth() + 20 );
	m_Label_ShadeR1->setMaximumWidth( fontMetrics.maxWidth() + 20 );
	m_Label_ShadeG0->setMaximumWidth( fontMetrics.maxWidth() + 20 );
	m_Label_ShadeG1->setMaximumWidth( fontMetrics.maxWidth() + 20 );
	m_Label_ShadeB0->setMaximumWidth( fontMetrics.maxWidth() + 20 );
	m_Label_ShadeB1->setMaximumWidth( fontMetrics.maxWidth() + 20 );
	m_Label_ShadeA0->setMaximumWidth( fontMetrics.maxWidth() + 20 );
	m_Label_ShadeA1->setMaximumWidth( fontMetrics.maxWidth() + 20 );
	m_Label_ShadeR0->setAlignment( Qt::AlignBottom | Qt::AlignHCenter );
	m_Label_ShadeR1->setAlignment( Qt::AlignTop    | Qt::AlignHCenter );
	m_Label_ShadeG0->setAlignment( Qt::AlignBottom | Qt::AlignHCenter );
	m_Label_ShadeG1->setAlignment( Qt::AlignTop    | Qt::AlignHCenter );
	m_Label_ShadeB0->setAlignment( Qt::AlignBottom | Qt::AlignHCenter );
	m_Label_ShadeB1->setAlignment( Qt::AlignTop    | Qt::AlignHCenter );
	m_Label_ShadeA0->setAlignment( Qt::AlignBottom | Qt::AlignHCenter );
	m_Label_ShadeA1->setAlignment( Qt::AlignTop    | Qt::AlignHCenter );

	QHBoxLayout *hBoxLayoutScaleMarkings = new QHBoxLayout();
	for ( int i = 0; i < 20; i++ ) {
		hBoxLayoutScaleMarkings->addWidget( m_Label_ScaleMarking[ i ] );
	}
	QSizePolicy sizeHalf( QSizePolicy::Preferred, QSizePolicy::Preferred );
	QSizePolicy sizeNorm( QSizePolicy::Preferred, QSizePolicy::Preferred );
	sizeHalf.setHorizontalStretch( 1 );
	sizeNorm.setHorizontalStretch( 2 );
	for ( int i = 0; i < MAX_SCALE_MARKINGS; i++ ) {
		m_Label_ScaleMarking[ i ]->setSizePolicy( sizeNorm );
		m_Label_ScaleMarking[ i ]->setAlignment( Qt::AlignCenter );
		m_Label_ScaleMarking[ i ]->hide();
	}
	m_Label_ScaleMarking[ 0 ]->setAlignment( Qt::AlignLeft );
	m_Label_ScaleMarking[ 1 ]->setAlignment( Qt::AlignRight );
	m_Label_ScaleMarking[ 0 ]->setSizePolicy( sizeHalf );
	m_Label_ScaleMarking[ 1 ]->setSizePolicy( sizeHalf );
	m_Label_ScaleMarking[ 0 ]->show();
	m_Label_ScaleMarking[ 1 ]->show();

	QFrame *frame = new QFrame;
	frame->setFrameStyle( QFrame::Box | QFrame::Sunken );
	QHBoxLayout *hBoxLayoutInfo = new QHBoxLayout();
	QHBoxLayout *hBoxLayoutFrame = new QHBoxLayout();
	hBoxLayoutFrame->addWidget( m_Label_Value );
	hBoxLayoutFrame->addWidget( m_Label_CurrentValue );
	hBoxLayoutFrame->addWidget( m_Label_Level );
	hBoxLayoutFrame->addWidget( m_Label_CurrentLevel );
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	hBoxLayoutFrame->setMargin( 0 );
#endif
	hBoxLayoutFrame->setContentsMargins( 0, 0, 0, 0 );
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	hBoxLayoutInfo->setMargin( 0 );
#endif
	hBoxLayoutInfo->setContentsMargins( 0, 0, 0, 0 );
	frame->setLayout( hBoxLayoutFrame );
	hBoxLayoutInfo->addWidget( frame );

	QVBoxLayout *vBoxLayoutR = new QVBoxLayout();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	vBoxLayoutR->setMargin( 0 );
#endif
	vBoxLayoutR->setSpacing( 0 );
	vBoxLayoutR->setContentsMargins( 0, 0, 0, 0 );
	vBoxLayoutR->addWidget( m_Label_ShadeR1 );
	vBoxLayoutR->addWidget( m_Label_ShadeR0 );
	QVBoxLayout *vBoxLayoutG = new QVBoxLayout();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	vBoxLayoutG->setMargin( 0 );
#endif
	vBoxLayoutG->setSpacing( 0 );
	vBoxLayoutG->setContentsMargins( 0, 0, 0, 0 );
	vBoxLayoutG->addWidget( m_Label_ShadeG1 );
	vBoxLayoutG->addWidget( m_Label_ShadeG0 );
	QVBoxLayout *vBoxLayoutB = new QVBoxLayout();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	vBoxLayoutB->setMargin( 0 );
#endif
	vBoxLayoutB->setSpacing( 0 );
	vBoxLayoutB->setContentsMargins( 0, 0, 0, 0 );
	vBoxLayoutB->addWidget( m_Label_ShadeB1 );
	vBoxLayoutB->addWidget( m_Label_ShadeB0 );
	QVBoxLayout *vBoxLayoutA = new QVBoxLayout();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	vBoxLayoutA->setMargin( 0 );
#endif
	vBoxLayoutA->setSpacing( 0 );
	vBoxLayoutA->setContentsMargins( 0, 0, 0, 0 );
	vBoxLayoutA->addWidget( m_Label_ShadeA1 );
	vBoxLayoutA->addWidget( m_Label_ShadeA0 );

	QSpacerItem *spacer0 = new QSpacerItem( 8, 8,
		QSizePolicy::Expanding, QSizePolicy::Fixed );
	QSpacerItem *spacer1 = new QSpacerItem( 8, 8,
		QSizePolicy::Expanding, QSizePolicy::Fixed );
	QSpacerItem *spacer2 = new QSpacerItem( 8, 8,
		QSizePolicy::Expanding, QSizePolicy::Fixed );
	QSpacerItem *spacer3 = new QSpacerItem( 8, 2,
		QSizePolicy::Expanding, QSizePolicy::Fixed );
	QSpacerItem *spacer4 = new QSpacerItem( 8, 2,
		QSizePolicy::Expanding, QSizePolicy::Fixed );
	QGridLayout *gridLayout = new QGridLayout( this );
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	gridLayout->setMargin( 0 );
#endif
	gridLayout->setVerticalSpacing( 0 );
	gridLayout->setHorizontalSpacing( 3 );
	gridLayout->setContentsMargins( 0, 0, 0, 0 );
	gridLayout->addLayout( vBoxLayoutR, 0, 0, Qt::AlignLeft );
	gridLayout->addWidget( m_ShadeR,    0, 1 );
	gridLayout->addItem(   spacer0,     1, 1 );
	gridLayout->addLayout( vBoxLayoutG, 2, 0, Qt::AlignLeft );
	gridLayout->addWidget( m_ShadeG,    2, 1 );
	gridLayout->addItem(   spacer1,     3, 1 );
	gridLayout->addLayout( vBoxLayoutB, 4, 0, Qt::AlignLeft );
	gridLayout->addWidget( m_ShadeB,    4, 1 );
	gridLayout->addItem(   spacer2,     5, 1 );
	gridLayout->addLayout( vBoxLayoutA, 6, 0, Qt::AlignLeft );
	gridLayout->addWidget( m_ShadeA,    6, 1 );
	gridLayout->addItem(   spacer3,     7, 1 );
	gridLayout->addLayout( hBoxLayoutScaleMarkings, 8, 1 );
	gridLayout->addItem(   spacer4,     9, 1 );
	gridLayout->addLayout( hBoxLayoutInfo, 10, 1 );

	connect( m_ShadeR, SIGNAL( colorsChanged() ),
			 this, SLOT( pointsUpdated() ) );
	connect( m_ShadeG, SIGNAL( colorsChanged() ),
			 this, SLOT( pointsUpdated() ) );
	connect( m_ShadeB, SIGNAL( colorsChanged() ),
			 this, SLOT( pointsUpdated() ) );
	connect( m_ShadeA, SIGNAL( colorsChanged() ),
			 this, SLOT( pointsUpdated() ) );

	connect( m_ShadeR, SIGNAL( mouseReleased() ),
			 this, SIGNAL( colorTableUpdated() ) );
	connect( m_ShadeG, SIGNAL( mouseReleased() ),
			 this, SIGNAL( colorTableUpdated() ) );
	connect( m_ShadeB, SIGNAL( mouseReleased() ),
			 this, SIGNAL( colorTableUpdated() ) );
	connect( m_ShadeA, SIGNAL( mouseReleased() ),
			 this, SIGNAL( colorTableUpdated() ) );

	connect( m_ShadeR, SIGNAL( updateInfo( ShadeWidget::ShadeType, float, float ) ),
			 this, SLOT( updateInfo( ShadeWidget::ShadeType, float, float ) ) );
	connect( m_ShadeG, SIGNAL( updateInfo( ShadeWidget::ShadeType, float, float ) ),
			 this, SLOT( updateInfo( ShadeWidget::ShadeType, float, float ) ) );
	connect( m_ShadeB, SIGNAL( updateInfo( ShadeWidget::ShadeType, float, float ) ),
			 this, SLOT( updateInfo( ShadeWidget::ShadeType, float, float ) ) );
	connect( m_ShadeA, SIGNAL( updateInfo( ShadeWidget::ShadeType, float, float ) ),
			 this, SLOT( updateInfo( ShadeWidget::ShadeType, float, float ) ) );
}

inline static bool x_less_than( const QPointF &p1, const QPointF &p2 )
{
	return p1.x() < p2.x();
}

void GradientEditor::pointsUpdated()
{
	double w = m_ShadeA->width();
	QGradientStops stops;

	QPolygonF points;

	points += m_ShadeR->points();
	points += m_ShadeG->points();
	points += m_ShadeB->points();
	points += m_ShadeA->points();

	qSort( points.begin(), points.end(), x_less_than );

	for ( int i = 0; i < points.size(); ++i )
	{
		double x = int( points.at( i ).x() );

		if ( ( i < ( points.size() - 1 ) ) && ( x == points.at( i + 1 ).x() ) )
		{
			continue;
		}

		QColor color( ( 0x00ff0000 & m_ShadeR->colorAt(   int( x ) ) ) >> 16,
					  ( 0x0000ff00 & m_ShadeG->colorAt( int( x ) ) ) >> 8,
					  ( 0x000000ff & m_ShadeB->colorAt(  int( x ) ) ),
					  ( 0xff000000 & m_ShadeA->colorAt( int( x ) ) ) >> 24);

		if ( ( x / w ) > 1 )
		{
			return;
		}

		stops << QGradientStop( x / w, color );
	}

	m_ShadeA->setGradientStops( stops );

	emit colorTableUpdated();
}

static void set_shade_points( const QPolygonF &points, ShadeWidget *shade )
{
	shade->colorTable()->setPoints( points );
	shade->colorTable()->setPointLock( 0, ColorTable::LockToLeft );
	shade->colorTable()->setPointLock( points.size() - 1, ColorTable::LockToRight );
	shade->update();
}

void GradientEditor::setGradientStops( const QGradientStops &stops )
{
	QPolygonF pts_red, pts_green, pts_blue, pts_alpha;

	double h_red   = m_ShadeR->height();
	double h_green = m_ShadeG->height();
	double h_blue  = m_ShadeB->height();
	double h_alpha = m_ShadeA->height();

	for ( int i = 0; i < stops.size(); ++i )
	{
		double pos = stops.at(i).first;
		QRgb color = stops.at(i).second.rgba();
		pts_red   << QPointF( pos * m_ShadeR->width(),   h_red   - qRed( color )   * h_red   / 255 );
		pts_green << QPointF( pos * m_ShadeG->width(), h_green - qGreen( color ) * h_green / 255 );
		pts_blue  << QPointF( pos * m_ShadeB->width(),  h_blue  - qBlue( color )  * h_blue  / 255 );
		pts_alpha << QPointF( pos * m_ShadeA->width(), h_alpha - qAlpha( color ) * h_alpha / 255 );
	}

	set_shade_points( pts_red,   m_ShadeR   );
	set_shade_points( pts_green, m_ShadeG );
	set_shade_points( pts_blue,  m_ShadeB  );
	set_shade_points( pts_alpha, m_ShadeA );
}

void GradientEditor::setColorControlPointToParam()
{
	QPair<float, float> controlpoint;
	param.resetColorControlPoint();
	for ( int i = 0; i < m_ShadeR->colorTable()->points().size(); i++ ) {
		controlpoint.first =
			m_ShadeR->colorTable()->points().at(i).x() / m_ShadeR->width();
		controlpoint.second =
			m_ShadeR->colorTable()->points().at(i).y() / m_ShadeR->height();
		param.setColorControlPoint( RED, controlpoint );
	}
	for ( int i = 0; i < m_ShadeG->colorTable()->points().size(); i++ ) {
		controlpoint.first =
			m_ShadeG->colorTable()->points().at(i).x() / m_ShadeG->width();
		controlpoint.second =
			m_ShadeG->colorTable()->points().at(i).y() / m_ShadeG->height();
		param.setColorControlPoint( GREEN, controlpoint );
	}
	for ( int i = 0; i < m_ShadeB->colorTable()->points().size(); i++ ) {
		controlpoint.first =
			m_ShadeB->colorTable()->points().at(i).x() / m_ShadeB->width();
		controlpoint.second =
			m_ShadeB->colorTable()->points().at(i).y() / m_ShadeB->height();
		param.setColorControlPoint( BLUE, controlpoint );
	}
	for ( int i = 0; i < m_ShadeA->colorTable()->points().size(); i++ ) {
		controlpoint.first =
			m_ShadeA->colorTable()->points().at(i).x() / m_ShadeA->width();
		controlpoint.second =
			m_ShadeA->colorTable()->points().at(i).y() / m_ShadeA->height();
		param.setColorControlPoint( ALPHA, controlpoint );
	}
}

void GradientEditor::getColorControlPointFromParam()
{
	m_ShadeR->colorTable()->points().clear();
	m_ShadeG->colorTable()->points().clear();
	m_ShadeB->colorTable()->points().clear();
	m_ShadeA->colorTable()->points().clear();

	QPolygonF pts_red, pts_green, pts_blue, pts_alpha;
	int cp_num;
	cp_num = param.getColorControlPointNum( RED );
	for ( int i = 0; i < cp_num; ++i )
	{
		pts_red   <<
			QPointF(
				param.getColorControlPoint( RED, i ).first * m_ShadeR->width(),
				param.getColorControlPoint( RED, i ).second * m_ShadeR->height() );
	}
	cp_num = param.getColorControlPointNum( GREEN );
	for ( int i = 0; i < cp_num; ++i )
	{
		pts_green   <<
			QPointF(
				param.getColorControlPoint( GREEN, i ).first * m_ShadeG->width(),
				param.getColorControlPoint( GREEN, i ).second * m_ShadeG->height() );
	}
	cp_num = param.getColorControlPointNum( BLUE );
	for ( int i = 0; i < cp_num; ++i )
	{
		pts_blue   <<
			QPointF(
				param.getColorControlPoint( BLUE, i ).first * m_ShadeB->width(),
				param.getColorControlPoint( BLUE, i ).second * m_ShadeB->height() );
	}
	cp_num = param.getColorControlPointNum( ALPHA );
	for ( int i = 0; i < cp_num; ++i )
	{
		pts_alpha   <<
			QPointF(
				param.getColorControlPoint( ALPHA, i ).first * m_ShadeA->width(),
				param.getColorControlPoint( ALPHA, i ).second * m_ShadeA->height() );
	}
	set_shade_points( pts_red,   m_ShadeR   );
	set_shade_points( pts_green, m_ShadeG   );
	set_shade_points( pts_blue,  m_ShadeB   );
	set_shade_points( pts_alpha, m_ShadeA   );

	pointsUpdated();
}

void GradientEditor::setConnectionType( ConnectionType connectionType )
{
	m_ConnecctionType = connectionType;

	if ( ( connectionType != CurveConnection ) &&
		 ( connectionType != LineConnection ) &&
		 ( connectionType != NoConnection ) ) {
		return;
	}

	if ( connectionType == CurveConnection ) {
		getRedShade()->colorTable()->setConnectionType( ColorTable::CurveConnection );
		getGreenShade()->colorTable()->setConnectionType( ColorTable::CurveConnection );
		getBlueShade()->colorTable()->setConnectionType( ColorTable::CurveConnection );
		getAlphaShade()->colorTable()->setConnectionType( ColorTable::CurveConnection );
	} else if ( connectionType == LineConnection ) {
		getRedShade()->colorTable()->setConnectionType( ColorTable::LineConnection );
		getGreenShade()->colorTable()->setConnectionType( ColorTable::LineConnection );
		getBlueShade()->colorTable()->setConnectionType( ColorTable::LineConnection );
		getAlphaShade()->colorTable()->setConnectionType( ColorTable::LineConnection );
	} else {
		getRedShade()->colorTable()->setConnectionType( ColorTable::NoConnection );
		getGreenShade()->colorTable()->setConnectionType( ColorTable::NoConnection );
		getBlueShade()->colorTable()->setConnectionType( ColorTable::NoConnection );
		getAlphaShade()->colorTable()->setConnectionType( ColorTable::NoConnection );
	}

	getRedShade()->update();
	getGreenShade()->update();
	getBlueShade()->update();
	getAlphaShade()->update();

	pointsUpdated();
}

void GradientEditor::toggleConnectionType()
{
	if ( m_ConnecctionType == CurveConnection ) {
		setConnectionType( LineConnection );
	} else {
		setConnectionType( CurveConnection );
	}
}

void GradientEditor::setScaleMarkings()
{
	QSizePolicy sizeHalf( QSizePolicy::Preferred, QSizePolicy::Preferred );
	QSizePolicy sizeNorm( QSizePolicy::Preferred, QSizePolicy::Preferred );
	sizeHalf.setHorizontalStretch( 1 );
	sizeNorm.setHorizontalStretch( 2 );
	for ( int i = 0; i < 20; i++ ) {
		m_Label_ScaleMarking[ i ]->setSizePolicy( sizeNorm );
		m_Label_ScaleMarking[ i ]->setAlignment( Qt::AlignCenter );
		m_Label_ScaleMarking[ i ]->hide();
	}
	int marknum = param.getColorBarMajorTickMarkNum();
	char format[ 32 ];
	char str[ 32 ];
	char strtmp1[ 32 ];
	char strtmp2[ 32 ];
	for ( int i = 0; i < marknum; i++ ) {
		float value;
		if ( !param.IsLogScale() ) {
			value = param.getVolumeThresholdMin()
				+ ( param.getVolumeThresholdMax() - param.getVolumeThresholdMin() )
				/ static_cast< float >( marknum - 1 ) * static_cast< float >( i );
		} else {
			float logvalue = log10( param.getVolumeThresholdMin() )
				+ ( log10( param.getVolumeThresholdMax() ) - log10( param.getVolumeThresholdMin() ) )
				/ static_cast< float >( marknum - 1 ) * static_cast< float >( i );
			value = pow( 10.0, logvalue );
		}
		if ( ( value == 0.0 ) || ( ( fabs( value ) > 0.0001 ) && ( fabs( value ) < 10000.0 ) ) ) {
			sprintf( format, "%%.%df", param.getColorBarDecimalPlacesNum() );
			sprintf( str, format, value );
		} else {
			sprintf( format, "%%.%de", param.getColorBarDecimalPlacesNum() );
			sprintf( strtmp1, format, value );
			int eposition = 0;
			int length = static_cast< int >( strlen( strtmp1 ) );
			for ( int p = 0; p < length; p++ ) {
				if ( strtmp1[ p ] == 'e' ) {
					eposition = p;
					break;
				}
			}
			int q = 0;
			for ( int p = ( eposition + 2 ); p < length; p++ ) {
				strtmp2[ q ] = strtmp1[ p ];
				q++;
			}
			strtmp1[ eposition + 2 ] = '\0';
			strtmp2[ q ] = '\0';
			int exp = atoi( strtmp2 );
			sprintf( str, "%s%d", strtmp1, exp );
		}
		m_Label_ScaleMarking[ i ]->setText( str );
		m_Label_ScaleMarking[ i ]->setSizePolicy( sizeNorm );
		m_Label_ScaleMarking[ i ]->setAlignment( Qt::AlignCenter );
		m_Label_ScaleMarking[ i ]->show();
	}
	m_Label_ScaleMarking[ 0 ]->setAlignment( Qt::AlignLeft );
	m_Label_ScaleMarking[ 0 ]->setSizePolicy( sizeHalf );
	m_Label_ScaleMarking[ 0 ]->show();
	if ( marknum > 0 ) {
		m_Label_ScaleMarking[ marknum - 1 ]->setAlignment( Qt::AlignRight );
		m_Label_ScaleMarking[ marknum - 1 ]->setSizePolicy( sizeHalf );
	} else {
		m_Label_ScaleMarking[ 1 ]->setAlignment( Qt::AlignRight );
		m_Label_ScaleMarking[ 1 ]->setSizePolicy( sizeHalf );
		m_Label_ScaleMarking[ 1 ]->show();
	}
}

void GradientEditor::updateInfo
	( ShadeWidget::ShadeType type, float value, float level )
{
	if ( type == ShadeWidget::RedShade ) {
		m_Label_Level->setText( tr( "R: " ) );
	} else if ( type == ShadeWidget::GreenShade ) {
		m_Label_Level->setText( tr( "G: " ) );
	} else if ( type == ShadeWidget::BlueShade ) {
		m_Label_Level->setText( tr( "B: " ) );
	} else {
		m_Label_Level->setText( tr( "Opacity: " ) );
	}
	m_Label_CurrentValue->setNum( value );
	m_Label_CurrentLevel->setText( QString::number( level, 'f', 2 ) );
}

GradientWidget::GradientWidget( QWidget *parent )
	: QWidget( parent )
{
	QGroupBox *editorGroup = new QGroupBox( this );
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	editorGroup->setAttribute( Qt::WA_ContentsPropagated );
#endif
	editorGroup->setTitle( "Color Editor" );

	QVBoxLayout *editorGroupLayout = new QVBoxLayout( editorGroup );
	m_editor = new GradientEditor( editorGroup );
	editorGroupLayout->addWidget( m_editor );

	QGroupBox *presetColorTableGroup = new QGroupBox( this );
#if QT_VERSION < QT_VERSION_CHECK(4, 1, 0)
	presetColorTableGroup->setAttribute( Qt::WA_ContentsPropagated );
#endif
	presetColorTableGroup->setTitle( "Sample Color Tables" );

	QGridLayout *presetColorTableGroupLayout = new QGridLayout( presetColorTableGroup );
	m_PresetColorTable01 = new PresetColorTableWidget(1);
	m_PresetColorTable02 = new PresetColorTableWidget(2);
	m_PresetColorTable03 = new PresetColorTableWidget(3);
	m_PresetColorTable04 = new PresetColorTableWidget(4);
	m_PresetColorTable05 = new PresetColorTableWidget(5);
	m_PresetColorTable06 = new PresetColorTableWidget(6);
	m_PresetColorTable07 = new PresetColorTableWidget(7);
	m_PresetColorTable08 = new PresetColorTableWidget(8);
	m_PresetColorTable09 = new PresetColorTableWidget(9);
	m_PresetColorTable10 = new PresetColorTableWidget(10);
	m_PresetColorTable11 = new PresetColorTableWidget(11);
	m_PresetColorTable12 = new PresetColorTableWidget(12);
	presetColorTableGroupLayout->addWidget( m_PresetColorTable01, 0, 0 );
	presetColorTableGroupLayout->addWidget( m_PresetColorTable02, 0, 1 );
	presetColorTableGroupLayout->addWidget( m_PresetColorTable03, 0, 2 );
	presetColorTableGroupLayout->addWidget( m_PresetColorTable04, 1, 0 );
	presetColorTableGroupLayout->addWidget( m_PresetColorTable05, 1, 1 );
	presetColorTableGroupLayout->addWidget( m_PresetColorTable06, 1, 2 );
	presetColorTableGroupLayout->addWidget( m_PresetColorTable07, 2, 0 );
	presetColorTableGroupLayout->addWidget( m_PresetColorTable08, 2, 1 );
	presetColorTableGroupLayout->addWidget( m_PresetColorTable09, 2, 2 );
	presetColorTableGroupLayout->addWidget( m_PresetColorTable10, 3, 0 );
	presetColorTableGroupLayout->addWidget( m_PresetColorTable11, 3, 1 );
	presetColorTableGroupLayout->addWidget( m_PresetColorTable12, 3, 2 );

	QVBoxLayout *mainLayout = new QVBoxLayout( this );
	mainLayout->addWidget( editorGroup );
	mainLayout->addWidget( presetColorTableGroup );
	mainLayout->addStretch( 1 );
	setLayout( mainLayout );

	QTimer::singleShot( 50, this, SLOT( setDefault01() ) );

	connect( m_editor, SIGNAL( colorTableUpdated() ),
			this, SLOT( setColorTableToParam() ) );

	connect( m_PresetColorTable01, SIGNAL( setPresetColorTable() ), this, SLOT( setDefault01() ) );
	connect( m_PresetColorTable02, SIGNAL( setPresetColorTable() ), this, SLOT( setDefault02() ) );
	connect( m_PresetColorTable03, SIGNAL( setPresetColorTable() ), this, SLOT( setDefault03() ) );
	connect( m_PresetColorTable04, SIGNAL( setPresetColorTable() ), this, SLOT( setDefault04() ) );
	connect( m_PresetColorTable05, SIGNAL( setPresetColorTable() ), this, SLOT( setDefault05() ) );
	connect( m_PresetColorTable06, SIGNAL( setPresetColorTable() ), this, SLOT( setDefault06() ) );
	connect( m_PresetColorTable07, SIGNAL( setPresetColorTable() ), this, SLOT( setDefault07() ) );
	connect( m_PresetColorTable08, SIGNAL( setPresetColorTable() ), this, SLOT( setDefault08() ) );
	connect( m_PresetColorTable09, SIGNAL( setPresetColorTable() ), this, SLOT( setDefault09() ) );
	connect( m_PresetColorTable10, SIGNAL( setPresetColorTable() ), this, SLOT( setDefault10() ) );
	connect( m_PresetColorTable11, SIGNAL( setPresetColorTable() ), this, SLOT( setDefault11() ) );
	connect( m_PresetColorTable12, SIGNAL( setPresetColorTable() ), this, SLOT( setDefault12() ) );
}

void GradientWidget::setDefault( int config )
{
	QGradientStops stops;
	QPolygonF points;

	switch ( config )
	{
		case 1:
			// note: argument of fromRgba() is 0xAARRGGBB
			stops << QGradientStop( 0.00, QColor::fromRgba( 0x190000ff ) );
			stops << QGradientStop( 0.33, QColor::fromRgba( 0x1900ffff ) );
			stops << QGradientStop( 0.50, QColor::fromRgba( 0x1900ff00 ) );
			stops << QGradientStop( 0.67, QColor::fromRgba( 0x19ffff00 ) );
			stops << QGradientStop( 1.00, QColor::fromRgba( 0x19ff0000 ) );
			// "rainbow1"
			break;

		case 2:
			stops << QGradientStop( 0.00, QColor::fromRgba( 0xff0000ff ) );
			stops << QGradientStop( 0.50, QColor::fromRgba( 0xff7fff7f ) );
			stops << QGradientStop( 1.00, QColor::fromRgba( 0xffff0000 ) );
			break;

		case 3:
			stops << QGradientStop( 0.00, QColor::fromRgba( 0x19FF00FF ) );
			stops << QGradientStop( 0.20, QColor::fromRgba( 0x190000FF ) );
			stops << QGradientStop( 0.40, QColor::fromRgba( 0x1900FFFF ) );
			stops << QGradientStop( 0.60, QColor::fromRgba( 0x1900FF00 ) );
			stops << QGradientStop( 0.80, QColor::fromRgba( 0x19FFFF00 ) );
			stops << QGradientStop( 1.00, QColor::fromRgba( 0x19FF0000 ) );
			// "rainbow2"
			break;

		case 4:
			stops << QGradientStop( 0.00, QColor::fromRgba( 0x19000000 ) );
			stops << QGradientStop( 0.17, QColor::fromRgba( 0x190000FF ) );
			stops << QGradientStop( 0.33, QColor::fromRgba( 0x1900FFFF ) );
			stops << QGradientStop( 0.50, QColor::fromRgba( 0x1900FF00 ) );
			stops << QGradientStop( 0.67, QColor::fromRgba( 0x19FFFF00 ) );
			stops << QGradientStop( 0.83, QColor::fromRgba( 0x19FF0000 ) );
			stops << QGradientStop( 1.00, QColor::fromRgba( 0x19FFFFFF ) );
			// "rainbow3"
			break;

		case 5:
			stops << QGradientStop( 0.00, QColor::fromRgba( 0x00BBBBBB ) );
			stops << QGradientStop( 0.08, QColor::fromRgba( 0x50CCCCCC ) );
			stops << QGradientStop( 0.15, QColor::fromRgba( 0x50FFFFFF ) );
			stops << QGradientStop( 1.00, QColor::fromRgba( 0x50FFFFFF ) );
			// "monochrome"
			break;

		case 6:
			stops << QGradientStop( 0.00, QColor::fromRgba( 0x190000FF ) );
			stops << QGradientStop( 0.50, QColor::fromRgba( 0x19FFFFFF ) );
			stops << QGradientStop( 1.00, QColor::fromRgba( 0x19FF0000 ) );
			// "blue_white_red"
			break;

		case 7:
			stops << QGradientStop( 0.00, QColor::fromRgba( 0x19994C00 ) );
			stops << QGradientStop( 0.50, QColor::fromRgba( 0x19FFFFFF ) );
			stops << QGradientStop( 1.00, QColor::fromRgba( 0x1900FF00 ) );
			// "brown_white_green"
			break;

		case 8:
			stops << QGradientStop( 0.00, QColor::fromRgba( 0x197F0C7F ) );
			stops << QGradientStop( 0.50, QColor::fromRgba( 0x19006626 ) );
			stops << QGradientStop( 1.00, QColor::fromRgba( 0x19FF7F00 ) );
			// "purple_green_orange"
			break;

		case 9:
			stops << QGradientStop( 0.00, QColor::fromRgba( 0x190C00B2 ) );
			stops << QGradientStop( 0.50, QColor::fromRgba( 0x19FF00FF ) );
			stops << QGradientStop( 1.00, QColor::fromRgba( 0x19FFFF00 ) );
			// "blue_red_yeallow"
			break;

		case 10:
			stops << QGradientStop( 0.00, QColor::fromRgba( 0x19F2E500 ) );
			stops << QGradientStop( 0.33, QColor::fromRgba( 0x19721972 ) );
			stops << QGradientStop( 0.66, QColor::fromRgba( 0x19FF8000 ) );
			stops << QGradientStop( 1.00, QColor::fromRgba( 0x1900267F ) );
			// "yellow_purple_orange_blue"
			break;

		case 11:
			stops << QGradientStop( 0.00, QColor::fromRgba( 0x19268CCC ) );
			stops << QGradientStop( 0.50, QColor::fromRgba( 0x19FFFFFF ) );
			stops << QGradientStop( 0.52, QColor::fromRgba( 0x198C7200 ) );
			stops << QGradientStop( 0.64, QColor::fromRgba( 0x19D89900 ) );
			stops << QGradientStop( 1.00, QColor::fromRgba( 0x19FFFFFF ) );
			// "chrome"
			break;

		case 12:
			stops << QGradientStop( 0.00, QColor::fromRgba( 0x190000FF ) );
			stops << QGradientStop( 0.50, QColor::fromRgba( 0x190000FF ) );
			stops << QGradientStop( 0.52, QColor::fromRgba( 0x19FF0000 ) );
			stops << QGradientStop( 1.00, QColor::fromRgba( 0x19FF0000 ) );
			// "blue_red"
			break;

		default:
			qWarning( "bad default: %d\n", config );
			break;
	}

	m_editor->setGradientStops( stops );
	m_editor->setScaleMarkings();
	m_editor->pointsUpdated();
}

void GradientWidget::setColorTableToParam()
{
	m_editor->getRedShade()->colorTable()->set256Table();
	m_editor->getGreenShade()->colorTable()->set256Table();
	m_editor->getBlueShade()->colorTable()->set256Table();
	m_editor->getAlphaShade()->colorTable()->set256Table();

	FloatVector colortable;
	colortable  = m_editor->getRedShade()->colorTable()->get256Table();
	colortable += m_editor->getGreenShade()->colorTable()->get256Table();
	colortable += m_editor->getBlueShade()->colorTable()->get256Table();
	colortable += m_editor->getAlphaShade()->colorTable()->get256Table();
	if ( colortable.size() == 1024 ) {
		for ( int i = 0; i < 256; i++ ) {
			param.setColorTableR( i, colortable[ 256 * 0 + i ] );
			param.setColorTableG( i, colortable[ 256 * 1 + i ] );
			param.setColorTableB( i, colortable[ 256 * 2 + i ] );
			if ( i == 255 ) {
				param.setColorTableA( i, 0.0 );
			} else {
				param.setColorTableA( i, colortable[ 256 * 3 + i ] );
			}
		}
//		emit setColorTableToParamSucceeded();
		emit valueChanged( CHANGE_COLORTABLE );
	}
}

void GradientWidget::setScaleMarkings()
{
	m_editor->setScaleMarkings();
}

PresetColorTableWidget::PresetColorTableWidget( int presetColorTableID, QWidget *parent )
	: QWidget( parent )
{
	QFrame *frame = new QFrame( this );
	frame->setFrameStyle( QFrame::Panel | QFrame::Sunken );
	frame->setGeometry( this->geometry() );

	setMinimumHeight( 20 );
	setAutoFillBackground( false );

	setColorTable( presetColorTableID );
}

void PresetColorTableWidget::setColorTable( int presetColorTableID )
{
	switch ( presetColorTableID )
	{
		case 1:
			// 0xAARRGGBB
			m_GradientStops << QGradientStop( 0.00, QColor::fromRgba( 0x190000ff ) );
			m_GradientStops << QGradientStop( 0.33, QColor::fromRgba( 0x1900ffff ) );
			m_GradientStops << QGradientStop( 0.50, QColor::fromRgba( 0x1900ff00 ) );
			m_GradientStops << QGradientStop( 0.67, QColor::fromRgba( 0x19ffff00 ) );
			m_GradientStops << QGradientStop( 1.00, QColor::fromRgba( 0x19ff0000 ) );
			// "rainbow1"
			break;

		case 2:
			m_GradientStops << QGradientStop( 0.00, QColor::fromRgba( 0xff0000ff ) );
			m_GradientStops << QGradientStop( 0.50, QColor::fromRgba( 0xff7fff7f ) );
			m_GradientStops << QGradientStop( 1.00, QColor::fromRgba( 0xffff0000 ) );
			break;

		case 3:
			m_GradientStops << QGradientStop( 0.00, QColor::fromRgba( 0x19FF00FF ) );
			m_GradientStops << QGradientStop( 0.20, QColor::fromRgba( 0x190000FF ) );
			m_GradientStops << QGradientStop( 0.40, QColor::fromRgba( 0x1900FFFF ) );
			m_GradientStops << QGradientStop( 0.60, QColor::fromRgba( 0x1900FF00 ) );
			m_GradientStops << QGradientStop( 0.80, QColor::fromRgba( 0x19FFFF00 ) );
			m_GradientStops << QGradientStop( 1.00, QColor::fromRgba( 0x19FF0000 ) );
			// "rainbow2"
			break;

		case 4:
			m_GradientStops << QGradientStop( 0.00, QColor::fromRgba( 0x19000000 ) );
			m_GradientStops << QGradientStop( 0.17, QColor::fromRgba( 0x190000FF ) );
			m_GradientStops << QGradientStop( 0.33, QColor::fromRgba( 0x1900FFFF ) );
			m_GradientStops << QGradientStop( 0.50, QColor::fromRgba( 0x1900FF00 ) );
			m_GradientStops << QGradientStop( 0.67, QColor::fromRgba( 0x19FFFF00 ) );
			m_GradientStops << QGradientStop( 0.83, QColor::fromRgba( 0x19FF0000 ) );
			m_GradientStops << QGradientStop( 1.00, QColor::fromRgba( 0x19FFFFFF ) );
			// "rainbow3"
			break;

		case 5:
			m_GradientStops << QGradientStop( 0.00, QColor::fromRgba( 0x00BBBBBB ) );
			m_GradientStops << QGradientStop( 0.08, QColor::fromRgba( 0x50CCCCCC ) );
			m_GradientStops << QGradientStop( 0.15, QColor::fromRgba( 0x50FFFFFF ) );
			m_GradientStops << QGradientStop( 1.00, QColor::fromRgba( 0x50FFFFFF ) );
			// "monochrome"
			break;

		case 6:
			m_GradientStops << QGradientStop( 0.00, QColor::fromRgba( 0x190000FF ) );
			m_GradientStops << QGradientStop( 0.50, QColor::fromRgba( 0x19FFFFFF ) );
			m_GradientStops << QGradientStop( 1.00, QColor::fromRgba( 0x19FF0000 ) );
			// "blue_white_red"
			break;

		case 7:
			m_GradientStops << QGradientStop( 0.00, QColor::fromRgba( 0x19994C00 ) );
			m_GradientStops << QGradientStop( 0.50, QColor::fromRgba( 0x19FFFFFF ) );
			m_GradientStops << QGradientStop( 1.00, QColor::fromRgba( 0x1900FF00 ) );
			// "brown_white_green"
			break;

		case 8:
			m_GradientStops << QGradientStop( 0.00, QColor::fromRgba( 0x197F0C7F ) );
			m_GradientStops << QGradientStop( 0.50, QColor::fromRgba( 0x19006626 ) );
			m_GradientStops << QGradientStop( 1.00, QColor::fromRgba( 0x19FF7F00 ) );
			// "purple_green_orange"
			break;

		case 9:
			m_GradientStops << QGradientStop( 0.00, QColor::fromRgba( 0x190C00B2 ) );
			m_GradientStops << QGradientStop( 0.50, QColor::fromRgba( 0x19FF00FF ) );
			m_GradientStops << QGradientStop( 1.00, QColor::fromRgba( 0x19FFFF00 ) );
			// "blue_red_yeallow"
			break;

		case 10:
			m_GradientStops << QGradientStop( 0.00, QColor::fromRgba( 0x19F2E500 ) );
			m_GradientStops << QGradientStop( 0.33, QColor::fromRgba( 0x19721972 ) );
			m_GradientStops << QGradientStop( 0.66, QColor::fromRgba( 0x19FF8000 ) );
			m_GradientStops << QGradientStop( 1.00, QColor::fromRgba( 0x1900267F ) );
			// "yellow_purple_orange_blue"
			break;

		case 11:
			m_GradientStops << QGradientStop( 0.00, QColor::fromRgba( 0x19268CCC ) );
			m_GradientStops << QGradientStop( 0.50, QColor::fromRgba( 0x19FFFFFF ) );
			m_GradientStops << QGradientStop( 0.52, QColor::fromRgba( 0x198C7200 ) );
			m_GradientStops << QGradientStop( 0.64, QColor::fromRgba( 0x19D89900 ) );
			m_GradientStops << QGradientStop( 1.00, QColor::fromRgba( 0x19FFFFFF ) );
			// "chrome"
			break;

		case 12:
			m_GradientStops << QGradientStop( 0.00, QColor::fromRgba( 0x190000FF ) );
			m_GradientStops << QGradientStop( 0.50, QColor::fromRgba( 0x190000FF ) );
			m_GradientStops << QGradientStop( 0.52, QColor::fromRgba( 0x19FF0000 ) );
			m_GradientStops << QGradientStop( 1.00, QColor::fromRgba( 0x19FF0000 ) );
			// "blue_red"
			break;

		default:
			m_GradientStops << QGradientStop( 0.00, QColor::fromRgba( 0x190000ff ) );
			m_GradientStops << QGradientStop( 0.33, QColor::fromRgba( 0x1900ffff ) );
			m_GradientStops << QGradientStop( 0.50, QColor::fromRgba( 0x1900ff00 ) );
			m_GradientStops << QGradientStop( 0.67, QColor::fromRgba( 0x19ffff00 ) );
			m_GradientStops << QGradientStop( 1.00, QColor::fromRgba( 0x19ff0000 ) );
			// Same as case 1
			break;
	}
}

void PresetColorTableWidget::paintEvent( QPaintEvent * )
{
	m_Gradient = QLinearGradient( 0, 0, width(), 0 );

	for ( int i = 0; i < m_GradientStops.size(); ++i ) {
		QColor c = m_GradientStops.at(i).second;
		m_Gradient.setColorAt( m_GradientStops.at(i).first, QColor( c.red(), c.green(), c.blue() ) );
	}
	QPainter p( this );
	p.fillRect( rect(), m_Gradient );
}

void PresetColorTableWidget::mousePressEvent( QMouseEvent *event )
{
	if ( event->button() == Qt::LeftButton ) {
		update();
		emit setPresetColorTable();
	}
}
