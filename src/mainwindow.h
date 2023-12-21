//
// Volume Data Visualizer for Google Earth
//
// mainwindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtGui>
#else
#include <QtWidgets>
#endif

#include "../src/define.h"
#include "../src/grads.h"
#include "../src/parameter.h"
#include "app_parameter.h"
#include "viewer.h"
#include "panels/control_panel.h"
#include "panels/viewer_control.h"
#include "panels/kml_control.h"
#include "panels/font_control.h"
#include "panels/fileinfo_panel.h"
#include "gradients.h"

/*
namespace Ui {
    class MainWindow;
}
*/

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow( QWidget *parent = 0 );
	~MainWindow();

signals:
	void execTimerEvent( int time );
	void openGradsFileSucceeded();
	void setCurrentTimeSucceeded();
	void changeViewMode( bool state );
	void getSnapShot( QString dirPath, int stepnum );

public slots:
//	void updateColorTable();
	void timerEvent();
	void setCurrentTime( int time );
	void toggleAnimationState();
	void toggleFullScreen() { m_IsFullScreen = !m_IsFullScreen; }
	void toggleViewMode();
	void toggleProjection();
	void toggleStateControlPanel();
	void toggleConnectionTypeColorEditor();
	void setObjectToCenter();
	void openGradsFile( const QString &filename );
	void changeViewerSize();

protected:
	void changeEvent( QEvent *e );
	void keyPressEvent( QKeyEvent *event );

private:
	void makeWidget();
	void setupControlPanel();
	void setupMenuBar();
	void setupStatusBar();
	void setupToolBar();

//	Ui::MainWindow *ui;

	QWidget     *m_CentralWidget;
	QGridLayout *m_GridLayout;
	QVBoxLayout *m_vBoxLayout;
	QHBoxLayout *m_hBoxLayout;

	Viewer   *m_Viewer;
	QSlider  *m_HSlider_TimeSlider;
	QPushButton *m_Button_StartAnimation;

	ControlPanel *m_ControlPanel;

	QMenuBar *m_MenuBar;
	QMenu    *m_Menu_File;
	QAction  *m_Action_File_Open;
	QAction  *m_Action_File_Save;
	QAction  *m_Action_File_Load;
	QAction  *m_Action_File_SaveColorTable;
	QAction  *m_Action_File_LoadColorTable;
	QAction  *m_Action_File_Exit;
	QMenu    *m_Menu_Export;
	QAction  *m_Action_Export_KML;
	QAction  *m_Action_Export_SequentialImages;
	QAction  *m_Action_Export_Movie;
	QMenu    *m_Menu_Settings;
	QAction  *m_Action_Settings_ViewerSize;
	QAction  *m_Action_Settings_KMLImageSize;
	QAction  *m_Action_Settings_Font;

	QStatusBar *m_StatusBar;

	QToolBar *m_ToolBar;
	QAction  *m_Action_ToolButton_DefaultView;
	QAction  *m_Action_ToolButton_FrontView;
	QAction  *m_Action_ToolButton_LeftSideView;
	QAction  *m_Action_ToolButton_BottomView;
	QAction  *m_Action_ToolButton_Projection;
	QAction  *m_Action_ToolButton_Centering;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	QElapsedTimer    *m_Time;
#else
	QTime    *m_Time;
#endif
	QTimer   *m_InterruptTimer;	// Internal Timer for Interrupt
	QMargins  m_Margins;
	QString   m_CurrentDir;

	bool      m_IsAnimation;
	bool      m_IsFullScreen;
	bool      m_IsSphereView;
	bool      m_IsVisibleControlPanel;

	ViewerControl *m_ViewerControl;
	KMLControl    *m_KMLControl;
	FontControl   *m_FontControl;

private slots:
	void openGrADS();
	void saveSettings();
	void loadSettings();
	void saveColorTable();
	void loadColorTable();
	void exitApp();
	void exportKML();
	void exportSequentialImages();
	void exportMovie();
	void setViewerSize();
	void setKMLImageSize();
	void setFont();
};

#endif // MAINWINDOW_H
