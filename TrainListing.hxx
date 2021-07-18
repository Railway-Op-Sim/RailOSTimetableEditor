//-------------------------------------------------------------------------//
//              ROS Timetable Editor TrainSet Train Class                      //
//                                                                         //
// This file provides part of the source code towards the standalone       //
// timetable editor constructed using the Qt v5.15 framework.              //
// for Railway Operation Simulator, a railway signalling simulation        //
// by Albert Ball, Copyright (c) 2010 [original development].              //
//                                                                         //
// Copyright (C) 2021 Kristian Zarebski                                    //
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

// The provision of these TrainSet class templates for the editor are provided //
// with thanks to Mark J. for assembling the required information.         //


#ifndef TrainSet_HXX
#define TrainSet_HXX

#include "train_type.hxx"
#include <QString>
#include <QMap>

/*! @namespace TrainSet
  
Namespace containing all TrainSet multiple units

@author K. Zarebski, Mark J.

@copyright GNU Public License v3

@date last modified 2021-02-18
*/

namespace TrainSet
{


	//! 90+MK3+DVT (9 Car)
	extern const TrainType class_90_mk3_dvt;

	//! 91+MK4+DVT (9 Car)
	extern const TrainType class_91_mk4_dvt;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_313">Class 313/0 (3 Car) TrainSet Classification</a>
	extern const TrainType class_313_0;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_313">Class 313/1 (3 Car) TrainSet Classification</a>
	extern const TrainType class_313_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_313">Class 313/2 (3 Car) TrainSet Classification</a>
	extern const TrainType class_313_2;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_314">Class 314/2 (3 Car) TrainSet Classification</a>
	extern const TrainType class_314_2;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_315">Class 315/8 (3 Car) TrainSet Classification</a>
	extern const TrainType class_315_8;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_317">Class 317/1 (4 Car) TrainSet Classification</a>
	extern const TrainType class_317_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_317">Class 317/5 (4 Car) TrainSet Classification</a>
	extern const TrainType class_317_5;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_317">Class 317/6 (4 Car) TrainSet Classification</a>
	extern const TrainType class_317_6;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_317">Class 317/7 (4 Car) TrainSet Classification</a>
	extern const TrainType class_317_7;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_317">Class 317/8 (4 Car) TrainSet Classification</a>
	extern const TrainType class_317_8;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_318_(3_Car)">Class 318 (3 Car) TrainSet Classification</a>
	extern const TrainType class_318;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_319_(4_Car)">Class 319 (4 Car) TrainSet Classification</a>
	extern const TrainType class_319;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_320">Class 320/3 (3 Car) TrainSet Classification</a>
	extern const TrainType class_320_3;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_321">Class 321/3 (4 Car) TrainSet Classification</a>
	extern const TrainType class_321_3;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_321">Class 321/4 (4 Car) TrainSet Classification</a>
	extern const TrainType class_321_4;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_322_(4_Car)">Class 322 (4 Car) TrainSet Classification</a>
	extern const TrainType class_322;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_323_(3_Car)">Class 323 (3 Car) TrainSet Classification</a>
	extern const TrainType class_323;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_325_(4_Car)">Class 325 (4 Car) TrainSet Classification</a>
	extern const TrainType class_325;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_331">Class 331/0 (3 Car) TrainSet Classification</a>
	extern const TrainType class_331_0;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_331">Class 331/1 (4 Car) TrainSet Classification</a>
	extern const TrainType class_331_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_332_(4_Car)">Class 332 (4 Car) TrainSet Classification</a>
	extern const TrainType class_332;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_333_(4_Car)">Class 333 (4 Car) TrainSet Classification</a>
	extern const TrainType class_333;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_334_(3_Car)">Class 334 (3 Car) TrainSet Classification</a>
	extern const TrainType class_334;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_345_(9_Car)">Class 345 (9 Car) TrainSet Classification</a>
	extern const TrainType class_345;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_350">Class 350/1 (4 Car) TrainSet Classification</a>
	extern const TrainType class_350_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_350">Class 350/2 (4 Car) TrainSet Classification</a>
	extern const TrainType class_350_2;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_350">Class 350/3 (4 Car) TrainSet Classification</a>
	extern const TrainType class_350_3;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_350">Class 350/4 (4 Car) TrainSet Classification</a>
	extern const TrainType class_350_4;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_357_(4_Car)">Class 357 (4 Car) TrainSet Classification</a>
	extern const TrainType class_357;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_360">Class 360/1 (4 Car) TrainSet Classification</a>
	extern const TrainType class_360_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_360">Class 360/5 (5 Car) TrainSet Classification</a>
	extern const TrainType class_360_5;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_365_(4_Car)">Class 365 (4 Car) TrainSet Classification</a>
	extern const TrainType class_365;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_373_(16_Car)">Class 373 (16 Car) TrainSet Classification</a>
	extern const TrainType class_373;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_375_(3_Car)">Class 375 (3 Car) TrainSet Classification</a>
	extern const TrainType class_375;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_376_(5_Car)">Class 376 (5 Car) TrainSet Classification</a>
	extern const TrainType class_376;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_377">Class 377/1 (4 Car) TrainSet Classification</a>
	extern const TrainType class_377_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_377">Class 377/2 (4 Car) TrainSet Classification</a>
	extern const TrainType class_377_2;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_377">Class 377/3 (3 Car) TrainSet Classification</a>
	extern const TrainType class_377_3;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_377">Class 377/4 (4 Car) TrainSet Classification</a>
	extern const TrainType class_377_4;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_377">Class 377/5 (4 Car) TrainSet Classification</a>
	extern const TrainType class_377_5;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_377">Class 377/6 (5 Car) TrainSet Classification</a>
	extern const TrainType class_377_6;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_377">Class 377/7 (5 Car) TrainSet Classification</a>
	extern const TrainType class_377_7;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_378">Class 378/0 (3 Car) TrainSet Classification</a>
	extern const TrainType class_378_0;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_378">Class 378/1 (5 Car) TrainSet Classification</a>
	extern const TrainType class_378_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_378">Class 378/2 (7 Car) TrainSet Classification</a>
	extern const TrainType class_378_2;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_379_(4_Car)">Class 379 (4 Car) TrainSet Classification</a>
	extern const TrainType class_379;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_380">Class 380/0 (3 Car) TrainSet Classification</a>
	extern const TrainType class_380_0;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_380">Class 380/1 (4 Car) TrainSet Classification</a>
	extern const TrainType class_380_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_387">Class 387/1 (4 Car) TrainSet Classification</a>
	extern const TrainType class_387_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_387">Class 387/2 (4 Car) TrainSet Classification</a>
	extern const TrainType class_387_2;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_387">Class 387/3 (4 Car) TrainSet Classification</a>
	extern const TrainType class_387_3;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_390">Class 390/0 (9 Car) TrainSet Classification</a>
	extern const TrainType class_390_0;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_390">Class 390/1 (11 Car) TrainSet Classification</a>
	extern const TrainType class_390_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_395_(6_Car)">Class 395 (6 Car) TrainSet Classification</a>
	extern const TrainType class_395;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_399_(3_Car)">Class 399 (3 Car) TrainSet Classification</a>
	extern const TrainType class_399;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_442_(5_Car)">Class 442 (5 Car) TrainSet Classification</a>
	extern const TrainType class_442;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_444_(5_Car)">Class 444 (5 Car) TrainSet Classification</a>
	extern const TrainType class_444;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_450_(4_Car)">Class 450 (4 Car) TrainSet Classification</a>
	extern const TrainType class_450;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_455_(4_Car)">Class 455 (4 Car) TrainSet Classification</a>
	extern const TrainType class_455;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_456_(2_Car)">Class 456 (2 Car) TrainSet Classification</a>
	extern const TrainType class_456;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_458">Class 458/0 (4 Car) TrainSet Classification</a>
	extern const TrainType class_458_0;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_458">Class 458/5 (5 Car) TrainSet Classification</a>
	extern const TrainType class_458_5;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_460_(8_Car)">Class 460 (8 Car) TrainSet Classification</a>
	extern const TrainType class_460;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_465_(4_Car)">Class 465 (4 Car) TrainSet Classification</a>
	extern const TrainType class_465;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_466_(2_Car)">Class 466 (2 Car) TrainSet Classification</a>
	extern const TrainType class_466;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_482_(2_Car)">Class 482 (2 Car) TrainSet Classification</a>
	extern const TrainType class_482;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_483_(3_Car)">Class 483 (3 Car) TrainSet Classification</a>
	extern const TrainType class_483;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_507_(3_Car)">Class 507 (3 Car) TrainSet Classification</a>
	extern const TrainType class_507;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_508_(3_Car)">Class 508 (3 Car) TrainSet Classification</a>
	extern const TrainType class_508;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_700_(8_Car)">Class 700 (8 Car) TrainSet Classification</a>
	extern const TrainType class_700;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_707_(5_Car)">Class 707 (5 Car) TrainSet Classification</a>
	extern const TrainType class_707;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_717_(6_Car)">Class 717 (6 Car) TrainSet Classification</a>
	extern const TrainType class_717;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_801_(5_Car)">Class 801 (5 Car) TrainSet Classification</a>
	extern const TrainType class_801;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_139_(1_Car)">Class 139 (1 Car) TrainSet Classification</a>
	extern const TrainType class_139;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_142_(2_Car)">Class 142 (2 Car) TrainSet Classification</a>
	extern const TrainType class_142;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_143">Class 143/0 (2 Car) TrainSet Classification</a>
	extern const TrainType class_143_0;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_143">Class 143/3 (2 Car) TrainSet Classification</a>
	extern const TrainType class_143_3;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_143">Class 143/6 (2 Car) TrainSet Classification</a>
	extern const TrainType class_143_6;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_144_(2_Car)">Class 144 (2 Car) TrainSet Classification</a>
	extern const TrainType class_144;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_150">Class 150/0 (3 Car) TrainSet Classification</a>
	extern const TrainType class_150_0;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_150">Class 150/1 (2 Car) TrainSet Classification</a>
	extern const TrainType class_150_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_150">Class 150/2 (2 Car) TrainSet Classification</a>
	extern const TrainType class_150_2;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_150">Class 150/9 (3 Car) TrainSet Classification</a>
	extern const TrainType class_150_9;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_153_(1_Car)">Class 153 (1 Car) TrainSet Classification</a>
	extern const TrainType class_153;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_155_(2_Car)">Class 155 (2 Car) TrainSet Classification</a>
	extern const TrainType class_155;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_156_(2_Car)">Class 156 (2 Car) TrainSet Classification</a>
	extern const TrainType class_156;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_158_(2_Car)">Class 158 (2 Car) TrainSet Classification</a>
	extern const TrainType class_158;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_159_(3_Car)">Class 159 (3 Car) TrainSet Classification</a>
	extern const TrainType class_159;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_165">Class 165/0 (2 Car) TrainSet Classification</a>
	extern const TrainType class_165_0;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_165">Class 165/1 (2 Car) TrainSet Classification</a>
	extern const TrainType class_165_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_166_(3_Car)">Class 166 (3 Car) TrainSet Classification</a>
	extern const TrainType class_166;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_168_(3_Car)">Class 168 (3 Car) TrainSet Classification</a>
	extern const TrainType class_168;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_170">Class 170/1 (2 Car) TrainSet Classification</a>
	extern const TrainType class_170_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_170">Class 170/2 (2 Car) TrainSet Classification</a>
	extern const TrainType class_170_2;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_170">Class 170/3 (3 Car) TrainSet Classification</a>
	extern const TrainType class_170_3;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_170">Class 170/4 (3 Car) TrainSet Classification</a>
	extern const TrainType class_170_4;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_170">Class 170/5 (2 Car) TrainSet Classification</a>
	extern const TrainType class_170_5;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_170">Class 170/6 (3 Car) TrainSet Classification</a>
	extern const TrainType class_170_6;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_171">Class 171/7 (2 Car) TrainSet Classification</a>
	extern const TrainType class_171_7;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_171">Class 171/8 (4 Car) TrainSet Classification</a>
	extern const TrainType class_171_8;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_172">Class 172/0 (2 Car) TrainSet Classification</a>
	extern const TrainType class_172_0;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_172">Class 172/1 (2 Car) TrainSet Classification</a>
	extern const TrainType class_172_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_172">Class 172/2 (2 Car) TrainSet Classification</a>
	extern const TrainType class_172_2;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_172">Class 172/3 (3 Car) TrainSet Classification</a>
	extern const TrainType class_172_3;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_175">Class 175/0 (2 Car) TrainSet Classification</a>
	extern const TrainType class_175_0;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_175">Class 175/1 (3 Car) TrainSet Classification</a>
	extern const TrainType class_175_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_180_*Zephyr*_(5_Car)">Class 180 *Zephyr* (5 Car) TrainSet Classification</a>
	extern const TrainType class_180_zephyr;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_185_*Pennine*_(3_Car)">Class 185 *Pennine* (3 Car) TrainSet Classification</a>
	extern const TrainType class_185_pennine;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_220_*Voyager*_(4_Car)">Class 220 *Voyager* (4 Car) TrainSet Classification</a>
	extern const TrainType class_220_voyager;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_221_*Super_Voyager*_(4_Car)">Class 221 *Super Voyager* (4 Car) TrainSet Classification</a>
	extern const TrainType class_221_super_voyager;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_222">Class 222/0 *Meridian* (5 Car) TrainSet Classification</a>
	extern const TrainType class_222_0_meridian;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_195">Class 195/0 (2 Car) TrainSet Classification</a>
	extern const TrainType class_195_0;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_195">Class 195/1 (3 Car) TrainSet Classification</a>
	extern const TrainType class_195_1;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_230_(2_Car)">Class 230 (2 Car) TrainSet Classification</a>
	extern const TrainType class_230;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_800">Class 800/1 *IET* (5 Car) TrainSet Classification</a>
	extern const TrainType class_800_1_iet;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_800">Class 800/1 *Azuma* (5 Car) TrainSet Classification</a>
	extern const TrainType class_800_1_azuma;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_800">Class 800/2 *IET* (9 Car) TrainSet Classification</a>
	extern const TrainType class_800_2_iet;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_802_*IET*_(5_Car)">Class 802 *IET* (5 Car) TrainSet Classification</a>
	extern const TrainType class_802_iet;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_802_*Paragon*_(5_Car)">Class 802 *Paragon* (5 Car) TrainSet Classification</a>
	extern const TrainType class_802_paragon;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_802_*Nova*_(5_Car)">Class 802 *Nova* (5 Car) TrainSet Classification</a>
	extern const TrainType class_802_nova;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_68+MK5+DVT_*Nova*_(5_Car)">Class 68+MK5+DVT *Nova* (5 Car) TrainSet Classification</a>
	extern const TrainType class_68_mk5_dvt_nova;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_67+MK3+DVT_(4_Car)">Class 67+MK3+DVT (4 Car) TrainSet Classification</a>
	extern const TrainType class_67_mk3_dvt;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_43_HST_*Castle*">Class 43 HST *Castle* TrainSet Classification</a>
	extern const TrainType class_43_hst_castle;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_43_HST_(2+6)">Class 43 HST (2+6) TrainSet Classification</a>
	extern const TrainType class_43_hst_2_6_;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_43_HST_(2+7)">Class 43 HST (2+7) TrainSet Classification</a>
	extern const TrainType class_43_hst_2_7_;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_43_HST_(2+8)">Class 43 HST (2+8) TrainSet Classification</a>
	extern const TrainType class_43_hst_2_8_;

    //! <a href="https://en.wikipedia.org/wiki/British_Rail_Class_43_HST_(2+9)">Class 43 HST (2+9) TrainSet Classification</a>
	extern const TrainType class_43_hst_2_9_;


//------------ Subway Rolling Stock ----------------//

	extern const TrainType lu_1972ts;

	extern const TrainType lu_1973ts;

	extern const TrainType lu_1992ts;

	extern const TrainType lu_1995ts;

	extern const TrainType lu_1996ts;

	extern const TrainType lu_2009ts;

	extern const TrainType lu_a60_a62_stock;

	extern const TrainType lu_d78_stock;

	extern const TrainType lu_c69_c77_stock;

	extern const TrainType lu_s7_stock;

	extern const TrainType lu_s8_stock;

	extern const TrainType spt_g_stock;

    extern const QMap<QString, const TrainType*> TrainSet;
};

#endif // TrainSet_HXX
