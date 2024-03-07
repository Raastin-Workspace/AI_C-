#include "Simul.h"

/*****************************************************************************/
/********************* CLASS SIMULATEACTION ***********************************/
/*****************************************************************************/

SimulateAction::SimulateAction( ActionT act ,  double d1 ,  double d2)
{
 // first initialize variables
 actionType = act;
//  dPower = UNKNOWN_VALUE;
//  dAngle = UNKNOWN_VALUE;
//  dX = UNKNOWN_VALUE;
//  dY = UNKNOWN_VALUE;
//  bFoul = false;

     switch ( act )
     {
       case DASH:
         dPower = d1;
         if(d2==UNKNOWN_VALUE)
             dAngle = 0.0;
         else
             dAngle = d2;
         break;
       case TURN:
           dAngle = d1;
           break;
       case KICK:
           dPower = d1;
           dAngle = d2;
           break;
       case TACKLE:
         dAngle = d1;
         bFoul = ( fabs( d2 - 0.0 ) > EPS );
         break;
       case MOVE:
           dX = d1;
           dY = d2;
           break;
       case CATCH:
         break;
       default:
           break;
   }


}

void SimulateAction::doAction( PlayerAgent *agent  )
{
 // set the variables that have a meaning for the specified command type

   switch ( actionType )
   {
       case DASH:
           agent->doDash(dPower , dAngle);
           break;
       case TURN:
           agent->doTurn(dAngle);
           break;
       case KICK:
           agent->doKick(dPower , dAngle);
           break;
       case TACKLE:
           agent->doTackle(dAngle , bFoul);
           break;
       case MOVE:
           agent->doMove(dX , dY);
           break;
       case CATCH:
           agent->doCatch();
           break;
       default:
           break;
   }
}


/*****************************************************************************/
/********************* CLASS SIMUL ******************************************/
/*****************************************************************************/


/// ////////////////////////////
////////////////////////////////
void Simul :: operator = ( const Simul &s ){

   simulType = s.simulType ;
   globalPosition = s.globalPosition ;
   velocity = s.velocity ;
   bodyAngle = s.bodyAngle ;
   PT = s.PT ;
   effort = s.effort ;
   playerNumber = s.playerNumber ;
   decay = s.decay ;
   size = s.size ;
   PA = s.PA ;
}
/// ////////////////////////////
////////////////////////////////
Simul :: Simul ( PlayerAgent * pa ){

   PA = pa ;
}
/// ////////////////////////////
////////////////////////////////
Simul :: Simul( ){

}
/// ////////////////////////////
////////////////////////////////
Simul :: Simul ( PlayerAgent * pa , SimulT simT , int nr ){

   PA = pa ;

   if( simT == SIM_BALL ){

       setBallSim( & PA->world().ball() ) ;
   }
   else if( simT == SIM_AGENT ){

       setSelfSim( & PA->world().self() ) ;
   }
   else if( simT == SIM_TEAMMATE ){

       setTeammateSim( PA->world().ourPlayer( nr ) ) ;
   }
   else{

       setOpponentSim( PA->world().theirPlayer( nr ) ) ;
   }
}
/// ////////////////////////////
////////////////////////////////
Simul :: Simul ( PlayerAgent * pa , const AbstractPlayerObject * player ){

    PA = pa ;
    setSimulType( SIM_PLAYER ) ;
    setPosition( player->pos() ) ;
    setVelocity( player->vel() ) ;
    setBodyAngle( player->body().degree() ) ;
    setPT( player->playerTypePtr() ) ;
    setEffort( player->playerTypePtr()->effortMax() ) ;
    setPlayerNumber( player->unum() ) ;
    setDecay( player->playerTypePtr()->playerDecay() ) ;
    setSize( PLAYER_SIZE ) ;

}
/// ////////////////////////////
////////////////////////////////
void Simul :: setSelfSim ( const SelfObject * selfPlayer ){

   setSimulType( SIM_AGENT ) ;
   setPosition( selfPlayer->pos() ) ;
   setVelocity( selfPlayer->vel() ) ;
   setBodyAngle( selfPlayer->body().degree() ) ;
   setPT( selfPlayer->playerTypePtr() ) ;
   setEffort( selfPlayer->effort() ) ;
   setPlayerNumber( selfPlayer->unum() ) ;
   setDecay( selfPlayer->playerType().playerDecay() ) ;
   setSize( PLAYER_SIZE ) ;
}
/// ////////////////////////////
////////////////////////////////
void    Simul :: setBallSim( const BallObject * ball ){

   setSimulType( SIM_BALL ) ;
   setPosition( ball->pos() ) ;
   setVelocity( ball->vel() ) ;
   setDecay( BALL_DECAY ) ;
   setSize( BALL_SIZE ) ;
}
/// ////////////////////////////
////////////////////////////////
void    Simul :: setTeammateSim( const AbstractPlayerObject * player ){

   setSimulType( SIM_TEAMMATE ) ;
   setPosition( player->pos() ) ;
   setVelocity( player->vel() ) ;
   setBodyAngle( player->body().degree() ) ;
   setPT( player->playerTypePtr() ) ;
   setEffort( player->playerTypePtr()->effortMax() ) ;
   setPlayerNumber( player->unum() ) ;
   setDecay( player->playerTypePtr()->playerDecay() ) ;
   setSize( PLAYER_SIZE ) ;
}
/// ////////////////////////////
////////////////////////////////
void    Simul :: setOpponentSim( const AbstractPlayerObject * player ){

   setSimulType( SIM_OPPONENT ) ;
   setPosition( player->pos() ) ;
   setVelocity( player->vel() ) ;
   setBodyAngle( player->body().degree() ) ;
   setPT( player->playerTypePtr() ) ;
   setEffort( player->playerTypePtr()->effortMax() ) ;
   setPlayerNumber( player->unum() ) ;
   setDecay( player->playerTypePtr()->playerDecay() ) ;
   setSize( PLAYER_SIZE ) ;
}
/// ////////////////////////////
////////////////////////////////
/**! medriki bool*/ void Simul :: setSimulType( SimulT s ){

   simulType = s ;
   /**! medriki return true;*/
}
/// ////////////////////////////
////////////////////////////////
SimulT  Simul :: getSimulType() const {

   return simulType ;
}
/// ////////////////////////////
////////////////////////////////
/**! medriki bool*/ void Simul :: setPlayerNumber( int n ){

   playerNumber = n ;
   /**! medriki return true;*/
}
/// ////////////////////////////
////////////////////////////////
int  Simul :: getPlayerNumber() const {

   return playerNumber ;
}
/// ////////////////////////////
////////////////////////////////
/**! medriki bool*/ void Simul :: setPosition( Vector2D p ){

   globalPosition = p ;
   /**! medriki return true;*/
}
/// ////////////////////////////
////////////////////////////////
Vector2D Simul :: getPosition() const {

   return globalPosition ;
}
/// ////////////////////////////
////////////////////////////////
/**! medriki bool*/ void Simul :: setVelocity( Vector2D v ){

   velocity = v ;
   /**! medriki return true;*/
}
/// ////////////////////////////
////////////////////////////////
Vector2D Simul :: getVelocity() const {

   return velocity ;
}
/// ////////////////////////////
////////////////////////////////
/**! medriki bool*/ void	Simul :: setBodyAngle( AngleDeg a ){

   bodyAngle = a ;
   /**! medriki return true;*/
}
/// ////////////////////////////
////////////////////////////////
AngleDeg Simul :: getBodyAngle() const {

   return 	bodyAngle ;
}
/// ////////////////////////////
////////////////////////////////
/**! medriki bool*/ void	Simul :: setDecay( double d ){

   decay = d ;
   /**! medriki return true;*/
}
/// ////////////////////////////
////////////////////////////////
double	Simul :: getDecay() const {

   return 	decay ;
}
/// ////////////////////////////
////////////////////////////////
/**! medriki bool*/ void	Simul :: setEffort( double e ){

   effort = e ;
   /**! medriki return true;*/
}
/// ////////////////////////////
////////////////////////////////
double	Simul :: getEffort() const {

   return 	effort ;
}
/// ////////////////////////////
////////////////////////////////
/**! medriki bool*/ void	Simul :: setSize( double s ){

   size = s ;
   /**! medriki return true;*/
}
/// ////////////////////////////
////////////////////////////////
double	Simul :: getSize() const {

   return 	size ;
}
/// ////////////////////////////
////////////////////////////////
/**! medriki bool*/ void	Simul :: setPA( PlayerAgent * pa ){

   PA = pa ;
   /**! medriki return true;*/
}
/// ////////////////////////////
////////////////////////////////
PlayerAgent *	Simul :: getPA() const {

   return 	PA ;
}
/// ////////////////////////////
////////////////////////////////
/**! medriki bool*/ void	Simul :: setPT( const PlayerType  * p ){

   PT = p ;
   /**! medriki return true;*/
}
/// ////////////////////////////
////////////////////////////////
const PlayerType *	Simul :: getPT() const {

   return 	PT ;
}
/// ////////////////////////////
////////////////////////////////
double	Simul :: An( double A1 , double dORq , int n , bool q ){

   if( q ){

       return ( A1 * pow( dORq , n - 1 ) ) ;
   }
   else{

       return ( A1 + ( n - 1 ) * dORq ) ;
   }
}

/// ////////////////////////////
////////////////////////////////
double	Simul :: An( double A1 , double q , double d , int n ){

   return ( ( A1 * pow( q , n - 1 ) ) + ( d * ( 1 - pow( q , n - 1 ) ) / ( 1 - q ) ) ) ;
}
/// ////////////////////////////
////////////////////////////////
double	Simul :: Sn( double A1 , double dORq , int n , bool q ){

   if( q ){

       return ( A1 * ( 1 - pow( dORq , n ) ) / ( 1 - dORq ) ) ;
   }
   else{

       return ( ( n / 2 ) * ( A1 + An ( A1 , dORq , n ) ) ) ;
   }
}
/// ////////////////////////////
////////////////////////////////
double	Simul :: Sn( double A1 , double q , double d , int n ){

   return ( ( A1 * ( 1 - pow( q , n ) ) / ( 1 - q ) ) + ( d / ( 1 - q ) * ( n - ( ( 1 - pow( q , n ) ) / ( 1 - q ) ) ) ) ) ;

}
/// ////////////////////////////
////////////////////////////////
double	Simul :: Sn2( double A1 , double q , double d , double n ){

   return ( ( A1 * ( 1 - pow( q , n ) ) / ( 1 - q ) ) + ( d / ( 1 - q ) * ( n - ( ( 1 - pow( q , n ) ) / ( 1 - q ) ) ) ) ) ;

}
/// ////////////////////////////
////////////////////////////////
double	Simul :: N( double Sn , double A1 , double dORq , bool q ){

       if( q ){

           return log( ( 1 - ( Sn * ( 1 - dORq ) / A1 ) ) / log( dORq ) ) ;
       }
       else{

           return ( ( dORq - A1 ) + ( sqrt( pow( dORq - A1 , 2 ) + 8 * dORq * ( Sn ) ) ) ) / ( 2 * dORq ) ;
       }

}
/*/// ////////////////////////////
////////////////////////////////
double	Simul :: N(double Sn , double A1 , double q , double d ){

       double X=1-q;

       double Y=1-( (Sn*X) )


}
*/
/// ////////////////////////////
////////////////////////////////
void Simul :: dynamicsBallSimulate( int cycle ){

   /**! medriki */
//    int         n 	= cycle;
//    double	a1	= this->getVelocity().length();
//    double 	q 	= this->getDecay();
//    double	qN	= pow(q , n);
//    double	S	= a1*(qN-1)/(q-1);

//    this->setPosition(this->getPosition()+Vector2D::polar2vector(S , this->getVelocity().dir().degree()));
//    this->setVelocity(this->getVelocity()*qN);

   this->globalPosition += Vector2D::polar2vector( this->velocity.length() * ( 1 - pow( BALL_DECAY , cycle ) ) / ( MINES_ONE_BALL_DECAY ) , this->velocity.dir().degree() ) ;
   this->velocity *= pow( BALL_DECAY , cycle ) ;
}
/// ////////////////////////////
////////////////////////////////
void Simul :: dynamicsBallSimulate_1_cycle(){

//    this->setPosition(this->getPosition() + this->getVelocity() );
//    this->setVelocity(this->getVelocity()*this->getDecay());

   this->globalPosition += this->getVelocity() ;
   this->velocity *= BALL_DECAY ;
}
/// ////////////////////////////
////////////////////////////////
void Simul :: actionBallSimulate( Simul & body , SimulateAction & bodyCommand /* , int cycle , bool ballCatched*/){

   /** it's need to repair */
   if( bodyCommand.actionType == KICK && body.isKickable( * this ) )    {
   /**! medriki */
//        AngleDeg ang = AngleDeg::normalize_angle( bodyCommand.dAngle + body.getBodyAngle().degree() ) ;
//        this->setVelocity( this->getVelocity() + Vector2D::polar2vector( body.getActualSpeedProduct( * this , bodyCommand.dPower ) ,  ang.degree() ) ) ;

////        if( this->velocity.length() > speedMax )
////            this->velocity.setLength( speedMax );

//        this->setPosition( this->getPosition() + this->getVelocity() ) ;
//        this->setVelocity( this->getVelocity() * this->getDecay() ) ;

       this->velocity += Vector2D::polar2vector( body.getActualSpeedProduct( * this , bodyCommand.dPower ) ,
                                                 AngleDeg::normalize_angle( bodyCommand.dAngle + body.getBodyAngle().degree() ) ) ;
       this->globalPosition += this->velocity ;
       this->velocity *= this->decay ;
   }
   else{

       this->globalPosition += this->velocity ;
       this->velocity *= this->decay ;
   }

   //    if( bodyCommand.ActionType == TACKLE )
//    {

//        bool  iFoul  = bodyCommand.bFoul;
//        double iAngle = bodyCommand.dAngle;

//        Vector2D player_2_ball = this->getPosition() - body.getPosition();
//        player_2_ball.rotate( -body.getBodyAngle() );

//        double tackle_dist = ( player_2_ball.x > 0.0
//                               ? TACKLE_DIST
//                               : TACKLE_BACK_DIST );

//        if ( std::fabs( tackle_dist ) <= 1.0e-5 )
//        {
//            body.dynamicsPlayerSimulate();
//            this->dynamicsBallSimulate();
//            continue;
//        }


//        double  exponent=TACKLE_EXPONENT;
//        /// bikhi foul exponent now

//        double prob = ( std::pow( std::fabs( player_2_ball.x ) / tackle_dist ,
//                                  exponent )
//                        + std::pow( std::fabs( player_2_ball.y ) / TACKLE_WIDTH ,
//                                    exponent ) );

//        if ( prob > 1.0 )
//        {
//            body.dynamicsPlayerSimulate();
//            this->dynamicsBallSimulate();
//            continue;
//        }





//        double angle =doubleNormalize( AngleDeg::normalize_angle( iAngle ) , MIN_TACKLE_MOMENT , MAX_TACKLE__MOMENT);
//        double eff_power = ( MAX_BACK_TACKLE_POWER + ( ( MAX_TACKLE_POWER - MAX_BACK_TACKLE_POWER ) * ( 1.0 - ( std::fabs( angle ) / M_PI ) ))) * TACKLE_POWER_RATE;


//        eff_power *= 1.0 - 0.5*( std::fabs( player_2_ball.dir().degree() ) / M_PI );

//        Vector2D accel = Vector2D::polar2vector( eff_power ,
//                                    angle + body.getBodyAngle() );


//        velocity += accel;
//        /*
//        if( velocity.length() > speedMax )
//                velocity.setLength( speedMax );
//        */
//    }

////    if(!ballCatched)
////    {
//        globalPosition+=velocity;
//        velocity*=decay;
////    }
////    else{
////                    //globalPosition=body.getPosition();
////                    velocity=Vector2D(0 , 0);
////    }




}
/// ////////////////////////////
////////////////////////////////
void Simul :: dynamicsPlayerSimulate(int cycle ){

   /**! medriki */
//    int         n 	= cycle;
//    double	a1	= this->getVelocity().length();
//    double 	q 	= this->getDecay();
//    double	qN	= pow(q , n);
//    double	S	= a1*(qN-1)/(q-1);

//    this->setPosition(this->getPosition()+Vector2D::polar2vector(S , this->getVelocity().dir().degree()));
//    this->setVelocity(this->getVelocity()*qN);

   this->globalPosition += Vector2D::polar2vector( this->velocity.length() * (pow( this->decay , cycle ) - 1 ) / ( this->decay - 1 ) , this->velocity.dir().degree() ) ;
   this->velocity *= pow( this->decay , cycle ) ;
}
/// ////////////////////////////
////////////////////////////////
void Simul :: dynamicsPlayerSimulate_1_cycle(){

//    this->setPosition(this->getPosition() + this->getVelocity() );
//    this->setVelocity(this->getVelocity()*this->getDecay());

   this->globalPosition += this->getVelocity() ;
   this->velocity *= decay ;
}
/// ////////////////////////////
////////////////////////////////
void Simul :: actionPlayerSimulate( SimulateAction & bodyCommand /* , int cycle*/ ){

   /**! medriki */
//    if( bodyCommand.actionType == DASH ){

//        Vector2D    vel = Vector2D( 0.0 , 0.0 ) ;

//        double	edp = 0 ;
//        double	ang = 0 ;

////        if(iPower<0 ){
////                iAngle=180;
////                iPower*=-1;
////        }

//        double dash_rate = this->getDashRate(bodyCommand.dAngle) ;

//        edp = this->getPT()->dashPowerRate() * bodyCommand.dPower * this->getEffort() * dash_rate ;
//        ang = AngleDeg::normalize_angle( getBodyAngle().degree() + iAngle ) ;
//        vel = Vector2D::polar2vector( edp , ang );


//        this->setPosition( this->getVelocity() + vel + this->getPosition() ) ;
//        this->setVelocity( ( this->getVelocity() + vel ) * this->getDecay() ) ;
//    }
//    else if( bodyCommand.actionType == TURN ){

//        double ang = this->maxTurnAng( bodyCommand.dAngle );

//        this->setBodyAngle( AngleDeg::normalize_angle( this->getBodyAngle().degree() + ang ) ) ;

//        this->setPosition( this->getVelocity() + this->getPosition() ) ;
//        this->setVelocity( this->getVelocity() * this->getDecay() ) ;
//    }
//    else if( bodyCommand.actionType == MOVE ){

//        this->setPosition( Vector2D( bodyCommand.dX , bodyCommand.dY ) );
//    }
//    else{

//        this->dynamicsPlayerSimulate_1_cycle();
//    }

   if( bodyCommand.actionType == DASH ){


       this->velocity += Vector2D::polar2vector( this->getPT()->dashPowerRate() * bodyCommand.dPower * this->getEffort() * getDashRate( bodyCommand.dAngle )
                                           , AngleDeg::normalize_angle( getBodyAngle().degree() + bodyCommand.dAngle ) ) ;
       this->globalPosition += this->velocity ;
       this->velocity *= this->decay ;
   }
   else if( bodyCommand.actionType == TURN ){

           this->setBodyAngle( AngleDeg::normalize_angle( bodyAngle.degree() + this->maxTurnAng( bodyCommand.dAngle ) ) ) ;
           this->globalPosition += this->velocity ;
           this->velocity *= this->decay ;
   }
   else{

       this->dynamicsPlayerSimulate_1_cycle() ;
   }
}
/// ////////////////////////////
////////////////////////////////
//double		Simul :: getDistanceTo(Simul & sim ){

//    return this->getPosition().dist(sim.getPosition());
//}
/// ////////////////////////////
////////////////////////////////
bool Simul :: isKickable( Simul & ball ) const {

   return ball.getPosition().dist( this->getPosition() ) < this->getPT()->kickableArea() ;
}
/// ////////////////////////////
////////////////////////////////
bool Simul :: isKickable( Vector2D & pos ) const {

   return this->getPosition().dist( pos ) < this->getPT()->kickableArea() ;
}
/// ////////////////////////////
////////////////////////////////
bool Simul :: isCollision( Simul & ball ) const {

   return ball.getPosition().dist( this->getPosition() ) <= ( this->getPT()->playerSize() + ball.getSize() + NOISE ) ;
}
/// ////////////////////////////
////////////////////////////////
double Simul :: getActualSpeedProduct( Simul & ball , double & power ){

   /**! medriki */
//    typeNormalize( power , -100.0 , +100.0 ) ;

//    double dir_diff = fabs( AngleDeg::normalize_angle( this->getBodyAngle().degree() - ( ball.getPosition() - this->getPosition() ).dir().degree() ) ) ;
//    double dist = this->getPosition().dist( ball.getPosition() ) - BALL_SIZE - this->getPT()->playerSize() ;

//    return power * KICK_POWER_RATE * ( 1 - (0.25 * ( dir_diff / 180.0 ) ) - ( 0.25 * ( dist / this->getPT()->kickableMargin() ) ) );

   typeNormalize( power , -100.0 , +100.0 ) ;

   return power * KICK_POWER_RATE * ( 1 -
                            ( 0.25 * ( fabs( AngleDeg::normalize_angle( this->bodyAngle.degree() - ( ball.globalPosition - this->globalPosition ).dir().degree() ) ) / 180.0 ) ) -
                            ( 0.25 * ( ( this->globalPosition.dist( ball.globalPosition ) - BALL_PLAYER_SIZE ) / this->getPT()->kickableMargin() ) ) );
}
/// ////////////////////////////
////////////////////////////////
double Simul :: getActualPower( Simul & ball , double & speed) const {

   /**! medriki */
//    double dir_diff = fabs( AngleDeg::normalize_angle( this->getBodyAngle().degree() - ( ball.getPosition() - this->getPosition() ).dir().degree() ) ) ;
//    double dist = this->getPosition().dist( ball.getPosition() ) - BALL_PLAYER_SIZE ;

//    double power = speed / ( KICK_POWER_RATE * ( 1.0 - ( 0.25 * ( dir_diff / 180.0 ) ) - ( 0.25 * ( dist / this->getPT()->kickableMargin() ) ) ) ) ;

//    if( power > 100.0 ){

//        power = 100.0 ;
//        speed = 100.0 * ( KICK_POWER_RATE * ( 1.0 - 0.25 * dir_diff / 180.0 - 0.25 * dist / this->getPT()->kickableMargin() ) ) ;
//    }

//    return power ;

   double power = speed / ( KICK_POWER_RATE * ( 1.0 -
                                                ( 0.25 * ( fabs( AngleDeg::normalize_angle( this->bodyAngle.degree() - ( ball.globalPosition - this->globalPosition ).dir().degree() ) ) / 180.0 ) ) -
                                                ( 0.25 * ( ( this->globalPosition.dist( ball.globalPosition ) - BALL_PLAYER_SIZE  ) / this->getPT()->kickableMargin() ) )
                                                ) ) ;

   if( power > 100.0 ){
       power = 100.0 ;
       speed = 100.0 * ( KICK_POWER_RATE * ( 1.0 -
                                             ( 0.25 * ( fabs( AngleDeg::normalize_angle( this->bodyAngle.degree() - ( ball.globalPosition - this->globalPosition ).dir().degree() ) ) / 180.0 ) ) -
                                             ( 0.25 * ( ( this->globalPosition.dist( ball.globalPosition ) - BALL_PLAYER_SIZE  ) / this->getPT()->kickableMargin() ) )
                                             ) ) ;
   }

   return power ;
}
/// ////////////////////////////
////////////////////////////////
double Simul :: getDashRate( double dir ){

   double dir_rate = 1.0;

   dir = fabs( DASH_ANGLE_STEP * rint( AngleDeg::normalize_angle( dir ) / DASH_ANGLE_STEP ) ) ;

   dir_rate = ( dir  > 90.0 ) ?
              BACK_DASH_RATE - ( ( BACK_DASH_RATE - SIDE_DASH_RATE ) * ( 1.0 - ( dir - 90.0 ) / 90.0 ) )
              : SIDE_DASH_RATE + ( ( 1.0 - SIDE_DASH_RATE )* ( 1.0 -  dir / 90.0 ) );

   typeNormalize(  dir_rate , 0.0 ,  1.0 );

   return dir_rate;
}
/// ////////////////////////////
////////////////////////////////
Vector2D Simul :: getActualVelocityAfterDash( double & power , double & dir ){

   /**! medriki */
//       typeNormalize(power , MIN_POWER , MAX_POWER );


////    if(iPower<0.0 ){
////         iAngle = 180.0;
////         iPower*=-1.0;
////    }

//    double dash_rate = getDashRate( dir ) ;
//    double edp = this->getPT()->dashPowerRate() * power * this->getEffort() * dash_rate ;

//    /*if( velocity.length() > speedMax )
//            velocity.setLength( speedMax ) ;
//    */
//    return this->getVelocity() + Vector2D::polar2vector( edp , AngleDeg::normalize_angle( this->getBodyAngle().degree() + dir ) ) ;

   typeNormalize(power , MIN_POWER , MAX_POWER );

   return this->velocity + Vector2D::polar2vector( ( this->PT->dashPowerRate() * power * this->effort *  this->getDashRate( dir )  ) ,
                                                   AngleDeg::normalize_angle( this->getBodyAngle().degree() + dir ) ) ;
}
/// ////////////////////////////
////////////////////////////////
double  Simul :: maxTurnAng( double iAng ){

//    typeNormalize( iAng , MIN_MOMENT , MAX_MOMENT ) ;
//    return iAng/(1.0 + this->getPT()->inertiaMoment() * this->getVelocity().length());

   typeNormalize( iAng , MIN_MOMENT , MAX_MOMENT ) ;
   return iAng / ( 1.0 + this->PT->inertiaMoment() * this->velocity.length() ) ;
}
/// ////////////////////////////
////////////////////////////////
double  Simul :: turnAng( double & iAng ){

   double turn = iAng * (1.0 + this->getPT()->inertiaMoment() * this->getVelocity().length() ) ;
   typeNormalize( turn , MIN_MOMENT , MAX_MOMENT ) ;
   return  turn;
}
////////////////////////////
/// ////////////////////////////////
Simul  Simul :: finalSim( double minSpeed ){

   double dis = ( this->getVelocity().length() - minSpeed ) / ( 1.0 - this->getDecay() ) ;

   if( dis < 0.0 ){
       return  * this;
   }

   Simul temp(PA);
   temp=*this;

   //    temp.setPosition( temp.getPosition() + Vector2D::polar2vector( dis , this->getVelocity().dir().degree() ) ) ;
//    temp.setVelocity( Vector2D::polar2vector( minSpeed , temp.getVelocity().dir() ) ) ;

   temp.globalPosition = this->globalPosition + Vector2D::polar2vector( dis , this->velocity.dir().degree() ) ;
   temp.velocity = Vector2D::polar2vector( minSpeed , this->velocity.dir() ) ;

   return temp;
}
/// ////////////////////////////
////////////////////////////////
//int     Simul :: turnTimeTo( double turnDir , double angTurn ){

//    int cycle = 0 ;

//    double  speed = this->getVelocity().length() ;

//    turnDir = fabs( AngleDeg::normalize_angle( turnDir ) ) ;

//    while( turnDir > angTurn ){

//        turnDir -= (180.0 / ( 1.0 + this->getPT()->inertiaMoment() * speed ) );
//        speed *= this->getDecay() ;
//        cycle ++ ;

//    }

//    return cycle;
//}
/// ////////////////////////////
////////////////////////////////
int     Simul :: turnTimeToPlusSim( Vector2D pos , double angTurn ){

   int cycle = 0 ;

   double  dir	= ( pos - this->getPosition() - this->getVelocity() ).dir().degree() ;
   double  ang = AngleDeg::normalize_angle( dir - this->getBodyAngle().degree() ) ;

   SimulateAction   soc = TURN ;

   while( fabs( ang ) > angTurn ){

       soc.dAngle = this->turnAng( ang ) ;

       this->actionPlayerSimulate( soc ) ;
       cycle ++ ;
       dir = ( pos - this->getPosition() - this->getVelocity() ).dir().degree();
       ang = AngleDeg::normalize_angle( dir-this->getBodyAngle().degree() ) ;

   }

   return cycle ;
}
/// ////////////////////////////
////////////////////////////////
int     Simul :: dashTimeTo ( Vector2D posEnd ){

   int cycle = 0 ;

   double dis=posEnd.dist(this->getPosition());
   double speed=this->getVelocity().length();
   double edp=this->getPT()->dashPowerRate()*100.0*this->getEffort();

   while(dis>this->PT->kickableMargin() ){
       speed+=edp;
       //(speed>temp.speedMax)?speed=temp.speedMax:1;
       dis-=speed;
       speed*=this->getDecay();
       cycle++;
   }

   return cycle;

}
/// ////////////////////////////
////////////////////////////////
int     Simul :: dashTime ( double dis /*, double dashAng */){

   int cycle = 0 ;


   double speed=this->getVelocity().length();
   double edp=this->getPT()->dashPowerRate()*100.0*this->getEffort()/**this->getDashRate(dashAng)*/;

   while(dis>this->PT->kickableMargin() ){
       speed+=edp;
       //(speed>temp.speedMax)?speed=temp.speedMax:1;
       dis-=speed;
       speed*=this->getDecay();
       cycle++;
   }

   return cycle;

}
/// ////////////////////////////
////////////////////////////////
int     Simul :: calculateBallTimeTo(double dis , double minSpeed ){

   Simul   temp(PA);
   temp=this->finalSim(minSpeed);
   double finalDist=temp.getPosition().dist(getPosition());
   if(finalDist<dis)
       return UNKNOWN_VALUE;

   double X=1.0-(dis*(MINES_ONE_BALL_DECAY)/getVelocity().length());

   return  ceil((log10(X))/(log10(BALL_DECAY)));

   /*double speedThr=(finalDist-dis)*(1-decay);
   double speed=velocity.length();

   return  ceil((log10(speed/speedThr))/(log10(decay)));*/


}
/// ////////////////////////////
////////////////////////////////
double Simul :: maxRand(Simul   ball , SimulateAction soc ){

   double dir_diff=fabs(AngleDeg::normalize_angle(this->getBodyAngle().degree()-(ball.getPosition()-this->getPosition()).dir().degree()));
   double dist=this->getPosition().dist(ball.getPosition())-BALL_SIZE-PLAYER_SIZE;

   double pos_rate = 0.5 + 0.25 * ( dir_diff/180.0 + dist/this->PT->kickableMargin() );    // [0.5 ,  1.0]

   double speed_rate = 0.5 + 0.5 * ( ball.getVelocity().length() / ( /*ball maxSpeed*/3.0 * /* ballDecay */ 0.94 ) );

   // [0 ,  2*kick_rand]
   return this->PT->kickRand() * ( soc.dPower/ /* maxPower*/ 100.0 ) * ( pos_rate + speed_rate );


}
/// ////////////////////////////
////////////////////////////////
Vector2D Simul :: maxKickNoise(Simul   ball , SimulateAction soc , double ang ){


   return  Vector2D::polar2vector( /*drand( 0.0 ,  max_rand )*/ maxRand(ball , soc) ,  /*drand( -M_PI ,  M_PI )*/ ang  );
}
/// ////////////////////////////
////////////////////////////////
double Simul :: maxRand(Vector2D posBall , double speed , SimulateAction soc ){

   double dir_diff=fabs(AngleDeg::normalize_angle(this->getBodyAngle().degree()-(posBall-this->getPosition()).dir().degree()));
   double dist=this->getPosition().dist(posBall) - 0.085 - this->getSize();

   double pos_rate = 0.5 + 0.25 * ( dir_diff/180.0 + dist/this->PT->kickableMargin() );    // [0.5 ,  1.0]

   double speed_rate = 0.5 + 0.5 * ( speed / ( /*ball maxSpeed*/3 * /* ballDecay */ 0.94 ) );

   // [0 ,  2*kick_rand]
   return this->PT->kickRand() * ( soc.dPower/ /* maxPower*/ 100.0 ) * ( pos_rate + speed_rate );


}
/// ////////////////////////////
////////////////////////////////
Vector2D Simul :: maxKickNoise(Vector2D posBall , double speed , SimulateAction soc , double ang ){


   return  Vector2D::polar2vector( /*drand( 0.0 ,  max_rand )*/ maxRand(posBall , speed , soc) ,  /*drand( -M_PI ,  M_PI )*/ ang  );
}
/// ////////////////////////////
////////////////////////////////
int	Simul :: timeTo(Vector2D pos ){

   if(this->getPosition().dist(pos)<this->getPT()->kickableArea())
       return 0;
   int time=this->turnTimeToPlusSim(pos , 5.0);

   time+=this->dashTimeTo(pos);
   return time;
}
///// ////////////////////////////
//////////////////////////////////
//int     Simul :: turnTime( Vector2D pos , double angTurn ){

//   int cycle = 0 ;

//   double  dir	= ( pos - this->getPosition() - this->getVelocity() ).dir().degree() ;
//   double  ang = AngleDeg::normalize_angle( dir - this->getBodyAngle().degree() ) ;

//   SimulateAction   soc = TURN ;

//   while( fabs( ang ) > angTurn ){

//       soc.dAngle = this->turnAng( ang ) ;

//       this->actionPlayerSimulate( soc ) ;
//       cycle ++ ;
//       dir = ( pos - this->getPosition() - this->getVelocity() ).dir().degree();
//       ang = AngleDeg::normalize_angle( dir-this->getBodyAngle().degree() ) ;

//   }

//   return cycle ;
//}
/// ////////////////////////////
////////////////////////////////
int	Simul :: reachTime(Vector2D pos ){

    if(this->getPosition().dist(pos)<this->getPT()->kickableArea())
       return 0;

    Simul   temp = * this ;

    int time = temp.turnTimeToPlusSim(pos , 5.0);

    time += temp.dashTimeTo(pos);

    return time;
}


/// ////////////////////////////
////////////////////////////////
SimulateAction Simul :: moveToPosSim( Vector2D posTo ,  double angWhenToTurn ,
                            double dDistBack ,  bool bMoveBack ,  int iCycles )
{
// previously we only turned relative to position in next cycle ,  now take
// angle relative to position when you're totally rolled out...
//  Vector2D posPred   = WMS->predictAgentPos( 1 ,  0 );
  Vector2D posAgent  = getPosition();
  Vector2D posPred   = finalSim(0.0).getPosition();

  double      angBody   = getBodyAngle().degree();
  double      angTo     = ( posTo - posPred ).dir().degree();
              angTo     = AngleDeg::normalize_angle( angTo - angBody );
  double      angBackTo = AngleDeg::normalize_angle( angTo + 180.0 );

  double      dDist     = posAgent.dist( posTo );

  if( bMoveBack )
  {
    if( fabs( angBackTo ) < angWhenToTurn )
      return dashToPointSim( posTo ,  iCycles );
    else
      return turnBackToPointSim( posTo );
  }
  else if(  fabs( angTo     ) < angWhenToTurn ||
           (fabs( angBackTo ) < angWhenToTurn && dDist < dDistBack ) )
    return dashToPointSim( posTo ,  iCycles );
  else
    return directTowardsSim( posTo ,  angWhenToTurn );
//return turnBodyToPointSim( posTo );
}
/// ////////////////////////////
////////////////////////////////
SimulateAction Simul :: dashToPointSim( Vector2D pos ,  int iCycles )
{
  double dDashPower = getPowerForDash(
                                 pos - getPosition() ,
                                 getBodyAngle().degree() ,
                                 getVelocity() ,
                                 getEffort() ,
                                 iCycles                              );
  return SimulateAction( DASH ,  dDashPower ,  0.0 );
}
/// ////////////////////////////
////////////////////////////////
SimulateAction Simul :: turnBackToPointSim( Vector2D pos ,  int iCycles )
{

  dynamicsPlayerSimulate(iCycles);
  Vector2D globalPositional = getPosition();
  double angTurn        = (pos - globalPositional).dir().degree();
  angTurn              -= (getBodyAngle().degree() + 180);
  angTurn               = AngleDeg::normalize_angle( angTurn );
  angTurn               = getAngleForTurn( angTurn ,  getVelocity().length()/pow(decay , iCycles));

  return SimulateAction( TURN ,  angTurn );
}
/// ////////////////////////////
////////////////////////////////
SimulateAction Simul :: directTowardsSim( Vector2D posTurnTo ,
   double angWhenToTurn ,  Vector2D *pos ,  Vector2D *vel ,  double *angBody  )
{
//  return turnBodyToPointSim( posTurnTo );
  // copy values or initialize when not set
  Vector2D posAgent= (pos    ==NULL)?getPosition():*pos;
  Vector2D velAgent= (vel    ==NULL)?getVelocity():*vel;
  double  angBodyAgent= (angBody==NULL)?getBodyAngle().degree():*angBody;

  // first predict what happens when the agents rolls out.
  Vector2D posPred    = finalSim(0.0).getPosition();
  double      angTo      = ( posTurnTo - posPred ).dir().degree();
  double      ang        = AngleDeg::normalize_angle( angTo - angBodyAgent );
  double      angNeck = 0;

  int iTurn = 0;
  while( fabs( ang ) > angWhenToTurn && iTurn < 5 )
  {
    iTurn++;
    SimulateAction   soc=TURN;
    soc.dAngle=getAngleForTurn( ang ,  velAgent.length() );

    posPred    = finalSim(0.0).getPosition();
    angTo      = ( posTurnTo - posPred ).dir().degree();
    ang        = AngleDeg::normalize_angle( angTo - angBodyAgent );
    angNeck = 0;


    ang = AngleDeg::normalize_angle( angTo - angBodyAgent );
  }
  posAgent = (pos    ==NULL)?getPosition():*pos;
  velAgent = (vel    ==NULL)?getVelocity():*vel;
  angBodyAgent = (angBody==NULL)?getBodyAngle().degree():*angBody;

  switch( iTurn )
  {
    case 0:   cerr << "direct towards: 0 turns" ;
              return SimulateAction( ILLEGAL );
    case 1:
    case 2:   return turnBodyToPointSim( posTurnTo ,  2 );
    default:  return dashToPointSim(
             (pos==NULL)?getPosition():*pos   );  // stop
  }
}
/// ////////////////////////////
////////////////////////////////
SimulateAction Simul :: turnBodyToPointSim( Vector2D pos ,  int iCycles )
{
  dynamicsPlayerSimulate(iCycles);
  Vector2D globalPositional = getPosition();
  double angTurn        = (pos - globalPositional).dir().degree();
  angTurn              -= getBodyAngle().degree();
  angTurn               = AngleDeg::normalize_angle( angTurn );
  angTurn               = getAngleForTurn( angTurn ,  getVelocity().length()/pow(decay , iCycles));

  return SimulateAction( TURN ,  angTurn );
}
/// ////////////////////////////
////////////////////////////////
double Simul :: getPowerForDash( Vector2D posRelTo ,  double angBody ,
              Vector2D vel ,  double dEffort ,  int iCycles )
{
  // the distance desired is the x-direction to the relative position we
  // we want to move to. If point lies far away ,  we dash maximal. Furthermore
  // we subtract the x contribution of the velocity because it is not necessary
  // to dash maximal.
  double dDist = posRelTo.rotate(-angBody).x; // get distance in direction
  if( iCycles <= 0 ) iCycles = 1;
  double dAcc  = dDist *  ( 1 - getDecay() )/( 1 - pow( getDecay() ,  iCycles ) ) ;
                                                   // get speed to travel now
//  if( dAcc > getSpeedMaxSimul() )             // if too far away
//    dAcc = getSpeedMaxSimul();                // set maximum speed
  dAcc -= vel.rotate(-angBody).x;             // subtract current velocity

  // acceleration = dash_power * dash_power_rate * effort ->
  // dash_power = acceleration / (dash_power_rate * effort )
  double dDashPower = dAcc/(getPT()->dashPowerRate() * dEffort );
  if( dDashPower > 100 )
    return 100;
  else if( dDashPower < 0.0 )
    return 0.0;
  else
    return dDashPower;
}

double Simul :: getAngleForTurn( double angDesiredAngle ,  double dSpeed)
{
  double a = angDesiredAngle * (1.0 + getPT()->inertiaMoment() * dSpeed );
  if( a > 180.0 )
    return 180.0 ;
  else if ( a < -180.0 )
    return -180.0 ;
  else
    return a;
}

