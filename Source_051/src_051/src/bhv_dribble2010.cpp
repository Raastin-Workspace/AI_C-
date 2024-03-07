
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <vector>

#include "bhv_basic_offensive_kick.h"
#include "bhv_dribble2010.h"

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

rcsc::Vector2D Smart_DribbleIO::getDribblePos(rcsc::PlayerAgent * agent,double & ang)
{
    const rcsc::WorldModel & wm = agent->world();
    const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();

    double asi;
    
    Cone2D cone ( agent->world().self().pos() , m_ang , m_dist ) ;


    if(fabs(wm.self().pos().y)>19 && wm.self().pos().x < 30 )
        asi = 0;

    else if ( agent->world().self().pos().x > 45 )
        asi = (Vector2D (48.5,0.0) - agent->world().self().pos()).th().degree() ;
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
        if ( cone.isInCone ( pos_opponent , asi ) )
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
		v2.push_back ( Cone2D::normalizeAngle(temp + (m_ang+1)) ) ;
		v2.push_back ( Cone2D::normalizeAngle(temp - (m_ang+1)) ) ;
		//std::cout << " angleToMe : " << temp << std::endl;
		//std::cout << " right: " << Cone2D::normalizeAngle(temp + (m_ang+1)) << std::endl;
		//std::cout << " left: " << Cone2D::normalizeAngle(temp - (m_ang+1)) << std::endl;
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
	//		std::cout << " dif " << fabs(Cone2D::normalizeAngle(v3.at(i)-asi)) << std::endl;
			if ( fabs(Cone2D::normalizeAngle(v3.at(i)-asi)) < d_min )
			{
				d_min = fabs(Cone2D::normalizeAngle(v3.at(i)-asi)) ;
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
Smart_DribbleIO::execute( rcsc::PlayerAgent * agent )
{

    const rcsc::WorldModel & wm = agent -> world();
	const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();
double temp ;
    rcsc::Vector2D pos=getDribblePos(agent,temp);
    if(pos.y>30)
        pos.y=30;

    if(pos.y<-30)
        pos.y=-30;

    if(pos.x>50)
        pos.x=50;

    if(pos.x<-50)
        pos.x=-50;


   Cone2D cone ( agent->world().self().pos() , 70 , 15 ) ;

    double asi = (Vector2D (52.5,0.0) - agent->world().self().pos()).th().degree() ;
                
    bool opp_in = false ;
    const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();
    for ( rcsc::PlayerPtrCont::const_iterator it = opps.begin();
           it != opps_end;
           ++it )
    {
        Vector2D pos_opponent = (*it)->pos() ;
        if ( cone.isInCone ( pos_opponent , asi ) )
        {
                opp_in = true ;
                break ;
        }
    }


	if ( !opp_in )
	{
	    rcsc::Body_Dribble2008( rcsc::Vector2D ( 52.5 , 0.0 ) ,
                           0.05 ,
                           100  ,
                           100.0 ,
                           true
                           ).execute( agent ) ;
        return true ;
	}
		
    Vector2D pos2 = Vector2D( 50 , 0 ) ;

    if( wm.self().pos().y > 17 )
        pos2.y =  7 ;
    if( wm.self().pos().y < -17 )
        pos2.y = -7 ;


    if( wm.getOpponentGoalie() )
        if( wm.self().pos().dist( wm.getOpponentGoalie()->pos() ) < 4 )
            return rcsc::Body_HoldBall2008().execute( agent ) ;

    if( wm.self().pos().x > 30 &&
     rcsc::Body_Dribble2008( pos2,
                            0.05 ,
                            100  ,
                            100.0 ,
                            bWithDodge( agent )
                            ).execute( agent ) )
        return true ;


    if( bWithDodge( agent ) )
        return
        rcsc::Body_Dribble2008( pos  ,
                                0.05 ,
                                100.0,
                                100.0 ,
                                bWithDodge(agent)
                                ).execute( agent ) ;
    else
        return
        rcsc::Body_Dribble2008( pos  ,
                                2.5  ,
                                100.0,
                                100.0 ,
                                bWithDodge(agent)
                                ).execute( agent ) ;

}

bool
Smart_DribbleIO::bWithDodge( rcsc::PlayerAgent * agent )
{
    const rcsc::WorldModel & wm = agent->world();
    rcsc::Vector2D self_pos = wm.self().pos() ;
    rcsc::Vector2D target ;


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


double Smart_DribbleIO::getDribbleLen(rcsc::PlayerAgent * agent ,int num,double ang, rcsc::Vector2D myPos , rcsc::Vector2D oppPos )
{

    const rcsc::WorldModel & wm = agent->world();
    rcsc::Vector2D endPos;

    for(double i=0.1 ; i<=15 ; i+=0.1)
    {

        endPos=wm.ball().pos()+Vector2D::polar2vector(i,ang);
        for(int j=1 ; j<=11 ; j++)
            if(wm.theirPlayer(j))
            if(endPos.dist(wm.theirPlayer(j)->pos()) < wm.self().pos().dist(endPos) || endPos.x > 50 || fabs(endPos.y)>30  )
            return i;
    }

    return 15;

}


bool Smart_DribbleIO::noDodge(rcsc::PlayerAgent * agent)
{
const rcsc::WorldModel & wm = agent->world();
rcsc::Vector2D self_pos=wm.self().pos();
double temp ;
double ang=(getDribblePos(agent,temp)-self_pos).th().degree();

if(self_pos.dist(rcsc::Vector2D(57,0)) < 20)
return true;
return false;	
	
	
}	
	
