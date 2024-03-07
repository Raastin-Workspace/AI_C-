#ifndef SAEED_BHV_SMART_DRIBBLE_H
#define SAEED_BHV_SMART_DRIBBLE_H

#include <rcsc/geom/vector_2d.h>
#include <rcsc/player/soccer_action.h>

#define DIST_DEFAULT 10
#define DIST_DEFAULT_PENALTY 5
#define ANG_DEFAULT 55
#define STEP_DEFAULT 3
#define DIST_FRONT_DEFUALT 20


class Smart_DribbleIO
    : public rcsc::SoccerBehavior {
private:

    double m_dist ;
    double m_ang ;
    double m_step ;

public:

    

    Smart_DribbleIO ( double dist , double ang , double step ) :
    m_dist ( dist ) , m_ang ( ang ) , m_step ( step ) {}
	double getDribbleLen(rcsc::PlayerAgent * agent, int num , bool isTeammate);
    Smart_DribbleIO ( ) :
    m_dist ( DIST_DEFAULT ) , m_ang ( ANG_DEFAULT ) , m_step ( STEP_DEFAULT ) {}
    
    bool execute( rcsc::PlayerAgent * agent );
    rcsc::Vector2D getDribblePos (rcsc::PlayerAgent * agent,double & ang) ;
    rcsc::Vector2D getDribblePos1 (rcsc::PlayerAgent * agent) ;
    rcsc::Vector2D getClosetsetTeam(rcsc::PlayerAgent *agent);

double getLenght( rcsc::PlayerAgent * agent , double ang , int cycle ,double *dist) ;

bool bWithDodge( rcsc::PlayerAgent * agent ) ;
bool noDodge(rcsc::PlayerAgent * agent);
double getDribbleLen(rcsc::PlayerAgent * agent ,int num,double ang, rcsc::Vector2D myPos , rcsc::Vector2D oppPos );
double getDirectionOfWidestAngle(rcsc::PlayerAgent * agent , double a, double b,int cycle);


};

class Cone2D
{
private:
    rcsc::Vector2D m_start;
    double m_ang ;
    double m_dist ;
    
public:




    Cone2D ( const rcsc::Vector2D & start ,
            const double & ang ,
            const double & dist )
    : m_start(start) , m_ang (ang) , m_dist (dist)
    {}

    bool isInCone ( rcsc::Vector2D rePos , const double & body_ang )
    {

            rcsc::Vector2D dif = rePos - m_start ;
            if ( dif.r() < m_dist )
            if ( dif.th().degree() > body_ang - m_ang &&
            dif.th().degree() < body_ang + m_ang )
            return true ;
            return false ;
    }
    
    static double normalizeAngle ( double angle ) 
    {
		while( angle > 180.0  ) angle -= 360.0;
		while( angle < -180.0 ) angle += 360.0;

		return ( angle );
	}


    

};

#endif
