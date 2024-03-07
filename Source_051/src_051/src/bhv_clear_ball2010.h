/*!
      Coded by Shayan Salehian
*/

#ifndef CLEARBALL_2010
#define CLEARBALL_2010

#include <rcsc/common/server_param.h>
#include <rcsc/common/logger.h>
#include <rcsc/player/soccer_action.h>
#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/geom/vector_2d.h>
#include <rcsc/action/kick_table.h>
#include <rcsc/action/body_smart_kick.h>
#include <rcsc/action/body_kick_one_step.h>

#include <iostream>
#include <vector>

using namespace std ;

class Clear_Ball
    : public rcsc::SoccerBehavior {
private:

public:
    Clear_Ball ( )
    { }

	bool           execute                  ( rcsc::PlayerAgent * agent ) ;
    bool doForceKick(rcsc::PlayerAgent * agent);

};

#endif
