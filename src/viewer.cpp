//
// Volume Data Visualizer for Google Earth
//
// viewer.cpp
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

#include "viewer.h"

Texture3D    texture3d;

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
PFNGLTEXIMAGE3DPROC glTexImage3D;
#endif
#else
#ifdef Q_OS_WIN
PFNGLTEXIMAGE3DPROC glTexImage3D;
#endif
#endif

Viewer::Viewer( QWidget *parent ) :
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	QOpenGLWidget( parent )
#else
	QGLWidget( parent )
#endif
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CompatibilityProfile);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	format.setVersion(2, 1);
	format.setSamples(4);
	format.setDepthBufferSize(24);
	setFormat( format );
	setAutoFillBackground( false );
//	setAutoBufferSwap( true );
#else
	setFormat( QGL::Rgba | QGL::DoubleBuffer | QGL::DepthBuffer );
	setAutoFillBackground( false );
	setAutoBufferSwap( true );
#endif

	m_IsDataLoaded   = false;
	m_IsAutoRotation = false;
	m_IsSphereView   = false;

	m_LightPosition[0] = 0.0;
	m_LightPosition[1] = 0.0;
	m_LightPosition[2] = 10.0;
	m_LightPosition[3] = 1.0;

	m_CurrentAngle = 0.0;

	m_ScaleX = 1.0;
	m_ScaleY = 1.0;
	m_ScaleZ = 1.0;

	m_ViewDirection = VIEW_DEFAULT;
	m_OrthoScale = 1.0;
}

Viewer::~Viewer()
{
}

void Viewer::initializeGL()
{
#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
	glTexImage3D =
		( PFNGLTEXIMAGE3DPROC )wglGetProcAddress( "glTexImage3D" );
#endif
#else
#ifdef Q_OS_WIN
	glTexImage3D =
		( PFNGLTEXIMAGE3DPROC )wglGetProcAddress( "glTexImage3D" );
#endif
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	m_GLFunc = QOpenGLContext::currentContext()->functions();
	m_GLFunc->glClearColor(
		param.getBGColor().redF(),
		param.getBGColor().greenF(),
		param.getBGColor().blueF(), 1.0f);
#else
	qglClearColor( param.getBGColor() );
#endif

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_NORMALIZE );
	glEnable( GL_SMOOTH );

	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    m_Earth = new Earth;
	m_Logo  = new Logo( this );
    m_ColorBar = new ColorBar( this );
	m_BoundingBox = new BoundingBox( this );
	m_TimeDisplay = new TimeDisplay( this );
	m_ColorSlice = new ColorSlice;
	m_ContourLine = new ContourLine;
	m_Topo = new Topo;
	m_Coastline = new Coastline;
}

void Viewer::resizeGL( int w, int h )
{
	setupViewport( w, h );
}

void Viewer::setupViewport( int w, int h )
{
	glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	GLdouble vpWidth  = static_cast< GLdouble >( w ); // Width of Viewport
	GLdouble vpHeight = static_cast< GLdouble >( h ); // Height of Viewport
	if ( param.getProjection() == PERSPECTIVE ) {
		gluPerspective( 60.0, vpWidth / vpHeight, 0.01, 100.0 );
	} else {
		if ( w >= h ) {

			glOrtho( -vpWidth / vpHeight / 2.0 * 1.3,
					  vpWidth / vpHeight / 2.0 * 1.3,
					 -0.5 * 1.3, 0.5 * 1.3,
					 -100.0, 100.0 );
		} else {

			glOrtho( -0.5 * 1.3, 0.5 * 1.3,
					 -vpHeight / vpWidth / 2.0 * 1.3,
					  vpHeight / vpWidth / 2.0 * 1.3,
					 -100.0, 100.0 );
		}
	}
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void Viewer::paintGL()
{
    makeCurrent();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	m_GLFunc->glClearColor(
		param.getBGColor().redF(),
		param.getBGColor().greenF(),
		param.getBGColor().blueF(), 1.0f);
#else
	qglClearColor( param.getBGColor() );
#endif
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glLightfv( GL_LIGHT0, GL_POSITION, m_LightPosition );

    GLfloat light0_diffuse[]  = { 1.0f, 1.0f, 1.0f, 0.0 };
    GLfloat light0_ambient[]  = { 0.3f, 0.3f, 0.3f, 0.0 };
    GLfloat light0_specular[] = { 1.0f, 1.0f, 1.0f, 0.0 };
    GLfloat light0_position[] = { 1.0f, 1.0f, 1.0f, 0.0 };

    GLfloat light1_diffuse[]  = { 1.0f, 1.0f, 1.0f, 0.0 };
    GLfloat light1_ambient[]  = { 0.3f, 0.3f, 0.3f, 0.0 };
    GLfloat light1_specular[] = { 1.0f, 1.0f, 1.0f, 0.0 };
//	GLfloat light1_position[] = { 0.0, 0.0, 1.0f, 0.0 };
    GLfloat light1_position[] = { -1.0f, 1.0f, -1.0f, 0.0 };

	glShadeModel( GL_SMOOTH );

	glLightfv( GL_LIGHT0, GL_AMBIENT,  light0_ambient );
	glLightfv( GL_LIGHT0, GL_SPECULAR, light0_specular );
	glLightfv( GL_LIGHT0, GL_DIFFUSE,  light0_diffuse  );
	glLightfv( GL_LIGHT0, GL_POSITION, light0_position );
	glLightfv( GL_LIGHT1, GL_AMBIENT,  light1_ambient  );
	glLightfv( GL_LIGHT1, GL_SPECULAR, light1_specular );
	glLightfv( GL_LIGHT1, GL_DIFFUSE,  light1_diffuse  );
	glLightfv( GL_LIGHT1, GL_POSITION, light1_position );
	glEnable( GL_LIGHT0 );
	glEnable( GL_LIGHT1 );

	gluLookAt( 0.0, 0.0, 2.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

	if ( m_IsDataLoaded ) {
		glPushMatrix();
		if ( !m_IsSphereView ) {
			backupViewParameters();
			glTranslatef( param.getObjectPosX(),
						  param.getObjectPosY(),
						  param.getObjectPosZ() );
			glRotatef( param.getObjectAnglePitch(), 1.0, 0.0, 0.0 );
			glRotatef( param.getObjectAngleYaw(),   0.0, 1.0, 0.0 );
			glRotatef( m_CurrentAngle, 0.0, 1.0, 0.0 ); // for auto rotation
			glRotatef( -90.0, 1.0, 0.0, 0.0 );

			glPushMatrix();
			if ( m_ScaleZ == 0.0 ) {
				if ( param.getZUnit() == ZUNIT_METER ) {
					glTranslatef( 0.0, 0.0,
						m_ScaleZ_SingleLayer * param.getZScale() );
				} else if ( param.getZUnit() == ZUNIT_KILOMETER ) {
					glTranslatef( 0.0, 0.0,
						m_ScaleZ_SingleLayer * param.getZScale() * 1000.0 );
				}
			}
			glPushMatrix();
				if ( param.getZUnit() == ZUNIT_METER ) {
					glScalef( m_ScaleX, m_ScaleY,
							  m_ScaleZ * param.getZScale() );
				} else if ( param.getZUnit() == ZUNIT_KILOMETER ) {
					glScalef( m_ScaleX, m_ScaleY,
							  m_ScaleZ * param.getZScale() * 1000.0 );
				}

				if ( param.getProjection() == ORTHO ) {
					glScalef( m_OrthoScale, m_OrthoScale, m_OrthoScale );
				}
				m_BoundingBox->draw( BACK_SIDE );
			glPopMatrix();
			glPopMatrix();

			glPushMatrix();
				float offset = 0.0;
				if ( grads.getGridZMax() != grads.getGridZMin() ) {
					offset = grads.getGridZMin() / ( grads.getGridZMax() - grads.getGridZMin() );
				}
				glScalef( m_ScaleX, m_ScaleY, 1.0 );
				if ( param.getProjection() == ORTHO ) {
					glScalef( m_OrthoScale, m_OrthoScale, m_OrthoScale );
				}
				if ( param.getZUnit() == ZUNIT_METER ) {
					if ( m_ScaleZ != 0.0 ) {
						glTranslatef( -0.5, -0.5,
							( -0.5 - offset ) * m_ScaleZ * param.getZScale() );
					} else {
						glTranslatef( -0.5, -0.5, 0.0 );
					}
				} else if ( param.getZUnit() == ZUNIT_KILOMETER ) {
					if ( m_ScaleZ != 0.0 ) {
						glTranslatef( -0.5, -0.5,
							( -0.5 - offset ) * m_ScaleZ * param.getZScale() * 1000.0 );
					} else {
						glTranslatef( -0.5, -0.5, 0.0 );
					}
				}
				m_Coastline->draw();
				m_Topo->draw();
			glPopMatrix();

			glPushMatrix();
			if ( m_ScaleZ == 0.0 ) {
				if ( param.getZUnit() == ZUNIT_METER ) {
					glTranslatef( 0.0, 0.0,
						m_ScaleZ_SingleLayer * param.getZScale() );
				} else if ( param.getZUnit() == ZUNIT_KILOMETER ) {
					glTranslatef( 0.0, 0.0,
						m_ScaleZ_SingleLayer * param.getZScale() * 1000.0 );
				}
			}
			glPushMatrix();
				if ( param.getZUnit() == ZUNIT_METER ) {
					glScalef( m_ScaleX, m_ScaleY,
							  m_ScaleZ * param.getZScale() );
				} else if ( param.getZUnit() == ZUNIT_KILOMETER ) {
					glScalef( m_ScaleX, m_ScaleY,
							  m_ScaleZ * param.getZScale() * 1000.0 );
				}
				if ( param.getProjection() == ORTHO ) {
					glScalef( m_OrthoScale, m_OrthoScale, m_OrthoScale );
				}
				m_ContourLine->draw();
				m_ColorSlice->draw();
				m_ContourLine->draw();
				m_VolumeRender->draw();
				m_BoundingBox->draw( FRONT_SIDE );
			glPopMatrix();
			glPopMatrix();
		} else {
			GLdouble clip[] = { 0.0, 0.0, 1.0, 0.0 };
			glClipPlane( GL_CLIP_PLANE0, clip );

			glTranslatef( param.getObjectPosSphereX(),
						  param.getObjectPosSphereY(),
						  param.getObjectPosSphereZ() );
			glRotatef( param.getObjectAngleSpherePitch(), 1.0, 0.0, 0.0 );
			glRotatef( param.getObjectAngleSphereYaw(),   0.0, 1.0, 0.0 );
			glRotatef( m_CurrentAngle, 0.0, 1.0, 0.0 ); // for auto rotation
			glRotatef( -90.0, 1.0, 0.0, 0.0 );

            m_Earth->draw();
			m_BoundingBox->drawSphereBoundingBox();
			m_ContourLine->drawSphereData();
			m_ColorSlice->drawSphereData();
			m_ContourLine->drawSphereData();
            m_VolumeRender->drawSphereData();
			m_BoundingBox->drawSphereBoundingBox();
		}
		glPopMatrix();

		m_ColorBar->draw();
		m_TimeDisplay->draw();
		m_Logo->draw();
	}
}

void Viewer::showEvent( QShowEvent *event )
{
	Q_UNUSED( event );
}

void Viewer::mousePressEvent( QMouseEvent *event )
{
	Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
	if ( modifiers & Qt::ShiftModifier ) {
		setCursor( Qt::ClosedHandCursor );
	}
	m_LastMousePosition = event->pos();
}

void Viewer::mouseReleaseEvent( QMouseEvent *event )
{
	if ( event->button() == Qt::LeftButton ) {
		setCursor( Qt::ArrowCursor );
	}
}

void Viewer::mouseDoubleClickEvent( QMouseEvent *event )
{
	if ( event->button() == Qt::LeftButton ) {
//		param.setBGColor( QColorDialog::getColor( param.getBGColor(), this ) ); // for Qt4?
		param.setBGColor( QColorDialog::getColor( param.getBGColor(), this, "Select Color", QColorDialog::DontUseNativeDialog ) );
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		m_GLFunc->glClearColor(
			param.getBGColor().redF(),
			param.getBGColor().greenF(),
			param.getBGColor().blueF(), 1.0f);
#else
		qglClearColor( param.getBGColor() );
#endif
	}
}

void Viewer::mouseMoveEvent( QMouseEvent *event )
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    int dx = event->position().x() - m_LastMousePosition.x();
    int dy = event->position().y() - m_LastMousePosition.y();
#else
    int dx = event->x() - m_LastMousePosition.x();
    int dy = event->y() - m_LastMousePosition.y();
#endif
    float objX, objY, objZ, objPitch, objYaw;
	if ( !m_IsSphereView ) {
		objX     = param.getObjectPosX();
		objY     = param.getObjectPosY();
		objZ     = param.getObjectPosZ();
		objPitch = param.getObjectAnglePitch();
		objYaw   = param.getObjectAngleYaw();
	} else {
		objX     = param.getObjectPosSphereX();
		objY     = param.getObjectPosSphereY();
		objZ     = param.getObjectPosSphereZ();
		objPitch = param.getObjectAngleSpherePitch();
		objYaw   = param.getObjectAngleSphereYaw();
	}

	Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
	if ( event->buttons() & Qt::LeftButton ) {
		if ( modifiers & Qt::ShiftModifier ) {
			objX += dx * 0.0025;
			objY -= dy * 0.0025;
		} else {
			objYaw   += dx * 0.5;
			objPitch += dy * 0.5;
		}
	} else if ( event->buttons() & Qt::RightButton ) {
		if ( param.getProjection() == PERSPECTIVE ) {
			objZ += dy * 0.015;
		} else {
			m_OrthoScale += dy * 0.005;
		}
	}

	if ( !m_IsSphereView ) {
		if ( m_ViewDirection == VIEW_DEFAULT ) {
			param.setObjectPos( objX, objY, objZ );
			param.setObjectAngle( objPitch, objYaw, 0.0 );
		} else {
			param.setObjectPos( 2, objZ );
		}
	} else {
		param.setObjectPosSphere( objX, objY, objZ );
		param.setObjectAngleSphere( objPitch, objYaw, 0.0 );
	}

    m_LastMousePosition = event->pos();
}

void Viewer::wheelEvent( QWheelEvent *event )
{
	float objZ, objPitch, objYaw;
	if ( !m_IsSphereView ) {
		objZ = param.getObjectPosZ();
		objPitch = param.getObjectAnglePitch();
		objYaw   = param.getObjectAngleYaw();
	} else {
		objZ = param.getObjectPosSphereZ();
		objPitch = param.getObjectAngleSpherePitch();
		objYaw   = param.getObjectAngleSphereYaw();
	}

	Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
	if ( modifiers & Qt::ShiftModifier ) {
		if ( m_ViewDirection == VIEW_DEFAULT ) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
			objPitch += static_cast< float >( event->angleDelta().y() ) * 0.05;
#else
			objPitch += static_cast< float >( event->delta() ) * 0.05;
#endif
		}
	} else if ( modifiers & Qt::ControlModifier ) {
		if ( m_ViewDirection == VIEW_DEFAULT ) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
			objYaw += static_cast< float >( event->angleDelta().y() ) * 0.05;
#else
			objYaw += static_cast< float >( event->delta() ) * 0.05;
#endif
		}
	} else {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
		objZ += static_cast< float >( event->angleDelta().y() ) / 120.0 * 0.05;
#else
		objZ += static_cast< float >( event->delta() ) / 120.0 * 0.05;
#endif
	}

	if ( !m_IsSphereView ) {
		param.setObjectPos( param.getObjectPosX(),
							param.getObjectPosY(), objZ );
		param.setObjectAngle( objPitch, objYaw, 0.0 );
	} else {
		param.setObjectPosSphere( param.getObjectPosSphereX(),
								  param.getObjectPosSphereY(), objZ );
		param.setObjectAngleSphere( objPitch, objYaw, 0.0 );
	}
}

void Viewer::animate( int time )
{
	if ( m_IsAutoRotation == true ) {
		m_CurrentAngle = 360.0 / 30.0 * fmod( time / 1000.0, 60.0 );
	}
	updateGL();
}

void Viewer::setColorTable()
{
	texture3d.setColorTable();
//	m_ContourLine->recalc();
	m_ContourLine->setLevels();

	updateGL();
}

void Viewer::setGradsData()
{
	m_IsDataLoaded = true;
	m_ViewDirection = VIEW_DEFAULT;
	m_OrthoScale = 1.0;

	m_ColorBar->initialize();

	param.setProjection( PERSPECTIVE );
	setupViewport( width(), height() );

	double distX = 2.0 * M_PI * EARTH_RADIUS_WE_WGS84
				  * cos( ( grads.getGridYMin() + ( grads.getGridYMax() - grads.getGridYMin() ) / 2.0 ) / 180.0 * M_PI )
				  / 360.0 * ( grads.getGridXMax() - grads.getGridXMin() );
	double distY = 2.0 * M_PI * EARTH_RADIUS_NS_WGS84 / 360.0 * ( grads.getGridYMax() - grads.getGridYMin() );
	double distZ = ( grads.getGridZMax() - grads.getGridZMin() );
	float maxDist = ( float )( std::max( std::max( distX, distY ), distZ ) );
	m_ScaleX = distX / maxDist;
	m_ScaleY = distY / maxDist;
	m_ScaleZ = distZ / maxDist;
	m_ScaleZ_SingleLayer = grads.getGridZMin() / maxDist;

	texture3d.initialize();
    setCurrentData();

//	m_Topo->setData();
	m_Topo->initialize();
	m_Coastline->setData();

	updateGL();
}

void Viewer::setCurrentData()
{
    texture3d.setData();
	texture3d.setColorTable();
	m_ColorBar->initialize();
	m_ContourLine->initialize();

	updateGL();
}

void Viewer::valueChanged( ValueType type )
{
	setFocus();

    if ( type == CHANGE_DATARANGE ) {
		setCurrentData();
	} else if ( type == CHANGE_COLORTABLE ) {
		setColorTable();
	} else if ( type == CHANGE_CONTOUR_LINE_PARAM ) {
		m_ContourLine->recalc();
	} else if ( type == CHANGE_LOGO_IMAGE ) {
		setLogoTexture();
//	} else if ( type == CHANGE_TOPO_STATE ) {
	} else if ( type == CHANGE_TOPO_DATASET ) {
		m_Topo->setData();
	} else if ( type == CHANGE_TOPO_SCALE ) {
		m_Topo->setData();
	} else if ( type == CHANGE_COASTLINE_FILE ) {
		m_Coastline->setData();
	} else {
		m_ColorBar->initialize();
	}

	updateGL();
}

void Viewer::setViewDirection( ViewDirection direction )
{
	switch ( direction ) {
		case VIEW_DEFAULT:
			m_ViewDirection = VIEW_DEFAULT;
			param.setViewDirection( VIEW_DEFAULT );
			restoreViewParameters();
			break;
		case VIEW_FRONT:
			m_ViewDirection = VIEW_FRONT;
			param.setViewDirection( VIEW_FRONT );
			param.setObjectAngle(  90.0,   0.0, 0.0 );
			break;
		case VIEW_REAR:
			m_ViewDirection = VIEW_REAR;
			param.setViewDirection( VIEW_REAR );
			param.setObjectAngle( -90.0, 180.0, 0.0 );
			break;
		case VIEW_TOP:
			m_ViewDirection = VIEW_TOP;
			param.setViewDirection( VIEW_TOP );
			param.setObjectAngle(   0.0, 180.0, 0.0 );
			break;
		case VIEW_BOTTOM:
			m_ViewDirection = VIEW_BOTTOM;
			param.setViewDirection( VIEW_BOTTOM );
			param.setObjectAngle(   0.0,   0.0, 0.0 );
			break;
		case VIEW_LEFT:
			m_ViewDirection = VIEW_LEFT;
			param.setViewDirection( VIEW_LEFT );
			param.setObjectAngle(   0.0,  90.0, 0.0 );
			break;
		case VIEW_RIGHT:
			m_ViewDirection = VIEW_RIGHT;
			param.setViewDirection( VIEW_RIGHT );
			param.setObjectAngle(   0.0, 270.0, 0.0 );
			break;
	}

	updateGL();
}

void Viewer::setViewDirectionDefault()
{
	m_ViewDirection = VIEW_DEFAULT;
	param.setViewDirection( VIEW_DEFAULT );
	restoreViewParameters();
	updateGL();
}

void Viewer::setViewDirectionFront()
{
	m_ViewDirection = VIEW_FRONT;
	param.setViewDirection( VIEW_FRONT );
	param.setObjectAngle(  90.0,   0.0, 0.0 );
	updateGL();
}

void Viewer::setViewDirectionRear()
{
	m_ViewDirection = VIEW_REAR;
	param.setViewDirection( VIEW_REAR );
	param.setObjectAngle( -90.0, 180.0, 0.0 );
	updateGL();
}

void Viewer::setViewDirectionTop()
{
	m_ViewDirection = VIEW_TOP;
	param.setViewDirection( VIEW_TOP );
	param.setObjectAngle(   0.0, 180.0, 0.0 );
	updateGL();
}

void Viewer::setViewDirectionBottom()
{
	m_ViewDirection = VIEW_BOTTOM;
	param.setViewDirection( VIEW_BOTTOM );
	param.setObjectAngle(   0.0,   0.0, 0.0 );
	updateGL();
}

void Viewer::setViewDirectionLeft()
{
	m_ViewDirection = VIEW_LEFT;
	param.setViewDirection( VIEW_LEFT );
	param.setObjectAngle(   0.0,  90.0, 0.0 );
	updateGL();
}

void Viewer::setViewDirectionRight()
{
	m_ViewDirection = VIEW_RIGHT;
	param.setViewDirection( VIEW_RIGHT );
	param.setObjectAngle(   0.0, 270.0, 0.0 );
	updateGL();
}

void Viewer::backupViewParameters()
{
	if ( m_ViewDirection == VIEW_DEFAULT ) {
		m_ObjectPosX_Backup = param.getObjectPosX();
		m_ObjectPosY_Backup = param.getObjectPosY();
		m_ObjectPosZ_Backup = param.getObjectPosZ();
		m_ObjectAnglePitch_Backup = param.getObjectAnglePitch();
		m_ObjectAngleYaw_Backup = param.getObjectAngleYaw();
	}
}

void Viewer::restoreViewParameters()
{
	param.setObjectPos( 0, m_ObjectPosX_Backup );
	param.setObjectPos( 1, m_ObjectPosY_Backup );
	param.setObjectAngle(
		m_ObjectAnglePitch_Backup, m_ObjectAngleYaw_Backup, 0.0 );
}

void Viewer::toggleProjection()
{
	if ( param.getProjection() == PERSPECTIVE ) {
		param.setProjection( ORTHO );
	} else {
		param.setProjection( PERSPECTIVE );
	}
	setupViewport( width(), height() );
}

void Viewer::setLogoTexture()
{
	m_Logo->readTexture( param.getLogo() );
}

bool Viewer::renderToPNG( QString dirPath, int time )
{
	glPixelTransferi( GL_MAP_COLOR, GL_FALSE );
	glReadBuffer( GL_FRONT );
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	QImage image = grabFramebuffer();
#else
	QImage image = grabFrameBuffer();
#endif
	QString saveFileName =
		QString( dirPath + "/snapshot_%1.png" ).arg( time, 4, 10, QChar( '0' ) );
	if ( !image.save( saveFileName ) ) {
		return false;
	}
	return true;
}

bool Viewer::exportKML()
{
	KML kml;
	if ( !( kml.exportKML() ) ) {
		return false;
	}
	return true;
}
