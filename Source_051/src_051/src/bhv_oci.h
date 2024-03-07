/*!
      Coded by Shayan Salehian
*/

#ifndef OCI_2010
#define OCI_2010

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


class oci
    : public rcsc::SoccerBehavior {
private:

public:
    oci ( )
    { }

	bool           ball_reach_safe      ( rcsc::PlayerAgent * agent , 
                                          rcsc::Vector2D , int , double , bool = false ) ;
	int            opp_reach_cycle      ( rcsc::PlayerAgent * agent ,
	                                      rcsc::Vector2D , bool = false ) ;
	int            ball_reach_cycle     ( rcsc::PlayerAgent * agent ,
	                                      rcsc::Vector2D , int , double ) ;
	int            player_reach_cycle   ( rcsc::PlayerAgent * agent ,
										  rcsc::PlayerPtrCont::const_iterator ,
	                                      rcsc::Vector2D ) ;

private:

};

#endif
