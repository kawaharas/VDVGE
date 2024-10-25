//
// Volume Data Visualizer for Google Earth
//
// compatibility.h
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

#ifndef COMPATIBILITY_H
#define COMPATIBILITY_H

#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
typedef QOpenGLWidget QGLWidget;
#else
#include <QGLWidget>
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
#define ENDL Qt::endl
#else
#define ENDL endl
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#define qSort std::sort
#endif

#include <QRegularExpression>
QStringList splitString(QString regEx, QString str);

#endif // COMPATIBILITY_H
