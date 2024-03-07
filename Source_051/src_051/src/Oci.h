//#ifndef _OCI
//#define _OCI
//#include <rcsc/common/server_param.h>
//#include <rcsc/common/logger.h>
//#include <rcsc/player/debug_client.h>
//#include <rcsc/geom/vector_2d.h>
//#include <rcsc/soccer_math.h>
//#include <rcsc/math_util.h>
//#include <algorithm>
//#include <iostream>
//#include <cmath>
//#include <vector>
//#include "strategy.h"
//#include <iostream>
//#include <vector>
//#include "Simul.h"

//using namespace std;
//using namespace rcsc;

//class OCI
//    : public SoccerBehavior {
//private:

//public:
//    OCI ( )
//    { }



//};

//#endif

#ifndef _OCI_H
#define _OCI_H

//#include "WorldModel.h"
#include <rcsc/player/soccer_action.h>
#include <rcsc/common/player_type.h>
#include <rcsc/player/player_agent.h>
#include <rcsc/player/abstract_player_object.h>
#include <rcsc/player/self_object.h>
#include <Mark.h>
#include <Simul.h>


#include "strategy.h"
#include "iostream"

using namespace rcsc;
using namespace std;



//enum SimulT	{
//        SIM_BALL,
//        SIM_PLAYER,
//        SIM_AGENT,
//        SIM_ILLEGAL
//};

enum CommandT {
  CMD_ILLEGAL,      /*!< illegal command */
  CMD_DASH,         /*!< dash command (player only)         */
  CMD_TURN,         /*!< turn command (player only)         */
  CMD_TURNNECK,     /*!< turn_neck command (player only)    */
  CMD_CHANGEVIEW,   /*!< change view command (player only)  */
  CMD_CATCH,        /*!< catch command (goalie only)        */
  CMD_KICK,         /*!< kick command (player only)         */
  CMD_MOVE,         /*!< move command                       */
  CMD_SENSEBODY,    /*!< sense_body command (player only)   */
  CMD_SAY,          /*!< say command                        */
  CMD_CHANGEPLAYER, /*!< change_player command (coach only) */
  CMD_ATTENTIONTO,  /*!< pay attention to specific player   */
  CMD_TACKLE,       /*!< tackle in current body direction   */
  CMD_POINTTO,      /*!< point arm towards a point on field */
  CMD_MAX_COMMANDS  /*!< maximum number of commands         */
} ;



class SoccerCommand
{



public:

  // different variables that are used by the different possible commands
  // only the variables that are related to the current commandType have
  // legal values
  CommandT     commandType; /*!< type of this command                        */
  double       dAngle;      /*!< angle of this command (for turn,turn_neck)  */
  double       dPower;      /*!< power of this command (for kick,dash)       */

  double       dX;          /*!< x coordinate (for move)                     */
  double       dY;          /*!< y coordinate (for move)                     */
  int          iTimes;      /*!< how many cycles will a command  be sent     */
  bool         bFoul;

  SoccerCommand( CommandT com = CMD_ILLEGAL, double d1=22771,
                                             double d2=22771,
                                             double d3=22771 );


  // command to set the different values of the SoccerCommand
  void makeCommand( CommandT com, double     d1 = 22771,
                                  double     d2 = 22771,
                                  double     d3 = 22771      );


  bool isIllegal  (                                                       );
  void doCommand( PlayerAgent *agent  );


  // used to return the string representation of this SoccerCommand
} ;


class Simul2
{

protected:

        SimulT		simulType;
        ObjectT		objType;
        VecPosition	posGlob;
        VecPosition	posVelocity;
        AngleDeg        bodyAng;
        int          conf;
        double		decay;
        double		dashPowerRate;
        double		inertiaMoment;
        double		size;
        double		speedMax;
        double		staminaIncMax;
        double		effortMax;
        const PlayerType *	HPS;



        PlayerAgent *PA;

public:







        Simul2(PlayerAgent *pa );

        Simul2();


        void        operator = ( const Simul2 &d );

        bool		setTypeSimul        	(SimulT sim);
        SimulT		getTypeSimul        	() const;
        bool		setObjTypeSimul     	(ObjectT o);
        ObjectT		getObjTypeSimul     	() const;
        bool		setPosSimul 		(VecPosition p);
        VecPosition	getPosSimul 		() const;
        bool		setVelSimul 		(VecPosition v);
        VecPosition	getVelSimul         	() const;
        bool		setBodyAngSimul     	(AngleDeg a);
        AngleDeg	getBodyAngSimul     	() const;
        bool		setConfidenceSimul	(int a);
        int		getConfidenceSimul	() const;
        bool		setDecaySimul           (double a);
        double		getDecaySimul           () const;
        bool		setDashPowerRateSimul   (double a);
        double		getDashPowerRateSimul   () const;
        bool		setInertiaMomentSimul   (double a);
        double		getInertiaMomentSimul   () const;
        bool		setSizeSimul        	(double a);
        double		getSizeSimul        	() const;
        bool		setSpeedMaxSimul	(double a);
        double		getSpeedMaxSimul        () const;
        bool		setStaminaIncMaxSimul   (double a);
        double		getStaminaIncMaxSimul   () const;
        bool		setEffortMaxSimul       (double a);
        double		getEffortMaxSimul   	() const;
        bool		setHPSSimul 		(const PlayerType * a);
        const PlayerType *	getHPSSimul		() const;

        ////////////////////////////////////////////////////////////////////////
        bool isTeammate( ObjectT o )
        {
          return o >= OBJECT_TEAMMATE_1 && o <= OBJECT_TEAMMATE_UNKNOWN;
        }

        bool isOpponent( ObjectT o )
        {
          return o >= OBJECT_OPPONENT_1 && o <= OBJECT_OPPONENT_UNKNOWN;
        }

        bool isGoalie( ObjectT o )
        {
          return o == OBJECT_TEAMMATE_1 || o == OBJECT_OPPONENT_1;
        }

        bool isBall( ObjectT o )
        {
          return o == OBJECT_BALL;
        }

        int getNr( ObjectT o )
        {
          if( o >= OBJECT_OPPONENT_1 && o <= OBJECT_OPPONENT_11 )
            return o - OBJECT_OPPONENT_1 +1 ;
          else if( o >= OBJECT_TEAMMATE_1 && o <= OBJECT_TEAMMATE_11 )
            return o - OBJECT_TEAMMATE_1 +1 ;
          else
            return -1;
        }
        ObjectT getAgentObjectType(const WorldModel &wm){

           return (ObjectT)(OBJECT_TEAMMATE_1+wm.self().unum()-1);



        }

        const AbstractPlayerObject * getPlayer(const WorldModel &wm,ObjectT obj){

            const AbstractPlayerObject * player=0;

            if(isOpponent(obj) /*&& wm.theirPlayer(getNr(obj))*//**kill*/)
                player = wm.theirPlayer(getNr(obj));
            else if(isTeammate(obj) /*&& wm.ourPlayer(getNr(obj))*/)
                player = wm.ourPlayer(getNr(obj));

            return player;



        }

        /*Vector2D    toVector(VecPosition pos){

          return Vector2D(pos.getX(),pos.getY());

        };

        VecPosition    toVecPos(Vector2D pos){

          return VecPosition(pos.x,pos.y);

        };*/

        void		getSimulFromType	(const WorldModel &wm,ObjectT o,SimulT sim);

        double	An(double A1,double dORq,int n,bool q=0);
        double	An(double A1,double q,double d,int n);
        double	Sn(double A1,double dORq,int n,bool q=0);
        double	Sn(double A1,double q,double d,int n);
        double	N(double Sn,double A1,double dORq,bool q=0);

        void dynamicsSimulateBall(int cycle=1);
        void actionSimulateBall(Simul2 body,SoccerCommand bodyCommand,int cycle=1,bool ballCatched = false);
        void dynamicsSimulatePlayer(int cycle=1);
        void actionSimulatePlayer(SoccerCommand bodyCommand,int cycle=1);


        double		getDistance					(Simul2 sim);

        bool isGoalieSim();
        bool isTeammateSim();
        bool isOpponentSim();
        bool isKickable(Simul2 &ball)const;
        bool isKickable(VecPosition pos)const;
        bool isCollision(Simul2 &ball)const;
        double getActualSpeedProduct(Simul2 &ball,double power);
        double getActualPower(Simul2 &ball,double &speed)const;
        double getDashRate(int dir);
                VecPosition	getActualVelAfterDash(double power,int dir);
        double  maxTurnAng(double iAng=180);
        double  turnAng(double iAng);

        Simul2	finalSim(double minSpeed);

        int     turnTimeTo(double turnDir,double angTurn);
        int     turnTimeToPlusSim(VecPosition pos,double angTurn);
        int     dashTimeTo(VecPosition posEnd);
        int     calculateBallTimeTo(double dis,double minSpeed);


        double maxRand(Simul2   ball,SoccerCommand soc);
        VecPosition maxKickNoise(Simul2 ball,SoccerCommand soc,double ang);
        double maxRand(VecPosition posBall,double speed,SoccerCommand soc);
        VecPosition maxKickNoise(VecPosition posBall,double speed,SoccerCommand soc,double ang);


        int	timeTo(VecPosition pos);



        SoccerCommand   moveToPosSim               ( VecPosition   posTo,
                                                  double        angWhenToTurn,
                                                  double        dDistDashBack = 0.0,
                                                  bool          bMoveBack = false,
                                                  int           iCycles = 1        );
        SoccerCommand   dashToPointSim             ( VecPosition   pos,
                                                  int           iCycles = 1        );

        SoccerCommand   turnBodyToPointSim         ( VecPosition   pos,
                                                  int           iPos = 1           );
        SoccerCommand   turnBackToPointSim         ( VecPosition   pos,
                                                  int           iPos = 1           );
        SoccerCommand   directTowardsSim           ( VecPosition   posTo,
                                                  double        angWhenToTurn,
                                                  VecPosition   *pos = NULL,
                                                  VecPosition   *vel = NULL,
                                                  double        *angBody  = NULL   );
        double        getPowerForDash            ( VecPosition    posRelTo,
                                                   double         angBody,
                                                   VecPosition    vel,
                                                   double         dEffort,
                                                   int            iCycles = 1      );
        double        getAngleForTurn            ( double         angDesiredAngle,
                                                   double         dSpeed,
                                                   ObjectT        o=OBJECT_ILLEGAL);



};


#endif
