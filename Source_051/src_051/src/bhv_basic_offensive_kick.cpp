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
#include <iostream>

#endif

#include "bhv_basic_offensive_kick.h"
#include "bhv_clear_ball2010.h"

#include <rcsc/action/body_advance_ball.h>
#include <rcsc/action/body_dribble.h>
#include <rcsc/action/body_smart_kick.h>
#include <rcsc/action/body_hold_ball.h>
#include <rcsc/action/body_pass.h>
#include <rcsc/action/neck_scan_field.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>
#include <rcsc/action/neck_turn_to_goalie_or_scan.h>
#include <rcsc/action/neck_turn_to_point.h>
#include <rcsc/action/neck_turn_to_relative.h>
#include <rcsc/action/body_clear_ball2009.h>
#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>
#include <rcsc/geom/sector_2d.h>
#include "Dribble.h"
#include "bhv_dribble2010.h"
#include "Block.h"
#include "neck_offensive_intercept_neck.h"

using namespace rcsc;
using namespace std;

/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_BasicOffensiveKick::execute( PlayerAgent * agent )
{
    const WorldModel & wm = agent->world();
    MostafPass *a;
    Bhv_Block *b;
    int nr;
    agent->setNeckAction( new Neck_TurnToPoint(a->where(agent)));
    // cout<<wm.time()<<"  "<<__FILE__<<" startKick "<<__LINE__<<" "<<wm.self().unum()<<endl;
//    if(wm.self().viewWidth()!=ViewWidth::NARROW){

//        agent->doChangeView( ViewWidth::NARROW );

//    }


//    if(Kicking.cycle+Kicking.step>wm.time().cycle()){

//        Body_SmartKick  smart(Kicking.end,Kicking.speed,Kicking.M_speed,Kicking.step-1);

//        if(smart.execute(agent)){

//            setKicking(wm,smart);
//            agent->setNeckAction( new Neck_TurnToPoint(Kicking.end));
//            return true;

//        }

//    }
//    if(a->SHOOOOOOT(agent,Kicking)){

//        Body_SmartKick  smart(Kicking.end,Kicking.speed,Kicking.M_speed,Kicking.step);

//        if(smart.execute(agent)){
//            agent->setNeckAction( new Neck_TurnToPoint(Kicking.end));
//            return true;

//            cout<<wm.time().cycle()<<"  shoot : "<<endl;
//        }
//    }
//    if(a->throughPass(agent,Kicking,nr)){

//        Body_SmartKick  smart(Kicking.end,Kicking.speed,Kicking.M_speed,Kicking.step);

//        if(smart.execute(agent)){

//            Vector2D vel=Vector2D::polar2vector(smart.sequence().speed_,(Kicking.end-a->startKick(smart)).dir().degree());
//            agent->addSayMessage(new PassMessage(nr,Kicking.end,a->startKick(smart),vel));
//            agent->setNeckAction( new Neck_TurnToPoint(Kicking.end));
//            return true;

//            cout<<wm.time().cycle()<<"  through : "<<nr<<"  "<<Kicking.end<<endl;
//        }
//    }
//    if(a->leadingPass(agent,Kicking,nr)){

//        Body_SmartKick  smart(Kicking.end,Kicking.speed,Kicking.M_speed,Kicking.step);

//        if(smart.execute(agent)){

//            Vector2D vel=Vector2D::polar2vector(smart.sequence().speed_,(Kicking.end-a->startKick(smart)).dir().degree());
//            agent->addSayMessage(new PassMessage(nr,Kicking.end,a->startKick(smart),vel));
//            cout<<wm.time().cycle()<<"  lead : "<<nr<<"  "<<Kicking.end<<endl;
//            agent->setNeckAction( new Neck_TurnToPoint(Kicking.end));
//            return true;

//        }
//    }
//    if(a->directPass(agent,Kicking,nr)){

//        Body_SmartKick  smart(Kicking.end,Kicking.speed,Kicking.M_speed,Kicking.step);

//        if(smart.execute(agent)){

//            Vector2D vel=Vector2D::polar2vector(smart.sequence().speed_,(Kicking.end-a->startKick(smart)).dir().degree());
//            agent->addSayMessage(new PassMessage(nr,Kicking.end,a->startKick(smart),vel));

//            cout<<wm.time().cycle()<<"  direct : "<<nr<<"  "<<Kicking.end<<endl;
//            agent->setNeckAction( new Neck_TurnToPoint(Kicking.end));
//            return true;

//        }
//    }

/*    if ( MostafPass().execute ( agent ) )
    {

        return true ;
    }
    else */if(wm.self().pos().x<-25){

        Circle2D cir((wm.self().pos()),6.0);

        if(b->getOppInCircle(agent,cir)!=0 /*&& a->getClearDir(agent,5)*/ ){
//            if(fabs(wm.self().pos().y) < 18 && Body_ClearBall2009().execute(agent))
//                ;
//            else
//                Clear_Ball().execute(agent);
            /*Body_ClearBall2009().execute(agent)*/;
//        if(b->getOppInCircle(agent,cir)!=0 /*&& a->getClearDir(agent,5)*/)
//            Clear_Ball().execute(agent);

            a->ClearTheFuckingBallFuckingGoalie(agent);


        }
        else{
//            // cout<<wm.time()<<"  "<<__FILE__<<" dribble "<<__LINE__<<endl;
//            Smart_Dribble().execute(agent);
            if(Smart_Dribble().execute(agent));
                // cout<<wm.time()<<"  "<<__FILE__<<" dribble "<<__LINE__<<endl;
            else if(rcsc::Body_HoldBall2008().execute(agent));
                // cout<<wm.time()<<"  "<<__FILE__<<" HoldBall "<<__LINE__<<endl;
            else{
                a->ClearTheFuckingBallFuckingGoalie(agent);
                // cout<<wm.time()<<"  "<<__FILE__<<" noJob "<<__LINE__<<endl;
            }

        }
    }
    else if(wm.self().pos().x<-15){

        Circle2D cir((wm.self().pos()),4.0);

//        if(b->getOppInCircle(agent,cir)>1 /*&& a->getClearDir(agent,5)*/ && Body_ClearBall2009().execute(agent))
//            Clear_Ball().execute(agent);
            /*Body_ClearBall2009().execute(agent)*/;
        if(b->getOppInCircle(agent,cir)>1 /*&& a->getClearDir(agent,5)*/)
//            Clear_Ball().execute(agent);
        {
//                    if(fabs(wm.self().pos().y) < 18 && Body_ClearBall2009().execute(agent))
//                        ;
//                    else
//                        Clear_Ball().execute(agent);
                    /*Body_ClearBall2009().execute(agent)*/;
//                if(b->getOppInCircle(agent,cir)!=0 /*&& a->getClearDir(agent,5)*/)

//                    Clear_Ball().execute(agent);
    a->ClearTheFuckingBallFuckingGoalie(agent);
        }

        else{
//            // cout<<wm.time()<<"  "<<__FILE__<<" dribble "<<__LINE__<<endl;
//            Smart_Dribble().execute(agent);
            if(Smart_Dribble().execute(agent));
                // cout<<wm.time()<<"  "<<__FILE__<<" dribble "<<__LINE__<<endl;
            else if(rcsc::Body_HoldBall2008().execute(agent));
                // cout<<wm.time()<<"  "<<__FILE__<<" HoldBall "<<__LINE__<<endl;
            else{
                a->ClearTheFuckingBallFuckingGoalie(agent);
                // cout<<wm.time()<<"  "<<__FILE__<<" noJob "<<__LINE__<<endl;
            }


        }



    }
    else{

        if(Smart_Dribble().execute(agent));
            // cout<<wm.time()<<"  "<<__FILE__<<" dribble "<<__LINE__<<endl;
        else if(rcsc::Body_HoldBall2008().execute(agent));
            // cout<<wm.time()<<"  "<<__FILE__<<" HoldBall "<<__LINE__<<endl;
        else{
            a->ClearTheFuckingBallFuckingGoalie(agent);
            // cout<<wm.time()<<"  "<<__FILE__<<" noJob "<<__LINE__<<endl;
        }

    }

    agent->setNeckAction( new Neck_TurnToPoint(a->where(agent)));

    // cout<<wm.time()<<"  "<<__FILE__<<" endkick "<<__LINE__<<" "<<wm.self().unum()<<endl;
    return true;

}
