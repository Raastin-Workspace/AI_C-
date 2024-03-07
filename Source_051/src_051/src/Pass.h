#ifndef _PASS
#define _PASS
#include <rcsc/player/view_mode.h>
#include <rcsc/player/say_message_builder.h>
#include <rcsc/common/server_param.h>
#include <rcsc/common/logger.h>
//#include <rcsc/player/soccer_action.h>
//#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/geom/vector_2d.h>
#include <rcsc/action/kick_table.h>
#include <rcsc/action/body_smart_kick.h>
#include <rcsc/action/body_kick_one_step.h>
#include <rcsc/action/body_kick_to_relative.h>
#include <rcsc/action/neck_turn_to_point.h>
#include <rcsc/action/body_dribble.h>
#include <rcsc/action/body_hold_ball.h>
#include <rcsc/action/neck_scan_field.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>
#include <rcsc/action/body_clear_ball.h>
#include <rcsc/common/server_param.h>
#include <rcsc/common/logger.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/geom/vector_2d.h>
#include <rcsc/player/intercept_table.h>
#include <rcsc/action/kick_table.h>
#include <rcsc/action/body_advance_ball2009.h>
#include <rcsc/action/body_smart_kick.h>
#include <rcsc/action/body_kick_one_step.h>
#include <rcsc/action/body_kick_to_relative.h>
#include <rcsc/common/server_param.h>
//#include <rcsc/common/player_type.h>
#include <rcsc/common/logger.h>
#include <rcsc/soccer_math.h>
#include <rcsc/math_util.h>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>
#include "strategy.h"
#include <iostream>
#include <vector>
///#include "bhv_pass20102.h"
#include "bhv_clear_ball2010.h"

#include "Mark.h"
#include "Oci.h"
using namespace std;
using namespace rcsc;





class MostafPass
    : public SoccerBehavior {
private:

public:
    MostafPass ( )
    { }



    int             getCount(const WorldModel & wm,double dir,double PM);
    int             getCount(const WorldModel & wm,Vector2D pos,double PM);
    double          calculate_PM(const WorldModel & wm,Vector2D start,Vector2D end,double speed,double minSpeed=0.4);

    bool            OCI(PlayerAgent * agent,VecPosition start,VecPosition end,double speed,int minConf,double minSpeed,int step,double rand=0,bool withRand=false,int Ext=0);
    bool            IOOCI(PlayerAgent * agent,Vector2D start,Vector2D end,double speed,NewKick & kick,int minConf,double minSpeed,int step,double rand=0,bool withRand=false,int Ext=0);
    bool            OCC(PlayerAgent * agent,VecPosition start,VecPosition end,double speed,int minConf,double minSpeed,int step,double rand=0,bool withRand=false);

    double      oneStepRand(PlayerAgent * agent,double dPower=100);
    double      twoStepRand(PlayerAgent * agent,Body_SmartKick kick,double dPower=100);

    double      kickRand(PlayerAgent * agent, Body_SmartKick &kick, double dPower=100);

    bool      isInGoal(VecPosition start,VecPosition end,double speed,double rand,double Y=6.8);

    bool    setKick(const WorldModel & wm,NewKick &kick,Body_SmartKick a);
    Vector2D    startKick(const WorldModel &wm, Body_SmartKick &a);
    int    stepKick(Body_SmartKick a);
    bool        SHOOOOOOT(PlayerAgent * agent,NewKick &kick);



    double  dakheli(VecPosition a,VecPosition b){

        return a.getMagnitude()*b.getMagnitude()*cosDeg(a.getDirection()-b.getDirection());

    }
    double  dakheli(Vector2D a,Vector2D b){

        return a.length()*b.length()*cosDeg(a.dir().degree()-b.dir().degree());

    }

    double      flagScoring(PlayerAgent * agent,VecPosition    pos,VecPosition posFrom,int minConf=10);
    double      flagScoring(PlayerAgent * agent,Vector2D   pos,Vector2D posFrom,int minConf=10);
    spd       getDirectSpeed(Vector2D &start, Vector2D &end);
    spd       getLeadSpeed(PlayerAgent * agent, int &nr, Vector2D &start, Vector2D &end, int &step, int avans=1);
    spd       getThroughSpeed(PlayerAgent * agent, int &nr, Vector2D &start, Vector2D &end, int &step, int &time, int avans=1);

    bool	directPass(PlayerAgent * agent,NewKick &kicking,int &nr,bool better=1);
    bool	goalieDirectPass(PlayerAgent * agent,NewKick &kicking,int &nr,bool better=1);
    bool        isInField(Vector2D pos, double xend=51.5, double yend=33);
    void        setInField(Vector2D &pos, double maxX=48);
    void        setInField(VecPosition &pos, double maxX=48);
    int         timeToReach(double speed,double dis,int  & time);
    bool        throughPass(PlayerAgent * agent,NewKick &kicking,int &nr,bool better=1);

    bool        leadingPass(PlayerAgent * agent,NewKick &kicking,int &nr,bool better=1);

    double	posScoreInFieldForUs(VecPosition pos);
    double	getCofficient(double Ang);
    double      FinalMostafScoring(VecPosition pos,VecPosition inform,int minimalConf);

    VecPosition     turnPoint               ( PlayerAgent * agent,int = 2                       );
    double        getAngBetween       ( double, double, double = 1.0 , double = 1.0   );


    Vector2D        seen(PlayerAgent *agent);


    Vector2D        where(PlayerAgent *agent,int ballMinConf = 5);
    bool getClearDir(PlayerAgent *agent,int minConf);
    bool clear(PlayerAgent *agent, int minConf, double minDist, double minScore, double minAngle, double maxAngle, double minSpeed,NewKick &kicking);
    bool    getClear(PlayerAgent * agent , double & minDis , double & minAng ,double & maxAng,double & minSpeed , int & minConf , bool & forceKick);

    bool doForceKick(PlayerAgent *agent,Vector2D pos);


    bool           execute                  ( rcsc::PlayerAgent * agent,NewKick &kicking ) ;
    double         get_first_speed_of_end   ( rcsc::PlayerAgent * agent ,
                                              rcsc::Vector2D , double ) ;
    bool           is_goal_chance_situation ( rcsc::PlayerAgent * agent , rcsc::PlayerPtrCont::const_iterator ) ;
    bool           is_better_pos_situation  ( rcsc::PlayerAgent * agent , rcsc::PlayerPtrCont::const_iterator , double& , int& ) ;
    bool           is_out_situation         ( rcsc::PlayerAgent * agent ) ;

    double         goal_chance_score        ( rcsc::PlayerAgent * agent , rcsc::PlayerPtrCont::const_iterator ) ;

    int            get_min_or_max           ( vector <double> , bool is_max ) ;

    double         best_first_speed         ( rcsc::PlayerAgent * agent , rcsc::Vector2D , int ) ;

    double         get_max_speed_of_step  ( rcsc::PlayerAgent * agent , int , rcsc::Vector2D ) ;

    double			ranking(rcsc::PlayerAgent * agent,rcsc::Vector2D pos);

//      double			getCofficient(double Ang);
    double			congestionScore(rcsc::PlayerAgent * agent,rcsc::Vector2D pos);

    double			posScoreInFieldForUs(rcsc::Vector2D pos				);
    double			posScoreInFieldForClear(rcsc::Vector2D pos				);
    double          calcScore(rcsc::PlayerAgent * agent,rcsc::Vector2D pos);

    bool    ClearTheFuckingBall(rcsc::PlayerAgent * agent,NewKick &kicking);
    bool    ClearTheFuckingBallFuckingGoalie(rcsc::PlayerAgent * agent);


    bool	directPassSabok(PlayerAgent * agent,NewKick &kicking,int &nr);
    bool    leadingPassSabok(PlayerAgent * agent,NewKick &kicking,int &nr);
    bool    throughPassSabok(PlayerAgent * agent, NewKick &kicking, int &nr);

private:

};

#endif

