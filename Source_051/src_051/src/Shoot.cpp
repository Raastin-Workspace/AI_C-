#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include "Shoot.h"
#include "Pass.h"


#define OCC_ANG_TURN 45
#define ANG_TURN 3
#define BACK_DIS_DASH 5


using namespace std;
using namespace rcsc;

#define XFACTOR 0.98

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
int Shoot::getCount(const WorldModel & wm,double dir,double PM){

       return max(wm.dirCount(dir),max(wm.dirCount(AngleDeg::normalize_angle(dir+PM)),wm.dirCount(AngleDeg::normalize_angle(dir-PM))));
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
int Shoot::getCount(const WorldModel & wm,Vector2D pos,double PM){

       double dir=(pos-wm.self().pos()).dir().degree();
       return max(wm.dirCount(dir),max(wm.dirCount(AngleDeg::normalize_angle(dir+PM)),wm.dirCount(AngleDeg::normalize_angle(dir-PM))));
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
double  Shoot::calculate_PM(const WorldModel & wm,Vector2D & start,Vector2D & end,double & speed,double minSpeed){

    double dis=start.dist(end);

    if(dis>(speed-minSpeed)/MINES_ONE_BALL_DECAY)
        return 100.0;

    double X=1-(dis*(MINES_ONE_BALL_DECAY)/speed);

    int time =ceil((log10(X))/(log10(BALL_DECAY)));

    dis=Simul().Sn(0.0,0.4,0.6,max(time-2,0));

    Vector2D pos=end+Vector2D::polar2vector(dis,AngleDeg::normalize_angle((end-start).dir().degree()+90.0));

    return doubleNormalize(getAngDiff((pos-start).dir().degree(),(end-start).dir().degree()),5.0,15);
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
double Shoot::getFirstEndOneStep(const PlayerAgent * agent,double speed ){

    const WorldModel & wm = agent->world();

//    vector <double> dis;

    double dir_diff=fabs(AngleDeg::normalize_angle(wm.self().body().degree()-(wm.ball().pos()-wm.self().pos()).dir().degree()));
    double dist=(wm.ball().pos()-wm.self().pos()).r()- BALL_SIZE-PLAYER_SIZE;

    double pos_rate = 0.5 + 0.25 * ( dir_diff/180.0 + dist/ wm.self().playerType().kickableMargin() );    // [0.5 ,  1.0]

    double speed_rate = 0.5 + 0.5 * ( wm.ball().vel().length() / ( /*ball maxSpeed*/3.0 * /* ballDecay */ BALL_DECAY ) );

//    dis.push_back(wm.self().playerType().kickRand() * ( pos_rate + speed_rate )  );
    return (wm.self().playerType().kickRand() * ( pos_rate + speed_rate ) * 0.5 / (speed));
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
double Shoot::getFirstEndTwoStep(const PlayerAgent * agent){

    const WorldModel & wm = agent->world();

    return wm.self().playerType().kickRand()  *0.55;
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
bool    Shoot::execute(PlayerAgent *agent){


    const WorldModel & wm = agent->world();

    if(wm.self().pos().dist(Vector2D(52.5,0.0))>25)
        return false;
//    if(wm.theirPlayer(wm.theirGoalieUnum()))
//        cout<<"goalieCount : "<<wm.theirPlayer(wm.theirGoalieUnum())->posCount()<<endl;
//    else
//        cout<<"no such devises"<<endl;

    Vector2D ballPos = wm.ball().pos(),
             selfPos = wm.self().pos(),
             leftGoal = Vector2D(52.5,-7.1),
             rightGoal = Vector2D(52.5,7.1),
             centerGoal = Vector2D(52.5,0.0);


    Line2D  goalLine(centerGoal, 90.0 );

    double  oneStepNoise = getFirstEndOneStep(agent);
    double  dirNoise = atanDeg(oneStepNoise);

//    cout<<"oneStepNois and dir :"<<oneStepNoise<<"  "<<dirNoise<<endl;

//    cout<<"player Rand : "<<wm.self().playerType().kickRand()<<endl;
//    double  firstDir = (ballPos-Vector2D(52.5, -7.0 +(oneStepNoise*ballPos.dist(Vector2D(52.5,-7.0))))).dir().degree(),
//            endDir = (ballPos-Vector2D(52.5, 7.0 - (oneStepNoise*ballPos.dist(Vector2D(52.5,7.0))))).dir().degree(),
//            dirPart = fabs(AngleDeg::normalize_angle(endDir - firstDir))/30;


    double  dirPart = fabs(AngleDeg::normalize_angle((rightGoal-ballPos).dir().degree() - (leftGoal-ballPos).dir().degree()))/30;

//    cout<<"dirPart : "<<dirPart<<endl;

    vector  <Vector2D> positions;
    positions = getDir(agent,leftGoal,rightGoal,10);

//    cout<<"parts : "<<positions.size()-1<<endl;
    struct  stc{
        Vector2D start;
        Vector2D end;

    }xName[positions.size() -1];

//    vector <struct stc> positions;

    xName[0].start = goalLine.intersection(Line2D(ballPos,(leftGoal-ballPos).dir().degree()+dirNoise));
    xName[positions.size() -2].end = goalLine.intersection(Line2D(ballPos,(rightGoal-ballPos).dir().degree()-dirNoise));


    Line2D temp = goalLine;
    double dir1 = 0,
           dir2 = 0;

    for (int i = 0; i < positions.size() - 2 ; ++i) {

        dir1=(positions.at(i+1)-ballPos).dir().degree();
        dir2=atanDeg(1.05/(positions.at(i+1)-ballPos).r());

        temp = Line2D(ballPos,dir1-dirNoise-dir2);
        xName[i].end = goalLine.intersection(temp);

        temp = Line2D(ballPos,dir1+dirNoise+dir2);
        xName[i+1].start = goalLine.intersection(temp);

    }


    struct  stc2{
        double start;
        double end;

    }xName2[positions.size() -1];


    for (int i = 0; i < positions.size()-1; ++i) {

        xName2[i].start = (xName[i].start-ballPos).dir().degree();
        xName2[i].end = (xName[i].end-ballPos).dir().degree();
    }

    vector < Vector2D > ends;

    for (int i = 0; i < positions.size()-1 ; ++i) {

        double dir = xName2[i].start;

        for (int j = 0; dir <= xName2[i].end ; dir += dirPart) {
            ends.push_back(goalLine.intersection(Line2D(ballPos,dir)));

        }
    }

    for (int i = 0; i < ends.size(); ++i) {

        if(fabs(ends.at(i).y) > 7.0)
            continue;


        double minSpeed=1.0+(ballPos.dist(ends.at(i))*0.06);

        Body_SmartKick smart(ends.at(i),3,minSpeed,1);

        if(!smart.execute(agent) )
            continue;

        double speed = smart.sequence().speed_;

        if(!isInGoal(ballPos,ends.at(i),smart.sequence().speed_,(oneStepNoise*3)/speed,7.1))
            continue;

        int conf = getCount(wm,ends.at(i),5/*calculate_PM(wm,selfPos,ends.at(i),speed)*/);

        if(conf>3 )
            continue;

//        double rand = kickRand(agent,smart);
//        if(!isInGoal((startKick(wm,smart)),(end),smart.sequence().speed_,rand,7.0))
//            continue;
MostafPass *m;
        if(OCC(agent,startKick(wm,smart),ends.at(i),smart.sequence().speed_,10,0.3,stepKick(smart),oneStepNoise/**speed*/,false) &&
                ( OCI(agent,startKick(wm,smart),ends.at(i),smart.sequence().speed_,10,0.3,stepKick(smart),oneStepNoise/**speed*/,false,0,0) ||
                  m->OCI(agent,toVecPos(startKick(wm,smart)),toVecPos(ends.at(i)),smart.sequence().speed_,10,0.3,stepKick(smart),oneStepNoise/**speed*/,false,0))){
//            cout<<wm.time().cycle()<<"  end : "<<ends.at(i)<<"  speed  "<<smart.sequence().speed_<<endl;
//            cout<<wm.time().cycle()<<" start: "<<startKick(wm,smart)<<"  step  "<<stepKick(smart)<<endl;
            return true;
        }

    }

    return false;

}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
bool    Shoot::execute23(PlayerAgent *agent,NewKick &kick){


    const WorldModel & wm = agent->world();

    if(wm.self().pos().dist(Vector2D(52.5,0.0))>25)
        return false;
//    if(wm.theirPlayer(wm.theirGoalieUnum()))
//        cout<<"goalieCount : "<<wm.theirPlayer(wm.theirGoalieUnum())->posCount()<<endl;
//    else
//        cout<<"no such devises"<<endl;

    Vector2D ballPos = wm.ball().pos(),
             selfPos = wm.self().pos(),
             leftGoal = Vector2D(52.5,-7.1),
             rightGoal = Vector2D(52.5,7.1),
             centerGoal = Vector2D(52.5,0.0);


    Line2D  goalLine(centerGoal, 90.0 );

    double  MultiStepNoise = getFirstEndTwoStep(agent);
    double  dirNoise = atanDeg(MultiStepNoise);

    //cout<<"MultiStepNois and dir :"<<MultiStepNoise<<"  "<<dirNoise<<endl;
    dirNoise = 0.0;
//    cout<<"player Rand : "<<wm.self().playerType().kickRand()<<endl;
//    double  firstDir = (ballPos-Vector2D(52.5, -7.0 +(MultiStepNoise*ballPos.dist(Vector2D(52.5,-7.0))))).dir().degree(),
//            endDir = (ballPos-Vector2D(52.5, 7.0 - (MultiStepNoise*ballPos.dist(Vector2D(52.5,7.0))))).dir().degree(),
//            dirPart = fabs(AngleDeg::normalize_angle(endDir - firstDir))/30;


    double  dirPart = fabs(AngleDeg::normalize_angle((rightGoal-ballPos).dir().degree() - (leftGoal-ballPos).dir().degree()))/30;

//    cout<<"dirPart : "<<dirPart<<endl;

    vector  <Vector2D> positions;
    positions = getDir(agent,leftGoal,rightGoal,10);

//    cout<<"parts : "<<positions.size()-1<<endl;
    struct  stc{
        Vector2D start;
        Vector2D end;

    }xName[positions.size() -1];

//    vector <struct stc> positions;

    xName[0].start = goalLine.intersection(Line2D(ballPos,(leftGoal-ballPos).dir().degree()+dirNoise));
    xName[positions.size() -2].end = goalLine.intersection(Line2D(ballPos,(rightGoal-ballPos).dir().degree()-dirNoise));


    Line2D temp = goalLine;
    double dir1 = 0,
           dir2 = 0;

    for (int i = 0; i < positions.size() - 2 ; ++i) {

        dir1=(positions.at(i+1)-ballPos).dir().degree();
        dir2=atanDeg(1.05/(positions.at(i+1)-ballPos).r());

        temp = Line2D(ballPos,dir1-dirNoise-dir2);
        xName[i].end = goalLine.intersection(temp);

        temp = Line2D(ballPos,dir1+dirNoise+dir2);
        xName[i+1].start = goalLine.intersection(temp);

    }


    struct  stc2{
        double start;
        double end;

    }xName2[positions.size() -1];


    for (int i = 0; i < positions.size()-1; ++i) {

        xName2[i].start = (xName[i].start-ballPos).dir().degree();
        xName2[i].end = (xName[i].end-ballPos).dir().degree();
    }

    vector < Vector2D > ends;

    for (int i = 0; i < positions.size()-1 ; ++i) {

        double dir = xName2[i].start;

        for (int j = 0; dir <= xName2[i].end ; dir += dirPart) {
            ends.push_back(goalLine.intersection(Line2D(ballPos,dir)));

        }
    }

    for (int j = 2; j < 4; ++j) {

        int step = j;
        for (int i = 0; i < ends.size(); ++i) {

            if(fabs(ends.at(i).y) > 7.0)
                continue;

            double minSpeed=1.0+(ballPos.dist(ends.at(i))*0.06);

            Body_SmartKick smart(ends.at(i),3.0,minSpeed,step);

            if(!smart.execute(agent) )
                continue;

//            if(step != stepKick(smart)){
//                cout<<step<<" step , kickstep  "<<stepKick(smart)<<endl;
//                continue;

//            }

//            if(!isInGoal(firstKickPos(wm,smart),ends.at(i),smart.sequence().speed_,MultiStepNoise*3,7.0))
//                continue;


            double speed = smart.sequence().speed_;
            int conf = getCount(wm,ends.at(i),/*calculate_PM(wm,selfPos,ends.at(i),speed)*/5);

            if(conf>3 )
                continue;

    //        double rand = kickRand(agent,smart);
    //        if(!isInGoal((startKick(wm,smart)),(end),smart.sequence().speed_,rand,7.0))
    //            continue;
        MostafPass *m;

            if(OCC(agent,startKick(wm,smart),ends.at(i),smart.sequence().speed_,10,0.3,stepKick(smart),MultiStepNoise/**smart.sequence().speed_*/,/*true*/false) &&
                    ( OCI(agent,startKick(wm,smart),ends.at(i),smart.sequence().speed_,10,0.3,stepKick(smart),MultiStepNoise/**smart.sequence().speed_*/,false,0,0) ||
                      m->OCI(agent,toVecPos(startKick(wm,smart)),toVecPos(ends.at(i)),smart.sequence().speed_,10,0.3,stepKick(smart),MultiStepNoise/**smart.sequence().speed_*/,false,0))){
                //cout<<wm.time().cycle()<<"  end : "<<ends.at(i)<<"  speed  "<<smart.sequence().speed_<<endl;
                //cout<<wm.time().cycle()<<" start: "<<startKick(wm,smart)<<"  step  "<<stepKick(smart)<<endl;
                setKick(wm,kick,smart);
                kick.reciverUnum = 666;
                return true;
            }

        }

    }

    return false;






}




bool Shoot::OCC(PlayerAgent * agent,Vector2D start,Vector2D end,double speed,int minConf,double minSpeed,int step,double rand,bool withRand){

    const WorldModel & wm = agent->world();

    Simul   opp;
    double  edp;
    double  edpBack;
    int     conf;





    if(wm.theirPlayer(wm.theirGoalieUnum()) && wm.theirPlayer(wm.theirGoalieUnum())->posCount() < minConf){

        opp=Simul(agent,SIM_OPPONENT,wm.theirGoalieUnum());

        edp = 100 * opp.getPT()->dashPowerRate() * opp.getEffort(); //*dash_rate
        edpBack= edp*0.95 ;
        conf = wm.theirPlayer(wm.theirGoalieUnum())->posCount();

    }

    else{
        cout<<wm.time().cycle()<<"  cheshBaste"<<endl;
           return true;

    }


    double dir=(end-start).dir().degree();
    Vector2D vel=Vector2D::polar2vector(speed,dir);

    int config = max(0,getCount(wm,(end-(wm.self().pos())).dir().degree(),5));



    Vector2D RNoiseVel=vel,LNoiseVel=vel,BNoiseVel=vel;
    if(withRand){
        RNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir+90));
        LNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir-90));
        BNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir+180));
    }

    if(withRand==false){
        Line2D    line=Line2D(start,end);

        Vector2D temp1,temp2;
        Circle2D  cir=Circle2D(opp.getPosition()+opp.getVelocity(),1.3/*opp.getPT().dCatchableAreaL*/);
        if(cir.intersection(line,&temp1,&temp2)==false)
            ;
        else if(temp1.isBetweenXY(start/*+vel*/,end) || temp2.isBetweenXY(start/*+vel*/,end))
            return false;

        //Vector2D posOnLine=line.projection(opp.getPosition()+opp.getVelocity());

        //if(line.getDistanceWithPoint(opp.getPosition()+opp.getVelocity())/*-(conf/2)*/<1.3/*opp.getPT().dCatchableAreaL*/ && posOnLine.isBetweenXY(start+vel,end) )
            //return false;



    }
    else{
        Line2D    line=Line2D(start+BNoiseVel-vel,end);

        Vector2D temp1,temp2;
        Circle2D  cir=Circle2D(opp.getPosition()+opp.getVelocity(),1.3/*opp.getPT().dCatchableAreaL*/);
        if(cir.intersection(line,&temp1,&temp2)==false)
            ;
        else if(temp1.isBetweenXY(start+BNoiseVel-vel,end) || temp2.isBetweenXY(start+BNoiseVel-vel,end))
            return false;

        //Vector2D posOnLine=line.projection(opp.getPosition()+opp.getVelocity());

        //if(line.getDistanceWithPoint(opp.getPosition()+opp.getVelocity())/*-(conf/2)*/<1.3/*opp.getPT().dCatchableAreaL*/ && posOnLine.isBetweenXY(start+BNoiseVel,end) ){
            //return false;

        //}
         line=Line2D(start+RNoiseVel-vel,end);
        Vector2D REnd=start+RNoiseVel-vel+Vector2D::polar2vector(start.dist(end),RNoiseVel.dir().degree());
        line=Line2D(start,REnd);

        if(cir.intersection(line,&temp1,&temp2)==false)
            ;
        else if(temp1.isBetweenXY(start+RNoiseVel-vel,end) || temp2.isBetweenXY(start+RNoiseVel-vel,end))
            return false;
        //posOnLine=line.projection(opp.getPosition()+opp.getVelocity());

        //if(line.getDistanceWithPoint(opp.getPosition()+opp.getVelocity())/*-(conf/2)*/<1.3/*opp.getPT().dCatchableAreaL*/ && posOnLine.isBetweenXY(start+RNoiseVel,end) ){
            //return false;

        //}

         line=Line2D(start+LNoiseVel-vel,end);
        Vector2D LEnd=start+LNoiseVel-vel+Vector2D::polar2vector(start.dist(end),LNoiseVel.dir().degree());
        line=Line2D(start,LEnd);

        if(cir.intersection(line,&temp1,&temp2)==false)
            ;
        else if(temp1.isBetweenXY(start+LNoiseVel-vel,end) || temp2.isBetweenXY(start+LNoiseVel-vel,end))
            return false;
        //posOnLine=line.projection(opp.getPosition()+opp.getVelocity());

        //if(line.getDistanceWithPoint(opp.getPosition()+opp.getVelocity())/*-(conf/2)*/<1.3/*opp.getPT().dCatchableAreaL*/ && posOnLine.isBetweenXY(start+LNoiseVel,end) ){
            //return false;

        //}


    }
    vel *= BALL_DECAY;
    BNoiseVel *= BALL_DECAY;
    RNoiseVel *= BALL_DECAY;
    LNoiseVel *= BALL_DECAY;
    int xStart = 0;
    int ExtTime = 1;
    if(withRand==false){


        Simul   ball(agent,SIM_BALL);

        ball.setVelocity(vel);

        Vector2D posBall=start;
        ball.setPosition(posBall);
        int time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

        if(time==UNKNOWN_VALUE)
            return false;

//        cout<<wm.time().cycle()<<"  time toGoal "<<time<<endl;
        Simul   temp(agent);





        for(int i= xStart ; i<=time + ExtTime ; i++){



            temp = opp;


            int turnTime=temp.turnTimeToPlusSim(posBall,OCC_ANG_TURN);


            double dis;
            if(i-turnTime/*+config*/+conf<=0)
                dis=0;
            else
                dis=temp.Sn2(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime  /*+config*/+conf);

            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }


            temp = opp;


            Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);


            turnTime=temp.turnTimeToPlusSim(posBack,OCC_ANG_TURN);



            if(i-turnTime/*+config/**/+conf<=0)
                dis=0;
            else
                dis=temp.Sn(temp.getVelocity().r()+edp,temp.getDecay(),edpBack,i-turnTime/*+config*/+conf);

            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }

            vel*=BALL_DECAY;
            posBall+=vel;

        }
    }
    else{
        //////////                    back noise


        Simul   ball(agent,SIM_BALL);

        ball.setVelocity(BNoiseVel);

        Vector2D posBall=start+BNoiseVel-vel;
        ball.setPosition(posBall);
        int time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

        if(time==UNKNOWN_VALUE)
            return false;

        Simul   temp(agent);
        for(int i= xStart ; i<=time + ExtTime ; i++){


            temp = opp;


            int turnTime=temp.turnTimeToPlusSim(posBall,OCC_ANG_TURN);


            double dis;
            if(i-turnTime/*+config*/+conf<=0)
                dis=0;
            else
                dis=temp.Sn2(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime+ 1.0/*+config*/+conf);

            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }


            temp = opp;


            Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);


            turnTime=temp.turnTimeToPlusSim(posBack,OCC_ANG_TURN);



            if(i-turnTime/*+config*/+conf<=0)
                dis=0;
            else
                dis=temp.Sn(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime/*+config*/+conf);

            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }

            BNoiseVel*=BALL_DECAY;
            posBall+=BNoiseVel;

        }
        //////////                    right noise


        ball.setVelocity(RNoiseVel);

        posBall=start+RNoiseVel-vel;
        ball.setPosition(posBall);

        time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

        if(time==UNKNOWN_VALUE)
            return false;

//        Simul   temp(agent);
        for(int i= xStart ; i<=time + ExtTime ; i++){


            temp = opp;


            int turnTime=temp.turnTimeToPlusSim(posBall,OCC_ANG_TURN);


            double dis;
            if(i-turnTime/*+config*/+conf<=0)
                dis=0;
            else
                dis=temp.Sn(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime+ 1.0/*+config*/+conf);

            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }


            temp = opp;


            Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);


            turnTime=temp.turnTimeToPlusSim(posBack,OCC_ANG_TURN);



            if(i-turnTime/*+config*/+conf<=0)
                dis=0;
            else
                dis=temp.Sn(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime/*+config*/+conf);

            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }

            RNoiseVel*=BALL_DECAY;
            posBall+=RNoiseVel;

        }
        //////////                    left noise


        ball.setVelocity(LNoiseVel);

        posBall=start+LNoiseVel-vel;
        ball.setPosition(posBall);

        time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

        if(time==UNKNOWN_VALUE)
            return false;

//        Simul   temp(agent);
        for(int i= xStart ; i<=time + ExtTime ; i++){


            temp = opp;


            int turnTime=temp.turnTimeToPlusSim(posBall,OCC_ANG_TURN);


            double dis;
            if(i-turnTime/*+config*/+conf<=0)
                dis=0;
            else
                dis=temp.Sn(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime+ 1.0/*+config*/+conf);

            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }


            temp = opp;


            Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);


            turnTime=temp.turnTimeToPlusSim(posBack,OCC_ANG_TURN);



            if(i-turnTime/*+config*/+conf<=0)
                dis=0;
            else
                dis=temp.Sn(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime/*+config*/+conf);

            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }

            LNoiseVel*=BALL_DECAY;
            posBall+=LNoiseVel;

        }
    }
    return true;
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
double Shoot::oneStepRand(PlayerAgent * agent,double dPower){

    const WorldModel & wm = agent->world();

    double dir_diff=fabs(AngleDeg::normalize_angle(wm.self().body().degree()-(wm.ball().pos()-wm.self().pos()).dir().degree()));
    double dist=wm.self().pos().dist(wm.ball().pos())-0.385;

    double pos_rate = 0.5 + 0.25 * ( dir_diff/180 + dist/wm.self().playerType().kickableMargin() );    // [0.5, 1.0]

    double speed_rate = 0.5 + 0.5 * ( wm.ball().vel().length() / ( /*ball maxSpeed*/3 * /* ballDecay */ BALL_DECAY ) );

    // [0, 2*kick_rand]
    return wm.self().playerType().kickRand() * ( dPower/ /* maxPower*/ 100 ) * ( pos_rate + speed_rate );


}










































////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
bool Shoot::OCI(PlayerAgent * agent,Vector2D start,Vector2D end,double speed,int minConf,double minSpeed,int step,double rand,bool withRand,int Ext,int moveExt){

    const WorldModel & wm = agent->world();

    struct stc{

        Simul   opp;
        double  edp;
        double  edpBack;
        int     conf;
        int     bodyConf;
        double  kickableArea;
    }info[11];

    Simul temp(agent);

    Vector2D    selfPos=wm.self().pos();
    int nr=0;

//    int confing = max(1,getCount(wm,(end-(wm.self().pos())).dir().degree(),0))+Ext-1;
    minConf = getCount(wm,end,calculate_PM(wm,selfPos,end,speed))+Ext+moveExt;


    for(int i=1;i<12;i++){
        if(wm.theirPlayer(i)/* && wm.theirGoalieUnum()!= i */&& wm.theirPlayer(i)->seenPosCount() <= minConf){
//            info[nr].opp=temp;
            info[nr].opp = Simul(agent,SIM_OPPONENT,i);
            info[nr].edp=100*info[nr].opp.getPT()->dashPowerRate()*info[nr].opp.getEffort();//*dash_rate
            info[nr].edpBack=info[nr].edp*0.6/*100*info[nr].opp.getPT()->dashPowerRate()*info[nr].opp.getEffort()*info[nr].opp.getDashRate(180)*/;

            info[nr].conf=wm.theirPlayer(i)->posCount();
            info[nr].bodyConf=wm.theirPlayer(i)->bodyCount();
            info[nr].kickableArea=info[nr].opp.getPT()->kickableArea();

            nr++;
            //cout<<"add on player : "<<i<<"Object : "<<(ObjectT)(OBJECT_OPPONENT_1+(i-1))<<endl;

        }
    }

    if(nr==0){

        return true;


    }

    double dir=(end-start).dir().degree();
    Vector2D vel=Vector2D::polar2vector(speed,dir);

    Vector2D RNoiseVel=vel,LNoiseVel=vel,BNoiseVel=vel;
    if(withRand){
        RNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir+90));
        LNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir-90));
        BNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir+180));
    }

    if(withRand==false){
        Line2D    line=Line2D(start,end);

        for(int j=0; j<nr ; j++){

            Vector2D posOnLine=line.projection(info[j].opp.getPosition());
            temp = info[j].opp;
            for(int k=1;k<=info[j].conf;k++)
                temp.moveToPosSim(posOnLine,5);





            Vector2D temp1,temp2;
            Circle2D  cir=Circle2D(temp.getPosition()/*+temp.getVelocity()*/,info[j].kickableArea);
            if(cir.intersection(line,&temp1,&temp2)==0)
                continue;
            else if(temp1.isBetweenXY(start/*+vel*/,end) || temp2.isBetweenXY(start/*+vel*/,end))
                return false;


            temp1,temp2;
            cir=Circle2D(temp.getPosition(),info[j].kickableArea);
            if(cir.intersection(line,&temp1,&temp2)==0)
                continue;
            else if(temp1.isBetweenXY(start/*+vel*/,end) || temp2.isBetweenXY(start/*+vel*/,end))
                return false;
            //Vector2D posOnLine=line.projection(temp.getPosition()+temp.getVelocity());
            //if(line.getDistanceWithPoint(temp.getPosition()+temp.getVelocity())/*-(conf/2)*/<info[j].kickableArea&& posOnLine.isBetweenXY(start+vel,end) )
                //return false;

        }
    }
//    else{

//        Line2D    line=Line2D(start,end);
//        Vector2D posOnLine;
//        Vector2D temp1,temp2;
//        Circle2D  cir;

//        for(int j=0; j<nr ; j++){


//            Vector2D BEnd=start+Vector2D::polar2vector(start.dist(end)*1.05,BNoiseVel.dir().degree());

//            line=Line2D(start,BEnd);

//            posOnLine=line.projection(info[j].opp.getPosition());

//            temp = info[j].opp;
//            for(int k=1;k<=info[j].conf;k++)
//                temp.moveToPosSim(posOnLine,5);

//            cir=Circle2D(temp.getPosition()+temp.getVelocity(),info[j].kickableArea);

//            if((cir.intersection(line,&temp1,&temp2)==true) &&(temp1.isBetweenXY(start+BNoiseVel/*-vel*/,end) || temp2.isBetweenXY(start+BNoiseVel/*-vel*/,BEnd)))
//                return false;

//            cir=Circle2D(temp.getPosition(),info[j].kickableArea);

//            if((cir.intersection(line,&temp1,&temp2)==true) &&(temp1.isBetweenXY(start+BNoiseVel/*-vel*/,end) || temp2.isBetweenXY(start+BNoiseVel/*-vel*/,BEnd)))
//                return false;

//            //Vector2D posOnLine=line.projection(temp.getPosition()+temp.getVelocity());
//            //if(line.getDistanceWithPoint(temp.getPosition()+temp.getVelocity())/*-(conf/2)*/<info[j].kickableArea && posOnLine.isBetweenXY(start+BNoiseVel,end) )
//                //return false;

//            Vector2D REnd=start+Vector2D::polar2vector(start.dist(end)*1.05,RNoiseVel.dir().degree());

//            line=Line2D(start,REnd);

//            posOnLine=line.projection(info[j].opp.getPosition());
//            temp = info[j].opp;
//            for(int k=1;k<=info[j].conf;k++)
//                temp.moveToPosSim(posOnLine,5);

//            cir=Circle2D(temp.getPosition()+temp.getVelocity(),info[j].kickableArea);

//            if((cir.intersection(line,&temp1,&temp2)==true) &&(temp1.isBetweenXY(start+RNoiseVel/*-vel*/,REnd) || temp2.isBetweenXY(start+RNoiseVel/*-vel*/,REnd)))
//                return false;

//            cir=Circle2D(temp.getPosition(),info[j].kickableArea);

//            if((cir.intersection(line,&temp1,&temp2)==true) &&(temp1.isBetweenXY(start+RNoiseVel/*-vel*/,REnd) || temp2.isBetweenXY(start+RNoiseVel/*-vel*/,REnd)))
//                return false;

//            //Vector2D posOnLine=line.projection(temp.getPosition()+temp.getVelocity());
//            //if(line.getDistanceWithPoint(temp.getPosition()+temp.getVelocity())/*-(conf/2)*/< info[j].kickableArea&& posOnLine.isBetweenXY(start+RNoiseVel,REnd) )
//                //return false;

//            Vector2D LEnd=start+Vector2D::polar2vector(start.dist(end)*1.05,LNoiseVel.dir().degree());

//            line=Line2D(start,LEnd);

//            posOnLine=line.projection(info[j].opp.getPosition());

//            temp = info[j].opp;
//            for(int k=1;k<=info[j].conf;k++)
//                temp.moveToPosSim(posOnLine,5);

//            cir=Circle2D(temp.getPosition()+temp.getVelocity(),info[j].kickableArea);

//            if((cir.intersection(line,&temp1,&temp2)==true) &&(temp1.isBetweenXY(start+LNoiseVel/*-vel*/,LEnd) || temp2.isBetweenXY(start+LNoiseVel/*-vel*/,LEnd)))
//                return false;

//            cir=Circle2D(temp.getPosition()+temp.getVelocity(),info[j].kickableArea);

//            if((cir.intersection(line,&temp1,&temp2)==true) &&(temp1.isBetweenXY(start+LNoiseVel/*-vel*/,LEnd) || temp2.isBetweenXY(start+LNoiseVel/*-vel*/,LEnd)))
//                return false;

//            //Vector2D posOnLine=line.projection(temp.getPosition()+temp.getVelocity());
//            //if(line.getDistanceWithPoint(temp.getPosition()+temp.getVelocity())/*-(conf/2)*/<info[j].kickableArea && posOnLine.isBetweenXY(start+BNoiseVel,LEnd) )
//                //return false;
//        }

//    }




    vel *= BALL_DECAY;
    BNoiseVel *= BALL_DECAY;
    RNoiseVel *= BALL_DECAY;
    LNoiseVel *= BALL_DECAY;
    int xStart = 0;
    int ExtTime = moveExt;

    if(start.x<-20){
        xStart += step;
        ExtTime += step;
    }


    if(withRand==false){
        Simul   ball(agent,SIM_BALL);

        ball.setVelocity(vel);
        ball.setPosition(start);
        Vector2D posBall=start;//+vel;

        int time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

        if(time==UNKNOWN_VALUE)
            return false;

        for(int i=xStart ; i<=time+ExtTime ; i++){


            for(int j=0; j<nr ; j++){

                temp = info[j].opp;

                int turnTime = 0;
//                if(info[j].bodyConf <= 1)
                    temp.turnTimeToPlusSim(posBall,ANG_TURN);


                double dis;

                if(i-turnTime+/*confing+*/info[j].conf<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,i-turnTime/*+confing*/+info[j].conf);

                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                    return false;
                }

            }

            for(int j=0; j<nr ; j++)
                if(posBall.dist(info[j].opp.getPosition())<BACK_DIS_DASH){

                temp = info[j].opp;
                Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);

                int turnTime = 0;
//                if(info[j].bodyConf==0)
                    temp.turnTimeToPlusSim(posBack,ANG_TURN);



                double dis;
                if(i-turnTime+/*confing+*/info[j].conf<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edpBack,min(i-turnTime+/*confing+*/info[j].conf,3));

                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                    return false;
                }

            }
            vel*=BALL_DECAY;
            posBall+=vel;


            if(i==time + ExtTime)
                for(i=time+1;i<=time+ ExtTime + Ext;i++){

                    for(int j=0; j<nr ; j++){

                        temp = info[j].opp;

                        int turnTime = 0;
//                        if(info[j].bodyConf==0)
                            temp.turnTimeToPlusSim(posBall,ANG_TURN);



                        double dis;
                        if(i-turnTime+/*confing+*/info[j].conf<=0)
                            dis=0;
                        else
                            dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,i-turnTime+/*confing+*/info[j].conf);

                        if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                            return false;
                        }

                    }



                }


        }
    }
//    else{
//        //////////                    back noise
//        Simul   ball(agent,SIM_BALL);

//        ball.setVelocity(BNoiseVel);

//        Vector2D posBall=start-vel+BNoiseVel;
//        ball.setPosition(posBall);

//        int time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

//        if(time==UNKNOWN_VALUE)
//            return false;

//        for(int i=step ; i<=time ; i++){


//            for(int j=0; j<nr ; j++){

//                temp = info[j].opp;

//                int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


//                double dis;
//                if(i-turnTime+confing+info[j].conf<=0)
//                    dis=0;
//                else
//                    dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,i-turnTime+confing+info[j].conf);

//                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
//                    return false;
//                }

//            }

//            for(int j=0; j<nr ; j++)
//                if(posBall.dist(info[j].opp.getPosition())<BACK_DIS_DASH){

//                temp = info[j].opp;
//                Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);
//                int turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);


//                double dis;
//                if(i-turnTime+confing+info[j].conf<=0)
//                    dis=0;
//                else
//                    dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,min(i-turnTime+confing+info[j].conf,3));

//                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
//                    return false;
//                }

//            }
//            BNoiseVel*=BALL_DECAY;
//            posBall+=BNoiseVel;

//            if(i==time)
//                for(i=time+1;i<=time+Ext;i++){

//                    for(int j=0; j<nr ; j++){

//                        temp = info[j].opp;

//                        int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


//                        double dis;
//                        if(i-turnTime+confing+info[j].conf<=0)
//                            dis=0;
//                        else
//                            dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,i-turnTime+confing+info[j].conf);

//                        if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
//                            return false;
//                        }

//                    }



//                }


//        }
//        //////////                    right noise

//        ball.setVelocity(RNoiseVel);

//        posBall=start-vel+RNoiseVel;
//        ball.setPosition(posBall);

//        time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

//        if(time==UNKNOWN_VALUE)
//            return false;

//        for(int i=step ; i<=time ; i++){


//            for(int j=0; j<nr ; j++){

//                temp = info[j].opp;

//                int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


//                double dis;
//                if(i-turnTime+confing+info[j].conf<=0)
//                    dis=0;
//                else
//                    dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,i-turnTime+confing+info[j].conf);

//                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
//                    return false;
//                }

//            }

//            for(int j=0; j<nr ; j++)
//                if(posBall.dist(info[j].opp.getPosition())<BACK_DIS_DASH){

//                temp = info[j].opp;
//                Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);
//                int turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);


//                double dis;
//                if(i-turnTime+confing+info[j].conf<=0)
//                    dis=0;
//                else
//                    dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,min(i-turnTime+confing+info[j].conf,3));

//                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
//                    return false;
//                }

//            }
//            RNoiseVel*=BALL_DECAY;
//            posBall+=RNoiseVel;


//            if(i==time)
//                for(i=time+1;i<=time+Ext;i++){

//                    for(int j=0; j<nr ; j++){

//                        temp = info[j].opp;

//                        int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


//                        double dis;
//                        if(i-turnTime+confing+info[j].conf<=0)
//                            dis=0;
//                        else
//                            dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,i-turnTime+confing+info[j].conf);

//                        if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
//                            return false;
//                        }

//                    }



//                }

//        }
//        //////////                    left noise


//        ball.setVelocity(LNoiseVel);

//         posBall=start-vel+LNoiseVel;
//         ball.setPosition(posBall);

//        time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

//        if(time==UNKNOWN_VALUE)
//            return false;


//        for(int i=step ; i<=time ; i++){


//            for(int j=0; j<nr ; j++){

//                temp = info[j].opp;

//                int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


//                double dis;
//                if(i-turnTime+confing+info[j].conf<=0)
//                    dis=0;
//                else
//                    dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,i-turnTime+confing+info[j].conf);

//                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
//                    return false;
//                }

//            }

//            for(int j=0; j<nr ; j++)
//                if(posBall.dist(info[j].opp.getPosition())<BACK_DIS_DASH){

//                temp = info[j].opp;
//                Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);
//                int turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);


//                double dis;
//                if(i-turnTime+confing+info[j].conf<=0)
//                    dis=0;
//                else
//                    dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,min(i-turnTime+confing+info[j].conf,3));

//                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
//                    return false;
//                }

//            }
//            LNoiseVel*=BALL_DECAY;
//            posBall+=LNoiseVel;


//            if(i==time)
//                for(i=time+1;i<=time+Ext;i++){

//                    for(int j=0; j<nr ; j++){

//                        temp = info[j].opp;

//                        int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


//                        double dis;
//                        if(i-turnTime+confing+info[j].conf<=0)
//                            dis=0;
//                        else
//                            dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,i-turnTime+confing+info[j].conf);

//                        if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
//                            return false;
//                        }

//                    }



//                }

//        }
//    }

    return true;
//    return OCC(agent,start,end,speed,minConf,minSpeed,step,rand,withRand);
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
double Shoot::twoStepRand(PlayerAgent * agent,Body_SmartKick kick,double dPower){

    const WorldModel & wm = agent->world();

    int step=kick.sequence().pos_list_.size();
    Vector2D posBall=kick.sequence().pos_list_.at(step-2);

    Vector2D posAgent=wm.self().pos()+wm.self().vel();
    if(step==3)
        posAgent+=wm.self().vel()*0.4;

    double dir_diff=fabs(AngleDeg::normalize_angle(wm.self().body().degree()-(posBall-posAgent).dir().degree()));
    double dist=posAgent.dist(posBall)-0.385;

    double pos_rate = 0.5 + 0.25 * ( dir_diff/180 + dist/wm.self().playerType().kickableMargin() );    // [0.5, 1.0]

    double speed_rate = 0.75;//0.5 + 0.5 * ( wm.ball().vel().length() / ( /*ball maxSpeed*/3 * /* ballDecay */ BALL_DECAY ) );

    // [0, 2*kick_rand]
    return wm.self().playerType().kickRand() * ( dPower/ /* maxPower*/ 100 ) * ( pos_rate + speed_rate );


}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
double Shoot::kickRand(PlayerAgent * agent,Body_SmartKick kick,double dPower){

    const WorldModel & wm = agent->world();
    int step=kick.sequence().pos_list_.size();
    Vector2D posBall,posAgent;
    double  ballSpeed;

    if(step==1){
        posBall=wm.ball().pos();
        posAgent=wm.self().pos();
        ballSpeed=wm.ball().vel().length();

    }
    else if(step==2){
        posBall=kick.sequence().pos_list_.at(0);
        posAgent=wm.self().pos()+wm.self().vel();
        ballSpeed=/*kick.sequence().speed_*0.3*/1;

    }
    else if(step==3){
        posBall=kick.sequence().pos_list_.at(1);
        posAgent=wm.self().pos()+wm.self().vel()+wm.self().vel()*wm.self().playerType().playerDecay();
        ballSpeed=/*kick.sequence().speed_*0.3*/1;

    }
    else{
        cout<<"hey jo"<<endl;
        return 1000;

    }




    double dir_diff=fabs(AngleDeg::normalize_angle(wm.self().body().degree()-(posBall-posAgent).dir().degree()));
    double dist=posAgent.dist(posBall)-0.385;

    double pos_rate = 0.5 + 0.25 * ( dir_diff/180 + dist/wm.self().playerType().kickableMargin() );    // [0.5, 1.0]

    double speed_rate = 0.5 + 0.5 * ( ballSpeed / ( /*ball maxSpeed * ballDecay */ 2.82 ) );

    // [0, 2*kick_rand]
//    return wm.self().playerType().kickRand() * ( kick.sequence().power_/ /* maxPower*/ 100 ) * ( pos_rate + speed_rate );
    return wm.self().playerType().kickRand() * ( pos_rate + speed_rate ) / 2;


}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
bool    Shoot::isInGoal(Vector2D start,Vector2D end,double speed,double rand,double Y){

    double  dir=(end-start).dir().degree();
    Vector2D velKick=Vector2D::polar2vector(speed,dir);
    Vector2D RNoise=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir+90));
    Vector2D LNoise=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir-90));
    double  RDir=/*dir+atanDeg(rand);//*/(velKick+RNoise).dir().degree();
    double  LDir=/*dir+atanDeg(rand);//*/(velKick+LNoise).dir().degree();

    Line2D    RLine=Line2D(start/*-velKick*/,RDir);
    Line2D    LLine=Line2D(start/*-velKick*/,LDir);

    Line2D    line=Line2D(Vector2D(52.5,0),90);

    Vector2D RPos=line.intersection(RLine);
    Vector2D LPos=line.intersection(LLine);

    if(fabs(RPos.y) > Y || fabs(LPos.y)> Y)
        return false;
    return true;

}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
bool    Shoot::setKick(const WorldModel & wm,NewKick &kick,Body_SmartKick a){

    kick.cycle=wm.time().cycle();
    kick.step=a.sequence().pos_list_.size();
    kick.end=a.M_target_point;
    kick.M_speed=a.M_first_speed_thr;
    kick.speed=a.M_first_speed;
    return kick.step;
};
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
Vector2D    Shoot::startKick(const WorldModel & wm,Body_SmartKick &a){

    if(a.sequence().pos_list_.size() > 1 )
        return a.sequence().pos_list_.at(a.sequence().pos_list_.size()-2);
    else if(a.sequence().pos_list_.size() == 1 )
        return wm.ball().pos();
    else{
        cout<<"hey jo at startkick"<<endl;
        return Vector2D(1000,1000);
    }

//    if(a.sequence().pos_list_.size() > 0 )
//        return a.sequence().pos_list_.at(a.sequence().pos_list_.size()-1);
//    else{
//        cout<<"hey jo at startkick"<<endl;
//        return Vector2D(1000,1000);
//    }

}

Vector2D    Shoot::firstKickPos(const WorldModel & wm,Body_SmartKick &a){

    if(a.sequence().pos_list_.size() > 1 )
        return a.sequence().pos_list_.at(a.sequence().pos_list_.size()-2);
    else if(a.sequence().pos_list_.size() == 1 )
        return wm.ball().pos();
    else{
        cout<<"hey jo at startkick"<<endl;
        return Vector2D(1000,1000);
    }

//    if(a.sequence().pos_list_.size() > 0 )
//        return a.sequence().pos_list_.at(a.sequence().pos_list_.size()-1);
//    else{
//        cout<<"hey jo at startkick"<<endl;
//        return Vector2D(1000,1000);
//    }

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int    Shoot::stepKick(Body_SmartKick a){

        return a.sequence().pos_list_.size();

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
bool    Shoot::SHOOOOOOT(PlayerAgent * agent,NewKick &kick){
    const WorldModel & wm = agent->world();

    Vector2D start=wm.self().pos();


    if(start.dist(Vector2D(52.5,0.0))>25)
        return false;


    double y=-7.0;
    for(int i=0;i<=56; i++){
        Vector2D end=Vector2D(52.5,y);

        y += 0.25 ;

        int conf=getCount(wm,(end-start).dir().degree(),5);

        if(conf>3 )
            continue;


        double minSpeed=2.4+(start.dist(end)*0.06);

        if(minSpeed > 2.8 )
            minSpeed = 2.8;

        Body_SmartKick smart(end,3,minSpeed,1);

        if(!smart.execute(agent) )
            continue;
        double rand = /*getFirstEndOneStep(agent)*3;//*/kickRand(agent,smart);
        if(!isInGoal(firstKickPos(wm,smart),(end),smart.sequence().speed_,rand,6.9))
            continue;

        if(OCC(agent,startKick(wm,smart),end,smart.sequence().speed_,10,0.3,1,rand,false)){

            kick.reciverUnum = 666;
            setKick(wm,kick,smart);
            return true;
        }


    }
    y=-7.0;
    for(int i=0;i<=56;i++){

        Vector2D end=Vector2D(52.5,y);

        y+=0.25 ;
        int conf=getCount(wm,(end-start).dir().degree(),5);

        if(conf>3 )
            continue;

        double minSpeed=1.0+(start.dist(end)*0.06);

        Body_SmartKick smart(end,3,minSpeed,2);

        if(!smart.execute(agent) || smart.sequence().pos_list_.size()!=2 )
            continue;

        double rand =/* getFirstEndTwoStep(agent);//*/kickRand(agent,smart);
        if(!isInGoal(firstKickPos(wm,smart),end,smart.sequence().speed_,rand ,6.9))
            continue;

        if(OCC(agent,(startKick(wm,smart)),(end),smart.sequence().speed_,10,0.3,2,rand,false)){

            setKick(wm,kick,smart);
            kick.reciverUnum = 666;
            return true;
        }

    }

    y=-7.0;
    for(int i=0;i<=56; i++){
        Vector2D end=Vector2D(52.5,y);


        y+=0.25 ;
        int conf=getCount(wm,(end-start).dir().degree(),5);

        if(conf>3 )
            continue;

        double minSpeed=1.0+(start.dist(end)*0.06);

        Body_SmartKick smart(end,3,minSpeed,3);

        if(!smart.execute(agent) || smart.sequence().pos_list_.size()!=3 )
            continue;

        double rand = /*getFirstEndTwoStep(agent);//*/kickRand(agent,smart);

        if(!isInGoal((firstKickPos(wm,smart)),(end),smart.sequence().speed_,rand,6.9))
            continue;

        if(OCC(agent,(startKick(wm,smart)),(end),smart.sequence().speed_,10,0.3,3,rand,false)){

            setKick(wm,kick,smart);
            kick.reciverUnum = 666;
            return true;
        }


    }


    return false;

}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
vector < Vector2D >    Shoot::getDir(PlayerAgent *agent,Vector2D firsPos,Vector2D endPos,int minConf){

    const WorldModel & wm = agent->world();

    vector <AngleDeg> dir;
    vector <int> nr;
    vector <Vector2D > position;

    Vector2D    selfPos=wm.self().pos();

    AngleDeg from = AngleDeg::normalize_angle((firsPos-selfPos).dir().degree());
    AngleDeg to = AngleDeg::normalize_angle((endPos-selfPos).dir().degree());



    dir.push_back(from);
    nr.push_back(200);
    position.push_back(firsPos);

    for (int i = 1 ; i < 12; ++i) {

        if(wm.theirPlayer(i) && wm.theirPlayer(i)->posCount()<minConf && (wm.theirPlayer(i)->pos()-selfPos).dir().isWithin(from,to) ){

//            cout<<"yep"<<endl;
            dir.push_back(AngleDeg::normalize_angle((wm.theirPlayer(i)->pos()-selfPos).dir().degree()));
            nr.push_back(1);
            position.push_back(wm.theirPlayer(i)->pos());


        }

    }

    dir.push_back(to);
    nr.push_back(0);
    position.push_back(endPos);

    for (int i = 1; i < dir.size()-1; ++i) {
        for (int j = 1; j < dir.size()-1; ++j) {
            if(i!=j && dir.at(i).isWithin(from,dir.at(j)))
                nr.at(i)++;

        }

    }


    for (int i = 0; i < dir.size(); ++i) {
        for (int j = i+1 ; j < dir.size(); ++j) {
            if(nr.at(i)<nr.at(j)){

                typeSwap(nr.at(i),nr.at(j));
                typeSwap(dir.at(i),dir.at(j));
                typeSwap(position.at(i),position.at(j));


            }

        }

    }

    return position;


}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//bool    Shoot::getClearDir(PlayerAgent *agent,int minConf){

//    const WorldModel & wm = agent->world();

//    vector <AngleDeg> dir;
//    vector <int> nr;
//    vector <Vector2D > position;

//    Vector2D    selfPos=wm.self().pos();
//    Vector2D    ballPos=wm.ball().pos();

//    double dist = 25;
//    Vector2D firsPos = selfPos + Vector2D::polar2vector(dist,-60.0);
//    Vector2D endPos = selfPos + Vector2D::polar2vector(dist,+60.0);

//    AngleDeg from = AngleDeg::normalize_angle((firsPos-selfPos).dir().degree());
//    AngleDeg to = AngleDeg::normalize_angle((endPos-selfPos).dir().degree());



//    dir.push_back(from);
//    nr.push_back(200);
//    position.push_back(firsPos);

//    for (int i = 1 ; i < 12; ++i) {

//        if(wm.theirPlayer(i) && wm.theirPlayer(i)->posCount()<minConf && wm.theirPlayer(i)->distFromSelf()<dist && (wm.theirPlayer(i)->pos()-selfPos).dir().isWithin(from,to) ){

////            cout<<"yep"<<endl;
//            dir.push_back(AngleDeg::normalize_angle((wm.theirPlayer(i)->pos()-selfPos).dir().degree()));
//            nr.push_back(1);
//            position.push_back(wm.theirPlayer(i)->pos());


//        }

//    }

//    dir.push_back(to);
//    nr.push_back(0);
//    position.push_back(endPos);

//    cout<<"tedad :: "<<position.size()-1<<endl;

//    for (int i = 1; i < dir.size()-1; ++i) {
//        for (int j = 1; j < dir.size()-1; ++j) {
//            if(i!=j && dir.at(i).isWithin(from,dir.at(j)))
//                nr.at(i)++;

//        }

//    }


//    for (int i = 0; i < dir.size(); ++i) {
//        for (int j = i+1 ; j < dir.size(); ++j) {
//            if(nr.at(i)<nr.at(j)){

//                typeSwap(nr.at(i),nr.at(j));
//                typeSwap(dir.at(i),dir.at(j));
//                typeSwap(position.at(i),position.at(j));


//            }

//        }

//    }


//    vector < Vector2D > ends;

//    ends.push_back( position.at(0));

//    for (int i = 0; i < position.size()-1; ++i) {


//        Vector2D temp = selfPos + Vector2D::polar2vector(20,getBisectorTwoAngles( (position.at(i)-selfPos).dir().degree() , (position.at(i+1)-selfPos).dir().degree() ));
//        ends.push_back(temp);

//        cout<<"to : "<<ends.at(i)<<endl;

//    }

//    ends.push_back( position.at(position.size()-1));



//    for (int j = 1; j < 4; ++j) {


//        for (int i = 0; i < ends.size(); ++i) {

//            double minSpeed=1.0+(ballPos.dist(ends.at(i))*0.06);

//            Body_SmartKick smart(ends.at(i),3,minSpeed,j);

//            if(!smart.execute(agent) )
//                continue;

////            double speed = smart.sequence().speed_;


//            int conf = getCount(wm,ends.at(i),/*calculate_PM(wm,selfPos,ends.at(i),speed)*/5);

//            if(conf>3 )
//                continue;


//            if(OCI(agent,startKick(wm,smart),ends.at(i),smart.sequence().speed_,10,0.3,stepKick(smart),0.0,false,0,0)){

//                return true;
//            }

//        }

//    }

//    Vector2D bestPos = firsPos;
//    double  ang=0.0;
//    for (int i = 0; i < position.size() - 1; ++i) {
//        if(getAngDiff( (position.at(i)-selfPos).dir().degree() , (position.at(i+1)-selfPos).dir().degree() ) > ang ){
//            ang = getAngDiff( (position.at(i)-selfPos).dir().degree() , (position.at(i+1)-selfPos).dir().degree() );
//            bestPos = ends.at(i+1);
//        }
//    }

//    return doForceKick(agent,bestPos);




//}

//bool Shoot::doForceKick(PlayerAgent *agent,Vector2D pos)
//{

//    const WorldModel & wm = agent->world();

//    if ( wm.gameMode().type() == GameMode::PlayOn
//         && ! wm.self().goalie()
//         && wm.self().isKickable()
//         )
//    {
//        dlog.addText( Logger::TEAM,
//                      __FILE__": simultaneous kick" );
//        agent->debugClient().addMessage( "SimultaneousKick" );
//        Vector2D goal_pos( ServerParam::i().pitchHalfLength(), 0.0 );

//        if ( wm.self().pos().x > 36.0
//             && wm.self().pos().absY() > 10.0 )
//        {
//            goal_pos.x = 45.0;
//            dlog.addText( Logger::TEAM,
//                          __FILE__": simultaneous kick cross type" );
//        }
//        Body_SmartKick( pos,
//                          ServerParam::i().ballSpeedMax(),1,3
//                          ).execute( agent );

//        return true;
//    }

//    return false;
//}
