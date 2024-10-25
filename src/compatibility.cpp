//
// Volume Data Visualizer for Google Earth
//
// compatibility.cpp
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

#include "compatibility.h"

QStringList splitString(QString regEx, QString str)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
//	const static QRegularExpression &reg = QRegularExpression( regEx );
//	return str.split( reg, Qt::SkipEmptyParts );
	return str.split( QRegularExpression( regEx ), Qt::SkipEmptyParts );
#else
	return str.split( QRegExp( regEx ), QString::SkipEmptyParts );
#endif
}
