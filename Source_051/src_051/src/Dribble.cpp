#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <vector>

#include "bhv_basic_offensive_kick.h"
#include "Dribble.h"
#include <rcsc/action/body_dribble.h>
#include <rcsc/action/body_hold_ball.h>
#include <rcsc/action/neck_scan_field.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>
#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/player/self_intercept_v13.h>
#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>
#include <rcsc/geom/sector_2d.h>
#include <iostream>
//#include "mostafPredicte.h"
#include "rcsc/action/body_smart_kick.h"
#include "rcsc/action/body_intercept2009.h"
#include <rcsc/action/basic_actions.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/neck_turn_to_ball_or_scan.h>
#include <rcsc/geom/ray_2d.h>
#include <rcsc/soccer_math.h>
#include <rcsc/math_util.h>
#include <rcsc/timer.h>
///*make*/#include "neck_offensive_intercept_neck.h"




//using namespace std ;

///*-------------------------------------------------------------------*/
//using rcsc::Vector2D ;

//rcsc::Vector2D Bhv_SmartDribble::getDribblePos(rcsc::PlayerAgent * agent)
//{
//    const rcsc::WorldModel & wm = agent->world();
//    const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();

//    double asi;

//    _cone cone ( agent->world().self().pos() , m_ang , m_dist ) ;


//    if(fabs(wm.self().pos().y)>19 && wm.self().pos().x < 36 )
//        asi = 0 ;

//    else if ( agent->world().self().pos().x > 48 && wm.self().pos().absY() < 10 )
//        asi = (Vector2D (45.5,0.0) - agent->world().self().pos()).th().degree() ;
//    else
//    {

//        if( wm.self().pos().y > 20 )
//            asi = (Vector2D (52.5,7.0) - agent->world().self().pos()).th().degree() ;
//        else if( wm.self().pos().y < -20 )
//            asi = (Vector2D (52.5,-7.0) - agent->world().self().pos()).th().degree() ;
//        else
//            asi = (Vector2D (52.5,0.0) - agent->world().self().pos()).th().degree() ;
//    }

//    std::vector <Vector2D> v1 ;

//    bool opp_in = false ;
//    const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();
//    for ( rcsc::PlayerPtrCont::const_iterator it = opps.begin();
//           it != opps_end;
//           ++it )
//    {
//        if( wm.getOpponentGoalie() && (*it) == wm.getOpponentGoalie() )
//            continue ;

//        Vector2D pos_opponent = (*it)->pos() ;
//        if ( cone.isInCone ( pos_opponent , asi ) )
//        {
//                opp_in = true ;
//                v1.push_back (pos_opponent) ;
//        }
//    }
//   // std::cout << " v1 size: " << v1.size() << std::endl;
//    if ( !opp_in )
//    {
//                //std::cout << " return asi " << asi << std::endl;
//        return wm.self().pos() + Vector2D :: polar2vector ( DIST_FRONT_DEFUALT , asi ) ;
//    }

//    std::vector <double> v2 ;

//    double temp ;
//    for ( int i = 0 ; i < v1.size() ; ++i )
//    {
//                //std::cout << " opponent " << i+1 << std::endl;
//                temp = (v1.at(i) - agent->world().self().pos()).th().degree() ;
//                v2.push_back ( _cone::normalizeAngle(temp + (m_ang+1)) ) ;
//                v2.push_back ( _cone::normalizeAngle(temp - (m_ang+1)) ) ;
//                //std::cout << " angleToMe : " << temp << std::endl;
//                //std::cout << " right: " << _cone::normalizeAngle(temp + (m_ang+1)) << std::endl;
//                //std::cout << " left: " << _cone::normalizeAngle(temp - (m_ang+1)) << std::endl;
//        }

//    std::vector <double> v3 ;

//    for ( int i = 0 ; i < v2.size() ; ++i )
//    {
//                //std::cout << " angle " << i << std::endl;
//                //std::cout << " ang: " << v2.at(i) << std::endl;
//                opp_in = false ;
//                const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();
//                for ( rcsc::PlayerPtrCont::const_iterator it = opps.begin();
//                           it != opps_end;
//                           ++it )
//                {
//                        if( wm.getOpponentGoalie() && (*it) == wm.getOpponentGoalie() )
//                            continue ;

//                        Vector2D pos_opponent = (*it)->pos() ;
//                        if ( cone.isInCone ( pos_opponent , v2.at(i) ) )
//                        {
//                                        opp_in = true ;
//                                        break ;
//                        }
//                }
//                if ( opp_in )
//                {
//                //	std::cout << " opponent in " << std::endl;
//                        continue ;
//                }
//                else
//                {
//                //	std::cout << " no opponent in angle " << std::endl;
//                        v3.push_back ( v2.at(i) ) ;
//                }
//        }

//        //std::cout << " size of v3 " << v3.size() << std::endl;

//        if ( v3.size() > 1 )
//        {
//        //	std::cout << " get min " << std::endl;
//                double d_min = 10000 ;
//                int i_min ;
//                for ( int i = 0 ; i < v3.size() ; ++i )
//                {
//        //		std::cout << " angle " << i << std::endl;
//        //		std::cout << " ang " << v3.at(i) << std::endl;
//        //		std::cout << " asi " << asi << std::endl;
//        //		std::cout << " dif " << fabs(_cone::normalizeAngle(v3.at(i)-asi)) << std::endl;
//                        if ( fabs(_cone::normalizeAngle(v3.at(i)-asi)) < d_min )
//                        {
//                                d_min = fabs(_cone::normalizeAngle(v3.at(i)-asi)) ;
//                                i_min = i ;
//                        }
//                }
//        //	std::cout << " return min " << v3.at(i_min) << std::endl;
//                return wm.self().pos() +
//                                Vector2D::polar2vector( DIST_FRONT_DEFUALT , v3.at(i_min) ) ;
//        }
//        else if ( v3.size() == 1 )
//        {
//        //	std::cout << " return 0om " << v3.at(0) << std::endl;
//                return wm.self().pos() + Vector2D :: polar2vector ( DIST_FRONT_DEFUALT , v3.at(0) ) ;
//        }
//        else
//        {
//        //	std::cout << " again do it " << std::endl;
//                m_ang *= 0.75 ;
//                m_dist *= 0.95 ;
//                return getDribblePos(agent) ;
//        }
//}

//bool
//Bhv_SmartDribble::execute( rcsc::PlayerAgent * agent )
//{

//    const rcsc::WorldModel & wm = agent -> world();
//    rcsc::Vector2D pos=getDribblePos(agent);


//    if(pos.y>33)
//        pos.y=33;

//    if(pos.y<-33)
//        pos.y=-33;

//    if(pos.x>5)
//        pos.x=50;

//    if(pos.x<-50)
//        pos.x=-50;


//        /*if( wm.self().stamina() > 5000 )
//        {
//            double ang1 = ( wm.self().pos().x > 30 ? ( Vector2D(52,0)-wm.self().pos()).th().degree() : 0 ) ;
//            double ang  = (pos-wm.self().pos()).th().degree();
//            double ang2 = ( wm.self().pos().y > 0 ? ang1+35 : ang1-35 ) ;

//            double dis1 ;
//            double dis  ;
//            double dis2 ;


//            double speed1 = getLenght( agent , ang1 , 3 , &dis1 ) ;
//            double speed  = getLenght( agent , ang  , 3 , &dis  ) ;
//            double speed2 = getLenght( agent , ang2 , 3 , &dis2 ) ;


//            rcsc::Vector2D pos1=wm.ball().pos()+ Vector2D::polar2vector( DIST_FRONT_DEFUALT , ang1 ) ;
//            rcsc::Vector2D pos2=wm.ball().pos()+ Vector2D::polar2vector( DIST_FRONT_DEFUALT , ang2 ) ;


//            double g_dis = 0 ;


//            bool p_area = false ;
//            if( wm.ball().pos().x > 35 && wm.ball().pos().absY() < 21 )
//                p_area = false ;


//            int max_c ,  ave_c ;

//            wm.dirRangeCount( ang1 , dis1+1 , &max_c, NULL, &ave_c ) ;
//            if( dis1 > 6 && ave_c <= 1 && max_c <= 2 && dis1 > g_dis && ! p_area )
//            {
//                cout << "canDribbleFast1  " << wm.time().cycle() << endl ;
//                if(rcsc::Body_KickOneStep(pos1,speed1).execute(agent))
//                    return true;
//            }

//            wm.dirRangeCount( ang2, dis2+1 , &max_c, NULL, &ave_c ) ;
//            if( wm.self().pos().absY() > 18 && dis2 > 6 && ave_c <= 1 && max_c <= 2  && dis2 > g_dis && ! p_area  )
//            {
//                cout << "canDribbleFast2  " << wm.time().cycle() << endl;
//                if(rcsc::Body_KickOneStep(pos2,speed2).execute(agent))
//                    return true;
//            }

//            wm.dirRangeCount( ang , dis+1 , &max_c, NULL, &ave_c ) ;
//            if( dis > 6 && ave_c <= 1 && max_c <= 2 && dis > g_dis && ! p_area  )
//            {
//                cout << "canDribbleFast  " << wm.time().cycle() << endl;
//                if(rcsc::Body_KickOneStep(pos,speed).execute(agent))
//                    return true;
//            }
//        }*/

//    /*double ang = (pos - wm.self().pos()).dir().degree();
//    double d;
//    double speed = Predict(wm).best_speed_for_dribble(wm.self(),wm.ball().vel().length(),wm.ball().vel().dir().degree(),ang,2,&d);
//    if( speed > 0 )
//    {
//        cout<<"Fast"<<speed<<" "<<wm.time().cycle()<<endl;
//        if(rcsc::Body_KickOneStep(pos,speed).execute(agent))
//            return true;
//    }*/


//    double dir = (pos-wm.self().pos()).th().degree() ;


////    if( Bhv_SelfPass().execute(agent) )
//  //      return true ;


//    double dash_power=100;


//    if( wm.getOpponentGoalie() )
//        if( wm.self().pos().dist( wm.getOpponentGoalie()->pos() ) < 2.5 )
//            return rcsc::Body_HoldBall2008().execute( agent ) ;


//    if( wm.getOpponentGoalie() )
//        if( wm.self().pos().dist( wm.getOpponentGoalie()->pos() ) < 5.5 )
//            dash_power=100;



//    double thr = 8.5 ;
//    if( bWithDodge( agent ) )
//        thr = 0.5 ;
//    else if( wm.self().pos().x > 35 )
//        thr = 1.5 ;
///*

//    if( wm.self().pos().x > 35 &&
//     rcsc::Body_Dribble2008( pos2,
//                            thr ,
//                            dash_power  ,
//                            100.0 ,
//                            bWithDodge( agent )
//                            ).execute( agent ) )
//        return true ;
//*/

//    return
//    rcsc::Body_Dribble2008( pos  ,
//                            thr ,
//                            dash_power ,
//                            100.0 ,
//                            bWithDodge(agent)
//                            ).execute( agent ) ;

//}

//bool
//Bhv_SmartDribble::bWithDodge( rcsc::PlayerAgent * agent )
//{
//    const WorldModel & wm = agent->world() ;
//    Vector2D self_pos = wm.self().pos() ;
//    Vector2D target ;

//    if( self_pos.x > 35 && self_pos.absY() > 17 )
//        target = Vector2D( 52.5 , 0 ) ;
//    else
//        target = Vector2D( 10 , 0 ) ;

//    double ang = ( target - self_pos ).th().degree() ;


//    self_pos = Vector2D( wm.self().pos().x - 1 , wm.self().pos().y ) ;

//    Vector2D pos_r = self_pos + Vector2D::polar2vector( 3.5 , ang + 90 ) ;
//    Vector2D pos_l = self_pos + Vector2D::polar2vector( 3.5 , ang - 90 ) + Vector2D::polar2vector( 6.0 , ang ) ;

//    Rect2D rect( pos_l , pos_r ) ;


//    if( wm.getOpponentGoalie() && wm.self().pos().dist( wm.getOpponentGoalie()->pos() ) < 4 )
//        return true ;


//    Vector2D pos_close( 1000 , 1000 )  ;
//    if( wm.getOpponentNearestToSelf( 3 , true ) )
//        pos_close = wm.getOpponentNearestToSelf( 3 , true )->pos() ;


//    if( self_pos.x > 35 && self_pos.absY() > 17 && self_pos.x < 48 )
//        return true ;

//    if( rect.contains( pos_close ) && self_pos.x < 45 )
//        return true ;

//    if( pos_close.dist( self_pos ) < 4.5 && pos_close.x >= self_pos.x && pos_close.y < self_pos.y+3.5 && pos_close.y > self_pos.y-3.5 )
//        return true ;

//    if( self_pos.x > 30 && wm.ourPlayer( 11 ) && ( wm.ourPlayer(11)->posCount() > 3 || wm.ourPlayer(11)->pos().x < 35 )  )
//        return true ;

//    return false ;

//}


//double Bhv_SmartDribble::getDribbleLen(rcsc::PlayerAgent * agent ,int num,double ang, rcsc::Vector2D myPos , rcsc::Vector2D oppPos )
//{

//    const rcsc::WorldModel & wm = agent->world();
//    rcsc::Vector2D endPos;

//    for(double i=0.1 ; i<=15 ; i+=0.1)
//    {

//        endPos=wm.ball().pos()+Vector2D::polar2vector(i,ang);
//        for(int j=1 ; j<=11 ; j++)
//            if(wm.theirPlayer(j))
//            if(endPos.dist(wm.theirPlayer(j)->pos()) < wm.self().pos().dist(endPos) || endPos.x > 50 || fabs(endPos.y)>30  )
//            return i;
//    }

//    return 15;

//}



//double
//Bhv_SmartDribble::getLenght(rcsc::PlayerAgent *agent, double ang, int cycle, double *dist)
//{
//    const rcsc::WorldModel & wm = agent->world();
//    double x = 0;
//    Vector2D pos = wm.self().pos();


//    for( double i = 3 ; i > 0.15 ; i -= 0.15 )
//    {
//        int cycle_self = 0   ;
//        int cycle_opp  = -10 ;

//        Interception inter( wm.ball().pos() , i , ang ) ;

//        Vector2D vel = wm.self().vel()  ;
//        AngleDeg ang = wm.self().body() ;

//        cycle_self = inter.getReachCycle( wm.self().pos() , &vel , &ang
//                                                          , wm.self().posCount() , wm.self().playerType().kickableArea(), wm.self().playerType().playerSpeedMax() ) ;
//     // Predict(wm).best_point( wm.self() , wm.ball().pos() , wm.ball().pos()+Vector2D::polar2vector( i*0.94 , ang ) , i , i*0.94 , &cycle_self ) ;

//        for( PlayerPtrCont::const_iterator it = wm.opponentsFromBall().begin();
//             it != wm.opponentsFromBall().end();
//             it++)
//        {
//            int c ;


//            AngleDeg ang = (*it)->body() ;
//            Vector2D vel = (*it)->vel()  ;

//            c = inter.getReachCycle( (*it)->pos() , &vel , &ang , (*it)->posCount()
//                                    , wm.ourPlayerType( (*it)->unum() )->kickableArea(), wm.ourPlayerType((*it)->unum())->playerSpeedMax() ) ;

//            //Predict(wm).best_point( *(*it) , wm.ball().pos() , wm.ball().pos()+Vector2D::polar2vector( i*0.94 , ang ) , i , i*0.94 , &c ) ;

//            if( (*it) == wm.getOpponentGoalie() )
//                c -= 2 ;

//            if( c < cycle_opp )
//                cycle_opp = c ;
//        }

//        if( cycle_opp - cycle_self >= cycle )
//        {
//            *dist = calc_sum_geom_series( i , 0.94 , cycle_self ) ;
//            return i ;
//        }
//    }

///*
//    for( double i = 20;i >= 2 ; i -= 0.25 )
//    {
//        Vector2D pos1 = pos+Vector2D::polar2vector( i,ang );
//        int cycle_opp = Predict(wm).fastest_opp_to_point_time(wm,pos1);
//        int cycle_self = Predict(wm).predict_nr_cycle_to( pos1,wm.self() );
//        if( (cycle_opp - cycle_self >= cycle)  && pos1.x<50  && fabs(pos1.y)<30.5 )
//        {
//            x = i;
//            break ;
//        }
//    }
//    if( x == 0)
//        return 0;
//    else
//    {
//        *dist = x;
//        Vector2D pos2 = wm.ball().pos()+Vector2D::polar2vector(x,ang);
//        int cycle = Predict(wm).predict_nr_cycle_to( pos2,wm.self() );

//                return calc_first_term_geom_series( x , 0.94 , cycle );

//    }
//    */
//}


//double Bhv_SmartDribble::getDirectionOfWidestAngle(rcsc::PlayerAgent * agent , double a, double b,int cycle){

//    const rcsc::WorldModel & wm = agent -> world();
//    int max=-10;
//    double dis,ang;

//    for(int i=a ; i<=b ; i+=3)
//    {
//        double speed=getLenght( agent , i , cycle , &dis);
//        if(dis>max)
//        {
//            max=dis;
//            ang=i;
//        }
//    }

//    return ang;
//}



//bool Bhv_SmartDribble::noDodge(rcsc::PlayerAgent * agent)
//{
//    const rcsc::WorldModel & wm = agent->world();
//    rcsc::Vector2D self_pos=wm.self().pos();
//    double ang=(getDribblePos(agent)-self_pos).th().degree();

//    if(self_pos.dist(rcsc::Vector2D(57,0)) < 20)
//    return true;
//    return false;
//}



/*-------------------------------------------------------------------*/
/*!

*/
bool
IntentionSelfPass::finished( const rcsc::PlayerAgent * agent )
{
    if ( M_step <= 0 )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": finished() empty queue" );
        return true;
    }

    if ( M_count >= 1
         && agent->world().self().isKickable() )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": finished() kickable" );
        return true;
    }

    if ( agent->world().existKickableOpponent()
         || agent->world().existKickableTeammate() )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": finished() exist kickable player" );
        return true;
    }

    if ( agent->world().ball().pos().x < agent->world().self().pos().x )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": finished() ball is back." );
        return true;
    }

    return false;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
IntentionSelfPass::execute( rcsc::PlayerAgent * agent )
{
    --M_step;
    ++M_count;

    if ( M_count <= 3 )
    {
        if ( rcsc::Body_GoToPoint( M_target_point, 0.3,
                                   rcsc::ServerParam::i().maxPower()
                                   ).execute( agent ) )
        {
            agent->setNeckAction( new rcsc::Neck_ScanField() );

            agent->debugClient().addMessage( "I_SelfPassGoTo" );
            agent->debugClient().setTarget( M_target_point );
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": intention. go to (%.1f %1.f) ",
                                M_target_point.x, M_target_point.y );
            return true;
        }
    }
    else
    {
        rcsc::Body_Intercept2009().execute( agent );

#if 0
//        int self_min = agent->world().interceptTable()->selfReachCycle();
//        int opp_min = agent->world().interceptTable()->opponentReachCycle();
//        if ( self_min == 4 && opp_min >= 2 )
//        {
//            agent->setViewAction( new rcsc::View_Wide() );
//        }
//        else if ( self_min == 3 && opp_min >= 2 )
//        {
//            agent->setViewAction( new rcsc::View_Normal() );
//        }
//        agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );
#else
        agent->setNeckAction(  /***make new Neck_OffensiveInterceptNeck()*/ new rcsc::Neck_TurnToBallOrScan());
#endif

        agent->debugClient().addMessage( "I_SelfPassIntercept" );
        agent->debugClient().setTarget( M_target_point );
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": intention. intercept (%.1f %1.f) ",
                            M_target_point.x, M_target_point.y );
            return true;
    }

    return false;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
Bhv_SelfPass::execute2( rcsc::PlayerAgent * agent )
{
    if ( ! agent->world().self().isKickable() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << " not ball kickable!"
                  << std::endl;
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__":  not kickable" );
        return false;
    }

    rcsc::dlog.addText( rcsc::Logger::TEAM,
                        __FILE__": Bhv_SelfPass" );

    const rcsc::WorldModel & wm = agent->world();

    rcsc::MSecTimer timer;

    const rcsc::Ray2D body_ray( wm.self().pos(), wm.self().body() );
    const double goal_area_x
        = rcsc::ServerParam::i().pitchHalfLength()
        - rcsc::ServerParam::i().goalAreaLength() * 0.5;
    const rcsc::Rect2D target_area = rcsc::Rect2D::from_center( goal_area_x,
                                                                0.0,
                                                                rcsc::ServerParam::i().goalAreaLength(),
                                                                30.0 );

    /*if ( wm.self().body().abs() < 25.0
         || target_area.intersection( body_ray, NULL, NULL ) > 0 )
    {
        agent->debugClient().addRectangle( target_area );
        if ( doKickDashes( agent ) )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": self_pass_elapsed(1)=%.3f [ms]",
                                timer.elapsedReal() );
            return true;
        }
    }*/
//#if 0
    //for ( double r = 0.0; r < 1.2; r += 0.1 )
    /*for ( double r = 0.9; r >= 0.0; r -= 0.1 )
    {
        rcsc::Vector2D target_point( 52.0, wm.self().pos().y * r );
        if ( doKickTurnDashes( agent, target_point ) )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": self_pass_elapsed(2)=%.3f [ms]",
                                timer.elapsedReal() );
            return true;
        }
    }*/
//#else
    std::vector< double > target_y;
    for ( double y = 0.0;
          y < wm.self().pos().absY() && y < 30.0;
          y += 1.0 )
    {
        target_y.push_back( y );
    }
    target_y.push_back( wm.self().pos().absY() );

    for ( std::vector< double >::iterator y = target_y.begin();
          y != target_y.end();
          ++y )
    {
        rcsc::Vector2D target_point( 52.0, *y );
        if ( wm.self().pos().y < 0.0 ) target_point.y *= -1.0;
        //if ( std::fabs( target_point.y - wm.self().pos().y ) > 16.0 ) continue;
        if ( ( target_point - wm.self().pos() ).th().abs() > 40.0 ) continue;

        if ( doKickTurnDashes( agent, target_point ) )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": self_pass_elapsed(2)=%.3f [ms]",
                                timer.elapsedReal() );
            return true;
        }
    }
//#endif

    rcsc::dlog.addText( rcsc::Logger::TEAM,
                        __FILE__": self_pass_elapsed(3)=%.3f [ms]",
                        timer.elapsedReal() );

    return false;
}
bool
Bhv_SelfPass::execute( rcsc::PlayerAgent * agent )
{
    if ( ! agent->world().self().isKickable() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << " not ball kickable!"
                  << std::endl;
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__":  not kickable" );
        return false;
    }

    rcsc::dlog.addText( rcsc::Logger::TEAM,
                        __FILE__": Bhv_SelfPass" );

    const rcsc::WorldModel & wm = agent->world();

    rcsc::MSecTimer timer;

    const rcsc::Ray2D body_ray( wm.self().pos(), wm.self().body() );
    const double goal_area_x
        = rcsc::ServerParam::i().pitchHalfLength()
        - rcsc::ServerParam::i().goalAreaLength() * 0.5;
    const rcsc::Rect2D target_area = rcsc::Rect2D::from_center( goal_area_x,
                                                                0.0,
                                                                rcsc::ServerParam::i().goalAreaLength(),
                                                                30.0 );

    if ( wm.self().body().abs() < 45.0
         || target_area.intersection( body_ray, NULL, NULL ) > 0 )
    {
        agent->debugClient().addRectangle( target_area );
        if ( doKickDashes( agent ) )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": self_pass_elapsed(1)=%.3f [ms]",
                                timer.elapsedReal() );
            return true;
        }
    }
//#if 0
    //for ( double r = 0.0; r < 1.2; r += 0.1 )
    for ( double r = 0.9; r >= 0.0; r -= 0.1 )
    {
        rcsc::Vector2D target_point( 42.0, wm.self().pos().y * r );
        if ( doKickTurnDashes( agent, target_point ) )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": self_pass_elapsed(2)=%.3f [ms]",
                                timer.elapsedReal() );
            return true;
        }
    }
//#else
    std::vector< double > target_y;
    for ( double y = 0.0;
          y < wm.self().pos().absY() && y < 30.0;
          y += 1.0 )
    {
        target_y.push_back( y );
    }
    target_y.push_back( wm.self().pos().absY() );

    for ( std::vector< double >::iterator y = target_y.begin();
          y != target_y.end();
          ++y )
    {
        rcsc::Vector2D target_point( 42.0, *y );
        if ( wm.self().pos().y < 0.0 ) target_point.y *= -1.0;
        //if ( std::fabs( target_point.y - wm.self().pos().y ) > 16.0 ) continue;
        if ( ( target_point - wm.self().pos() ).th().abs() > 40.0 ) continue;

        if ( doKickTurnDashes( agent, target_point ) )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": self_pass_elapsed(2)=%.3f [ms]",
                                timer.elapsedReal() );
            return true;
        }
    }
//#endif

    rcsc::dlog.addText( rcsc::Logger::TEAM,
                        __FILE__": self_pass_elapsed(3)=%.3f [ms]",
                        timer.elapsedReal() );

    return false;
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_SelfPass::doKickDashes( rcsc::PlayerAgent * agent )
{
    static std::vector< rcsc::Vector2D > self_state;

    rcsc::dlog.addText( rcsc::Logger::TEAM,
                        "__ selfPass doKickDashes" );

    const rcsc::WorldModel & wm = agent->world();

    createSelfCache( agent, 0, 18,
                     wm.self().body(),
                     self_state ); // no_turn, max_dash=18

    int n_dash = self_state.size();

    if ( n_dash < 6 )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": too short dash step %d",
                            n_dash );
        return false;
    }

    for ( ; n_dash >= 6; n_dash -= 2 )
    {
        const rcsc::Vector2D receive_pos = self_state[n_dash - 1];

        if ( ! canKick( agent, 0, n_dash, receive_pos ) )
        {
            continue;
        }

        if ( ! checkOpponent( agent, 0, n_dash, receive_pos ) )
        {
            continue;
        }

        if ( doKick( agent, 0, n_dash, receive_pos ) )
        {
            return true;
        }
    }

    return false;
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_SelfPass::doKickTurnDashes( rcsc::PlayerAgent * agent,
                                const rcsc::Vector2D & target_point )
{
    static std::vector< rcsc::Vector2D > self_state;

    const rcsc::WorldModel & wm = agent->world();

    const rcsc::Vector2D my_pos = wm.self().inertiaFinalPoint();
    const rcsc::AngleDeg target_angle = ( target_point - my_pos ).th();

    //
    // check the required turn step
    //
    const double angle_diff = ( target_angle - wm.self().body() ).abs();
    if ( angle_diff > 120.0 )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "__ selfPass doKickTurnDashes target_point=(%.1f %.1f)"
                            " too big angle_diff=%.1f",
                            target_point.x, target_point.y,
                            angle_diff );
        return false;
    }

    {
        double turn_margin = 180.0;
        double target_dist = my_pos.dist( target_point );
        if ( wm.self().playerType().kickableArea() < target_dist )
        {
            turn_margin = rcsc::AngleDeg::asin_deg( wm.self().playerType().kickableArea() / target_dist );
        }
        turn_margin = std::max( turn_margin, 15.0 );

        if ( turn_margin > angle_diff )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                "__ selfPass doKickTurnDashes target_point=(%.1f %.1f)"
                                " too small angle_diff=%.1f < turn_margin=%.1f",
                                target_point.x, target_point.y,
                                angle_diff, turn_margin );
            return false;
        }
    }

    if ( angle_diff
         > wm.self().playerType().effectiveTurn( rcsc::ServerParam::i().maxMoment(),
                                                 wm.self().vel().r() ) )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "__ selfPass doKickTurnDashes target_point=(%.1f %.1f)"
                            " cannot turn by one step."
                            " angle_diff=%.1f",
                            target_point.x, target_point.y,
                            angle_diff );
        return false;
    }

    createSelfCache( agent,
                     1, 12, // turn=1, max_dash=12
                     target_angle,
                     self_state );

    int n_dash = self_state.size() - 1;

    if ( n_dash < 6 )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": too short dash step %d",
                            n_dash );
        return false;
    }

    for ( ; n_dash >= 6; n_dash -= 2 )
    {
        const rcsc::Vector2D receive_pos = self_state[n_dash - 1];

        if ( ! canKick( agent, 1, n_dash, receive_pos ) )
        {
            continue;
        }

        if ( ! checkOpponent( agent, 1, n_dash, receive_pos ) )
        {
            continue;
        }

        if ( doKick( agent, 1, n_dash, receive_pos ) )
        {
            return true;
        }
    }

    return false;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
Bhv_SelfPass::createSelfCache( rcsc::PlayerAgent * agent,
                               const int n_turn,
                               const int n_dash,
                               const rcsc::AngleDeg & accel_angle,
                               std::vector< rcsc::Vector2D > & self_state )
{
    self_state.clear();
    self_state.reserve( n_turn + n_dash );

    const rcsc::WorldModel & wm = agent->world();
    const rcsc::PlayerType & self_type = wm.self().playerType();

    const double dash_power = rcsc::ServerParam::i().maxPower();
    const double stamina_thr = rcsc::ServerParam::i().recoverDecThrValue() + 350.0;

    double my_stamina = ( wm.self().stamina()
                          + ( self_type.staminaIncMax()
                              * wm.self().recovery() ) );
    double my_effort = wm.self().effort();
    double my_recovery = wm.self().recovery();

    rcsc::Vector2D my_pos = wm.self().pos();
    rcsc::Vector2D my_vel = wm.self().vel();

    my_pos += my_vel;
    my_vel *= self_type.playerDecay();

    for ( int i = 0; i < n_turn; ++i )
    {
        my_pos += my_vel;
        my_vel *= self_type.playerDecay();
        my_stamina+=100;
        self_state.push_back( my_pos );
    }

    /*wm.self().playerType().predictStaminaAfterWait( rcsc::ServerParam::i(),
                                                    1 + n_turn,
                                                    &my_stamina,
                                                    &my_effort,
                                                    my_recovery );
*/
    for ( int i = 0; i < n_dash; ++i )
    {
        if ( my_stamina < stamina_thr )
        {
            break;
        }

        double available_stamina =  std::max( 0.0, my_stamina - stamina_thr );
        double consumed_stamina = dash_power;
        consumed_stamina = std::min( available_stamina,
                                     consumed_stamina );
        double used_power = consumed_stamina;
        double max_accel_mag = ( std::fabs( used_power )
                                 * self_type.dashPowerRate()
                                 * my_effort );
        double accel_mag = max_accel_mag;
        if ( self_type.normalizeAccel( my_vel,
                                       accel_angle,
                                       &accel_mag ) )
        {
            used_power *= accel_mag / max_accel_mag;
        }

        rcsc::Vector2D dash_accel
            = rcsc::Vector2D::polar2vector( std::fabs( used_power )
                                            * my_effort
                                            * self_type.dashPowerRate(),
                                            accel_angle );
        my_vel += dash_accel;
        my_pos += my_vel;

        if ( my_pos.x > rcsc::ServerParam::i().pitchHalfLength() - 3.5 )
        {
            break;
        }

        const rcsc::AngleDeg target_angle = ( my_pos - wm.ball().pos() ).th();

        if ( my_pos.absY() > rcsc::ServerParam::i().pitchHalfWidth() - 3.5
             &&  ( ( my_pos.y > 0.0 && target_angle.degree() > 0.0 )
                   || ( my_pos.y < 0.0 && target_angle.degree() < 0.0 )
                   )
             )
        {
            break;
        }

        my_vel *= self_type.playerDecay();

        /*self_type.predictStaminaAfterOneDash( rcsc::ServerParam::i(),
                                              used_power,
                                              &my_stamina,
                                              &my_effort,
                                              &my_recovery );
        */
        my_stamina-=used_power;
        self_state.push_back( my_pos );
    }

}

/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_SelfPass::checkOpponent( rcsc::PlayerAgent * agent,
                             const int n_turn,
                             const int n_dash,
                             const rcsc::Vector2D & receive_pos )
{
    const rcsc::WorldModel & wm = agent->world();

    const double max_moment = rcsc::ServerParam::i().maxMoment();
    const rcsc::PlayerPtrCont::const_iterator o_end = wm.opponentsFromSelf().end();

    for ( rcsc::PlayerPtrCont::const_iterator o = wm.opponentsFromSelf().begin();
          o != o_end;
          ++o )
    {
        const rcsc::PlayerType * player_type = (*o)->playerTypePtr();

        bool goalie = false;
        double control_area = player_type->kickableArea();
        if ( (*o)->goalie()
             && receive_pos.x > rcsc::ServerParam::i().theirPenaltyAreaLineX()
             && receive_pos.absY() < rcsc::ServerParam::i().penaltyAreaHalfWidth() )
        {
            goalie = true;
            control_area = rcsc::ServerParam::i().catchableArea();
        }

        const rcsc::Vector2D & opos = ( (*o)->seenPosCount() <= (*o)->posCount()
                                        ? (*o)->seenPos()
                                        : (*o)->pos() );
        const int vel_count = std::min( (*o)->seenVelCount(), (*o)->velCount() );
        const rcsc::Vector2D & ovel = ( (*o)->seenVelCount() <= (*o)->velCount()
                                        ? (*o)->seenVel()
                                        : (*o)->vel() );

        rcsc::Vector2D opp_pos = ( vel_count <= 1
                                   ? rcsc::inertia_n_step_point( opos, ovel,
                                                                 n_turn + n_dash + 1,
                                                                 player_type->playerDecay() )
                                   : opos + ovel );
        rcsc::Vector2D opp_to_target = receive_pos - opp_pos;

        double opp_to_target_dist = opp_to_target.r();

        int opp_turn_step = 0;
        if ( (*o)->bodyCount() <= 5
             || vel_count <= 5 )
        {
            double angle_diff = ( (*o)->bodyCount() <= 1
                                  ? ( opp_to_target.th() - (*o)->body() ).abs()
                                  : ( opp_to_target.th() - ovel.th() ).abs() );

            double turn_margin = 180.0;
            if ( control_area < opp_to_target_dist )
            {
                turn_margin = rcsc::AngleDeg::asin_deg( control_area / opp_to_target_dist );
            }
            turn_margin = std::max( turn_margin, 15.0 );

            double opp_speed = ovel.r();
            while ( angle_diff > turn_margin )
            {
                angle_diff -= player_type->effectiveTurn( max_moment, opp_speed );
                opp_speed *= (*o)->playerTypePtr()->playerDecay();
                ++opp_turn_step;
            }
        }

        opp_to_target_dist -= control_area;
        opp_to_target_dist -= 0.2;
        opp_to_target_dist -= (*o)->distFromSelf() * 0.01;

        if ( opp_to_target_dist < 0.0 )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                "____ selfPass n_turn=%d n_dash=%d"
                                " opponent %d(%.1f %.1f) is already at receive point"
                                " (%.1f %.1f)",
                                n_turn,
                                n_dash,
                                (*o)->unum(),
                                (*o)->pos().x, (*o)->pos().y,
                                receive_pos.x, receive_pos.y );
            return false;
        }

        int opp_reach_step = player_type->cyclesToReachDistance( opp_to_target_dist );
        opp_reach_step += opp_turn_step;
        opp_reach_step -= rcsc::bound( 0, (*o)->posCount(), 5 );

        if ( opp_reach_step <= n_turn + n_dash )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                "____ selfPass n_turn=%d n_dash=%d"
                                " opponent %d (%.1f %.1f) can reach faster then self."
                                " target=(%.1f %.1f) opp_step=%d opp_turn=%d",
                                n_turn,
                                n_dash,
                                (*o)->unum(),
                                (*o)->pos().x, (*o)->pos().y,
                                receive_pos.x, receive_pos.y,
                                opp_reach_step,
                                opp_turn_step );
            return false;
        }
#ifdef DEBUG
        else
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                "______ selfPass n_turn=%d n_dash=%d"
                                " opponent %d (%.1f %.1f) ok."
                                " target=(%.1f %.1f) opp_step=%d opp_turn=%d",
                                n_turn,
                                n_dash,
                                (*o)->unum(),
                                (*o)->pos().x, (*o)->pos().y,
                                receive_pos.x, receive_pos.y,
                                opp_reach_step,
                                opp_turn_step );
        }
#endif
    }

    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_SelfPass::canKick( rcsc::PlayerAgent *agent,
                       const int n_turn,
                       const int n_dash,
                       const rcsc::Vector2D & receive_pos )
{
    const rcsc::WorldModel & wm = agent->world();
    const rcsc::AngleDeg target_angle = ( receive_pos - wm.ball().pos() ).th();

    //
    // check kick possibility
    //
    double first_speed = rcsc::calc_first_term_geom_series( wm.ball().pos().dist( receive_pos ),
                                                            rcsc::ServerParam::i().ballDecay(),
                                                            1 + 0 + n_dash );
    rcsc::Vector2D max_vel = rcsc::KickTable::calc_max_velocity( target_angle,
                                                                 wm.self().kickRate(),
                                                                 wm.ball().vel() );
    if ( max_vel.r() < first_speed )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "____ selfPass canKick n_turn=%d n_dash=%d cannot kick by one step."
                            " first_speed=%.2f > max_speed=%.2f",
                            n_turn,
                            n_dash,
                            first_speed,
                            max_vel.r() );
        return false;
    }

    //
    // check collision
    //
    const rcsc::Vector2D my_next = wm.self().pos() + wm.self().vel();
    const rcsc::Vector2D ball_next = wm.ball().pos()
        + ( receive_pos - wm.ball().pos() ).setLengthVector( first_speed );

    if ( my_next.dist( ball_next ) < ( wm.self().playerType().playerSize()
                                       + rcsc::ServerParam::i().ballSize() + 0.1 ) )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "____ selfPass n_turn=%d n_dash=%d maybe collision. first_speed=%.2f",
                            n_turn,
                            n_dash,
                            first_speed );
        return false;
    }

    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_SelfPass::doKick( rcsc::PlayerAgent * agent,
                      const int n_turn,
                      const int n_dash,
                      const rcsc::Vector2D & receive_pos )
{
    const rcsc::WorldModel & wm = agent->world();

    double first_speed = rcsc::calc_first_term_geom_series( wm.ball().pos().dist( receive_pos ),
                                                            rcsc::ServerParam::i().ballDecay(),
                                                            1 + n_turn + n_dash );

//     rcsc::AngleDeg target_angle = ( receive_pos - wm.ball().pos() ).th();
//     rcsc::Vector2D max_vel = rcsc::KickTable::calc_max_velocity( target_angle,
//                                                                  wm.self().kickRate(),
//                                                                  wm.ball().vel() );
//     if ( max_vel.r() < first_speed )
//     {
//         rcsc::dlog.addText( rcsc::Logger::TEAM,
//                             "__ selfPass cannot kick by one step. first_speed=%.2f > max_speed=%.2f",
//                             first_speed,
//                             max_vel.r() );
//         return false;
//     }

//     rcsc::Vector2D ball_next = wm.ball().pos()
//         + ( receive_pos - wm.ball().pos() ).setLengthVector( first_speed );
//     rcsc::Vector2D my_next = wm.self().pos() + wm.self().vel();
//     if ( my_next.dist( ball_next ) < ( wm.self().playerType().playerSize()
//                                        + rcsc::ServerParam::i().ballSize() + 0.1 ) )
//     {
//         rcsc::dlog.addText( rcsc::Logger::TEAM,
//                             "__ selfPass maybe collision. first_speed=%.2f",
//                             first_speed );
//         return false;
//     }

    if ( rcsc::Body_SmartKick( receive_pos,
                               first_speed,
                               first_speed * 0.99,
                               1 ).execute( agent ) )
    {
//        agent->setViewAction( new rcsc::View_Wide() );
//        agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan( 2 ) );

        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "__ selfPass n_turn=%d n_dash=%d receive_pos=(%.1f %.1f) first_speed=%.2f",
                            n_turn,
                            n_dash,
                            receive_pos.x, receive_pos.y,
                            first_speed );

        agent->debugClient().addMessage( "SelfPass%d+%d", n_turn, n_dash );
        agent->debugClient().setTarget( receive_pos );

        agent->setIntention( new IntentionSelfPass( receive_pos, n_turn + n_dash ) );

        return true;
    }

    rcsc::dlog.addText( rcsc::Logger::TEAM,
                        "__ selfPass failed smart kick. n_turn=%d n_dash=%d"
                        " receive_pos=(%.1f %.1f) first_speed=%.2f",
                        n_turn,
                        n_dash,
                        receive_pos.x, receive_pos.y,
                        first_speed );

    return false;
}


/////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
//const double Interception::NEVER_CYCLE = 1000.0;
/////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
//namespace {
//inline
//const
//double & logBallDecay()
//{
//    static const double LOG_BALL_DECAY = std::log( ServerParam::i().ballDecay() );
//    return LOG_BALL_DECAY;
//}

//};
/////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
//Interception::Interception( const Vector2D & ball_pos,
//                            const double & ball_first_speed,
//                            const AngleDeg & ball_vel_angle )
//    : M_ball_first_pos( ball_pos )
//    , M_ball_first_speed( ball_first_speed )
//    , M_ball_vel_angle( ball_vel_angle )
//    , M_ball_x_constant( ball_first_speed
//                         / (1.0 - ServerParam::i().ballDecay()) )
//    , M_ball_x_d_constant( (-ball_first_speed * logBallDecay())
//                           / (1.0 - ServerParam::i().ballDecay()) )
//{

//}
/////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
//Interception::Interception( const Vector2D & ball_pos,
//                            const Vector2D & ball_vel )
//    : M_ball_first_pos( ball_pos )
//    , M_ball_first_speed( ball_vel.r() )
//    , M_ball_vel_angle( ball_vel.th() )
//    , M_ball_x_constant( M_ball_first_speed
//                         / (1.0 - ServerParam::i().ballDecay()) )
//    , M_ball_x_d_constant( (-M_ball_first_speed * logBallDecay())
//                           / (1.0 - ServerParam::i().ballDecay()) )
//{

//}
/////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
//double Interception::getReachCycle( const Vector2D & player_pos,
//                             const Vector2D * player_vel,
//                             const AngleDeg * /*player_angle*/,
//                             const int player_count,
//                             const double & control_buf,
//                             const double & player_max_speed ) const
//{
//    static const int MAX_LOOP = 20;
//    static const double MIN_ERROR = 0.05;

//    // player related param
//    /*
//      const double player_max_speed
//      = ( player_type
//      ? player_type->playerSpeedMax()
//      : ServerParam::i().defaultPlayerSpeedMax() );
//      const double control_buf
//      = ( goalie
//      ? ServerParam::i().catchAreaLength()
//      : ( player_type
//      ? ( ServerParam::i().ballSize()
//      + player_type->playerSize()
//      + player_type->kickableMargin() )
//      : ServerParam::i().defaultKickableArea() ) );
//    */

//    // relative to ball pos and ball velocity angle
//    Vector2D start_point = player_pos;
//    start_point -= M_ball_first_pos;

//    ///////////////////////////////////////////
//    // check kickable state at current
//    if ( start_point.r2() < square( control_buf ) )
//    {
//#ifdef TRACE_NEWTON
//        ////cout << "  Newton: ZERO" << endl;
//#endif
//        return 0.0;
//    }


//    // rotation
//    start_point.rotate( - M_ball_vel_angle );


//    // consider player confidence
//    if ( player_count == 0 )
//    {
//        // if player is almost all stopped,
//        // it is necessary for him to accelarate his dash speed at first.
//        if ( player_vel && player_vel->r() < 0.1 )
//        {
//            if ( start_point.absY() > control_buf )
//            {
//                start_point.y += player_max_speed * ( start_point.y > 0.0 ? 1.0 : -1.0 );
//            }
//            else
//            {
//                start_point.x += player_max_speed * ( start_point.x > 0.0 ? 1.0 : -1.0 );
//            }
//        }
//    }
//    else
//    {
//        double virtual_dash = player_max_speed * 0.8;
//        virtual_dash *= std::min(5, player_count);
//        if ( start_point.absY() > virtual_dash )
//        {
//            start_point.y -= virtual_dash * ( start_point.y > 0.0 ? 1.0 : -1.0 );
//        }
//        else
//        {
//            double x_dash = std::sqrt( square( virtual_dash )
//                                       - square( start_point.y ) );
//            start_point.y = 0.0;
//            if ( start_point.absX() > x_dash )
//            {
//                start_point.x -= x_dash * ( start_point.x > 0.0 ? 1.0 : -1.0 );
//            }
//            else
//            {
//                start_point.x = 0.0;
//            }
//        }
//    }



//    /////////////////////////////////////////////////////////////////////////
//    // Newton method

//    const double start_y2 = square( start_point.y );

//    const double bdecay = ServerParam::i().ballDecay();

//    // t: cycles to reach the ball
//    // f: distance(ball_pos, player_first_pos) - (player_total_dash)
//    // f_d: differential of f

//    double t = 0.0;
//    double f, f_d;
//    int counter = 0;
//    do
//    {
//        counter++;
//        //double ball_x
//        //    = util::inertia_n_step_distance( M_ball_first_speed, t, bdecay );
//        double ball_x = M_ball_x_constant * ( 1.0 - std::pow( bdecay, t) );
//        //double ball_x_d
//        //    = (-M_ball_first_speed * std::pow( bdecay, t ) * LOG_BALL_DECAY )
//        //    / (1.0 - ServerParam::i().ballDecay() );
//        double ball_x_d = M_ball_x_d_constant * std::pow( ServerParam::i().ballDecay(), t );
//        // distance to ball at t from player first pos
//        double dist_to_ball = std::sqrt( square( ball_x - start_point.x )
//                                         + start_y2 );
//        f = dist_to_ball - player_max_speed * t - control_buf;
//        f_d = (ball_x - start_point.x) * ball_x_d / dist_to_ball - player_max_speed;
//        if ( ball_x < start_point.x && f_d != 0.0 )
//        {
//            //t = t - f / f_d;
//            t -= f / f_d;
//#ifdef TRACE_NEWTON
//            ////cout << "  Newton:" << counter << ": update_1 cyc=" << t
//                 << "  f=" << f
//                 << "  f_d=" << f_d
//                 << endl;
//#endif
//        }
//        else if ( ball_x > start_point.x && f_d < 0.0 )
//        {
//            t -= f / f_d;
//#ifdef TRACE_NEWTON
//            ////cout << "  Newton:" << counter << ":  update_2 cyc=" << t
//                 << "  f=" << f
//                 << "  f_d=" << f_d
//                 << endl;
//#endif
//        }
//        else
//        {
//            t += f / player_max_speed;
//#ifdef TRACE_NEWTON
//            ////cout << "  Newton:" << counter << ":  update_3 cyc=" << t
//                 << "  f=" << f
//                 << "  f_d=" << f_d;
//#endif
//            // if differential of f is positive value,
//            // ball is moving far away from player faster than player's speed.
//            // And if f_d is small, Newton method required many loop.
//            // so, it is good idea to add the cycles enforcely.
//            if ( f_d > 0.0
//                 // || ( f > 0.01 && fabs(f_d) < 0.01 )
//                 )
//            {
//                t += 10.0; // XXX Magic Number XXX
//#ifdef TRACE_NEWTON
//                ////cout << "  update enforcely. cyc=" << t;
//#endif
//            }
//#ifdef TRACE_NEWTON
//            ////cout << endl;
//#endif
//        }

//        if ( fabs(f) < MIN_ERROR )
//        {
//            break;
//        }
//    }
//    while ( counter < MAX_LOOP );


//    if ( std::fabs(f) > MIN_ERROR )
//    {
//        return NEVER_CYCLE;
//    }

//    return t;
//}
/////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
//double Interception::getReachCycleGreedly( const Vector2D & player_pos,
//                                    const Vector2D * /*player_vel*/,
//                                    const AngleDeg * /*player_angle*/,
//                                    const int /*player_count*/,
//                                    const double & control_buf,
//                                    const double & player_max_speed ,
//                                                                        const int maxCycles) const
//{
//        int maxCyclesToSimulate = maxCycles;
//        if ( maxCyclesToSimulate > 100 )
//                maxCyclesToSimulate = 100;
//    // player related param
//    /*
//      const double player_max_speed
//      = ( player_type
//      ? player_type->playerSpeedMax()
//      : ServerParam::i().defaultPlayerSpeedMax() );
//      const double control_buf
//      = ( goalie
//      ? ServerParam::i().catchAreaLength()
//      : ( player_type
//      ? ( ServerParam::i().ballSize()
//      + player_type->playerSize()
//      + player_type->kickableMargin() )
//      : ServerParam::i().defaultKickableArea() ) ) - 0.05;
//    */

//    const double pitch_x_max = ServerParam::i().pitchHalfLength() + 5.0;
//    const double pitch_y_max = ServerParam::i().pitchHalfWidth() + 5.0;

//    Vector2D ball_pos( M_ball_first_pos );
//    Vector2D ball_vel
//        = Vector2D::polar2vector( M_ball_first_speed, M_ball_vel_angle );

//    Vector2D start_point = player_pos;
//    start_point -= M_ball_first_pos;
//    start_point.rotate( -M_ball_vel_angle );
//    double player_cyc_to_intersect;
//        if ( player_max_speed >= 0.1 )
//                player_cyc_to_intersect = std::max(0.0,
//                   std::ceil( ( start_point.absY() - control_buf * 0.5 )
//                              / player_max_speed )
//                   );
//        else
//                player_cyc_to_intersect = 0;
//    double cycle = std::max( 0.0, player_cyc_to_intersect - 1.0 );
//    double player_dash = player_max_speed * cycle;
//    ball_pos = inertia_n_step_point( ball_pos,
//                                     ball_vel,
//                                     (int)cycle,
//                                     ServerParam::i().ballDecay() );
//    ball_vel *= std::pow( ServerParam::i().ballDecay(), cycle );
//#ifdef TRACE_NEWTON
//    ////cout << "  Greedy: start=" << cycle << endl;
//#endif
//        bool isBallIn = true;
//                if ( ( fabs(ball_pos.x) > pitch_x_max ) or ( fabs(ball_pos.y) > pitch_y_max ) )
//                        isBallIn = false;
//    while ( ( (int)cycle <= maxCyclesToSimulate ) and ( isBallIn ) )
//    {
//        if ( player_pos.dist2( ball_pos ) < square( player_dash + control_buf ) )
//        {
//#ifdef TRACE_NEWTON
//            ////cout << "  Greedy: " << cycle
//                 << "  dist_to_ball: " << player_pos.dist(ball_pos)
//                 << "  player_dash+ctrl: " << player_dash + control_buf
//                 << endl;
//#endif
//            return cycle;
//        }

//        player_dash += player_max_speed;
//        ball_pos += ball_vel;
//        ball_vel *= ServerParam::i().ballDecay();
//        cycle += 1.0;

//                if ( ( fabs(ball_pos.x) > pitch_x_max ) or ( fabs(ball_pos.y) > pitch_y_max ) )
//                        isBallIn = false;
//    }


//    return NEVER_CYCLE;
//}
/////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
//double Interception::getReachCycleBallDelayed( const Vector2D & player_pos,
//                                 const double & control_buf,
//                                 const double & player_max_speed ,
//                                                                 int ballDelay ,
//                                                                 const int maxCycles ) const
//{
//        int maxCyclesToSimulate = maxCycles;
//        if ( maxCyclesToSimulate > 100 )
//                maxCyclesToSimulate = 100;
//    // player related param
//    /*
//      const double player_max_speed
//      = ( player_type
//      ? player_type->playerSpeedMax()
//      : ServerParam::i().defaultPlayerSpeedMax() );
//      const double control_buf
//      = ( goalie
//      ? ServerParam::i().catchAreaLength()
//      : ( player_type
//      ? ( ServerParam::i().ballSize()
//      + player_type->playerSize()
//      + player_type->kickableMargin() )
//      : ServerParam::i().defaultKickableArea() ) ) - 0.05;
//    */

//    const double pitch_x_max = ServerParam::i().pitchHalfLength() + 5.0;
//    const double pitch_y_max = ServerParam::i().pitchHalfWidth() + 5.0;

//    Vector2D ball_pos( M_ball_first_pos );
//    Vector2D ball_vel
//        = Vector2D::polar2vector( M_ball_first_speed, M_ball_vel_angle );

//    Vector2D start_point = player_pos;
//    start_point -= M_ball_first_pos;
//    start_point.rotate( -M_ball_vel_angle );
//    double player_cyc_to_intersect;
//        if ( player_max_speed >= 0.1 )
//                player_cyc_to_intersect = std::max(0.0,
//                   std::ceil( ( start_point.absY() - control_buf * 0.5 )
//                              / player_max_speed )
//                   );
//        else
//                player_cyc_to_intersect = 0;
//    double cycle = std::max( 0.0, player_cyc_to_intersect - 1.0 );
//    double player_dash = player_max_speed * cycle;
//    double ballChangeCycle = 0.f;
//        if ( (int)cycle > ballDelay )
//                ballChangeCycle = cycle - (double)ballDelay;
//        if ( (int)ballChangeCycle )
//        {
//                ball_pos = inertia_n_step_point( ball_pos,
//                                         ball_vel,
//                                             (int)ballChangeCycle,
//                                         ServerParam::i().ballDecay() );
//            ball_vel *= std::pow( ServerParam::i().ballDecay(), ballChangeCycle );
//        }
//#ifdef TRACE_NEWTON
//    ////cout << "  Greedy: start=" << cycle << endl;
//#endif
//        bool isBallIn = true;
//                if ( ( fabs(ball_pos.x) > pitch_x_max ) or ( fabs(ball_pos.y) > pitch_y_max ) )
//                        isBallIn = false;
//    while ( ( (int)cycle <= maxCyclesToSimulate ) and ( isBallIn ) )
//    {
//        if ( player_pos.dist2( ball_pos ) < square( player_dash + control_buf ) )
//        {
//#ifdef TRACE_NEWTON
//            ////cout << "  Greedy: " << cycle
//                 << "  dist_to_ball: " << player_pos.dist(ball_pos)
//                 << "  player_dash+ctrl: " << player_dash + control_buf
//                 << endl;
//#endif
//            return cycle;
//        }

//        player_dash += player_max_speed;
//        if ( (int)cycle > ballDelay )
//                {
//                        ball_pos += ball_vel;
//                        ball_vel *= ServerParam::i().ballDecay();
//                }
//        cycle += 1.0;

//                if ( ( fabs(ball_pos.x) > pitch_x_max ) or ( fabs(ball_pos.y) > pitch_y_max ) )
//                        isBallIn = false;
//    }


//    return NEVER_CYCLE;
//}
/////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
//double Interception::getReachCyclePlayerDelayed( const Vector2D & player_pos,
//                                 const double & control_buf,
//                                 const double & player_max_speed ,
//                                                                 int playerDelay ,
//                                                                 const int maxCycles ) const
//{
//        int maxCyclesToSimulate = maxCycles;
//        if ( maxCyclesToSimulate > 100 )
//                maxCyclesToSimulate = 100;

//    const double pitch_x_max = ServerParam::i().pitchHalfLength() + 5.0;
//    const double pitch_y_max = ServerParam::i().pitchHalfWidth() + 5.0;

//    Vector2D ball_pos( M_ball_first_pos );
//    Vector2D ball_vel
//        = Vector2D::polar2vector( M_ball_first_speed, M_ball_vel_angle );

//        double cycle = 1.f;
//        while ( cycle <= playerDelay )
//        {
//                ball_pos += ball_vel;
//                ball_vel *= ServerParam::i().ballDecay();
//                if ( player_pos.dist2( ball_pos ) < square( control_buf ) ) // Capture Without Move!!
//                        return cycle;
//                cycle += 1.f;
//        }

//        Vector2D start_point = player_pos;
//    start_point -= ball_pos;
//    start_point.rotate( -M_ball_vel_angle ); // No diffrence!
//    double player_cyc_to_intersect;
//        if ( player_max_speed >= 0.1 )
//                player_cyc_to_intersect = std::max(0.0,
//                   std::ceil( ( start_point.absY() - control_buf * 0.5 )
//                              / player_max_speed )
//                   );
//        else
//                player_cyc_to_intersect = 0;
//        player_cyc_to_intersect = std::max( 0.0, player_cyc_to_intersect - 1.0 );
//        cycle = (double)playerDelay + player_cyc_to_intersect;
//    double player_dash = player_max_speed * player_cyc_to_intersect;

//        ball_pos = M_ball_first_pos;
//        ball_vel = Vector2D::polar2vector( M_ball_first_speed, M_ball_vel_angle );
//        ball_pos = inertia_n_step_point( ball_pos,
//                                     ball_vel,
//                                         (int)cycle,
//                                         ServerParam::i().ballDecay() );
//    ball_vel *= std::pow( ServerParam::i().ballDecay(), cycle );
//        bool isBallIn = true;
//                if ( ( fabs(ball_pos.x) > pitch_x_max ) or ( fabs(ball_pos.y) > pitch_y_max ) )
//                        isBallIn = false;
//    while ( ( (int)cycle <= maxCyclesToSimulate ) and ( isBallIn ) )
//    {
//        if ( player_pos.dist2( ball_pos ) < square( player_dash + control_buf ) )
//        {
//#ifdef TRACE_NEWTON
//            ////cout << "  Greedy: " << cycle
//                 << "  dist_to_ball: " << player_pos.dist(ball_pos)
//                 << "  player_dash+ctrl: " << player_dash + control_buf
//                 << endl;
//#endif
//            return cycle;
//        }

//        player_dash += player_max_speed;
//                ball_pos += ball_vel;
//                ball_vel *= ServerParam::i().ballDecay();
//        cycle += 1.0;

//                if ( ( fabs(ball_pos.x) > pitch_x_max ) or ( fabs(ball_pos.y) > pitch_y_max ) )
//                        isBallIn = false;
//    }


//    return NEVER_CYCLE;
//}
/////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
//double Interception::getReachCycleNonDelayed(const Vector2D & player_pos,
//                                 const double & control_buf,
//                                 const double & player_max_speed ,
//                                                                 const int maxCycles ) const
//{
//        int maxCyclesToSimulate = maxCycles;
//        if ( maxCyclesToSimulate > 100 )
//                maxCyclesToSimulate = 100;
//    // player related param
//    /*
//      const double player_max_speed
//      = ( player_type
//      ? player_type->playerSpeedMax()
//      : ServerParam::i().defaultPlayerSpeedMax() );
//      const double control_buf
//      = ( goalie
//      ? ServerParam::i().catchAreaLength()
//      : ( player_type
//      ? ( ServerParam::i().ballSize()
//      + player_type->playerSize()
//      + player_type->kickableMargin() )
//      : ServerParam::i().defaultKickableArea() ) ) - 0.05;
//    */

//    const double pitch_x_max = ServerParam::i().pitchHalfLength() + 5.0;
//    const double pitch_y_max = ServerParam::i().pitchHalfWidth() + 5.0;

//    Vector2D ball_pos( M_ball_first_pos );
//    Vector2D ball_vel
//        = Vector2D::polar2vector( M_ball_first_speed, M_ball_vel_angle );

//    Vector2D start_point = player_pos;
//    start_point -= M_ball_first_pos;
//    start_point.rotate( -M_ball_vel_angle );
//    double player_cyc_to_intersect;
//        if ( player_max_speed >= 0.1 )
//                player_cyc_to_intersect = std::max(0.0,
//                   std::ceil( ( start_point.absY() - control_buf * 0.5 )
//                              / player_max_speed )
//                   );
//        else
//                player_cyc_to_intersect = 0;
//    double cycle = std::max( 0.0, player_cyc_to_intersect - 1.0 );
//    double player_dash = player_max_speed * cycle;
//    ball_pos = inertia_n_step_point( ball_pos,
//                                     ball_vel,
//                                     (int)cycle,
//                                     ServerParam::i().ballDecay() );
//    ball_vel *= std::pow( ServerParam::i().ballDecay(), cycle );
//#ifdef TRACE_NEWTON
//    ////cout << "  Greedy: start=" << cycle << endl;
//#endif
//        bool isBallIn = true;
//                if ( ( fabs(ball_pos.x) > pitch_x_max ) or ( fabs(ball_pos.y) > pitch_y_max ) )
//                        isBallIn = false;
//    while ( ( (int)cycle <= maxCyclesToSimulate ) and ( isBallIn ) )
//    {
//        if ( player_pos.dist2( ball_pos ) < square( player_dash + control_buf ) )
//        {
//#ifdef TRACE_NEWTON
//            ////cout << "  Greedy: " << cycle
//                 << "  dist_to_ball: " << player_pos.dist(ball_pos)
//                 << "  player_dash+ctrl: " << player_dash + control_buf
//                 << endl;
//#endif
//            return cycle;
//        }

//        player_dash += player_max_speed;
//        ball_pos += ball_vel;
//        ball_vel *= ServerParam::i().ballDecay();
//        cycle += 1.0;

//                if ( ( fabs(ball_pos.x) > pitch_x_max ) or ( fabs(ball_pos.y) > pitch_y_max ) )
//                        isBallIn = false;
//    }


//    return NEVER_CYCLE;
//}
/////^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
//double Interception::getReachCycleWithDelay( const Vector2D & player_pos,
//                                 const double & control_buf,
//                                 const double & player_max_speed ,
//                                                                 int playerDelay ,
//                                                                 int ballDelay ,
//                                                                 const int maxCycles ) const
//{
//        if ( playerDelay == ballDelay )
//        {
//                double rawResult = getReachCycleNonDelayed ( player_pos , control_buf , player_max_speed , maxCycles );
//                return rawResult + (double)playerDelay;
//        }
//        else if ( playerDelay > ballDelay )
//        {
//                int resPlayerDelay = playerDelay - ballDelay ;
//                double rawResult = getReachCyclePlayerDelayed ( player_pos , control_buf , player_max_speed , resPlayerDelay , maxCycles );
//                return rawResult + (double)ballDelay;
//        }
//        else
//        {
//                int resBallDelay = ballDelay - playerDelay ;
//                double rawResult = getReachCycleBallDelayed ( player_pos , control_buf , player_max_speed , resBallDelay , maxCycles );
//                return rawResult + (double)playerDelay;
//        }
//}

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <vector>

#include "bhv_basic_offensive_kick.h"
#include "dribble.h"

#include <rcsc/action/body_advance_ball.h>
#include <rcsc/action/body_dribble.h>
#include <rcsc/action/body_hold_ball.h>
#include <rcsc/action/body_pass.h>
#include <rcsc/action/neck_scan_field.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>


#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>

#include <rcsc/player/self_intercept_v13.h>
#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>
#include <rcsc/geom/sector_2d.h>
#include <rcsc/action/body_clear_ball.h>
#include <rcsc/action/body_kick_one_step.h>
#include <iostream>

using namespace std ;

/*-------------------------------------------------------------------*/
using rcsc::Vector2D ;

rcsc::Vector2D Smart_Dribble::getDribblePos(rcsc::PlayerAgent * agent,double & ang)
{
    const rcsc::WorldModel & wm = agent->world();
    const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();

    double asi;

    Cone2DDRIB cone ( agent->world().self().pos() , m_ang , m_dist ) ;



    if(fabs(wm.self().pos().y)>19 && wm.self().pos().x < 30 )
        asi = 0;

    else if ( agent->world().self().pos().x > 45 )
//        asi = (Vector2D (52.0,0.0) - agent->world().self().pos()).th().degree() ;
        return  Vector2D (52.0,0.0);
    else
        asi = (Vector2D (52.5,0.0) - agent->world().self().pos()).th().degree() ;

    std::vector <Vector2D> v1 ;

    bool opp_in = false ;
    const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();
    for ( rcsc::PlayerPtrCont::const_iterator it = opps.begin();
           it != opps_end;
           ++it )
    {
        Vector2D pos_opponent = (*it)->pos() ;
        if ( cone.isInCone ( pos_opponent , asi ) && !(*it)->isTackling()  )
        {
                opp_in = true ;
                v1.push_back (pos_opponent) ;
        }
    }
   // std::cout << " v1 size: " << v1.size() << std::endl;
    if ( !opp_in )
    {
                //std::cout << " return asi " << asi << std::endl;
                ang  = asi ;
        return wm.self().pos() + Vector2D :: polar2vector ( DIST_FRONT_DEFUALT , asi ) ;
    }

    std::vector <double> v2 ;

    double temp ;
    for ( int i = 0 ; i < v1.size() ; ++i )
    {
                //std::cout << " opponent " << i+1 << std::endl;
                temp = (v1.at(i) - agent->world().self().pos()).th().degree() ;
                v2.push_back ( Cone2DDRIB::normalizeAngle(temp + (m_ang+1)) ) ;
                v2.push_back ( Cone2DDRIB::normalizeAngle(temp - (m_ang+1)) ) ;
                //std::cout << " angleToMe : " << temp << std::endl;
                //std::cout << " right: " << Cone2DDRIB::normalizeAngle(temp + (m_ang+1)) << std::endl;
                //std::cout << " left: " << Cone2DDRIB::normalizeAngle(temp - (m_ang+1)) << std::endl;
        }

    std::vector <double> v3 ;

    for ( int i = 0 ; i < v2.size() ; ++i )
    {
                //std::cout << " angle " << i << std::endl;
                //std::cout << " ang: " << v2.at(i) << std::endl;
                opp_in = false ;
                const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();
                for ( rcsc::PlayerPtrCont::const_iterator it = opps.begin();
                           it != opps_end;
                           ++it )
                {
                        Vector2D pos_opponent = (*it)->pos() ;
                        if ( cone.isInCone ( pos_opponent , v2.at(i) ) )
                        {
                                        opp_in = true ;
                                        break ;
                        }
                }
                if ( opp_in )
                {
                //	std::cout << " opponent in " << std::endl;
                        continue ;
                }
                else
                {
                //	std::cout << " no opponent in angle " << std::endl;
                        v3.push_back ( v2.at(i) ) ;
                }
        }

        //std::cout << " size of v3 " << v3.size() << std::endl;

        if ( v3.size() > 1 )
        {
        //	std::cout << " get min " << std::endl;
                double d_min = 10000 ;
                int i_min ;
                for ( int i = 0 ; i < v3.size() ; ++i )
                {
        //		std::cout << " angle " << i << std::endl;
        //		std::cout << " ang " << v3.at(i) << std::endl;
        //		std::cout << " asi " << asi << std::endl;
        //		std::cout << " dif " << fabs(Cone2DDRIB::normalizeAngle(v3.at(i)-asi)) << std::endl;
                        if ( fabs(Cone2DDRIB::normalizeAngle(v3.at(i)-asi)) < d_min )
                        {
                                d_min = fabs(Cone2DDRIB::normalizeAngle(v3.at(i)-asi)) ;
                                i_min = i ;
                        }
                }
        //	std::cout << " return min " << v3.at(i_min) << std::endl;
                ang = v3.at(i_min);
                return wm.self().pos() +
                                Vector2D::polar2vector( DIST_FRONT_DEFUALT , v3.at(i_min) ) ;
        }
        else if ( v3.size() == 1 )
        {
        //	std::cout << " return 0om " << v3.at(0) << std::endl;
                ang = v3.at(0);
                return wm.self().pos() + Vector2D :: polar2vector ( DIST_FRONT_DEFUALT , v3.at(0) ) ;
        }
        else
        {
        //	std::cout << " again do it " << std::endl;
                m_ang *= 0.75 ;
                m_dist *= 0.95 ;
                double temp ;
                return getDribblePos(agent,temp) ;
        }
}

bool
Smart_Dribble::execute( rcsc::PlayerAgent * agent )
{



    const rcsc::WorldModel & wm = agent -> world();


    if(wm.self().pos().dist(Vector2D(52.5,0.0))>20 && wm.self().pos().dist(Vector2D(-52.5,0.0))>30 && Bhv_SelfPass().execute(agent)){
        cout<<"selfpass first"<<endl;
           return true ;
    }
   if(wm.self().pos().dist(Vector2D(52.5,0.0))>20 && wm.self().pos().dist(Vector2D(-52.5,0.0))>30 && Bhv_SelfPass().execute2(agent)  ){
        cout<<"selfpass second"<<endl;
           return true ;
   }


    const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();
double temp ;
    rcsc::Vector2D pos=getDribblePos(agent,temp);
    if(pos.y>33)
        pos.y=33;

    if(pos.y<-33)
        pos.y=-33;

    if(pos.x>52.0)
        pos.x=52.0;

    if(pos.x<-52.0)
        pos.x=-52.0;


   Cone2DDRIB cone ( agent->world().self().pos() , 70 , 15 ) ;

    double asi = (Vector2D (52.5,0.0) - agent->world().self().pos()).th().degree() ;

    bool opp_in = false ;
    const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();
    for ( rcsc::PlayerPtrCont::const_iterator it = opps.begin();
           it != opps_end;
           ++it )
    {
        Vector2D pos_opponent = (*it)->pos() ;
        if ( cone.isInCone ( pos_opponent , asi ) && !(*it)->isTackling() )
        {
                opp_in = true ;
                break ;
        }
    }


        if ( !opp_in )
        {
            if(rcsc::Body_Dribble2008( rcsc::Vector2D ( 52.5 , 0.0 ) ,
                           0.05 ,
                           100  ,
                           100.0 ,
                           bWithDodge(agent)
                           ).execute( agent ) )
                cout<<"smart first"<<endl;
        return true ;
        }

    Vector2D pos2 = Vector2D( 52 , 0 ) ;

    if( wm.self().pos().y > 17 )
        pos2.y =  7 ;
    if( wm.self().pos().y < -17 )
        pos2.y = -7 ;


    if( wm.getOpponentGoalie() )
        if( wm.self().pos().dist( wm.getOpponentGoalie()->pos() ) < 3 )
            return rcsc::Body_HoldBall2008().execute( agent ) ;

    if( wm.self().pos().x > 30 &&
     rcsc::Body_Dribble2008( pos2,
                            0.05 ,
                            100  ,
                            100.0 ,
                            bWithDodge( agent )
                            ).execute( agent ) )
    {
        cout<<"smart sec"<<endl;
        return true ;
    }


    if( bWithDodge( agent ) &&
        rcsc::Body_Dribble2008( pos  ,
                                0.05 ,
                                100.0,
                                100.0 ,
                                bWithDodge(agent)
                                ).execute( agent ) )
    {
        cout<<"smart thre"<<endl;
        return true;
    }
    else if( rcsc::Body_Dribble2008( pos  ,
                                2.5  ,
                                100.0,
                                100.0 ,
                                bWithDodge(agent)
                                ).execute( agent ) )
    {
        cout<<"smart four"<<endl;
        return true;
    }

    else if(rcsc::Body_HoldBall2008().execute(agent)){
        cout<<"hold final"<<endl;
        return true;
    }
    return false;
}

bool
Smart_Dribble::bWithDodge( rcsc::PlayerAgent * agent )
{
//    return true;
    const rcsc::WorldModel & wm = agent->world();
    rcsc::Vector2D self_pos = wm.self().pos() ;
    rcsc::Vector2D target ;

if(self_pos.x>0 && wm.self().unum()>8/* && self_pos.absY() <20 */)
    return false;
else
    return true;

        if(noDodge(agent))
        return false;

    if( self_pos.x > 30 && self_pos.absY() > 19 )
        target = Vector2D( 52.5 , 0 ) ;
    else
        target = Vector2D( 10 , 0 ) ;

    double ang = ( target - self_pos ).th().degree() ;


    self_pos = Vector2D( wm.self().pos().x - 1 , wm.self().pos().y ) ;

    Vector2D pos_r = self_pos + Vector2D::polar2vector( 3.5 , ang + 90 ) ;
    Vector2D pos_l = self_pos + Vector2D::polar2vector( 3.5 , ang - 90 ) + Vector2D::polar2vector( 6.0 , ang ) ;

    rcsc::Rect2D rect( pos_l , pos_r ) ;


    if( wm.getOpponentGoalie() && wm.self().pos().dist( wm.getOpponentGoalie()->pos() ) < 6 )
        return true ;


    Vector2D pos_close( 1000 , 1000 )  ;
    if( wm.getOpponentNearestToSelf( 3 , true ) )
        pos_close = wm.getOpponentNearestToSelf( 3 , true )->pos() ;

    if( rect.contains( pos_close ) && self_pos.x < 45 )
        return true ;

    if( pos_close.dist( self_pos ) < 4.5 && pos_close.x >= self_pos.x && pos_close.y < self_pos.y+3.5 && pos_close.y > self_pos.y-3.5 )
        return true ;

    if( self_pos.x > 30 && wm.ourPlayer( 11 ) && ( wm.ourPlayer(11)->posCount() > 3 || wm.ourPlayer(11)->pos().x < 35 )  )
        return true ;

    return false ;

}


double Smart_Dribble::getDribbleLen(rcsc::PlayerAgent * agent ,int num,double ang, rcsc::Vector2D myPos , rcsc::Vector2D oppPos )
{

    const rcsc::WorldModel & wm = agent->world();
    rcsc::Vector2D endPos;

    for(double i=0.1 ; i<=15 ; i+=0.1)
    {

        endPos=wm.ball().pos()+Vector2D::polar2vector(i,ang);
        for(int j=1 ; j<=11 ; j++)
            if(wm.theirPlayer(j) && !wm.theirPlayer(j)->isTackling() )
            if(endPos.dist(wm.theirPlayer(j)->pos()) < wm.self().pos().dist(endPos) || endPos.x > 50 || fabs(endPos.y)>30  )
            return i;
    }

    return 15;

}


bool Smart_Dribble::noDodge(rcsc::PlayerAgent * agent)
{
const rcsc::WorldModel & wm = agent->world();
rcsc::Vector2D self_pos=wm.self().pos();
double temp ;
double ang=(getDribblePos(agent,temp)-self_pos).th().degree();

if(self_pos.dist(rcsc::Vector2D(57,0)) < 20)
return true;
return false;


}


