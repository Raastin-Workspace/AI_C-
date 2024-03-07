/*!
      Coded by Shayan Salehian
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_oci.h"
#include "bhv_dribble2010.h"
#include <iostream>
#include <vector>

using namespace std;

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool oci :: ball_reach_safe ( rcsc::PlayerAgent * agent , 
                              rcsc::Vector2D target_point ,
							  int step ,
							  double first_speed ,
							  bool is_pass )
{
	const rcsc::WorldModel & wm = agent->world() ;
	const rcsc::ServerParam & param = rcsc::ServerParam::i();
	
	int opponent_cycles , time , i ;
	rcsc::Vector2D pos_ball = wm.ball().pos();
	
	int ball_cycles      = ball_reach_cycle ( agent , target_point , step , first_speed ) ;
    double distance      = pos_ball.dist ( target_point ) ;
	rcsc::AngleDeg angle = ( target_point - pos_ball ).th();

	for ( i = 1 ; i <= ball_cycles ; i++ )
	{
		pos_ball = pos_ball + rcsc::Vector2D::polar2vector ( first_speed * pow ( 0.94 , i - 1 ) , angle ) ;

		time = opp_reach_cycle ( agent , pos_ball , is_pass ) ;

		if ( time <= i )
		{
			return false ;
		}
	}

	/// rcsc::dlog.addText( rcsc::Logger::SHOOT,
                         /// ": opponent Reach : %d ", time );

	/// rcsc::dlog.addText( rcsc::Logger::SHOOT,
                         /// ": ball_cycles : %d ", ball_cycles );

	return true ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int oci :: opp_reach_cycle ( rcsc::PlayerAgent * agent , rcsc::Vector2D point , bool is_pass )
{
	const rcsc::WorldModel & wm = agent->world();
	const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();
	const rcsc::ServerParam & param = rcsc::ServerParam::i();
	
	bool count_goalie = false ;
	
	vector <int> cycle ;
	cycle.push_back ( 1000 ) ; // if cycle stay empty defaul is 1000
	
	const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();
	for ( rcsc::PlayerPtrCont::const_iterator it = opps.begin();
                  it != opps_end;
                  ++it )
	{
		int confidence = (*it)->posCount() ;

		if ( confidence > 8 && !(*it)->goalie() )  // I Can't See him
		{
			continue ;
		}
		if ( confidence > 15 && (*it)->goalie() )  // I Can't See him
		{
			continue ;
		}
        
        rcsc::Vector2D new_point = point + rcsc::Vector2D::polar2vector ( 3.0 , ( point - wm.self().pos() ).th().degree() ) ;
        double ang1 = ( new_point - wm.self().pos() ).th().degree() ;
        double ang2 = ( (*it)->pos() - new_point ).th().degree() ;
        
        if ( is_pass && fabs ( ang1 - ang2 ) < 45.0 )
        {
            continue ;
        }

		const rcsc::PlayerType * player_type = (*it)->playerTypePtr();
		const double kick_dist = player_type->kickableArea();
		const double tackle_dist = param.tackleDist() ;
		const double catch_dist = param.catchableArea() ;

		int i ;
		const double player_decay      = player_type->playerDecay()  ;
		const double player_max_speed  = player_type->realSpeedMax() ;
		const double player_effort     = player_type->effortMax() ;
		const double player_dash_power = ( player_effort *
		                                   param.defaultDashPowerRate() *
										   param.maxDashPower() ) ;

		rcsc::AngleDeg opp_body_angle , opp_velocity_dir ;
		
		opp_body_angle = opp_velocity_dir = 0.0 ;

		rcsc::Vector2D opp_pos        = (*it)->pos() ;
		rcsc::AngleDeg main_ang_2_pos = ( point - opp_pos ).th() ;
		rcsc::AngleDeg ang_2_pos      = main_ang_2_pos ;
		
		double distance = opp_pos . dist ( point ) ;

		double opp_speed , opp_velocity_r ;
	
		bool should_turn = false ;

		// see opponent body direction in 2 cycles
		opp_body_angle = (*it)->bodyCount() <= 2 ? (*it)->body() : ang_2_pos ;

		// see opponent Velocity direction in 1 cycles
		opp_velocity_dir = (*it)->velCount() <= 1 ? (*it)->vel().th() : opp_body_angle ;
		opp_velocity_r   = (*it)->velCount() <= 1 ? (*it)->vel().r() : 0.2 ;
			
		opp_speed  =  opp_velocity_r * 2.5 ;

		rcsc::Line2D opp_line ( opp_pos , opp_pos +
		                            rcsc::Vector2D::polar2vector ( 5.0 , opp_body_angle ) ) ;
		
		double line_dist_pos = opp_line . dist ( point ) ;

		double player_dist , goalie_dist , catch_or_kick_dist , dist ;
 		
		if ( (*it)->goalie() )
		{
			count_goalie = true ;
			
			catch_or_kick_dist = ( (*it)->pos().x > param.theirPenaltyAreaLineX() ) ? catch_dist 
			                                                                             : kick_dist ;
			goalie_dist = ( ang_2_pos - opp_body_angle ).abs() < 90.0 ? tackle_dist
				                                                     : catch_or_kick_dist ;
			if ( line_dist_pos > goalie_dist )
			{
				should_turn = true ;
				goalie_dist = tackle_dist ;
			}
		}
		else
		{
			player_dist = ( ang_2_pos - opp_body_angle ).abs() < 90.0 ? tackle_dist
				                                                     : kick_dist ;
			if ( line_dist_pos > player_dist )
			{
				should_turn = true ;
				player_dist = tackle_dist ;
			}
		}

		dist = (*it)->goalie() ? goalie_dist : player_dist ;

		if ( opp_pos . dist ( point ) < dist )
		{
			cycle.push_back ( 0 ) ;
			continue ;
		}

		opp_speed      = ( opp_speed * player_decay ) ;
		opp_speed      = min ( player_max_speed , opp_speed ) ;
		opp_pos        = opp_pos + rcsc::Vector2D::polar2vector ( opp_speed , opp_velocity_dir ) ;
		ang_2_pos      = ( point - opp_pos ) . th() ;
		main_ang_2_pos = ( point - opp_pos ) . th() ;

		for ( i = 1 ; i < distance * 2 ; ++i )
		{
			if ( should_turn )
			{
				should_turn    = false ;
				opp_speed      = ( opp_speed * player_decay ) ;
				opp_speed      = min ( player_max_speed , opp_speed ) ;
				opp_pos        = opp_pos + rcsc::Vector2D::polar2vector ( opp_speed , opp_velocity_dir ) ;
				ang_2_pos      = ( point - opp_pos ) . th() ;
				main_ang_2_pos = ( point - opp_pos ) . th() ;
		
				continue ;
			}
			else
			{
				opp_speed    = ( opp_speed * player_decay ) + player_dash_power ;
				opp_speed    = min ( player_max_speed , opp_speed ) ;
				opp_pos   = opp_pos + rcsc::Vector2D::polar2vector ( opp_speed , ang_2_pos ) ;
				ang_2_pos = ( point - opp_pos ) . th() ;
			}
			
			if ( opp_pos . dist ( point ) < dist )
			{
				cycle.push_back ( i ) ;
				break ;
			}
		}
	}
	
	int min = cycle [0] , i ;
	
	for ( i = 1 ; i <= cycle.size() ; i++ )
	{
		if ( cycle[i] < min )
		{
			min = cycle[i] ;
		}
	}
	
	return min ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int oci :: ball_reach_cycle ( rcsc::PlayerAgent * agent , 
                              rcsc::Vector2D point , int step , double first_speed )
{
	const rcsc::WorldModel & wm = agent->world() ;
	const rcsc::ServerParam & param = rcsc::ServerParam::i();
	rcsc::Vector2D pos_ball = wm.ball().pos() ;
	double distance = pos_ball.dist ( point ) ;
	double speed = 0 ;
	int i ;

    rcsc::AngleDeg angle = ( point - pos_ball ) . th();
	
	if ( distance < 1.0 )
	{
		return 0 ;
	}

	for ( i = step - 1 ; i < 40 ; i++ )
	{
		speed += first_speed ;
		first_speed *= param.ballDecay() ;

		if ( ( speed + 1.0 ) >= distance )
		{
			return i ;
		}
	}
	
	return 1000 ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int oci :: player_reach_cycle ( rcsc::PlayerAgent * agent , rcsc::PlayerPtrCont::const_iterator it , rcsc::Vector2D point )
{
	const rcsc::WorldModel & wm = agent->world();
	const rcsc::ServerParam & param = rcsc::ServerParam::i();
	
	int confidence = (*it)->posCount() ;

	if ( confidence > 8 && !(*it)->goalie() )  // I Can't See him
	{
		return 1000 ;
	}
	if ( confidence > 15 && (*it)->goalie() )  // I Can't See him
	{
		return 1000 ;
	}

	const rcsc::PlayerType * player_type = (*it)->playerTypePtr();
	const double kick_dist = player_type->kickableArea();
	const double tackle_dist = param.tackleDist() ;
	const double catch_dist = param.catchableArea() ;

	int i ;
	const double player_decay      = player_type->playerDecay()  ;
	const double player_max_speed  = player_type->realSpeedMax() ;
	const double player_effort     = player_type->effortMax() ;
	const double player_dash_power = ( player_effort *
	                                   param.defaultDashPowerRate() *
									   param.maxDashPower() ) ;

	rcsc::AngleDeg player_body_angle , player_velocity_dir ;
	
	player_body_angle = player_velocity_dir = 0.0 ;

	rcsc::Vector2D player_pos     = (*it)->pos() ;
	rcsc::AngleDeg main_ang_2_pos = ( point - player_pos ).th() ;
	rcsc::AngleDeg ang_2_pos      = main_ang_2_pos ;
	
	double distance = player_pos.dist ( point ) ;

	double player_speed , player_velocity_r ;
	
	bool should_turn = false ;

	// see player body direction in 2 cycles
	player_body_angle = (*it)->bodyCount() < 2 ? (*it)->body() : ang_2_pos ;

	// see player Velocity direction in 1 cycles
	player_velocity_dir = (*it)->velCount() <= 1 ? (*it)->vel().th() : player_body_angle ;
	player_velocity_r   = (*it)->velCount() <= 1 ? (*it)->vel().r() : 0.2 ;
			
	player_speed  =  player_velocity_r * 2.5 ;

	rcsc::Line2D player_line ( player_pos , player_pos +
                               rcsc::Vector2D::polar2vector ( 5.0 , player_body_angle ) ) ;
		
	double line_dist_pos = player_line.dist ( point ) ;

	double player_dist , goalie_dist , catch_or_kick_dist , dist ;
 		
	if ( (*it)->goalie() )
	{
		catch_or_kick_dist = ( (*it)->pos().x > param.theirPenaltyAreaLineX() ) ? catch_dist : kick_dist ;
		goalie_dist = ( ang_2_pos - player_body_angle ).abs() < 90.0 ? tackle_dist
				                                                     : catch_or_kick_dist ;
		if ( line_dist_pos > goalie_dist )
		{
			should_turn = true ;
			goalie_dist = tackle_dist ;
		}
	}
	else
	{
		player_dist = ( ang_2_pos - player_body_angle ).abs() < 90.0 ? tackle_dist : kick_dist ;
		
		if ( line_dist_pos > player_dist )
		{
			should_turn = true ;
			player_dist = tackle_dist ;
		}
	}

	dist = (*it)->goalie() ? goalie_dist : player_dist ;

	if ( player_pos.dist ( point ) < dist )
	{
		return 0 ;
	}

	player_speed      = ( player_speed * player_decay ) ;
	player_speed      = min ( player_max_speed , player_speed ) ;
	player_pos        = player_pos + rcsc::Vector2D::polar2vector ( player_speed , player_velocity_dir ) ;
	ang_2_pos         = ( point - player_pos ) . th() ;
	main_ang_2_pos    = ( point - player_pos ) . th() ;

	for ( i = 1 ; i < distance * 2 ; ++i )
	{
		if ( should_turn )
		{
			should_turn       = false ;
			player_speed      = ( player_speed * player_decay ) ;
			player_speed      = min ( player_max_speed , player_speed ) ;
			player_pos        = player_pos + rcsc::Vector2D::polar2vector ( player_speed , player_velocity_dir ) ;
			ang_2_pos         = ( point - player_pos ) . th() ;
			main_ang_2_pos    = ( point - player_pos ) . th() ;
	
			continue ;
		}
		else
		{
			player_speed    = ( player_speed * player_decay ) + player_dash_power ;
			player_speed    = min ( player_max_speed , player_speed ) ;
			player_pos   = player_pos + rcsc::Vector2D::polar2vector ( player_speed , ang_2_pos ) ;
			ang_2_pos = ( point - player_pos ) . th() ;
		}
		
		if ( player_pos.dist ( point ) < dist )
		{
			return i ;
		}
	}
	return 1000 ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
