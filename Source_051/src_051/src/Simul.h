#ifndef _SIMUL_H
#define _SIMUL_H

#include <rcsc/player/soccer_action.h>
#include <rcsc/common/player_type.h>
#include <rcsc/player/player_agent.h>
#include <rcsc/player/abstract_player_object.h>
#include <rcsc/player/self_object.h>


#include "strategy.h"
#include "iostream"

using namespace rcsc;
using namespace std;

//! ball's Parameter:
#define BALL_DECAY 0.94
#define MINES_ONE_BALL_DECAY 0.06
#define BALL_SIZE 0.085

//! player's Parameter:
#define PLAYER_SIZE 0.3
#define MIN_MOMENT -180.0
#define MAX_MOMENT 180.0
#define MIN_POWER -100.0
#define MAX_POWER 100.0
#define KICK_POWER_RATE 0.027

//! tackle's Parameter:
#define TACKLE_DIST 2.0
#define TACKLE_BACK_DIST 0.0
#define TACKLE_EXPONENT 6.0
#define FOUL_EXPONENT 10.0
#define TACKLE_WIDTH 1.25
#define MIN_TACKLE_MOMENT -90.0
#define MAX_TACKLE__MOMENT 90.0
#define MAX_BACK_TACKLE_POWER 0.0
#define MAX_TACKLE_POWER 100.0
#define TACKLE_POWER_RATE 0.027

//! Parameters:
//#define M_PI
#define NOISE 0.1
#define UNKNOWN_VALUE 22771
#define BALL_PLAYER_SIZE 0.385
#define DASH_ANGLE_STEP 45.0
#define BACK_DASH_RATE 0.6
#define SIDE_DASH_RATE 0.4

enum SimulT	{
    SIM_BALL ,
    SIM_PLAYER ,
    SIM_OPPONENT ,
    SIM_TEAMMATE ,
    SIM_AGENT ,
    SIM_ILLEGAL
};

enum ActionT {
  ILLEGAL ,
  DASH ,
  TURN ,
  CATCH ,
  KICK ,
  MOVE ,
  TACKLE ,
  MAX_COMMANDS
} ;



class SimulateAction
{



public:

  ActionT   actionType;
  double    dAngle;
  double    dPower;
  double    dX;
  double    dY;
  bool      bFoul;

  SimulateAction( ActionT act = ILLEGAL ,  double d1=UNKNOWN_VALUE ,
                                        double d2=UNKNOWN_VALUE ) ;

  void doAction( PlayerAgent *agent   ) ;


} ;

//struct  playerVariableValues{
//    Vector2D                globalPosition;
//    Vector2D                velocity;
//    AngleDeg                bodyAngle;
//    const PlayerType *      PT;
//    double                  effort;
//    //int                     confidence;
//    int                     playerNumber;


//};

class Simul
{

protected:

        SimulT                  simulType;
        Vector2D                globalPosition;
        Vector2D                velocity;
        AngleDeg                bodyAngle;
        const PlayerType *      PT;
        double                  effort;
        int                     playerNumber;
        double                  decay;
        double                  size;
        PlayerAgent *           PA;


public:

        void        operator = ( const Simul & s  ) ;

        Simul ( PlayerAgent * pa  ) ;
        Simul (  ) ;
        Simul ( PlayerAgent * pa , SimulT simT , int nr = 0  ) ;
        Simul ( PlayerAgent * pa , const AbstractPlayerObject * player ) ;

        void    setSelfSim ( const SelfObject * selfPlayer ) ;
        void    setBallSim ( const BallObject * ball ) ;
        void    setTeammateSim ( const AbstractPlayerObject * player ) ;
        void    setOpponentSim ( const AbstractPlayerObject * player ) ;


        /**! medriki bool*/void setSimulType            ( SimulT s ) ;
        SimulT                  getSimulType            ()  const ;
        /**! medriki bool*/void setPosition             ( Vector2D p ) ;
        Vector2D                getPosition             () const ;
        /**! medriki bool*/void setVelocity             ( Vector2D v ) ;
        Vector2D                getVelocity         	() const ;
        /**! medriki bool*/void setBodyAngle            ( AngleDeg a ) ;
        AngleDeg                getBodyAngle            () const ;
        /**! medriki bool*/void setPT                   ( const PlayerType * p ) ;
        const PlayerType *      getPT                   () const ;
        /**! medriki bool*/void setEffort               ( double e ) ;
        double                  getEffort               () const ;
        /**! medriki bool*/void setPlayerNumber         ( int n ) ;
        int                     getPlayerNumber     	() const ;
        /**! medriki bool*/void setDecay                ( double d ) ;
        double                  getDecay                () const ;
        /**! medriki bool*/void setSize                 ( double s ) ;
        double                  getSize                 () const ;
        /**! medriki bool*/void setPA                 ( PlayerAgent * pa ) ;
        PlayerAgent *           getPA                 () const ;


        double	An  ( double A1 , double dORq , int n , bool q = 0 ) ;
        double	An  ( double A1 , double q , double d , int n ) ;
        double	Sn  ( double A1 , double dORq , int n , bool q = 0 ) ;
        double	Sn  ( double A1 , double q , double d , int n ) ;
        double	Sn2  ( double A1 , double q , double d , double n ) ;
        double	N   ( double Sn , double A1 , double dORq , bool q = 0 ) ;

        void dynamicsBallSimulate           ( int cycle ) ;
        void dynamicsBallSimulate_1_cycle   () ;
        void actionBallSimulate             ( Simul & body , SimulateAction & bodyCommand /* , bool ballCatched = false*/ ) ;
        void dynamicsPlayerSimulate         ( int cycle ) ;
        void dynamicsPlayerSimulate_1_cycle () ;
        void actionPlayerSimulate           ( SimulateAction & bodyCommand /* , int cycle=1*/ ) ;


//        double	getDistanceTo(Simul & sim ) ;

//        bool isGoalieSim( ) ;
//        bool isTeammateSim( ) ;
//        bool isOpponentSim( ) ;

        bool    isKickable  ( Simul & ball ) const ;
        bool    isKickable  ( Vector2D & pos ) const ;
        bool    isCollision ( Simul & ball ) const ;

        double  getActualSpeedProduct   ( Simul & ball , double & power ) ;
        double  getActualPower          ( Simul & ball , double & speed ) const ;

        double      getDashRate             ( double dir ) ;
        Vector2D    getActualVelocityAfterDash   ( double & power , double & dir ) ;

        double  maxTurnAng  ( double iAng = 180.0 ) ;
        double  turnAng     ( double & iAng ) ;

        Simul	finalSim            ( double minSpeed ) ;
//        int     turnTimeTo          ( double turnDir , double angTurn ) ;
        int     turnTimeToPlusSim   ( Vector2D pos , double angTurn ) ;
        int     dashTimeTo          ( Vector2D posEnd ) ;
        int     dashTime            ( double dis /*, double dashAng*/ ) ;
        int     calculateBallTimeTo ( double dis , double minSpeed ) ;
        int     timeTo              ( Vector2D pos ) ;
        int     reachTime           ( Vector2D pos ) ;


        double      maxRand         ( Simul   ball , SimulateAction soc ) ;
        Vector2D    maxKickNoise    ( Simul   ball ,  SimulateAction soc , double ang ) ;
        double      maxRand         ( Vector2D posBall , double speed ,  SimulateAction soc ) ;
        Vector2D    maxKickNoise    ( Vector2D posBall , double speed ,  SimulateAction soc , double ang ) ;




         SimulateAction   moveToPosSim               ( Vector2D   posTo ,
                                                  double        angWhenToTurn ,
                                                  double        dDistDashBack = 0.0 ,
                                                  bool          bMoveBack = false ,
                                                  int           iCycles = 1         ) ;
         SimulateAction   dashToPointSim             ( Vector2D   pos ,
                                                  int           iCycles = 1         ) ;

         SimulateAction   turnBodyToPointSim         ( Vector2D   pos ,
                                                  int           iPos = 1            ) ;
         SimulateAction   turnBackToPointSim         ( Vector2D   pos ,
                                                  int           iPos = 1            ) ;
         SimulateAction   directTowardsSim           ( Vector2D   posTo ,
                                                  double        angWhenToTurn ,
                                                  Vector2D   *pos = NULL ,
                                                  Vector2D   *vel = NULL ,
                                                  double        *angBody  = NULL    ) ;
        double        getPowerForDash            ( Vector2D    posRelTo ,
                                                   double         angBody ,
                                                   Vector2D    vel ,
                                                   double         dEffort ,
                                                   int            iCycles = 1       ) ;
        double        getAngleForTurn            ( double         angDesiredAngle ,
                                                   double         dSpeed ) ;



};

/**! change it : " write it by macro " */
template <class type>
void typeNormalize(type &a , type min , type max){

    if(a<min)
        a=min;
    else if(a>max)
        a=max;

}
template <class type>
void typeSwap(type &a , type &b){

    type c = b ;
    b = a ;
    a = c ;
}


//struct NewKick{

//    int     cycle;
//    int step;
//    rcsc::Vector2D    end;
//    double speed;
//    double M_speed;
//    NewKick(){

//        step=-1;
//        cycle=-1;
//        end=rcsc::Vector2D(0,0);
//        speed=0;
//        M_speed=0;
//    }


//};

//struct  spd{
//    double min;
//    double max;
//};



#endif
