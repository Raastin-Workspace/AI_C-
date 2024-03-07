// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "role_center_forward.h"


#include "bhv_basic_offensive_kick.h"
#include "bhv_basic_move.h"

#include "bhv_Atk_move.h"
#include "bhv_Med_move.h"
#include "bhv_Def_move.h"

#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>

#include <rcsc/common/logger.h>

using namespace rcsc;

const std::string RoleCenterForward::NAME( "CenterForward" );

/*-------------------------------------------------------------------*/
/*!

 */
namespace {
rcss::RegHolder role = SoccerRole::creators().autoReg( &RoleCenterForward::create,
                                                       RoleCenterForward::NAME );
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
RoleCenterForward::execute( PlayerAgent * agent )
{
    bool kickable = agent->world().self().isKickable();
    if ( agent->world().existKickableTeammate()
         && agent->world().teammatesFromBall().front()->distFromBall()
         < agent->world().ball().distFromSelf() )
    {
        kickable = false;
    }

    if ( kickable )
    {
        doKick( agent );
    }
    else
    {
        doMove( agent );
    }

    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
RoleCenterForward::doKick( PlayerAgent * agent )
{

    Bhv_BasicOffensiveKick().execute( agent );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
RoleCenterForward::doMove( PlayerAgent * agent )
{
    if(agent->world().self().unum() > 8 )
        Bhv_AtkMove().execute( agent );
    else if(agent->world().self().unum() > 5 )
        Bhv_MedMove().execute( agent );
    else
        Bhv_DefMove().execute( agent );


//    Bhv_BasicMove().execute( agent );
}
