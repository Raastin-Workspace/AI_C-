/*!
      Coded by Shayan Salehian
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_shoot2010.h"
#include "bhv_oci.h"

#include <iostream>
#include <vector>

using namespace std;

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool ShootIO::execute ( rcsc::PlayerAgent * agent )
{
	// use tackle in it
	
	// logger
	
	/// char log_str [ 20 ] = "" ;
	/// sprintf ( log_str , "./logs/%d" , agent->world().self().unum() ) ;
	/// rcsc::GameTime t = agent->world().time();
	/// rcsc::dlog.setLogFlag ( &t , rcsc::Logger::SHOOT , true ) ;
	/// rcsc::dlog.open ( log_str ) ;

	const rcsc::WorldModel & wm = agent->world();
	
	double dist_to_goal = agent->world().self().pos().dist ( rcsc::Vector2D ( 52.5 , 0.0 ) ) ;
	
	if ( !agent->world().self().isKickable() || ( dist_to_goal >= 13.0 && agent->world().self().pos().x > 48.0 ) )
	{
		return false ;
	}

	const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();
	const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();
	
	rcsc::Vector2D shot_point ;
    double speed ;
    
    if ( dist_to_goal < 20.0 )
    {
        if ( shoot_point ( agent , 1 , shot_point , speed ) )
        {
            cout<<"one Step";
            rcsc::Body_SmartKick( shot_point ,
                                    speed ,
                                    speed * 0.96 ,
                                    1 ).execute( agent );
            return true ;
        }

        if ( shoot_point ( agent , 2 , shot_point , speed ) )
        {
            cout<<"two Step";
            rcsc::Body_SmartKick( shot_point ,
                                    speed ,
                                    speed * 0.96 ,
                                    2 ).execute( agent );

            return true ;
        }
    }

	if ( shoot_point ( agent , 3 , shot_point , speed ) )
    {
        cout<<"3 Step";
		rcsc::Body_SmartKick( shot_point ,
                              speed ,
                              speed * 0.96 ,
                              3 ).execute( agent );
							  
		return true ;
	}

	double closest_to_me = 30.0 ;
	
	for ( rcsc::PlayerPtrCont::const_iterator itt = opps.begin();
	      itt != opps_end;
	      ++itt )
	{
		rcsc::Vector2D pos_opponent = (*itt)->pos() ;
		
		if ( wm.ball().pos().dist ( pos_opponent ) < closest_to_me )
		{
			closest_to_me = wm.ball().pos().dist ( pos_opponent ) ;
		}
	}

	if ( closest_to_me < 5.0 && wm.ball().pos().x > 42.0 && wm.ball().pos().absY() < 10.0 )
	{
		int sign = ( wm.ball().pos().y > 0.0 ) ? 1 : -1 ;
		rcsc::Vector2D pos_side ( 52.5 , 5.8 * sign ) ;
		
		if ( agent->world().getOpponentGoalie() )
		{
			pos_side.y = ( agent->world().getOpponentGoalie()->pos().y ) > 0.0 ? -5.8 : 5.8 ;
		}
		
		if ( closest_to_me > 3.0 )
		{
            cout<<"fucking type";
			rcsc::Body_SmartKick( pos_side , get_max_speed_of_step ( agent , 2 , pos_side )  , get_max_speed_of_step ( agent , 2 , pos_side ) * 0.96 ,2 ).execute( agent ) ;
			return true ;
		}
		else
		{
			if ( get_max_speed_of_step ( agent , 1 , pos_side ) > 2.0 )
			{
                cout<<"another fucking type";
				rcsc::Body_SmartKick( pos_side , get_max_speed_of_step ( agent , 1 , pos_side )  , get_max_speed_of_step ( agent , 1 , pos_side ) * 0.96 ,1 ).execute( agent ) ;
				return true ;
			}
		}
	} 

	if ( agent->world().getOpponentGoalie() &&
	     agent->world().getOpponentGoalie()->pos().dist ( agent->world().self().pos() ) < 3.0 )   // so close to goalie , maybe ball jump from him
	{
        cout<<"and such a fucking type";
		rcsc::Body_SmartKick( rcsc::Vector2D ( 52.5 , 0.0 ) ,
                              3.0 ,
                              3.0 * 0.96 ,
                              2 ).execute( agent );
		return true ;
	}

	return false ;
	
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool ShootIO::shoot_point ( rcsc::PlayerAgent * agent , int step ,
                          rcsc::Vector2D &point , double &speed )
{
	const rcsc::WorldModel & wm = agent->world() ;
	rcsc::Vector2D pos_agent = wm.self().pos() ; 

	oci *oc ;
	rcsc::Vector2D opp_goal ( 52.5 , 0.0 ) ;
	double distance = pos_agent.dist ( opp_goal ) ;

	if ( distance > 35.0 )
	{
		if ( ( agent->world().getOpponentGoalie() && agent->world().getOpponentGoalie()->pos().dist ( pos_agent ) > 15.0 )
		     || !agent->world().getOpponentGoalie() )
		{
			return false ;
		}
	}


	double Y_pos ;
	distance = distance < 10.0 ? 10.0 : distance ;
	double to_goal_abs = ( opp_goal - pos_agent ).th().abs() ;
	rcsc::AngleDeg to_goal = ( opp_goal - pos_agent ).th() ;
	
	double noise = ( distance / 15.0 ) / 4.0 ;
	noise += ( to_goal_abs / 25.0 ) ;
		
	rcsc::AngleDeg view1 = agent->world().self().face() + wm.self().viewWidth().width() / 2.0 ;
	rcsc::AngleDeg view2 = agent->world().self().face() - wm.self().viewWidth().width() / 2.0 ;
	
	if ( agent->world().dirCount( to_goal ) >= 3 )
	{
		agent->setNeckAction( new rcsc::Neck_TurnToPoint( opp_goal ) );
	}
	
	if ( agent->world().dirCount( to_goal ) <= 3 &&
	     !agent->world().getOpponentGoalie() )
	{
		speed = get_max_speed_of_step ( agent , step , opp_goal ) ;
		
		if ( oc->ball_reach_safe ( agent ,
		                           opp_goal ,
							       step , speed ) )
		{
			point = opp_goal ;
			return true ;
		}
	}
	
	for ( Y_pos = 6.5 - noise ; Y_pos >= 0.0 ; Y_pos -= 0.3 )
	{
		rcsc::Vector2D point1 = rcsc::Vector2D ( 52.5 , Y_pos ) ;
		rcsc::Vector2D point2 = rcsc::Vector2D ( 52.5 , -Y_pos ) ;
		
		double speed1 = get_max_speed_of_step ( agent , step , point1 ) ;
		double speed2 = get_max_speed_of_step ( agent , step , point2 ) ;
		
		if ( oc->ball_reach_safe ( agent ,
		                           point1 ,
							       step , speed1 ) )
		{
			point = point1 ;
			return true ;
		}
		if ( oc->ball_reach_safe ( agent ,
		                           point2 ,
							       step , speed2 ) )
		{
			point = point2 ;
			return true ;
		}
	}
	
	return false ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

double ShootIO:: getPowerForKick ( rcsc::PlayerAgent * agent , double angle , double dFirstSpeed )
{
	const rcsc::WorldModel & wm = agent->world() ;
	
	double        angBody       = wm.self().body().degree() ;
	double        ballSpeed     = wm.ball().vel().r() ;
	double        ballDir       = wm.ball().vel().th().degree() ;
	double        dPower ;
	double        kickDirection = angle + angBody ; // Global Angle
	double        angDiff       = fabs ( kickDirection - ballDir ) ;
	double        cosAngDiff    = rcsc::AngleDeg::cos_deg ( angDiff ) ;
	double        ballSpeedCos  = ballSpeed * cosAngDiff ;

	dPower = ( dFirstSpeed - ballSpeedCos ) / getActualKickPowerRate( agent );

	return  dPower ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

double ShootIO::getActualKickPowerRate ( rcsc::PlayerAgent * agent )
{
	const rcsc::WorldModel & wm = agent->world() ;
	double ang_to_ball = ( wm.ball().pos() - wm.self().pos() ).th().degree() ;
	double dir_diff      = fabs ( wm.self().body().degree() - ang_to_ball );
	double dist          = wm.self().pos().dist( wm.ball().pos() ) ;
	
    return 0.027 * ( 1 - 0.25 * dir_diff/180.0 - 0.25 * dist / wm.self().playerType().kickableArea() ) ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

double ShootIO::get_max_speed_of_step ( rcsc::PlayerAgent * agent , int step , rcsc::Vector2D point )
{
    double speed ;

    rcsc::Vector2D one_step_vel = rcsc::KickTable::calc_max_velocity( ( point - agent->world().ball().pos() ).th(),
                                                                        agent->world().self().kickRate(),
                                                                        agent->world().ball().vel() );
    double one_step_speed = one_step_vel.r();

    if ( step == 1 )
    {
        speed = one_step_speed ;
    }
    else if ( step == 2 )
    {
        speed = ( 3.0 + one_step_speed ) / 2.0 ;
    }
    else
    {
        speed = 3.0 ;
    }

    return speed ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

