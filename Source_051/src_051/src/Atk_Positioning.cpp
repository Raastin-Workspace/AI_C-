#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif



#include<Pass.h>
#include "bhv_Atk_move.h"
#include "Atk_Positioning.h"
#include <rcsc/player/penalty_kick_state.h>
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
/*-------------------------------------------------------------------*/
using rcsc::Vector2D ;

using std::cout;
using std::endl;

using namespace rcsc;



double Positioning::min(double a, double b){

 if(a<b)
     return a;
 return b;
}

double Positioning::max(double a, double b){

 if(a>b)
     return a;
 return b;
}




////////////////////////////////////////////////////////////////////////

bool Positioning::isBallTheirs( PlayerAgent * agent ,double kickable, VecPosition pos )
{
    const WorldModel & wm = agent->world();
    int i;

    if( pos == VecPosition( -1000, -1000 ) )
            pos = toVecPos(wm.ball().pos());


    for( int i=1 ; i < 12 ;	i++)
        if(wm.theirPlayer(i) && toVecPos(wm.theirPlayer(i)->pos()).getDistanceTo( pos ) < kickable /* coach? */ + 0.385 )
            return true;

    return false;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool Positioning::isBallOurs( PlayerAgent * agent ,bool b )
{
    const WorldModel & wm = agent->world();
    int i;
    Vector2D posBall = wm.ball().pos();
    for ( i = 1 ; i <= 11 ; i++ )
    {
            if ( (b && wm.self().unum() == i) || !wm.ourPlayer(i) )
            {
                    continue ;
            }

            if ( wm.ourPlayer(i)->pos().dist( posBall ) <=  wm.ourPlayer(i)->playerTypePtr()->kickableArea() )
                    return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
int Positioning::getOppInCircle(PlayerAgent * agent ,Circle cir){


    const WorldModel & wm = agent->world();
    int nr=0;
    for( int i=1 ; i < 12 ;	i++)
        if(wm.theirPlayer(i) && toVecPos(wm.theirPlayer(i)->pos()).getDistanceTo(cir.getCenter())<cir.getRadius())
            nr++;


    return nr;






}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
int Positioning::getTtInCircle(PlayerAgent * agent ,Circle cir){


    const WorldModel & wm = agent->world();
    int nr=0;
    for( int i=1 ; i < 12 ;	i++)
        if(wm.ourPlayer(i) && toVecPos(wm.ourPlayer(i)->pos()).getDistanceTo(cir.getCenter())<cir.getRadius())
            nr++;


    return nr;






}
///^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
///^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
bool    Positioning::goTo(PlayerAgent *agent,Vector2D posTo,double ang,double maxDashPower,double disWhenTrue){


    Vector2D agentPos = agent->world().self().pos();
    Line2D line1 = Line2D(agentPos,AngleDeg::normalize_angle(90+ang));
    Line2D line2 = Line2D(posTo,AngleDeg::normalize_angle(ang));
    Vector2D    posGo = line1.intersection(line2);

    int sign=1;
    if((posGo-agentPos).dir().degree()<0)
        sign=-1;

    if(posGo.dist(posTo)>5)
        return false;

    double angBody=agent->world().self().body().degree();

    if(getAngDiff(ang,angBody)>10){
         if(Body_TurnToAngle(ang).execute(agent));
            return true;
    }
    if(posTo.dist(agentPos)>disWhenTrue){



        if(posGo.dist(agentPos)>0.5){
            if(agent->doDash(maxDashPower,90*sign))
                return true;
        }
        if (  agentPos.dist(posTo)> disWhenTrue  ){
            if(Body_GoToPoint2010(posTo,disWhenTrue,maxDashPower ).execute(agent))
             return true;
        }

    }

    return true;


}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
///^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
///^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
bool    Positioning::goTo2(PlayerAgent *agent,Vector2D posTo,double ang,double maxDashPower,double disWhenTrue){


    Vector2D agentPos = agent->world().self().pos();
    Line2D line1 = Line2D(agentPos,AngleDeg::normalize_angle(ang));
    Line2D line2 = Line2D(posTo,AngleDeg::normalize_angle(90+ang));
    Vector2D    posGo = line1.intersection(line2);

    int sign=1;
    if((posTo-agentPos).dir().degree()<0)
        sign=-1;

    if(agentPos.dist(posTo)>5 || agentPos.x> agent->world().offsideLineX()-2 )
        return false;

    if(posGo.dist(agentPos)>disWhenTrue)
        if(Body_GoToPoint2010(posGo,disWhenTrue,maxDashPower ).execute(agent))
             return true;



    double angBody=agent->world().self().body().degree();

    if(getAngDiff(ang,angBody)>10){
         if(Body_TurnToAngle(ang).execute(agent));
            return true;
    }
    if(posTo.dist(agentPos)>disWhenTrue){



        if(posTo.dist(agentPos)>disWhenTrue){
            if(agent->doDash(maxDashPower,90*sign))
                return true;
        }
//        if (  agentPos.dist(posTo)> disWhenTrue  ){
//            if(Body_GoToPoint2010(posTo,disWhenTrue,maxDashPower ).execute(agent))
//             return true;
//        }

    }
    Body_TurnToAngle(ang).execute(agent);
    return true;


}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/*
VecPosition Positioning::targetPos(VecPosition center,int  number){

    VecPosition posBall=getBallPos();
    VecPosition target=posBall;
    if(number==11){

        if(posBall.getX()<0){
            if(fabs(posBall.getY())>15){
                target=strPosTeammate(6);
                lastNr=6;
            }
            else if(posBall.getY()>4){
                target=strPosTeammate(8);
                lastNr=8;
            }
            else if(posBall.getY()<-4){
                target=strPosTeammate(7);
                lastNr=7;
            }
            else{
                target=strPosTeammate(lastNr);
            }
        }
        else if(posBall.getX()<30){
            if(posBall.getY()>5){
                target=strPosTeammate(7);
                lastNr=7;
            }
            else if(posBall.getY()<-5){
                target=strPosTeammate(8);
                lastNr=8;
            }
            else{
                target=strPosTeammate(lastNr);
            }

        }
        else{
            if(posBall.getY()>20){
                target=strPosTeammate(8);
                lastNr=8;
            }
            else if(posBall.getY()<-20){
                target=strPosTeammate(7);
                lastNr=7;
            }
            else{
                target=posBall;
            }


        }


    }
    else if(number==10){

        if(posBall.getX()<0){
           target=strPosTeammate(8);
           lastNr=8;

        }
        else if(posBall.getX()<25){
            if(posBall.getY()<-5){
                target=strPosTeammate(8);
                lastNr=8;
            }
            else if(posBall.getY()<0){
                target=strPosTeammate(lastNr);
            }
            else{
                target=strPosTeammate(11);
                lastNr=11;
            }

        }
        else{
            if(posBall.getY()<-15){
                target=strPosTeammate(11);
                lastNr=11;
            }
            else{
                target=posBall;
            }



        }

    }
    else if(number==9){


        if(posBall.getX()<0){
           target=strPosTeammate(7);
           lastNr=7;
       }
        else if(posBall.getX()<25){
            if(posBall.getY()>5){
                target=strPosTeammate(7);
                lastNr=7;
            }
            else if(posBall.getY()>0){
                target=strPosTeammate(lastNr);
            }
            else{
                target=strPosTeammate(11);
                lastNr=11;
            }

        }
        else{
            if(posBall.getY()>15){
                target=strPosTeammate(11);
                lastNr=11;
            }
            else{
                target=posBall;
            }



        }

    }
    else if(number==8){
        if(posBall.getX()<-10){
            if(posBall.getY()<-15){
                target=strPosTeammate(6);
                lastNr=6;
            }
            else{
                target=posBall;
            }
        }
        else if(posBall.getX()<40){
            target=posBall;

        }
        else{
            if(posBall.getY()<-15){
                target=strPosTeammate(7);
                lastNr=7;
            }
            else{
                target=posBall;
            }



        }

    }
    else if(number==7){
        if(posBall.getX()<-10){
            if(posBall.getY()>15){
                target=strPosTeammate(6);
                lastNr=6;
            }
            else{
                target=posBall;
            }
        }
        else if(posBall.getX()<40){
            target=posBall;

        }
        else{
            if(posBall.getY()>15){
                target=strPosTeammate(8);
                lastNr=8;
            }
            else{
                target=posBall;
            }



        }

    }
    else{
        target=posBall;

    }
    return target;

}*/
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

int     Positioning::positioningRadius(VecPosition center){

    if(center.getX()>34)
        return 14;
    else if(center.getX()>0)
        return 10;
    else if(center.getX()>-20)
        return 7;
    else
        return 4;
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

int     Positioning::coneScore(PlayerAgent * agent ,VecPosition center,VecPosition target,int minConf){

    const WorldModel & wm = agent->world();
    double  minR=2.5;

    for(int i=1;i<12;i++)
        if(wm.theirPlayer(i) &&  minConf>=wm.theirPlayer(i)->posCount())
            if(getConeDist(toVecPos(wm.theirPlayer(i)->pos()),target,center)<minR)
                minR=getConeDist(toVecPos(wm.theirPlayer(i)->pos()),target,center);

    return reRate(minR,0.0,3.0,0.0,10.0);

}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

VecPosition Positioning::circlePositioning(PlayerAgent * agent ,VecPosition center){


    const WorldModel & wm = agent->world();


    if(wm.self().unum()<7 || wm.ball().pos().dist(toVector(center))>50 || wm.gameMode().type()!= GameMode::PlayOn )
        return center;


    if(wm.self().unum()>8 && wm.ball().pos().x<36 && wm.self().unum()>8 && wm.ball().pos().x > -5.0 )
        return center;

    VecPosition target=toVecPos(wm.ball().pos());//targetPos(center,getPlayerNumber());
    int         radius=7.0;//positioningRadius(center);


    struct  stc{
        VecPosition pos;
        double      score;

    }points[radius*8];
    double  dir=(target-center).getDirection();

    int length=1;
    VecPosition    backward ;
    MostafPass * m;

    int k = 0 ;

    for(int i=0;i<radius*8;length++)
        for(int j=0;j<360 && i<radius*8;j+=45){
            points[k].pos=center+VecPosition(length,VecPosition::normalizeAngle(j+dir),POLAR);
            if(target.getDistanceTo(points[k].pos)<5)
                continue;

            if(points[k].pos.getX()>/*wm.theirDefensePlayerLineX()*/Bhv_AtkMove().getOffside(agent) -1 )
                points[k].pos.setX(/*wm.theirDefensePlayerLineX()*/Bhv_AtkMove().getOffside(agent) -1 );

            m->setInField(points[k].pos,51.5);
            if(/*points[i].pos.getX()>35 || wm.self().unum() < */9  ){

                backward = VecPosition(3,(points[k].pos-target).getDirection(),POLAR);
                points[k].score= coneScore(agent,backward+points[k].pos,target);
            }
            else
                points[k].score=m->flagScoring(agent,points[k].pos,target);//coneScore(agent,backward+points[i].pos,target);
            i++;
            k++;
        }


    VecPosition finalPos=center;
    double      finalScore;

    if(/*finalPos.getX()>35 || wm.self().unum() < */9){

        backward = VecPosition(3,(finalPos-target).getDirection(),POLAR);
        finalScore= coneScore(agent,backward+finalPos,target);
    }
    else
        finalScore=m->flagScoring(agent,finalPos,target);//coneScore(agent,finalPos,target);

    for(int i=0;i</*radius*8*/k;i++)
        if(finalScore<points[i].score){
            finalPos=points[i].pos;
            finalScore=points[i].score;
        }


return finalPos;
}


double Positioning::getConeDist(VecPosition pos,VecPosition start,VecPosition end){

  Line        line       = Line::makeLineFromTwoPoints( start, end );
  VecPosition posOnLine = line.getPointOnLineClosestTo( pos );
  if(posOnLine.isBetweenXY(start,end))
      return    ((posOnLine.getDistanceTo(pos))/(posOnLine.getDistanceTo(start)));
  else
      return 1000;


}


bool    Positioning::offenseMode(PlayerAgent * agent){

    const WorldModel & wm = agent->world();

    int selfMin=wm.interceptTable()->selfExhaustReachCycle(),
        oppMin=wm.interceptTable()->opponentReachCycle(),
        ttMin=wm.interceptTable()->teammateReachCycle();


    if(min(selfMin,ttMin)-5>oppMin)
        return false;
    if(min(selfMin,ttMin)+5<oppMin)
        return true;

    if(wm.ball().pos().x>36.0 && min(selfMin,ttMin)-3<oppMin)
        return true;

    if(wm.existKickableOpponent() && min(selfMin,ttMin) > 2)
        return false;

    if(wm.existKickableTeammate() ){
        if(wm.ball().pos().x<-36 && oppMin<5 )
            return false;
        else
            return true;


    }


    if(wm.ball().pos().x>0.0 && min(selfMin,ttMin)<=oppMin)
        return true;

    if(wm.ball().pos().x<0.0 && min(selfMin,ttMin)>=oppMin)
        return false;

    return min(selfMin,ttMin)<=oppMin;





}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

//bool Positioning::newAtkPositioning(PlayerAgent *agent){

//    const WorldModel  & wm = agent->world();

//    const int   selfNr = wm.self().unum();
//    Vector2D    formationPos = Strategy::i().getPosition( selfNr );
//    Vector2D    ballPos = wm.ball().pos();

//    double  radius = 10.0;
//    static Vector2D positioningPolar(0.0,0.0);

//    const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();

//    const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();
//    for ( rcsc::PlayerPtrCont::const_iterator it = opps.begin();
//           it != opps_end;
//           ++it ){
        
//        if()


//    }


//  }









