////      bhv_best_dribble.h
////
////      Copyright 2009 saeed <saeed@saeed>
////
////      This program is free software; you can redistribute it and/or modify
////      it under the terms of the GNU General Public License as published by
////      the Free Software Foundation; either version 2 of the License, or
////      (at your option) any later version.
////
////      This program is distributed in the hope that it will be useful,
////      but WITHOUT ANY WARRANTY; without even the implied warranty of
////      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
////      GNU General Public License for more details.
////
////      You should have received a copy of the GNU General Public License
////      along with this program; if not, write to the Free Software
////      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
////      MA 02110-1301, USA.


///////////////////////////////////////////////////////////////////////

//#ifndef BHV_SMART_DRIBBLE_H
//#define BHV_SMART_DRIBBLE_H

//#include <rcsc/geom/vector_2d.h>
//#include <rcsc/player/soccer_action.h>

//#define DIST_DEFAULT 10
//#define DIST_DEFAULT_PENALTY 5
//#define ANG_DEFAULT 55
//#define STEP_DEFAULT 3
//#define DIST_FRONT_DEFUALT 20

//class Bhv_SmartDribble
//    : public rcsc::SoccerBehavior {
//private:

//    double m_dist ;
//    double m_ang ;
//    double m_step ;

//public:



//    Bhv_SmartDribble ( double dist , double ang , double step ) :
//    m_dist ( dist ) , m_ang ( ang ) , m_step ( step ) {}
//        double getDribbleLen(rcsc::PlayerAgent * agent, int num , bool isTeammate);
//    Bhv_SmartDribble ( ) :
//    m_dist ( DIST_DEFAULT ) , m_ang ( ANG_DEFAULT ) , m_step ( STEP_DEFAULT ) {}

//    bool execute( rcsc::PlayerAgent * agent );
//    rcsc::Vector2D getDribblePos (rcsc::PlayerAgent * agent) ;
//    rcsc::Vector2D getDribblePos1 (rcsc::PlayerAgent * agent) ;
//    rcsc::Vector2D getClosetsetTeam(rcsc::PlayerAgent *agent);

//double getLenght( rcsc::PlayerAgent * agent , double ang , int cycle ,double *dist) ;

//bool bWithDodge( rcsc::PlayerAgent * agent ) ;
//bool noDodge(rcsc::PlayerAgent * agent);
//double getDribbleLen(rcsc::PlayerAgent * agent ,int num,double ang, rcsc::Vector2D myPos , rcsc::Vector2D oppPos );
//double getDirectionOfWidestAngle(rcsc::PlayerAgent * agent , double a, double b,int cycle);
//bool canDribbleFast(rcsc::PlayerAgent * agent , rcsc::Vector2D *pos , double *speed);

//};

#ifndef SMART_DRIBBLE_H
#define SMART_DRIBBLE_H
#include <rcsc/player/soccer_intention.h>

#include <rcsc/geom/vector_2d.h>
#include <rcsc/player/soccer_action.h>

#include "rcsc/action/body_smart_kick.h"
#include "rcsc/action/body_intercept2009.h"
#include <rcsc/action/basic_actions.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/neck_turn_to_ball_or_scan.h>
#include <rcsc/action/body_dribble.h>
#include <rcsc/action/body_hold_ball.h>
#include <rcsc/action/neck_scan_field.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>
//#include "neck_offensive_intercept_neck.h"
#include <rcsc/geom/ray_2d.h>
#include <rcsc/soccer_math.h>
#include <rcsc/math_util.h>
#include <rcsc/player/soccer_intention.h>
#define DIST_DEFAULT 10
#define DIST_DEFAULT_PENALTY 5
#define ANG_DEFAULT 55
#define STEP_DEFAULT 3
#define DIST_FRONT_DEFUALT 20

class IntentionSelfPass
    : public rcsc::SoccerIntention {
private:

    const rcsc::Vector2D M_target_point;
    int M_step;
    int M_count;

public:

    IntentionSelfPass( const rcsc::Vector2D & target_point,
                       const int step )
        : M_target_point( target_point )
        , M_step( step )
        , M_count( 0 )
      { }

    bool finished( const rcsc::PlayerAgent * agent );

    bool execute( rcsc::PlayerAgent * agent );

};

class Bhv_SelfPass
    : public rcsc::SoccerBehavior {
private:

public:

    bool execute2( rcsc::PlayerAgent * agent );
    bool execute( rcsc::PlayerAgent * agent );

private:

    bool doKickDashes( rcsc::PlayerAgent * agent );
    bool doKickTurnDashes( rcsc::PlayerAgent * agent,
                           const rcsc::Vector2D & target_point );

    void createSelfCache( rcsc::PlayerAgent * agent,
                          const int n_turn,
                          const int n_dash,
                          const rcsc::AngleDeg & accel_angle,
                          std::vector< rcsc::Vector2D > & self_state );
    bool checkOpponent( rcsc::PlayerAgent * agent,
                        const int n_turn,
                        const int n_dash,
                        const rcsc::Vector2D & receive_pos );
    bool canKick( rcsc::PlayerAgent * agent,
                  const int n_turn,
                  const int n_dash,
                  const rcsc::Vector2D & receive_pos );
    bool doKick( rcsc::PlayerAgent * agent,
                 const int n_turn,
                 const int n_dash,
                 const rcsc::Vector2D & receive_pos );
};


//class _cone
//{
//private:
//    rcsc::Vector2D m_start;
//    double m_ang ;
//    double m_dist ;

//public:




//    _cone ( const rcsc::Vector2D & start ,
//            const double & ang ,
//            const double & dist )
//    : m_start(start) , m_ang (ang) , m_dist (dist)
//    {}

//    bool isInCone ( rcsc::Vector2D rePos , const double & body_ang )
//    {
//            rcsc::Vector2D dif = rePos - m_start ;
//            if ( dif.r() < m_dist )
//            if ( dif.th().degree() > body_ang - m_ang &&
//            dif.th().degree() < body_ang + m_ang )
//            return true ;
//            return false ;
//    }

//    static double normalizeAngle ( double angle )
//    {
//                while( angle > 180.0  ) angle -= 360.0;
//                while( angle < -180.0 ) angle += 360.0;

//                return ( angle );
//        }




//};

//class Interception {
//public:
//    static const double NEVER_CYCLE;

//private:

//    const Vector2D M_ball_first_pos;

//    const double M_ball_first_speed;

//    const AngleDeg M_ball_vel_angle;


//    const double M_ball_x_constant;

//    const double M_ball_x_d_constant;

//public:


//    Interception( const Vector2D & ball_pos,
//                  const double & ball_first_speed,
//                  const AngleDeg & ball_vel_angle );

//    Interception( const Vector2D & ball_pos,
//                  const Vector2D & ball_vel );

//    double getReachCycle( const Vector2D & player_pos,
//                          const Vector2D * player_vel,
//                          const AngleDeg * player_angle,
//                          const int player_count,
//                          const double & control_buf,
//                          const double & player_max_speed ) const;

//    double getReachCycleGreedly( const Vector2D & player_pos,
//                                 const Vector2D * player_vel,
//                                 const AngleDeg * player_angle,
//                                 const int player_count,
//                                 const double & control_buf,
//                                 const double & player_max_speed ,
//                                                                 const int maxCycles = 100 ) const;

//        double getReachCycleBallDelayed( const Vector2D & player_pos,
//                                 const double & control_buf,
//                                 const double & player_max_speed ,
//                                                                 int ballDelay = 0 ,
//                                                                 const int maxCycles = 100 ) const;

//        double getReachCyclePlayerDelayed( const Vector2D & player_pos,
//                                 const double & control_buf,
//                                 const double & player_max_speed ,
//                                                                 int playerDelay = 0 ,
//                                                                 const int maxCycles = 100 ) const;

//        double getReachCycleNonDelayed(const Vector2D & player_pos,
//                                 const double & control_buf,
//                                 const double & player_max_speed ,
//                                                                 const int maxCycles = 100 ) const;

//        double getReachCycleWithDelay( const Vector2D & player_pos,
//                                 const double & control_buf,
//                                 const double & player_max_speed ,
//                                                                 int playerDelay = 0 ,
//                                                                 int ballDelay = 0 ,
//                                                                 const int maxCycles = 100 ) const;
//};
//#endif




class Smart_Dribble
    : public rcsc::SoccerBehavior {
private:

    double m_dist ;
    double m_ang ;
    double m_step ;

public:



    Smart_Dribble ( double dist , double ang , double step ) :
    m_dist ( dist ) , m_ang ( ang ) , m_step ( step ) {}
        double getDribbleLen(rcsc::PlayerAgent * agent, int num , bool isTeammate);
    Smart_Dribble ( ) :
    m_dist ( DIST_DEFAULT ) , m_ang ( ANG_DEFAULT ) , m_step ( STEP_DEFAULT ) {}

    bool execute( rcsc::PlayerAgent * agent );
    rcsc::Vector2D getDribblePos (rcsc::PlayerAgent * agent,double & ang) ;
    rcsc::Vector2D getDribblePos1 (rcsc::PlayerAgent * agent) ;
    rcsc::Vector2D getClosetsetTeam(rcsc::PlayerAgent *agent);

double getLenght( rcsc::PlayerAgent * agent , double ang , int cycle ,double *dist) ;

bool bWithDodge( rcsc::PlayerAgent * agent ) ;
bool noDodge(rcsc::PlayerAgent * agent);
double getDribbleLen(rcsc::PlayerAgent * agent ,int num,double ang, rcsc::Vector2D myPos , rcsc::Vector2D oppPos );
double getDirectionOfWidestAngle(rcsc::PlayerAgent * agent , double a, double b,int cycle);


};

class Cone2DDRIB
{
private:
rcsc::Vector2D m_start;
double m_ang ;
double m_dist ;

public:




Cone2DDRIB ( const rcsc::Vector2D & start ,
        const double & ang ,
        const double & dist )
: m_start(start) , m_ang (ang) , m_dist (dist)
{}

bool isInCone ( rcsc::Vector2D rePos , const double & body_ang )
{
        rcsc::Vector2D dif = rePos - m_start ;
        if ( dif.r() < m_dist )
        if ( dif.th().degree() > body_ang - m_ang &&
        dif.th().degree() < body_ang + m_ang )
        return true ;
        return false ;
}

static double normalizeAngle ( double angle )
{
            while( angle > 180.0  ) angle -= 360.0;
            while( angle < -180.0 ) angle += 360.0;

            return ( angle );
    }




};
#endif
