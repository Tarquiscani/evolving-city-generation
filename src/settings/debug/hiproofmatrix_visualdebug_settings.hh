#ifndef GM_VISUALDEBUG_HIPROOFMATRIX_SETTINGS_HH
#define GM_VISUALDEBUG_HIPROOFMATRIX_SETTINGS_HH


#include "debug_settings.hh"


////
//	It enables the Visual Debug of the Hip Roof Generation Algorithm.
//  There are multiple sub-options to visually inspect different parts of the algorithm. This is because the
//  Visual Debug records every tiny step of the algorithm and keeps it in memory until the game is closed. 
//  It quickly fills up the RAM when everything is enabled.
//  Only effective when VISUALDEBUG is active.
////
#define VISUALDEBUG_OPT_HIPROOFMATRIX true

////
//	It enables the Visual Debug to record some particular steps of the Hip Roof Generation Algorithm, those
//  related to the projection of the roof shape edges on the surface.
//  Only effective when VISUALDEBUG and VISUALDEBUG_OPT_HIPROOFMATRIX are active.
////
#define VISUALDEBUG_OPT_HIPROOFMATRIX_OPT_PROJECT_BORDERS true

////
//	It enables the Visual Debug to record some particular steps of the Hip Roof Generation Algorithm, those
//  that computes the polygons starting from the discrete heightmap of the roof.
//  Only effective when VISUALDEBUG and VISUALDEBUG_OPT_HIPROOFMATRIX are active.
////
#define VISUALDEBUG_OPT_HIPROOFMATRIX_OPT_COMPUTE_POLYGONS true



////
//  IMPLEMENTATION
//  Do not touch.
////
#define HIPROOFMATRIX_VISUALDEBUG                   (VISUALDEBUG && VISUALDEBUG_OPT_HIPROOFMATRIX)
#define HIPROOFMATRIX_VISUALDEBUG_PROJECT_BORDERS	(HIPROOFMATRIX_VISUALDEBUG && VISUALDEBUG_OPT_HIPROOFMATRIX_OPT_PROJECT_BORDERS)
#define HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS	(HIPROOFMATRIX_VISUALDEBUG && VISUALDEBUG_OPT_HIPROOFMATRIX_OPT_COMPUTE_POLYGONS)



#endif //GM_VISUALDEBUG_HIPROOFMATRIX_SETTINGS_HH