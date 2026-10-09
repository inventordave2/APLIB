// APLIB_EXTRA_TYPEDEFS_H

#ifndef APLIB_EXTRA_TYPEDEFS_H
#define APLIB_EXTRA_TYPEDEFS_H

extern AP e; // Euler's Constant.
extern AP GR; // The "Golden Ratio".
extern AP FGC; // The "Fine-Grain Constant." ( ~ 1/137 )
extern AP AP0;
extern AP AP1;
extern AP AP2;
extern AP AP3;
extern AP AP10;
extern AP AP16;
extern AP AP1_2;
extern AP AP1_3;
extern AP AP1_4;
extern AP AP1_5;
extern AP AP3_4;
extern AP AP4_5;
extern AP AP2_3;
extern AP AP1_7;
extern AP AP2_7;
extern AP AP22_7;
extern AP AP_PI;


typedef struct Planck   {

	AP Time;
	AP Length;
	AP Mass;
	AP Energy;

	AP Constant;
	AP ReducedConstant;

} Planck;
extern struct Planck* planck;
typedef struct aplib_rational_t {

	int n;
	int m;

} aplib_rational_t;
#define rational_t aplib_rational_t

typedef struct aplib_result_t {

	double r;
	struct aplib_rational_t n_m;

} aplib_result_t;
#define result_t aplib_result_t


/**
struct vector_t (*vec)( struct extern AP t, struct AP x, struct AP y, struct AP z, struct AP k );
struct point_t (*point)( struct AP t, struct AP x, struct AP y, struct AP z, struct AP k );
struct circle_t (*circle)( struct point_t* origin, struct AP radius, struct vector_t* momentum, struct AP* segments );

struct triangle_t (*triangulate)( struct AP opp, struct AP adj, struct AP hyp, struct AP hyp_adj_angle, struct AP adj_opp_angle, struct AP hyp_opp_angle );
// any of these individual ptr args can be defined as NULL on a givn fnc invocation,
// but at least 3 non-NULL operands need to be present, and correct.
*/

typedef struct angle_t	{

	AP v;
	int type; // radians or degrees, etc

	int clockwise;

} angle_t;
typedef struct vector_t	{

	AP t;
	AP x;
	AP y;
	AP z;
	AP k;

	int type;

} vector_t;
#define Angle struct angle_t
#define Vector struct vector_t

typedef struct circle_t	{

	AP radius;
	Vector* origin;
	AP segments;

	int type;
} circle_t;
typedef struct triangle_t {

	AP opp;
	AP adj;
	AP hyp;

	Angle* hyp_adj_angle;
	Angle* hyp_opp_angle;
	Angle* opp_adj_angle;

	int type; // isoscoles, equilateral, rightangle
} triangle_t;
#define Circle struct circle_t
#define Triangle struct triangle_t



#endif
