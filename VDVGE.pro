# -------------------------------------------------
# Project created by QtCreator 2010-06-09T12:09:09
# -------------------------------------------------
QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += openglwidgets core5compat

TARGET = vdvge
TEMPLATE = app
DESTDIR = build

win32:VERSION = 1.2.0.0
else:VERSION = 1.2.0
QMAKE_TARGET_COMPANY = Japan Agency for Marine-Earth Science and Technology (JAMSTEC)
QMAKE_TARGET_PRODUCT = VDVGE
QMAKE_TARGET_DESCRIPTION = Volume Data Visualizer for Google Earth
QMAKE_TARGET_COPYRIGHT = (C) 2021-2024 Shintaro Kawahara

CONFIG += release
#CONFIG += release static
#CONFIG += debug
SOURCES += \
	src/app_parameter.cpp \
	src/boundingbox.cpp \
	src/coastline.cpp \
	src/colorbar.cpp \
	src/colorslice.cpp \
	src/colortable.cpp \
	src/compatibility.cpp \
	src/contourline.cpp \
	src/earth.cpp \
	src/errorcode.cpp \
	src/gradients.cpp \
	src/grads.cpp \
	src/interpolator.cpp \
	src/kml.cpp \
	src/logo.cpp \
	src/main.cpp \
	src/mainwindow.cpp \
	src/parameter.cpp \
	src/texture3d.cpp \
	src/timedisplay.cpp \
	src/topo.cpp \
	src/util.cpp \
	src/viewer.cpp \
	src/volumerender.cpp \
	panels/boundingbox_control.cpp \
	panels/coastline_control.cpp \
	panels/colorbar_control.cpp \
	panels/colorslice_control.cpp \
	panels/contourline_control.cpp \
	panels/control_panel.cpp \
	panels/data_panel.cpp \
	panels/datarange_control.cpp \
	panels/effect_control.cpp \
	panels/fileinfo_panel.cpp \
	panels/font_control.cpp \
	panels/gradsinfo.cpp \
	panels/worldmap.cpp \
	panels/kml_control.cpp \
	panels/logo_control.cpp \
	panels/misc_panel.cpp \
	panels/preferences_panel.cpp \
	panels/timerange_control.cpp \
	panels/topo_control.cpp \
	panels/topo_panel.cpp \
	panels/viewer_control.cpp \
	panels/visualization_panel.cpp \
	panels/volumerender_control.cpp \
	panels/zscale_control.cpp
HEADERS += \
	src/app_parameter.h \
	src/boundingbox.h \
	src/coastline.h \
	src/colorbar.h \
	src/colorslice.h \
	src/colortable.h \
	src/common.h \
	src/compatibility.h \
	src/contourline.h \
	src/define.h \
	src/earth.h \
	src/errorcode.h \
	src/gradients.h \
	src/grads.h \
	src/interpolator.h \
	src/kml.h \
	src/logo.h \
	src/mainwindow.h \
	src/parameter.h \
	src/texture3d.h \
	src/timedisplay.h \
	src/topo.h \
	src/util.h \
	src/viewer.h \
	src/volumerender.h \
	panels/boundingbox_control.h \
	panels/data_panel.h \
	panels/datarange_control.h \
	panels/effect_control.h \
	panels/fileinfo_panel.h \
	panels/font_control.h \
	panels/gradsinfo.h \
	panels/coastline_control.h \
	panels/colorbar_control.h \
	panels/colorslice_control.h \
	panels/contourline_control.h \
	panels/control_panel.h \
	panels/kml_control.h \
	panels/logo_control.h \
	panels/misc_panel.h \
	panels/preferences_panel.h \
	panels/timerange_control.h \
	panels/topo_panel.h \
	panels/topo_control.h \
	panels/viewer_control.h \
	panels/visualization_panel.h \
	panels/volumerender_control.h \
	panels/worldmap.h \
	panels/zscale_control.h

RESOURCES += \
	resources.qrc

macx {
    QMAKE_INFO_PLIST = $$PWD/Info.plist.in
	QMAKE_MACOS_DEPLOYMENT_TARGET = 10.5
	QMAKE_APPLE_DEVICE_ARCHS = x86_64
	greaterThan(QT_MAJOR_VERSION, 5) {
	    QMAKE_MACOS_DEPLOYMENT_TARGET = 10.13
		QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64
	}
	greaterThan(QT_MAJOR_VERSION, 6) {
	    QMAKE_MACOS_DEPLOYMENT_TARGET = 11
		QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64
		greaterThan(QT_MINOR_VERSION, 8) {
		    QMAKE_MACOS_DEPLOYMENT_TARGET = 12
		}
	}
}

unix:!macx {
	LIBS += -lGLU
}

win32 {
	greaterThan(QT_MAJOR_VERSION, 4): LIBS += -lglu32 -lopengl32
}

OPTION += console
OPTION -= app_bundle
