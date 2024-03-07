#ifndef BLOCK_H
#define BLOCK_H

#include <rcsc/geom/vector_2d.h>
#include <rcsc/player/soccer_action.h>
#include"Simul.h"
#include <rcsc/action/body_go_to_point.h>

#define DIST_DEFAULT 10
#define ANG_DEFAULT 20
#define STEP_DEFAULT 3
#define DIST_FRONT_DEFUALT 20

using namespace rcsc;


struct  block{

    Vector2D    end;
    int         plusT;
    int         cycle;
};
static block NewBlock;

class Bhv_Block
    : public rcsc::SoccerBehavior {
private:

    //double m_dist ;
   // double m_ang ;
    //double m_step ;


public:

    double  getAngDiff(double a , double b);

    int             ttTimeToGetBall ( PlayerAgent * agent ,int, Vector2D = Vector2D( -1000, -1000 ), Vector2D vellBall = Vector2D( -1000, -1000 ) );
    int             oppTimeToGetBall( PlayerAgent * agent ,int, Vector2D = Vector2D( -1000, -1000 ), Vector2D vellBall = Vector2D( -1000, -1000 ) );
    int             timeToGetBall   ( PlayerAgent * agent ,const AbstractPlayerObject * , Vector2D = Vector2D( -1000, -1000 ), Vector2D vellBall = Vector2D( -1000, -1000 ) );

    int     getBlocker  ( PlayerAgent * agent , Vector2D & posBlock                                     );



double          getOppDribbleAngle      ( PlayerAgent * agent ,Vector2D                         );
double          getOppDribbleSpeed      ( Vector2D                         );
bool            shouldBlock             ( PlayerAgent * agent                                     );
bool   doBlock                 ( PlayerAgent * agent                                     );
bool   closingOnOpp            ( PlayerAgent * agent                                     );
Vector2D closingOnOppPos            ( PlayerAgent * agent                                     );
bool   possessingTheBall       ( PlayerAgent * agent                                     );
bool            isDangerArea            ( Vector2D                         );
bool            shouldCheckBall         ( PlayerAgent * agent                                     );
bool    setBlock(PlayerAgent * agent,Vector2D end,int T);









// tackle

double          ballSpeedAfterTackle    ( PlayerAgent * agent ,double                              );
double          ballDirAfterTackle      ( PlayerAgent * agent ,double                              );
double          ballSpeedAfterTackle    ( PlayerAgent * agent ,Vector2D                         );
double          ballDirAfterTackle      ( PlayerAgent * agent ,Vector2D                         );
bool            shouldTackle            ( PlayerAgent * agent ,double &, bool &                    );
bool            canGetBallNextCycle     ( PlayerAgent * agent                                      );
double          getTackleRelAng         ( PlayerAgent * agent                                      );
bool            tackleToPos             ( PlayerAgent * agent ,Vector2D, SimulateAction &        );
bool            isBallHeadingToGoal     ( PlayerAgent * agent                                      );
bool            isBetween           ( double , double , double                      );

bool		isBallTheirs        ( PlayerAgent * agent ,double = 0.7, Vector2D = Vector2D( -1000, -1000 ) );
bool		isBallOurs          ( PlayerAgent * agent ,bool b = false                                );
int getOppInCircle(PlayerAgent * agent ,Circle2D cir);
int getTtInCircle(PlayerAgent * agent ,Circle2D cir);
bool    goTo(PlayerAgent *agent,Vector2D posTo,double bodyAng,double maxDashPower,double disWhenTrue=0.5);


bool tackle( rcsc::PlayerAgent * agent );
bool bTackle( rcsc::PlayerAgent * agent );



#define EPSILON 0.0001
double Deg2Rad( double x )
{
  return ( x * M_PI / 180 );
}
double cosDeg( double x )
{
  return ( cos( Deg2Rad( x ) ) );
}

double sinDeg( double x )
{
  return ( sin( Deg2Rad( x ) ) );
}

//double tanDeg( double x )
//{
//  return ( tan( Deg2Rad( x ) ) );
//}
double Rad2Deg( double x )
{
  return ( x * 180 / M_PI );
}

//double atanDeg( double x )
//{
//  return ( Rad2Deg( atan( x ) ) );
//}
double atan2Deg( double x, double y )
{
  if( fabs( x ) < EPSILON && fabs( y ) < EPSILON )
    return ( 0.0 );

  return ( Rad2Deg( atan2( x, y ) ) );
}
double getBisectorTwoAngles( double angMin, double angMax )
{
  // separate sine and cosine part to circumvent boundary problem
  return (
            atan2Deg( (sinDeg( angMin) + sinDeg( angMax ) )/2.0,
                      (cosDeg( angMin) + cosDeg( angMax ) )/2.0 ) );
}

bool isAngInInterval( double ang, double angMin, double angMax )
{
  // convert all angles to interval 0..360
  if( ( ang    + 360 ) < 360 ) ang    += 360;
  if( ( angMin + 360 ) < 360 ) angMin += 360;
  if( ( angMax + 360 ) < 360 ) angMax += 360;

  if( angMin < angMax ) // 0 ---false-- angMin ---true-----angMax---false--360
    return angMin < ang && ang < angMax ;
  else                  // 0 ---true--- angMax ---false----angMin---true---360
    return !( angMax < ang && ang < angMin );
}


};

#endif
