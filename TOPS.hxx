//-------------------------------------------------------------------------//
//              ROS Timetable Editor TOPS Train Class                      //
//                                                                         //
// This file provides part of the source code towards the standalone       //
// timetable editor constructed using the Qt v5.14 framework.              //
// for Railway Operation Simulator, a railway signalling simulation        //
// by Albert Ball, Copyright (c) 2010 [original development].              //
//                                                                         //
// Copyright (C) 2020 Kristian Zarebski                                    //
//                                                                         //
// This program is free software: you can redistribute it and/or modify    //
// it under the terms of the GNU General Public License as published by    //
// the Free Software Foundation, either version 3 of the License, or       //
// (at your option) any later version.                                     //
//                                                                         //
// This program is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of          //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           //
// GNU General Public License for more details.                            //
//                                                                         //
// You should have received a copy of the GNU General Public License       //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.   //
//                                                                         //
// Efforts to improve on this application are welcomed, with the request   //
// that any improvements you deem notable be offered back to the           //
// original project.                                                       //
//                                                                         //
//-------------------------------------------------------------------------//

// The provision of these TOPS class templates for the editor are provided //
// with thanks to Mark J. for assembling the required information.         //


#ifndef TOPS_HXX
#define TOPS_HXX

#include "train_type.hxx"
#include <QString>
#include <QMap>

/*! @namespace TOPS
  
Namespace containing all TOPS multiple units

@author K. Zarebski, Mark J.

@copyright GNU Public License v3

@date last modified 2020-02-17
*/

namespace TOPS
{


	//! 91+MK4+DVT
	extern const TrainType class_91;

	//! 90+MK3+DVT
	extern const TrainType class_90;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_313">Class 313/0 TOPS Classification</a>
	extern const TrainType class_313_0;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_313">Class 313/1 TOPS Classification</a>
	extern const TrainType class_313_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_313">Class 313/2 TOPS Classification</a>
	extern const TrainType class_313_2;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_314">Class 314/2 TOPS Classification</a>
	extern const TrainType class_314_2;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_315">Class 315/8 TOPS Classification</a>
	extern const TrainType class_315_8;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_317">Class 317/1 TOPS Classification</a>
	extern const TrainType class_317_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_317">Class 317/5 TOPS Classification</a>
	extern const TrainType class_317_5;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_317">Class 317/6 TOPS Classification</a>
	extern const TrainType class_317_6;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_317">Class 317/7 TOPS Classification</a>
	extern const TrainType class_317_7;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_317">Class 317/8 TOPS Classification</a>
	extern const TrainType class_317_8;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_318">Class 318 TOPS Classification</a>
	extern const TrainType class_318;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_319">Class 319 TOPS Classification</a>
	extern const TrainType class_319;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_320">Class 320/3 TOPS Classification</a>
	extern const TrainType class_320_3;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_321">Class 321/3 TOPS Classification</a>
	extern const TrainType class_321_3;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_321">Class 321/4 TOPS Classification</a>
	extern const TrainType class_321_4;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_322">Class 322 TOPS Classification</a>
	extern const TrainType class_322;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_323">Class 323 TOPS Classification</a>
	extern const TrainType class_323;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_325">Class 325 TOPS Classification</a>
	extern const TrainType class_325;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_331">Class 331/0 TOPS Classification</a>
	extern const TrainType class_331_0;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_331">Class 331/1 TOPS Classification</a>
	extern const TrainType class_331_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_332">Class 332 TOPS Classification</a>
	extern const TrainType class_332;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_333">Class 333 TOPS Classification</a>
	extern const TrainType class_333;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_334">Class 334 TOPS Classification</a>
	extern const TrainType class_334;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_345">Class 345 TOPS Classification</a>
	extern const TrainType class_345;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_350">Class 350/1 TOPS Classification</a>
	extern const TrainType class_350_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_350">Class 350/2 TOPS Classification</a>
	extern const TrainType class_350_2;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_350">Class 350/3 TOPS Classification</a>
	extern const TrainType class_350_3;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_350">Class 350/4 TOPS Classification</a>
	extern const TrainType class_350_4;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_357">Class 357 TOPS Classification</a>
	extern const TrainType class_357;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_360">Class 360/1 TOPS Classification</a>
	extern const TrainType class_360_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_360">Class 360/5 TOPS Classification</a>
	extern const TrainType class_360_5;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_365">Class 365 TOPS Classification</a>
	extern const TrainType class_365;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_373">Class 373 TOPS Classification</a>
	extern const TrainType class_373;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_375">Class 375 TOPS Classification</a>
	extern const TrainType class_375;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_376">Class 376 TOPS Classification</a>
	extern const TrainType class_376;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_377">Class 377/1 TOPS Classification</a>
	extern const TrainType class_377_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_377">Class 377/2 TOPS Classification</a>
	extern const TrainType class_377_2;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_377">Class 377/3 TOPS Classification</a>
	extern const TrainType class_377_3;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_377">Class 377/4 TOPS Classification</a>
	extern const TrainType class_377_4;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_377">Class 377/5 TOPS Classification</a>
	extern const TrainType class_377_5;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_377">Class 377/6 TOPS Classification</a>
	extern const TrainType class_377_6;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_377">Class 377/7 TOPS Classification</a>
	extern const TrainType class_377_7;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_378">Class 378/0 TOPS Classification</a>
	extern const TrainType class_378_0;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_378">Class 378/1 TOPS Classification</a>
	extern const TrainType class_378_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_378">Class 378/2 TOPS Classification</a>
	extern const TrainType class_378_2;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_379">Class 379 TOPS Classification</a>
	extern const TrainType class_379;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_380">Class 380/0 TOPS Classification</a>
	extern const TrainType class_380_0;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_380">Class 380/1 TOPS Classification</a>
	extern const TrainType class_380_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_399">Class 399 TOPS Classification</a>
	extern const TrainType class_399;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_139">Class 139 TOPS Classification</a>
	extern const TrainType class_139;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_142">Class 142 TOPS Classification</a>
	extern const TrainType class_142;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_143">Class 143/0 TOPS Classification</a>
	extern const TrainType class_143_0;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_143">Class 143/3 TOPS Classification</a>
	extern const TrainType class_143_3;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_143">Class 143/6 TOPS Classification</a>
	extern const TrainType class_143_6;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_144">Class 144 TOPS Classification</a>
	extern const TrainType class_144;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_150">Class 150/0 TOPS Classification</a>
	extern const TrainType class_150_0;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_150">Class 150/1 TOPS Classification</a>
	extern const TrainType class_150_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_150">Class 150/2 TOPS Classification</a>
	extern const TrainType class_150_2;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_150">Class 150/9 TOPS Classification</a>
	extern const TrainType class_150_9;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_153">Class 153 TOPS Classification</a>
	extern const TrainType class_153;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_155">Class 155 TOPS Classification</a>
	extern const TrainType class_155;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_156">Class 156 TOPS Classification</a>
	extern const TrainType class_156;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_158">Class 158 TOPS Classification</a>
	extern const TrainType class_158;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_159">Class 159 TOPS Classification</a>
	extern const TrainType class_159;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_165">Class 165/0 TOPS Classification</a>
	extern const TrainType class_165_0;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_165">Class 165/1 TOPS Classification</a>
	extern const TrainType class_165_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_166">Class 166 TOPS Classification</a>
	extern const TrainType class_166;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_168">Class 168 TOPS Classification</a>
	extern const TrainType class_168;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_170">Class 170/1 TOPS Classification</a>
	extern const TrainType class_170_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_170">Class 170/2 TOPS Classification</a>
	extern const TrainType class_170_2;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_170">Class 170/3 TOPS Classification</a>
	extern const TrainType class_170_3;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_170">Class 170/4 TOPS Classification</a>
	extern const TrainType class_170_4;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_170">Class 170/5 TOPS Classification</a>
	extern const TrainType class_170_5;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_170">Class 170/6 TOPS Classification</a>
	extern const TrainType class_170_6;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_171">Class 171/7 TOPS Classification</a>
	extern const TrainType class_171_7;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_171">Class 171/8 TOPS Classification</a>
	extern const TrainType class_171_8;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_172">Class 172/0 TOPS Classification</a>
	extern const TrainType class_172_0;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_172">Class 172/1 TOPS Classification</a>
	extern const TrainType class_172_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_172">Class 172/2 TOPS Classification</a>
	extern const TrainType class_172_2;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_172">Class 172/3 TOPS Classification</a>
	extern const TrainType class_172_3;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_175">Class 175/0 TOPS Classification</a>
	extern const TrainType class_175_0;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_175">Class 175/1 TOPS Classification</a>
	extern const TrainType class_175_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_180">Class 180 TOPS Classification</a>
	extern const TrainType class_180;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_185">Class 185 TOPS Classification</a>
	extern const TrainType class_185;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_220">Class 220 TOPS Classification</a>
	extern const TrainType class_220;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_221">Class 221 TOPS Classification</a>
	extern const TrainType class_221;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_222">Class 222/0 TOPS Classification</a>
	extern const TrainType class_222_0;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_222">Class 222/1 TOPS Classification</a>
	extern const TrainType class_222_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_195">Class 195/0 TOPS Classification</a>
	extern const TrainType class_195_0;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_195">Class 195/1 TOPS Classification</a>
	extern const TrainType class_195_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_230">Class 230 TOPS Classification</a>
	extern const TrainType class_230;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_800">Class 800/1 TOPS Classification</a>
	extern const TrainType class_800_1;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_800">Class 800/2 TOPS Classification</a>
	extern const TrainType class_800_2;

	//! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_802">Class 802 TOPS Classification</a>
	extern const TrainType class_802;

	//! 68+MK5+DVT
	extern const TrainType class_68;

	//! 67+MK3+DVT
	extern const TrainType class_67;

	//! HST *Castle*
	extern const TrainType hst_castle;

	//! HST
	extern const TrainType hst;

	extern const QMap<QString, const TrainType*> tops;
};

#endif // TOPS_HXX
