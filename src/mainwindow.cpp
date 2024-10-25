//
// Volume Data Visualizer for Google Earth
//
// mainwindow.cpp
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

#include "mainwindow.h"
//#include "ui_mainwindow.h"

Grads grads;
Parameter param;
AppParameter appParam;

//MainWindow::MainWindow( QWidget *parent ) :
//	QMainWindow( parent ), ui( new Ui::MainWindow )
MainWindow::MainWindow( QWidget *parent ) :
	QMainWindow( parent )
{
//	ui->setupUi( this );

	setWindowTitle( tr( "VDVGE" ) );
	setEnabled( true );
	resize( 993, 800 );
	QSizePolicy mwSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	mwSizePolicy.setHorizontalStretch( 0 );
	mwSizePolicy.setVerticalStretch( 0 );
	mwSizePolicy.setHeightForWidth( sizePolicy().hasHeightForWidth() );
	setSizePolicy( mwSizePolicy );
	setMinimumSize( 993, 800 );
	m_CentralWidget = new QWidget( this );
//	QSizePolicy cwSizePolicy( QSizePolicy::Maximum, QSizePolicy::Fixed );
	QSizePolicy cwSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	cwSizePolicy.setHorizontalStretch( 0 );
	cwSizePolicy.setVerticalStretch( 0 );
	cwSizePolicy.setHeightForWidth( m_CentralWidget->sizePolicy().hasHeightForWidth() );
	m_CentralWidget->setSizePolicy( cwSizePolicy );
//	m_CentralWidget->setMinimumSize( 992, 600 );
	m_GridLayout = new QGridLayout( m_CentralWidget );
	m_GridLayout->setSpacing( 6 );
	m_GridLayout->setContentsMargins( 11, 11, 11, 11 );
	m_hBoxLayout = new QHBoxLayout();
	m_hBoxLayout->setSpacing( 6 );
	m_hBoxLayout->setSizeConstraint( QLayout::SetDefaultConstraint );
	m_vBoxLayout = new QVBoxLayout();
	m_Viewer = new Viewer( m_CentralWidget );
	QSizePolicy viewerSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	viewerSizePolicy.setHorizontalStretch( 0 );
	viewerSizePolicy.setVerticalStretch( 0 );
	viewerSizePolicy.setHeightForWidth( m_Viewer->sizePolicy().hasHeightForWidth() );
	m_Viewer->setSizePolicy( viewerSizePolicy );
	m_Viewer->setFocusPolicy( Qt::NoFocus );

//	setMinimumSize( 993, 800 );
//	setMinimumSize( 720, 640 );
	makeWidget();

	m_HSlider_TimeSlider = new QSlider;
	QSizePolicy timeSliderSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
	timeSliderSizePolicy.setHorizontalStretch( 0 );
	timeSliderSizePolicy.setVerticalStretch( 0 );
	m_HSlider_TimeSlider->setSizePolicy( timeSliderSizePolicy );
	m_HSlider_TimeSlider->setMaximum( 1000 );
	m_HSlider_TimeSlider->setPageStep( 1 );
	m_HSlider_TimeSlider->setOrientation( Qt::Horizontal );
	m_HSlider_TimeSlider->setTickPosition( QSlider::NoTicks );

	QIcon playAnimationIcon;
	playAnimationIcon.addFile( ":/resources/play.png", QSize(), QIcon::Normal, QIcon::Off );
	playAnimationIcon.addFile( ":/resources/pause.png", QSize(), QIcon::Normal, QIcon::On );
	m_Button_StartAnimation = new QPushButton;
	QSizePolicy playAnimationSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
	playAnimationSizePolicy.setHorizontalStretch( 0 );
	playAnimationSizePolicy.setVerticalStretch( 0 );
	m_Button_StartAnimation->setSizePolicy( playAnimationSizePolicy );
	m_Button_StartAnimation->setMinimumSize( 16, 16 );
	m_Button_StartAnimation->setMaximumSize( 16, 16 );
	m_Button_StartAnimation->setIcon( playAnimationIcon );
	m_Button_StartAnimation->setFlat( true );
	connect( m_HSlider_TimeSlider, SIGNAL( valueChanged( int ) ),
			 this, SLOT( setCurrentTime( int ) ) );
	connect( m_Button_StartAnimation, SIGNAL( released() ),
			 this, SLOT( toggleAnimationState() ) );

	QHBoxLayout *hBoxLayout = new QHBoxLayout();
	hBoxLayout->addWidget( m_HSlider_TimeSlider );
	hBoxLayout->addWidget( m_Button_StartAnimation );
//	ui->widget_Viewer->hide();
//	ui->verticalLayout_Viewer->addWidget( m_Viewer );
//	ui->horizontalLayout_MainWindow->addWidget( m_Viewer );
//	ui->verticalLayout_Viewer->addLayout( hBoxLayout );
	m_vBoxLayout->addWidget( m_Viewer );
	m_vBoxLayout->addLayout( hBoxLayout );
	m_HSlider_TimeSlider->hide();
	m_Button_StartAnimation->hide();

	m_hBoxLayout->addLayout( m_vBoxLayout );
	m_hBoxLayout->addWidget( m_ControlPanel );

	m_GridLayout->addLayout( m_hBoxLayout, 0, 0, 1, 1 );
	setCentralWidget( m_CentralWidget );

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
	QString ffmpegFileName = QCoreApplication::applicationDirPath() + "/ffmpeg.exe";
	QFile ffmpegEXE( ffmpegFileName );
	if ( ffmpegEXE.exists() == true ) {
		m_Action_Export_Movie->setDisabled( false );
	}
#else
	QString ffmpegFileName = "/ffmpeg";
	QString pathString = QString( getenv( "PATH" ) );
	QStringList pathList = splitString( ":", pathString );
	for ( int i = 0; i < pathList.size(); i++ ) {
		if ( QFile::exists( pathList.at(i) + ffmpegFileName ) ) {
			m_Action_Export_Movie->setDisabled( false );
			break;
		}
	}
#endif
#else
#ifdef Q_OS_WIN
	QString ffmpegFileName = QCoreApplication::applicationDirPath() + "/ffmpeg.exe";
	QFile ffmpegEXE( ffmpegFileName );
	if ( ffmpegEXE.exists() == true ) {
		m_Action_Export_Movie->setDisabled( false );
	}
#else
	QString ffmpegFileName = "/ffmpeg";
	QString pathString = QString( getenv( "PATH" ) );
	QStringList pathList = splitString( ":", pathString );
	for ( int i = 0; i < pathList.size(); i++ ) {
		if ( QFile::exists( pathList.at(i) + ffmpegFileName ) ) {
			m_Action_Export_Movie->setDisabled( false );
			break;
		}
	}
#endif
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	m_Time  = new QElapsedTimer;
#else
	m_Time  = new QTime;
#endif
	m_InterruptTimer = new QTimer;
	m_IsAnimation  = false;
	m_IsFullScreen = false;
	m_IsSphereView = false;
	m_IsVisibleControlPanel = true;
	m_CurrentDir = QDir::homePath();

	connect( m_InterruptTimer, SIGNAL( timeout() ),
			 this, SLOT( timerEvent() ) );

//	connect( this, SIGNAL( changeViewMode( bool ) ),
//			 ui->widget_Viewer, SLOT( setViewMode( bool ) ) );
//	connect( this, SIGNAL( getSnapShot( QString, int ) ),
//			 ui->widget_Viewer, SLOT( renderToPNG( QString, int ) ) );
	connect( this, SIGNAL( changeViewMode( bool ) ),
			 m_Viewer, SLOT( setViewMode( bool ) ) );
	connect( this, SIGNAL( getSnapShot( QString, int ) ),
			 m_Viewer, SLOT( renderToPNG( QString, int ) ) );

	// Sets the Current Time as Start Time of Execution
	m_Time->start();
	// Starts the Internal Timer with a Timeout Interval of 10 msec
	m_InterruptTimer->start( 10 );

	// Get Margins of MainLayout to toggle Full Screen Mode and Window Mode
//	m_Margins = ui->gridLayout->contentsMargins();
	m_Margins = m_GridLayout->contentsMargins();

	// Get Initial Size of Viewer
//	param.setViewerSize( ui->widget_Viewer->size() );
	param.setViewerSize( m_Viewer->size() );
	m_ViewerControl = new ViewerControl();
	connect( m_ViewerControl, SIGNAL( valueChanged( ValueType ) ),
			 this, SLOT( changeViewerSize() ) );

	m_KMLControl = new KMLControl();
	param.setKMLMaxImageSize(
		KML_MAX_IMAGE_SIZE_X,
		KML_MAX_IMAGE_SIZE_Y,
		KML_MAX_IMAGE_SIZE_Z );

	m_FontControl = new FontControl;

	appParam.initialize();
	ErrorCode err = appParam.load();
	if ( err != APPPARAM_NO_ERROR ) {
		appParam.initialize();
		statusBar()->showMessage( "Failed to load app parameters. Use default values." );
	}

	// Check Topography Files
	QString topoFileName = QCoreApplication::applicationDirPath() + "/ETOPO5.DOS";
	QFile topoFile( topoFileName );
	if ( topoFile.exists() ) {
		appParam.setStateTopo( true );
	}
}

MainWindow::~MainWindow()
{
//	delete ui;
}

void MainWindow::changeEvent( QEvent *e )
{
	QMainWindow::changeEvent( e );
	switch ( e->type() ) {
	case QEvent::LanguageChange:
//		ui->retranslateUi( this );
		break;
	default:
		break;
	}
}

void MainWindow::keyPressEvent( QKeyEvent *event )
{
	switch( event->key() )
	{
		case Qt::Key_F1 :
			toggleFullScreen();
			if ( m_IsFullScreen ) {
				m_ToolBar->hide();
//				ui->gridLayout->setContentsMargins( 0, 0, 0, 0 );
				m_GridLayout->setContentsMargins( 0, 0, 0, 0 );
				menuBar()->hide();
				statusBar()->hide();
				m_ControlPanel->hide();
				if ( grads.getGridSizeT() > 1 ) {
					m_HSlider_TimeSlider->hide();
					m_Button_StartAnimation->hide();
				}
				showFullScreen();
			} else {
				m_ToolBar->show();
//				ui->gridLayout->setContentsMargins( m_Margins );
				m_GridLayout->setContentsMargins( m_Margins );
				menuBar()->show();
				statusBar()->show();
				m_ControlPanel->show();
				if ( grads.getGridSizeT() > 1 ) {
					m_HSlider_TimeSlider->show();
					m_Button_StartAnimation->show();
				}
				showNormal();
			}
			break;
		case Qt::Key_F2 :
			toggleViewMode();
			break;
		case Qt::Key_F3 :
			toggleStateControlPanel();
			break;
		case Qt::Key_F12 :
			toggleConnectionTypeColorEditor();
			break;
		case Qt::Key_A :
			toggleAnimationState();
			break;
		case Qt::Key_C :
			{
				QString dirPath;

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
				dirPath = QFileDialog::getExistingDirectory(
					this, tr( "Choose Directory to Save" ), m_CurrentDir,
					QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog );
#else
				dirPath = QFileDialog::getExistingDirectory(
					this, tr( "Choose Directory to Save" ), m_CurrentDir,
					QFileDialog::ShowDirsOnly );
#endif
#else
#ifdef Q_OS_WIN
				dirPath = QFileDialog::getExistingDirectory(
					this, tr( "Choose Directory to Save" ), m_CurrentDir,
					QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog );
#else
				dirPath = QFileDialog::getExistingDirectory(
					this, tr( "Choose Directory to Save" ), m_CurrentDir,
					QFileDialog::ShowDirsOnly );
#endif
#endif

				if ( !dirPath.isEmpty() ) {
					m_CurrentDir = dirPath;
					emit getSnapShot( dirPath, param.getCurrentStep() );
				}
				break;
			}
		case Qt::Key_Escape :
			{
				QMessageBox::StandardButton reply
					= QMessageBox::question( this, tr( "Message" ),
						tr( "Quit Application?" ),
						QMessageBox::Yes | QMessageBox::No );
				if ( reply == QMessageBox::Yes ) {
					close();
				}
				break;
			}
		default:
			break;
	}
}

void MainWindow::openGradsFile( const QString &filename )
{
	QString msg;
	ErrorCode err = grads.openFile( filename );
	if ( err == GRADS_NO_ERROR ) {
		param.initialize();

		msg.prepend( "File opened : " + filename.toLocal8Bit() );
		statusBar()->showMessage( msg );

		if ( grads.getGridSizeT() > 1 ) {
			m_HSlider_TimeSlider->show();
			m_HSlider_TimeSlider->setMinimum( 0 );
			m_HSlider_TimeSlider->setMaximum( grads.getGridSizeT() - 1 );
			m_Button_StartAnimation->show();
		} else {
			m_HSlider_TimeSlider->hide();
			m_Button_StartAnimation->hide();
		}
//		param.setViewerSize( ui->widget_Viewer->size() );
		param.setViewerSize( m_Viewer->size() );

		m_ToolBar->setDisabled( false );

		m_Menu_Export->setDisabled( false );
		m_Action_File_Save->setDisabled( false );
		m_Action_File_Load->setDisabled( false );
		m_Action_File_SaveColorTable->setDisabled( false );
		m_Action_File_LoadColorTable->setDisabled( false );

		m_HSlider_TimeSlider->setValue( 0 );
		m_Button_StartAnimation->setIcon( QIcon( ":/resources/play.png" ) );
		m_IsAnimation = false;

		m_Action_ToolButton_DefaultView->setChecked( true );
		m_Action_ToolButton_Projection->setIcon( QIcon( ":/resources/perspective.png" ) );

//		emit openGradsFileSucceeded();
		m_Viewer->setGradsData();
//		ui->widget_Viewer->setGradsData();
//		ui->widget_ColorEditor->setDefault01();
//		ui->widget_PreferencesPanel->setGrads();
//		ui->widget_FileInfo->setGrads();
		m_ControlPanel->initialize();
	} else {
		msg  = "Could not open GrADS file.\n";
		msg += "ERROR: " + getErrString( err );
		QMessageBox::warning( this, tr( "Warning" ),
			msg, QMessageBox::Ok | QMessageBox::Cancel );
		return;
	}
}

void MainWindow::setCurrentTime( int time )
{
	if ( grads.setData( time ) == true ) {
//		emit setCurrentTimeSucceeded();
//		ui->widget_Viewer->setCurrentData();
		m_Viewer->setCurrentData();
	}
	param.setCurrentStep( m_HSlider_TimeSlider->value() );
}

void MainWindow::timerEvent()
{
	int currentStep = param.getCurrentStep();

	if ( m_IsAnimation ) {
		if ( currentStep > param.getTimeEnd() ) {
			currentStep = param.getTimeBegin();
		}
//		current_step_data_0 = ( int )fmod( current_time * FPS, TIMESTEP_DATA_0 );
//		m_Time->elapsed()
		param.setCurrentStep( currentStep + 1 );
		m_HSlider_TimeSlider->setValue( currentStep );
	}
	if ( currentStep < param.getTimeBegin() ) {
		param.setCurrentStep( param.getTimeBegin() );
		m_HSlider_TimeSlider->setValue( param.getTimeBegin() );
	} else if ( currentStep > param.getTimeEnd() ) {
		param.setCurrentStep( param.getTimeEnd() );
		m_HSlider_TimeSlider->setValue( param.getTimeEnd() );
	}
//	emit execTimerEvent( m_Time->elapsed() );
//	ui->widget_Viewer->animate( m_Time->elapsed() );
	m_Viewer->animate( m_Time->elapsed() );
}

void MainWindow::toggleAnimationState()
{
	m_IsAnimation = !m_IsAnimation;
	if ( m_IsAnimation ) {
		m_Button_StartAnimation->setIcon( QIcon( ":/resources/pause.png" ) );
	} else {
		m_Button_StartAnimation->setIcon( QIcon( ":/resources/play.png"  ) );
		m_Time->restart();
	}
}

void MainWindow::toggleViewMode()
{
	m_IsSphereView = !m_IsSphereView;
	if ( m_IsSphereView ) {
		m_ToolBar->setDisabled( true );
		statusBar()->showMessage( "Change Viewer Type to Spherical Mode." );
	} else {
		m_ToolBar->setDisabled( false );
		statusBar()->showMessage( "Change Viewer Type to Box Mode." );
	}
	emit changeViewMode( m_IsSphereView );
}

void MainWindow::toggleProjection()
{
	if ( param.getProjection() == ORTHO ) {
		m_Action_ToolButton_Projection->setIcon(
			QIcon( ":/resources/perspective.png" ) );
	} else {
		m_Action_ToolButton_Projection->setIcon(
			QIcon( ":/resources/ortho.png" ) );
	}
//	ui->widget_Viewer->toggleProjection();
	m_Viewer->toggleProjection();
}

void MainWindow::toggleStateControlPanel()
{
	m_IsVisibleControlPanel = !m_IsVisibleControlPanel;
	if ( m_IsVisibleControlPanel ) {
		m_ToolBar->show();
		m_ControlPanel->show();
	} else {
		if ( m_IsFullScreen ) {
			m_ToolBar->hide();
		}
		m_ControlPanel->hide();
	}

//	ui->widget_Viewer->hide(); // refresh viewer size
//	ui->widget_Viewer->show(); // refresh viewer size
//	param.setViewerSize( ui->widget_Viewer->size() );
	m_Viewer->hide(); // refresh viewer size
	m_Viewer->show(); // refresh viewer size
	param.setViewerSize( m_Viewer->size() );
}

void MainWindow::toggleConnectionTypeColorEditor()
{
/*
	if ( ui->widget_ColorEditor->getEditor()->getConnectionType()
		== GradientEditor::CurveConnection ) {
		ui->widget_ColorEditor->getEditor()->setConnectionType( GradientEditor::LineConnection );
	} else {
		ui->widget_ColorEditor->getEditor()->setConnectionType( GradientEditor::CurveConnection );
	}
*/
	m_ControlPanel->colorTablePanel()->getEditor()->toggleConnectionType();
}

void MainWindow::setObjectToCenter()
{
	param.setObjectToCenter();
}
/*
void MainWindow::updateColorTable()
{
	ui->widget_ColorEditor->getEditor()->getColorControlPointFromParam();
	ui->widget_ColorEditor->setColorTableToParam();
}
*/
void MainWindow::changeViewerSize()
{
//	QSize viewerSize  = ui->widget_Viewer->size();
	QSize viewerSize  = m_Viewer->size();
	QSize diffSize = param.getViewerSize() - viewerSize;
	setFixedSize( size() + diffSize );
}

void MainWindow::makeWidget()
{
	setupControlPanel();
	setupMenuBar();
	setupStatusBar();
	setupToolBar();
}

void MainWindow::setupControlPanel()
{
	m_ControlPanel = new ControlPanel();
//	ui->horizontalLayout_MainWindow->addWidget( m_ControlPanel );
//	connect( m_ControlPanel, SIGNAL( valueChanged( ValueType ) ),
//			 ui->widget_Viewer, SLOT( valueChanged( ValueType ) ) );
	connect( m_ControlPanel, SIGNAL( valueChanged( ValueType ) ),
			 m_Viewer, SLOT( valueChanged( ValueType ) ) );
}

void MainWindow::setupMenuBar()
{
	// "File" Menu
	m_Menu_File = new QMenu( tr( "File" ) );
	m_Action_File_Open = new QAction(  tr( "Open GrADS File" ), this );
	m_Action_File_Save = new QAction( tr( "Save Settings" ), this );
	m_Action_File_Load = new QAction( tr( "Load Settings" ), this );
	m_Action_File_SaveColorTable = new QAction( tr( "Save Color Table" ), this );
	m_Action_File_LoadColorTable = new QAction( tr( "Load Color Table" ), this );
	m_Action_File_Exit = new QAction( tr( "Exit" ), this );
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	m_Action_File_Open->setShortcut( QKeySequence( Qt::CTRL | Qt::Key_O ) );
	m_Action_File_Save->setShortcut( QKeySequence( Qt::CTRL | Qt::Key_S ) );
	m_Action_File_Load->setShortcut( QKeySequence( Qt::CTRL | Qt::Key_L ) );
	m_Action_File_SaveColorTable->setShortcut( QKeySequence( Qt::CTRL | Qt::SHIFT | Qt::Key_S ) );
	m_Action_File_LoadColorTable->setShortcut( QKeySequence( Qt::CTRL | Qt::SHIFT | Qt::Key_L ) );
	m_Action_File_Exit->setShortcut( QKeySequence( Qt::CTRL | Qt::Key_Escape ) );
#else
	m_Action_File_Open->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_O ) );
	m_Action_File_Save->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_S ) );
	m_Action_File_Load->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_L ) );
	m_Action_File_SaveColorTable->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_S ) );
	m_Action_File_LoadColorTable->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_L ) );
	m_Action_File_Exit->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_Escape ) );
#endif
	m_Menu_File->addAction( m_Action_File_Open );
	m_Menu_File->addSeparator();
	m_Menu_File->addAction( m_Action_File_Save );
	m_Menu_File->addAction( m_Action_File_Load );
	m_Menu_File->addAction( m_Action_File_SaveColorTable );
	m_Menu_File->addAction( m_Action_File_LoadColorTable );
	m_Menu_File->addSeparator();
	m_Menu_File->addAction( m_Action_File_Exit );
	m_Action_File_Save->setDisabled( true );
	m_Action_File_Load->setDisabled( true );
	m_Action_File_SaveColorTable->setDisabled( true );
	m_Action_File_LoadColorTable->setDisabled( true );
	connect( m_Action_File_Open, SIGNAL( triggered() ), this, SLOT ( openGrADS() ) );
	connect( m_Action_File_Save, SIGNAL( triggered() ), this, SLOT( saveSettings() ) );
	connect( m_Action_File_Load, SIGNAL( triggered() ), this, SLOT( loadSettings() ) );
	connect( m_Action_File_SaveColorTable, SIGNAL( triggered() ), this, SLOT( saveColorTable() ) );
	connect( m_Action_File_LoadColorTable, SIGNAL( triggered() ), this, SLOT( loadColorTable() ) );
	connect( m_Action_File_Exit, SIGNAL( triggered() ), this, SLOT( exitApp() ) );

	// "Export" Menu
	m_Menu_Export = new QMenu( tr( "Export" ) );
	m_Action_Export_KML = new QAction(  tr( "Google Earth (KML+COLLADA)" ), this );
	m_Action_Export_SequentialImages = new QAction(  tr( "Sequential Images" ), this );
	m_Action_Export_Movie = new QAction(  tr( "Movie" ), this );
	m_Action_Export_KML->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_E, Qt::CTRL + Qt::Key_K ) );
	m_Action_Export_SequentialImages->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_E, Qt::CTRL + Qt::Key_I ) );
	m_Action_Export_Movie->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_E, Qt::CTRL + Qt::Key_M ) );
	m_Action_Export_Movie->setDisabled( true );
	m_Menu_Export->addAction( m_Action_Export_KML );
	m_Menu_Export->addAction( m_Action_Export_SequentialImages );
	m_Menu_Export->addAction( m_Action_Export_Movie );
	m_Menu_Export->setDisabled( true );
	connect( m_Action_Export_KML, SIGNAL( triggered() ), this, SLOT( exportKML() ) );
	connect( m_Action_Export_SequentialImages, SIGNAL( triggered() ), this, SLOT( exportSequentialImages() ) );
	connect( m_Action_Export_Movie, SIGNAL( triggered() ), this, SLOT( exportMovie() ) );

	// "Settings" Menu
	m_Menu_Settings = new QMenu( tr( "Settings" ) );
	m_Action_Settings_ViewerSize = new QAction(  tr( "Viewer Size" ), this );
	m_Action_Settings_KMLImageSize = new QAction(  tr( "KML Image Size" ), this );
	m_Action_Settings_Font = new QAction(  tr( "Font" ), this );
	m_Menu_Settings->addAction( m_Action_Settings_ViewerSize );
	m_Menu_Settings->addAction( m_Action_Settings_KMLImageSize );
	m_Menu_Settings->addAction( m_Action_Settings_Font );
	connect( m_Action_Settings_ViewerSize, SIGNAL( triggered() ), this, SLOT( setViewerSize() ) );
	connect( m_Action_Settings_KMLImageSize, SIGNAL( triggered() ), this, SLOT( setKMLImageSize() ) );
	connect( m_Action_Settings_Font, SIGNAL( triggered() ), this, SLOT( setFont() ) );

	m_MenuBar = new QMenuBar;
	m_MenuBar->addMenu( m_Menu_File );
	m_MenuBar->addMenu( m_Menu_Export );
	m_MenuBar->addMenu( m_Menu_Settings );
	setMenuBar( m_MenuBar );
}

void MainWindow::setupStatusBar()
{
	m_StatusBar = new QStatusBar;
	setStatusBar( m_StatusBar );
}

void MainWindow::setupToolBar()
{
	m_ToolBar = new QToolBar;
	m_ToolBar->setMovable( false );

	QActionGroup *actionGroup;
	actionGroup = new QActionGroup( m_ToolBar );
	m_Action_ToolButton_DefaultView = new QAction(
		QIcon( ":/resources/default.png" ),
		"D", actionGroup );
	m_Action_ToolButton_FrontView = new QAction(
		QIcon( ":/resources/frontview.png" ),
		"F", actionGroup );
	m_Action_ToolButton_LeftSideView = new QAction(
		QIcon( ":/resources/leftsideview.png" ),
		"L", actionGroup );
	m_Action_ToolButton_BottomView = new QAction(
		QIcon( ":/resources/bottomview.png" ),
		"B", actionGroup );
	m_Action_ToolButton_Projection = new QAction(
		QIcon( ":/resources/perspective.png" ),
		"Projection Mode", this );
	m_Action_ToolButton_Centering = new QAction(
		QIcon( ":/resources/centering.png" ),
		"Centering", this );

	m_Action_ToolButton_DefaultView->setToolTip( "Default View" );
	m_Action_ToolButton_FrontView->setToolTip( "Front View" );
	m_Action_ToolButton_LeftSideView->setToolTip( "Left Side View" );
	m_Action_ToolButton_BottomView->setToolTip( "Bottom View" );
	m_Action_ToolButton_DefaultView->setCheckable( true );
	m_Action_ToolButton_FrontView->setCheckable( true );
	m_Action_ToolButton_LeftSideView->setCheckable( true );
	m_Action_ToolButton_BottomView->setCheckable( true );
	m_Action_ToolButton_Projection->setCheckable( false );
	m_Action_ToolButton_Centering->setCheckable( false );
	m_Action_ToolButton_DefaultView->setChecked( true );
	actionGroup->setExclusive( true );
	m_ToolBar->addAction( m_Action_ToolButton_DefaultView );
	m_ToolBar->addAction( m_Action_ToolButton_FrontView );
	m_ToolBar->addAction( m_Action_ToolButton_LeftSideView );
	m_ToolBar->addAction( m_Action_ToolButton_BottomView );
	m_ToolBar->addSeparator();
	m_ToolBar->addAction( m_Action_ToolButton_Projection );
	m_ToolBar->addSeparator();
	m_ToolBar->addAction( m_Action_ToolButton_Centering );

	addToolBar( m_ToolBar );

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
/*
	connect( m_Action_ToolButton_DefaultView, SIGNAL( activated() ),
		ui->widget_Viewer, SLOT( setViewDirectionDefault() ) );
	connect( m_Action_ToolButton_FrontView, SIGNAL( activated() ),
		ui->widget_Viewer, SLOT( setViewDirectionFront() ) );
	connect( m_Action_ToolButton_LeftSideView, SIGNAL( activated() ),
		ui->widget_Viewer, SLOT( setViewDirectionLeft() ) );
	connect( m_Action_ToolButton_BottomView, SIGNAL( activated() ),
		ui->widget_Viewer, SLOT( setViewDirectionBottom() ) );
*/
	connect( m_Action_ToolButton_DefaultView, SIGNAL( activated() ),
		m_Viewer, SLOT( setViewDirectionDefault() ) );
	connect( m_Action_ToolButton_FrontView, SIGNAL( activated() ),
		m_Viewer, SLOT( setViewDirectionFront() ) );
	connect( m_Action_ToolButton_LeftSideView, SIGNAL( activated() ),
		m_Viewer, SLOT( setViewDirectionLeft() ) );
	connect( m_Action_ToolButton_BottomView, SIGNAL( activated() ),
		m_Viewer, SLOT( setViewDirectionBottom() ) );
#else
/*
	connect( m_Action_ToolButton_DefaultView, SIGNAL( triggered() ),
		ui->widget_Viewer, SLOT( setViewDirectionDefault() ) );
	connect( m_Action_ToolButton_FrontView, SIGNAL( triggered() ),
		ui->widget_Viewer, SLOT( setViewDirectionFront() ) );
	connect( m_Action_ToolButton_LeftSideView, SIGNAL( triggered() ),
		ui->widget_Viewer, SLOT( setViewDirectionLeft() ) );
	connect( m_Action_ToolButton_BottomView, SIGNAL( triggered() ),
		ui->widget_Viewer, SLOT( setViewDirectionBottom() ) );
*/
	connect( m_Action_ToolButton_DefaultView, SIGNAL( triggered() ),
		m_Viewer, SLOT( setViewDirectionDefault() ) );
	connect( m_Action_ToolButton_FrontView, SIGNAL( triggered() ),
		m_Viewer, SLOT( setViewDirectionFront() ) );
	connect( m_Action_ToolButton_LeftSideView, SIGNAL( triggered() ),
		m_Viewer, SLOT( setViewDirectionLeft() ) );
	connect( m_Action_ToolButton_BottomView, SIGNAL( triggered() ),
		m_Viewer, SLOT( setViewDirectionBottom() ) );
#endif

	connect( m_Action_ToolButton_Projection, SIGNAL( triggered() ),
		this, SLOT( toggleProjection() ) );
	connect( m_Action_ToolButton_Centering, SIGNAL( triggered() ),
		this, SLOT( setObjectToCenter() ) );

	m_ToolBar->setDisabled( true );
}

void MainWindow::openGrADS()
{
	// Open New GrADS Data Descriptor File
	QString filename;

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
	filename = QFileDialog::getOpenFileName(
			this, "Open File",
			m_CurrentDir, "Grads Data Descriptor File (*.ctl)",
			0, QFileDialog::DontUseNativeDialog );
#else
	filename = QFileDialog::getOpenFileName(
			this, "Open File",
			m_CurrentDir, "Grads Data Descriptor File (*.ctl)" );
#endif
#else
#ifdef Q_OS_WIN
	filename = QFileDialog::getOpenFileName(
			this, "Open File",
			m_CurrentDir, "Grads Data Descriptor File (*.ctl)",
			0, QFileDialog::DontUseNativeDialog );
#else
	filename = QFileDialog::getOpenFileName(
			this, "Open File",
			m_CurrentDir, "Grads Data Descriptor File (*.ctl)" );
#endif
#endif

	if ( !filename.isEmpty() ) {
		m_CurrentDir = QFileInfo( filename ).path();
		openGradsFile( filename );
		QString str = QString( tr( "VDVGE - " ) );
		str += QFileInfo( grads.getCtlFileName() ).fileName();
		setWindowTitle( str );
	}
}

void MainWindow::saveSettings()
{
	QString filename;

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
	filename = QFileDialog::getSaveFileName(
			this, "Save File", m_CurrentDir,
			"VDVGE Save File (*.save);;All Files(*)",
			0, QFileDialog::DontUseNativeDialog );
#else
	filename = QFileDialog::getSaveFileName(
			this, "Save File", m_CurrentDir,
			"VDVGE Save File (*.save);;All Files(*)" );
#endif
#else
#ifdef Q_OS_WIN
	filename = QFileDialog::getSaveFileName(
			this, "Save File", m_CurrentDir,
			"VDVGE Save File (*.save);;All Files(*)",
			0, QFileDialog::DontUseNativeDialog );
#else
	filename = QFileDialog::getSaveFileName(
			this, "Save File", m_CurrentDir,
			"VDVGE Save File (*.save);;All Files(*)" );
#endif
#endif

	if ( !filename.isEmpty() ) {
		QString suffix = QFileInfo( filename ).suffix();
		if ( suffix != "save" ) {
			filename = filename + ".save";
		}
		m_CurrentDir = QFileInfo( filename ).path();
		QApplication::setOverrideCursor( Qt::WaitCursor );
//		ui->widget_ColorEditor->getEditor()->setColorControlPointToParam();
		m_ControlPanel->colorTablePanel()->getEditor()->setColorControlPointToParam();
		param.save( filename );
		QString msg;
		msg.prepend( "Saved Parameters to : " + filename.toLocal8Bit() );
		statusBar()->showMessage( msg );
		QApplication::restoreOverrideCursor();
	}
}

void MainWindow::loadSettings()
{
	QString filename;

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
	filename = QFileDialog::getOpenFileName(
			this, "Load File", m_CurrentDir,
			"VDVGE Save File (*.save);;All Files(*)",
			0, QFileDialog::DontUseNativeDialog );
#else
	filename = QFileDialog::getOpenFileName(
			this, "Load File", m_CurrentDir,
			"VDVGE Save File (*.save);;All Files(*)" );
#endif
#else
#ifdef Q_OS_WIN
	filename = QFileDialog::getOpenFileName(
			this, "Load File", m_CurrentDir,
			"VDVGE Save File (*.save);;All Files(*)",
			0, QFileDialog::DontUseNativeDialog );
#else
	filename = QFileDialog::getOpenFileName(
			this, "Load File", m_CurrentDir,
			"VDVGE Save File (*.save);;All Files(*)" );
#endif
#endif

	if ( !filename.isEmpty() ) {
		QString msg;
		m_CurrentDir = QFileInfo( filename ).path();
		QApplication::setOverrideCursor( Qt::WaitCursor );
		ErrorCode err = param.load( filename );
		if ( err == PARAM_NO_ERROR ) {
//			emit openGradsFileSucceeded();
			m_Viewer->setGradsData();
//			ui->widget_Viewer->setGradsData();
//			ui->widget_ColorEditor->setDefault01();
//			ui->widget_PreferencesPanel->setGrads();
//			ui->widget_FileInfo->setGrads();
//			ui->widget_ColorEditor->getEditor()->getColorControlPointFromParam();
			m_ControlPanel->initialize();
			m_ControlPanel->colorTablePanel()->getEditor()->getColorControlPointFromParam();
			msg.prepend( "Loaded Parameters from : " + filename.toLocal8Bit() );
			statusBar()->showMessage( msg );
			QApplication::restoreOverrideCursor();
		} else {
			QApplication::restoreOverrideCursor();
			msg.prepend( "Failed to Load Parameters from : " + filename.toLocal8Bit() );
			statusBar()->showMessage( msg );
			msg  = "Syntax Error!\n";
			msg += "ERROR: " + getErrString( err );
			QMessageBox::warning( this, tr( "Warning" ),
				msg, QMessageBox::Ok );
		}
	}
}

void MainWindow::saveColorTable()
{
	QString filename;

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
	filename = QFileDialog::getSaveFileName(
			this, "Save Color Table", m_CurrentDir,
			"VDVGE Color Table File (*.colortable);;All Files(*)",
			0, QFileDialog::DontUseNativeDialog );
#else
	filename = QFileDialog::getSaveFileName(
			this, "Save Color Table", m_CurrentDir,
			"VDVGE Color Table File (*.colortable);;All Files(*)" );
#endif
#else
#ifdef Q_OS_WIN
	filename = QFileDialog::getSaveFileName(
			this, "Save Color Table", m_CurrentDir,
			"VDVGE Color Table File (*.colortable);;All Files(*)",
			0, QFileDialog::DontUseNativeDialog );
#else
	filename = QFileDialog::getSaveFileName(
			this, "Save Color Table", m_CurrentDir,
			"VDVGE Color Table File (*.colortable);;All Files(*)" );
#endif
#endif

	if ( !filename.isEmpty() ) {
		QString suffix = QFileInfo( filename ).suffix();
		if ( suffix != "colortable" ) {
			filename = filename + ".colortable";
		}
		m_CurrentDir = QFileInfo( filename ).path();
		QApplication::setOverrideCursor( Qt::WaitCursor );
//		ui->widget_ColorEditor->getEditor()->setColorControlPointToParam();
		m_ControlPanel->colorTablePanel()->getEditor()->setColorControlPointToParam();
		param.saveColorTable( filename );
		QString msg;
		msg.prepend( "Saved ColorTable to : " + filename.toLocal8Bit() );
		statusBar()->showMessage( msg );
		QApplication::restoreOverrideCursor();
	}
}

void MainWindow::loadColorTable()
{
	QString filename;

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
	filename = QFileDialog::getOpenFileName(
			this, "Load Color Table", m_CurrentDir,
			"VDVGE Color Table File (*.colortable);;All Files(*)",
			0, QFileDialog::DontUseNativeDialog );
#else
	filename = QFileDialog::getOpenFileName(
			this, "Load Color Table", m_CurrentDir,
			"VDVGE Color Table File (*.colortable);;All Files(*)" );
#endif
#else
#ifdef Q_OS_WIN
	filename = QFileDialog::getOpenFileName(
			this, "Load Color Table", m_CurrentDir,
			"VDVGE Color Table File (*.colortable);;All Files(*)",
			0, QFileDialog::DontUseNativeDialog );
#else
	filename = QFileDialog::getOpenFileName(
			this, "Load Color Table", m_CurrentDir,
			"VDVGE Color Table File (*.colortable);;All Files(*)" );
#endif
#endif

	if ( !filename.isEmpty() ) {
		QString msg;
		m_CurrentDir = QFileInfo( filename ).path();
		QApplication::setOverrideCursor( Qt::WaitCursor );
		ErrorCode err = param.loadColorTable( filename );
		if ( err == PARAM_COLORTABLE_NO_ERROR ) {
//			emit openGradsFileSucceeded();
			m_Viewer->setGradsData();
//			ui->widget_Viewer->setGradsData();
//			ui->widget_ColorEditor->setDefault01();
//			ui->widget_PreferencesPanel->setGrads();
//			ui->widget_FileInfo->setGrads();
//			ui->widget_ColorEditor->getEditor()->getColorControlPointFromParam();
			m_ControlPanel->initialize();
			m_ControlPanel->colorTablePanel()->getEditor()->getColorControlPointFromParam();
			msg.prepend( "Loaded ColorTable from : " + filename.toLocal8Bit() );
			statusBar()->showMessage( msg );
			QApplication::restoreOverrideCursor();
		} else {
			QApplication::restoreOverrideCursor();
			msg.prepend( "Failed to Load ColorTable from : " + filename.toLocal8Bit() );
			statusBar()->showMessage( msg );
			msg  = "Syntax Error!\n";
			msg += "ERROR: " + getErrString( err );
			QMessageBox::warning( this, tr( "Warning" ),
				msg, QMessageBox::Ok );
		}
	}
}

void MainWindow::exitApp()
{
	QMessageBox::StandardButton reply
		= QMessageBox::question( this, tr( "Message" ),
			tr( "Quit Application?" ),
			QMessageBox::Yes | QMessageBox::No );
	if ( reply == QMessageBox::Yes ) {
		close();
	}
}

void MainWindow::exportKML()
{
//	ui->widget_Viewer->exportKML();
	m_Viewer->exportKML();
}

void MainWindow::exportSequentialImages()
{
	QString dirPath;

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
	dirPath = QFileDialog::getExistingDirectory(
		this, tr( "Choose Directory to Save" ), m_CurrentDir,
		QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog );
#else
	dirPath = QFileDialog::getExistingDirectory(
		this, tr( "Choose Directory to Save" ), m_CurrentDir,
		QFileDialog::ShowDirsOnly );
#endif
#else
#ifdef Q_OS_WIN
	dirPath = QFileDialog::getExistingDirectory(
		this, tr( "Choose Directory to Save" ), m_CurrentDir,
		QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog );
#else
	dirPath = QFileDialog::getExistingDirectory(
		this, tr( "Choose Directory to Save" ), m_CurrentDir,
		QFileDialog::ShowDirsOnly );
#endif
#endif

	if ( !dirPath.isEmpty() ) {
		m_CurrentDir = dirPath;
		statusBar()->showMessage( "Exporting Sequential Images....." );
		QApplication::setOverrideCursor( Qt::WaitCursor );
		int currentStep = param.getCurrentStep();
		for ( int t = param.getTimeBegin(); t <= param.getTimeEnd(); t++ ) {
			param.setCurrentStep( t );
			QApplication::processEvents( QEventLoop::ExcludeUserInputEvents );
			m_HSlider_TimeSlider->setValue( t );
			QApplication::processEvents( QEventLoop::ExcludeUserInputEvents );
			emit getSnapShot( dirPath, t );
		}
		param.setCurrentStep( currentStep );
		m_HSlider_TimeSlider->setValue( currentStep );
		QApplication::restoreOverrideCursor();
		statusBar()->showMessage( "Finished to Export Sequential Images." );
	}
}

void MainWindow::exportMovie()
{
	QString savefile;

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
	savefile = QFileDialog::getSaveFileName(
			this, "Save as Movie File", m_CurrentDir,
			"Windows Media Video 8 (*.wmv);;All Files(*)",
			0, QFileDialog::DontUseNativeDialog );
#else
	savefile = QFileDialog::getSaveFileName(
			this, "Save as Movie File", m_CurrentDir,
			"Windows Media Video 8 (*.wmv);;All Files(*)" );
#endif
#else
#ifdef Q_OS_WIN
	savefile = QFileDialog::getSaveFileName(
			this, "Save as Movie File", m_CurrentDir,
			"Windows Media Video 8 (*.wmv);;All Files(*)",
			0, QFileDialog::DontUseNativeDialog );
#else
	savefile = QFileDialog::getSaveFileName(
			this, "Save as Movie File", m_CurrentDir,
			"Windows Media Video 8 (*.wmv);;All Files(*)" );
#endif
#endif

	if ( !savefile.isEmpty() ) {
		m_CurrentDir = QFileInfo( savefile ).path();
		statusBar()->showMessage( "Exporting Movie File....." );

		QDir tmpDir;
		tmpDir.setPath( QFileInfo( savefile ).path() );
		if ( !tmpDir.exists( "_tmpVDVGE" ) ) {
			tmpDir.mkdir( "_tmpVDVGE" );
		}
		tmpDir.cd( "_tmpVDVGE" );

		// generate sequential images
		QApplication::setOverrideCursor( Qt::WaitCursor );
		int currentStep = param.getCurrentStep();
		for ( int t = param.getTimeBegin(); t <= param.getTimeEnd(); t++ ) {
			QApplication::processEvents( QEventLoop::ExcludeUserInputEvents );
			param.setCurrentStep( t );
			m_HSlider_TimeSlider->setValue( t );
			emit getSnapShot( tmpDir.path(), t - param.getTimeBegin() );
		}

		// output movie
		QString filename = QFileInfo( savefile ).fileName();
		QString suffix   = QFileInfo( savefile ).suffix();
		if ( suffix != "wmv" ) {
			filename = filename + ".wmv";
		}
		QString command1 = QString( "cd \"" + tmpDir.path() + "\"");
		QString command2;

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
		QString appPath = QCoreApplication::applicationDirPath();
		command2 = QString( "\"" + appPath + "/ffmpeg.exe\" -y -r 12 -i " );
		command2 += QString( "./snapshot_%04d.png ../" );
		command2 += filename;
#else
		command2 = QString( "ffmpeg -y -r 12 -i ./snapshot_%04d.png ../" );
		command2 += filename;
#endif
#else
#ifdef Q_OS_WIN
		QString appPath = QCoreApplication::applicationDirPath();
		command2 = QString( "\"" + appPath + "/ffmpeg.exe\" -y -r 12 -i " );
		command2 += QString( "./snapshot_%04d.png ../" );
		command2 += filename;
#else
		command2 = QString( "ffmpeg -y -r 12 -i ./snapshot_%04d.png ../" );
		command2 += filename;
#endif
#endif

		QString command  = command1 + " && " + command2;
		system( command.toLocal8Bit().constData() );

		QStringList filters;
		filters << "snapshot_*.png";
		tmpDir.setNameFilters( filters );
		QStringList filtered_files = tmpDir.entryList( QDir::Files, QDir::Name );
		QList<QString>::iterator f;
		for ( f = filtered_files.begin(); f != filtered_files.end(); ++f ) {
			QFile tempFile( tmpDir.path() + "/" + (*f) );
			tmpDir.remove( tempFile.fileName() );
		}
		tmpDir.cdUp();
		tmpDir.rmdir( "_tmpVDVGE" );

		param.setCurrentStep( currentStep );
		m_HSlider_TimeSlider->setValue( currentStep );

		statusBar()->showMessage( "Finished to Export Movie File." );

		QApplication::restoreOverrideCursor();
	}
}

void MainWindow::setViewerSize()
{
	m_ViewerControl->popup();
}

void MainWindow::setKMLImageSize()
{
	m_KMLControl->popup();
}

void MainWindow::setFont()
{
	m_FontControl->popup();
}
