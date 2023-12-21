//
// Volume Data Visualizer for Google Earth
//
// colortable.h
//
// Original Code: hoverpoints.h
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

#ifndef COLORTABLE_H
#define COLORTABLE_H

#include <QtGui>
#include <QMetaType>
//class QBypassWidget;

#include "../src/parameter.h"
extern Parameter param;

typedef QVector<float> FloatVector;

class ColorTable : public QObject
{
	Q_OBJECT

public:
	enum PointShape {
		CircleShape,
		RectangleShape
	};

	enum LockType {
		LockToLeft   = 0x01,
		LockToRight  = 0x02,
		LockToTop    = 0x04,
		LockToBottom = 0x08
	};

	enum SortType {
		NoSort,
		XSort,
		YSort
	};

	enum ConnectionType {
		NoConnection,
		LineConnection,
		CurveConnection
	};

	ColorTable( QWidget *widget, PointShape shape );

	bool eventFilter( QObject *object, QEvent *event );

	void paintPoints();
	void set256Table();

	inline QRectF boundingRect() const;
	void setBoundingRect( const QRectF &boundingRect ) { m_bounds = boundingRect; }

	QPolygonF points() const { return m_points; }
	void setPoints( const QPolygonF &points );

	QSizeF pointSize() const { return m_pointSize; }
	void setPointSize( const QSizeF &size ) { m_pointSize = size; }

	SortType sortType() const { return m_sortType; }
	void setSortType( SortType sortType ) { m_sortType = sortType; }

	ConnectionType connectionType() const { return m_connectionType; }
	void setConnectionType( ConnectionType connectionType );

	void setConnectionPen( const QPen &pen ) { m_connectionPen = pen; }
	void setShapePen( const QPen &pen ) { m_pointPen = pen; }
	void setShapeBrush( const QBrush &brush ) { m_pointBrush = brush; }

	void setPointLock( int pos, LockType lock ) { m_locks[pos] = lock; }

	void setEditable( bool editable ) { m_editable = editable; }
	bool editable() const { return m_editable; }

	double interpolation( int x );

	QVector<float> get256Table() { return m_256table; }

public slots:
	void setEnabled( bool enabled );
	void setDisabled( bool disabled ) { setEnabled( !disabled ); }

signals:
	void pointsChanged( const QPolygonF &points );

public:
	void firePointChange();

private:
	inline QRectF pointBoundingRect( int i ) const;
	void movePoint( int i, const QPointF &newPos, bool emitChange = true );

	QWidget *m_widget;

	QPolygonF m_points;
	QRectF m_bounds;
	PointShape m_shape;
	SortType m_sortType;
	ConnectionType m_connectionType;

	QVector<uint> m_locks;

	QSizeF m_pointSize;
	int m_currentIndex;
	bool m_editable;
	bool m_enabled;

	QPen m_pointPen;
	QBrush m_pointBrush;
	QPen m_connectionPen;

	QVector<float> m_256table;
};


inline QRectF ColorTable::pointBoundingRect( int i ) const
{
	QPointF p = m_points.at(i);
	double  w = m_pointSize.width();
	double  h = m_pointSize.height();
	double  x = p.x() - w / 2;
	double  y = p.y() - h / 2;
	return QRectF(x, y, w, h);
}

inline QRectF ColorTable::boundingRect() const
{
	if ( m_bounds.isEmpty() )
		return m_widget->rect();
	else
		return m_bounds;
}

#endif // COLORTABLE_H
