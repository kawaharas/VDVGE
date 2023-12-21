//
// Volume Data Visualizer for Google Earth
//
// app_parameter.cpp
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

#include "app_parameter.h"

AppParameter::AppParameter()
{
	m_StateNativeDialog = false;
	m_StateTopo = false;
	m_StateCoastline = false;
	m_MaxTex3D_W = 128;
	m_MaxTex3D_H = 128;
	m_MaxTex3D_D =  64;
	m_FontColorBarString.setFamily( "helvetica" );
	m_FontColorBarString.setPixelSize( 12 );
	m_FontColorBarString.setBold( false );
	m_FontColorBarString.setItalic( false );
	m_FontColorBarValue.setFamily( "helvetica" );
	m_FontColorBarValue.setPixelSize( 12 );
	m_FontColorBarValue.setBold( false );
	m_FontColorBarValue.setItalic( false );
	m_FontTimeDisplay.setFamily( "helvetica" );
	m_FontTimeDisplay.setPixelSize( 12 );
	m_FontTimeDisplay.setBold( false );
	m_FontTimeDisplay.setItalic( false );
}

void AppParameter::initialize()
{
	m_StateGUI = true;

	m_StateNativeDialog = false;
	m_StateTopo = false;
	m_StateCoastline = false;
	m_MaxTex3D_W = 128;
	m_MaxTex3D_H = 128;
	m_MaxTex3D_D =  64;
	m_FontColorBarString.setFamily( "helvetica" );
	m_FontColorBarString.setPixelSize( 12 );
	m_FontColorBarString.setBold( false );
	m_FontColorBarString.setItalic( false );
	m_FontColorBarValue.setFamily( "helvetica" );
	m_FontColorBarValue.setPixelSize( 12 );
	m_FontColorBarValue.setBold( false );
	m_FontColorBarValue.setItalic( false );
	m_FontTimeDisplay.setFamily( "helvetica" );
	m_FontTimeDisplay.setPixelSize( 12 );
	m_FontTimeDisplay.setBold( false );
	m_FontTimeDisplay.setItalic( false );
//	m_FontTimeDisplay.setHintingPreference(QFont::HintingPreference::PreferNoHinting);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//	m_FontColorBarString.setFamily( "Courier New" );
//	m_FontColorBarValue.setFamily( "Courier New" );
//	m_FontTimeDisplay.setFamily( "Courier New" );
//	m_FontTimeDisplay.setStyleStrategy( QFont::NoAntialias );
//	m_FontTimeDisplay.setStyleStrategy( QFont::NoSubpixelAntialias );
//	m_FontTimeDisplay.setStyleStrategy( QFont::PreferAntialias );
#endif
}

bool AppParameter::save()
{
	QString iniFileName;

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
	iniFileName = QCoreApplication::applicationDirPath() + "/VDVGE.ini";
#else
	iniFileName = QDir::homePath() + "/.VDVGE";
#endif
#else
#ifdef Q_OS_WIN
	iniFileName = QCoreApplication::applicationDirPath() + "/VDVGE.ini";
#else
	iniFileName = QDir::homePath() + "/.VDVGE";
#endif
#endif

	QFile iniFile( iniFileName );
	iniFile.open( QIODevice::WriteOnly | QIODevice::Text );
	QTextStream stream( &iniFile );

	stream << "#\n";
	stream << "# Volume Data Visualizer for Google Earth\n";
	stream << "# Initialize File\n";
	stream << "#\n\n";

	stream << "MAX_TEX3D_W " << m_MaxTex3D_W;
	stream << "MAX_TEX3D_H " << m_MaxTex3D_H;
	stream << "MAX_TEX3D_D " << m_MaxTex3D_D << ENDL;

	if ( m_StateNativeDialog == false ) {
		stream << "USE_NATIVE_DIALOG 0" << ENDL;
	} else {
		stream << "USE_NATIVE_DIALOG 1" << ENDL;
	}

	iniFile.close();

	return true;
}

ErrorCode AppParameter::load()
{
	QString iniFileName;

#if QT_VERSION < 0x050000
#ifdef Q_WS_WIN
	iniFileName = QCoreApplication::applicationDirPath() + "/VDVGE.ini";
#else
	iniFileName = QDir::homePath() + "/.VDVGE";
#endif
#else
#ifdef Q_OS_WIN
	iniFileName = QCoreApplication::applicationDirPath() + "/VDVGE.ini";
#else
	iniFileName = QDir::homePath() + "/.VDVGE";
#endif
#endif

	QFile iniFile( iniFileName );
	if ( iniFile.exists() ) {
		iniFile.open( QIODevice::ReadOnly | QIODevice::Text );
		QTextStream stream( &iniFile );
		QString lineString;
		QString token1, token2;
		QStringList stringList;
		int stateNativeDialog, maxTex3D_W, maxTex3D_H, maxTex3D_D;
		bool ok;
		while ( !stream.atEnd() ) {
			lineString = stream.readLine();
			if ( lineString[0] != QChar( '#' ) ) {
				stringList = splitString( "\\s+", lineString );
				if ( stringList.size() >= 2 ) {
					token1 = ( stringList.at( 0 ) ).toUpper();
					token2 = ( stringList.at( 1 ) ).toUpper();
					if ( token1 == "MAX_TEX3D_W" ) {
						maxTex3D_W = token2.toInt( &ok );
						if ( ok ) {
							int i = 0;
							while ( 1 ) {
								int power2 = static_cast< int >(
									pow( 2.0, static_cast< double >( i ) ) );
								if ( maxTex3D_W <= power2 ) {
									maxTex3D_W = power2;
									break;
								} else {
									i++;
								}
							}
							m_MaxTex3D_W = static_cast< GLsizei >( maxTex3D_W );
						} else {
							return APPPARAM_ERROR_MAXTEX3D_W;
						}
					} else if ( token1 == "MAX_TEX3D_H" ) {
						maxTex3D_H = token2.toInt( &ok );
						if ( ok ) {
							int i = 0;
							while ( 1 ) {
								int power2 = static_cast< int >(
									pow( 2.0, static_cast< double >( i ) ) );
								if ( maxTex3D_H <= power2 ) {
									maxTex3D_H = power2;
									break;
								} else {
									i++;
								}
							}
							m_MaxTex3D_H = static_cast< GLsizei >( maxTex3D_H );
						} else {
							return APPPARAM_ERROR_MAXTEX3D_H;
						}
					} else if ( token1 == "MAX_TEX3D_D" ) {
						maxTex3D_D = token2.toInt( &ok );
						if ( ok ) {
							int i = 0;
							while ( 1 ) {
								int power2 = static_cast< int >(
									pow( 2.0, static_cast< double >( i ) ) );
								if ( maxTex3D_D <= power2 ) {
									maxTex3D_D = power2;
									break;
								} else {
									i++;
								}
							}
							m_MaxTex3D_D = static_cast< GLsizei >( maxTex3D_D );
						} else {
							return APPPARAM_ERROR_MAXTEX3D_D;
						}
					} else if ( token1 == "USE_NATIVE_DIALOG" ) {
						stateNativeDialog = token2.toInt( &ok );
						if ( ok ) {
							if ( stateNativeDialog == 0 ) {
								m_StateNativeDialog = false;
							} else {
								m_StateNativeDialog = true;
							}
						} else {
							return APPPARAM_ERROR_USE_NATIVE_DIALOG;
						}
					} else {
						return APPPARAM_ERROR_SYNTAX;
					}
				}
			}
		}
		iniFile.close();
	} else {
		// create an initialize file if it is not exist
		iniFile.open( QIODevice::WriteOnly | QIODevice::Text );
		QTextStream stream( &iniFile );

		stream << "#\n";
		stream << "# Volume Data Visualizer for Google Earth\n";
		stream << "# Initialize File\n";
		stream << "#\n\n";

		stream << "MAX_TEX3D_W 128" << ENDL;
		stream << "MAX_TEX3D_H 128" << ENDL;
		stream << "MAX_TEX3D_D 64"  << ENDL << ENDL;

		stream << "USE_NATIVE_DIALOG 0" << ENDL;

		iniFile.close();
	}

	return APPPARAM_NO_ERROR;
}
