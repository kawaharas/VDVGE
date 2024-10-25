//
// Volume Data Visualizer for Google Earth
//
// grads.cpp
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

#include "grads.h"
#include <ctime>

extern AppParameter appParam;

Grads::Grads()
{
	m_IsDataLoaded = false;
}

Grads::~Grads()
{
	cleanupMemory();
}

ErrorCode Grads::openFile( const QString &filename )
{
	cleanupMemory();
/*
	if ( m_Data != NULL ) {
		delete [] m_Data;
	}
	if ( m_CoordX != NULL ) {
		delete [] m_CoordX;
	}
	if ( m_CoordX != NULL ) {
		delete [] m_CoordY;
	}
	if ( m_CoordX != NULL ) {
		delete [] m_CoordZ;
	}
*/
	m_IsYRev = false;
	m_IsZRev = false;
	m_Min =  1e32f;
	m_Max = -1e32f;
	m_FileHeader = 0;
	m_Undef = 0.0;
	m_VarInfo.clear();
	m_Option.clear();
	m_IsSequential = false;
/*
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	QTextCodec::setCodecForTr( QTextCodec::codecForLocale() );
	QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale() );
#else
//	QTextCodec::setCodecForLocale( QTextCodec::codecForLocale() );
	QTextCodec::setCodecForLocale( QTextCodec::codecForName( "UTF-8" ) );
#endif
*/
	// Open GrADS Control File
	m_FileNameCtl = filename;
	QFile file( filename );
	QFileInfo fileinfo( file );
	if ( fileinfo.suffix() != "ctl" ) {
		if ( appParam.getStateGUI() ) {
			QString msg;
#if QT_VERSION >= 0x050600 // Qt 5.6 or over
			msg.asprintf( "\"%s\" is not Grads control file.", m_FileNameCtl.toStdString().c_str() );
#else
			msg.sprintf( "\"%s\" is not Grads control file.", m_FileNameCtl.toStdString().c_str() );
#endif
			QMessageBox::critical( 0, "File Open Error", msg );
		}
		return GRADS_ERROR_CTLFILE_OPEN;
	}

	bool fileopened;
	fileopened = file.open( QIODevice::ReadOnly | QIODevice::Text );
	if ( fileopened != true ) {
		if ( appParam.getStateGUI() ) {
			QString msg;
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
			msg.asprintf( "Could not open \"%s\".", m_FileNameCtl.toStdString().c_str() );
#else
			msg.sprintf( "Could not open \"%s\".", m_FileNameCtl.toStdString().c_str() );
#endif
			QMessageBox::critical( 0, "File Open Error", msg );
		}
		return GRADS_ERROR_CTLFILE_OPEN;
	}

	// Read GrADS Control File
	QTextStream inputStream( &file );
	QString lineString;
	QString tempString;
	QString token1, token2;
	QStringList stringList;
	bool ok;

	while ( !inputStream.atEnd() ) {
		lineString = inputStream.readLine();
//		if ( lineString[0] != QChar( '#' ) ) {
		if ( lineString[0] != QChar( '*' ) ) {
			stringList = splitString( "\\s+", lineString );
			if ( stringList.size() >= 2 ) {
				token1 = ( stringList.at( 0 ) ).toUpper();
				token2 = ( stringList.at( 1 ) ).toUpper();
				if ( token1 == "DSET" ) {
					m_FileNameData = stringList.at( 1 );
					if ( m_FileNameData.data()[0] == '^' ) {
						m_FileNameData.remove( 0, 1 );
						if ( m_FileNameData.indexOf( "%" ) != -1 ) {
							return GRADS_ERROR_DSET;
						}
						if ( m_FileNameData.indexOf( "/" ) != 0 ) {
							QString filepath = fileinfo.absolutePath();
							m_FileNameData.prepend( filepath.toLocal8Bit() + "/" );
						}
					}
//					if ( !( QFileInfo( m_FileNameData ).exists() ) ) {
					if ( !( QFileInfo::exists( m_FileNameData ) ) ) {
						return GRADS_ERROR_DSET;
					}
				} else if ( token1 == "XDEF" ) {
					m_GridSizeX = token2.toInt( &ok );
					if ( !ok ) {
						return GRADS_ERROR_XDEF;
					}
					m_CoordX = new double[ m_GridSizeX ];
					m_AxisFormatX = ( stringList.at( 2 ) ).toUpper();
					if ( m_AxisFormatX == "LINEAR" ) {
						double sx = ( stringList.at( 3 ) ).toDouble( &ok );
						if ( !ok ) {
							return GRADS_ERROR_XDEF;
						}
						double dx = ( stringList.at( 4 ) ).toDouble( &ok ) * m_GridSizeX / ( m_GridSizeX - 1 );
						if ( !ok ) {
							return GRADS_ERROR_XDEF;
						}
						for ( int n = 0; n < m_GridSizeX; n++ ) {
							m_CoordX[n] = sx + ( double )n * dx;
						}
					} else if ( m_AxisFormatX == "LEVELS" ) {
						int count = 0;
						if ( stringList.size() > 3 ) {
							for ( int i = 3; i < stringList.size(); i++ ) {
								m_CoordX[ count ] = stringList.at(i).toDouble( &ok );
								if ( !ok ) {
									return GRADS_ERROR_XDEF;
								}
								count++;
							}
						}
						if ( count < m_GridSizeX ) {
							while(1) {
								tempString = inputStream.readLine();
								QStringList list;
								list = splitString( " ", tempString );
								for ( int i = 0; i < list.size(); i++ ) {
									m_CoordX[ count ] = list.at(i).toDouble( &ok );
									if ( !ok ) {
										return GRADS_ERROR_XDEF;
									}
									count++;
								}
								if ( count >= m_GridSizeX ) {
									break;
								}
							}
						}
					} else {
						return GRADS_ERROR_XDEF;
					}
				} else if ( token1 == "YDEF" ) {
					m_GridSizeY = token2.toInt( &ok );
					if ( !ok ) {
						return GRADS_ERROR_YDEF;
					}
					m_CoordY = new double[ m_GridSizeY ];
					m_AxisFormatY = ( stringList.at( 2 ) ).toUpper();
					if ( m_AxisFormatY == "LINEAR" ) {
						double sy = ( stringList.at( 3 ) ).toDouble( &ok );
						if ( !ok ) {
							return GRADS_ERROR_YDEF;
						}
//						double dy = ( stringList.at( 4 ) ).toDouble( &ok );
						double dy = ( stringList.at( 4 ) ).toDouble( &ok ) * m_GridSizeY / ( m_GridSizeY - 1 );
						if ( !ok ) {
							return GRADS_ERROR_YDEF;
						}
						for ( int n = 0; n < m_GridSizeY; n++ ) {
							m_CoordY[n] = sy + ( double )n * dy;
						}
					} else if ( m_AxisFormatY == "LEVELS" ) {
						int count = 0;
						if ( stringList.size() > 3 ) {
							for ( int i = 3; i < stringList.size(); i++ ) {
								m_CoordY[ count ] = stringList.at(i).toDouble( &ok );
								if ( !ok ) {
									return GRADS_ERROR_YDEF;
								}
								count++;
							}
						}
						if ( count < m_GridSizeY ) {
							while(1) {
								tempString = inputStream.readLine();
								QStringList list;
								list = splitString( " ", tempString );
								for ( int i = 0; i < list.size(); i++ ) {
									m_CoordY[ count ] = list.at(i).toDouble( &ok );
									if ( !ok ) {
										return GRADS_ERROR_YDEF;
									}
									count++;
								}
								if ( count >= m_GridSizeY ) {
									break;
								}
							}
						}
					} else {
						return GRADS_ERROR_YDEF;
					}
				} else if ( token1 == "ZDEF" ) {
					m_GridSizeZ = token2.toInt( &ok );
					if ( !ok ) {
						return GRADS_ERROR_ZDEF;
					}
					m_CoordZ = new double[ m_GridSizeZ ];
					m_AxisFormatZ = ( stringList.at( 2 ) ).toUpper();
					if ( m_AxisFormatZ == "LINEAR" ) {
						double sz = ( stringList.at( 3 ) ).toDouble( &ok );
						if ( !ok ) {
							return GRADS_ERROR_ZDEF;
						}
						double dz = ( stringList.at( 4 ) ).toDouble( &ok );
						if ( !ok ) {
							return GRADS_ERROR_ZDEF;
						}
						for ( int n = 0; n < m_GridSizeZ; n++ ) {
							m_CoordZ[n] = sz + ( double )n * dz;
						}
					} else if ( m_AxisFormatZ == "LEVELS" ) {
						int count = 0;
						if ( stringList.size() > 3 ) {
							for ( int i = 3; i < stringList.size(); i++ ) {
								m_CoordZ[ count ] = stringList.at(i).toDouble( &ok );
								if ( !ok ) {
									return GRADS_ERROR_ZDEF;
								}
								count++;
							}
						}
						if ( count < m_GridSizeZ ) {
							while(1) {
								tempString = inputStream.readLine();
								QStringList list;
								list = splitString( " ", tempString );
								for ( int i = 0; i < list.size(); i++ ) {
									m_CoordZ[ count ] = list.at(i).toDouble( &ok );
									if ( !ok ) {
										return GRADS_ERROR_ZDEF;
									}
									count++;
								}
								if ( count >= m_GridSizeZ ) {
									break;
								}
							}
						}
////////////////////////////////////////////////////////////
						if ( m_CoordZ[ 0 ] > m_CoordZ[ m_GridSizeZ - 1 ] ) {
							m_IsZRev = !m_IsZRev;
//							double CoordZTemp[ m_GridSizeZ ];
//							double *CoordZTemp = (double *)malloc(sizeof(double) * m_GridSizeZ);
							double *CoordZTemp = new double[ m_GridSizeZ ];
							for ( int i = 0; i < m_GridSizeZ; i++ ) {
								CoordZTemp[ m_GridSizeZ - 1 - i ] = m_CoordZ[ i ];
							}
							for ( int i = 0; i < m_GridSizeZ; i++ ) {
								m_CoordZ[ i ] = CoordZTemp[ i ];
							}
							delete [] CoordZTemp;
						}
////////////////////////////////////////////////////////////
					} else {
						return GRADS_ERROR_ZDEF;
					}
				} else if ( token1 == "TDEF" ) {
					QString Month[] = { "jan", "feb", "mar", "apr", "may", "jun",
										"jul", "aug", "sep", "oct", "nov", "dec" };
					m_GridSizeT = token2.toInt( &ok );
					if ( !ok ) {
						return GRADS_ERROR_TDEF;
					}
					m_AxisFormatT = ( stringList.at( 2 ) ).toUpper();
					if ( m_AxisFormatT != "LINEAR" ) {
						return GRADS_ERROR_TDEF;
					}

					QString StartTimeString = ( stringList.at( 3 ) ).toUpper();
					if ( StartTimeString.indexOf( ":" ) == -1 ) {
						if ( StartTimeString.indexOf( "Z" ) == 0 ) {
							StartTimeString.prepend( "00:00" );
						} else if ( StartTimeString.indexOf( "Z" ) == 1 ) {
							StartTimeString.prepend( "0" );
							StartTimeString.insert( 2, ":00" );
						} else if ( StartTimeString.indexOf( "Z" ) == 2 ) {
							StartTimeString.insert( 2, ":00" );
						} else if ( StartTimeString.indexOf( "Z" ) == -1 ) {
							int PositionOfMonthStrings;
							for ( int i = 0; i < 12; i++ ) {
								PositionOfMonthStrings =
									StartTimeString.indexOf(
										Month[ i ], 0, Qt::CaseInsensitive );
								if ( PositionOfMonthStrings != -1 ) {
									break;
								}
							}
							if ( PositionOfMonthStrings == 0 ) {
								StartTimeString.prepend( "00:00Z01" );
							} else if ( PositionOfMonthStrings == 1 ) {
								StartTimeString.prepend( "00:00Z0" );
							} else if ( PositionOfMonthStrings == 2 ) {
								StartTimeString.prepend( "00:00Z" );
							} else {
								return GRADS_ERROR_TDEF;
							}
						} else {
							return GRADS_ERROR_TDEF;
						}
					} else if ( StartTimeString.indexOf( ":" ) == 0 ) {
						StartTimeString.prepend( "00" );
					} else if ( StartTimeString.indexOf( ":" ) == 1 ) {
						StartTimeString.prepend( "0" );
					} else if ( StartTimeString.indexOf( ":" ) > 2 ) {
						return GRADS_ERROR_TDEF;
					}
					if ( StartTimeString.indexOf( "Z" ) == 4 ) {
						StartTimeString.insert( 3, "0" );
					}
					if ( StartTimeString.indexOf( "Z" ) == 5 ) {
						// second (additional parameter for VDVGE)
						StartTimeString.insert( 5, ":00" );
					}

					m_TimeInfo.stime.tm_year  = 0;
					m_TimeInfo.stime.tm_mon   = 0;
					m_TimeInfo.stime.tm_mday  = 0;
					m_TimeInfo.stime.tm_hour  = 0;
					m_TimeInfo.stime.tm_min   = 0;
					m_TimeInfo.stime.tm_sec   = 0;
					m_TimeInfo.stime.tm_isdst = -1;
					m_TimeInfo.itime.year   = 0;
					m_TimeInfo.itime.month  = 0;
					m_TimeInfo.itime.hour   = 0;
					m_TimeInfo.itime.day    = 0;
					m_TimeInfo.itime.minute = 0;
					m_TimeInfo.itime.second = 0;
					m_TimeInfo.isAfter1900 = false;
					if ( StartTimeString.data()[2] == ':' ) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
						m_TimeInfo.stime.tm_hour =
							QStringView(StartTimeString).mid( 0, 2 ).toString().toInt( &ok );
#else
						m_TimeInfo.stime.tm_hour =
							StartTimeString.midRef( 0, 2 ).toString().toInt( &ok );
#endif
						if ( !ok ) {
							return GRADS_ERROR_TDEF;
						}
						StartTimeString.remove( 0, 3 );
					}
					if ( StartTimeString.data()[2] == ':' ) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
						m_TimeInfo.stime.tm_min =
							QStringView(StartTimeString).mid( 0, 2 ).toString().toInt( &ok );
#else
						m_TimeInfo.stime.tm_min =
							StartTimeString.midRef( 0, 2 ).toString().toInt( &ok );
#endif
						if ( !ok ) {
							return GRADS_ERROR_TDEF;
						}
						StartTimeString.remove( 0, 3 );
					}
					if ( StartTimeString.data()[2].toUpper() == 'Z' ) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
						m_TimeInfo.stime.tm_sec =
							QStringView(StartTimeString).mid( 0, 2 ).toString().toInt( &ok );
#else
						m_TimeInfo.stime.tm_sec =
							StartTimeString.midRef( 0, 2 ).toString().toInt( &ok );
#endif
						if ( !ok ) {
							return GRADS_ERROR_TDEF;
						}
						StartTimeString.remove( 0, 3 );
					}
					int PositionOfMonthStrings;
					for ( int i = 0; i < 12; i++ ) {
						PositionOfMonthStrings =
							StartTimeString.indexOf( Month[ i ], 0, Qt::CaseInsensitive );
						if ( PositionOfMonthStrings != -1 ) {
							break;
						}
					}
					if ( PositionOfMonthStrings == 2 ) {
						if ( ( StartTimeString.size() == 9 ) || ( StartTimeString.size() == 7 ) ) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
							m_TimeInfo.stime.tm_mday =
								QStringView(StartTimeString).mid( 0, 2 ).toString().toInt( &ok );
#else
							m_TimeInfo.stime.tm_mday =
								StartTimeString.midRef( 0, 2 ).toString().toInt( &ok );
#endif
							if ( !ok ) {
								return GRADS_ERROR_TDEF;
							}
							StartTimeString.remove( 0, 2 );
						} else {
							return GRADS_ERROR_TDEF;
						}
					} else if ( PositionOfMonthStrings == 1 ) {
						if ( ( StartTimeString.size() == 8 ) || ( StartTimeString.size() == 6 ) ) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
							m_TimeInfo.stime.tm_mday =
								QStringView(StartTimeString).mid( 0, 1 ).toString().toInt( &ok );
#else
							m_TimeInfo.stime.tm_mday =
								StartTimeString.midRef( 0, 1 ).toString().toInt( &ok );
#endif
							if ( !ok ) {
								return GRADS_ERROR_TDEF;
							}
							StartTimeString.remove( 0, 1 );
						} else {
							return GRADS_ERROR_TDEF;
						}
					} else if ( PositionOfMonthStrings == 0 ) {
						if ( ( StartTimeString.size() == 7 ) || ( StartTimeString.size() == 5 ) ) {
							m_TimeInfo.stime.tm_mday = 1;
						} else {
							return GRADS_ERROR_TDEF;
						}
					} else {
						return GRADS_ERROR_TDEF;
					}
					for ( int i = 0; i < 12; i++ ) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
						if ( QStringView(StartTimeString).mid( 0, 3 ).toString().toLower() == Month[ i ] ) {
#else
						if ( StartTimeString.midRef( 0, 3 ).toString().toLower() == Month[ i ] ) {
#endif
							m_TimeInfo.stime.tm_mon = i;
							break;
						}
					}
					StartTimeString.remove( 0, 3 );
					if ( StartTimeString.size() == 4 ) {
						m_TimeInfo.startYear = StartTimeString.toInt( &ok );
						if ( !ok ) {
							return GRADS_ERROR_TDEF;
						}
					} else if ( StartTimeString.size() == 2 ) {
						m_TimeInfo.startYear = StartTimeString.toInt( &ok );
						if ( !ok ) {
							return GRADS_ERROR_TDEF;
						} else {
							m_TimeInfo.startYear += 1950;
						}
					} else {
						return GRADS_ERROR_TDEF;
					}
					if ( m_TimeInfo.startYear >= 1900 ) {
						m_TimeInfo.stime.tm_year = StartTimeString.toInt( &ok );
						if ( !ok ) {
							return GRADS_ERROR_TDEF;
						} else {
							m_TimeInfo.stime.tm_year -= 1900;
						}
						m_TimeInfo.isAfter1900 = true;
					} else {
						m_TimeInfo.isAfter1900 = false;
					}

					QString TimeIncrementString = ( stringList.at( 4 ) ).toLower();
					QString IncrementType[] = { "yr", "mo", "dy", "hr", "mn", "sc" };
					for ( int i = 0; i < 6; i++ ) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
						if ( QStringView(TimeIncrementString).right( 2 ) == IncrementType[ i ] ) {
#else
						if ( TimeIncrementString.rightRef( 2 ) == IncrementType[ i ] ) {
#endif
							TimeIncrementString.remove( TimeIncrementString.length() - 2, 2 );
							int inc = TimeIncrementString.toInt( &ok );
							if ( !ok ) {
								return GRADS_ERROR_TDEF;
							}
							switch ( i ) {
								case 0:
									m_TimeInfo.itime.year   = inc;
									break;
								case 1:
									m_TimeInfo.itime.month  = inc;
									break;
								case 2:
									m_TimeInfo.itime.day	= inc;
									break;
								case 3:
									m_TimeInfo.itime.hour   = inc;
									break;
								case 4:
									m_TimeInfo.itime.minute = inc;
									break;
								case 5:
									m_TimeInfo.itime.second = inc;
									break;
								default:
									break;
							}
							break;
						}
					}
				} else if ( token1 == "VARS" ) {
					m_VarNum = token2.toInt( &ok );
					if ( !ok ) {
						return GRADS_ERROR_VARS;
					}
					for ( int n = 0; n < m_VarNum; n++ ) {
						lineString = inputStream.readLine();
						if ( tempString[0] != QChar( '#' ) ) {
							stringList = splitString( "\\s+", lineString );
						}
						VarInfomation varinfo;
						varinfo.name  = stringList.at( 0 );
						varinfo.level = ( stringList.at( 1 ) ).toInt( &ok );
						if ( !ok ) {
							return GRADS_ERROR_VARS;
						}
						if ( varinfo.level == 0 ) {
							varinfo.level = 1;
						}
						varinfo.units = stringList.at( 2 );
						QString description;
						for ( int i = 0; i < 3; i++ )
						{
							stringList.removeAt( 0 );
						}
						description = stringList.join( " " );
						varinfo.description = description;
						m_VarInfo.push_back( varinfo );
					}
				} else if ( token1 == "FILEHEADER" ) {
					m_FileHeader = static_cast< int >( token2.toInt( &ok ) );
					if ( !ok ) {
						return GRADS_ERROR_FILEHEADER;
					}
				} else if ( token1 == "OPTIONS" ) {
					if ( stringList.size() > 1 ) {
						for ( int i = 1; i < stringList.size(); i++ ) {
							m_Option.push_back( stringList.at( i ).toUpper() );
						}
						if ( m_Option.indexOf( "SEQUENTIAL" ) != -1 ) {
							m_IsSequential = true;
						}
						if ( m_Option.indexOf( "YREV" ) != -1 ) {
							m_IsYRev = !m_IsYRev;
						}
						if ( m_Option.indexOf( "ZREV" ) != -1 ) {
							m_IsZRev = !m_IsZRev;
						}
						if ( ( m_Option.indexOf( "TEMPLATE" ) != -1 ) ||
							 ( m_Option.indexOf( "CRAY_32BIT_IEEE" ) != -1 ) ||
							 ( m_Option.indexOf( "PASCALS" ) != -1 ) ||
							 ( m_Option.indexOf( "365_DAY_CALENDAR" ) != -1 ) ) {
							return GRADS_ERROR_OPTIONS;
						}
					}
				} else if ( token1 == "UNDEF" ) {
					m_Undef = static_cast< float >( token2.toFloat( &ok ) );
					if ( !ok ) {
						return GRADS_ERROR_UNDEF;
					}
/*
				// unsupported options
				} else if ( token1 == "DTYPE" ) {
					return GRADS_ERROR_DTYPE;
				} else if ( token1 == "CHSUB" ) {
					return GRADS_ERROR_CHSUB;
				} else if ( token1 == "INDEX" ) {
					return GRADS_ERROR_INDEX;
				} else if ( token1 == "STNMAP" ) {
					return GRADS_ERROR_STNMAP;
*/
				}
			}
		} else {
			// add advanced features
		}
	}
	file.close();

	// Memory Allocation for Data Aria
	m_Data = new float[ m_GridSizeX * m_GridSizeY * m_GridSizeZ ];
	if ( m_Data == NULL ) {
		if ( appParam.getStateGUI() ) {
			QString msg;
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
			msg.asprintf( "Could not allocate memory to read data." );
#else
			msg.sprintf( "Could not allocate memory to read data." );
#endif
			QMessageBox::critical( 0, "Error", msg );
		}
		return GRADS_ERROR_MEM_ALOOC;
	}

	// Correspondence to the File Path Containing 2-Byte Character
	QByteArray byteArray = m_FileNameData.toLocal8Bit();
	m_DataFilePtr = fopen( byteArray.data(), "rb" );

	if ( setData( 0 ) == false ) {
		return GRADS_ERROR_DATAFILE_OPEN;
	}

	checkMinMax();

	m_IsDataLoaded = true;

	return GRADS_NO_ERROR;
}

void Grads::checkMinMax()
{
	QFile file( m_FileNameData );
	file.open( QIODevice::ReadOnly );
	QDataStream stream( &file );
	stream.device();
	if ( m_Option.indexOf( "BIG_ENDIAN" ) != -1 ) {
		stream.setByteOrder( QDataStream::BigEndian );
	} else if ( m_Option.indexOf( "LITTLE_ENDIAN" ) != -1 ) {
		stream.setByteOrder( QDataStream::LittleEndian );
	} else if ( m_Option.indexOf( "BYTESWAPPED" ) != -1 ) {
		if ( QSysInfo::ByteOrder == QSysInfo::LittleEndian ) {
			stream.setByteOrder( QDataStream::BigEndian );
		} else {
			stream.setByteOrder( QDataStream::LittleEndian );
		}
	} else {
		if ( QSysInfo::ByteOrder == QSysInfo::LittleEndian ) {
			stream.setByteOrder( QDataStream::LittleEndian );
		} else {
			stream.setByteOrder( QDataStream::BigEndian );
		}
	}
	stream.setFloatingPointPrecision( QDataStream::SinglePrecision );
	float max = -1e32f;
	float min =  1e32f;

	if ( !m_IsSequential ) {
		QFileInfo fileinfo( file );
		stream.skipRawData( m_FileHeader );
		if ( fileinfo.size() >= 1000000000 ) {
			// over 1 GB
	//		int finishRate = 500;
	//		int samplingInterval = 10000;
			int finishRate = 50;
			int samplingInterval = 100;
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
			std::srand( ( unsigned )time( 0 ) );
#else
			qsrand( ( unsigned )time( 0 ) );
#endif
			for ( int rate = 1; rate <= finishRate; rate++ ) {
				unsigned long dataCount = m_GridSizeX * m_GridSizeY * m_GridSizeZ;
	//			unsigned long dataCount = m_GridSizeX * m_GridSizeY * m_GridSizeZ * m_GridSizeT;
				unsigned long roopSize = dataCount / finishRate;
				if ( rate == finishRate )
					roopSize = dataCount - roopSize * ( finishRate - 1 );
				int roopNum = roopSize / samplingInterval;
				for ( int count = 0; count < roopNum; count++ ) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
					int targetPos = std::rand() % samplingInterval + 1;
#else
					int targetPos = qrand() % samplingInterval + 1;
#endif
					stream.skipRawData( ( targetPos - 1 ) * sizeof( float ) );
					float val;
					stream >> val;
					if ( val != m_Undef ) {
						if( val > max ) max = val;
						if( val < min ) min = val;
					}
					stream.skipRawData( ( samplingInterval - targetPos ) * sizeof( float ) );
				}
			}

		} else {

			ulong dataCount = m_GridSizeX * m_GridSizeY * m_GridSizeZ * m_GridSizeT;
			for ( ulong i = 0; i < dataCount; i++ ) {
				float val;
				stream >> val;
				if ( val != static_cast< float >( m_Undef ) ) {
					if( val > max ) max = val;
					if( val < min ) min = val;
				}
			}
		}
	} else {
		int fortranHeaderSize = 4;
		int fortranFooterSize = 4;
		for ( qint64 k = 0; k < m_GridSizeT; k++ ) {
			for ( qint64 j = 0; j < m_GridSizeZ; j++ ) {
				stream.skipRawData( fortranHeaderSize );
				for ( qint64 i = 0; i < ( m_GridSizeX * m_GridSizeY ); i++ ) {
					float val;
					stream >> val;
					if ( val != static_cast< float >( m_Undef ) ) {
						if( val > max ) max = val;
						if( val < min ) min = val;
					}
				}
				stream.skipRawData( fortranFooterSize );
			}
		}
	}
	file.close();

	m_Min = min;
	m_Max = max;
}

bool Grads::setData( int time )
{
	int fortranHeaderSize = 4;
	int fortranFooterSize = 4;
	int totalSizeZperStep = 0;
	for ( int i = 0; i < m_VarInfo.size(); i++ ) {
		totalSizeZperStep += m_VarInfo.at(i).level;
	}
	int varIndex = 0;	// current version can read only 1st variable
	int offsetSizeZ = 0;
	for ( int i = 0; i < varIndex; i++ ) {
		offsetSizeZ += m_VarInfo.at(i).level;
	}
	qint64 layerSize = sizeof( float ) * m_GridSizeX * m_GridSizeY;
	qint64 skipSize = layerSize * ( totalSizeZperStep * time + offsetSizeZ );
	if ( !m_IsSequential ) {
		skipSize += m_FileHeader;
	}

	QFile file( m_FileNameData );
	file.open( QIODevice::ReadOnly );
	file.seek( skipSize );
	QDataStream stream( &file );
	stream.device();
	stream.setFloatingPointPrecision( QDataStream::SinglePrecision );
	if ( m_Option.indexOf( "BIG_ENDIAN" ) != -1 ) {
		stream.setByteOrder( QDataStream::BigEndian );
	} else if ( m_Option.indexOf( "LITTLE_ENDIAN" ) != -1 ) {
		stream.setByteOrder( QDataStream::LittleEndian );
	} else if ( m_Option.indexOf( "BYTESWAPPED" ) != -1 ) {
		if ( QSysInfo::ByteOrder == QSysInfo::LittleEndian ) {
			stream.setByteOrder( QDataStream::BigEndian );
		} else {
			stream.setByteOrder( QDataStream::LittleEndian );
		}
	} else {
		if ( QSysInfo::ByteOrder == QSysInfo::LittleEndian ) {
			stream.setByteOrder( QDataStream::LittleEndian );
		} else {
			stream.setByteOrder( QDataStream::BigEndian );
		}
	}

	for ( qint64 k = 0; k < m_GridSizeZ; k++ ) {
		if ( m_IsSequential ) {
			stream.skipRawData( fortranHeaderSize );
		}
		for ( qint64 j = 0; j < m_GridSizeY; j++ ) {
			for ( qint64 i = 0; i < m_GridSizeX; i++ ) {
				qint64 idxY = j;
				qint64 idxZ = k;
				if ( m_IsYRev ) {
					idxY = ( m_GridSizeY - 1 ) - j;
				}
				if ( m_IsZRev ) {
					idxZ = ( m_GridSizeZ - 1 ) - k;
				}
				qint64 idx = m_GridSizeX * m_GridSizeY * idxZ + m_GridSizeX * idxY + i;
				stream >> m_Data[ idx ];
			}
		}
		if ( m_IsSequential ) {
			stream.skipRawData( fortranFooterSize );
		}
	}

	file.close();

	return true;
}

void Grads::cleanupMemory()
{
	if ( m_Data != NULL ) {
		delete [] m_Data;
	}
	if ( m_CoordX != NULL ) {
		delete [] m_CoordX;
	}
	if ( m_CoordX != NULL ) {
		delete [] m_CoordY;
	}
	if ( m_CoordX != NULL ) {
		delete [] m_CoordZ;
	}
}
