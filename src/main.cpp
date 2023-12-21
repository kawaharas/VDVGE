//
// Volume Data Visualizer for Google Earth
//
// main.cpp
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

#include <qglobal.h>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtGui/QApplication>
#else
#include <QApplication>
#endif
#include "mainwindow.h"
#include "colortable.h"
#include "common.h"
#include "kml.h"
#include <iostream>
#include <cstdio>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QTextCodec>
#endif

extern Grads grads;
extern Parameter param;
extern AppParameter appParam;

enum OptionType {
	GRADS_CTL,
	VDVGE_SAVE,
	VDVGE_CTABLE,
	NONE
};

struct Option {
	QString optString;
	OptionType type;
};

int main( int argc, char *argv[] )
{
#ifndef __APPLE__
	if ( argc == 1 ) {
#endif
		//////////////////////////////////////////////////////////////////////
		// Window Mode
		//////////////////////////////////////////////////////////////////////

		qRegisterMetaType<FloatVector>( "FloatVector" );

		QApplication application( argc, argv );
		QApplication::setStyle( "plastique" );
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
//		QApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
#endif
		// Specifying Character Encoding
		// ( for Path and File Name includes Double-Byte Character Set )
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
		QTextCodec::setCodecForTr( QTextCodec::codecForLocale() );
		QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale() );
#else
//		QTextCodec::setCodecForLocale( QTextCodec::codecForLocale() );
		QTextCodec::setCodecForLocale( QTextCodec::codecForName( "UTF-8" ) );
#endif

#ifndef __APPLE__
//		FreeConsole(); // hide console window with GUI mode
#endif

		MainWindow window;
		window.show();

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
		param.setMaximumViewerSize( application.primaryScreen()->geometry().size() );
#else
		param.setMaximumViewerSize( application.desktop()->size() );
#endif

		return application.exec();

#ifndef __APPLE__
	} else {

		//////////////////////////////////////////////////////////////////////
		// Commandline Execution Mode
		//////////////////////////////////////////////////////////////////////

//		QCoreApplication application( argc, argv );
		QApplication application( argc, argv, false ); // third element "false" means disable GUI

		// Specifying Character Encoding
		// ( for Path and File Name includes Double-Byte Character Set )
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
		QTextCodec::setCodecForTr( QTextCodec::codecForLocale() );
		QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale() );
#else
//		QTextCodec::setCodecForLocale( QTextCodec::codecForLocale() );
		QTextCodec::setCodecForLocale( QTextCodec::codecForName( "UTF-8" ) );
#endif

		struct Option option[3];
		for ( int i = 0; i < 3; i++ ) {
			option[i].optString = QString( argv[i+1] );
			option[i].type = NONE;
		}

		int option_num = argc - 1;
		if ( option_num > 4 ) {
			option_num = 3;
		}

		QTextStream stream( stdout );
		stream << ENDL;
		stream << "===== Batch Proccessing Mode =====" << ENDL;
		for ( int i = 0; i < option_num; i++ ) {
			QString suffix = QFileInfo( option[i].optString ).suffix();
			if ( i == 0 ) {
				if ( suffix == "ctl" ) {
					stream << "GrADS Ctl File : " << option[i].optString << ENDL;
				} else {
					stream << "Usage: VDVGE.exe grads_ctl_file (vdvge_save) (vdvge_colortable)" << ENDL;
					stream << "Aborted." << ENDL;
					return -1;
				}
			} else {
				if ( suffix == "save" ) {
					option[i].type = VDVGE_SAVE;
					stream << "VDVGE Save File: " << option[i].optString << ENDL;
				} else if ( suffix == "colortable" ) {
					option[i].type = VDVGE_CTABLE;
					stream << "ColorTable File: " << option[i].optString << ENDL;
				} else {
					stream << "Usage: VDVGE.exe grads_ctl_file (vdvge_save) (vdvge_colortable)" << ENDL;
					stream << "Aborted." << ENDL;
					return -1;
				}
			}
		}

		appParam.initialize();
		appParam.setStateGUI( false ); // disable GUI

		ErrorCode err;
		err = grads.openFile( option[0].optString );
		if ( err != GRADS_NO_ERROR ) {
			stream << "ERROR: " << getErrString( err ) << ENDL;
			return -1;
		}
		param.initialize();
		for ( int i = 1; i < option_num; i++ ) {
			if ( option[i].type == VDVGE_SAVE ) {
				err = param.load( option[i].optString );
				if ( err != PARAM_NO_ERROR ) {
					stream << "ERROR: " << getErrString( err ) << ENDL;
					return -1;
				}
			} else if ( option[i].type == VDVGE_CTABLE ) {
				err = param.loadColorTable( option[i].optString );
				if ( err != PARAM_COLORTABLE_NO_ERROR ) {
					stream << "ERROR: " << getErrString( err ) << ENDL;
					return -1;
				}
			}
		}

		param.set256Table();

		stream << ENDL;
		stream << "Exporting Files....." << ENDL;

		KML *kml = new KML();
		kml->disableGUI();
		kml->exportKML();

		delete kml;

		stream << "Finished." << ENDL;
	}
#endif

	return 0;
}
