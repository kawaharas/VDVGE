//
// Volume Data Visualizer for Google Earth
//
// gradients.h
//
// Original Code: gradients.h
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

#ifndef GRADIENTS_H
#define GRADIENTS_H

#include <QtGui>
#include "define.h"
#include "colortable.h"
#include "parameter.h"

const int MAX_SCALE_MARKINGS = 20;

extern Parameter param;

class ColorTable;
class PresetColorTableWidget;

class ShadeWidget : public QWidget
{
	Q_OBJECT

public:
	enum ShadeType
	{
		RedShade,
		GreenShade,
		BlueShade,
		ARGBShade
	};

	ShadeWidget(ShadeType type, QWidget *parent);

	void setGradientStops(const QGradientStops &stops);

	void paintEvent(QPaintEvent *e);

	QSize sizeHint() const { return QSize( 380, 100 ); }
	QPolygonF points() const { return m_colorTable->points(); }

	ColorTable *colorTable() const { return m_colorTable; }

	uint colorAt(int x);

	void mouseReleaseEvent ( QMouseEvent * event );

	void mouseMoveEvent ( QMouseEvent * event );

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	void enterEvent( QEnterEvent * event );
#else
	void enterEvent( QEvent * event );
#endif
	void leaveEvent( QEvent * event );

signals:
	void colorsChanged();
	void mouseReleased();
	void updateInfo( ShadeWidget::ShadeType, float, float );

private:
	void generateShade();

	ShadeType       m_ShadeType;
	QImage          m_shade;
	ColorTable     *m_colorTable;
	QLinearGradient m_alpha_gradient;
	QLabel         *m_Label;

	bool            m_IsEnter;
	QPoint          m_MousePos;
	float           m_Value;
	float           m_Level;
	int             m_TextMargin;
};

class GradientEditor : public QWidget
{
	Q_OBJECT

public:
	GradientEditor( QWidget *parent );

	enum ConnectionType {
		NoConnection,
		LineConnection,
		CurveConnection
	};

	void setGradientStops( const QGradientStops &stops );

	ShadeWidget* getRedShade()   { return m_ShadeR;   }
	ShadeWidget* getGreenShade() { return m_ShadeG; }
	ShadeWidget* getBlueShade()  { return m_ShadeB;  }
	ShadeWidget* getAlphaShade() { return m_ShadeA; }

	void setConnectionType( ConnectionType );
	ConnectionType getConnectionType() { return m_ConnecctionType; }
	void setColorControlPointToParam();
	void getColorControlPointFromParam();
	void toggleConnectionType();

public slots:
	void pointsUpdated();
	void setScaleMarkings();
	void updateInfo( ShadeWidget::ShadeType, float, float );

signals:
	void colorTableUpdated();

private:
	ConnectionType m_ConnecctionType;
	ShadeWidget *m_ShadeR;
	ShadeWidget *m_ShadeG;
	ShadeWidget *m_ShadeB;
	ShadeWidget *m_ShadeA;
	QLabel      *m_Label_ScaleMarking[20];
	QLabel      *m_Label_Value;
	QLabel      *m_Label_Level;
	QLabel      *m_Label_CurrentValue;
	QLabel      *m_Label_CurrentLevel;
};


class GradientWidget : public QWidget
{
	Q_OBJECT

public:
	GradientWidget( QWidget *parent = 0 );
	GradientEditor* getEditor() { return m_editor; }

public slots:
	void setDefault01() { setDefault(1); }
	void setDefault02() { setDefault(2); }
	void setDefault03() { setDefault(3); }
	void setDefault04() { setDefault(4); }
	void setDefault05() { setDefault(5); }
	void setDefault06() { setDefault(6); }
	void setDefault07() { setDefault(7); }
	void setDefault08() { setDefault(8); }
	void setDefault09() { setDefault(9); }
	void setDefault10() { setDefault(10); }
	void setDefault11() { setDefault(11); }
	void setDefault12() { setDefault(12); }

	void setColorTableToParam();
	void setScaleMarkings();

signals:
	void setColorTableToParamSucceeded();
	void valueChanged( ValueType );

private:
	void setDefault( int i );

	GradientEditor *m_editor;
	PresetColorTableWidget *m_PresetColorTable01;
	PresetColorTableWidget *m_PresetColorTable02;
	PresetColorTableWidget *m_PresetColorTable03;
	PresetColorTableWidget *m_PresetColorTable04;
	PresetColorTableWidget *m_PresetColorTable05;
	PresetColorTableWidget *m_PresetColorTable06;
	PresetColorTableWidget *m_PresetColorTable07;
	PresetColorTableWidget *m_PresetColorTable08;
	PresetColorTableWidget *m_PresetColorTable09;
	PresetColorTableWidget *m_PresetColorTable10;
	PresetColorTableWidget *m_PresetColorTable11;
	PresetColorTableWidget *m_PresetColorTable12;
};

class PresetColorTableWidget : public QWidget
{
	Q_OBJECT

public:
	PresetColorTableWidget( int presetColorTableID, QWidget *parent = 0 );
	void paintEvent( QPaintEvent * );

private:
	QLinearGradient m_Gradient;
	QGradientStops  m_GradientStops;

protected:
	void setColorTable( int presetColorTableID );
	void mousePressEvent( QMouseEvent *event );

signals:
	void setPresetColorTable();
};

#endif // GRADIENTS_H
