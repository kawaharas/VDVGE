//
// Volume Data Visualizer for Google Earth
//
// colortable.cpp
//
// Original Code: hoverpoints.cpp
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

#include "colortable.h"

ColorTable::ColorTable( QWidget *widget, PointShape shape )
	: QObject( widget )
{
	m_widget = widget;
	widget->installEventFilter(this);

	m_connectionType = CurveConnection;
	m_sortType = NoSort;
	m_shape = shape;
	m_pointPen = QPen(QColor(255, 255, 255, 191), 1);
	m_connectionPen = QPen(QColor(255, 255, 255, 127), 2);
	m_pointBrush = QBrush(QColor(191, 191, 191, 127));
	m_pointSize = QSize(11, 11);
	m_currentIndex = -1;
	m_editable = true;
	m_enabled = true;

	connect( this, SIGNAL( pointsChanged( const QPolygonF & ) ),
		m_widget, SLOT( update() ) );
}


void ColorTable::setEnabled(bool enabled)
{
	if (m_enabled != enabled) {
		m_enabled = enabled;
		m_widget->update();
	}
}


bool ColorTable::eventFilter( QObject *object, QEvent *event )
{
	if ( object == m_widget && m_enabled ) {
		switch ( event->type() ) {

		case QEvent::MouseButtonPress:
		{
			QMouseEvent *me = (QMouseEvent *) event;

			QPointF clickPos = me->pos();
			int index = -1;
			for (int i=0; i<m_points.size(); ++i) {
				QPainterPath path;
				if (m_shape == CircleShape)
					path.addEllipse(pointBoundingRect(i));
				else
					path.addRect(pointBoundingRect(i));

				if (path.contains(clickPos)) {
					index = i;
					break;
				}
			}

			if (me->button() == Qt::LeftButton) {
				if (index == -1) {
					if (!m_editable)
						return false;
					int pos = 0;
					// Insert sort for x or y
					if (m_sortType == XSort) {
						for (int i=0; i<m_points.size(); ++i)
							if (m_points.at(i).x() > clickPos.x()) {
								pos = i;
								break;
							}
					} else if (m_sortType == YSort) {
						for (int i=0; i<m_points.size(); ++i)
							if (m_points.at(i).y() > clickPos.y()) {
								pos = i;
								break;
							}
					}

					m_points.insert(pos, clickPos);
					m_locks.insert(pos, 0);
					m_currentIndex = pos;
					firePointChange();
				} else {
					m_currentIndex = index;
				}
				return true;

			} else if (me->button() == Qt::RightButton) {
				if (index >= 0 && m_editable) {
					if (m_locks[index] == 0) {
						m_locks.remove(index);
						m_points.remove(index);
					}
					firePointChange();
					return true;
				}
			}

		}
		break;

		case QEvent::MouseButtonRelease:
			m_currentIndex = -1;
			break;

		case QEvent::MouseMove:
			if (m_currentIndex >= 0)
				movePoint(m_currentIndex, ((QMouseEvent *)event)->pos());
			break;

		case QEvent::Resize:
		{
			QResizeEvent *e = (QResizeEvent *) event;
			if (e->oldSize().width() == 0 || e->oldSize().height() == 0)
				break;
			double stretch_x = e->size().width() / double(e->oldSize().width());
			double stretch_y = e->size().height() / double(e->oldSize().height());
			for (int i=0; i<m_points.size(); ++i) {
				QPointF p = m_points[i];
				movePoint(i, QPointF(p.x() * stretch_x, p.y() * stretch_y), false);
			}

			firePointChange();
			break;
		}

		case QEvent::Paint:
		{
			QWidget *that_widget = m_widget;
			m_widget = 0;
			QApplication::sendEvent(object, event);
			m_widget = that_widget;
			paintPoints();

			return true;
		}
		default:
			break;
		}
	}

	return false;
}

void ColorTable::paintPoints()
{
	QPainter p;
	p.begin(m_widget);

	p.setRenderHint(QPainter::Antialiasing);

	if (m_connectionPen.style() != Qt::NoPen && m_connectionType != NoConnection) {
		p.setPen(m_connectionPen);

		if (m_connectionType == CurveConnection) {

			QPainterPath path;
			path.moveTo(m_points.at(0));
			for (int i=1; i<m_points.size(); ++i) {
				QPointF p1 = m_points.at(i-1);
				QPointF p2 = m_points.at(i);
				double distance = p2.x() - p1.x();

				path.cubicTo(p1.x() + distance / 2, p1.y(),
							 p1.x() + distance / 2, p2.y(),
							 p2.x(), p2.y());
			}
			p.drawPath(path);

			set256Table();

		} else {
			p.drawPolyline(m_points);
		}
	}

	p.setPen(m_pointPen);
	p.setBrush(m_pointBrush);

	for (int i=0; i<m_points.size(); ++i) {
		QRectF bounds = pointBoundingRect(i);
		if (m_shape == CircleShape)
			p.drawEllipse(bounds);
		else
			p.drawRect(bounds);
	}
}


void ColorTable::set256Table()
{
	m_256table.clear();

	for ( int x = 0; x < 255; x++ ) {
		int x_256level = ( int )( ( float )x / 254.0 * m_widget->size().width() );
		int y = ( int )interpolation( x_256level );
		if ( y < 0 ) {
			y = 0;
		} else if ( y >  m_widget->size().height() ) {
			y =  m_widget->size().height();
		}
		float y_temp = ( float )( m_widget->size().height() - y ) / ( float )m_widget->size().height();
		m_256table.push_back( y_temp );
	}
	m_256table.push_back( 0.0 ); // for level = 255 (reserved: out of range and undef)
}

static QPointF bound_point(const QPointF &point, const QRectF &bounds, int lock)
{
	QPointF p = point;

	double left = bounds.left();
	double right = bounds.right();
	double top = bounds.top();
	double bottom = bounds.bottom();

	if (p.x() < left || (lock & ColorTable::LockToLeft)) p.setX(left);
	else if (p.x() > right || (lock & ColorTable::LockToRight)) p.setX(right);

	if (p.y() < top || (lock & ColorTable::LockToTop)) p.setY(top);
	else if (p.y() > bottom || (lock & ColorTable::LockToBottom)) p.setY(bottom);

	return p;
}

void ColorTable::setPoints(const QPolygonF &points)
{
	m_points.clear();
	for (int i=0; i<points.size(); ++i)
		m_points << bound_point(points.at(i), boundingRect(), 0);

	m_locks.clear();
	if (m_points.size() > 0) {
		m_locks.resize(m_points.size());

		m_locks.fill(0);
	}
}


void ColorTable::movePoint(int index, const QPointF &point, bool emitUpdate)
{
	m_points[index] = bound_point(point, boundingRect(), m_locks.at(index));
	if ( ( point.x() >= 0 ) &&
		 ( point.x() <= boundingRect().width() ) &&
		 ( point.y() >= 0 ) &&
		 ( point.y() <= boundingRect().height() ) ) {
	}

	if (emitUpdate)
		firePointChange();
}


inline static bool x_less_than(const QPointF &p1, const QPointF &p2)
{
	return p1.x() < p2.x();
}


inline static bool y_less_than(const QPointF &p1, const QPointF &p2)
{
	return p1.y() < p2.y();
}

void ColorTable::firePointChange()
{
	if ( m_sortType != NoSort ) {

		QPointF oldCurrent;
		if ( m_currentIndex != -1 ) {
			oldCurrent = m_points[ m_currentIndex ];
		}

		if ( m_sortType == XSort )
			qSort( m_points.begin(), m_points.end(), x_less_than );
		else if ( m_sortType == YSort )
			qSort( m_points.begin(), m_points.end(), y_less_than );

		// check and remove duplicate points
		for ( int i = 0; i < m_points.size(); i++ ) {
			int num_of_duplicate = 0;
			for ( int j = ( i + 1 ); j < m_points.size(); j++ ) {
				if ( m_points.at( j ) == m_points.at( i ) ) {
					num_of_duplicate++;
				}
			}
			for ( int k = 0; k < num_of_duplicate; k++ ) {
				m_points.remove( i );
				m_locks.remove( i );
			}
		}

		for ( int i = 0; i < m_points.size(); i++ ) {
			for ( int j = ( i + 1 ); j < m_points.size(); j++ ) {
				if ( static_cast< int >( m_points.at( i ).x() ) ==
					 static_cast< int >( m_points.at( j ).x() ) ) {
					if ( m_points.at( i ) == oldCurrent ) {
						m_points.remove( j );
						m_locks.remove( j );
					} else {
						m_points.remove( i );
						m_locks.remove( i );
					}
				}
			}
		}
		m_locks.first() = LockToLeft;
		m_locks.last()  = LockToRight;

		// Compensate for changed order...
		if ( m_currentIndex != -1 ) {
			for ( int i=0; i<m_points.size(); ++i ) {
				if ( m_points[i] == oldCurrent ) {
					m_currentIndex = i;
					break;
				}
			}
		}
	}

	emit pointsChanged(m_points);
}

double ColorTable::interpolation( int x )
{
	for ( int i = 1; i < m_points.size(); ++i ) {
		QPointF p1 = m_points.at(i-1);
		QPointF p2 = m_points.at(i);
		if( x <= p2.x() ) {
			double x1 = p1.x();
			double x2 = p2.x();
			double y1 = p1.y();
			double y2 = p2.y();
			if ( m_connectionType == CurveConnection ) {
				// Ferguson / Coons Curve
				double leftBoundAngle  = 0.0;
				double rightBoundAngle = 0.0;
				double dy1 = tan( leftBoundAngle );
				double dy2 = tan( rightBoundAngle );
				double a = ((dy1+dy2) - 2*(y1-y2)/(x1-x2))/pow(x1-x2, 2);
				double b = 0.5*(dy1-dy2)/(x1-x2) - 1.5*a*(x1+x2);
				double c = (y1-y2)/(x1-x2) - (x1*x1+x1*x2+x2*x2)*a - (x1+x2)*b;
				double d = y2 - (a*x2*x2*x2 + b*x2*x2 + c*x2);
				double val = a*x*x*x + b*x*x + c*x + d;
				return val;
			} else {
				// Linear
				double x1 = p1.x();
				double y1 = p1.y();
				double x2 = p2.x();
				double y2 = p2.y();
				double val = ( y2 - y1 ) / ( x2 - x1 ) * ( static_cast< double >( x ) - x1 ) + y1;
				return val;
			}
		}
	}
	return 0;
}

void ColorTable::setConnectionType( ConnectionType connectionType )
{
	m_connectionType = connectionType;
}
