/*!
      Coded by Shayan Salehian
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//#include "bhv_pass2010.h"
#include "bhv_dribble2010.h"
//#include "bhv_oci.h"
//#include "bhv_dribble2010.h"
#include "bhv_shoot2010.h"
#include "bhv_clear_ball2010.h"

using namespace std;

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool Clear_Ball :: execute ( rcsc::PlayerAgent * agent )
{		
    const rcsc::WorldModel & wm = agent->world();
    const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();

    Smart_DribbleIO dr ( 30.0 , 20.0 ,  3.0 ) ;
	double ang ;
	dr.getDribblePos ( agent , ang ) ;

    ShootIO *sh ;

    const rcsc::PlayerObject * nearest_opp
        = ( opps.empty()
            ? static_cast< rcsc::PlayerObject * >( 0 )
            : opps.front() );
    const double nearest_opp_dist = ( nearest_opp
                                      ? nearest_opp->distFromSelf()
                                      : 1000.0 );
	
	rcsc::Vector2D point = wm.self().pos() + rcsc::Vector2D::polar2vector ( 30.0 , ang ) ;

	rcsc::Vector2D one_step_vel ;

	double max_one_step_speed ;

	one_step_vel = rcsc::KickTable::calc_max_velocity( ( point - wm.self().pos() ).th() ,
								                           wm.self().kickRate(),
								                           wm.ball().vel() );
	max_one_step_speed = one_step_vel.r();
	
	if ( ang >= 80.0 ) 
		ang = 80.0 ;
	if ( ang <= -80.0 ) 
		ang = -80.0 ;		
	
	if ( agent->world().dirCount( ang ) >= 3 )
	{
		agent->setNeckAction( new rcsc::Neck_TurnToPoint( point ) );
	}
		
	if ( nearest_opp_dist < 5 || wm.ball().pos().x < -47.0 )
	{
		if ( max_one_step_speed > 1.6 )
		{
			rcsc::Body_SmartKick( point , max_one_step_speed , max_one_step_speed * 0.96 , 1 ).execute( agent ) ;
		}
		else
		{
			double speedTwo = sh->get_max_speed_of_step ( agent , 2 , point ) ;
			rcsc::Body_SmartKick( point , speedTwo  , speedTwo * 0.96 , 2 ).execute( agent ) ;
		}
	}
	else
	{
		rcsc::Body_SmartKick( point , 3.0 , 3.0 * 0.96 , 3 ).execute( agent ) ;
	}

    return true ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
