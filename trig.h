#ifndef DAVELIB_TRIG_H
#define DAVELIB_TRIG_H

typedef struct vector2d  {

  double x;
  double y;

} vector2d;


typedef struct triangle_side_t    {

    vector2d v1;
    vector2d v2;

} triangle_side_t;


typedef struct triangle_t    {

    vector2d A;
    vector2d B;
    vector2d C;

    double hyp;
    double opp;
    double adj;

    double sine_;
    double cos_;
    double tan_;

    double a;
    double b;
    double c; // (1/2) * PI in a r.a. triangle.

} triangle_t;

#define Triangle struct triangle_t

extern double generate_half_pi(void);
extern double deg_to_rad(double degrees, double half_pi);
extern double rad_to_deg(double radians, double half_pi);


extern double sine( double x, int terms );
extern double cosine( double x, int terms );

extern double tangent( double sine_, double cos_ );
extern double tangent_alt( double angle_in_radians, int precision );

extern double angle( double tan_ );
extern double atan_dave( double tan_ );
extern double divide_and_conquer_angle( double sine_, double cos_ );

extern double root2( double a );


extern struct triangle_t triangle_from_hyp_vector( triangle_side_t hypoteneuse );
extern struct triangle_t triangle_from_sides_vectors( triangle_side_t hypoteneuse, triangle_side_t opp_side, triangle_side_t adjacent_side );



#define triangle_from_opp_adj(opp,adj) Triangle T; T.hyp = root2((opp*opp)+(adj*adj)); T.sine_ = T.opp/T.hyp; T.cos_ = T.adj/T.hyp; T.tan_ = T.opp/T.adj; T.a = angle( T.sine_, T.cos_ ); T.c = generate_half_pi(); T.b = T.c - T.a; (T);
#define triangle_from_opp_adj_2(opp,adj) vector2d A; vector2d B; vector2d C; A.x = 0.0; A.y = 0.0; B.x = adj; B.y = opp; C.x = adj; C.y = 0.0; 


#endif

