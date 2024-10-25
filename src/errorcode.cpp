//
// Volume Data Visualizer for Google Earth
//
// errorcode.cpp
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

#include "errorcode.h"

QString getErrString( ErrorCode err )
{
	QString errString;

	switch( err ) {
		case GRADS_NO_ERROR:
			errString = "GRADS_NO_ERROR";
			break;
		case GRADS_ERROR_CTLFILE_OPEN:
			errString = "GRADS_ERROR_CTLFILE_OPEN";
			break;
		case GRADS_ERROR_DATAFILE_OPEN:
			errString = "GRADS_ERROR_DATAFILE_OPEN";
			break;
		case GRADS_ERROR_MEM_ALOOC:
			errString = "GRADS_ERROR_MEM_ALOOC";
			break;
		case GRADS_ERROR_DSET:
			errString = "GRADS_ERROR_DSET";
			break;
		case GRADS_ERROR_XDEF:
			errString = "GRADS_ERROR_XDEF";
			break;
		case GRADS_ERROR_YDEF:
			errString = "GRADS_ERROR_YDEF";
			break;
		case GRADS_ERROR_ZDEF:
			errString = "GRADS_ERROR_ZDEF";
			break;
		case GRADS_ERROR_TDEF:
			errString = "GRADS_ERROR_TDEF";
			break;
		case GRADS_ERROR_VARS:
			errString = "GRADS_ERROR_VARS";
			break;
		case GRADS_ERROR_FILEHEADER:
			errString = "GRADS_ERROR_FILEHEADER";
			break;
		case GRADS_ERROR_OPTIONS:
			errString = "GRADS_ERROR_OPTIONS";
			break;
		case GRADS_ERROR_UNDEF:
			errString = "GRADS_ERROR_UNDEF";
			break;
		case GRADS_ERROR_DTYPE:
			errString = "GRADS_ERROR_DTYPE";
			break;
		case GRADS_ERROR_CHSUB:
			errString = "GRADS_ERROR_CHSUB";
			break;
		case GRADS_ERROR_INDEX:
			errString = "GRADS_ERROR_INDEX";
			break;
		case GRADS_ERROR_STNMAP:
			errString = "GRADS_ERROR_STNMAP";
			break;
		case GRADS_ERROR_UNPACK:
			errString = "GRADS_ERROR_UNPACK";
			break;
		case GRADS_ERROR_THEADER:
			errString = "GRADS_ERROR_THEADER";
			break;
		case GRADS_ERROR_HEADERBYTES:
			errString = "GRADS_ERROR_HEADERBYTES";
			break;
		case GRADS_ERROR_TRAILERBYTES:
			errString = "GRADS_ERROR_TRAILERBYTES";
			break;
		case GRADS_ERROR_XYHEADER:
			errString = "GRADS_ERROR_XYHEADER";
			break;
		case GRADS_ERROR_XVAR:
			errString = "GRADS_ERROR_XVAR";
			break;
		case GRADS_ERROR_YVAR:
			errString = "GRADS_ERROR_YVAR";
			break;
		case GRADS_ERROR_ZVAR:
			errString = "GRADS_ERROR_ZVAR";
			break;
		case GRADS_ERROR_STID:
			errString = "GRADS_ERROR_STID";
			break;
		case GRADS_ERROR_TVAR:
			errString = "GRADS_ERROR_TVAR";
			break;
		case GRADS_ERROR_TOFFVAR:
			errString = "GRADS_ERROR_TOFFVAR";
			break;
		case GRADS_ERROR_CACHESIZE:
			errString = "GRADS_ERROR_CACHESIZE";
			break;
		case GRADS_ERROR_PDEF:
			errString = "GRADS_ERROR_PDEF";
			break;
		case GRADS_ERROR_EDEF:
			errString = "GRADS_ERROR_EDEF";
			break;
		case GRADS_ERROR_VECTORPAIRS:
			errString = "GRADS_ERROR_VECTORPAIRS";
			break;
		case GRADS_ERROR_ATMARK:
			errString = "GRADS_ERROR_";
			break;

		case PARAM_NO_ERROR:
			errString = "PARAM_NO_ERROR";
			break;
		case PARAM_COLORTABLE_NO_ERROR:
			errString = "PARAM_COLORTABLE_NO_ERROR";
			break;

		case PARAM_ERROR_SAVEFILE_OPEN:
			errString = "PARAM_ERROR_SAVEFILE_OPEN";
			break;
		case PARAM_ERROR_TIME_BEGIN:
			errString = "PARAM_ERROR_TIME_BEGIN";
			break;
		case PARAM_ERROR_TIME_END:
			errString = "PARAM_ERROR_TIME_END";
			break;
		case PARAM_ERROR_TIME_SKIP:
			errString = "PARAM_ERROR_TIME_SKIP";
			break;
		case PARAM_ERROR_TIME_CONVERSION_TO_UTC:
			errString = "PARAM_ERROR_TIME_CONVERSION_TO_UTC";
			break;
		case PARAM_ERROR_ZUNIT:
			errString = "PARAM_ERROR_ZUNIT";
			break;
		case PARAM_ERROR_ZSCALE:
			errString = "PARAM_ERROR_ZSCALE";
			break;
		case PARAM_ERROR_OBJECT_X:
			errString = "PARAM_ERROR_OBJECT_X";
			break;
		case PARAM_ERROR_OBJECT_Y:
			errString = "PARAM_ERROR_OBJECT_Y";
			break;
		case PARAM_ERROR_OBJECT_Z:
			errString = "PARAM_ERROR_OBJECT_Z";
			break;
		case PARAM_ERROR_OBJECT_PITCH:
			errString = "PARAM_ERROR_OBJECT_PITCH";
			break;
		case PARAM_ERROR_OBJECT_YAW:
			errString = "PARAM_ERROR_OBJECT_YAW";
			break;
		case PARAM_ERROR_OBJECT_X_SPHERE:
			errString = "PARAM_ERROR_OBJECT_X_SPHERE";
			break;
		case PARAM_ERROR_OBJECT_Y_SPHERE:
			errString = "PARAM_ERROR_OBJECT_Y_SPHERE";
			break;
		case PARAM_ERROR_OBJECT_Z_SPHERE:
			errString = "PARAM_ERROR_OBJECT_Z_SPHERE";
			break;
		case PARAM_ERROR_OBJECT_PITCH_SPHERE:
			errString = "PARAM_ERROR_OBJECT_PITCH_SPHERE";
			break;
		case PARAM_ERROR_OBJECT_YAW_SPHERE:
			errString = "PARAM_ERROR_OBJECT_YAW_SPHERE";
			break;
		case PARAM_ERROR_VOLUME_THRESHOLD_MIN:
			errString = "PARAM_ERROR_VOLUME_THRESHOLD_MIN";
			break;
		case PARAM_ERROR_VOLUME_THRESHOLD_MAX:
			errString = "PARAM_ERROR_VOLUME_THRESHOLD_MAX";
			break;
		case PARAM_ERROR_LOGSCALE:
			errString = "PARAM_ERROR_LOGSCALE";
			break;
		case PARAM_ERROR_VOLUME_X:
			errString = "PARAM_ERROR_VOLUME_X";
			break;
		case PARAM_ERROR_VOLUME_Y:
			errString = "PARAM_ERROR_VOLUME_Y";
			break;
		case PARAM_ERROR_VOLUME_Z:
			errString = "PARAM_ERROR_VOLUME_Z";
			break;
		case PARAM_ERROR_VOLUME_X_SLICES:
			errString = "PARAM_ERROR_VOLUME_X_SLICES";
			break;
		case PARAM_ERROR_VOLUME_Y_SLICES:
			errString = "PARAM_ERROR_VOLUME_Y_SLICES";
			break;
		case PARAM_ERROR_VOLUME_Z_SLICES:
			errString = "PARAM_ERROR_VOLUME_Z_SLICES";
			break;
		case PARAM_ERROR_COLORSLICE_X:
			errString = "PARAM_ERROR_COLORSLICE_X";
			break;
		case PARAM_ERROR_COLORSLICE_Y:
			errString = "PARAM_ERROR_COLORSLICE_Y";
			break;
		case PARAM_ERROR_COLORSLICE_Z:
			errString = "PARAM_ERROR_COLORSLICE_Z";
			break;
		case PARAM_ERROR_COLORSLICE_X_POS:
			errString = "PARAM_ERROR_COLORSLICE_X_POS";
			break;
		case PARAM_ERROR_COLORSLICE_Y_POS:
			errString = "PARAM_ERROR_COLORSLICE_Y_POS";
			break;
		case PARAM_ERROR_COLORSLICE_Z_POS:
			errString = "PARAM_ERROR_COLORSLICE_Z_POS";
			break;
		case PARAM_ERROR_COLORSLICE_CLAMP_TO_GROUND:
			errString = "PARAM_ERROR_COLORSLICE_CLAMP_TO_GROUND";
			break;
		case PARAM_ERROR_COLORSLICE_ALPHA:
			errString = "PARAM_ERROR_COLORSLICE_ALPHA";
			break;
		case PARAM_ERROR_COLORSLICE_OPACITY:
			errString = "PARAM_ERROR_COLORSLICE_OPACITY";
			break;
		case PARAM_ERROR_CONTOURLINE_X:
			errString = "PARAM_ERROR_CONTOURLINE_X";
			break;
		case PARAM_ERROR_CONTOURLINE_Y:
			errString = "PARAM_ERROR_CONTOURLINE_Y";
			break;
		case PARAM_ERROR_CONTOURLINE_Z:
			errString = "PARAM_ERROR_CONTOURLINE_Z";
			break;
		case PARAM_ERROR_CONTOURLINE_X_POS:
			errString = "PARAM_ERROR_CONTOURLINE_X_POS";
			break;
		case PARAM_ERROR_CONTOURLINE_Y_POS:
			errString = "PARAM_ERROR_CONTOURLINE_Y_POS";
			break;
		case PARAM_ERROR_CONTOURLINE_Z_POS:
			errString = "PARAM_ERROR_CONTOURLINE_Z_POS";
			break;
		case PARAM_ERROR_CONTOURLINE_THRESHOLD_MIN:
			errString = "PARAM_ERROR_CONTOURLINE_THRESHOLD_MIN";
			break;
		case PARAM_ERROR_CONTOURLINE_THRESHOLD_MAX:
			errString = "PARAM_ERROR_CONTOURLINE_THRESHOLD_MAX";
			break;
		case PARAM_ERROR_CONTOURLINE_INTERVAL:
			errString = "PARAM_ERROR_CONTOURLINE_INTERVAL";
			break;
		case PARAM_ERROR_CONTOURLINE_WIDTH:
			errString = "PARAM_ERROR_CONTOURLINE_WIDTH";
			break;
		case PARAM_ERROR_BACKGROUND_COLOR:
			errString = "PARAM_ERROR_BACKGROUND_COLOR";
			break;
		case PARAM_ERROR_BOUNDINGBOX:
			errString = "PARAM_ERROR_BOUNDINGBOX";
			break;
		case PARAM_ERROR_BOUNDINGBOX_AXISLABEL:
			errString = "PARAM_ERROR_BOUNDINGBOX_AXISLABEL";
			break;
		case PARAM_ERROR_COLORBAR:
			errString = "PARAM_ERROR_COLORBAR";
			break;
		case PARAM_ERROR_COLORBAR_DIRECTION:
			errString = "PARAM_ERROR_COLORBAR_DIRECTION";
			break;
		case PARAM_ERROR_COLORBAR_TYPE:
			errString = "PARAM_ERROR_COLORBAR_TYPE";
			break;
		case PARAM_ERROR_COLORBAR_DECIMAL_PLACES:
			errString = "PARAM_ERROR_COLORBAR_DECIMAL_PLACES";
			break;
		case PARAM_ERROR_COLORBAR_MAJOR_TICKMARKS:
			errString = "PARAM_ERROR_COLORBAR_MAJOR_TICKMARKS";
			break;
		case PARAM_ERROR_COLORBAR_MINOR_TICKMARKS:
			errString = "PARAM_ERROR_COLORBAR_MINOR_TICKMARKS";
			break;
		case PARAM_ERROR_COLORBAR_MAJOR_TICKMARK_TYPE:
			errString = "PARAM_ERROR_COLORBAR_MAJOR_TICKMARK_TYPE";
			break;
		case PARAM_ERROR_COLORBAR_MINOR_TICKMARK_TYPE:
			errString = "PARAM_ERROR_COLORBAR_MINOR_TICKMARK_TYPE";
			break;
		case PARAM_ERROR_TIMEDISPLAY:
			errString = "PARAM_ERROR_TIMEDISPLAY";
			break;
		case PARAM_ERROR_LOGO:
			errString = "PARAM_ERROR_LOGO";
			break;
		case PARAM_ERROR_LOGOFILE_OPEN:
			errString = "PARAM_ERROR_LOGOFILE_OPEN";
			break;
		case PARAM_ERROR_COLORTABLE:
			errString = "PARAM_ERROR_COLORTABLE";
			break;
		case PARAM_ERROR_COLORTABLE_FILE_OPEN:
			errString = "PARAM_ERROR_COLORTABLE_FILE_OPEN";
			break;
		case PARAM_ERROR_COLORTABLE_RED:
			errString = "PARAM_ERROR_COLORTABLE_RED";
			break;
		case PARAM_ERROR_COLORTABLE_GREEN:
			errString = "PARAM_ERROR_COLORTABLE_GREEN";
			break;
		case PARAM_ERROR_COLORTABLE_BLUE:
			errString = "PARAM_ERROR_COLORTABLE_BLUE";
			break;
		case PARAM_ERROR_COLORTABLE_ALPHA:
			errString = "PARAM_ERROR_COLORTABLE_ALPHA";
			break;

		case PARAM_ERROR_KML_MAX_IMAGE_SIZE:
			errString = "PARAM_ERROR_KML_MAX_IMAGE_SIZE";
			break;

		case COASTLINE_NO_ERROR:
			errString = "COASTLINE_NO_ERROR";
			break;
		case COASTLINE_ERROR_FILE_OPEN:
			errString = "COASTLINE_ERROR_FILE_OPEN";
			break;
		case COASTLINE_ERROR_SYNTAX:
			errString = "COASTLINE_ERROR_SYNTAX";
			break;

		case APPPARAM_NO_ERROR:
			errString = "APPPARAM_NO_ERROR";
			break;

		default:
			errString = "ERROR_UNDEFINED_PARAMETER";
			break;
	}

	return errString;
}
