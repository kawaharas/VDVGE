//
// Volume Data Visualizer for Google Earth
//
// util.cpp
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

#include "util.h"

TimeDef getActualTime( int step )
{
	TimeDef timedef;
	struct tm time;
	int yearfix_num = 0;

	TimeInfo timeinfo = grads.getTimeInfo();
	int currentYear = timeinfo.startYear + timeinfo.itime.year * step
		+ ( int )floor( ( double )( timeinfo.stime.tm_mon + timeinfo.itime.month  * step ) / 12.0 );
	int currentMonth = ( timeinfo.stime.tm_mon + timeinfo.itime.month * step ) % 12;
	time.tm_mon   = currentMonth;
	time.tm_mday  = timeinfo.stime.tm_mday + timeinfo.itime.day    * step;
	time.tm_hour  = timeinfo.stime.tm_hour + timeinfo.itime.hour   * step;
	time.tm_min   = timeinfo.stime.tm_min  + timeinfo.itime.minute * step;
	time.tm_sec   = timeinfo.stime.tm_sec  + timeinfo.itime.second * step;
	time.tm_isdst = -1;

	if ( ( currentYear < 1971 ) || ( currentYear > 2030 ) )  {
		// mktime() does not work properly before 1970/01/01 09:00:00.
		yearfix_num  = currentYear - 1971;
		time.tm_year = 71;
	} else {
		// struct tm.tm_year start counting from 1900.
		time.tm_year = currentYear - 1900;
	}

	time_t tim;
	tim = mktime( &time );
	struct tm *ctime;
	ctime = localtime( &tim );

	timedef.year   = ctime->tm_year + 1900 + yearfix_num;
	timedef.month  = ctime->tm_mon + 1;
	timedef.day    = ctime->tm_mday;
	timedef.hour   = ctime->tm_hour;
	timedef.minute = ctime->tm_min;
	timedef.second = ctime->tm_sec;

	return timedef;
}

QString getTimeString( TimeDef time )
{
	QString Month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
						"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
/*
	QString TimeString = QString("%1:%2 %3 %4, %5")
						.arg( time.hour  , 2, 10, QChar('0') )
						.arg( time.minute, 2, 10, QChar('0') )
						.arg( Month[ time.month - 1 ] )
						.arg( time.day   , 2, 10, QChar('0') )
						.arg( time.year  , 4, 10, QChar('0') );
*/
	TimeInfo timeinfo = grads.getTimeInfo();
	QString TimeString;
	if ( timeinfo.itime.second == 0 ) {
		TimeString = QString("%1:%2 %3 %4, %5")
			.arg( time.hour  , 2, 10, QChar('0') )
			.arg( time.minute, 2, 10, QChar('0') )
			.arg( Month[ time.month - 1 ] )
			.arg( time.day   , 2, 10, QChar('0') )
			.arg( time.year  , 4, 10, QChar('0') );
	} else {
		TimeString = QString("%1:%2:%3 %4 %5, %6")
			.arg( time.hour  , 2, 10, QChar('0') )
			.arg( time.minute, 2, 10, QChar('0') )
			.arg( time.second, 2, 10, QChar('0') )
			.arg( Month[ time.month - 1 ] )
			.arg( time.day   , 2, 10, QChar('0') )
			.arg( time.year  , 4, 10, QChar('0') );
	}

	return TimeString;
}

QString getTimeStringKML( TimeDef time )
{
/*
	QString TimeString = QString( "%1-%2-%3T%4:%5:%6Z" )
					   .arg( time.year  , 4, 10, QChar( '0' ) )
					   .arg( time.month , 2, 10, QChar( '0' ) )
					   .arg( time.day   , 2, 10, QChar( '0' ) )
					   .arg( time.hour  , 2, 10, QChar( '0' ) )
					   .arg( time.minute, 2, 10, QChar( '0' ) )
					   .arg( time.second, 2, 10, QChar( '0' ) );
*/
	QString TimeString = QString( "%1-%2-%3T%4:%5:%6" )
					   .arg( time.year  , 4, 10, QChar( '0' ) )
					   .arg( time.month , 2, 10, QChar( '0' ) )
					   .arg( time.day   , 2, 10, QChar( '0' ) )
					   .arg( time.hour  , 2, 10, QChar( '0' ) )
					   .arg( time.minute, 2, 10, QChar( '0' ) )
					   .arg( time.second, 2, 10, QChar( '0' ) );

	if ( param.IsTimeConversionToUTC() ) {
		TimeString.append( param.getTimeConversionToUTC() );
	} else {
		TimeString.append( "Z" );
	}

	return TimeString;
}
