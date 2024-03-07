#include "Oci.h"

/*****************************************************************************/
/********************* CLASS SOCCERCOMMAND ***********************************/
/*****************************************************************************/



/*! This is a constructor for the SoccerCommand class. It creates a command
    using the passed arguments (with all default illegal values). Depending
    on the specified CommandT the parameters are used in different ways. See
    the method makeCommand for an explanation of these values.
    \param com commandType for this SoccerCommand
    \param d1 1st argument, meaning depends on com (default 22771)
    \param d2 2nd argument, meaning depends on com (default 22771)
    \param d3 3rd argument, meaning depends on com (default 22771)
    \return SoccerCommand with the specified parameters.*/
SoccerCommand::SoccerCommand( CommandT com, double d1, double d2, double d3 )
{
  // first initialize variables
  commandType = com;
  dPower      = 22771;
  dAngle      = 22771;


  if( com != CMD_SAY )
    makeCommand( commandType, d1, d2, d3 );
}

/*! This method create a SoccerCommand from the specified command type and the
    parameters. The parameters have a different meaning depending on the given
    command type. Not all command types are listed, since the other command
    types need different parameters. So see the other overloaded methods for
    that.
    - CMD_DASH:        d1 = power for dashing, d2 = angle of dashing
    - CMD_TURN:        d1 = angle body is turned
    - CMD_TURNNECK     d1 = angle neck is turned
    - CMD_KICK         d1 = power for kick command, d2 = angle for kick
    - CMD_MOVE         d1 = x position, d2 = y position, d3 = body_angle
    - CMD_CATCH        d1 = catch angle
    - CMD_CHANGEPLAYER d1 = player number, d2 = nr of new player type
    - CMD_ATTENTIONTO  d1 = which team, d2 = player number
    - CMD_TACKLE       d1 = power of the tackle, d2 = foul
    \param com command type specifying the kind of command
    \param d1 meaning depends on the specified command type (see above)
    \param d2 meaning depends on the specified command type (see above)
    \param d3 meaning depends on the specified command type (see above) */
void SoccerCommand::makeCommand( CommandT com, double d1,
                                 double d2, double d3 )
{
  // set the variables that have a meaning for the specified command type
  commandType = com;
  switch ( com )
  {
    case CMD_TURN:
    case CMD_TURNNECK:
    case CMD_CATCH:
      dAngle = d1;
      break;
    case CMD_DASH:
      dPower = d1;
      if(d2==22771)
        dAngle = 0;
      else
          dAngle = d2;

      break;
    case CMD_KICK:
      dPower = d1;
      dAngle = d2;
      break;
    case CMD_MOVE:
      dX = d1;
      dY = d2;
      if( d3 != 22771 )
        dAngle = d3;
      else
        dAngle = 0;
      break;
    case CMD_CHANGEPLAYER:
    case CMD_ATTENTIONTO:
    case CMD_POINTTO:
      dX = d1;
      dY = d2;
      break;
    case CMD_TACKLE:
          dAngle = d1;
          bFoul = d2;
          break;
    default:
      break;
  }
}



/*! This method returns whether this SoccerCommand is illegal, that is the
    SoccerCommand hasn't been filled yet. This means that no command would be
    performed when this command is sent to the server.
    \return bool indicating whether the current Command is illegal */
bool SoccerCommand::isIllegal( )
{
  return commandType == CMD_ILLEGAL;
}



void SoccerCommand::doCommand( PlayerAgent *agent  )
{
  // set the variables that have a meaning for the specified command type

      switch ( commandType )
  {
    case CMD_TURN:
        agent->doTurn(dAngle);
        break;
    case CMD_TURNNECK:
      agent->doTurnNeck(dAngle);
      break;
    case CMD_CATCH:
      agent->doCatch();
      break;

    case CMD_DASH:
      agent->doDash(dPower,dAngle);
      break;
    case CMD_KICK:
      agent->doKick(dPower,dAngle);
      break;
    case CMD_MOVE:
      agent->doMove(dX,dY);
      break;

    case CMD_CHANGEPLAYER:
    case CMD_ATTENTIONTO:
    case CMD_POINTTO:
      break;
    case CMD_TACKLE:
      agent->doTackle(dAngle,bFoul);
      break;
    default:
      break;
  }
}




/*****************************************************************************/
/********************* CLASS SIMUL ******************************************/
/*****************************************************************************/

Simul2::Simul2( PlayerAgent *pa)
{
  PA=pa;

}
/// ////////////////////////////
////////////////////////////////
Simul2::Simul2( )
{

}
/// ////////////////////////////
////////////////////////////////
void Simul2::operator = ( const Simul2 &d )
{

    PA=d.PA;
    simulType=d.simulType;
    objType=(d.objType);
    posGlob=(d.posGlob);
    posVelocity=(d.posVelocity);
    bodyAng=(d.bodyAng);
    conf=(d.conf);
    dashPowerRate=(d.dashPowerRate);
    inertiaMoment=(d.inertiaMoment);
    size=(d.size);
    decay=(d.decay);
    speedMax=(d.speedMax);
    staminaIncMax=(d.staminaIncMax);
    effortMax=(d.effortMax);
    HPS=(d.HPS);
}
/// ////////////////////////////
////////////////////////////////
bool Simul2::setTypeSimul( SimulT sim )
{
  simulType = sim;
  return true;
}
/// ////////////////////////////
////////////////////////////////
SimulT  Simul2::getTypeSimul() const
{
  return simulType;
}
/// ////////////////////////////
////////////////////////////////
bool Simul2::setObjTypeSimul( ObjectT o )
{
  objType = o;
  return true;
}
/// ////////////////////////////
////////////////////////////////
ObjectT  Simul2::getObjTypeSimul() const
{
  return objType;
}
/// ////////////////////////////
////////////////////////////////
bool Simul2::setPosSimul( VecPosition p )
{
  posGlob = p;
  return true;
}
/// ////////////////////////////
////////////////////////////////
VecPosition Simul2::getPosSimul() const
{
  return posGlob;
}
/// ////////////////////////////
////////////////////////////////
bool Simul2::setVelSimul( VecPosition p )
{
  posVelocity = p;
  return true;
}
/// ////////////////////////////
////////////////////////////////
VecPosition Simul2::getVelSimul() const
{
  return posVelocity;
}
/// ////////////////////////////
////////////////////////////////
bool	Simul2::setBodyAngSimul(AngleDeg a)
{
        bodyAng=a;

        return true;
}
/// ////////////////////////////
////////////////////////////////
AngleDeg    Simul2::getBodyAngSimul() const
{
        return 	bodyAng;
}
/// ////////////////////////////
////////////////////////////////
bool	Simul2::setConfidenceSimul(int a)
{
        conf=a;
        return true;
}
/// ////////////////////////////
////////////////////////////////
int	Simul2::getConfidenceSimul() const
{
        return 	conf;
}
/// ////////////////////////////
////////////////////////////////
bool	Simul2::setDecaySimul(double a)
{
        decay=a;
        return true;
}
/// ////////////////////////////
////////////////////////////////
double	Simul2::getDecaySimul() const
{
        return 	decay;
}
/// ////////////////////////////
////////////////////////////////
bool	Simul2::setDashPowerRateSimul(double a)
{
        dashPowerRate=a;
        return true;
}
/// ////////////////////////////
////////////////////////////////
double	Simul2::getDashPowerRateSimul() const
{
        return 	dashPowerRate;
}
/// ////////////////////////////
////////////////////////////////
bool	Simul2::setInertiaMomentSimul(double a)
{
        inertiaMoment=a;
        return true;
}
/// ////////////////////////////
////////////////////////////////
double	Simul2::getInertiaMomentSimul() const
{
        return 	inertiaMoment;
}
/// ////////////////////////////
////////////////////////////////
bool	Simul2::setSizeSimul(double a)
{
        size=a;
        return true;
}
/// ////////////////////////////
////////////////////////////////
double	Simul2::getSizeSimul() const
{
        return 	size;
}
/// ////////////////////////////
////////////////////////////////
bool	Simul2::setSpeedMaxSimul(double a)
{
        speedMax=a;
        return true;
}
/// ////////////////////////////
////////////////////////////////
double	Simul2::getSpeedMaxSimul() const
{
        return 	speedMax;
}
/// ////////////////////////////
////////////////////////////////
bool	Simul2::setStaminaIncMaxSimul(double a)
{
        staminaIncMax=a;
        return true;
}
/// ////////////////////////////
////////////////////////////////
double	Simul2::getStaminaIncMaxSimul() const
{
        return 	staminaIncMax;
}
/// ////////////////////////////
////////////////////////////////
bool	Simul2::setEffortMaxSimul(double a)
{
        effortMax=a;
        return true;
}
/// ////////////////////////////
////////////////////////////////
double	Simul2::getEffortMaxSimul() const
{
        return 	effortMax;
}
/// ////////////////////////////
////////////////////////////////
bool	Simul2::setHPSSimul(const PlayerType  * a)
{
        HPS=a;
        return true;
}
/// ////////////////////////////
////////////////////////////////
const PlayerType *	Simul2::getHPSSimul() const
{
        return 	HPS;
}
/// ////////////////////////////
////////////////////////////////
/// ////////////////////////////
////////////////////////////////
void Simul2::getSimulFromType(const  WorldModel &wm,ObjectT o,SimulT sim)
{


    ObjectT agentOBJ=(ObjectT)(OBJECT_TEAMMATE_1+wm.self().unum()-1);

    if(/*getNr(o)==wm.self().unum()*/sim==SIM_AGENT || agentOBJ==o){
        const SelfObject * player = &wm.self();

        setTypeSimul(sim);
        setObjTypeSimul(o);
        setPosSimul(toVecPos(player->pos()));
        setVelSimul(toVecPos(player->vel()));
        setBodyAngSimul(player->body());
        setConfidenceSimul(player->posCount());
        setDashPowerRateSimul(player->playerType().dashPowerRate());
        setInertiaMomentSimul(player->playerType().inertiaMoment());
        setSizeSimul(player->playerType().playerSize());
        setDecaySimul(player->playerType().playerDecay());
        setStaminaIncMaxSimul(player->playerType().staminaIncMax());
        setEffortMaxSimul(player->effort());
        setHPSSimul(player->playerTypePtr());
        setSpeedMaxSimul(player->playerType().finalSpeed(100,effortMax));


    }
    else if(isOpponent(o) || isTeammate(o)){

        const AbstractPlayerObject *player=getPlayer(wm,o);

        if(!player)
            return;

        setTypeSimul(sim);
        setObjTypeSimul(o);
        setPosSimul(toVecPos(player->pos()));
        setVelSimul(toVecPos(player->vel()));
        setBodyAngSimul(player->body());
        setConfidenceSimul(player->posCount());
        setDashPowerRateSimul(player->playerTypePtr()->dashPowerRate());
        setInertiaMomentSimul(player->playerTypePtr()->inertiaMoment());
        setSizeSimul(player->playerTypePtr()->playerSize());
        setDecaySimul(player->playerTypePtr()->playerDecay());
        setStaminaIncMaxSimul(player->playerTypePtr()->staminaIncMax());
        setEffortMaxSimul(player->playerTypePtr()->effortMax());
        setHPSSimul(player->playerTypePtr());
        setSpeedMaxSimul(player->playerTypePtr()->finalSpeed(100,effortMax));


    }
    else if(o==OBJECT_BALL ){


        setTypeSimul(sim);
        setObjTypeSimul(o);
        setPosSimul(toVecPos(wm.ball().pos()));
        setVelSimul(toVecPos(wm.ball().vel()));
        setConfidenceSimul(wm.ball().posCount());
        setSizeSimul(0.085);
        setDecaySimul(0.94);
        setSpeedMaxSimul(3.1);
        setEffortMaxSimul(0.0);

    }

}
/// ////////////////////////////
////////////////////////////////
double	Simul2::An(double A1,double dORq,int n,bool q){

        if(q)
                return ( A1 * pow(dORq,n-1) );
        else
                return ( A1 + (n-1)*dORq );

}

/// ////////////////////////////
////////////////////////////////
double	Simul2::An(double A1,double q,double d,int n){

        return ( (A1*pow(q,n-1)) + (d*(1-pow(q,n-1))/(1-q)) );

}
/// ////////////////////////////
////////////////////////////////
double	Simul2::Sn(double A1,double dORq,int n,bool q){

        if(q)
                return ( A1 * (1-pow(dORq,n)) / (1-dORq) );
        else
                return ( (n/2) * ( A1 + An(A1,dORq,n) ) );

}
/// ////////////////////////////
////////////////////////////////
double	Simul2::Sn(double A1,double q,double d,int n){

        return ( (A1*(1-pow(q,n))/(1-q)) + ( d/(1-q)*(n-((1-pow(q,n))/(1-q))) ) );

}
/// ////////////////////////////
////////////////////////////////
double	Simul2::N(double Sn,double A1,double dORq,bool q){

        if(q)
                return log( 1 - ( Sn * (  1-dORq  ) / A1 ) ) / log( dORq ) ;

        else
            return ((dORq-A1)+(sqrt(pow(dORq-A1,2)+8*dORq*(Sn))))/(2*dORq);


}
/*/// ////////////////////////////
////////////////////////////////
double	Simul2::N(double Sn,double A1,double q,double d){

        double X=1-q;

        double Y=1-( (Sn*X) )


}
*/
/// ////////////////////////////
////////////////////////////////
void Simul2::dynamicsSimulateBall(int cycle)
{

    //int		n 	= cycle;
    double	a	= posVelocity.getMagnitude();
    //double 	q 	= decay;
    double	decayN	= pow(decay,cycle);
    double	S	= a*(decayN-1)/(decay-1);

    posGlob+=VecPosition(S,posVelocity.getDirection(),POLAR);
    posVelocity*=decayN;

}
/// ////////////////////////////
////////////////////////////////
void Simul2::actionSimulateBall(Simul2 body,SoccerCommand bodyCommand,int cycle,bool ballCatched)
{

    for(int i = 1 ;i <= cycle;i++){

        if( bodyCommand.commandType == CMD_KICK && !ballCatched && body.isKickable(*this))
        {

                        float iPower = bodyCommand.dPower;
                        float iAngle = bodyCommand.dAngle;

                        AngleDeg ang = AngleDeg::normalize_angle(iAngle+body.getBodyAngSimul().degree());
                        posVelocity += VecPosition( body.getActualSpeedProduct(*this,iPower), ang.degree(),POLAR ) ;
                        /*
                        if( posVelocity.getMagnitude() > speedMax )
                                posVelocity.setMagnitude( speedMax );
*/
        }

        if(!ballCatched)
        {
                        posGlob+=posVelocity;
                        posVelocity*=decay;
        }
        else{
                        //posGlob=body.getPosSimul();
                        posVelocity=VecPosition(0,0);
        }
        body.actionSimulatePlayer(bodyCommand,1);

   }
}
/// ////////////////////////////
////////////////////////////////
void Simul2::dynamicsSimulatePlayer(int cycle){

    //int		n 	= cycle;
    double	a	= posVelocity.getMagnitude();
    //double 	q 	= decay;
    double	decayN	= pow(decay,cycle);
    double	S	= a*(decayN-1)/(decay-1);

    posGlob+=VecPosition(S,posVelocity.getDirection(),POLAR);
    posVelocity*=decayN;

}
/// ////////////////////////////
////////////////////////////////
void Simul2::actionSimulatePlayer(SoccerCommand bodyCommand,int cycle){

        double	edp=0;
        double	ang=0;

        VecPosition vel=VecPosition(0,0);

        if( bodyCommand.commandType == CMD_DASH){

                float iPower = bodyCommand.dPower;
                float iAngle = bodyCommand.dAngle;

                if(iPower<0){
                        iAngle = 180;//AngleDeg::normalize_angle(180);
                        iPower*=-1;
                }

                double dash_rate=getDashRate(iAngle);



                edp = dashPowerRate*iPower*effortMax*dash_rate;
                ang = AngleDeg::normalize_angle(bodyAng.degree()+iAngle);
                vel = VecPosition( edp , ang ,POLAR);

                for(int i = 1 ;i <= cycle;i++){

                        posVelocity += vel;

                        if( posVelocity.getMagnitude() > speedMax ){

                                posVelocity.setMagnitude( speedMax );
                                cout<<"setting Speed Player Max"<<endl;
                        }


                        posGlob += posVelocity;
                        posVelocity*=decay;

                }
                /*double dist=Sn(posVelocity.getMagnitude()+edp,decay,edp,cycle);
                double speed=An(posVelocity.getMagnitude()+edp,decay,edp,cycle);

                posGlob +=VecPosition(dist,posVelocity.getDirection(),POLAR);
                posVelocity.setMagnitude(speed*decay);
                */
        }

        else if( bodyCommand.commandType == CMD_MOVE && cycle > 0 ){

                float iAngle = bodyCommand.dAngle;
                posGlob = VecPosition(bodyCommand.dX,bodyCommand.dY);
                setBodyAngSimul(AngleDeg::normalize_angle(iAngle));
                posVelocity = VecPosition(0,0);
        }

        else if( bodyCommand.commandType == CMD_TURN){

                float iAngle =  bodyCommand.dAngle;

                for(int i = 1 ;i <= cycle;i++){

                        ang = this->maxTurnAng(iAngle);
                        setBodyAngSimul(AngleDeg::normalize_angle(bodyAng.degree()+ang));
                        posGlob += posVelocity;
                        posVelocity*=decay;
                }

        }
        else{

            this->dynamicsSimulatePlayer(cycle);


        }



}
/// ////////////////////////////
////////////////////////////////
double		Simul2::getDistance(Simul2 sim){

        return getPosSimul().getDistanceTo(sim.getPosSimul());

}
/// ////////////////////////////
////////////////////////////////
bool Simul2::isGoalieSim(){

        if(isGoalie(objType))
                return true;
        return false;
}
/// ////////////////////////////
////////////////////////////////
bool Simul2::isTeammateSim(){

        if(isTeammate(objType))
                return true;
        return false;

}
/// ////////////////////////////
////////////////////////////////
bool Simul2::isOpponentSim(){

        if(isOpponent(objType))
                return true;

        return false;

}
/// ////////////////////////////
////////////////////////////////
bool Simul2::isKickable(Simul2 &ball)const
{

    return (getPosSimul().getDistanceTo(ball.getPosSimul())<HPS->kickableArea());

}
/// ////////////////////////////
////////////////////////////////
bool Simul2::isKickable(VecPosition pos)const
{

    return (getPosSimul().getDistanceTo(pos)<HPS->kickableArea());

}
/// ////////////////////////////
////////////////////////////////
bool Simul2::isCollision(Simul2 &ball)const
{

    return (getPosSimul().getDistanceTo(ball.getPosSimul())<=this->size+ball.getSizeSimul()+0.1);

}
/// ////////////////////////////
////////////////////////////////
double Simul2::getActualSpeedProduct(Simul2 &ball,double power)
{

    power=doubleNormalize(power,-100.0,+100.0);
    double dir_diff=fabs(AngleDeg::normalize_angle(bodyAng.degree()-(ball.getPosSimul()-getPosSimul()).getDirection()));
    double dist=getPosSimul().getDistanceTo(ball.getPosSimul())-ball.getSizeSimul()-this->getSizeSimul();

    return power * 0.027* ( 1 - (0.25 * (dir_diff/180.0)) - (0.25 * (dist / HPS->kickableMargin())));
}
/// ////////////////////////////
////////////////////////////////
double Simul2::getActualPower(Simul2 &ball,double &speed)const
{

    double dir_diff=fabs(AngleDeg::normalize_angle(bodyAng.degree()-(ball.getPosSimul()-getPosSimul()).getDirection()));
    double dist=getPosSimul().getDistanceTo(ball.getPosSimul())-ball.getSizeSimul()-this->getSizeSimul();
    double power=speed/( 0.027* ( 1 - (0.25 * (dir_diff/180.0 ))- (0.25 * (dist / HPS->kickableMargin()))));
    if(power>100){
        power=100;
        speed = 100 * ( 0.027* ( 1 - 0.25 * dir_diff/180.0 - 0.25 * dist / HPS->kickableMargin() ));
    }

    return power;

}
/// ////////////////////////////
////////////////////////////////
double Simul2::getDashRate(int dir){

    double dir_rate=1;

    dir=AngleDeg::normalize_angle(dir);
    dir=fabs(dir);

    if(dir!=0){
        dir_rate = (fabs( dir ) > 90.0)?
                   0.6-((0.6-0.4)* ( 1.0 - ( fabs( dir ) - 90.0 ) / 90.0 ) )
                   : 0.4 + ( ( 1.0 - 0.4 )* ( 1.0 - fabs( dir ) / 90.0 ) );

        dir_rate = doubleNormalize(  dir_rate,0.0, 1.0 );
    }

        return dir_rate;

}
/// ////////////////////////////
////////////////////////////////
VecPosition Simul2::getActualVelAfterDash(double power,int dir)
{

    power=doubleNormalize(power,-100.0,+100.0);

    VecPosition velocity=posVelocity;

    float iPower = power;
    float iAngle = dir;

    if(iPower<0){
         iAngle = 180;
         iPower*=-1;
    }
    double dash_rate=getDashRate(dir);



    double edp = dashPowerRate*iPower*effortMax*dash_rate;
    velocity += VecPosition( edp , AngleDeg::normalize_angle(bodyAng.degree()+iAngle),POLAR) ;

    /*if( velocity.getMagnitude() > speedMax )
            velocity.setMagnitude( speedMax );
    */
    return velocity;

}
/// ////////////////////////////
////////////////////////////////
double  Simul2::maxTurnAng(double iAng){

    iAng=doubleNormalize(iAng,-180,180);
    return iAng/(1.0 + inertiaMoment * posVelocity.getMagnitude());

}
/// ////////////////////////////
////////////////////////////////
double  Simul2::turnAng(double iAng){

    double turn = iAng*(1.0 + inertiaMoment * posVelocity.getMagnitude());
    return  doubleNormalize(turn,-180,180);

}
////////////////////////////
/// ////////////////////////////////
Simul2 Simul2::finalSim(double minSpeed){

    double dis=(posVelocity.getMagnitude()-minSpeed)/(1-decay);

    if(dis<0){
        return  *this;
    }
    Simul2 temp(PA);
    temp=*this;
    temp.posGlob+=VecPosition(dis,posVelocity.getDirection(),POLAR);
    temp.posVelocity.setMagnitude(minSpeed);
    return temp;

}
/// ////////////////////////////
////////////////////////////////
int     Simul2::turnTimeTo(double turnDir,double angTurn){


    int cycle=0;

    double  speed=posVelocity.getMagnitude();

    turnDir =fabs(AngleDeg::normalize_angle(turnDir));

    while(turnDir > angTurn){

        turnDir-=(180/(1.0+inertiaMoment*speed));
        speed*=decay;
        cycle++;

    }

    return cycle;

}
/// ////////////////////////////
////////////////////////////////
int     Simul2::turnTimeToPlusSim(VecPosition pos,double angTurn){


    int cycle=0;

    double  dir	= (pos-getPosSimul()+getVelSimul()).getDirection();
//    double  ang = AngleDeg::normalize_angle(dir-bodyAng.degree());
    double  ang = VecPosition::normalizeAngle(dir-bodyAng.degree());

    SoccerCommand   soc=CMD_TURN;

    while(fabs(ang) > angTurn){

        soc.dAngle = this->turnAng(ang);

        this->actionSimulatePlayer(soc,1);
        cycle++;
        dir = (pos-getPosSimul()+getVelSimul()).getDirection();
        //    ang = AngleDeg::normalize_angle(dir-bodyAng.degree());
        ang = VecPosition::normalizeAngle(dir-bodyAng.degree());

    }
    return cycle;

}
/// ////////////////////////////
////////////////////////////////
int     Simul2::dashTimeTo(VecPosition posEnd){

    Simul2 temp(PA);
    temp=*this;

    int cycle=0;

    double dis=posEnd.getDistanceTo(temp.getPosSimul());
    double speed=temp.posVelocity.getMagnitude();
    double edp=dashPowerRate*100*temp.effortMax;

    while(dis>temp.HPS->kickableMargin()){
        speed+=edp;
        (speed>temp.speedMax)?speed=temp.speedMax:1;
        dis-=speed;
        speed*=decay;
        cycle++;
    }

    return cycle;

}
/// ////////////////////////////
////////////////////////////////
int     Simul2::calculateBallTimeTo(double dis,double minSpeed){

    Simul2   temp(PA);
    temp=this->finalSim(minSpeed);
    double finalDist=temp.getPosSimul().getDistanceTo(getPosSimul());
    if(finalDist<dis)
        return 22771;

    double X=1-(dis*(1-decay)/getVelSimul().getMagnitude());

    return  ceil((log10(X))/(log10(decay)));

    /*double speedThr=(finalDist-dis)*(1-decay);
    double speed=posVelocity.getMagnitude();

    return  ceil((log10(speed/speedThr))/(log10(decay)));*/


}
/// ////////////////////////////
////////////////////////////////
double Simul2::maxRand(Simul2 ball,SoccerCommand soc){

    double dir_diff=fabs(AngleDeg::normalize_angle(bodyAng.degree()-(ball.getPosSimul()-getPosSimul()).getDirection()));
    double dist=getPosSimul().getDistanceTo(ball.getPosSimul())-ball.getSizeSimul()-this->getSizeSimul();

    double pos_rate = 0.5 + 0.25 * ( dir_diff/180 + dist/HPS->kickableMargin() );    // [0.5, 1.0]

    double speed_rate = 0.5 + 0.5 * ( ball.getVelSimul().getMagnitude() / ( /*ball maxSpeed*/3 * /* ballDecay */ 0.94 ) );

    // [0, 2*kick_rand]
    return HPS->kickRand() * ( soc.dPower/ /* maxPower*/ 100 ) * ( pos_rate + speed_rate );


}
/// ////////////////////////////
////////////////////////////////
VecPosition Simul2::maxKickNoise(Simul2   ball,SoccerCommand soc,double ang){


    return  VecPosition( /*drand( 0.0, max_rand )*/ maxRand(ball,soc), /*drand( -M_PI, M_PI )*/ ang ,POLAR );
}
/// ////////////////////////////
////////////////////////////////
double Simul2::maxRand(VecPosition posBall,double speed,SoccerCommand soc){

    double dir_diff=fabs(AngleDeg::normalize_angle(bodyAng.degree()-(posBall-getPosSimul()).getDirection()));
    double dist=getPosSimul().getDistanceTo(posBall) - 0.085 - this->getSizeSimul();

    double pos_rate = 0.5 + 0.25 * ( dir_diff/180 + dist/HPS->kickableMargin() );    // [0.5, 1.0]

    double speed_rate = 0.5 + 0.5 * ( speed / ( /*ball maxSpeed*/3 * /* ballDecay */ 0.94 ) );

    // [0, 2*kick_rand]
    return HPS->kickRand() * ( soc.dPower/ /* maxPower*/ 100 ) * ( pos_rate + speed_rate );


}
/// ////////////////////////////
////////////////////////////////
VecPosition Simul2::maxKickNoise(VecPosition posBall,double speed,SoccerCommand soc,double ang){


    return  VecPosition( /*drand( 0.0, max_rand )*/ maxRand(posBall,speed,soc), /*drand( -M_PI, M_PI )*/ ang ,POLAR );
}
/// ////////////////////////////
////////////////////////////////
int	Simul2::timeTo(VecPosition pos){

    if(getPosSimul().getDistanceTo(pos)<getHPSSimul()->kickableArea())
        return 0;
    int time=turnTimeToPlusSim(pos,5);

    time+=dashTimeTo(pos);
    return time;
}

/// ////////////////////////////
////////////////////////////////
SoccerCommand Simul2::moveToPosSim( VecPosition posTo, double angWhenToTurn,
                            double dDistBack, bool bMoveBack, int iCycles )
{
// previously we only turned relative to position in next cycle, now take
// angle relative to position when you're totally rolled out...
//  VecPosition posPred   = WMS->predictAgentPos( 1, 0 );
  VecPosition posAgent  = getPosSimul();
  VecPosition posPred   = finalSim(0.0).getPosSimul();

  double      angBody   = getBodyAngSimul().degree();
  double      angTo     = ( posTo - posPred ).getDirection();
              angTo     = VecPosition::normalizeAngle( angTo - angBody );
  double      angBackTo = VecPosition::normalizeAngle( angTo + 180 );

  double      dDist     = posAgent.getDistanceTo( posTo );

  if( bMoveBack )
  {
    if( fabs( angBackTo ) < angWhenToTurn )
      return dashToPointSim( posTo, iCycles );
    else
      return turnBackToPointSim( posTo );
  }
  else if(  fabs( angTo     ) < angWhenToTurn ||
           (fabs( angBackTo ) < angWhenToTurn && dDist < dDistBack ) )
    return dashToPointSim( posTo, iCycles );
  else
    return directTowardsSim( posTo, angWhenToTurn );
//return turnBodyToPointSim( posTo );
}
/// ////////////////////////////
////////////////////////////////
SoccerCommand Simul2::dashToPointSim( VecPosition pos, int iCycles )
{
  double dDashPower = getPowerForDash(
                                 pos - getPosSimul(),
                                 getBodyAngSimul().degree(),
                                 getVelSimul(),
                                 getEffortMaxSimul(),
                                 iCycles                              );
  return SoccerCommand( CMD_DASH, dDashPower, 0.0 );
}
/// ////////////////////////////
////////////////////////////////
SoccerCommand Simul2::turnBackToPointSim( VecPosition pos, int iCycles )
{

  dynamicsSimulatePlayer(iCycles);
  VecPosition posGlobal = getPosSimul();
  double angTurn        = (pos - posGlobal).getDirection();
  angTurn              -= (getBodyAngSimul().degree() + 180);
  angTurn               = VecPosition::normalizeAngle( angTurn );
  angTurn               = getAngleForTurn( angTurn, getVelSimul().getMagnitude()/pow(decay,iCycles),
                                               getObjTypeSimul() );

  return SoccerCommand( CMD_TURN, angTurn );
}
/// ////////////////////////////
////////////////////////////////
SoccerCommand Simul2::directTowardsSim( VecPosition posTurnTo,
   double angWhenToTurn, VecPosition *pos, VecPosition *vel, double *angBody  )
{
//  return turnBodyToPointSim( posTurnTo );
  // copy values or initialize when not set
  VecPosition posAgent= (pos    ==NULL)?getPosSimul():*pos;
  VecPosition velAgent= (vel    ==NULL)?getVelSimul():*vel;
  double  angBodyAgent= (angBody==NULL)?getBodyAngSimul().degree():*angBody;

  // first predict what happens when the agents rolls out.
  VecPosition posPred    = finalSim(0.0).getPosSimul();
  double      angTo      = ( posTurnTo - posPred ).getDirection();
  double      ang        = VecPosition::normalizeAngle( angTo - angBodyAgent );
  double      angNeck = 0;

  int iTurn = 0;
  while( fabs( ang ) > angWhenToTurn && iTurn < 5 )
  {
    iTurn++;
    SoccerCommand   soc=CMD_TURN;
    soc.dAngle=getAngleForTurn( ang, velAgent.getMagnitude() ,getObjTypeSimul() );

    posPred    = finalSim(0.0).getPosSimul();
    angTo      = ( posTurnTo - posPred ).getDirection();
    ang        = VecPosition::normalizeAngle( angTo - angBodyAgent );
    angNeck = 0;


    ang = VecPosition::normalizeAngle( angTo - angBodyAgent );
  }
  posAgent = (pos    ==NULL)?getPosSimul():*pos;
  velAgent = (vel    ==NULL)?getVelSimul():*vel;
  angBodyAgent = (angBody==NULL)?getBodyAngSimul().degree():*angBody;

  switch( iTurn )
  {
    case 0:   cerr << "direct towards: 0 turns" ;
              return SoccerCommand( CMD_ILLEGAL );
    case 1:
    case 2:   return turnBodyToPointSim( posTurnTo, 2 );
    default:  return dashToPointSim(
             (pos==NULL)?getPosSimul():*pos   );  // stop
  }
}
/// ////////////////////////////
////////////////////////////////
SoccerCommand Simul2::turnBodyToPointSim( VecPosition pos, int iCycles )
{
  dynamicsSimulatePlayer(iCycles);
  VecPosition posGlobal = getPosSimul();
  double angTurn        = (pos - posGlobal).getDirection();
  angTurn              -= getBodyAngSimul().degree();
  angTurn               = VecPosition::normalizeAngle( angTurn );
  angTurn               = getAngleForTurn( angTurn, getVelSimul().getMagnitude()/pow(decay,iCycles),
                                               getObjTypeSimul() );

  return SoccerCommand( CMD_TURN, angTurn );
}
/// ////////////////////////////
////////////////////////////////
double Simul2::getPowerForDash( VecPosition posRelTo, double angBody,
              VecPosition vel, double dEffort, int iCycles )
{
  // the distance desired is the x-direction to the relative position we
  // we want to move to. If point lies far away, we dash maximal. Furthermore
  // we subtract the x contribution of the velocity because it is not necessary
  // to dash maximal.
  double dDist = posRelTo.rotate(-angBody).getX(); // get distance in direction
  if( iCycles <= 0 ) iCycles = 1;
  double dAcc  = dDist *  ( 1 - getDecaySimul() )/( 1 - pow( getDecaySimul(), iCycles ) ) ;
                                                   // get speed to travel now
  if( dAcc > getSpeedMaxSimul() )             // if too far away
    dAcc = getSpeedMaxSimul();                // set maximum speed
  dAcc -= vel.rotate(-angBody).getX();             // subtract current velocity

  // acceleration = dash_power * dash_power_rate * effort ->
  // dash_power = acceleration / (dash_power_rate * effort )
  double dDashPower = dAcc/(getHPSSimul()->dashPowerRate() * dEffort );
  if( dDashPower > 100 )
    return 100;
  else if( dDashPower < 0.0 )
    return 0.0;
  else
    return dDashPower;
}

double Simul2::getAngleForTurn( double angDesiredAngle, double dSpeed,
                                    ObjectT obj )
{
  AngDeg a = angDesiredAngle * (1.0 + getInertiaMomentSimul() * dSpeed );
  if( a > 180.0 )
    return 180.0 ;
  else if ( a < -180.0 )
    return -180.0 ;
  else
    return a;
}
