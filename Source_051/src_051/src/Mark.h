/*
Copyright (c) 2000-2003, Jelle Kok, University of Amsterdam
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the University of Amsterdam nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*! \file Geometry.h
<pre>
<b>File:</b>          Geometry.h
<b>Project:</b>       Robocup Soccer Simulation Team: UvA Trilearn
<b>Authors:</b>       Jelle Kok
<b>Created:</b>       12/02/2001
<b>Last Revision:</b> $ID$
<b>Contents:</b>      Header file for the classes VecPosition, Geometry, Line,
               Circle and Rectangle. All the member
               data and member method declarations for all these classes can be
               found in this file together with some auxiliary functions for
               numeric and goniometric purposes.
<hr size=2>
<h2><b>Changes</b></h2>
<b>Date</b>             <b>Author</b>          <b>Comment</b>
12/02/2001       Jelle Kok       Initial version created
09/06/2001       Remco de Boer   Version including full documentation completed
</pre>
*/

#ifndef _MARK_
#define _MARK_

#include "math.h"       // needed for M_PI constant
#include <string>       // needed for string
#include <iostream>
#include <rcsc/geom/vector_2d.h>
#include <rcsc/geom/line_2d.h>
using namespace std;
using namespace rcsc;

typedef double AngRad;  /*!< Type definition for angles in radians. */
typedef double AngDeg;  /*!< Type definition for angles in degrees. */

#define EPSILON 0.0001  /*!< Value used for floating point equality tests. */

// auxiliary numeric functions for determining the
// maximum and minimum of two given double values and the sign of a value
double max     ( double d1, double d2 );
double min     ( double d1, double d2 );
//int    sign    ( double d1            );

// auxiliary goniometric functions which enable you to
// specify angles in degrees rather than in radians
AngDeg Rad2Deg ( AngRad x             );
AngRad Deg2Rad ( AngDeg x             );
double cosDeg  ( AngDeg x             );
double sinDeg  ( AngDeg x             );
double tanDeg  ( AngDeg x             );
AngDeg atanDeg ( double x             );
double atan2Deg( double x,  double y  );
AngDeg acosDeg ( double x             );
AngDeg asinDeg ( double x             );

// various goniometric functions
bool   isAngInInterval     ( AngDeg ang,    AngDeg angMin,    AngDeg angMax );
AngDeg getBisectorTwoAngles( AngDeg angMin, AngDeg angMax );

/*! CoordSystem is an enumeration of the different specified
    coordinate systems.  The two possibilities are CARTESIAN or
    POLAR. These values are for instance used in the initializing a
    VecPosition. The CoordSystem indicates whether the supplied
    arguments represent the position in cartesian or in polar
    coordinates. */
enum CoordSystemT {
  CARTESIAN,
  POLAR
};


/*****************************************************************************/
/********************   CLASS VECPOSITION   **********************************/
/*****************************************************************************/

/*! This class contains an x- and y-coordinate of a position (x,y) as
    member data and methods which operate on this position. The
    standard arithmetic operators are overloaded and can thus be
    applied to positions (x,y). It is also possible to represent a
    position in polar coordinates (r,phi), since the class contains a
    method to convert these into cartesian coordinates (x,y). */
class VecPosition
{
  // private member data
private:

  double m_x;   /*!< x-coordinate of this position */
  double m_y;   /*!< y-coordinate of this position */

  // public methods
public:
  // constructor for VecPosition class
  VecPosition                               ( double            vx = 0,
                                              double            vy = 0,
                                              CoordSystemT      cs =CARTESIAN);

  // overloaded arithmetic operators
  VecPosition        operator -             (                                );
  VecPosition        operator +             ( const double      &d           );
  VecPosition        operator +             ( const VecPosition &p           );
  VecPosition        operator -             ( const double      &d           );
  VecPosition        operator -             ( const VecPosition &p           );
  VecPosition        operator *             ( const double      &d           );
  VecPosition        operator *             ( const VecPosition &p           );
  VecPosition        operator /             ( const double      &d           );
  VecPosition        operator /             ( const VecPosition &p           );
  void               operator =             ( const double      &d           );
  void               operator +=            ( const VecPosition &p           );
  void               operator +=            ( const double      &d           );
  void               operator -=            ( const VecPosition &p           );
  void               operator -=            ( const double      &d           );
  void               operator *=            ( const VecPosition &p           );
  void               operator *=            ( const double      &d           );
  void               operator /=            ( const VecPosition &p           );
  void               operator /=            ( const double      &d           );
  bool               operator !=            ( const VecPosition &p           );
  bool               operator !=            ( const double      &d           );
  bool               operator ==            ( const VecPosition &p           );
  bool               operator ==            ( const double      &d           );

  // methods for producing output
  friend ostream&    operator <<            ( ostream           &os,
                                              VecPosition       p            );
  void               show                   ( CoordSystemT      cs =CARTESIAN);
  string             str                    ( CoordSystemT      cs =CARTESIAN);

  // set- and get methods for private member variables
  bool               setX                   ( double            dX           );
  double             getX                   (                          ) const;
  double             x                   (                          ) const;
  bool               setY                   ( double            dY           );
  double             getY                   (                          ) const;
  double             y                   (                          ) const;

  // set- and get methods for derived position information
  void               setVecPosition         ( double            dX = 0,
                                              double            dY = 0,
                                              CoordSystemT      cs =CARTESIAN);
  double             getDistanceTo          ( const VecPosition p            );
  VecPosition        setMagnitude           ( double            d            );
  double             getMagnitude           (                          ) const;
  AngDeg             getDirection           (                          ) const;

  // comparison methods for positions
  bool               isInFrontOf            ( const VecPosition &p           );
  bool               isInFrontOf            ( const double      &d           );
  bool               isBehindOf             ( const VecPosition &p           );
  bool               isBehindOf             ( const double      &d           );
  bool               isLeftOf               ( const VecPosition &p           );
  bool               isLeftOf               ( const double      &d           );
  bool               isRightOf              ( const VecPosition &p           );
  bool               isRightOf              ( const double      &d           );
  bool               isBetweenX             ( const VecPosition &p1,
                                              const VecPosition &p2          );
  bool               isBetweenX             ( const double      &d1,
                                              const double      &d2          );
  bool               isBetweenY             ( const VecPosition &p1,
                                              const VecPosition &p2          );
  bool               isBetweenY             ( const double      &d1,
                                              const double      &d2          );
  bool               isBetweenXY             ( const VecPosition &p1,
                                              const VecPosition &p2          );

  // conversion methods for positions
  VecPosition        normalize              (                                );
  VecPosition        rotate                 ( AngDeg            angle        );
  VecPosition        globalToRelative       ( VecPosition       orig,
                                              AngDeg            ang          );
  VecPosition        relativeToGlobal       ( VecPosition       orig,
                                              AngDeg            ang          );
  VecPosition        getVecPositionOnLineFraction( VecPosition  &p,
                                              double            dFrac        );

  // static class methods
  static VecPosition getVecPositionFromPolar( double            dMag,
                                              AngDeg            ang          );
  static AngDeg      normalizeAngle         ( AngDeg            angle        );
  double	disToLine(VecPosition	a,VecPosition	b,double &dirTo);

  static double disToLine(Vector2D c,Vector2D	a,Vector2D	b,double &dirTo){


          Line2D line(a,b);


          Vector2D d=line.projection(c);

          if(d.isBetweenXY(a,b)){
              dirTo=(d-c).dir().degree();
              return c.dist(d);
          }
          else if(d.dist(a)>d.dist(b)){
              dirTo=(b-c).dir().degree();
              return c.dist(b);
          }
          else{
              dirTo=(a-c).dir().degree();
              return c.dist(a);
          }

  }


};

/*****************************************************************************/
/*********************   CLASS GEOMETRY   ************************************/
/*****************************************************************************/

/*! This class contains several static methods dealing with geometry.*/
class Geometry
{

public:

  // geometric series
  static double getLengthGeomSeries(double dFirst,double dRatio,double dSum  );
  static double getSumGeomSeries   (double dFirst,double dRatio,double dLen  );
  static double getSumInfGeomSeries(double dFirst,double dRatio              );
  static double getFirstGeomSeries (double dSum,  double dRatio,double dLen  );
  static double getFirstInfGeomSeries(double dSum,double dRatio              );

  // abc formula
  static int    abcFormula(double a,double b, double c, double *s1,double *s2);
};

/*****************************************************************************/
/********************** CLASS CIRCLE *****************************************/
/*****************************************************************************/

/*!This class represents a circle. A circle is defined by one VecPosition
   (which denotes the center) and its radius. */
class Circle
{
    VecPosition m_posCenter;            /*!< Center of the circle  */
    double      m_dRadius;              /*!< Radius of the circle  */

public:
    Circle( );
    Circle( VecPosition pos, double dR );

    void        show                  ( ostream& os = cout );

    // get and set methods
    bool        setCircle             ( VecPosition pos,
                                        double      dR  );
    bool        setRadius             ( double dR       );
    double      getRadius             (                 );
    bool        setCenter             ( VecPosition pos );
    VecPosition getCenter             (                 );
    double      getCircumference      (                 );
    double      getArea               (                 );

    // calculate intersection points and area with other circle
    bool        isInside              ( VecPosition pos );
    int         getIntersectionPoints ( Circle      c,
                                        VecPosition *p1,
                                        VecPosition *p2 );
    double      getIntersectionArea   ( Circle c        );


}  ;

/*****************************************************************************/
/*********************** CLASS LINE ******************************************/
/*****************************************************************************/

/*!This class contains the representation of a line. A line is defined
   by the formula ay + bx + c = 0. The coefficients a, b and c are stored
   and used in the calculations. */
class Line
{
  // a line is defined by the formula: ay + bx + c = 0
  double m_a; /*!< This is the a coefficient in the line ay + bx + c = 0 */
  double m_b; /*!< This is the b coefficient in the line ay + bx + c = 0 */
  double m_c; /*!< This is the c coefficient in the line ay + bx + c = 0 */

public:
  Line( double a, double b, double c );

  // print methods
  void        show( ostream& os = cout );
  friend      ostream& operator << (ostream & os, Line l);

  // get intersection points with this line
  VecPosition getIntersection            ( Line        line                  );
  int         getCircleIntersectionPoints( Circle      circle,
                                           VecPosition *posSolution1,
                                           VecPosition *posSolution2         );
  Line        getTangentLine             ( VecPosition pos                   );
  VecPosition getPointOnLineClosestTo    ( VecPosition pos                   );
  double      getDistanceWithPoint       ( VecPosition pos                   );
  bool        isInBetween                ( VecPosition pos,
                                           VecPosition point1,
                                           VecPosition point2                );

  // calculate associated variables in the line
  double      getYGivenX                 ( double      x );
  double      getXGivenY                 ( double      y );
  double      getACoefficient            (               ) const;
  double      getBCoefficient            (               ) const;
  double      getCCoefficient            (               ) const;

  // static methods to make a line using an easier representation.
  static Line makeLineFromTwoPoints      ( VecPosition pos1,
                                           VecPosition pos2                  );
  static Line makeLineFromPositionAndAngle( VecPosition vec,
                                           AngDeg angle                      );
};

/*****************************************************************************/
/********************** CLASS RECTANGLE **************************************/
/******************************************************************************/
/*!This class represents a rectangle. A rectangle is defined by two
   VecPositions the one at the upper left corner and the one at the
   right bottom. */
class Rect
{
  VecPosition m_posLeftTop;     /*!< top left position of the rectangle      */
  VecPosition m_posRightBottom; /*!< bottom right position of the rectangle  */

public:
  Rect                          ( VecPosition pos, VecPosition pos2 );

  void        show              ( ostream& os = cout                );

  // checks whether point lies inside the rectangle
  bool        isInside          ( VecPosition pos                   );

  // standard get and set methosd
  void        setRectanglePoints( VecPosition pos1,
                                  VecPosition pos2                  );
  bool        setPosLeftTop     ( VecPosition pos                   );
  VecPosition getPosLeftTop     (                                   );
  bool        setPosRightBottom ( VecPosition pos                   );
  VecPosition getPosRightBottom (                                   );
};

//class Cone2D
//{
//private:
//    VecPosition m_start;
//    double      m_ang;
//    double      m_dist;

//public:
//    Cone2D ( const VecPosition & start ,
//             const double & ang ,
//             const double & dist )
//    : m_start(start) , m_ang (ang) , m_dist (dist)
//    {}

//    bool isInCone ( VecPosition rePos , const double & body_ang )
//    {
//            VecPosition dif = rePos - m_start ;
//            if ( dif.getMagnitude() < m_dist )
//            if ( dif.getDirection() > body_ang - m_ang &&
//            dif.getDirection() < body_ang + m_ang )
//            return true;
//            return false;
//    }
//};
enum ObjectT { // don't change order
  OBJECT_BALL,             /*!< Ball                     */
  OBJECT_TEAMMATE_1,       /*!< Teammate nr 1            */    // teammates 61
  OBJECT_TEAMMATE_2,       /*!< Teammate nr 2            */
  OBJECT_TEAMMATE_3,       /*!< Teammate nr 3            */
  OBJECT_TEAMMATE_4,       /*!< Teammate nr 4            */
  OBJECT_TEAMMATE_5,       /*!< Teammate nr 5            */
  OBJECT_TEAMMATE_6,       /*!< Teammate nr 6            */
  OBJECT_TEAMMATE_7,       /*!< Teammate nr 7            */
  OBJECT_TEAMMATE_8,       /*!< Teammate nr 8            */
  OBJECT_TEAMMATE_9,       /*!< Teammate nr 9            */
  OBJECT_TEAMMATE_10,      /*!< Teammate nr 10           */
  OBJECT_TEAMMATE_11,      /*!< Teammate nr 11           */
  OBJECT_TEAMMATE_UNKNOWN, /*!< Teammate nr unkown       */
  OBJECT_OPPONENT_1,       /*!< Opponent nr 1            */    // opponents 73
  OBJECT_OPPONENT_2,       /*!< Opponent nr 2            */
  OBJECT_OPPONENT_3,       /*!< Opponent nr 3            */
  OBJECT_OPPONENT_4,       /*!< Opponent nr 4            */
  OBJECT_OPPONENT_5,       /*!< Opponent nr 5            */
  OBJECT_OPPONENT_6,       /*!< Opponent nr 6            */
  OBJECT_OPPONENT_7,       /*!< Opponent nr 7            */
  OBJECT_OPPONENT_8,       /*!< Opponent nr 8            */
  OBJECT_OPPONENT_9,       /*!< Opponent nr 9            */
  OBJECT_OPPONENT_10,      /*!< Opponent nr 10           */
  OBJECT_OPPONENT_11,      /*!< Opponent nr 11           */
  OBJECT_OPPONENT_UNKNOWN, /*!< Opponent nr unknown      */    // 84
  OBJECT_TEAMMATE_GOALIE,  /*!< Goalie of your side      */
  OBJECT_OPPONENT_GOALIE,  /*!< Goalie of opponent side  */
  OBJECT_ILLEGAL,          /*!< illegal object           */
} ;

struct NewKick{

    int             cycle;
    int             step;
    rcsc::Vector2D  end;
    double          speed;
    int             reciver;
    double          M_speed;

    int             reciverUnum;
    Vector2D        reciverVector;

    NewKick(){

        reciver=0;
        step=-1;
        cycle=-1;
        end=rcsc::Vector2D(0,0);
        speed=0;
        M_speed=0;

        reciverUnum = -1;
        reciverVector = Vector2D();
    }

};

struct  spd{
    double min;
    double max;
};

////////////////////////////////////////////////////////////////////////
double doubleNormalize(double a,double min,double max);
////////////////////////////////////////////////////////////////////////
 double getAngDiff( double a , double b );
///////////////////////////////////////////////////
void doubleSwap(double &a,double &b);
////////////////////////////////////////////////////////////////////////
void vecSwap(VecPosition &a,VecPosition &b);
////////////////////////////////////////////////////////////////////////
void vecSwap(Vector2D &a,Vector2D &b);
////////////////////////////////////////////////////////////////////////
void intSwap(int &a,int &b);
////////////////////////////////////////////////////////////////////////
void boolSwap(bool &a,bool &b);
////////////////////////////////////////////////////////////////////////
void objSwap(ObjectT &a,ObjectT &b);
////////////////////////////////////////////////////////////////////////
void kickSwap(NewKick &a,NewKick &b);
////////////////////////////////////////////////////////////////////////
Vector2D    toVector(VecPosition pos);
////////////////////////////////////////////////////////////////////////
VecPosition    toVecPos(Vector2D pos);
////////////////////////////////////////////////////////////////////////
enum ReRateMode {
    RRM_DIRECT,
    RRM_REVERSE
};
float reRate(float value, float a, float b, float c, float d, ReRateMode mode = RRM_DIRECT);
////////////////////////////////////////////////////////////////////////
#endif
