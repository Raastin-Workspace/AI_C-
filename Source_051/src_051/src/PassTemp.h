//#ifndef _PASS
//#define _PASS
//#include <rcsc/player/view_mode.h>
//#include <rcsc/player/say_message_builder.h>
//#include <rcsc/common/server_param.h>
//#include <rcsc/common/logger.h>
////#include <rcsc/player/soccer_action.h>
////#include <rcsc/player/player_agent.h>
//#include <rcsc/player/debug_client.h>
//#include <rcsc/geom/vector_2d.h>
//#include <rcsc/action/kick_table.h>
//#include <rcsc/action/body_smart_kick.h>
//#include <rcsc/action/body_kick_one_step.h>
//#include <rcsc/action/body_kick_to_relative.h>
//#include <rcsc/action/neck_turn_to_point.h>
//#include <rcsc/action/body_dribble.h>
//#include <rcsc/action/body_hold_ball.h>
//#include <rcsc/action/neck_scan_field.h>
//#include <rcsc/action/neck_turn_to_low_conf_teammate.h>
//#include <rcsc/action/body_clear_ball.h>
//#include <rcsc/common/server_param.h>
//#include <rcsc/common/logger.h>
//#include <rcsc/player/debug_client.h>
//#include <rcsc/geom/vector_2d.h>
//#include <rcsc/player/intercept_table.h>
//#include <rcsc/action/kick_table.h>
//#include <rcsc/action/body_advance_ball2009.h>
//#include <rcsc/action/body_smart_kick.h>
//#include <rcsc/action/body_kick_one_step.h>
//#include <rcsc/action/body_kick_to_relative.h>
//#include <rcsc/common/server_param.h>
////#include <rcsc/common/player_type.h>
//#include <rcsc/common/logger.h>
//#include <rcsc/soccer_math.h>
//#include <rcsc/math_util.h>
//#include <algorithm>
//#include <iostream>
//#include <cmath>
//#include <vector>
//#include "strategy.h"
/////#include "bhv_pass20102.h"

//#include "Simul.h"
//using namespace std;
//using namespace rcsc;





//class MostafPass
//    : public SoccerBehavior {
//private:

//public:
//    MostafPass ( )
//    { }



//    int             getCount(const WorldModel & wm,double dir,double PM);
//    int             getCount(const WorldModel & wm,Vector2D pos,double PM);
//    double          calculate_PM(const WorldModel & wm,Vector2D start,Vector2D end,double speed,double minSpeed=0.4);

//    bool            OCI(PlayerAgent * agent,Vector2D start,Vector2D end,double speed,int minConf,double minSpeed,int step,double rand=0,bool withRand=false,int Ext=0);
//    bool            OCC(PlayerAgent * agent,Vector2D start,Vector2D end,double speed,int minConf,double minSpeed,int step,double rand=0,bool withRand=false);

//    double      oneStepRand(PlayerAgent * agent,double dPower=100);
//    double      twoStepRand(PlayerAgent * agent,Body_SmartKick kick,double dPower=100);

//    double      kickRand(PlayerAgent * agent,Body_SmartKick kick,double dPower=100);

//    bool      isInGoal(Vector2D start,Vector2D end,double speed,double rand,double Y=6.8);

//    bool    setKick(const WorldModel & wm,NewKick &kick,Body_SmartKick a);
//    Vector2D    startKick(const WorldModel &wm, Body_SmartKick a);
//    int    stepKick(Body_SmartKick a);
//    bool        SHOOOOOOT(PlayerAgent * agent,NewKick &kick);



//    double  dakheli(Vector2D a,Vector2D b){

//        return a.r()*b.r()*cosDeg(a.dir().degree()-b.dir().degree());

//    }

//    double      flagScoring(PlayerAgent * agent,Vector2D    pos,Vector2D posFrom,int minConf=10);
//    spd       getDirectSpeed(Vector2D start,Vector2D end);
//    spd       getLeadSpeed(PlayerAgent * agent,int nr,Vector2D start,Vector2D end,int step,int avans=1);
//    spd       getThroughSpeed(PlayerAgent * agent,int nr,Vector2D start,Vector2D end,int step,int &time,int avans=1);

//    bool	directPass(PlayerAgent * agent,NewKick &kicking,int &nr,bool better=1);
//    bool        isInField(Vector2D pos);
//    void        setInField(Vector2D & pos, double maxX=48);
//    int         timeToReach(double speed,double dis,int  & time);
//    bool        throughPass(PlayerAgent * agent,NewKick &kicking,int &nr,bool better=1);

//    bool        leadingPass(PlayerAgent * agent,NewKick &kicking,int &nr,bool better=1);

//    double	posScoreInFieldForUs(Vector2D pos);
//    double	getCofficient(double Ang);
//    double      FinalMostafScoring(Vector2D pos,Vector2D inform,int minimalConf);

//    Vector2D     turnPoint               ( PlayerAgent * agent,int = 2                       );
//    double        getAngBetween       ( double, double, double = 1.0 , double = 1.0   );


//    Vector2D        seen(PlayerAgent *agent);


//    Vector2D        where(PlayerAgent *agent,int ballMinConf = 5);


//    vector<Vector2D> getDir(PlayerAgent *agent,Vector2D firsPos,Vector2D endPos,int minConf = 10);

































//private:
//    #define EPSILON 0.0001
//    double Deg2Rad( double x )
//    {
//      return ( x * M_PI / 180 );
//    }
//    double cosDeg( double x )
//    {
//      return ( cos( Deg2Rad( x ) ) );
//    }

//    double sinDeg( double x )
//    {
//      return ( sin( Deg2Rad( x ) ) );
//    }

//    //double tanDeg( double x )
//    //{
//    //  return ( tan( Deg2Rad( x ) ) );
//    //}
//    double Rad2Deg( double x )
//    {
//      return ( x * 180 / M_PI );
//    }

//    //double atanDeg( double x )
//    //{
//    //  return ( Rad2Deg( atan( x ) ) );
//    //}
//    double atan2Deg( double x, double y )
//    {
//      if( fabs( x ) < EPSILON && fabs( y ) < EPSILON )
//        return ( 0.0 );

//      return ( Rad2Deg( atan2( x, y ) ) );
//    }
//    double getBisectorTwoAngles( double angMin, double angMax )
//    {
//      // separate sine and cosine part to circumvent boundary problem
//      return (
//                atan2Deg( (sinDeg( angMin) + sinDeg( angMax ) )/2.0,
//                          (cosDeg( angMin) + cosDeg( angMax ) )/2.0 ) );
//    }

//    bool isAngInInterval( double ang, double angMin, double angMax )
//    {
//      // convert all angles to interval 0..360
//      if( ( ang    + 360 ) < 360 ) ang    += 360;
//      if( ( angMin + 360 ) < 360 ) angMin += 360;
//      if( ( angMax + 360 ) < 360 ) angMax += 360;

//      if( angMin < angMax ) // 0 ---false-- angMin ---true-----angMax---false--360
//        return angMin < ang && ang < angMax ;
//      else                  // 0 ---true--- angMax ---false----angMin---true---360
//        return !( angMax < ang && ang < angMin );
//    }
//    double  getAngDiff(double a , double b){

//        return fabs( AngleDeg::normalize_angle( b - a ) );

//    }

//    double tanDeg( double x )
//    {
//      return ( tan( Deg2Rad( x ) ) );
//    }

//    enum ReRateMode {
//        RRM_DIRECT,
//        RRM_REVERSE
//    };

//    ////////////////////////////////////////////////////////////////////////
//    float reRate(float value, float a, float b, float c, float d, ReRateMode mode= RRM_DIRECT  )
//    {
//            if (a == b)
//                    return c;

//            if (mode == RRM_DIRECT)
//            {
//            if (value < a)
//                            return c;
//                    if (value > b)
//                            return d;
//                    return (value - a) / (b - a) * (d - c) + c;
//            }
//            else
//            {
//                    if (value < a)
//                            return d;
//                    if (value > b)
//                            return c;
//                    return (value - a) / (b - a) * (c - d) + d;
//            }
//    }

//    double doubleNormalize(double a,double min,double max){

//        if(a<min)
//            a=min;
//        if(a>max)
//            a=max;
//        return a;



//    }


//    double	disToLine(Vector2D pos,Vector2D	a,Vector2D	b,double &dirTo){

//            Line2D line=Line2D(a,b);
//            Vector2D c= pos ;
//            Vector2D d=line.projection(c);

//            if(d.isBetweenXY(a,b)){
//                dirTo=(d-c).dir().degree();
//                return c.dist(d);
//            }
//            else if(d.dist(a)>d.dist(b)){
//                dirTo=(b-c).dir().degree();
//                return c.dist(b);
//            }
//            else{
//                dirTo=(a-c).dir().degree();
//                return c.dist(a);
//            }

//    }


//};

//#endif
