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

#include "bhv_Atk_move.h"

#include "strategy.h"

#include "bhv_basic_tackle.h"

#include <rcsc/action/basic_actions.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/body_intercept.h>
#include <rcsc/action/neck_turn_to_ball_or_scan.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>

#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/player/intercept_table.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>

#include "neck_offensive_intercept_neck.h"

#include "Block.h"
#include "Pass.h"
#include "Atk_Positioning.h"
using namespace rcsc;

/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_AtkMove::execute( PlayerAgent * agent )
{

    Bhv_Block * b;
    MostafPass *m;
    turnNeck(agent);
//    agent->setNeckAction( new Neck_OffensiveInterceptNeck() );

    const WorldModel & wm = agent->world();


    dlog.addText( Logger::TEAM,
                  __FILE__": Bhv_AtkMove" );


    bool bf=0;
    double rel=0;


    // cout<<wm.time()<<"  "<<__FILE__<<" startMove "<<__LINE__<<" "<<wm.self().unum()<<endl;
    //-----------------------------------------------
    // tackle
    if ( wm.ball().pos().x < 20 && Bhv_BasicTackle( 0.8, 80.0 ).execute( agent ) && !(wm.ball().pos().x>42 && fabs(wm.ball().pos().y)>10))
    {
        return true;
    }

    if( wm.ball().pos().x < 20 && (b->tackle(agent) || b->bTackle(agent)) && Bhv_BasicTackle(0.8 , 80.0).execute2(agent) && wm.ball().pos().x<36){

//        Bhv_BasicTackle(0.8 , 80.0).executeV14(agent,true);
//        agent->doTackle(0.0,true);

        return true;
    }


//    if(wm.ball().pos().x < 20 && ( b->tackle(agent) || b->bTackle(agent) || Bhv_BasicTackle( 0.8, 80.0 ).execute( agent ) )){
//        cout<<wm.time()<<"  "<<__FILE__<<" tackle "<<__LINE__<<" "<<wm.self().unum()<<endl;
//        agent->doTackle(0.0,true);
//        return true;
//    }

//    if ( b->shouldTackle(agent,rel,bf) && !wm.existKickableTeammate())
//    {
//        return agent->doTackle(rel,bf);

//    }

    /*--------------------------------------------------------*/
    // chase ball
    const int self_min = wm.interceptTable()->selfReachCycle();
    const int mate_min = wm.interceptTable()->teammateReachCycle();
    const int opp_min = wm.interceptTable()->opponentReachCycle();

    if ( ! wm.existKickableTeammate()
         && ( /*self_min <= 3
              || */( self_min <= mate_min
                   && self_min < opp_min + 1 )
              )
         )
    {
        dlog.addText( Logger::TEAM,
                      __FILE__": intercept" );
        Body_Intercept().execute( agent );
        agent->setNeckAction( new Neck_OffensiveInterceptNeck() );
        // cout<<wm.time()<<"  "<<__FILE__<<" Intercept false "<<__LINE__<<" "<<wm.self().unum()<<endl;
        return true;
    }

    Vector2D posBlock;

    if ( b->getBlocker(agent,posBlock) == wm.self().unum() )
    {

//        cout<<"cycle  :  "<<wm.time().cycle()<<"  playerUnum:   "<<wm.interceptTable()->fastestOpponent()->unum()<<"   oppMin:  "<<opp_min<<endl;
//        cout<<"cycle  :  "<<wm.time().cycle()<<"  playerUnum:   "<<wm.interceptTable()->fastestOpponent()->unum()<<"   self:  "<<self_min<<endl;
//        cout<<"cycle  :  "<<wm.time().cycle()<<"  blockPos:   "<<posBlock<<endl;


        if( posBlock == Vector2D(1000.0,1000.0) || wm.self().pos().x > 30 ){
            if(Strategy::getSitu(wm)==Offense_Situation)
                agent->setNeckAction( new Neck_TurnToPoint(m->where(agent,1)));
            else
                agent->setNeckAction( new Neck_TurnToBall() );
            // cout<<wm.time()<<"  "<<__FILE__<<" Intercept "<<__LINE__<<" "<<wm.self().unum()<<endl;
            Body_Intercept().execute( agent );
        }
        else{
            Body_GoToPoint2010(posBlock,0.5,100,-1.0,100,false,15).execute(agent);
            // cout<<wm.time()<<"  "<<__FILE__<<" block "<<__LINE__<<" "<<wm.self().unum()<<endl;
            agent->setNeckAction( new Neck_TurnToBall() );
        }
        //        agent->setNeckAction( new Neck_OffensiveInterceptNeck() );

        return true;
    }


    Vector2D target_point = Strategy::i().getPosition( wm.self().unum() );
    const double dash_power = Strategy::get_normal_dash_power( wm );


    if( wm.ball().pos().x> -20  && wm.ball().pos().x<36 && Strategy::getSitu(wm) != Defense_Situation /*&& wm.existKickableTeammate() && !wm.existKickableOpponent()*/){

        if(wm.ball().pos().x>wm.self().pos().x)
            target_point.x = wm.ball().pos().x;

//        if( wm.ball().pos().x< wm.offsideLineX()-2 && wm.offsideLineCount()<6)
//            target_point.x=wm.offsideLineX()-2;

        if( wm.ball().pos().x< /*wm.theirDefensePlayerLineX()*/ getOffside(agent) -1 )
            target_point.x=/*wm.theirDefensePlayerLineX() */getOffside(agent) -1;

            //target_point=toVector(a->circlePositioning(agent,toVecPos(target_point)));
    }

    Positioning * a;


    if( wm.ball().pos().x> -20 && Strategy::getSitu(wm) != Defense_Situation /*&& wm.existKickableTeammate() && !wm.existKickableOpponent()*/ )
        target_point=toVector(a->circlePositioning(agent,toVecPos(target_point)));

    if(target_point.dist(wm.ball().pos())<5 && wm.ball().pos().x> 0  )
        target_point = wm.ball().pos()+Vector2D::polar2vector(5,VecPosition::normalizeAngle((target_point-wm.ball().pos()).dir().degree()));


    double dist_thr = wm.ball().distFromSelf() * 0.1;
    if ( dist_thr < 1.0 ) dist_thr = 1.0;

    if( wm.self().pos().x> agent->world().offsideLineX()-2 )
            dist_thr = 1.0;

    dlog.addText( Logger::TEAM,
                  __FILE__": Bhv_AtkMove target=(%.1f %.1f) dist_thr=%.2f",
                  target_point.x, target_point.y,
                  dist_thr );

    agent->debugClient().addMessage( "AtkMove%.0f", dash_power );
    agent->debugClient().setTarget( target_point );
    agent->debugClient().addCircle( target_point, dist_thr );

    double dir=getBisectorTwoAngles(0,(wm.ball().pos()-target_point).dir().degree() );

    if(wm.ball().pos().x>36)
         dir = getBisectorTwoAngles((Vector2D(52.5,0.0)-target_point).dir().degree() ,(wm.ball().pos()-target_point).dir().degree() );
    if(wm.ball().pos().x<42 && wm.ball().pos().x>-10 &&  a->goTo2(agent,target_point,0,dash_power,dist_thr)){
        agent->setNeckAction( new Neck_TurnToPoint(offside(agent)) );
        // cout<<wm.time()<<"  "<<__FILE__<<" moveSosoli "<<__LINE__<<" "<<wm.self().unum()<<endl;
        return true;
    }


    if ( ! Body_GoToPoint( target_point, dist_thr, dash_power
                           ).execute( agent ) )
    {
        Body_TurnToBall().execute( agent );
//        Body_TurnToPoint(Vector2D(52.5,0.0)).execute(agent);

    }


    if(wm.ball().pos().dist(wm.self().pos())<3){
        agent->setNeckAction( new Neck_TurnToPoint(m->where(agent)));
    }
    else if(wm.ball().pos().x>30.0 ){
        agent->setNeckAction( new Neck_TurnToPoint(m->where(agent)));
    }
    else if(wm.ball().pos().x>-20.0){
        if ( wm.existKickableOpponent() && wm.ball().distFromSelf() < 9.0 )
            agent->setNeckAction( new Neck_TurnToBall() );
        else if(wm.self().unum()>8)
            agent->setNeckAction( new Neck_TurnToPoint(m->where(agent,2)));
        else
            agent->setNeckAction( new Neck_TurnToBallOrScan(2));
    }
    else{
        if (wm.existKickableOpponent() && wm.ball().distFromSelf() < 18.0 )
            agent->setNeckAction( new Neck_TurnToBall() );

        else
            agent->setNeckAction( new Neck_TurnToBallOrScan(2));

    }

    // cout<<wm.time()<<"  "<<__FILE__<<" endMove "<<__LINE__<<" "<<wm.self().unum()<<endl;
    return true;
}

//bool    Bhv_AtkMove::turnNeck(PlayerAgent *agent){

//    const WorldModel & wm = agent->world();
//    MostafPass *m;

//    if(wm.self().pos().dist(wm.ball().pos())<3 ){
//        if(wm.ball().pos().x>-15)
//            agent->setNeckAction( new Neck_TurnToPoint(m->where(agent)));
//        else if(wm.ball().pos().x>-36 /*&& !wm.existKickableOpponent()*/)
//            agent->setNeckAction( new Neck_TurnToPoint(m->where(agent)));
//        else
//            agent->setNeckAction( new Neck_TurnToBall() );
//    }
//    else
//        agent->setNeckAction( new Neck_TurnToBall() );


//    return true;
//}
bool    Bhv_AtkMove::turnNeck(PlayerAgent *agent){

    const WorldModel & wm = agent->world();
    MostafPass *m;

//    if(wm.self().pos().dist(wm.ball().pos())<3 ){
//        /*if(wm.ball().pos().x>-15)
//            agent->setNeckAction( new Neck_TurnToPoint(m->where(agent)));
//        else*/ if(wm.ball().pos().x>-36 /*&& !wm.existKickableOpponent()*/)
//            agent->setNeckAction( new Neck_TurnToPoint(m->where(agent)));
//        else
//            agent->setNeckAction( new Neck_TurnToBall() );
//    }
//    else
////        agent->setNeckAction( new Neck_TurnToBall() );
//        agent->setNeckAction( new Neck_OffensiveInterceptNeck() );

    if(wm.ball().pos().dist(wm.self().pos())<3){
        if(wm.ball().pos().x > -20)
            agent->setNeckAction( new Neck_TurnToPoint(m->where(agent)));
        else
            agent->setNeckAction( new Neck_TurnToBall() );
    }
    else if(wm.ball().pos().x>30.0 ){
        agent->setNeckAction( new Neck_TurnToPoint(m->where(agent,2)));
    }
    else if(wm.ball().pos().x>-20.0){
        if ( wm.existKickableOpponent() && wm.ball().distFromSelf() < 18.0 )
//            agent->setNeckAction( new Neck_TurnToBall() );
            agent->setNeckAction( new Neck_TurnToPoint(m->where(agent,1)));
        else if(wm.self().unum()>8)
            agent->setNeckAction( new Neck_TurnToPoint(m->where(agent,2)));
        else
//            agent->setNeckAction( new Neck_TurnToBallOrScan(2));
            agent->setNeckAction( new Neck_OffensiveInterceptNeck() );
    }
    else{
        if (wm.existKickableOpponent() && wm.ball().distFromSelf() < 18.0 )
            agent->setNeckAction( new Neck_TurnToBall() );

        else
//            agent->setNeckAction( new Neck_TurnToBallOrScan(2));
            agent->setNeckAction( new Neck_OffensiveInterceptNeck() );

    }

    return true;
}

//bool    Bhv_AtkMove::turnNeck(PlayerAgent *agent){

//    const WorldModel & wm = agent->world();
//    MostafPass *m;

//    if(wm.self().pos().dist(wm.ball().pos())<3 ){
//        if(wm.ball().pos().x>-15)
//            agent->setNeckAction( new Neck_TurnToPoint(m->where(agent)));
//        else if(wm.ball().pos().x>-36 /*&& !wm.existKickableOpponent()*/)
//            agent->setNeckAction( new Neck_TurnToPoint(m->where(agent)));
//        else
//            agent->setNeckAction( new Neck_TurnToBall() );
//    }
//    else
//        agent->setNeckAction( new Neck_TurnToBall() );


//    return true;
//}

double Bhv_AtkMove::getOffside( const rcsc::PlayerAgent * agent ){

    const WorldModel  & wm = agent->world() ;


    return wm.offsideLineX();

//    return wm.theirDefensePlayerLineX()-1;
    double x = /*wm.theirDefenseLineX() */0.0;
    for (int i = 1; i < 12 ; ++i) {

        if(i!= wm.theirGoalieUnum() && wm.theirPlayer(i) && wm.theirPlayer(i)->posCount()<6)
            if( wm.theirPlayer(i)->pos().x > x)
                x = wm.theirPlayer(i)->pos().x;


    }


    return x;

}

Vector2D    Bhv_AtkMove::offside( const rcsc::PlayerAgent * agent ){

    const WorldModel  & wm = agent->world() ;

    MostafPass *m;
    if(wm.self().unum() == 11){


        if(m->getCount(wm,90,5) > m->getCount(wm,-90,5))
            return wm.self().pos()+Vector2D::polar2vector(10,90.0);
        else
            return wm.self().pos()+Vector2D::polar2vector(10,-90.0);
    }
    else if(wm.self().unum() == 10){

        return wm.self().pos()+Vector2D::polar2vector(10,-90.0);
    }
    else{

        return wm.self().pos()+Vector2D::polar2vector(10,90.0);
    }


}

