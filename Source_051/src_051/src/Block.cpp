#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif


#include <rcsc/common/server_param.h>
#include "bhv_basic_move.h"
#include "Block.h"
#include "bhv_basic_tackle.h"
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
double  Bhv_Block::getAngDiff(double a , double b){

    return fabs( AngleDeg::normalize_angle( b - a ) );

}

////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////// Block......

int Bhv_Block::ttTimeToGetBall( PlayerAgent * agent ,int nr,Vector2D  posBall,Vector2D  vellBall)
{

    const WorldModel & wm = agent->world();
    const AbstractPlayerObject * player=wm.ourPlayer(nr);
    if(!player)
        return 1000;

    else
        return timeToGetBall(agent , player , posBall , vellBall );
}
////////////////////////////////////////////////////////////////////////

int Bhv_Block::oppTimeToGetBall( PlayerAgent * agent ,int nr,Vector2D  posBall, Vector2D vellBall  )
{

    const WorldModel & wm = agent->world();
    const AbstractPlayerObject * player=wm.ourPlayer(nr);
    if(!player)
        return 1000;

    else
        return timeToGetBall(agent , player , posBall , vellBall );
}
////////////////////////////////////////////////////////////////////////

int Bhv_Block::timeToGetBall( PlayerAgent * agent ,const AbstractPlayerObject * player,Vector2D  posBall, Vector2D vellBall  )
{

    const WorldModel & wm = agent->world();

    if( posBall == Vector2D( -1000, -1000 ) )
            posBall = wm.ball().pos();

    if( vellBall == Vector2D( -1000, -1000 ) )
            vellBall = wm.ball().vel();


//    if( ballSpeed == -1000 )
//            ballSpeed = wm.ball().vel().length();

//    if( ballDir == -1000 )
//            ballDir   = wm.ball().vel().dir().degree();

    Vector2D posObj  = player->pos();


    int i = 0;

    while( 1 )
    {

        Simul agentSim(agent,player);

        if( agentSim.timeTo( posBall ) <= i )
        {
                return i;
        }

        posBall += vellBall ;
        vellBall *= BALL_DECAY;

        if( /*ballSpeed < 0.001 &&*/ i > 50 )
        {
                break;
        }

        i++;
    }

    return 1000;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

double Bhv_Block::getOppDribbleAngle( PlayerAgent * agent ,Vector2D pos )
{


    if( pos.x > -25.0 )
    {
            return 180.0;
    }

    if( fabs( pos.y < 10.0 ) )
    {
            return ( Vector2D( -52.5, 0.0 ) - pos ).dir().degree();
    }

    if( pos.x > -37.0 && fabs( pos.y ) > 20.0 )
    {
            return 180.0;
    }

    return ( Vector2D( -52.5, 0.0 ) - pos ).dir().degree();

/*
    double dir1=(Vector2D(-52.5,7)-pos).dir().degree();
    double dir2=(Vector2D(-52.5,-7)-pos).dir().degree();
    double dir3=(Vector2D(-46.5,0)-pos).dir().degree();
    return getBisectorTwoAngles(dir3,getBisectorTwoAngles(dir1,dir2));
*/
    /*const WorldModel & wm = agent->world();


        Vector2D posAgent = wm.self().pos();
        Vector2D posBall  = wm.ball().pos();

        const PlayerObject * oppObj = wm.interceptTable()->fastestOpponent();

        if(!oppObj){

            double dir1=(Vector2D(-52.5,7)-posBall).dir().degree();
            double dir2=(Vector2D(-52.5,-7)-posBall).dir().degree();
            double dir3=(Vector2D(-41.5,0)-posBall).dir().degree();
            return getBisectorTwoAngles(dir1,dir2);

        }
        else{
            double dir1=(Vector2D(-52.5,7)-(oppObj->pos())).dir().degree();
            double dir2=(Vector2D(-52.5,-7)-(oppObj->pos())).dir().degree();
            double dir3=(Vector2D(-41.5,0)-(oppObj->pos())).dir().degree();

            return getBisectorTwoAngles(dir1,dir2);

        }

*/





    /*
    if( pos.x > -25.0 )
    {
            return 180.0;
    }

    if( fabs( pos.y < 10.0 ) )
    {
            return ( Vector2D( -52.5, 0.0 ) - pos ).dir().degree();
    }

    if( pos.x > -37.0 && fabs( pos.y ) > 20.0 )
    {
            return 180.0;
    }

    return ( Vector2D( -52.5, 0.0 ) - pos ).dir().degree();
  */

}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

double Bhv_Block::getOppDribbleSpeed( Vector2D pos )
{

    //return 0.71;
        if( pos.x > 5.0 )
        {
                return 0.5;
        }
        else if( fabs( pos.y ) < 10.0 )
        {
                return 1.0;
        }
        else if( pos.x > -40.0 && fabs( pos.y ) > 20.0 )
        {
                return 1.0;
        }
        else
        {
                return 0.7;
        }
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool Bhv_Block::shouldBlock(PlayerAgent * agent)
{
    const WorldModel & wm = agent->world();

    if( wm.interceptTable()->selfReachCycle()<=wm.interceptTable()->opponentReachCycle() ||
        wm.interceptTable()->teammateReachCycle()<wm.interceptTable()->opponentReachCycle()    )
    {
            return false;
    }

    // yet

    return true;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

int Bhv_Block::getBlocker(PlayerAgent * agent, Vector2D & posBlock)
{
    const WorldModel & wm = agent->world();

//    struct  stc{
//        const AbstractPlayerObject * player ;
//        int   reachCycle;


//    }
//    firstTt,
//    firstOpp,
//    secondTt,
//    secondtOpp,
//    selfInfo;

//    firstTt.player    = wm.interceptTable()->fastestTeammate();
//    firstOpp.player   = wm.interceptTable()->fastestOpponent();
//    secondTt.player   = wm.interceptTable()->secondTeammate();
//    secondtOpp.player = wm.interceptTable()->secondOpponent();
//    selfInfo.player       = wm.self();

//    firstTt.reachCycle      = wm.interceptTable()->teammateReachCycle();
//    firstOpp.reachCycle     = wm.interceptTable()->opponentReachCycle();
//    secondTt.reachCycle     = wm.interceptTable()->secondTeammateReachCycle();
//    secondtOpp.reachCycle   = wm.interceptTable()->secondOpponentReachCycle();
//    selfInfo.reachCycle     = wm.interceptTable()->selfReachCycle();

//    if( firstTt.reachCycle <= firstOpp.reachCycle ){
////        if(secondTt.reachCycle <= firstTt.reachCycle && secondTt.player.pos().dist())
//        return firstTt.player.unum();
//    }


    if( wm.interceptTable()->fastestOpponent() && wm.interceptTable()->selfReachCycle() <= wm.interceptTable()->opponentReachCycle()
        && wm.interceptTable()->selfReachCycle() <= wm.interceptTable()->teammateReachCycle() ){


        posBlock = Vector2D(1000.0,1000.0);
        return wm.self().unum();
    }

    if( wm.interceptTable()->fastestOpponent() && wm.interceptTable()->fastestTeammate() && wm.interceptTable()->teammateReachCycle() <= wm.interceptTable()->opponentReachCycle() ){


            posBlock = Vector2D(1000.0,1000.0);
            return wm.interceptTable()->fastestTeammate()->unum();
        }

        int minConf = 10 ;

        double XFactor = 1.05 ;
        double YFactor = 0.5 ;
        Simul   Ball(agent , SIM_BALL );
        Ball.dynamicsBallSimulate( wm.interceptTable()->opponentReachCycle() );




        struct  stc{
            int playerUnum;
            double playerTime;
            Vector2D pos;

            stc(){
                playerTime = 1000;
                playerUnum = 0;
            }
        }deff,half,ofens;

        // first group

        Vector2D    posBall = Ball.getPosition();
        double      dribbleSpeed = 0.7; //getOppDribbleSpeed(posBall); ;
        double      dribbleAng   = /*180.0; //*/getOppDribbleAngle(agent,posBall);
        vector <Simul> players;

        for (int nr = 2; nr < 6; ++nr) {
            if(wm.ourPlayer(nr) && wm.ourPlayer(nr)->posCount() < minConf && wm.ourPlayer(nr)->distFromBall() < 50.0 ){

                players.push_back(Simul(agent,SIM_TEAMMATE,nr));
            }

        }

        const int   maxCycle = 50;


        for (int i = wm.interceptTable()->opponentReachCycle() ; i < maxCycle; ++i) {

            for (int j = 0; j < players.size(); ++j) {

                double time = players.at(j).reachTime(posBall) * XFactor - YFactor ;

                if(time <= i ){

                    deff.playerUnum = players.at(j).getPlayerNumber();
                    deff.playerTime = time;
                    deff.pos = posBall ;
                    i = maxCycle;
                    break;

                }


            }
            posBall += Vector2D::polar2vector( dribbleSpeed , dribbleAng );
        }


        // second group
        posBall = Ball.getPosition();
//        dribbleSpeed = getOppDribbleSpeed(posBall); ;
//        dribbleAng   = getOppDribbleAngle(agent,posBall);
//        vector <Simul> players;

        players.clear();

        for (int nr = 6; nr < 9; ++nr) {
            if(wm.ourPlayer(nr) && wm.ourPlayer(nr)->posCount() < minConf && wm.ourPlayer(nr)->distFromBall() < 40.0 ){

                players.push_back(Simul(agent,SIM_TEAMMATE,nr));
            }

        }


        for (int i = wm.interceptTable()->opponentReachCycle() ; i < maxCycle; ++i) {

            for (int j = 0; j < players.size(); ++j) {

                double time = players.at(j).reachTime(posBall) * XFactor - YFactor ;

                if(time <= i ){

                    half.playerUnum = players.at(j).getPlayerNumber();
                    half.playerTime = time;
                    half.pos = posBall ;
                    i = maxCycle;
                    break;
                }


            }
            posBall += Vector2D::polar2vector( dribbleSpeed , dribbleAng );
        }

        // second group
        posBall = Ball.getPosition();
//        dribbleSpeed = getOppDribbleSpeed(posBall); ;
//        dribbleAng   = getOppDribbleAngle(agent,posBall);
//        vector <Simul> players;

        players.clear();


        for (int nr = 9; nr < 12; ++nr) {
            if(wm.ourPlayer(nr) && wm.ourPlayer(nr)->posCount() < minConf && wm.ourPlayer(nr)->distFromBall() < 30.0 ){

                players.push_back(Simul(agent,SIM_TEAMMATE,nr));
            }

        }


        for (int i = wm.interceptTable()->opponentReachCycle() ; i < maxCycle; ++i) {

            for (int j = 0; j < players.size(); ++j) {

                double time = players.at(j).reachTime(posBall) * XFactor - YFactor ;

                if(time <= i ){

                    ofens.playerUnum = players.at(j).getPlayerNumber();
                    ofens.playerTime = time;
                    ofens.pos = posBall ;
                    i = maxCycle;
                    break;
                }


            }
            posBall += Vector2D::polar2vector( dribbleSpeed , dribbleAng );
        }



        if(posBall.x > 35.0){
            if(half.playerTime <= ofens.playerTime){


                if(half.playerTime < 2.5)
                    posBlock = Vector2D(1000.0,1000.0);
                else
                    posBlock = half.pos;
                return  half.playerUnum;
            }
            else{

                if(ofens.playerTime < 4.5)
                    posBlock = Vector2D(1000.0,1000.0);
                else
                posBlock = ofens.pos;
                return  ofens.playerUnum;
            }
        }
        else if(posBall.x > -15.0){
            if(half.playerTime <= ofens.playerTime){

                if(half.playerTime < 1.5)
                    posBlock = Vector2D(1000.0,1000.0);
                else
                    posBlock = half.pos;
                return  half.playerUnum;
            }
            else{

                if(ofens.playerTime < 1.5)
                    posBlock = Vector2D(1000.0,1000.0);
                else
                posBlock = ofens.pos;
                return  ofens.playerUnum;
            }
        }
        else{
            if(half.playerTime <= deff.playerTime){

                if(half.playerTime < 1.5)
                    posBlock = Vector2D(1000.0,1000.0);
                else
                    posBlock = half.pos;
                return  half.playerUnum;
            }
            else{

                if(deff.playerTime < 1.5)
                    posBlock = Vector2D(1000.0,1000.0);
                else
                    posBlock = deff.pos;
                return  deff.playerUnum;
            }
        }






//    int iTmp;
//    const PlayerObject * player = wm.interceptTable()->fastestTeammate();

//    if(!player)
//        return 0;

////    ObjectT fastestTea = (ObjectT)(OBJECT_TEAMMATE_1+player->unum()-1);

//    if( wm.ball().pos().x > 0.0 )
//    {
//            return player->unum();
//    }

//    if( (wm.self().pos()).dist( wm.ball().pos() ) > 30.0 &&
//        player->unum() != wm.self().unum() )
//    {
//            return 0;
//    }

//    if( !shouldBlock(agent) || player->unum() == wm.self().unum() && iTmp <= 4 )
//    {
//            return player->unum();
//    }

////    Vector2D posBall = wm.ball().pos();



//    player = wm.interceptTable()->fastestOpponent();

//    Vector2D posOpp=posBall;
//    if(player)
//        posOpp=player->pos();


//    //ObjectT oppObj = (ObjectT)(OBJECT_OPPONENT_1+player->unum()-1);

//    bool checkBall = shouldCheckBall(agent);

//    Vector2D checkPos = checkBall ? posBall : posOpp;

//    double oppDribbleAngle = getOppDribbleAngle( agent,checkPos );

//    vector <int> num;
//    vector <double> dist;

//    for( int i = 1; i <= 11; i++ ) // sort teammates by distance
//    {
//            if( i == wm.ourGoalieUnum() ) continue;

//            if( !wm.ourPlayer(i) ) continue;

//            if( wm.ourPlayer(i)->posCount()>20 ) continue;


//            num.push_back( i );
//            dist.push_back( wm.ourPlayer(i)->pos().dist( posOpp ) );

//            int currentI = num.size()-1;

//            for( int j = currentI - 1 ; j >= 0; j-- )
//            {
//                    if( dist.at( currentI ) < dist.at( j ) )
//                    {
//                            typeSwap( num.at(currentI), num.at(j) );
//                            typeSwap( dist.at(currentI), dist.at(j) );
//                            currentI = j;
//                    }
//            }
//    }

//    for( int i = 0; i < num.size(); i++ )
//    {
//            const AbstractPlayerObject * objTea = wm.ourPlayer(num.at(i));

//            //Vector2D posTea = WM->posTeammate( num.at(i) );

//            double opp2agent = ( (wm.ourPlayer( num.at(i) )->pos()) - posOpp ).dir().degree();

//            if( getAngDiff( oppDribbleAngle, opp2agent ) < 30.0 ) // close && infront of opp
//            {
//                    return objTea->unum();
//            }

//            if( num.size() > i+1 ) // check next dist
//            {
//                    opp2agent = ( (wm.ourPlayer( num.at(i+1) )->pos()) - posOpp ).dir().degree();

//                    if( fabs( dist.at(i) - dist.at(i+1) ) > 10.0 || getAngDiff ( oppDribbleAngle, opp2agent ) > 30.0 )
//                    {
//                            return num.at(i);
//                    }
//            }
//            else
//            {
//                    return num.at(i);
//            }
//    }

//    return player->unum();
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool Bhv_Block::doBlock(PlayerAgent * agent)
{

    const WorldModel & wm = agent->world();

    Vector2D posAgent = (wm.self().pos());
    Vector2D posBall = wm.ball().pos();




    const PlayerObject * player = wm.interceptTable()->fastestOpponent();

    if(!player)
        return closingOnOpp(agent);


/*    if( posBall.x>-5.0 )
    {
         cout<<wm.time().cycle()<<"  22"<<endl;
            return possessingTheBall(agent);
    }
*/
    const AbstractPlayerObject * oppObj = player;
    Vector2D posOpp = player->pos();

    //Log.log( 250, "last Ball kickalbe obj : %d",WM->getLastObjBallKickable());

    bool checkBall = true;//shouldCheckBall(agent);

    Vector2D checkPos = checkBall ? posBall : posOpp;

    double diff = getAngDiff( ( posAgent - posOpp ).dir().degree() ,  getOppDribbleAngle( agent,checkPos ) );
/*
    if( diff < 13.0 && posAgent.dist( checkPos ) < 7.0 && getAngDiff( player->body().degree(), getOppDribbleAngle( agent,posOpp ) ) > 60.0 && getAngDiff( wm.ball().vel().dir().degree(), getOppDribbleAngle( agent,posOpp ) ) > 60.0 )
    {

         cout<<wm.time().cycle()<<"  33"<<endl;
            return possessingTheBall(agent);
    }

    else if( ( diff < 7.0 && posAgent.dist( checkPos ) < 3.0 ) || ( posAgent.dist( checkPos ) < 3.0 && getAngDiff( player->body().degree(), getOppDribbleAngle( agent, posOpp ) ) > 60.0 ) || ( diff < 7.0 && getAngDiff( wm.ball().vel().dir().degree(), getOppDribbleAngle( agent,posOpp ) ) < 10.0 ) || isDangerArea ( posBall ) )
    {
         cout<<wm.time().cycle()<<"  44"<<endl;
            return possessingTheBall(agent);
    }
*/
    if( diff < 15.0 && posAgent.dist( checkPos ) < 5.0 )
        {
             cout<<wm.time().cycle()<<"  44"<<endl;
                return possessingTheBall(agent);
        }
    else
    {
            return closingOnOpp(agent);
    }
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool Bhv_Block::closingOnOpp(PlayerAgent * agent)
{

    const WorldModel & wm = agent->world();
    if(wm.time().cycle()<NewBlock.cycle+NewBlock.plusT){
         cout<<wm.time().cycle()<<"  11"<<endl;
        return Body_GoToPoint2010(NewBlock.end,0.1,100,2,1,0,15 ).execute(agent);
    }
    Vector2D posAgent = (wm.self().pos());
    Vector2D posBall = wm.ball().pos();

    const PlayerObject * player = wm.interceptTable()->fastestOpponent();

    /*if(!player)
        return Body_Intercept2009(false).execute(agent);
*/
    const AbstractPlayerObject * oppObj = player;

    //Vector2D posOpp = player->pos();


    Vector2D pos;

    SimulateAction soc;

    double dribbleSpeed, dribbleAng;

    /*if( posBall.x > 0.0 )
    {
            //Log.log(250,"intercept");
            return intercept(false);
    }*/

    bool checkBall = shouldCheckBall(agent);

    //Log.log(250,"check ball : %d",checkBall);

    int start = timeToGetBall( agent,oppObj );

    //Log.log(250,"time to get ball ( start ) : %d",start);

    pos = posBall;

    double ballSpeed = wm.ball().vel().length();
    double ballDir   = wm.ball().vel().dir().degree();

    bool end = false;

    for( int i = 1; i < 40; i++ )
    {

            //Log.log(250,"----------i : %d",i);
            if( i <= start )
            {
                    //Log.log(250,"i < start");

                    pos += Vector2D::polar2vector( ballSpeed, ballDir );
                    //Log.log(250,"pos += %lf , %lf , POLAR",ballSpeed, ballDir);
                    //Log.log(250,"pos : %lf , %lf",pos.x,pos.y);

                    if( pos.x < -52.5  ) { pos.x = (-52.5); end = true; }
                    if( pos.x >  52.5  ) { pos.x = ( 52.5); end = true; }
                    if( pos.y >  34.0  ) { pos.y = ( 34.0); end = true; }
                    if( pos.y < -34.0  ) { pos.y = (-34.0); end = true; }

                    ballSpeed *= 0.94;

                    // if( i == start && !checkBall )
                    // {
                            // //Log.log(250,"NOW , pos = posOpp");
                            // pos = posOpp;
                    // }
            }
            else
            {
                    //Log.log(250,"dribbling withBall");

                    dribbleSpeed = getOppDribbleSpeed( pos );
                    dribbleAng   = getOppDribbleAngle( agent,pos );
                    //cout<<wm.time().cycle()<<"   ang:  "<<dribbleAng<<endl;
                    pos += Vector2D::polar2vector( dribbleSpeed, dribbleAng );

                    //Log.log(250,"pos += %lf , %lf , POLAR",dribbleSpeed, dribbleAng);
                    //Log.log(250,"pos : %lf , %lf",pos.x,pos.y);

                    if( pos.x < -52.5  ) { pos.x = (-52.5); end = true; }
                    if( pos.x >  52.5  ) { pos.x = ( 52.5); end = true; }
                    if( pos.y >  34.0  ) { pos.y = ( 34.0); end = true; }
                    if( pos.y < -34.0  ) { pos.y = (-34.0); end = true; }
            }

            Simul  agentSim(agent,SIM_AGENT);

            int reach = agentSim.timeTo( pos );

            //Log.log(250,"my Reach time to pos is : %d",reach);

            if( reach <= i )
            {
                    //Log.log( 250,"Reach together at %d cycle",reach);
                    //Log.log( 250,"Block pos : %lf , %lf",pos.x,pos.y);

                    //if( posBall.dist( pos ) < 2 ){ pos = posBall + Vector2D( 2, ( pos - posBall ).dir().degree(),POLAR);  }

                    //Log.log(250,"Final Pos : %lf , %lf",pos.x,pos.y);
//cout<<wm.time().cycle()<<"ha"<<endl;
//                cout<<wm.time().cycle()<<"  G : "<<pos<<"  B : "<<wm.ball().pos()<<"  O : "<<player->pos()<<endl;

                reach-=4;
                typeNormalize(reach,0,10);
                setBlock(agent,pos,reach);

                return Body_GoToPoint2010(pos,0.0,100,2,1,0,15 ).execute(agent);

            }

            if( end )
            {
                    //Log.log(250,"Reach to End, BREAK!!");
                    break;
            }
    }

    // just decrease distance

    //Log.log( 250,"soooo just decrease this");

    dribbleAng = getOppDribbleAngle( agent,posBall );
 cout<<wm.time().cycle()<<"  hear"<<endl;
    return Body_GoToPoint2010(( posBall + Vector2D::polar2vector( 12.0, dribbleAng )),0.1,100 ).execute(agent);
}


Vector2D Bhv_Block::closingOnOppPos(PlayerAgent * agent)
{

    const WorldModel & wm = agent->world();
    Vector2D posAgent = (wm.self().pos());
    Vector2D posBall = wm.ball().pos();

    const PlayerObject * player = wm.interceptTable()->fastestOpponent();

    /*if(!player)
        return Body_Intercept2009(false).execute(agent);
*/
    const AbstractPlayerObject * oppObj = player;

    //Vector2D posOpp = player->pos();


    Vector2D pos;

    SimulateAction soc;

    double dribbleSpeed, dribbleAng;

    /*if( posBall.x > 0.0 )
    {
            //Log.log(250,"intercept");
            return intercept(false);
    }*/

    bool checkBall = shouldCheckBall(agent);

    //Log.log(250,"check ball : %d",checkBall);

    int start = timeToGetBall( agent,oppObj );

    //Log.log(250,"time to get ball ( start ) : %d",start);

    pos = posBall;

    double ballSpeed = wm.ball().vel().length();
    double ballDir   = wm.ball().vel().dir().degree();

    bool end = false;

    for( int i = 1; i < 40; i++ )
    {

            //Log.log(250,"----------i : %d",i);
            if( i <= start )
            {
                    //Log.log(250,"i < start");

                    pos += Vector2D::polar2vector( ballSpeed, ballDir );
                    //Log.log(250,"pos += %lf , %lf , POLAR",ballSpeed, ballDir);
                    //Log.log(250,"pos : %lf , %lf",pos.x,pos.y);

                    if( pos.x < -52.5  ) { pos.x = (-52.5); end = true; }
                    if( pos.x >  52.5  ) { pos.x = ( 52.5); end = true; }
                    if( pos.y >  34.0  ) { pos.y = ( 34.0); end = true; }
                    if( pos.y < -34.0  ) { pos.y = (-34.0); end = true; }

                    ballSpeed *= 0.94;

                    // if( i == start && !checkBall )
                    // {
                            // //Log.log(250,"NOW , pos = posOpp");
                            // pos = posOpp;
                    // }
            }
            else
            {
                    //Log.log(250,"dribbling withBall");

                    dribbleSpeed = getOppDribbleSpeed( pos );
                    double dir1=(Vector2D(-52.5,7)-pos).dir().degree();
                    double dir2=(Vector2D(-52.5,-7)-pos).dir().degree();


                    dribbleAng   = getBisectorTwoAngles(dir1,dir2);//getOppDribbleAngle( agent,pos );
                    //cout<<wm.time().cycle()<<"   ang:  "<<dribbleAng<<endl;
                    pos += Vector2D::polar2vector( dribbleSpeed, dribbleAng );

                    //Log.log(250,"pos += %lf , %lf , POLAR",dribbleSpeed, dribbleAng);
                    //Log.log(250,"pos : %lf , %lf",pos.x,pos.y);

                    if( pos.x < -52.5  ) { pos.x = (-52.5); end = true; }
                    if( pos.x >  52.5  ) { pos.x = ( 52.5); end = true; }
                    if( pos.y >  34.0  ) { pos.y = ( 34.0); end = true; }
                    if( pos.y < -34.0  ) { pos.y = (-34.0); end = true; }
            }

            Simul  agentSim(agent,SIM_AGENT);


            int reach = agentSim.timeTo( pos );

            //Log.log(250,"my Reach time to pos is : %d",reach);

            if( reach <= i )
            {
                    //Log.log( 250,"Reach together at %d cycle",reach);
                    //Log.log( 250,"Block pos : %lf , %lf",pos.x,pos.y);

                    //if( posBall.dist( pos ) < 2 ){ pos = posBall + Vector2D( 2, ( pos - posBall ).dir().degree(),POLAR);  }

                    //Log.log(250,"Final Pos : %lf , %lf",pos.x,pos.y);
//cout<<wm.time().cycle()<<"ha"<<endl;
//                cout<<wm.time().cycle()<<"  G : "<<pos<<"  B : "<<wm.ball().pos()<<"  O : "<<player->pos()<<endl;
                reach-=4;
                typeNormalize(reach,0,10);
                setBlock(agent,pos,reach);

                return pos;

            }

            if( end )
            {
                    //Log.log(250,"Reach to End, BREAK!!");
                    break;
            }
    }

    // just decrease distance

    //Log.log( 250,"soooo just decrease this");

    dribbleAng = getOppDribbleAngle( agent,posBall );
 cout<<wm.time().cycle()<<"  hear"<<endl;
 return ( posBall + Vector2D::polar2vector( 12.0, dribbleAng ));
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool Bhv_Block::possessingTheBall(PlayerAgent * agent)
{

        return Body_Intercept2009(false).execute(agent);
        // ObjectT oppObj = WM->getFastestInSetTo( OBJECT_SET_OPPONENTS, OBJECT_BALL);
    // Vector2D posOpp = WM->getGlobalPosition( oppObj );
        //
        // double bodyAng = wm.self().body().degree();
        // Vector2D posAgent = (wm.self().pos());
//
        // if( getAngDiff( bodyAng, getOppDribbleAngle(posOpp) + 90.0 ) > 12.0 )
        // {
                // return turnBodyToPoint ( posAgent + Vector2D( 5.0, getOppDribbleAngle(posOpp) + 90.0 ) );
        // }
        // else
        // {
                // return SimulateAction( CMD_DASH, 100.0, AngleDeg::normalize_angle( (posOpp-posAgent).dir().degree() - bodyAng ) );
        // }
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool Bhv_Block::isDangerArea( Vector2D pos )
{

        if( pos.x<-10 && fabs( pos.y ) < 22.0 )
                return true;
        return false;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool Bhv_Block::shouldCheckBall(PlayerAgent * agent)
{
//    const WorldModel & wm = agent->world();

//    Vector2D posBall = wm.ball().pos();

//    const PlayerObject * player = wm.interceptTable()->fastestOpponent();

//    if(!player)
//        return true;

//    ObjectT oppObj = (ObjectT)(OBJECT_OPPONENT_1+player->unum()-1);

//    Vector2D posOpp = player->pos();

//    /**make nemdoonam if ( WM->getLastObjBallKickable() == oppObj ) // Opponent is not passing the ball
//    {
//            //Log.log( 250,"checkBall true1");
//            return true;
//    }*/

//    if ( WM->getLastObjBallKickable() != oppObj && ( !WM->isBallTheirs() || wm.ball().vel().length() > 0.7 ) ) // Opponent is passing the ball but opp will continue the way
//    {
//            Vector2D ball = posBall;

//            double ballSpeed = wm.ball().vel().length();
//            double ballDir   = wm.ball().vel().dir().degree();

//            int _time = timeToGetBall( oppObj );

//            if( _time == 1000 ) return false;

//            for( int i = 1; i < _time ; i++ )
//            {
//                    ball += Vector2D( ballSpeed, ballDir, POLAR );
//                    ballSpeed *= 0.94;
//            }

//            if( posOpp.x > ball.x )
//            {
//                    //Log.log( 250,"checkBall true2");
//                    return true;
//            }
//    }

    return false;
}


bool    Bhv_Block::setBlock(PlayerAgent * agent,Vector2D end,int T){


    const WorldModel & wm = agent->world();

    NewBlock.cycle=wm.time().cycle();
    NewBlock.end=end;
    NewBlock.plusT=T;
    return true;






}




////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

double Bhv_Block::ballSpeedAfterTackle( PlayerAgent * agent ,double tackleRelAng )
{

    const WorldModel & wm = agent->world();
    Vector2D posAgent = (wm.self().pos());
    Vector2D posBall = wm.ball().pos();
    double bodyAngle = wm.self().body().degree();
    double ballSpeed = wm.ball().vel().length();
    double ballDirection = wm.ball().vel().dir().degree();

//    ballSpeed *= 0.94;

    double effPower = ( 0.0 + (( 100.0 - 0.0 ) * (1.0 - fabs(  ( tackleRelAng ) ) / 180.0))) * 0.027;
    effPower *= 1.0 - 0.5 * ( fabs (  ( AngleDeg::normalize_angle( ( posBall - posAgent ).dir().degree() - bodyAngle ) ) ) / 180.0);;

    double tackleGlobalAng = AngleDeg::normalize_angle ( tackleRelAng + bodyAngle );

    Vector2D ballVel  = Vector2D::polar2vector( ballSpeed, ballDirection );
    Vector2D tackleVel = Vector2D::polar2vector( effPower, tackleGlobalAng );

    return (ballVel+tackleVel).r();
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

double Bhv_Block::ballDirAfterTackle( PlayerAgent * agent ,double tackleRelAng )
{
    const WorldModel & wm = agent->world();
    Vector2D posAgent = (wm.self().pos());
    Vector2D posBall = wm.ball().pos();
    double bodyAngle = wm.self().body().degree();
    double ballSpeed = wm.ball().vel().length();
    double ballDirection = wm.ball().vel().dir().degree();

//    ballSpeed *= 0.94;

    double effPower = ( 0.0 + (( 100.0 - 0.0 ) * (1.0 - fabs(  ( tackleRelAng ) ) / 180.0))) * 0.027;
    effPower *= 1.0 - 0.5 * ( fabs (  ( AngleDeg::normalize_angle( ( posBall - posAgent ).dir().degree() - bodyAngle ) ) ) / 180.0);;

    double tackleGlobalAng = AngleDeg::normalize_angle ( tackleRelAng + bodyAngle );

    Vector2D ballVel  = Vector2D::polar2vector( ballSpeed, ballDirection );
    Vector2D tackleVel = Vector2D::polar2vector( effPower, tackleGlobalAng );

    return (ballVel+tackleVel).dir().degree();
}

////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////// Tackle ...........

double Bhv_Block::ballSpeedAfterTackle( PlayerAgent * agent ,Vector2D pos )
{
    const WorldModel & wm = agent->world();
    Vector2D posAgent = (wm.self().pos());
    Vector2D posBall = wm.ball().pos();
    double bodyAngle = wm.self().body().degree();
    double ballSpeed = wm.ball().vel().length();
    double ballDirection = wm.ball().vel().dir().degree();

//    ballSpeed *= 0.94;

    double tackleRelAng = AngleDeg::normalize_angle( ( pos - posBall ).dir().degree() - bodyAngle );

    double effPower = ( 0.0 + (( 100.0 - 0.0 ) * (1.0 - fabs(  ( tackleRelAng ) ) / 180.0))) * 0.027;
    effPower *= 1.0 - 0.5 * ( fabs (  ( AngleDeg::normalize_angle( ( posBall - posAgent ).dir().degree() - bodyAngle ) ) ) / 180.0);;

    double tackleGlobalAng = AngleDeg::normalize_angle ( tackleRelAng + bodyAngle );

    Vector2D ballVel   = Vector2D::polar2vector( ballSpeed, ballDirection );
    Vector2D tackleVel = Vector2D::polar2vector( effPower, tackleGlobalAng );

    return (ballVel+tackleVel).r();
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

double Bhv_Block::ballDirAfterTackle( PlayerAgent * agent ,Vector2D pos )
{
    const WorldModel & wm = agent->world();
    Vector2D posAgent = (wm.self().pos());
    Vector2D posBall = wm.ball().pos();
    double bodyAngle = wm.self().body().degree();
    double ballSpeed = wm.ball().vel().length();
    double ballDirection = wm.ball().vel().dir().degree();

//    ballSpeed *= 0.94;

    double tackleRelAng = AngleDeg::normalize_angle( ( pos - posBall ).dir().degree() - bodyAngle );

    double effPower = ( 0.0 + (( 100.0 - 0.0 ) * (1.0 - fabs(  ( tackleRelAng ) ) / 180.0))) * 0.027;
    effPower *= 1.0 - 0.5 * ( fabs (  ( AngleDeg::normalize_angle( ( posBall - posAgent ).dir().degree() - bodyAngle ) ) ) / 180.0);;

    double tackleGlobalAng = AngleDeg::normalize_angle ( tackleRelAng + bodyAngle );

    Vector2D ballVel   = Vector2D::polar2vector( ballSpeed, ballDirection );
    Vector2D tackleVel = Vector2D::polar2vector( effPower, tackleGlobalAng );

    return (ballVel+tackleVel).dir().degree();
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool Bhv_Block::shouldTackle( PlayerAgent * agent ,double &relAng, bool &foul )
{
    const WorldModel & wm = agent->world();
    Vector2D posBall    = wm.ball().pos();
    double      tackleProb = wm.self().tackleProbability();
/***make omaran*/
    if( tackleProb == 0.0 )
    {
            return false;
    }

    if( wm.ball().posCount() != 0 )
    {
            return false;
    }

    foul = false;

    // Log.log ( 250, "WM->getProbTackleSucceeds() = %lf",WM->getProbTackleSucceeds() );
    //
    // Log.log ( 250, "WM->isBallTheirs() = %d",WM->isBallTheirs() );
    // Log.log ( 250, "WM->isBallOurs(true) = %d",WM->isBallOurs(true) );
    // Log.log ( 250, "WM->isBallKickable() = %d",WM->isBallKickable() );
    // Log.log ( 250, "canGetBallNextCycle() = %d",canGetBallNextCycle() );


    if ( !isBallTheirs(agent) && !isBallOurs(agent,true) && !wm.self().isKickable() && !canGetBallNextCycle(agent) ) // cutting !
    {
            if ( isBallHeadingToGoal(agent) ) // cut shoot
            {
                    // Log.log ( 250, "Ball Is Heading To Goal" );

                Line2D l(wm.ball().pos(),ballDirAfterTackle(agent,getTackleRelAng(agent)));
                Line2D l2(Vector2D(-52.5,0.0),90.0);

                Vector2D posIntersect = l.intersection( l2 );

                if( fabs(posIntersect.y) > 7.1 + 2.0)
                {
                            relAng = getTackleRelAng(agent);
                            return true;
                }
            }

            /// cut pass
    }

    // Log.log ( 250, "getLastBallOwner() = %d",WM->getLastBallOwner() );

    if ( isBallTheirs(agent, 1.0 ) /**make i cant && SoccerTypes::isOpponent( WM->getLastBallOwner() ) */)
    {
            if ( posBall.x > 0.0 && posBall.x < 32 && tackleProb > 0.75 )
            {
                    // Log.log ( 250, "wm.self().card() = %d",wm.self().card() );
                    if ( wm.self().card() == NO_CARD )
                            foul = true;

                    relAng = getTackleRelAng(agent);

                    Line2D ownGoal( Vector2D ( -52.5, 0.0 ), 90.0 );
                    Line2D tackleLine( wm.ball().pos(), ballDirAfterTackle(agent, relAng ) );

                    /*if ( fabs ( tackleLine.intersection( ownGoal).y ) < 7.1 + 2.0 )
                        return false;
*/



                    return true;
            }
            else if ( posBall.x < 0.0 && tackleProb > 0.83 )
            {
                    relAng = getTackleRelAng(agent);
                    return true;
            }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool Bhv_Block::canGetBallNextCycle(PlayerAgent * agent )
{
    const WorldModel & wm = agent->world();
    Vector2D posAgent, posBall ;

    if ( wm.self().isKickable() )
    {
            return true;
    }

    if(timeToGetBall(agent,wm.ourPlayer(wm.self().unum()))<=1)
        return true;
    /*posBall  = wm.ball().pos() + Vector2D ( wm.ball().vel().length()*0.94, wm.ball().vel().dir().degree(), POLAR ) ;
    posAgent = WM->predictAgentPosAfterCommand ( intercept(false) );

    if ( posAgent.dist(posBall) < SS->getMaximalKickDist() )
    {
            return true;
    }
    */
    return false;

}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

double Bhv_Block::getTackleRelAng(PlayerAgent * agent )
{
    const WorldModel & wm = agent->world();
    Vector2D posBall = wm.ball().pos();
    double bodyAngle = wm.self().body().degree();

    double min = AngleDeg::normalize_angle ( bodyAngle - 90.0 ) ;

    double score[7]={0.0};

    Line2D ownGoal( Vector2D ( -52.5, 0.0 ), 90.0 );
    Line2D theirGoal( Vector2D ( 52.5, 0.0 ), 90.0 );

    int scoreMaxNum = 3; // default = rel 0.0
    double scoreMax = -1000.0 ;

    if( isBallTheirs(agent, 1.0 ) )
    {
            score[3] += 3.0;
    }

    for ( int i = 0 ; i < 7 ; i++ ) // 7 step
    {
            double ang = AngleDeg::normalize_angle ( min + i*30.0 );
            double rel = -90.0 + (i*30.0);

            double dir = ballDirAfterTackle( agent,rel );
            double speed = ballSpeedAfterTackle(agent, rel );
            Vector2D pos = posBall + Vector2D::polar2vector( speed, dir );

            if( isBallTheirs(agent, 1.0, pos ) )
            {
                    score[i] -= 100.0;
            }

            if ( posBall.x < -10.0 )
            {
                    Line2D tackleLine( posBall, ballDirAfterTackle(agent, rel ) );

                    if ( fabs ( tackleLine.intersection( ownGoal).y ) < 7.1 + 2.0 )
                    {
                            if( isDangerArea( posBall ) )
                            {
                                    score[i] -= 100.0;
                                    continue;
                            }
                            else
                            {
                                    score[i] -= 2.0;
                            }
                    }
            }

            score[i] -= getOppInCircle(agent, Circle2D( posBall + Vector2D::polar2vector( 15.0, AngleDeg::normalize_angle( bodyAngle + rel ) ) , 15.0 ) );
            score[i] += getOppInCircle(agent, Circle2D( posBall + Vector2D::polar2vector( 15.0, AngleDeg::normalize_angle( bodyAngle + rel ) ) , 15.0 ) );

            if ( score[i] > scoreMax )
            {
                    scoreMax = score[i];
                    scoreMaxNum = i ;
            }
    }


    return ( -90.0 + (scoreMaxNum*30.0) );
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool Bhv_Block::tackleToPos ( PlayerAgent * agent ,Vector2D pos, SimulateAction &soc )
{
    const WorldModel & wm = agent->world();
    /// should recognize with DirAfterTackle & ......    uncompelete ...

    if ( wm.self().tackleProbability() < 0.9 ) return false;

    Vector2D posBall = wm.ball().pos();

    double bodyAngle = wm.self().body().degree();

    /*double min = AngleDeg::normalize_angle ( bodyAngle - 90.0 ) ;
    double max = AngleDeg::normalize_angle ( bodyAngle + 90.0 ) ;
*/
    double tackleAng = AngleDeg::normalize_angle (( pos - posBall ).dir().degree() -wm.self().body().degree());

    if ( isAngInInterval( tackleAng, -90.0, +90.0 ) )
    {
            double relTackleAng = AngleDeg::normalize_angle( tackleAng - bodyAngle );

            agent->doTackle(relTackleAng);

            return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////// ......
bool Bhv_Block::isBallHeadingToGoal( PlayerAgent * agent  )
    {
    const WorldModel & wm = agent->world();

    if( wm.ball().posCount()>5 ||
    fabs( wm.ball().pos().x ) <  -41.5 - 5.0 )
    {
    return false;
    }

    // make line from ball heading and goal line

    Line2D l = Line2D(wm.ball().pos(),wm.ball().vel().dir().degree());
    Line2D l2= Line2D( Vector2D(-52.5,0.0),90.0);

    // if intersection is outside goalwidth, not heading to goal
    Vector2D posIntersect = l.intersection( l2 );

    if( fabs(posIntersect.y) > 7.1 + 2.0)
    {

    return false;
    }
    Simul   BALL(agent,SIM_BALL);

    if(-52.5>BALL.finalSim(0.7).getPosition().x)
        return true;
    return false;

}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool Bhv_Block::isBetween( double ang, double min, double max)
{
        if ( min < max )
        {
                if ( ang < max && ang > min )
                        return true;
                return false;
        }
        else
        {
                if ( ang < max && ang > min )
                        return false;
                return true;
        }
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool Bhv_Block::isBallTheirs( PlayerAgent * agent ,double kickable, Vector2D pos )
{
    const WorldModel & wm = agent->world();
    int i;

    if( pos == Vector2D( -1000, -1000 ) )
            pos = wm.ball().pos();


    for( int i=1 ; i < 12 ;	i++)
        if(wm.theirPlayer(i) && (wm.theirPlayer(i)->pos()).dist( pos ) < kickable /* coach? */ + 0.385 )
            return true;

    return false;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool Bhv_Block::isBallOurs( PlayerAgent * agent ,bool b )
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
int Bhv_Block::getOppInCircle(PlayerAgent * agent ,Circle2D cir){


    const WorldModel & wm = agent->world();
    int nr=0;
    for( int i=1 ; i < 12 ;	i++)
        if(wm.theirPlayer(i) && (wm.theirPlayer(i)->pos()).dist(cir.center())<cir.radius())
            nr++;


    return nr;






}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
int Bhv_Block::getTtInCircle(PlayerAgent * agent ,Circle2D cir){


    const WorldModel & wm = agent->world();
    int nr=0;
    for( int i=1 ; i < 12 ;	i++)
        if(wm.ourPlayer(i) && wm.ourPlayer(i)->pos().dist(cir.center())<cir.radius())
            nr++;


    return nr;






}
///^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
///^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
bool    Bhv_Block::goTo(PlayerAgent *agent,Vector2D posTo,double ang,double maxDashPower,double disWhenTrue){


    Vector2D agentPos = agent->world().self().pos();
    Line2D line1 = Line2D(agentPos,AngleDeg::normalize_angle(90+ang));
    Line2D line2 = Line2D(posTo,AngleDeg::normalize_angle(ang));
    Vector2D    posGo = line1.intersection(line2);

    int sign=1;
    if((posGo-agentPos).dir().degree()<0)
        sign=-1;

    if(posGo.dist(posTo)>3)
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

    return false;


}



bool Bhv_Block::tackle( rcsc::PlayerAgent * agent )
{
    const rcsc::WorldModel & wm = agent->world();

//    if(wm.existKickableTeammate())
//        return false;



    rcsc::Vector2D self_pos = wm.self().pos() ;
    rcsc::Vector2D opp_pos(1000,1000) ;

    if( wm.getOpponentNearestToBall( 3 , true ) )
        opp_pos = wm.getOpponentNearestToBall( 3 , true )->pos() ;

    double s_dist = self_pos.dist( wm.ball().pos() ) ;
    double o_dist = opp_pos.dist( wm.ball().pos()  ) ;

    if( wm.existKickableOpponent() && wm.ball().posCount() < 2 && s_dist < rcsc::ServerParam::i().tackleDist() && !wm.existKickableTeammate()
     && wm.self().tackleProbability() > 0.85  )
    {
        cout<<"Mohsen Tackle"<<endl<<endl ;
        return true ;
    }
    return false ;
}

bool Bhv_Block::bTackle ( rcsc::PlayerAgent * agent)
{


    const rcsc::WorldModel & wm = agent->world();

//    if(wm.existKickableTeammate())
//        return false;


        double bodyAngle = wm.self().body().degree() ;
        double dis1=1.75;
        double dis2=0.70;
        double dis3=0.75;
        double ang1=bodyAngle-90.0;
        double ang2=bodyAngle+90;



        rcsc::Vector2D posAgent = wm.self().pos() ;
        rcsc::Vector2D top = wm.self().pos() + Vector2D::polar2vector ( dis1 , bodyAngle  ) ;
        //rcsc::Vector2D topLeft = top + Vector2D::polar2vector ( dis2 , ang1 );
        //rcsc::Vector2D downRight = posAgent + Vector2D::polar2vector ( dis3 , ang2 ) ;

        rcsc::Vector2D topLeft=wm.self().pos()+ Vector2D::polar2vector(1.75,wm.self().body().degree()) + Vector2D::polar2vector(0.7,wm.self().body().degree()-90);
        rcsc::Vector2D downRight=wm.self().pos()+Vector2D::polar2vector(0.75,wm.self().body().degree()+90);

        rcsc::Rect2D rect ( topLeft , downRight ) ;

        if ( rect.contains( wm.ball().pos() ) )
        {
                // //Log.log ( 200 , " posBall is Inside Rectangle " ) ;
                // //Log.log ( 200 , " Return True " ) ;
                return true ;
        }

        // //Log.log ( 200 , " Return False " ) ;
        return false ;
}

