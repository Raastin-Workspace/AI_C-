/*!
      Coded by Shayan Salehian
*/

#ifndef SHOOTIO_2010
#define SHOOTIO_2010

#include <rcsc/common/server_param.h>
#include <rcsc/common/logger.h>
#include <rcsc/player/soccer_action.h>
#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/geom/vector_2d.h>
#include <rcsc/action/kick_table.h>
#include <rcsc/action/body_smart_kick.h>
#include <rcsc/action/body_kick_one_step.h>
#include <rcsc/action/body_kick_to_relative.h>
#include <rcsc/action/neck_turn_to_point.h>

class ShootIO
    : public rcsc::SoccerBehavior {
private:

public:
    ShootIO ( )
    { }

	bool           execute              ( rcsc::PlayerAgent * agent ) ;
	bool           shoot_point          ( rcsc::PlayerAgent * agent ,
	                                      int ,
	                                      rcsc::Vector2D& ,
	                                      double& ) ;
	                                      
    double         getPowerForKick      ( rcsc::PlayerAgent * agent , double , double ) ;
	
	double         getActualKickPowerRate ( rcsc::PlayerAgent * agent ) ;
	double         get_max_speed_of_step  ( rcsc::PlayerAgent * agent , int , rcsc::Vector2D ) ;

private:

};

#endif
