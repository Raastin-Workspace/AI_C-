#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <rcsc/action/view_change_width.h>

#include "Pass.h"
#include "Shoot.h"
#include "bhv_oci.h"
#include "bhv_dribble2010.h"
#include "bhv_basic_offensive_kick.h"

#define ANG_TURN 1
#define BACK_DIS_DASH 5
using namespace std;
using namespace rcsc;

#define XFACTOR 1.00
#define SIXFACTOR 1.171
int MostafPass::getCount(const WorldModel & wm,double dir,double PM){


       return max(wm.dirCount(dir),max(wm.dirCount(VecPosition::normalizeAngle(dir+PM)),wm.dirCount(VecPosition::normalizeAngle(dir-PM))));

}
int MostafPass::getCount(const WorldModel & wm,Vector2D pos,double PM){


        double dir=(pos-wm.self().pos()).dir().degree();
       return max(wm.dirCount(dir),max(wm.dirCount(VecPosition::normalizeAngle(dir+PM)),wm.dirCount(VecPosition::normalizeAngle(dir-PM))));

}
double  MostafPass::calculate_PM(const WorldModel & wm,Vector2D start,Vector2D end,double speed,double minSpeed){

    double dis=start.dist(end);

    if(dis>(speed-minSpeed)/0.06)
        return 100.0;

    double X=1-(dis*(0.06)/speed);

    int time =ceil((log10(X))/(log10(0.94)));

    dis=Simul().Sn(0.0,0.4,0.6,max(time-2,0));

    Vector2D pos=end+Vector2D::polar2vector(dis,VecPosition::normalizeAngle((end-start).th().degree()+90.0));

    return doubleNormalize(getAngDiff((pos-start).th().degree(),(end-start).th().degree()),3.0,10);



}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool MostafPass::OCI(PlayerAgent * agent,VecPosition start,VecPosition end,double speed,int minConf,double minSpeed,int step,double rand,bool withRand,int Ext){

    const WorldModel & wm = agent->world();


    struct stc{

        Simul2   opp;
        double  edp;
        double  edpBack;
        int     conf;
        double  kickableArea;
    }info[11];

    Simul2 temp(agent);

    int nr=0;

    int confing = max(1,getCount(wm,(end-toVecPos(wm.self().pos())).getDirection(),0))+Ext-1;

    for(int i=1;i<12;i++){
        if(wm.theirPlayer(i) && wm.theirPlayer(i)->seenPosCount() <= minConf){
            info[nr].opp=temp;
            info[nr].opp.getSimulFromType(wm,(ObjectT)(OBJECT_OPPONENT_1+(i-1)),SIM_PLAYER);
            info[nr].edp=100*info[nr].opp.getDashPowerRateSimul()*info[nr].opp.getEffortMaxSimul();//*dash_rate
            info[nr].edpBack=100*info[nr].opp.getDashPowerRateSimul()*info[nr].opp.getEffortMaxSimul()*info[nr].opp.getDashRate(180);
            info[nr].conf=info[nr].opp.getConfidenceSimul();
            info[nr].kickableArea=info[nr].opp.getHPSSimul()->kickableArea();
            if(i==wm.theirGoalieUnum())
                info[nr].kickableArea=1.35;
            nr++;
            //cout<<"add on player : "<<i<<"Object : "<<(ObjectT)(OBJECT_OPPONENT_1+(i-1))<<endl;

        }
    }
    if(nr==0){

        return true;


    }

    double dir=(end-start).getDirection();
    VecPosition vel=VecPosition(speed,dir,POLAR);

    VecPosition RNoiseVel=vel,LNoiseVel=vel,BNoiseVel=vel;
    if(withRand){
        RNoiseVel+=VecPosition(rand,VecPosition::normalizeAngle(dir+90),POLAR);
        LNoiseVel+=VecPosition(rand,VecPosition::normalizeAngle(dir-90),POLAR);
        BNoiseVel+=VecPosition(rand,VecPosition::normalizeAngle(dir+180),POLAR);
    }

    if(withRand==false){
        Line    line=Line::makeLineFromTwoPoints(start,end);

        for(int j=0; j<nr ; j++){

            VecPosition posOnLine=line.getPointOnLineClosestTo(info[j].opp.getPosSimul());
            temp = info[j].opp;
            for(int k=1;k<=confing;k++)
                temp.moveToPosSim(posOnLine,5);





            VecPosition temp1,temp2;
            Circle  cir=Circle(temp.getPosSimul()+temp.getVelSimul(),info[j].kickableArea);
            if(line.getCircleIntersectionPoints(cir,&temp1,&temp2)==false)
                continue;
            else if(temp1.isBetweenXY(start/*+vel*/,end) || temp2.isBetweenXY(start/*+vel*/,end))
                return false;


            temp1,temp2;
            cir=Circle(temp.getPosSimul(),info[j].kickableArea);
            if(line.getCircleIntersectionPoints(cir,&temp1,&temp2)==false)
                continue;
            else if(temp1.isBetweenXY(start/*+vel*/,end) || temp2.isBetweenXY(start/*+vel*/,end))
                return false;
            //VecPosition posOnLine=line.getPointOnLineClosestTo(temp.getPosSimul()+temp.getVelSimul());
            //if(line.getDistanceWithPoint(temp.getPosSimul()+temp.getVelSimul())/*-(conf/2)*/<info[j].kickableArea&& posOnLine.isBetweenXY(start+vel,end) )
                //return false;

        }
    }
    else{

        Line    line=Line::makeLineFromTwoPoints(start,end);
        VecPosition posOnLine;
        VecPosition temp1,temp2;
        Circle  cir;

        for(int j=0; j<nr ; j++){


            VecPosition BEnd=start+VecPosition(start.getDistanceTo(end)*1.05,BNoiseVel.getDirection(),POLAR);

            line=Line::makeLineFromTwoPoints(start,BEnd);

            posOnLine=line.getPointOnLineClosestTo(info[j].opp.getPosSimul());

            temp = info[j].opp;
            for(int k=1;k<=confing;k++)
                temp.moveToPosSim(posOnLine,5);

            cir=Circle(temp.getPosSimul()+temp.getVelSimul(),info[j].kickableArea);

            if((line.getCircleIntersectionPoints(cir,&temp1,&temp2)==true) &&(temp1.isBetweenXY(start+BNoiseVel/*-vel*/,end) || temp2.isBetweenXY(start+BNoiseVel/*-vel*/,BEnd)))
                return false;

            cir=Circle(temp.getPosSimul(),info[j].kickableArea);

            if((line.getCircleIntersectionPoints(cir,&temp1,&temp2)==true) &&(temp1.isBetweenXY(start+BNoiseVel/*-vel*/,end) || temp2.isBetweenXY(start+BNoiseVel/*-vel*/,BEnd)))
                return false;

            //VecPosition posOnLine=line.getPointOnLineClosestTo(temp.getPosSimul()+temp.getVelSimul());
            //if(line.getDistanceWithPoint(temp.getPosSimul()+temp.getVelSimul())/*-(conf/2)*/<info[j].kickableArea && posOnLine.isBetweenXY(start+BNoiseVel,end) )
                //return false;

            VecPosition REnd=start+VecPosition(start.getDistanceTo(end)*1.05,RNoiseVel.getDirection(),POLAR);

            line=Line::makeLineFromTwoPoints(start,REnd);

            posOnLine=line.getPointOnLineClosestTo(info[j].opp.getPosSimul());
            temp = info[j].opp;
            for(int k=1;k<=confing;k++)
                temp.moveToPosSim(posOnLine,5);

            cir=Circle(temp.getPosSimul()+temp.getVelSimul(),info[j].kickableArea);

            if((line.getCircleIntersectionPoints(cir,&temp1,&temp2)==true) &&(temp1.isBetweenXY(start+RNoiseVel/*-vel*/,REnd) || temp2.isBetweenXY(start+RNoiseVel/*-vel*/,REnd)))
                return false;

            cir=Circle(temp.getPosSimul(),info[j].kickableArea);

            if((line.getCircleIntersectionPoints(cir,&temp1,&temp2)==true) &&(temp1.isBetweenXY(start+RNoiseVel/*-vel*/,REnd) || temp2.isBetweenXY(start+RNoiseVel/*-vel*/,REnd)))
                return false;

            //VecPosition posOnLine=line.getPointOnLineClosestTo(temp.getPosSimul()+temp.getVelSimul());
            //if(line.getDistanceWithPoint(temp.getPosSimul()+temp.getVelSimul())/*-(conf/2)*/< info[j].kickableArea&& posOnLine.isBetweenXY(start+RNoiseVel,REnd) )
                //return false;

            VecPosition LEnd=start+VecPosition(start.getDistanceTo(end)*1.05,LNoiseVel.getDirection(),POLAR);

            line=Line::makeLineFromTwoPoints(start,LEnd);

            posOnLine=line.getPointOnLineClosestTo(info[j].opp.getPosSimul());

            temp = info[j].opp;
            for(int k=1;k<=confing;k++)
                temp.moveToPosSim(posOnLine,5);

            cir=Circle(temp.getPosSimul()+temp.getVelSimul(),info[j].kickableArea);

            if((line.getCircleIntersectionPoints(cir,&temp1,&temp2)==true) &&(temp1.isBetweenXY(start+LNoiseVel/*-vel*/,LEnd) || temp2.isBetweenXY(start+LNoiseVel/*-vel*/,LEnd)))
                return false;

            cir=Circle(temp.getPosSimul()+temp.getVelSimul(),info[j].kickableArea);

            if((line.getCircleIntersectionPoints(cir,&temp1,&temp2)==true) &&(temp1.isBetweenXY(start+LNoiseVel/*-vel*/,LEnd) || temp2.isBetweenXY(start+LNoiseVel/*-vel*/,LEnd)))
                return false;

            //VecPosition posOnLine=line.getPointOnLineClosestTo(temp.getPosSimul()+temp.getVelSimul());
            //if(line.getDistanceWithPoint(temp.getPosSimul()+temp.getVelSimul())/*-(conf/2)*/<info[j].kickableArea && posOnLine.isBetweenXY(start+BNoiseVel,LEnd) )
                //return false;
        }

    }





    if(withRand==false){
        Simul2   ball(agent);
        ball.getSimulFromType(wm,OBJECT_BALL,SIM_BALL);
        ball.setVelSimul(vel);
        ball.setPosSimul(start);
        VecPosition posBall=start;//+vel;

        int time=ball.calculateBallTimeTo(start.getDistanceTo(end),minSpeed);

        if(time==22771)
            return false;

        for(int i=step ; i<=time ; i++){


            for(int j=0; j<nr ; j++){

                temp = info[j].opp;

                int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


                double dis;
                if(i-turnTime+confing+info[j].conf<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelSimul().getMagnitude()+info[j].edp,temp.getDecaySimul(),info[j].edp,i-turnTime+confing+info[j].conf);

                if(temp.getPosSimul().getDistanceTo(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                    return false;
                }

            }

            for(int j=0; j<nr ; j++)
                if(posBall.getDistanceTo(info[j].opp.getPosSimul())<BACK_DIS_DASH){

                temp = info[j].opp;
                VecPosition posBack=temp.getPosSimul()+(temp.getPosSimul()-posBall);
                int turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);


                double dis;
                if(i-turnTime+confing+info[j].conf<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelSimul().getMagnitude()+info[j].edp,temp.getDecaySimul(),info[j].edp,min(i-turnTime+confing+info[j].conf,3));

                if(temp.getPosSimul().getDistanceTo(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                    return false;
                }

            }
            vel*=0.94;
            posBall+=vel;


            if(i==time)
                for(i=time+1;i<=time+Ext;i++){

                    for(int j=0; j<nr ; j++){

                        temp = info[j].opp;

                        int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


                        double dis;
                        if(i-turnTime+confing+info[j].conf<=0)
                            dis=0;
                        else
                            dis=temp.Sn(temp.getVelSimul().getMagnitude()+info[j].edp,temp.getDecaySimul(),info[j].edp,i-turnTime+confing+info[j].conf);

                        if(temp.getPosSimul().getDistanceTo(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                            return false;
                        }

                    }



                }


        }
    }
    else{
        //////////                    back noise
        Simul2   ball(agent);
        ball.getSimulFromType(wm,OBJECT_BALL,SIM_BALL);
        ball.setVelSimul(BNoiseVel);

        VecPosition posBall=start-vel+BNoiseVel;
        ball.setPosSimul(posBall);

        int time=ball.calculateBallTimeTo(start.getDistanceTo(end),minSpeed);

        if(time==22771)
            return false;

        for(int i=step ; i<=time ; i++){


            for(int j=0; j<nr ; j++){

                temp = info[j].opp;

                int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


                double dis;
                if(i-turnTime+confing+info[j].conf<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelSimul().getMagnitude()+info[j].edp,temp.getDecaySimul(),info[j].edp,i-turnTime+confing+info[j].conf);

                if(temp.getPosSimul().getDistanceTo(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                    return false;
                }

            }

            for(int j=0; j<nr ; j++)
                if(posBall.getDistanceTo(info[j].opp.getPosSimul())<BACK_DIS_DASH){

                temp = info[j].opp;
                VecPosition posBack=temp.getPosSimul()+(temp.getPosSimul()-posBall);
                int turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);


                double dis;
                if(i-turnTime+confing+info[j].conf<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelSimul().getMagnitude()+info[j].edp,temp.getDecaySimul(),info[j].edp,min(i-turnTime+confing+info[j].conf,3));

                if(temp.getPosSimul().getDistanceTo(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                    return false;
                }

            }
            BNoiseVel*=0.94;
            posBall+=BNoiseVel;

            if(i==time)
                for(i=time+1;i<=time+Ext;i++){

                    for(int j=0; j<nr ; j++){

                        temp = info[j].opp;

                        int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


                        double dis;
                        if(i-turnTime+confing+info[j].conf<=0)
                            dis=0;
                        else
                            dis=temp.Sn(temp.getVelSimul().getMagnitude()+info[j].edp,temp.getDecaySimul(),info[j].edp,i-turnTime+confing+info[j].conf);

                        if(temp.getPosSimul().getDistanceTo(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                            return false;
                        }

                    }



                }


        }
        //////////                    right noise

        ball.setVelSimul(RNoiseVel);

        posBall=start-vel+RNoiseVel;
        ball.setPosSimul(posBall);

        time=ball.calculateBallTimeTo(start.getDistanceTo(end),minSpeed);

        if(time==22771)
            return false;

        for(int i=step ; i<=time ; i++){


            for(int j=0; j<nr ; j++){

                temp = info[j].opp;

                int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


                double dis;
                if(i-turnTime+confing+info[j].conf<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelSimul().getMagnitude()+info[j].edp,temp.getDecaySimul(),info[j].edp,i-turnTime+confing+info[j].conf);

                if(temp.getPosSimul().getDistanceTo(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                    return false;
                }

            }

            for(int j=0; j<nr ; j++)
                if(posBall.getDistanceTo(info[j].opp.getPosSimul())<BACK_DIS_DASH){

                temp = info[j].opp;
                VecPosition posBack=temp.getPosSimul()+(temp.getPosSimul()-posBall);
                int turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);


                double dis;
                if(i-turnTime+confing+info[j].conf<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelSimul().getMagnitude()+info[j].edp,temp.getDecaySimul(),info[j].edp,min(i-turnTime+confing+info[j].conf,3));

                if(temp.getPosSimul().getDistanceTo(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                    return false;
                }

            }
            RNoiseVel*=0.94;
            posBall+=RNoiseVel;


            if(i==time)
                for(i=time+1;i<=time+Ext;i++){

                    for(int j=0; j<nr ; j++){

                        temp = info[j].opp;

                        int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


                        double dis;
                        if(i-turnTime+confing+info[j].conf<=0)
                            dis=0;
                        else
                            dis=temp.Sn(temp.getVelSimul().getMagnitude()+info[j].edp,temp.getDecaySimul(),info[j].edp,i-turnTime+confing+info[j].conf);

                        if(temp.getPosSimul().getDistanceTo(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                            return false;
                        }

                    }



                }

        }
        //////////                    left noise


        ball.setVelSimul(LNoiseVel);

         posBall=start-vel+LNoiseVel;
         ball.setPosSimul(posBall);

        time=ball.calculateBallTimeTo(start.getDistanceTo(end),minSpeed);

        if(time==22771)
            return false;


        for(int i=step ; i<=time ; i++){


            for(int j=0; j<nr ; j++){

                temp = info[j].opp;

                int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


                double dis;
                if(i-turnTime+confing+info[j].conf<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelSimul().getMagnitude()+info[j].edp,temp.getDecaySimul(),info[j].edp,i-turnTime+confing+info[j].conf);

                if(temp.getPosSimul().getDistanceTo(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                    return false;
                }

            }

            for(int j=0; j<nr ; j++)
                if(posBall.getDistanceTo(info[j].opp.getPosSimul())<BACK_DIS_DASH){

                temp = info[j].opp;
                VecPosition posBack=temp.getPosSimul()+(temp.getPosSimul()-posBall);
                int turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);


                double dis;
                if(i-turnTime+confing+info[j].conf<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelSimul().getMagnitude()+info[j].edp,temp.getDecaySimul(),info[j].edp,min(i-turnTime+confing+info[j].conf,3));

                if(temp.getPosSimul().getDistanceTo(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                    return false;
                }

            }
            LNoiseVel*=0.94;
            posBall+=LNoiseVel;


            if(i==time)
                for(i=time+1;i<=time+Ext;i++){

                    for(int j=0; j<nr ; j++){

                        temp = info[j].opp;

                        int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


                        double dis;
                        if(i-turnTime+confing+info[j].conf<=0)
                            dis=0;
                        else
                            dis=temp.Sn(temp.getVelSimul().getMagnitude()+info[j].edp,temp.getDecaySimul(),info[j].edp,i-turnTime+confing+info[j].conf);

                        if(temp.getPosSimul().getDistanceTo(posBall)<dis+info[j].kickableArea/*+(1/i)*/){
                            return false;
                        }

                    }



                }

        }
    }

    return true;
//    return OCC(agent,start,end,speed,minConf,minSpeed,step,rand,withRand);
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool MostafPass::OCC(PlayerAgent * agent,VecPosition start,VecPosition end,double speed,int minConf,double minSpeed,int step,double rand,bool withRand){

    const WorldModel & wm = agent->world();

    Simul2   opp;
    double  edp;
    double  edpBack;
    int     conf;

    Simul2 temp(agent);


    if(wm.theirPlayer(wm.theirGoalieUnum()) && wm.theirPlayer(wm.theirGoalieUnum())->posCount() < minConf){
        opp=temp;
        opp.getSimulFromType(wm,(ObjectT)(OBJECT_OPPONENT_1+wm.theirGoalieUnum()-1),SIM_PLAYER);
        edp=100*opp.getDashPowerRateSimul()*opp.getEffortMaxSimul();//*dash_rate
        edpBack=100*opp.getDashPowerRateSimul()*opp.getEffortMaxSimul()*opp.getDashRate(180);
        conf=opp.getConfidenceSimul();

    }
    else
        return true;


    double dir=(end-start).getDirection();
    VecPosition vel=VecPosition(speed,dir,POLAR);

    int config = max(0,getCount(wm,(end-toVecPos(wm.self().pos())).getDirection(),5));



    VecPosition RNoiseVel=vel,LNoiseVel=vel,BNoiseVel=vel;
    if(withRand){
        RNoiseVel+=VecPosition(rand,VecPosition::normalizeAngle(dir+90),POLAR);
        LNoiseVel+=VecPosition(rand,VecPosition::normalizeAngle(dir-90),POLAR);
        BNoiseVel+=VecPosition(rand,VecPosition::normalizeAngle(dir+180),POLAR);
    }

    if(withRand==false){
        Line    line=Line::makeLineFromTwoPoints(start,end);

        VecPosition temp1,temp2;
        Circle  cir=Circle(opp.getPosSimul()+opp.getVelSimul(),1.3/*opp.getHPSSimul().dCatchableAreaL*/);
        if(line.getCircleIntersectionPoints(cir,&temp1,&temp2)==false)
            ;
        else if(temp1.isBetweenXY(start/*+vel*/,end) || temp2.isBetweenXY(start/*+vel*/,end))
            return false;

        //VecPosition posOnLine=line.getPointOnLineClosestTo(opp.getPosSimul()+opp.getVelSimul());

        //if(line.getDistanceWithPoint(opp.getPosSimul()+opp.getVelSimul())/*-(conf/2)*/<1.3/*opp.getHPSSimul().dCatchableAreaL*/ && posOnLine.isBetweenXY(start+vel,end) )
            //return false;



    }
    else{
        Line    line=Line::makeLineFromTwoPoints(start,end);

        VecPosition temp1,temp2;
        Circle  cir=Circle(opp.getPosSimul()+opp.getVelSimul(),1.3/*opp.getHPSSimul().dCatchableAreaL*/);
        if(line.getCircleIntersectionPoints(cir,&temp1,&temp2)==false)
            ;
        else if(temp1.isBetweenXY(start+BNoiseVel-vel,end) || temp2.isBetweenXY(start+BNoiseVel-vel,end))
            return false;

        //VecPosition posOnLine=line.getPointOnLineClosestTo(opp.getPosSimul()+opp.getVelSimul());

        //if(line.getDistanceWithPoint(opp.getPosSimul()+opp.getVelSimul())/*-(conf/2)*/<1.3/*opp.getHPSSimul().dCatchableAreaL*/ && posOnLine.isBetweenXY(start+BNoiseVel,end) ){
            //return false;

        //}

        VecPosition REnd=start+VecPosition(start.getDistanceTo(end),RNoiseVel.getDirection(),POLAR);
        line=Line::makeLineFromTwoPoints(start,REnd);

        if(line.getCircleIntersectionPoints(cir,&temp1,&temp2)==false)
            ;
        else if(temp1.isBetweenXY(start+RNoiseVel-vel,end) || temp2.isBetweenXY(start+RNoiseVel-vel,end))
            return false;
        //posOnLine=line.getPointOnLineClosestTo(opp.getPosSimul()+opp.getVelSimul());

        //if(line.getDistanceWithPoint(opp.getPosSimul()+opp.getVelSimul())/*-(conf/2)*/<1.3/*opp.getHPSSimul().dCatchableAreaL*/ && posOnLine.isBetweenXY(start+RNoiseVel,end) ){
            //return false;

        //}

        VecPosition LEnd=start+VecPosition(start.getDistanceTo(end),LNoiseVel.getDirection(),POLAR);
        line=Line::makeLineFromTwoPoints(start,LEnd);

        if(line.getCircleIntersectionPoints(cir,&temp1,&temp2)==false)
            ;
        else if(temp1.isBetweenXY(start+LNoiseVel-vel,end) || temp2.isBetweenXY(start+LNoiseVel-vel,end))
            return false;
        //posOnLine=line.getPointOnLineClosestTo(opp.getPosSimul()+opp.getVelSimul());

        //if(line.getDistanceWithPoint(opp.getPosSimul()+opp.getVelSimul())/*-(conf/2)*/<1.3/*opp.getHPSSimul().dCatchableAreaL*/ && posOnLine.isBetweenXY(start+LNoiseVel,end) ){
            //return false;

        //}


    }

    if(withRand==false){
        Simul2   ball(agent);
        ball.getSimulFromType(wm,OBJECT_BALL,SIM_BALL);
        ball.setVelSimul(vel);

        VecPosition posBall=start/*+vel*/;
        ball.setPosSimul(posBall);
        int time=ball.calculateBallTimeTo(start.getDistanceTo(end),minSpeed);

        if(time==22771)
            return false;



        for(int i=step ; i<=time ; i++){


            temp = opp;


            int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


            double dis;
            if(i-turnTime+config/*+conf*/<=0)
                dis=0;
            else
                dis=temp.Sn(temp.getVelSimul().getMagnitude()+edp,temp.getDecaySimul(),edp,i-turnTime+config/*+conf*/);

            if(temp.getPosSimul().getDistanceTo(posBall)<dis+1.3/**temp.getHPSSimul().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }


            temp = opp;


            VecPosition posBack=temp.getPosSimul()+(temp.getPosSimul()-posBall);


            turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);



            if(i-turnTime+config/*+conf*/<=0)
                dis=0;
            else
                dis=temp.Sn(temp.getVelSimul().getMagnitude()+edp,temp.getDecaySimul(),edp,i-turnTime+config/*+conf*/);

            if(temp.getPosSimul().getDistanceTo(posBall)<dis+1.3/**temp.getHPSSimul().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }

            vel*=0.94;
            posBall+=vel;

        }
    }
    else{
        //////////                    back noise

        Simul2   ball(agent);
        ball.getSimulFromType(wm,OBJECT_BALL,SIM_BALL);
        ball.setVelSimul(BNoiseVel);

        VecPosition posBall=start+BNoiseVel-vel;
        ball.setPosSimul(posBall);
        int time=ball.calculateBallTimeTo(start.getDistanceTo(end),minSpeed);

        if(time==22771)
            return false;


        for(int i=step ; i<=time ; i++){


            temp = opp;


            int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


            double dis;
            if(i-turnTime+config/*+conf*/<=0)
                dis=0;
            else
                dis=temp.Sn(temp.getVelSimul().getMagnitude()+edp,temp.getDecaySimul(),edp,i-turnTime+config/*+conf*/);

            if(temp.getPosSimul().getDistanceTo(posBall)<dis+1.3/**temp.getHPSSimul().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }


            temp = opp;


            VecPosition posBack=temp.getPosSimul()+(temp.getPosSimul()-posBall);


            turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);



            if(i-turnTime+config/*+conf*/<=0)
                dis=0;
            else
                dis=temp.Sn(temp.getVelSimul().getMagnitude()+edp,temp.getDecaySimul(),edp,i-turnTime+config/*+conf*/);

            if(temp.getPosSimul().getDistanceTo(posBall)<dis+1.3/**temp.getHPSSimul().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }

            BNoiseVel*=0.94;
            posBall+=BNoiseVel;

        }
        //////////                    right noise


        ball.setVelSimul(RNoiseVel);

        posBall=start+RNoiseVel-vel;
        ball.setPosSimul(posBall);

        time=ball.calculateBallTimeTo(start.getDistanceTo(end),minSpeed);

        if(time==22771)
            return false;


        for(int i=step ; i<=time ; i++){


            temp = opp;


            int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


            double dis;
            if(i-turnTime+config/*+conf*/<=0)
                dis=0;
            else
                dis=temp.Sn(temp.getVelSimul().getMagnitude()+edp,temp.getDecaySimul(),edp,i-turnTime+config/*+conf*/);

            if(temp.getPosSimul().getDistanceTo(posBall)<dis+1.3/**temp.getHPSSimul().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }


            temp = opp;


            VecPosition posBack=temp.getPosSimul()+(temp.getPosSimul()-posBall);


            turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);



            if(i-turnTime+config/*+conf*/<=0)
                dis=0;
            else
                dis=temp.Sn(temp.getVelSimul().getMagnitude()+edp,temp.getDecaySimul(),edp,i-turnTime+config/*+conf*/);

            if(temp.getPosSimul().getDistanceTo(posBall)<dis+1.3/**temp.getHPSSimul().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }

            RNoiseVel*=0.94;
            posBall+=RNoiseVel;

        }
        //////////                    left noise


        ball.setVelSimul(LNoiseVel);

        posBall=start+LNoiseVel-vel;
        ball.setPosSimul(posBall);

        time=ball.calculateBallTimeTo(start.getDistanceTo(end),minSpeed);

        if(time==22771)
            return false;


        for(int i=step ; i<=time ; i++){


            temp = opp;


            int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


            double dis;
            if(i-turnTime+config/*+conf*/<=0)
                dis=0;
            else
                dis=temp.Sn(temp.getVelSimul().getMagnitude()+edp,temp.getDecaySimul(),edp,i-turnTime+config/*+conf*/);

            if(temp.getPosSimul().getDistanceTo(posBall)<dis+1.3/**temp.getHPSSimul().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }


            temp = opp;


            VecPosition posBack=temp.getPosSimul()+(temp.getPosSimul()-posBall);


            turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);



            if(i-turnTime+config/*+conf*/<=0)
                dis=0;
            else
                dis=temp.Sn(temp.getVelSimul().getMagnitude()+edp,temp.getDecaySimul(),edp,i-turnTime+config/*+conf*/);

            if(temp.getPosSimul().getDistanceTo(posBall)<dis+1.3/**temp.getHPSSimul().dCatchableAreaL*/ /*+(1/i)*/){
                return false;
            }

            LNoiseVel*=0.94;
            posBall+=LNoiseVel;

        }
    }
    return true;
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
double MostafPass::oneStepRand(PlayerAgent * agent,double dPower){

    const WorldModel & wm = agent->world();

    double dir_diff=fabs(VecPosition::normalizeAngle(wm.self().body().degree()-(wm.ball().pos()-wm.self().pos()).dir().degree()));
    double dist=wm.self().pos().dist(wm.ball().pos())-0.385;

    double pos_rate = 0.5 + 0.25 * ( dir_diff/180 + dist/wm.self().playerType().kickableMargin() );    // [0.5, 1.0]

    double speed_rate = 0.5 + 0.5 * ( wm.ball().vel().length() / ( /*ball maxSpeed*/3 * /* ballDecay */ 0.94 ) );

    // [0, 2*kick_rand]
    return wm.self().playerType().kickRand() * ( dPower/ /* maxPower*/ 100 ) * ( pos_rate + speed_rate );


}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
double MostafPass::twoStepRand(PlayerAgent * agent,Body_SmartKick kick,double dPower){

    const WorldModel & wm = agent->world();

    int step=kick.sequence().pos_list_.size();
    Vector2D posBall=kick.sequence().pos_list_.at(step-2);

    Vector2D posAgent=wm.self().pos()+wm.self().vel();
    if(step==3)
        posAgent+=wm.self().vel()*0.4;

    double dir_diff=fabs(VecPosition::normalizeAngle(wm.self().body().degree()-(posBall-posAgent).dir().degree()));
    double dist=posAgent.dist(posBall)-0.385;

    double pos_rate = 0.5 + 0.25 * ( dir_diff/180 + dist/wm.self().playerType().kickableMargin() );    // [0.5, 1.0]

    double speed_rate = 0.75;//0.5 + 0.5 * ( wm.ball().vel().length() / ( /*ball maxSpeed*/3 * /* ballDecay */ 0.94 ) );

    // [0, 2*kick_rand]
    return wm.self().playerType().kickRand() * ( dPower/ /* maxPower*/ 100 ) * ( pos_rate + speed_rate );


}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
double MostafPass::kickRand(PlayerAgent * agent,Body_SmartKick & kick,double dPower){

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
        ballSpeed=/*kick.sequence().speed_*0.3*/0;

    }
    else if(step==3){
        posBall=kick.sequence().pos_list_.at(1);
        posAgent=wm.self().pos()+wm.self().vel()+wm.self().vel()*wm.self().playerType().playerDecay();
        ballSpeed=/*kick.sequence().speed_*0.3*/0;

    }
    else{
        cout<<"hey jo"<<endl;
        return 1000;

    }




    double dir_diff=fabs(VecPosition::normalizeAngle(wm.self().body().degree()-(posBall-posAgent).dir().degree()));
    double dist=posAgent.dist(posBall)-0.385;

    double pos_rate = 0.5 + 0.25 * ( dir_diff/180 + dist/wm.self().playerType().kickableMargin() );    // [0.5, 1.0]

    double speed_rate = 0.5 + 0.5 * ( ballSpeed / ( /*ball maxSpeed * ballDecay */ 2.82 ) );

    // [0, 2*kick_rand]
    return wm.self().playerType().kickRand() * ( kick.sequence().power_/ /* maxPower*/ 100 ) * ( pos_rate + speed_rate );


}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
bool    MostafPass::isInGoal(VecPosition start,VecPosition end,double speed,double rand,double Y){

    double  dir=(end-start).getDirection();
    VecPosition velKick=VecPosition(speed,dir,POLAR);
    VecPosition RNoise=VecPosition(rand,VecPosition::normalizeAngle(dir+90),POLAR);
    VecPosition LNoise=VecPosition(rand,VecPosition::normalizeAngle(dir-90),POLAR);
    double  RDir=(velKick+RNoise).getDirection();
    double  LDir=(velKick+LNoise).getDirection();

    Line    RLine=Line::makeLineFromPositionAndAngle(start-velKick,RDir);
    Line    LLine=Line::makeLineFromPositionAndAngle(start-velKick,LDir);

    Line    line=Line::makeLineFromPositionAndAngle(VecPosition(52.5,0),90);

    VecPosition RPos=line.getIntersection(RLine);
    VecPosition LPos=line.getIntersection(LLine);

    if(RPos.getY()>(Y) || RPos.getY()<(Y*(-1)) || LPos.getY()>(Y) || LPos.getY()<(Y*(-1)))
        return false;


    return true;

}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
bool    MostafPass::setKick(const WorldModel & wm,NewKick &kick,Body_SmartKick a){

    kick.cycle=wm.time().cycle();
    kick.step=a.sequence().pos_list_.size();
    kick.end=a.M_target_point;
    kick.M_speed=a.M_first_speed_thr;
    kick.speed=a.M_first_speed;
    return kick.step;
};
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
Vector2D    MostafPass::startKick(const WorldModel & wm,Body_SmartKick &a){

//    if(a.sequence().pos_list_.size() > 1 )
//        return a.sequence().pos_list_.at(a.sequence().pos_list_.size()-2);
//    else if(a.sequence().pos_list_.size() == 1 )
//        return wm.ball().pos();
//    else{
//        cout<<"hey jo at startkick"<<endl;
//        return Vector2D(1000,1000);
//    }

    if(a.sequence().pos_list_.size() > 0 )
        return a.sequence().pos_list_.at(a.sequence().pos_list_.size()-1);
    else{
        //cout<<"hey jo at startkick"<<endl;
        return Vector2D(1000,1000);
    }

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int    MostafPass::stepKick(Body_SmartKick a){

        return a.sequence().pos_list_.size();

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
bool    MostafPass::SHOOOOOOT(PlayerAgent * agent,NewKick &kick){


    const WorldModel & wm = agent->world();

    Vector2D start=wm.self().pos();


    if(start.dist(Vector2D(52.5,0.0))>20)
        return false;


    double y=-7.0;
    for(int i=0;i<=56;y+=0.25 && i++){
        Vector2D end=Vector2D(52.5,y);

        int conf=getCount(wm,(end-start).dir().degree(),5);

        double minSpeed=1.0+(start.dist(end)*0.06);

        Body_SmartKick smart(end,3,minSpeed,1);





        if(conf>3 )
            continue;
        if(!smart.execute(agent) )
            continue;
        double rand = kickRand(agent,smart);
        if(!isInGoal(toVecPos(startKick(wm,smart)),toVecPos(end),smart.sequence().speed_,rand,7.0))
            continue;

        if(OCI(agent,toVecPos(startKick(wm,smart)),toVecPos(end),smart.sequence().speed_,10,0.3,stepKick(smart),rand,true,0)){

            setKick(wm,kick,smart);
            return true;
        }



    }
    y=-7.0;
    for(int i=0;i<=56;y+=0.25 && i++){
        Vector2D end=Vector2D(52.5,y);

        int conf=getCount(wm,(end-start).dir().degree(),5);

        double minSpeed=1.0+(start.dist(end)*0.06);

        Body_SmartKick smart(end,3,minSpeed,2);





        if(conf>3 )
            continue;
        if(!smart.execute(agent) )
            continue;

        double rand = kickRand(agent,smart);
        if(!isInGoal(toVecPos(startKick(wm,smart)),toVecPos(end),smart.sequence().speed_,rand,7.0))
            continue;

        if(OCI(agent,toVecPos(startKick(wm,smart)),toVecPos(end),smart.sequence().speed_,10,0.3,stepKick(smart),rand,true,0)){

            setKick(wm,kick,smart);
            return true;
        }



    }

    y=-7.0;
    for(int i=0;i<=56;y+=0.25 && i++){
        Vector2D end=Vector2D(52.5,y);

        int conf=getCount(wm,(end-start).dir().degree(),5);

        double minSpeed=1.0+(start.dist(end)*0.06);

        Body_SmartKick smart(end,3,minSpeed,3);





        if(conf>3 )
            continue;
        if(!smart.execute(agent) )
            continue;

        double rand = kickRand(agent,smart);

        if(!isInGoal(toVecPos(startKick(wm,smart)),toVecPos(end),smart.sequence().speed_,rand,7.0))
            continue;

        if(OCI(agent,toVecPos(startKick(wm,smart)),toVecPos(end),smart.sequence().speed_,10,0.3,stepKick(smart),rand,true,0)){

            setKick(wm,kick,smart);
            kick.reciverUnum = 666;
            return true;
        }



    }


    return false;

}


double      MostafPass::flagScoring(PlayerAgent * agent,VecPosition    pos,VecPosition posFrom,int minConf){

    const WorldModel & wm = agent->world();

    VecPosition posOpp[11];
    int nr=0;

    for (int i = 1; i < 12; i++)
        if(wm.theirPlayer(i) && wm.theirPlayer(i)->posCount() <= minConf){

            posOpp[nr]=toVecPos(wm.theirPlayer(i)->pos());
            nr++;
        }

    double Rad=reRate(pos.x(),-52.5,60.5,7,15,RRM_REVERSE);

    if(posFrom.getX()<30){

        VecPosition	score=0;

        for (int j = 0; j < nr; j++)
        {



                VecPosition	R=posOpp[j]-pos;

                double  dir=0,dis=0;
                if(R.getMagnitude()>Rad)
                    continue;
                dis=reRate(Rad-R.getMagnitude(),0,Rad,0,10);
                int s=(R.getDirection())/fabs(R.getDirection());
                dir=fabs(R.getDirection());
                dir=reRate(dir,0,180,0,60,RRM_DIRECT);
                dir*=s;

                score+=VecPosition(dis,VecPosition::normalizeAngle(dir+180),POLAR);


        }

        double offScore=reRate(wm.offsideLineX()-pos.getX(),-8,50,0,33,RRM_REVERSE);
        VecPosition Z=VecPosition(offScore,0,POLAR);
        score+=Z;
        VecPosition pos=VecPosition(10,0,POLAR);
        double X=dakheli(score,pos)/(pow(pos.getMagnitude(),2));
        pos*=X;
        int s=1;
        if(fabs(score.getDirection())>90)
            s=-1;
        return  s*score.getMagnitude() ;


    }
    else{
        double dirTo=0;
        Rad=7;
        double offScore=20-reRate(pos.disToLine(VecPosition(51,7),VecPosition(51,-7),dirTo),0,30,0,20,RRM_DIRECT);
        VecPosition Z=VecPosition(offScore,dirTo,POLAR);

        VecPosition	score=0;
        for (int j = 0; j < 11; j++)
        {

                VecPosition	R=posOpp[j]-pos;

                double  dir=0,dis=0;
                if(R.getMagnitude()>Rad)
                    continue;
                dis=reRate(Rad-R.getMagnitude(),0,7,0,Rad);
                dir=VecPosition::normalizeAngle(R.getDirection()-dirTo);
                int s=(dir)/fabs(dir);
                dir=reRate(fabs(dir),0,180,0,60,RRM_DIRECT);
                dir*=s;

                score+=VecPosition(dis,VecPosition::normalizeAngle(dir+dirTo+180),POLAR);


        }

        score=score*(((  1.052  )))+Z*(((   2.6   )));
        VecPosition pos=VecPosition(10,dirTo,POLAR);
        double X=dakheli(score,pos)/(pow(pos.getMagnitude(),2));
        pos*=X;
        int s=1;
        if(fabs(VecPosition::normalizeAngle(score.getDirection()-dirTo))>90)
            s=-1;
       return  s*score.getMagnitude() ;


    }


}
/// //////////////////////////
double      MostafPass::flagScoring(PlayerAgent * agent,Vector2D    pos,Vector2D posFrom,int minConf){

    const WorldModel & wm = agent->world();

    Vector2D posOpp[11];
    int nr=0;

    for (int i = 1; i < 12; i++)
        if(wm.theirPlayer(i) && wm.theirPlayer(i)->posCount() <= minConf){

            posOpp[nr]= wm.theirPlayer(i)->pos();
            nr++;
        }

    double Rad=reRate(pos.x,-52.5,60.5,7,15,RRM_REVERSE);

    if(posFrom.x < 30){

        Vector2D	score = Vector2D(0.0,0.0);

        for (int j = 0; j < nr; j++)
        {



                Vector2D	R=posOpp[j]-pos;

                double  dir=0,dis=0;

                if(R.length()>Rad)
                    continue;

                dis=reRate(Rad-R.length(),0,Rad,0,10);
                int s=(R.dir().degree())/fabs(R.dir().degree());
                dir=fabs(R.dir().degree());
                dir=reRate(dir,0,180,0,60,RRM_DIRECT);
                dir*=s;

                score+=Vector2D::polar2vector(dis,AngleDeg::normalize_angle(dir+180));
        }

        double offScore=reRate(wm.offsideLineX()-pos.x ,-8,50,0,33,RRM_REVERSE);
        Vector2D Z=Vector2D::polar2vector(offScore,0);
        score+=Z;
        Vector2D pos=Vector2D::polar2vector(10,0);
        double X=dakheli(score,pos)/(pow(pos.length(),2));
        pos*=X;
        int s=1;
        if(fabs(score.dir().degree())>90)
            s=-1;
        return  s*score.length() ;


    }
    else{
        double dirTo=0;
        Rad=7;

        double offScore=20-reRate(VecPosition::disToLine(pos,Vector2D(51.0,7.0),Vector2D(51.0,-7.0),dirTo),0,30,0,20,RRM_DIRECT);
        Vector2D Z=Vector2D::polar2vector(offScore,dirTo);

        Vector2D	score=Vector2D(0.0,0.0);
        for (int j = 0; j < 11; j++)
        {

                Vector2D	R=posOpp[j]-pos;

                double  dir=0,dis=0;
                if(R.length()>Rad)
                    continue;
                dis=reRate(Rad-R.length(),0,7,0,Rad);
                dir=AngleDeg::normalize_angle(R.length()-dirTo);
                int s=(dir)/fabs(dir);
                dir=reRate(fabs(dir),0,180,0,60,RRM_DIRECT);
                dir*=s;

                score+=Vector2D::polar2vector(dis,AngleDeg::normalize_angle(dir+dirTo+180));


        }

        score=score*(((  1.052  )))+Z*(((   2.6   )));
        Vector2D pos=Vector2D::polar2vector(10,dirTo);
        double X=dakheli(score,pos)/(pow(pos.length(),2));
        pos*=X;
        int s=1;
        if(fabs(AngleDeg::normalize_angle(score.dir().degree()-dirTo))>90)
            s=-1;
       return  s*score.length() ;


    }


}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

spd      MostafPass::getDirectSpeed(Vector2D & start,Vector2D  & end){

    spd speed;

    // evolate speed
    // A1 = An*q+Sn*(1-q)
    speed.max=doubleNormalize(2.0/*finalMaxSpeed*/*0.94/*ballDecay*/+start.dist(end)*0.06/*1-ballDecay*/,0.0,3.0);

    if(end.x > 34.0 && fabs(end.y) < 18.0 )
        speed.max=doubleNormalize(2.2/*finalMaxSpeed*/*0.94/*ballDecay*/+start.dist(end)*0.06/*1-ballDecay*/,0.0,3.0);
    if(end.x < -30.0 )
        speed.max=doubleNormalize(1.5/*finalMaxSpeed*/*0.94/*ballDecay*/+start.dist(end)*0.06/*1-ballDecay*/,0.0,3.0);

    speed.min=doubleNormalize(0.4/*finalMinSpeed*/*0.94/*ballDecay*/+start.dist(end)*0.06/*1-ballDecay*/,0.0,2.4);


    return speed;
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

spd      MostafPass::getLeadSpeed(PlayerAgent * agent, int & nr, Vector2D & start, Vector2D & end, int & step, int avans){

    const WorldModel & wm = agent->world();
    spd speed;

    if(!wm.ourPlayer(nr)){
        speed.max=0.0;
        speed.min=0.0;
        return speed;
    }
    Simul2   sim(agent);
    sim.getSimulFromType(wm,(ObjectT)(OBJECT_TEAMMATE_1+nr-1),SIM_PLAYER);

    double     time=sim.dashTimeTo(toVecPos(end))+avans+0.3755+0.85;

    //  S*(1-q)/(1-pow(q,n))

//    double dirS=doubleNormalize(2.3/*finalMaxSpeed*/*0.94/*ballDecay*/+start.dist(end)*0.06/*1-ballDecay*/,0.0,3.0);

    double a=doubleNormalize(start.dist(end)*(0.06)/(1-pow(0.94,time)),0.0,3.0);

//    a = min(dirS,a);

    double b=doubleNormalize(0.4/*finalMinSpeed*/*0.94/*ballDecay*/+start.dist(end)*0.06/*1-ballDecay*/,0.0,3.0);

    speed.max=max(a,b);
    speed.min=min(a,b);
    return speed;



}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

spd      MostafPass::getThroughSpeed(PlayerAgent * agent,int & nr,Vector2D & start,Vector2D & end,int & step,int &time,int avans){

    const WorldModel & wm = agent->world();
    spd speed;

    if(!wm.ourPlayer(nr)){
        speed.max=0.0;
        speed.min=0.0;
        return speed;
    }
    Simul2   sim(agent);
    sim.getSimulFromType(wm,(ObjectT)(OBJECT_TEAMMATE_1+nr-1),SIM_PLAYER);

//    int     time;//=sim.dashTimeTo(end);

    if(wm.ourPlayer(nr)->bodyCount()<2)
        time=sim.timeTo(toVecPos(end))+avans/*+1*/;
    else
        time=sim.dashTimeTo(toVecPos(end))+avans;

//    ++time;
    //  S*(1-q)/(1-pow(q,n))

    double a=doubleNormalize(start.dist(end)*(0.06)/(1-pow(0.94,time/*+0.5*/)),0.0,3.0);
    double b=doubleNormalize(0.3/*MinfinalSpeed*/*0.94/*ballDecay*/+start.dist(end)*0.06/*1-ballDecay*/,0.0,3.0);

    speed.max=max(a,b);
    speed.min=min(a,b);
    return speed;

}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


bool	MostafPass::directPass(PlayerAgent * agent,NewKick &kicking,int &nr,bool better){

    const WorldModel & wm = agent->world();

    Vector2D	agentPos=wm.self().pos();
    int		agentNum=wm.self().unum();
    Vector2D posBall=wm.ball().pos();
    double	agentScore=flagScoring(agent,toVecPos(posBall),toVecPos(agentPos));
    double  agentScore2=calcScore(agent,agentPos)*SIXFACTOR;

    struct structer{
//        bool		can;
        Vector2D	pos;
        Vector2D    start;
        double      maxSpeed;
        int			step;
        int			conf;
        int			num;
        double  	score;
        double      rand;
        NewKick     kick;
    } points[30];

    int k=0;

    /// player Unum
    for( int i=2 ; i < 12 ;	i++)
        if(agentNum!=i && wm.ourPlayer(i) && wm.ourPlayer(i)->posCount()<3 && getCount(wm,wm.ourPlayer(i)->pos(),5)<2 && isInField(wm.ourPlayer(i)->pos())){
        for (int j = 1; j <= 3; j++)
        {

            points[k].pos=wm.ourPlayer(i)->pos()+wm.ourPlayer(i)->vel();

            if(points[k].pos.x < -35 && fabs(points[k].pos.y)<15)
                continue;

            points[k].num=i;

//            if(wm.ourPlayer(i)->bodyCount()<2)
//                points[k].pos+=VecPosition(0.5,wm.ourPlayer(i)->body().degree(),POLAR);

            spd A=getDirectSpeed(agentPos,points[k].pos);

            Body_SmartKick smart(points[k].pos,A.max,A.min,j);

            if(!smart.execute(agent))
                continue;
            if( j != smart.sequence().pos_list_.size() )
                continue;

//            points[k].can=true;

            points[k].step = j;


            points[k].conf=getCount(wm,points[k].pos,calculate_PM(wm,agentPos,points[k].pos,smart.sequence().speed_,0.15));

            if(  points[k].conf>1  ){

                continue;
            }

            Simul2 a;
            setKick(wm,points[k].kick,smart);
            points[k].start=startKick(wm,smart);
            points[k].maxSpeed=smart.sequence().speed_;
//            points[k].step=stepKick(smart);
            points[k].rand=kickRand(agent,smart);
            //points[k].conf=getCount(wm,wm.ourPlayer(i)->pos(),5);
//            cout<<"max speed"<<points[k].start.getDistanceTo(points[k].pos)<<endl;
//            cout<<"n : "<<a.N(points[k].start.getDistanceTo(points[k].pos),points[k].maxSpeed,0.94,true)<<endl;
            points[k].score=flagScoring(agent,toVecPos(points[k].pos),toVecPos(agentPos))*pow(XFACTOR,-2+a.N(points[k].start.dist(points[k].pos),points[k].maxSpeed,0.94,true));

            if( agentScore > points[k].score && agentScore2 > calcScore(agent,points[k].pos) )
                continue;



            k++;
        }
    }




    for (int i = 0; i < k; i++)
        for (int j=i+1; j < k; j++)
            if (points[i].score<points[j].score){
//        boolSwap(points[i].can,points[j].can);
        intSwap(points[i].num,points[j].num);
        intSwap(points[i].step,points[j].step);
        intSwap(points[i].conf,points[j].conf);
        vecSwap(points[i].pos,points[j].pos);
        vecSwap(points[i].start,points[j].start);
        doubleSwap(points[i].score,points[j].score);
        doubleSwap(points[i].rand,points[j].rand);
        doubleSwap(points[i].maxSpeed,points[j].maxSpeed);
        kickSwap(points[i].kick,points[j].kick);
    }


    for (int i = 0; i < k; i++)
    {
//        if( better==false || agentScore<points[i].score ||
//                agentScore2 < calcScore(agent,points[i].pos))
        {
            if( points[i].pos.dist(agentPos)>3 /** set && points[i].oppInCircle==0*/ ){

                if(/*points[i].can &&*/
                    ( ( Shoot().OCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,1,0)&& (cout<<"yes 11111111111111"<<endl) )  ||
                      ( OCI(agent,toVecPos(points[i].start),toVecPos(points[i].pos),points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,1) && (cout<<"yes 2222222222222"<<endl) )  ||
                        ( IOOCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,points[i].kick,6,0.3,points[i].step,points[i].rand/2,true,1) && (cout<<"yes boyyyyyyyyyyyyyy"<<endl) )) ){
                    kicking=points[i].kick;
                    kicking.reciver = points[i].num;
                    kicking.reciverUnum = points[i].num;
                    kicking.reciverVector = points[i].pos - wm.ourPlayer(points[i].num)->pos();


                    nr=points[i].num;
                    return true;
                }

            }
        }
    }


    return false;
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


bool	MostafPass::goalieDirectPass(PlayerAgent * agent,NewKick &kicking,int &nr,bool better){

    const WorldModel & wm = agent->world();

    Vector2D	agentPos=wm.self().pos();
    int         agentNum=wm.self().unum();
    Vector2D    posBall=wm.ball().pos();
    double      agentScore=flagScoring(agent,toVecPos(posBall),toVecPos(agentPos));


    struct structer{
        bool		can;
        Vector2D	pos;
        Vector2D    start;
        double      maxSpeed;
        int			step;
        int			conf;
        int			num;
        double  	score;
        double      rand;
        NewKick     kick;
    } points[30];

    int k=0;

    /// player Unum
    for( int i=2 ; i < 12 ;	i++)
        if(agentNum!=i && wm.ourPlayer(i) && wm.ourPlayer(i)->posCount()<3 && getCount(wm,wm.ourPlayer(i)->pos(),5)<2 && isInField(wm.ourPlayer(i)->pos())){
            cout<<"i :"<<i<<endl;
        for (int j = 1; j <= 1; j++)
        {
            points[k].num=i;
            points[k].pos=wm.ourPlayer(i)->pos()+wm.ourPlayer(i)->vel();

//            if(wm.ourPlayer(i)->bodyCount()<2)
//                points[k].pos+=VecPosition(0.5,wm.ourPlayer(i)->body().degree(),POLAR);

            spd A=getDirectSpeed(agentPos,points[k].pos);
            A.min = 1.5;

            Body_SmartKick smart(points[k].pos,A.max,A.min,j);


            points[k].can=smart.execute(agent);
            if(!points[k].can){
               cout<<__LINE__<<"  i :"<<i<<endl;
                continue;

            }
            points[k].step = j;
            if( points[k].step != stepKick(smart) ){

                cout<<__LINE__<<"  i :"<<i<<endl;
                continue;
            }


            points[k].conf=getCount(wm,points[k].pos,calculate_PM(wm,agentPos,points[k].pos,smart.sequence().speed_,0.15));

            if(  points[k].conf>1  ){

                cout<<__LINE__<<"  i :"<<i<<endl;
                continue;
            }

            Simul2 a;
            setKick(wm,points[k].kick,smart);
            points[k].start=startKick(wm,smart);
            points[k].maxSpeed=smart.sequence().speed_;
            points[k].step=stepKick(smart);
            points[k].rand=kickRand(agent,smart);
            //points[k].conf=getCount(wm,wm.ourPlayer(i)->pos(),5);
//            cout<<"max speed"<<points[k].start.getDistanceTo(points[k].pos)<<endl;
//            cout<<"n : "<<a.N(points[k].start.getDistanceTo(points[k].pos),points[k].maxSpeed,0.94,true)<<endl;
            points[k].score=flagScoring(agent,toVecPos(points[k].pos),toVecPos(agentPos))*pow(XFACTOR,-2+a.N(points[k].start.dist(points[k].pos),points[k].maxSpeed,0.94,true));
            k++;
        }
    }




    for (int i = 0; i < k; i++)
        for (int j=i+1; j < k; j++)
            if (points[i].score<points[j].score){
        boolSwap(points[i].can,points[j].can);
        intSwap(points[i].num,points[j].num);
        intSwap(points[i].step,points[j].step);
        intSwap(points[i].conf,points[j].conf);
        vecSwap(points[i].pos,points[j].pos);
        vecSwap(points[i].start,points[j].start);
        doubleSwap(points[i].score,points[j].score);
        doubleSwap(points[i].rand,points[j].rand);
        doubleSwap(points[i].maxSpeed,points[j].maxSpeed);
        kickSwap(points[i].kick,points[j].kick);
    }


    cout<<"k :  "<<k<<endl;
    for (int i = 0; i < k; i++)
    {
        if( better==false || agentScore<points[i].score )
        {
            if( points[i].pos.dist(agentPos)>3 /** set && points[i].oppInCircle==0*/ ){

                if(points[i].can &&
                    ( ( Shoot().OCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,1,0)&& (cout<<"yes 11111111111111"<<endl) )  ||
                      ( OCI(agent,toVecPos(points[i].start),toVecPos(points[i].pos),points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,1) && (cout<<"yes 2222222222222"<<endl) )  ||
                        ( IOOCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,points[i].kick,6,0.3,points[i].step,points[i].rand/2,true,1) && (cout<<"yes boyyyyyyyyyyyyyy"<<endl) )) ){
                    kicking=points[i].kick;
                    kicking.reciver = points[i].num;
                    kicking.reciverUnum = points[i].num;
                    kicking.reciverVector = points[i].pos - wm.ourPlayer(points[i].num)->pos();


                    nr=points[i].num;
                    return true;
                }

            }
        }
    }


    return false;
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool        MostafPass::isInField(Vector2D pos,double xend,double yend){

    if( (-1* yend) <pos.y && pos.y < yend && -48<pos.x && pos.x<xend)
        return true;

    return false;



}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

void        MostafPass::setInField(Vector2D &pos,double maxX){

    if( fabs(pos.y) > 32.0 )
        pos.y = ( 32.0 * (fabs(pos.y )/pos.y) ) ;

    if (fabs( pos.x ) >maxX )
        pos.x = ( maxX * (fabs(pos.x )/pos.x) ) ;

    return ;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

void        MostafPass::setInField(VecPosition &pos,double maxX){

    if( fabs(pos.getY()) > 33.0 )
        pos.setY( 33.0 * (fabs(pos.getY() )/pos.getY()) ) ;

    if (fabs( pos.getX() ) >maxX )
        pos.setX( maxX * (fabs(pos.getX() )/pos.getX()) ) ;

    return ;
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

int MostafPass::timeToReach(double speed,double dis,int & time){


    if(dis>(speed - 0.3)/0.06)
        return 100.0;

    double X=1-(dis*(0.06)/speed);

    time =ceil((log10(X))/(log10(0.94))) - time;

    return time;


}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool        MostafPass::throughPass(PlayerAgent * agent,NewKick &kicking,int &nr,bool better){

    const WorldModel & wm = agent->world();

    Vector2D posAgent=wm.self().pos();
    Vector2D posBall=wm.ball().pos();
    if(posAgent.x > 32 || posAgent.x < -32 )
        return false;

    double	agentScore=flagScoring(agent,toVecPos(posBall),toVecPos(posAgent));


    double  agentScore2=calcScore(agent,posAgent)*SIXFACTOR;


    struct structer{

//        bool			can;
        Vector2D		pos;
        Vector2D        start;
        double			maxSpeed;
        int             step;
        int             conf;
        int             num;
        double			score;
        double          rand;
        NewKick         kick;

    } points[486];

    int k=0;

    double  TG = tanDeg(15);

    /// player Unum
    for(int i=9 ; i<12 ; i++){


        if(!wm.ourPlayer(i) || i == wm.self().unum() || wm.ourPlayer(i)->posCount() > 4 || wm.ourPlayer(i)->pos().x > 45 )
            continue;

        Vector2D posTeammate=wm.ourPlayer(i)->pos();

        for(int j=0;j<18;j++){

            for(int f=0;f<3;f++){

                Vector2D posPass = posTeammate + Vector2D( j*1.666666667/**5.0*/ + 10.0 , (TG-(TG*f)) * (j*/*5.0*/1.666666667 + 10.0) );
                if(!isInField(posPass,45.0,33))
                    continue;


                /// step
                for(int s=1;s<=3;s++){

                    points[k].pos = posPass;


                    int time = 1;
                    spd A = getThroughSpeed(agent,i,posAgent,points[k].pos,s,time,0);

                    Body_SmartKick smart(points[k].pos ,A.max,A.min, points[k].step);

                    if(!smart.execute(agent))
                        continue;

                    if( s != smart.sequence().pos_list_.size() )
                        continue;


                    points[k].conf = getCount (wm,points[k].pos,calculate_PM(wm,posAgent,points[k].pos,smart.sequence().speed_,0.15));


                    if(  points[k].conf>2  ){

                        continue;
                    }

                    setKick(wm,points[k].kick,smart);

                    points[k].step = s;
                    points[k].num = i;
                    points[k].start=startKick(wm,smart);
                    points[k].maxSpeed=smart.sequence().speed_;
                    points[k].step=stepKick(smart);
                    points[k].rand=kickRand(agent,smart);
    //                    Simul2 a;
                    points[k].score=flagScoring(agent,toVecPos(points[k].pos),toVecPos(posAgent))/**pow(XFACTOR,-2+a.N(points[k].start.getDistanceTo(points[k].pos),points[k].maxSpeed,0.94,true))*/;
    //                    points[k].timing = timeToReach(smart.sequence().speed_ , points[k].pos.getDistanceTo(posAgent),time);
    //cout<<"                                "<<points[k].timing<<endl;


                    if( agentScore > points[k].score && agentScore2 > calcScore(agent,points[k].pos) )
                        continue;

                    k++;
                }
            }
        }
    }

    for (int i = 0; i < k; i++)
        for (int j=i+1; j < k; j++)
            if (points[i].score<points[j].score){
//        boolSwap(points[i].can,points[j].can);
        intSwap(points[i].num,points[j].num);
        intSwap(points[i].step,points[j].step);
        intSwap(points[i].conf,points[j].conf);
        vecSwap(points[i].pos,points[j].pos);
        vecSwap(points[i].start,points[j].start);
        doubleSwap(points[i].score,points[j].score);
        doubleSwap(points[i].rand,points[j].rand);
        doubleSwap(points[i].maxSpeed,points[j].maxSpeed);
        kickSwap(points[i].kick,points[j].kick);
//        intSwap(points[i].timing ,points[j].timing );
    }

    for (int i = 0; i < k; i++)
    {
//        if( better==false || agentScore<points[i].score||
//                agentScore2 < calcScore(agent,points[i].pos))
        {
            if( points[i].pos.dist(posAgent)>3 /** set && points[i].oppInCircle==0*/ ){
                if(/*points[i].can &&*/
                    ( ( Shoot().OCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,0,2) && (cout<<"yes 111111111111111"<<endl) ) ||
                      ( OCI(agent,toVecPos(points[i].start),toVecPos(points[i].pos),points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,2)) && (cout<<"yes 22222222222222222"<<endl)  ||
                        ( IOOCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,points[i].kick,6,0.3,points[i].step,points[i].rand/2,true,5) && (cout<<"yes boyyyyyyyyyyyyyy"<<endl) )) ){

                    kicking=points[i].kick;
                    kicking.reciver = points[i].num;
                    kicking.reciverUnum = points[i].num;
                    kicking.reciverVector = points[i].pos - wm.ourPlayer(points[i].num)->pos();

                    nr=points[i].num;
                    return true;
                }

            }
        }
    }

        return false;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool        MostafPass::leadingPass(PlayerAgent * agent,NewKick &kicking,int &nr,bool better){

    const WorldModel & wm = agent->world();

//    if(wm.ball().pos().x<-30)
//        return false;
    Vector2D posBall=wm.ball().pos();
    Vector2D posAgent=wm.self().pos();
    double	agentScore=flagScoring(agent,toVecPos(posBall),toVecPos(posAgent));
    double  agentScore2=calcScore(agent,posAgent)*SIXFACTOR;




    struct structer{
//        bool			can;
        Vector2D		pos;
        Vector2D        start;
        double			maxSpeed;
        int			step;
        int			conf;
        int			num;
        double      score;
        double      rand;
        NewKick     kick;
    } points[300];

    int k=0;

    /// player Unum
    for(int i=2 ; i<12 ; i++){


        if( !wm.ourPlayer(i) || i==wm.self().unum() || wm.ourPlayer(i)->seenPosCount() >0 || getCount(wm,wm.ourPlayer(i)->pos(),5) > 1 /*|| (wm.ourPlayer(i)->bodyCount()!=0 && wm.ourPlayer(i)->pos().dist(wm.self().pos())<40)*/)
            continue;

        int av=1;

        if(i<7)
            av=2;

        Vector2D posTeammate = wm.ourPlayer(i)->pos() + wm.ourPlayer(i)->vel();

        if(posTeammate.x < -30 && posTeammate.absY() <18 )
            continue;

        double dir=0;
        if(wm.ourPlayer(i)->bodyCount()==0)
            dir=wm.ourPlayer(i)->body().degree();
        else
            av++;

        for(int j=0;j<10;j++){

            Vector2D posPass = posTeammate+Vector2D::polar2vector(j+1,dir);

            if( isInField(posPass,48,31)==false   ){

                continue;
            }

            /// step
            for(int s=1;s<=3;s++){


                points[k].num=i;
                points[k].pos=posPass;
                points[k].step=s;

                spd A=getLeadSpeed(agent,points[k].num,posAgent,points[k].pos,points[k].step,av);
                Body_SmartKick smart(points[k].pos,A.max,A.min,points[k].step);

                if(!smart.execute(agent))
                    continue;

                if( s != smart.sequence().pos_list_.size() )
                    continue;

                if(smart.sequence().speed_ < 0.9 )
                    continue;

                points[k].conf=getCount(wm,points[k].pos,calculate_PM(wm,posAgent,points[k].pos,smart.sequence().speed_,0.15));

                if(  points[k].conf!=0  ){

                    continue;
                }

                setKick(wm,points[k].kick,smart);
                points[k].start=startKick(wm,smart);
                points[k].maxSpeed=smart.sequence().speed_;


                points[k].step=stepKick(smart);
                points[k].rand=kickRand(agent,smart);
                Simul2 a;
                points[k].score=flagScoring(agent,toVecPos(points[k].pos),toVecPos(posAgent))*pow(XFACTOR,max(-5+a.N(points[k].start.dist(points[k].pos),points[k].maxSpeed,0.94,true),0));

                if( agentScore > points[k].score && agentScore2 > calcScore(agent,points[k].pos) )
                    continue;


                k++;
            }
        }
    }


    for (int i = 0; i < k; i++)
        for (int j=i+1; j < k; j++)
            if (points[i].score<points[j].score){
//        boolSwap(points[i].can,points[j].can);
        intSwap(points[i].num,points[j].num);
        intSwap(points[i].step,points[j].step);
        intSwap(points[i].conf,points[j].conf);
        vecSwap(points[i].pos,points[j].pos);
        vecSwap(points[i].start,points[j].start);
        doubleSwap(points[i].score,points[j].score);
        doubleSwap(points[i].rand,points[j].rand);
        doubleSwap(points[i].maxSpeed,points[j].maxSpeed);
        kickSwap(points[i].kick,points[j].kick);
    }

    for (int i = 0; i < k; i++)
    {
//        if( better==false || agentScore<points[i].score||
//                agentScore2 < calcScore(agent,points[i].pos))
        {
            if( points[i].pos.dist(posAgent)>3 /** set && points[i].oppInCircle==0*/ ){

                if(/*points[i].can &&*/
                    ( ( Shoot().OCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,1,2) && (cout<<"yes 222222222222222"<<endl) ) ||
                      ( OCI(agent,toVecPos(points[i].start),toVecPos(points[i].pos),points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,3) ) && (cout<<"yes 22222222222222"<<endl)  ||
                        ( IOOCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,points[i].kick,6,0.3,points[i].step,points[i].rand/2,true,1) && (cout<<"yes boyyyyyyyyyyyyyy"<<endl) )) ){

                    kicking=points[i].kick;
                    kicking.reciver = points[i].num;
                    kicking.reciverUnum = points[i].num;
                    kicking.reciverVector = points[i].pos - wm.ourPlayer(points[i].num)->pos();


                    nr=points[i].num;
                    return true;
                }

            }
        }
    }


        return false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//double	MostafPass::posScoreInFieldForUs(VecPosition pos){

//    return ((110.368-VecPosition(52.5,0.0).getDistanceTo(pos))+((34-fabs(pos.getY()))/2.5)+((pos.getX()+52.5)/1.12))/2.194225455;
//    //return ((110.368-VecPosition(52.5,0.0).getDistanceTo(pos))+((34-fabs(pos.getY()))/4.5)+((pos.getX()+70.5)/1.0))/2.194225455;

//}
//double	MostafPass::getCofficient(double Ang){

//    AngDeg	temp=Ang/2;

//    return ((cosDeg(temp))+1)/2;

//}
//double          MostafPass::FinalMostafScoring(VecPosition pos,VecPosition inform,int minimalConf){

//    const WorldModel & wm = agent->world();

//    double scorePos = posScoreInFieldForUs(pos);

//    scorePos = WM->reRate(scorePos,0,108.520297883,0,120);


//    double dis = (inform.getX()+52.5);

//    dis = WM->reRate(dis,0,105,0,100);

//    dis = 110 - dis;
//    dis /= 8.33 ;

//    //dis = WM->reRate(dis , 1.2 , 9 , 3 , 12 );

//    double congScore = 0;

//    VecPosition	temp=VecPosition(55.0,0.0);
//    if(pos.getX()<20)temp=VecPosition(55,temp.getY());

//    double dirToGoal=(temp-pos).getDirection();

//    bool is=false;

//    for (int i = 1; i < 12; i++)
//        if((1-WM->getConfidence(WM->objOpponent(i)))*100<=minimalConf){
//        if(pos.getDistanceTo(WM->posOpponent(i))<dis)
//            congScore+=dis-((pos.getDistanceTo(WM->posOpponent(i)))*getCofficient(VecPosition::normalizeAngle(fabs((dirToGoal-((WM->posOpponent(i))-pos).getDirection())))));
//        if(pos.getDistanceTo((WM->posOpponent(i)))<2.0)
//            is=true;
//    }

//    congScore = WM->reRate(congScore,0,dis*0.75,0,100);

//    congScore = 100 -congScore;

//    congScore = WM->reRate(congScore,0,100,0,200);

//    if(is)congScore*=0.5;

//    return congScore + scorePos ;

//}


VecPosition MostafPass::turnPoint( PlayerAgent * agent,int ballMinConf )
{
    const WorldModel & wm = agent->world();

    VecPosition posAgent  = toVecPos(wm.self().pos());
    VecPosition posBall   = toVecPos(wm.ball().pos());
    AngDeg      bodyAngle = wm.self().body().degree();

    double view=wm.self().viewWidth();

//    switch( wm.self().viewWidth() )
//    {
//            case ViewWidth::NARROW:
//                    view = 60.0;
//                    break;
//            case ViewWidth::NORMAL:
//                    view = 120.0;
//                    break;
//            case ViewWidth::WIDE:
//                    view = 180.0;
//                    break;
//            default:
//                    view = 60.0;
//    }

    if( wm.ball().posCount() > ballMinConf && getAngDiff( bodyAngle, ( posBall - posAgent ).getDirection() ) < 90.0 + view )
    {
            return posBall;
    }

    /*ObjectT oppGoal = ( WM->getSide() == SIDE_LEFT ) ? OBJECT_GOAL_R : OBJECT_GOAL_L;

    if( WM->getClosestInSetTo( OBJECT_SET_PLAYERS, OBJECT_BALL ) == WM->getAgentObjectType() &&
        WM->getConfidence( oppGoal ) <= ballMinConf && getAngDiff( bodyAngle, ( WM->getGlobalPosition( oppGoal ) - posAgent ).getDirection() ) < 90.0 + view )
    {
            return WM->getGlobalPosition( oppGoal );
    }*/

    double step = 15.0;

    double start = VecPosition::normalizeAngle( bodyAngle - 90.0 );

    multimap <int, double> angConf;
    multimap <int, double>::reverse_iterator rit;
    multimap <int, double>::reverse_iterator rit2;

    for( int i = 0; i <= (int)(180.0/step); i++ )
    {
            AngDeg ang = VecPosition::normalizeAngle( start + i*step );

            angConf.insert( pair<int,double>(wm.dirCount( ang ), ang) );
    }

    vector <AngDeg> angs;

    for( rit = angConf.rbegin(); rit != angConf.rend(); rit++ )
    {
            bool good = true;
            bool BREAK = false;
            for( rit2 = rit;; rit2-- )
            {
                    if( rit2 == angConf.rbegin() )
                            BREAK=true;

                    // Log.log( 250,"diff between : %lf va %lf = %lf",rit->second,rit2->second, getAngDiff( rit->second, rit2->second ) );

                    if( getAngDiff( rit->second, rit2->second ) > view )
                    {
                            good = false;
                            break;
                    }

                    if( BREAK ) break;
            }

            if( !good )
            {
                    break;
            }

            // Log.log( 250,"push_back : %lf conf : %d",rit->second,rit->first );

            angs.push_back( rit->second );
    }

    sort(angs.begin(),angs.end());

    // Log.log( 250,"front : %lf back : %lf",angs.front(),angs.back() );

    double finalAng1 = getAngBetween( angs.front(), angs.back() );
    double finalAng2 = getAngBetween( angs.back(), angs.front() );

    double finalAng = ( getAngDiff( finalAng1, bodyAngle ) < getAngDiff( finalAng2, bodyAngle ) ) ? finalAng1 : finalAng2;

    // Log.log( 250,"finalAng : %lf ",finalAng );
    // Log.log( 250,"finalAng2 : %lf ",finalAng2 );
    // Log.log( 250,"body : %lf ",bodyAngle );
    // Log.log( 250,"diff1 : %lf ",getAngDiff( finalAng1, bodyAngle ) );
    // Log.log( 250,"diff2 : %lf ",getAngDiff( finalAng2, bodyAngle ) );

    return posAgent + VecPosition( 5.0, finalAng ,POLAR );
}
double MostafPass::getAngBetween( double ang1, double ang2, double n1, double n2 )
{
        // clockwise

        ang1 = VecPosition::normalizeAngle(ang1);
        ang2 = VecPosition::normalizeAngle(ang2);

        n1 = fabs(n1);
        n2 = fabs(n2);


        if( ang1 <= ang2 )
                return VecPosition::normalizeAngle( ((ang1*n1)+(ang2*n2))/(n1+n2) );
        else
        {
                ang2 += 360.0;
                return VecPosition::normalizeAngle( ((ang1*n1)+(ang2*n2))/(n1+n2) );
        }
}


Vector2D MostafPass::seen(PlayerAgent *agent){


    const WorldModel & wm = agent->world();

    vector <Vector2D> pos;
    vector <int > conf;



    double myX=min(wm.self().pos().x-7.0,33.0);
    for(int i=2;i<12;i++)
        if(wm.ourPlayer(i)){




            if(Strategy::i().getPosition(i).x>=myX){
                pos.push_back(Strategy::i().getPosition(i));
                conf.push_back(getCount(wm,pos.at(pos.size()-1),7));

            }

            if(i>8){
                pos.push_back(Strategy::i().getPosition(i)+Vector2D(12,0));
                conf.push_back(getCount(wm,pos.at(pos.size()-1),7));

            }
        }



    for (int i = 0; i < pos.size(); i++)
        for (int j=i+1; j < pos.size(); j++)
            if(conf.at(i)<conf.at(j)){
                intSwap(conf.at(i),conf.at(j));
                vecSwap(pos.at(i),pos.at(j));
            }

    double bodyAng=wm.self().body().degree();
    for (int i = 0; i < pos.size(); i++);
        //if()


}


Vector2D    MostafPass::where(PlayerAgent *agent, int ballMinConf){

    const WorldModel  & wm  = agent->world();

//    View_ChangeWidth(ViewWidth::NARROW).execute(agent);
    if(wm.self().viewWidth()!=ViewWidth::NARROW)
        agent->doChangeView( ViewWidth::NARROW );


    if( wm.ball().posCount() >= ballMinConf )
        if(fabs(AngleDeg::normalize_angle(((wm.ball().pos()-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107)
            return wm.ball().pos();

    int player[10][7]={ {3,4,6,7,8,9,11} ,//2
                        {2,5,6,7,8,10,11} ,//3
                        {2,3,6,7,8,9,11} ,//4
                        {2,3,6,7,8,10,11} ,//5
                        {7,8,9,10,11} ,//6
                        {6,8,9,11} ,//7
                        {6,7,10,11} ,//8
                        {6,7,8,10,11} ,//9
                        {6,7,8,9,11} ,//10
                        {7,8,9,10}//11
                      };





    struct stc{
        Vector2D    pos;
        int         conf;

        stc(){

            pos = Vector2D(100,100);
            conf= 22771;
        }
    } info[15];

    int selfNr = wm.self().unum() - 2 ;
    int nr = 0 ;


    bool goalChance = false;

    if(wm.ball().pos().x > 36  && fabs(wm.ball().pos().y) < 16 )
        goalChance = true;

    for (int i = 0; i < 7; ++i) {

        if(player[selfNr][i]!=0 &&player[selfNr][i]!=wm.self().unum() && wm.ourPlayer(player[selfNr][i]) ){

            info[nr].pos = wm.ourPlayer(player[selfNr][i])->pos();


            if(goalChance && ( info[nr].pos.x<36 ||  fabs(info[nr].pos.y)>14 ) )
                continue;

            if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){

                info[nr].conf= getCount( wm , info[nr].pos , 5 );

                if(wm.ourPlayer(player[selfNr][i])->bodyCount()<2){

                    Vector2D forward = Vector2D::polar2vector(2.5,wm.ourPlayer(player[selfNr][i])->body().degree());
                    if(fabs(AngleDeg::normalize_angle(((info[nr].pos+forward-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){

                        info[nr].pos += forward;
                        info[nr].conf= getCount( wm , info[nr].pos , 5 );

                    }


                }

                nr++;
            }
            else if(wm.ourPlayer(player[selfNr][i])->bodyCount()<2){

                Vector2D forward = Vector2D::polar2vector(2.5,wm.ourPlayer(player[selfNr][i])->body().degree());
                if(fabs(AngleDeg::normalize_angle(((info[nr].pos+forward-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){

                    info[nr].pos += forward;
                    info[nr].conf= getCount( wm , info[nr].pos , 5 );

                }


            }




            if(player[selfNr][i] > 8 && wm.ball().pos().x < 32 && wm.ball().pos().x > -20 ){


                info[nr].pos = wm.ourPlayer(player[selfNr][i])->pos() + Vector2D(14.50 ,0.0  );



                if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
                    info[nr].conf= getCount( wm , info[nr].pos, 5 );
                    nr++;
                }


            }
            if(player[selfNr][i] > 8 && wm.ball().pos().x < 15 && wm.ball().pos().x > -20 ){


                info[nr].pos = wm.ourPlayer(player[selfNr][i])->pos() + Vector2D(25.0 ,0.0  );
                if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
                    info[nr].conf= getCount( wm , info[nr].pos, 5 );
                    nr++;
                }


            }
        }
    }

    info[nr].pos = Vector2D(52.0 ,0.0  );
    if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
        info[nr].conf= getCount( wm , info[nr].pos, 5 );

        if(wm.self().pos().dist(Vector2D(52.5,0.0))<17)
            info[nr].conf*=2.5;
        nr++;
    }
    info[nr].pos = Vector2D(52.0 ,5.0  );
    if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
        info[nr].conf= getCount( wm , info[nr].pos, 5 );
        if(wm.self().pos().dist(Vector2D(52.5,0.0))<17)
            info[nr].conf*=2.5;
        nr++;
    }
    info[nr].pos = Vector2D(52.0 ,-5.0  );
    if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
        info[nr].conf= getCount( wm , info[nr].pos, 5 );
        if(wm.self().pos().dist(Vector2D(52.5,0.0))<17)
            info[nr].conf*=2.5;
        nr++;
    }

    for (int i = 0; i < nr; ++i) {
        for (int j = i+1; j < nr ; ++j) {
            if(info[i].conf<info[j].conf){
                intSwap(info[i].conf,info[j].conf);
                vecSwap(info[i].pos,info[j].pos);
            }

        }

    }

//    cout<<"cycle:"<<wm.time().cycle()<<"  nr:"<<wm.self().unum()<<"  ViewAction:"<<wm.self().viewWidth()<<"  conf:"<<info[0].conf<<"  to:"<<info[0].pos.x<<" , "<<info[0].pos.y<<endl;

    return info[0].pos;

}


/*
Vector2D    MostafPass::where(PlayerAgent *agent, int ballMinConf){

    const WorldModel  & wm  = agent->world();

//    View_ChangeWidth(ViewWidth::NARROW).execute(agent);
//    if(wm.self().viewWidth()!=ViewWidth::NARROW)
//        agent->doChangeView( ViewWidth::NARROW );


    if( wm.ball().posCount() >= ballMinConf )
        if(fabs(AngleDeg::normalize_angle(((wm.ball().pos()-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107)
            return wm.ball().pos();

    int player[10][7]={ {3,4,6,7,8,9,11} ,//2
                        {2,5,6,7,8,10,11} ,//3
                        {2,3,6,7,8,9,11} ,//4
                        {2,3,6,7,8,10,11} ,//5
                        {7,8,9,10,11} ,//6
                        {6,8,9,11} ,//7
                        {6,7,10,11} ,//8
                        {6,7,8,10,11} ,//9
                        {6,7,8,9,11} ,//10
                        {7,8,9,10}//11
                      };





    struct stc{
        Vector2D    pos;
        int         conf;

        stc(){

            pos = Vector2D(100,100);
            conf= 22771;
        }
    } info[15];

    int selfNr = wm.self().unum() - 2 ;
    int nr = 0 ;


    bool goalChance = false;

    if(wm.ball().pos().x > 40  && fabs(wm.ball().pos().y) < 16 )
        goalChance = true;

    for (int i = 0; i < 7; ++i) {

        if(player[selfNr][i]!=0 &&player[selfNr][i]!=wm.self().unum() && wm.ourPlayer(player[selfNr][i]) ){

            info[nr].pos = wm.ourPlayer(player[selfNr][i])->pos();


            if(goalChance && ( info[nr].pos.x<40 ||  fabs(info[nr].pos.y)>14 ) )
                continue;

            if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){

                info[nr].conf= getCount( wm , info[nr].pos , 5 );

                if(wm.ourPlayer(player[selfNr][i])->bodyCount()<2){

                    Vector2D forward = Vector2D::polar2vector(2.5,wm.ourPlayer(player[selfNr][i])->body().degree());
                    if(fabs(AngleDeg::normalize_angle(((info[nr].pos+forward-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){

                        info[nr].pos += forward;
                        info[nr].conf= getCount( wm , info[nr].pos , 5 );

                    }


                }

                nr++;
            }
            else if(wm.ourPlayer(player[selfNr][i])->bodyCount()<2){

                Vector2D forward = Vector2D::polar2vector(2.5,wm.ourPlayer(player[selfNr][i])->body().degree());
                if(fabs(AngleDeg::normalize_angle(((info[nr].pos+forward-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){

                    info[nr].pos += forward;
                    info[nr].conf= getCount( wm , info[nr].pos , 5 );

                }


            }




            if(player[selfNr][i] > 8 && wm.ball().pos().x < 32 && wm.ball().pos().x > -20 ){


                info[nr].pos = wm.ourPlayer(player[selfNr][i])->pos() + Vector2D(14.50 ,0.0  );



                if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
                    info[nr].conf= getCount( wm , info[nr].pos, 5 );
                    nr++;
                }


            }
            if(player[selfNr][i] > 8 && wm.ball().pos().x < 15 && wm.ball().pos().x > -20 ){


                info[nr].pos = wm.ourPlayer(player[selfNr][i])->pos() + Vector2D(25.0 ,0.0  );
                if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
                    info[nr].conf= getCount( wm , info[nr].pos, 5 );
                    nr++;
                }


            }
        }
    }

    info[nr].pos = Vector2D(52.0 ,0.0  );
    if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
        info[nr].conf= getCount( wm , info[nr].pos, 5 );

        if(wm.self().pos().dist(Vector2D(52.5,0.0))<17)
            info[nr].conf*=2.5;
        nr++;
    }
    info[nr].pos = Vector2D(52.0 ,5.0  );
    if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
        info[nr].conf= getCount( wm , info[nr].pos, 5 );
        if(wm.self().pos().dist(Vector2D(52.5,0.0))<17)
            info[nr].conf*=2.5;
        nr++;
    }
    info[nr].pos = Vector2D(52.0 ,-5.0  );
    if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
        info[nr].conf= getCount( wm , info[nr].pos, 5 );
        if(wm.self().pos().dist(Vector2D(52.5,0.0))<17)
            info[nr].conf*=2.5;
        nr++;
    }

    for (int i = 0; i < nr; ++i) {
        for (int j = i+1; j < nr ; ++j) {
            if(info[i].conf<info[j].conf){
                intSwap(info[i].conf,info[j].conf);
                vecSwap(info[i].pos,info[j].pos);
            }

        }

    }

//    cout<<"cycle:"<<wm.time().cycle()<<"  nr:"<<wm.self().unum()<<"  ViewAction:"<<wm.self().viewWidth()<<"  conf:"<<info[0].conf<<"  to:"<<info[0].pos.x<<" , "<<info[0].pos.y<<endl;

    return info[0].pos;

}

*/



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
bool    MostafPass::getClearDir(PlayerAgent *agent,int minConf){

    const WorldModel & wm = agent->world();

    vector <AngleDeg> dir;
    vector <int> nr;
    vector <Vector2D > position;

    Vector2D    selfPos=wm.self().pos();
    Vector2D    ballPos=wm.ball().pos();

    double dist = 25;
    Vector2D firsPos = selfPos + Vector2D::polar2vector(dist,-60.0);
    Vector2D endPos = selfPos + Vector2D::polar2vector(dist,+60.0);

    AngleDeg from = AngleDeg::normalize_angle((firsPos-selfPos).dir().degree());
    AngleDeg to = AngleDeg::normalize_angle((endPos-selfPos).dir().degree());



    dir.push_back(from);
    nr.push_back(200);
    position.push_back(firsPos);

    for (int i = 1 ; i < 12; ++i) {

        if(wm.theirPlayer(i) && wm.theirPlayer(i)->posCount()<minConf && wm.theirPlayer(i)->distFromSelf()<dist && (wm.theirPlayer(i)->pos()-selfPos).dir().isWithin(from,to) ){

//            cout<<"yep"<<endl;
            dir.push_back(AngleDeg::normalize_angle((wm.theirPlayer(i)->pos()-selfPos).dir().degree()));
            nr.push_back(1);
            position.push_back(wm.theirPlayer(i)->pos());


        }

    }

    dir.push_back(to);
    nr.push_back(0);
    position.push_back(endPos);

    //cout<<"tedad :: "<<position.size()-1<<endl;

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


    vector < Vector2D > ends;

    ends.push_back( position.at(0));

    for (int i = 0; i < position.size()-1; ++i) {


        Vector2D temp = selfPos + Vector2D::polar2vector(20,getBisectorTwoAngles( (position.at(i)-selfPos).dir().degree() , (position.at(i+1)-selfPos).dir().degree() ));
        ends.push_back(temp);

        //cout<<"to : "<<ends.at(i)<<endl;

    }

    ends.push_back( position.at(position.size()-1));



    for (int j = 1; j < 4; ++j) {


        for (int i = 0; i < ends.size(); ++i) {



            Body_SmartKick smart(ends.at(i),3,2,j);

            if(!smart.execute(agent) )
                continue;

//            double speed = smart.sequence().speed_;


            if(smart.sequence().pos_list_.size() != j )
                continue;

            int conf = getCount(wm,ends.at(i),/*calculate_PM(wm,selfPos,ends.at(i),speed)*/5);

            if(conf>3 )
                continue;


            NewKick kkkk;
            if(OCI(agent,toVecPos(startKick(wm,smart)),toVecPos(ends.at(i)),smart.sequence().speed_,10,0.3,stepKick(smart),0.0,false,0) ||
                Shoot().OCI(agent,startKick(wm,smart),ends.at(i),smart.sequence().speed_,10,0.3,stepKick(smart),0.0,false,0,0) ||
                    IOOCI(agent,startKick(wm,smart),ends.at(i),smart.sequence().speed_,kkkk,10,0.3,stepKick(smart),0.0,false,0) ){

                return true;
            }

        }

    }

    Vector2D bestPos = firsPos;
    double  ang=0.0;
    for (int i = 0; i < position.size() - 1; ++i) {
        if(getAngDiff( (position.at(i)-selfPos).dir().degree() , (position.at(i+1)-selfPos).dir().degree() ) > ang ){
            ang = getAngDiff( (position.at(i)-selfPos).dir().degree() , (position.at(i+1)-selfPos).dir().degree() );
            bestPos = ends.at(i+1);
        }
    }

    return doForceKick(agent,bestPos);




}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool    MostafPass::clear(PlayerAgent *agent, int minConf, double minDist, double minScore, double minAngle, double maxAngle, double minSpeed, NewKick &kicking){

    const WorldModel & wm = agent->world();

    vector <double> dir;
    vector <double> rel;
//    vector <int> unum;
    vector <Vector2D > position;
    vector <double> score ;
    vector <double> score2 ;

    Vector2D    ballPos=wm.ball().pos();
//    Vector2D    selfPos=wm.self().pos();
    Vector2D    selfPos = ballPos;

    Vector2D firstPos = selfPos + Vector2D::polar2vector(minDist,minAngle);
    Vector2D endPos = selfPos + Vector2D::polar2vector(minDist,maxAngle);

    double from = minAngle;
    double to = maxAngle;
    double maxRel = maxAngle - minAngle ;


//    unum.push_back(0);
    dir.push_back(from);
    rel.push_back( 0.0 );
    position.push_back(firstPos);

    int k=1;
    for (int i = 1 ; i < 12; ++i) {

        if(wm.theirPlayer(i) && wm.theirPlayer(i)->posCount() < minConf*3 && wm.theirPlayer(i)->distFromSelf() < minDist && wm.theirPlayer(i)->distFromSelf() > 3.0 /*&& (wm.theirPlayer(i)->pos()-selfPos).dir().isWithin(from,to) */){
//            unum.push_back(i);
            dir.push_back(AngleDeg::normalize_angle((wm.theirPlayer(i)->pos()-selfPos).dir().degree()));
            rel.push_back(AngleDeg::normalize_angle(dir[k] - minAngle ) );
            position.push_back(wm.theirPlayer(i)->pos());
            ++k;

        }

    }

//    unum.push_back(12);
    dir.push_back(to);
    rel.push_back(maxRel);
    position.push_back(endPos);




    for (int i = 0; i < rel.size(); ++i) {
        if( rel[i] < 0.0 ){
            rel[i]+=360.0;

        }

    }


    for (int i = 0; i < rel.size(); ++i) {
        for (int j = i+1; j < rel.size(); ++j) {
            if(rel[i] > rel[j]){
//                typeSwap(unum[i],unum[j]);
                typeSwap(rel[i],rel[j]);
                typeSwap(dir[i],dir[j]);
                typeSwap(position[i],position[j]);


            }


        }

    }
//    cout<<wm.time()<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<endl;

//    for (int i = 0; i < rel.size(); ++i) {

//        cout<<unum[i]<<"  "<<rel[i]<<"  "<<dir[i]<<endl;

//    }
//    cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;


//    for (int i = 1 ; i < 12; ++i) {

//        if(wm.theirPlayer(i) && wm.theirPlayer(i)->posCount()<minConf && wm.theirPlayer(i)->distFromSelf()<dist && (wm.theirPlayer(i)->pos()-selfPos).dir().isWithin(from,to) ){

////            cout<<"yep"<<endl;
//            dir.push_back(AngleDeg::normalize_angle((wm.theirPlayer(i)->pos()-selfPos).dir().degree()));
//            nr.push_back(1);
//            position.push_back(wm.theirPlayer(i)->pos());


//        }

//    }


//    //cout<<"tedad :: "<<position.size()-1<<endl;

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


    vector < Vector2D > ends;

    ends.push_back( firstPos );
    score2.push_back( min(rel[1]*2, (360.0 - (rel[rel.size()-1]) )*2) );
    score.push_back(posScoreInFieldForUs(firstPos));
//    cout<<"***************************************"<<endl;
//    cout<<unum[0]<<"   score : "<<*score.end()<<"  first "<<endl;
//    cout<<unum[0]<<"   score : "<<score[score.size()-1]<<"  first "<<endl;
//    cout<<unum[0]<<"   score : "<<score[0]<<"  first "<<endl;


    for (int i = 0; i < position.size(); ++i) {


        if( rel[i] < maxRel ){

            ends.push_back(selfPos + Vector2D::polar2vector(minDist,getBisectorTwoAngles( dir.at(i) , dir.at(i+1) )));

            score2.push_back(getAngDiff( dir.at(i) , dir.at(i+1) ));
            score.push_back(posScoreInFieldForUs(ends[i+1]));

//            cout<<unum[i+1]<<"   score : "<<score[i+1]<<endl;

        }
        else{

            ends.push_back( endPos );

            if(i == position.size()-1){
//                cout<<"no back"<<endl;
                score2.push_back(getAngDiff( dir.at(i) , dir.at(0) ));
            }
            else{
//                cout<<"with back : "<<unum[i+1]<<endl;
                score2.push_back(getAngDiff( dir.at(i) , dir.at(i+1) ));
            }

            score2[i+1] = min( score2[i+1] , score2.at(i) );
            score.push_back(posScoreInFieldForUs(endPos));
//            cout<<unum[i]<<"   score : "<<score[i+1]<<"  end "<<endl;

            break;
        }

    }


    for (int i = 0; i < score.size(); ++i) {
        for (int j = i+1 ; j < score.size(); ++j) {
            if(score.at(i)<score.at(j)){

                typeSwap(score[i],score[j]);
                typeSwap(score2[i],score2[j]);
                typeSwap(ends[i],ends[j]);


            }

        }

    }

    for (int i = 0; i < ends.size(); ++i) {

        int conf = getCount(wm,ends.at(i),/*calculate_PM(wm,selfPos,ends.at(i),speed)*/5);

        if(conf>minConf )
            continue;



        for (int j = 1; j < 4; ++j) {

            Body_SmartKick smart(ends.at(i),3,minSpeed,j);

            if(!smart.execute(agent) )
                continue;

            if(smart.sequence().pos_list_.size() != j )
                continue;

            NewKick kkkk;
            if(score2.at(i) > minScore ||
                    OCI(agent,toVecPos(startKick(wm,smart)),toVecPos(ends.at(i)),smart.sequence().speed_,10,0.3,stepKick(smart),0.0,false,0) ||
                Shoot().OCI(agent,startKick(wm,smart),ends.at(i),smart.sequence().speed_,10,0.3,stepKick(smart),0.0,false,0,0) ||
                    IOOCI(agent,startKick(wm,smart),ends.at(i),smart.sequence().speed_,kkkk,10,0.3,stepKick(smart),0.0,false,0) ){
//            if(score.at(i) > minScore){
                cout<<wm.time().cycle()<<"  "<<wm.self().unum() << "  "<<position[i];
                cout<<"  score "<<i<<" is : " <<score2.at(i)<<endl;

                cout<<"yeppppppppppppppppppppppppppppppppppp"<<endl;
                setKick(wm,kicking,smart);
                kicking.reciver = 222;
                kicking.reciverUnum = 222;
                kicking.reciverVector = Vector2D(0.0,0.0);


                return true;
            }

        }

    }



    return false;


//    for (int j = 1; j < 4; ++j) {


//        for (int i = 0; i < ends.size(); ++i) {



//            Body_SmartKick smart(ends.at(i),3,2,j);

//            if(!smart.execute(agent) )
//                continue;

////            double speed = smart.sequence().speed_;


//            int conf = getCount(wm,ends.at(i),/*calculate_PM(wm,selfPos,ends.at(i),speed)*/5);

//            if(conf>3 )
//                continue;


//            NewKick kkkk;
//            if(OCI(agent,toVecPos(startKick(wm,smart)),toVecPos(ends.at(i)),smart.sequence().speed_,10,0.3,stepKick(smart),0.0,false,0) ||
//                Shoot().OCI(agent,startKick(wm,smart),ends.at(i),smart.sequence().speed_,10,0.3,stepKick(smart),0.0,false,0,0) ||
//                    IOOCI(agent,startKick(wm,smart),ends.at(i),smart.sequence().speed_,kkkk,10,0.3,stepKick(smart),0.0,false,0) ){

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




}

bool    MostafPass::getClear(PlayerAgent * agent , double & minDis , double & minAng ,double & maxAng,double & minSpeed , int & minConf , bool & forceKick){

    const WorldModel & wm = agent->world();

    minConf = 4;
    forceKick = true;
    minDis = 27.0 ;


    if(wm.ball().pos().x < -30.0 ){

        if(wm.ball().pos().y < -15.0 ){

            minAng = -90.0 ;
            maxAng = 60.0 ;
            if(wm.interceptTable()->opponentReachCycle() < 2 ){

                minSpeed = 2.3 ;
                forceKick = true;

            }
            else {
                minSpeed = 2.7 ;
//                minDis = 40;
            }




        }
        else if(wm.ball().pos().y > 15.0 ){

            minAng = -60.0 ;
            maxAng = 90.0 ;
            if(wm.interceptTable()->opponentReachCycle() < 2 ){

                minSpeed = 2.3 ;
                forceKick = true;

            }
            else {
                minSpeed = 2.7 ;
//                minDis = 40;
            }




        }
        else{
            minAng = min((Vector2D(-52.5,-15.0)-wm.self().pos()).dir().degree(),(Vector2D(-52.5,+15.0)-wm.self().pos()).dir().degree()) ;
            maxAng = max((Vector2D(-52.5,-15.0)-wm.self().pos()).dir().degree(),(Vector2D(-52.5,+15.0)-wm.self().pos()).dir().degree()) ;
            if(wm.interceptTable()->opponentReachCycle() < 2 ){

//                minDis = 40.0;
                minSpeed = 2.3 ;
                forceKick = true;

            }
            else {
                minSpeed = 2.7 ;
//                minDis = 40;
            }


        }




    }
    else if(wm.ball().pos().x < -10.0 ){


        if(wm.ball().pos().y < -15.0 ){

            minAng = (Vector2D(15.0 ,-34.0)-wm.self().pos()).dir().degree() ;
            maxAng =  +40.0 ;
            if(wm.interceptTable()->opponentReachCycle() < 1 ){

                minSpeed = 2.3 ;
                forceKick = true;

            }
            else {
                minSpeed = 2.7 ;
//                minDis = 40;
            }




        }
        else if(wm.ball().pos().y > 15.0 ){

            minAng = -40.0 ;
            maxAng = (Vector2D(15.0 ,34.0)-wm.self().pos()).dir().degree() ; ;
            if(wm.interceptTable()->opponentReachCycle() < 1 ){

                minSpeed = 2.3 ;
                forceKick = true;

            }
            else {
                minSpeed = 2.7 ;
//                minDis = 40;
            }




        }
        else{
            minAng = -75.0 ;
            maxAng = +75.0 ;
            if(wm.interceptTable()->opponentReachCycle() < 1 ){

//                minDis = 40.0;
                minSpeed = 2.3 ;
                forceKick = true;

            }
            else {
                minSpeed = 2.7 ;
//                minDis = 40;
            }


        }






    }
    else if(wm.ball().pos().x < 20.0 ){


        if(wm.ball().pos().y < -15.0 ){

            minAng = (Vector2D(40.0 ,-34.0)-wm.self().pos()).dir().degree() ;
            maxAng =  (Vector2D(32.0 ,0.0)-wm.self().pos()).dir().degree() ;
            minSpeed = 2.7 ;
//            minDis = 40;





        }
        else if(wm.ball().pos().y > 15.0 ){

            minAng = (Vector2D(32.0 ,0.0)-wm.self().pos()).dir().degree() ;
            maxAng =  (Vector2D(40.0 ,34.0)-wm.self().pos()).dir().degree() ;
            minSpeed = 2.7 ;
//            minDis = 40;




        }
        else{
            minAng = (Vector2D(35.0 ,-34.0)-wm.self().pos()).dir().degree() ;
            maxAng = (Vector2D(35.0 ,34.0)-wm.self().pos()).dir().degree() ;
            minSpeed = 2.7 ;
//            minDis = 40;



        }







    }
    else{


        if(wm.ball().pos().y < -15.0 ){

            minAng = min((Vector2D(45.0,0.0)-wm.self().pos()).dir().degree(),(Vector2D(35.0,0.0)-wm.self().pos()).dir().degree()) ;
            maxAng = max((Vector2D(45.0,0.0)-wm.self().pos()).dir().degree(),(Vector2D(35.0,0.0)-wm.self().pos()).dir().degree());
            minSpeed = 2.7 ;
            minDis = Vector2D(52.5,0.0).dist(wm.self().pos()) + 5;





        }
        else if(wm.ball().pos().y > 15.0 ){

            minAng = min((Vector2D(45.0,0.0)-wm.self().pos()).dir().degree(),(Vector2D(35.0,0.0)-wm.self().pos()).dir().degree()) ;
            maxAng = max((Vector2D(45.0,0.0)-wm.self().pos()).dir().degree(),(Vector2D(35.0,0.0)-wm.self().pos()).dir().degree());
            minSpeed = 2.7 ;
            minDis = Vector2D(52.5,0.0).dist(wm.self().pos()) + 5;




        }
        else{
            minAng = min((Vector2D(52.5,6.5)-wm.self().pos()).dir().degree(),(Vector2D(52.5,-6.5)-wm.self().pos()).dir().degree()) ;
            maxAng = max((Vector2D(52.5,6.5)-wm.self().pos()).dir().degree(),(Vector2D(52.5,-6.5)-wm.self().pos()).dir().degree()) ;
            minSpeed = 2.9 ;
            minDis = Vector2D(52.5,0.0).dist(wm.self().pos()) + 7;



        }
    }










}

/////////////////////////


bool MostafPass::doForceKick(PlayerAgent *agent,Vector2D pos)
{

    const WorldModel & wm = agent->world();

    if ( wm.gameMode().type() == GameMode::PlayOn
         && ! wm.self().goalie()
         && wm.self().isKickable()
         )
    {
        dlog.addText( Logger::TEAM,
                      __FILE__": simultaneous kick" );
        agent->debugClient().addMessage( "SimultaneousKick" );
        Vector2D goal_pos( ServerParam::i().pitchHalfLength(), 0.0 );

        if ( wm.self().pos().x > 36.0
             && wm.self().pos().absY() > 10.0 )
        {
            goal_pos.x = 45.0;
            dlog.addText( Logger::TEAM,
                          __FILE__": simultaneous kick cross type" );
        }
        Body_SmartKick( pos,
                          ServerParam::i().ballSpeedMax(),2,3
                          ).execute( agent );

        return true;
    }

    return false;
}


bool MostafPass :: execute ( rcsc::PlayerAgent * agent ,NewKick &kicking )
{



    const rcsc::WorldModel & wm = agent->world();

//    if(wm.ball().pos().dist(Vector2D(52.5,0.0)) > 20 )
    if(wm.ball().pos().x < 36 )
        return false;


    rcsc::Vector2D	posBall=wm.ball().pos();
    rcsc::Vector2D	end;
    double spd;
    double minX=posBall.x-12;
    if(posBall.x>40)
        minX=30;

    //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;


    const rcsc::ServerParam & param = rcsc::ServerParam::i();
    const rcsc::PlayerPtrCont & teas = wm.teammatesFromSelf();
    const rcsc::PlayerPtrCont::const_iterator teas_end = teas.end();
    const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();
    const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();

    rcsc::Vector2D opp_goal ( 52.5 , 0.0 ) ;

    std::vector <double> gc_score ;
    std::vector <int> gc_num ;

    std::vector <double> bp_score ;
    std::vector <int> bp_num ;
    std::vector <int> bp_step ;

    double bp_sc ;
    int bp_st ;

    oci *oc ;
    //Shoot *sh ;

    std::vector <double> out_score ;
    std::vector <double> out_num ;

    //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;

    // pass in out situation

    if ( is_out_situation ( agent ) )
    {
        for ( rcsc::PlayerPtrCont::const_iterator it = teas.begin();
              it != teas_end;
              ++it )
        {
            rcsc::Vector2D pos_teammate = (*it)->pos() ;

            double speed = best_first_speed ( agent , pos_teammate , 1 ) ;

            bool oci  = oc->ball_reach_safe ( agent , pos_teammate , 1 , speed , true ) ;

            if (  wm.gameMode().type() == rcsc::GameMode::BackPass_
                  || wm.gameMode().type() == rcsc::GameMode::IndFreeKick_ )
            {
                if ( oci && !(*it)->goalie() && (*it)->pos().x > wm.self().pos().x - 2 )
                {
                    cout << "        **5rdtype" << endl ;
                    rcsc::Body_SmartKick( pos_teammate ,
                                          speed ,
                                          speed * 0.96 ,
                                          1 ).execute( agent );
                    return true ;
                }
            }

            if ( oci && !(*it)->goalie() )
            {
                double oclose = 1000 ;
                double oxdiff = (*it)->pos().x - wm.self().pos().x ;

                for ( rcsc::PlayerPtrCont::const_iterator itt = opps.begin();
                      itt != opps_end;
                      ++itt )
                {
                    if ( (*it)->pos().dist ( (*itt)->pos() ) < oclose )
                    {
                        oclose = (*it)->pos().dist ( (*itt)->pos() ) ;
                    }
                }
                out_score.push_back ( oclose + oxdiff ) ;
                out_num.push_back ( (*it)->unum() ) ;
            }
        }

        if ( out_score.size() != 0 )
        {
            int Number = out_num [ get_min_or_max ( out_score , true ) ] ;

            if ( wm.ourPlayer( Number ) )
            {
                double speed = best_first_speed ( agent , wm.ourPlayer( Number )->pos() , 1 ) ;

                rcsc::Body_SmartKick( wm.ourPlayer( Number )->pos() ,
                                      speed ,
                                      speed * 0.96 ,
                                      1 ).execute( agent );
                cout << "        **6rdtype" << endl ;
                return true ;
            }
        }

        return false ;
    }

    //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;

    for ( rcsc::PlayerPtrCont::const_iterator it = teas.begin();
          it != teas_end;
          ++it )
    {
        //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;

        rcsc::Vector2D pos_teammate = (*it)->pos() ;

        //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;
        if ( is_goal_chance_situation ( agent , it ) )
        {
            //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;

            gc_score.push_back ( goal_chance_score ( agent , it ) ) ;
            gc_num.push_back ( (*it)->unum() ) ;
        }
//        if ( is_better_pos_situation ( agent , it , bp_sc , bp_st ) )
//        {
//            bp_score.push_back ( bp_sc ) ;
//            bp_step.push_back ( bp_st ) ;
//            bp_num.push_back ( (*it)->unum() ) ;
//        }
    }

    //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;

    if ( gc_score.size() != 0 )
    {
        //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;

        int num = gc_num [ get_min_or_max ( gc_score , true ) ];
        rcsc::Vector2D best_teammate = wm.ourPlayer( gc_num [ get_min_or_max ( gc_score , true ) ] )->pos() ;

        double speed = get_max_speed_of_step ( agent , 2 , best_teammate ) ;

        rcsc::Body_SmartKick smart( best_teammate ,
                              speed ,
                              speed * 0.96 ,
                              2 );//

        if(!smart.execute( agent ))
            return false;


        setKick(wm,kicking,smart);
        kicking.reciver = num;
        kicking.reciverUnum = num;
        kicking.reciverVector = best_teammate - wm.ourPlayer(num)->pos();

        return true ;
    }

    //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;
//    if ( bp_score.size() != 0 )
//    {
//        rcsc::Vector2D best_teammate = wm.ourPlayer( bp_num [ get_min_or_max ( bp_score , true ) ] )->pos()  ;
//        int step = bp_step [ get_min_or_max ( bp_score , true ) ] ;

//        double speed = best_first_speed ( agent , best_teammate , step ) ;

//        /// cout << "better pos to " << bp_num [ get_min_or_max ( bp_score , true ) ] << endl ;
//        rcsc::Body_SmartKick( best_teammate ,
//                              speed ,
//                              speed * 0.96 ,
//                              step ).execute( agent );

//        return true ;
//    }

    //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;
    return false ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

double MostafPass :: get_first_speed_of_end ( rcsc::PlayerAgent * agent , rcsc::Vector2D pos , double end_speed )
{
    const rcsc::WorldModel & wm = agent->world();
    const rcsc::ServerParam & param = rcsc::ServerParam::i();

    //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;
    double dist = wm.ball().pos().dist ( pos ) ;

    double sum = 0 ;
    double speed ;

    sum += end_speed ;
    speed = end_speed ;
//cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;
    if ( sum >= dist )
    {
        return speed ;
    }
//cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;
    while ( true )
    {
        end_speed /= param.ballDecay() ;
        sum += end_speed ;

        speed = end_speed ;

        if ( sum > dist )
        {
            return speed ;
        }
    }
    //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool MostafPass :: is_goal_chance_situation ( rcsc::PlayerAgent * agent , rcsc::PlayerPtrCont::const_iterator it )
{

    const rcsc::WorldModel & wm = agent->world();
    const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();
    const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();

    //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;

    rcsc::Vector2D pos_teammate = (*it)->pos() ;
    rcsc::Vector2D opp_goal ( 52.5 , 0.0 ) ;

    double dist = wm.ball().pos().dist ( pos_teammate ) ;
    double dist_to_goal = wm.ball().pos().dist ( opp_goal ) ;
    double ang_to_tea = ( pos_teammate - wm.ball().pos() ).th().degree() ;

    bool pos_better = false ;

    if ( dist_to_goal > pos_teammate.dist ( opp_goal ) ||
         ( pos_teammate.absY() < 7.0 && wm.ball().pos().absY() > 7.0 && pos_teammate.x > 39.0 ) )
    {
        pos_better = true ;
    }

    if ( wm.ball().pos().x < 30.0 || (*it)->posCount() > 0 || (*it)->pos().x > wm.offsideLineX() )
    {
        //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;
        return false ;
    }

    //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;
    if ( pos_teammate.x > 36.0 && pos_teammate.absY() < 15.0 &&
         fabs ( wm.ball().pos().y - pos_teammate.y ) < 25.0 &&
          pos_better )
    {
        Cone2D cone ( wm.ball().pos() , 15.0 , dist ) ;
        bool opp_in = false ;

        for ( rcsc::PlayerPtrCont::const_iterator itt = opps.begin();
              itt != opps_end;
              ++itt )
        {
            rcsc::Vector2D pos_opponent = (*itt)->pos() ;

            if ( cone.isInCone ( pos_opponent , ang_to_tea ) )
            {
                opp_in = true ;
                break ;
            }
        }

        //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;
        if ( opp_in )
        {
            return false ;
        }

        return true ;
    }

    //cout<<wm.time().cycle()<<"  "<<__FILE__<<" "<<__LINE__<<endl;
    return false ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool MostafPass :: is_better_pos_situation ( rcsc::PlayerAgent * agent , rcsc::PlayerPtrCont::const_iterator it , double &score , int &step )
{
    return false;
    const rcsc::WorldModel & wm = agent->world();
    const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();
    const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();

    oci *oc ;

    const rcsc::PlayerObject * nearest_opp
        = ( opps.empty()
            ? static_cast< rcsc::PlayerObject * >( 0 )
            : opps.front() );
    const double nearest_opp_dist = ( nearest_opp
                                      ? nearest_opp->distFromSelf()
                                      : 1000.0 );

    int confidence = (*it)->posCount() ;
    int confidence_body = (*it)->bodyCount() ;
    int confidence_vel = (*it)->velCount() ;

    rcsc::Vector2D pos_teammate = (*it)->pos() ;

    rcsc::AngleDeg ang_to_teammate = ( pos_teammate - wm.self().pos() ).th() ;

    int confidence_dir = wm.dirCount ( ang_to_teammate ) ;

    if ( confidence_body == 0 && confidence_vel == 0 )
    {
        pos_teammate = (*it)->pos() + rcsc::Vector2D::polar2vector ( (*it)->vel().r() , (*it)->body().degree() ) ;
    }

    double dist = wm.self().pos().dist ( pos_teammate ) ;

    if ( (*it)->goalie() || (*it)->pos().x < -45.0 || (*it)->pos().x > wm.offsideLineX()
         || wm.self().pos().dist ( (*it)->pos() ) < 6.0 || confidence_dir > 1 )
    {
        return false ;
    }

    double speed1 = best_first_speed  ( agent , (*it)->pos() , 1  ) ;
    bool oci1 = oc->ball_reach_safe ( agent , pos_teammate , 1 , speed1 , true ) ;

    double closest_dist = 30.0 ;
    double x_diff = pos_teammate.x - wm.self().pos().x ;

    for ( rcsc::PlayerPtrCont::const_iterator itt = opps.begin();
          itt != opps_end;
          ++itt )
    {
        rcsc::Vector2D pos_opponent = (*itt)->pos() ;

        if ( pos_teammate.dist ( pos_opponent ) < closest_dist )
        {
            closest_dist = pos_teammate.dist ( pos_opponent ) ;
        }
    }

    double my_score = ( nearest_opp_dist * 80.0 ) + 30.0 ;

    if ( wm.self().pos().x < -36.0 && wm.self().pos().absY() < 15.0 )
    {
        my_score -= 100 ;
    }

    if ( wm.self().unum() <= 5 )
    {
        my_score -= 50 ;
    }

    ////

    double sc = 0.0 ;

    sc += ( closest_dist * 80.0 ) ;

    sc += ( ( 1 - confidence ) * 15.0 ) ;
    sc += ( ( 1 - confidence_body ) * 20.0 ) ;

    if ( wm.ball().pos().x < 34.0 || pos_teammate.x < 34.0 )
    {
        if ( x_diff < 0.0 )
        {
            if ( pos_teammate.x < -10.0 )
            {
                x_diff *= 5.0 ;
            }
            else
            {
                x_diff *= 2.0 ;
            }
        }

        sc += ( x_diff * 30.0 ) ;
    }

    if ( (*it)->pos().x < -36.0 && (*it)->pos().absY() < 13.0 )
    {
        sc -= 200 ;
    }

    if ( (*it)->unum() <= 5 )
    {
        sc -= 70 ;
    }

    if ( wm.self().unum() <= 5 && (*it)->unum() > 5 && x_diff > 0.0 )
    {
        sc = my_score + 1.0 ;
    }

    if ( oci1 )
    {
        step = 1 ;

        if ( sc > my_score )
        {
            score = sc ;
            return true ;
        }
        else
        {
            return false ; // problem is not oci , problem is score ! so return false ...
        }
    }

    double speed2 = best_first_speed  ( agent , (*it)->pos() , 2  ) ;
    bool oci2 = oc->ball_reach_safe ( agent , pos_teammate , 2 , speed2 , true ) ;

    if ( oci2 )
    {
        step = 2 ;

        if ( sc > my_score )
        {
            score = sc ;
            return true ;
        }
    }

    if ( dist > 25.0 )
    {
        double speed3 = best_first_speed  ( agent , (*it)->pos() , 3  ) ;
        bool oci3 = oc->ball_reach_safe ( agent , pos_teammate , 3 , speed3 , true ) ;

        if ( oci3 )
        {
            step = 3 ;

            if ( sc > my_score )
            {
                score = sc ;
                return true ;
            }
        }
    }

    return false ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool MostafPass :: is_out_situation ( rcsc::PlayerAgent * agent )
{
    const rcsc::WorldModel & wm = agent->world();

    if ( wm.gameMode().type() == rcsc::GameMode::PlayOn )
    {
        return false ;
    }
    return true ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

double MostafPass :: goal_chance_score ( rcsc::PlayerAgent * agent , rcsc::PlayerPtrCont::const_iterator it )
{
    oci *oc ;
    //Shoot *sh ;

    const rcsc::WorldModel & wm = agent->world();
    const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();
    const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();

    rcsc::Vector2D pos_teammate = (*it)->pos() ;
    rcsc::Vector2D opp_goal ( 52.5 , 0.0 ) ;

    double dist_to_goal = wm.ball().pos().dist ( opp_goal ) ;

    double closest_dist = 30.0 ;

    for ( rcsc::PlayerPtrCont::const_iterator itt = opps.begin();
           itt != opps_end;
           ++itt )
    {
        rcsc::Vector2D pos_opponent = (*itt)->pos() ;

        if ( pos_teammate.dist ( pos_opponent ) < closest_dist )
        {
            closest_dist = pos_teammate.dist ( pos_opponent ) ;
        }
    }

    bool oci = oc->ball_reach_safe ( agent , pos_teammate , 2 , get_max_speed_of_step ( agent , 2 , pos_teammate ) , true ) ;

    double sc = 0.0 ;
    sc = ( oci ) ? ( sc + 100.0 ) : ( sc + 0.0 ) ;
    sc -= ( dist_to_goal * 3.0 ) ;
    sc += ( closest_dist * 25.0 ) ;

    return sc ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int MostafPass :: get_min_or_max ( vector <double> v , bool is_max )
{
    int i ;
    double max , min ;
    int num ;

    if ( is_max )
    {
        max = v[0];
        num = 0 ;

        for ( i = 1 ; i < v.size() ; i++ )
        {
            if ( v[i] > max )
            {
                max = v[i] ;
                num = i ;
            }
        }
        return num ;
    }
    else
    {
        min = v[0];
        num = 0 ;

        for ( i = 1 ; i < v.size() ; i++ )
        {
            if ( v[i] < min )
            {
                min = v[i] ;
                num = i ;
            }
        }
        return num ;
    }
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

double MostafPass :: best_first_speed ( rcsc::PlayerAgent * agent , rcsc::Vector2D target , int step )
{
    const rcsc::WorldModel & wm = agent->world();

    double dist = wm.self().pos().dist ( target ) ;

    double speed = dist * 0.25 ;

    speed = min ( speed , 3.0 ) ;
    speed = max ( speed , 0.3 ) ;

    if ( step == 1 )
    {
        rcsc::Vector2D one_step_vel ;

        double max_one_step_speed ;

        one_step_vel = rcsc::KickTable::calc_max_velocity( ( target - wm.self().pos() ).th() ,
                                                           wm.self().kickRate(),
                                                           wm.ball().vel() );
        max_one_step_speed = one_step_vel.r();

        speed = std::min ( max_one_step_speed , speed );

        if ( is_out_situation ( agent ) && dist < 10.0 )
        {
            speed = std::min ( speed , 1.5 );
        }
    }

    return speed ;
}

/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


double MostafPass ::get_max_speed_of_step ( rcsc::PlayerAgent * agent , int step , rcsc::Vector2D point )
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




double	MostafPass::ranking(rcsc::PlayerAgent * agent,rcsc::Vector2D pos){

    const rcsc::WorldModel & world = agent->world();
    double sum=100;

    for (int i = 1; i < 12; i++)
        if(world.theirPlayer(i))
            if(pos.dist(world.theirPlayer(i)->pos())<10)
            sum=sum-(10-pos.dist(world.theirPlayer(i)->pos()));

    return sum;
}

/// //////////////////////////////////////////////////////////////// ///

double	MostafPass::getCofficient(double Ang){

    rcsc::AngleDeg	temp=Ang/2;

    return ((cos(temp.radian()))+1)/2;

}

/// //////////////////////////////////////////////////////////////// ///

double	MostafPass::congestionScore(rcsc::PlayerAgent * agent,rcsc::Vector2D pos){

    const rcsc::WorldModel & world = agent->world();
    double sum=0;
    rcsc::Vector2D	temp=rcsc::Vector2D(55.0,0.0);
    if(pos.x<20)temp=rcsc::Vector2D(55,temp.y);

    double dirToGoal=(temp-pos).dir().degree();

    for (int i = 1; i < 12; i++)
        if(world.theirPlayer(i))
            if(pos.dist(world.theirPlayer(i)->pos())<10)
                sum+=(10-pos.dist(world.theirPlayer(i)->pos()))*getCofficient(getAngDiff(dirToGoal,(world.theirPlayer(i)->pos()-pos).dir().degree()));

    return	100-sum;

}
/// //////////////////////////////////////////////////////////////// ///

double	MostafPass::posScoreInFieldForUs(rcsc::Vector2D pos){

    return ((110.368-rcsc::Vector2D(52.5,0.0).dist(pos))+((34-fabs(pos.y))/2.5)+((pos.x+52.5)/1.12))/2.194225455;

}
double	MostafPass::posScoreInFieldForClear(rcsc::Vector2D pos){

    return ((110.368-rcsc::Vector2D(-52.5,0.0).dist(pos))+((34-fabs(pos.y))/2.5)+((-pos.x+52.5)/1.12))/-2.194225455;

}

double MostafPass::calcScore(rcsc::PlayerAgent * agent,rcsc::Vector2D pos){

    return (posScoreInFieldForUs(pos)*2/*+ranking(agent,pos)*0.0*/+congestionScore(agent,pos)*0.75);
}








/// /////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool MostafPass::IOOCI(PlayerAgent * agent,Vector2D start,Vector2D end,double speed,NewKick & kick,int minConf,double minSpeed,int step,double rand,bool withRand,int Ext){

    if(kick.step == 0)
        return false;

    const WorldModel & wm = agent->world();

    struct stc{

        Simul   opp;
        double  edp;
        double  edpBack;
        int     conf;
        int     bodyConf;
        double  kickableArea;
    }info[11];

    int nr=0;

    double PM = calculate_PM(wm,start,end,speed,minSpeed);
    double angCount = getCount(wm,end,PM);

    if(angCount > minConf)
        return false;
    //int confing = min(minConf,getCount(wm,(end-toVecPos(wm.self().pos())).getDirection(),0))+Ext-1;

    for(int i=1;i<12;i++){
        if(wm.theirPlayer(i) && wm.theirPlayer(i)->seenPosCount() <= minConf){
            info[nr].opp=Simul(agent,SIM_OPPONENT,i);
            info[nr].edp=100*info[nr].opp.getPT()->dashPowerRate()*info[nr].opp.getPT()->effortMax();
            info[nr].edpBack=info[nr].edp*/*info[nr].opp.getDashRate(180)*/0.6;
            info[nr].conf=wm.theirPlayer(i)->posCount();
            info[nr].bodyConf=wm.theirPlayer(i)->bodyCount();
            info[nr].kickableArea=info[nr].opp.getPT()->kickableArea();
            nr++;
        }
    }

    if(nr==0)
        return true;

    //Vector2D start = kick.sequence().pos_list_.at(kick.sequence().pos_list_.size()-1);
    //Vector2D end = kick.M_target_point;
    //double speed = kick.sequence().speed_*BALL_DECAY;


    speed *= BALL_DECAY;

    double dir=(end-start).dir().degree();
    Vector2D vel=Vector2D::polar2vector(speed,dir);

    Vector2D RNoiseVel=vel,LNoiseVel=vel,BNoiseVel=vel;
    if(withRand){
        RNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir+90));
        LNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir-90));
        BNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir+180));
    }


    Simul temp(agent);

    int pluses = step-1;

    if(withRand==false){

        Line2D    line=Line2D(start,end);
        for(int j=0; j<nr ; j++){

            Vector2D posOnLine=line.projection(info[j].opp.getPosition());
            temp = info[j].opp;
            for(int k=1;k<=info[j].conf+pluses;k++)
                temp.moveToPosSim(posOnLine,5);

            Vector2D temp1,temp2;
            Circle2D  cir=Circle2D(temp.getPosition()+temp.getVelocity(),info[j].kickableArea);
            if(cir.intersection(line,&temp1,&temp2)==0)
                continue;
            else if(temp1.isBetweenXY(start,end) || temp2.isBetweenXY(start,end))
                return false;
        }
    }

    else{

        Vector2D BStart=(start+BNoiseVel-vel);
        Vector2D RStart=(start+RNoiseVel-vel);
        Vector2D LStart=(start+LNoiseVel-vel);

        Vector2D BEnd=BStart+Vector2D::polar2vector(BStart.dist(end),BNoiseVel.dir().degree());
        Vector2D REnd=RStart+Vector2D::polar2vector(RStart.dist(end),RNoiseVel.dir().degree());
        Vector2D LEnd=LStart+Vector2D::polar2vector(LStart.dist(end),LNoiseVel.dir().degree());

        Line2D    lineB=Line2D(start+BNoiseVel-vel,BEnd);
        Line2D    lineR=Line2D(start+RNoiseVel-vel,REnd);
        Line2D    lineL=Line2D(start+LNoiseVel-vel,LEnd);

        Vector2D posOnLine;
        Vector2D temp1,temp2;
        Circle2D  cir;

        for(int j=0; j<nr ; j++){

            posOnLine=lineB.projection(info[j].opp.getPosition());

            temp = info[j].opp;
            for(int k=1;k<=info[j].conf+pluses;k++)
                temp.moveToPosSim(posOnLine,5);

            cir=Circle2D(temp.getPosition()+temp.getVelocity(),info[j].kickableArea);

            if((cir.intersection(lineB,&temp1,&temp2)==true) &&(temp1.isBetweenXY(BStart,BEnd) || temp2.isBetweenXY(BStart,BEnd)))
                return false;

            if((cir.intersection(lineR,&temp1,&temp2)==true) &&(temp1.isBetweenXY(RStart,REnd) || temp2.isBetweenXY(RStart,REnd)))
                return false;

            if((cir.intersection(lineL,&temp1,&temp2)==true) &&(temp1.isBetweenXY(LStart,LEnd) || temp2.isBetweenXY(LStart,LEnd)))
                return false;

        }

    }




//    vel *= BALL_DECAY;
//    BNoiseVel *= BALL_DECAY;
//    RNoiseVel *= BALL_DECAY;
//    LNoiseVel *= BALL_DECAY;
//    int xStart = 0;
//    int ExtTime = moveExt;

//    if(start.x<-20){
//        xStart += step;
//        ExtTime += step;
//    }

    int xStart = 0;
    int ExtTime = 0;

//    int step;


    if(withRand==false){

        Simul   ball(agent,SIM_BALL);
        ball.setVelocity(vel);
        ball.setPosition(start);
        Vector2D posBall=start;

        int time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

        if(time==UNKNOWN_VALUE)
            return false;

        for(int i=xStart ; i<=time+ExtTime ; i++){

            for(int j=0; j<nr ; j++){

                temp = info[j].opp;
                int turnTime = temp.turnTimeToPlusSim(posBall,ANG_TURN);
                double dis;

                int cycles = i-turnTime+info[j].conf+pluses;

                if(  cycles <= 0 )
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,cycles);

                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea){
                    return false;
                }

            }

            for(int j=0; j<nr ; j++)
                if(posBall.dist(info[j].opp.getPosition())<BACK_DIS_DASH){

                temp = info[j].opp;
                Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);
                int turnTime = temp.turnTimeToPlusSim(posBack,ANG_TURN);
                double dis;
                int cycles = i-turnTime+info[j].conf+pluses;

                if(cycles<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelocity().r()+info[j].edpBack,temp.getDecay(),info[j].edpBack,min(cycles,3));

                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea){
                    return false;
                }

            }
            posBall+=vel;
            vel*=BALL_DECAY;

            if(i==time + ExtTime){
                for(i=time+1;i<=time+ ExtTime + Ext;i++){

                    for(int j=0; j<nr ; j++){

                        temp = info[j].opp;
                        int turnTime = temp.turnTimeToPlusSim(posBall,ANG_TURN);
                        double dis;
                        int cycles = i-turnTime+info[j].conf+pluses;

                        if(cycles<=0)
                            dis=0;
                        else
                            dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,cycles);

                        if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea){
                            return false;
                        }

                    }



                }
            }


        }
    }
    else{
        Vector2D BStart=(start+BNoiseVel-vel);
        Vector2D RStart=(start+RNoiseVel-vel);
        Vector2D LStart=(start+LNoiseVel-vel);

        Vector2D BEnd=BStart+Vector2D::polar2vector(BStart.dist(end),BNoiseVel.dir().degree());
        Vector2D REnd=RStart+Vector2D::polar2vector(RStart.dist(end),RNoiseVel.dir().degree());
        Vector2D LEnd=LStart+Vector2D::polar2vector(LStart.dist(end),LNoiseVel.dir().degree());

        //////////                    back noise
        Simul   ball(agent,SIM_BALL);
        ball.setVelocity(BNoiseVel);
        Vector2D posBall=BStart;
        ball.setPosition(posBall);

        int time=ball.calculateBallTimeTo(BStart.dist(BEnd),minSpeed);

        if(time==UNKNOWN_VALUE)
            return false;

        for(int i=xStart ; i<=time+ExtTime ; i++){
            for(int j=0; j<nr ; j++){

                temp = info[j].opp;
                int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);
                double dis;
                int cycles = i-turnTime+info[j].conf+pluses;

                if(cycles<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,cycles);

                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea){
                    return false;
                }

            }

            for(int j=0; j<nr ; j++)
                if(posBall.dist(info[j].opp.getPosition())<BACK_DIS_DASH){

                temp = info[j].opp;
                Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);
                int turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);
                double dis;
                int cycles = i-turnTime+info[j].conf+pluses;

                if(cycles<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelocity().r()+info[j].edpBack,temp.getDecay(),info[j].edpBack,min(cycles,3));

                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea){
                    return false;

                }

            }
            BNoiseVel*=BALL_DECAY;
            posBall+=BNoiseVel;

            if(i==time + ExtTime){
                for(i=time+1;i<=time+ ExtTime + Ext;i++){

                    for(int j=0; j<nr ; j++){

                        temp = info[j].opp;
                        int turnTime = temp.turnTimeToPlusSim(posBall,ANG_TURN);
                        double dis;
                        int cycles = i-turnTime+info[j].conf+pluses;

                        if(cycles<=0)
                            dis=0;
                        else
                            dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,cycles);

                        if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea){
                            return false;
                        }

                    }



                }
            }

        }
        //////////                    right noise

        ball.setVelocity(RNoiseVel);

        posBall=RStart;
        ball.setPosition(posBall);

        time=ball.calculateBallTimeTo(RStart.dist(REnd),minSpeed);

        if(time==UNKNOWN_VALUE)
            return false;

        for(int i=step ; i<=time ; i++){

            for(int j=0; j<nr ; j++){

                temp = info[j].opp;
                int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);
                double dis;
                int cycles = i-turnTime+info[j].conf+pluses;

                if(cycles<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,cycles);

                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea){
                    return false;
                }

            }

            for(int j=0; j<nr ; j++)
                if(posBall.dist(info[j].opp.getPosition())<BACK_DIS_DASH){

                temp = info[j].opp;
                Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);
                int turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);
                int cycles = i-turnTime+info[j].conf+pluses;

                double dis;
                if(cycles<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelocity().r()+info[j].edpBack,temp.getDecay(),info[j].edpBack,min(cycles,3));

                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea){
                    return false;
                }

            }
            RNoiseVel*=BALL_DECAY;
            posBall+=RNoiseVel;


            if(i==time + ExtTime){
                for(i=time+1;i<=time+ ExtTime + Ext;i++){

                    for(int j=0; j<nr ; j++){

                        temp = info[j].opp;
                        int turnTime = temp.turnTimeToPlusSim(posBall,ANG_TURN);
                        double dis;
                        int cycles = i-turnTime+info[j].conf+pluses;

                        if(cycles<=0)
                            dis=0;
                        else
                            dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,cycles);

                        if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea){
                            return false;
                        }

                    }



                }
            }
        }
        //////////                    left noise


        ball.setVelocity(LNoiseVel);

         posBall=LStart;
         ball.setPosition(posBall);

        time=ball.calculateBallTimeTo(LStart.dist(LEnd),minSpeed);

        if(time==UNKNOWN_VALUE)
            return false;


        for(int i=step ; i<=time ; i++){


            for(int j=0; j<nr ; j++){

                temp = info[j].opp;
                int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);
                double dis;
                int cycles = i-turnTime+info[j].conf+pluses;

                if(cycles<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,cycles);

                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea){
                    return false;
                }

            }

            for(int j=0; j<nr ; j++)
                if(posBall.dist(info[j].opp.getPosition())<BACK_DIS_DASH){

                temp = info[j].opp;
                Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);
                int turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);
                double dis;
                int cycles = i-turnTime+info[j].conf+pluses;

                if(cycles<=0)
                    dis=0;
                else
                    dis=temp.Sn(temp.getVelocity().r()+info[j].edpBack,temp.getDecay(),info[j].edpBack,min(cycles,3));

                if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea){
                    return false;
                }

            }
            LNoiseVel*=BALL_DECAY;
            posBall+=LNoiseVel;


            if(i==time + ExtTime){
                for(i=time+1;i<=time+ ExtTime + Ext;i++){

                    for(int j=0; j<nr ; j++){

                        temp = info[j].opp;
                        int turnTime = temp.turnTimeToPlusSim(posBall,ANG_TURN);
                        double dis;
                        int cycles = i-turnTime+info[j].conf+pluses;

                        if(cycles<=0)
                            dis=0;
                        else
                            dis=temp.Sn(temp.getVelocity().r()+info[j].edp,temp.getDecay(),info[j].edp,cycles);

                        if(temp.getPosition().dist(posBall)<dis+info[j].kickableArea){
                            return false;
                        }

                    }



                }
            }
        }
    }

    return true;
//    return OCC(agent,start,end,speed,minConf,minSpeed,step,rand,withRand);
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool    MostafPass::ClearTheFuckingBall(rcsc::PlayerAgent * agent, NewKick &kicking){


    cout<<agent->world().time()<<"  Fucking ClearBAll "<<endl;
    Vector2D goal(52.5,0.0);

double minDis,minAng,maxAng,minSpeed;
int minConf;
bool forcekick;
getClear(agent,minDis,minAng,maxAng,minSpeed,minConf,forcekick);

if(clear(agent,minConf,minDis,52.0,minAng,maxAng,minSpeed,kicking))
    return true;

//else if(getClearDir(agent,5))
//    return true;

else if(Clear_Ball().execute(agent))
    return true;
else
    return Body_HoldBall2008(true,goal).execute(agent);
return false;

}


bool    MostafPass::ClearTheFuckingBallFuckingGoalie(rcsc::PlayerAgent * agent){


    cout<<agent->world().time()<<"  Fucking ClearBAll "<<endl;
    Vector2D goal(52.5,0.0);

    static NewKick kick;

double minDis,minAng,maxAng,minSpeed;
int minConf;
bool forcekick;
getClear(agent,minDis,minAng,maxAng,minSpeed,minConf,forcekick);

    const rcsc::WorldModel & wm = agent->world();
    if( kick.cycle+kick.step>wm.time().cycle()){

//        cout<<wm.time().cycle()<<"  tryNnextStep : "<<endl;

        if(wm.ourPlayer(kick.reciverUnum) && wm.ourPlayer(kick.reciverUnum)->posCount() <= 1 ){

            kick.end = wm.ourPlayer(kick.reciverUnum)->pos() + kick.reciverVector;
        }
        else{


        }





        Body_SmartKick  smart(kick.end,kick.speed,kick.M_speed,kick.step-1);

        if( smart.execute(agent)  ){

//            cout<<wm.time().cycle()<<"  nextStep : accept"<<endl;

            Vector2D vel=Vector2D::polar2vector(smart.sequence().speed_,(kick.end-startKick(wm,smart)).dir().degree());



            kick.cycle=wm.time().cycle();
            kick.step=smart.sequence().pos_list_.size();
            kick.end=smart.M_target_point;
            kick.M_speed=smart.M_first_speed_thr;
            kick.speed=smart.M_first_speed;
            setKick(wm,Kicking,smart);
            Kicking.reciverUnum = 222;
            agent->setNeckAction( new Neck_TurnToPoint(kick.end));


            return true;

        }

    }
else if(clear(agent,minConf,minDis,52.0,minAng,maxAng,minSpeed,kick))
    return true;

//else if(getClearDir(agent,5))
//    return true;

else if(Clear_Ball().execute(agent))
    return true;
else
    return Body_HoldBall2008(true,goal).execute(agent);
return false;

}












































/// //////////////////////////////////////////////////////////////////////////////

bool	MostafPass::directPassSabok(PlayerAgent * agent,NewKick &kicking,int &nr){

    const WorldModel & wm = agent->world();

    Vector2D	agentPos=wm.self().pos();
    static int		agentNum=wm.self().unum();
    Vector2D posBall=wm.ball().pos();
    double	agentScore=flagScoring(agent,(posBall),(agentPos));
    double  agentScore2=calcScore(agent,agentPos)*SIXFACTOR;

    struct structer{

        Vector2D	pos;
        Vector2D    start;
        double      maxSpeed;
        int			step;
        int			conf;
        int			num;
        double  	score;
        double      rand;
        NewKick     kick;
    } points[30];

    int k=0;

    double offsideX = wm.offsideLineX() + 1.5 ;

    if (wm.offsideLineCount() > 5 )
        offsideX = 52.0;


    /// player Unum
    for( int i=2 ; i < 12 ;	i++){

        if(posBall.x > 36 ){
            i = 5;
            continue;
        }


        if(agentNum!=i && wm.ourPlayer(i) && wm.ourPlayer(i)->posCount()<3 && getCount(wm,wm.ourPlayer(i)->pos(),5)<2 && isInField(wm.ourPlayer(i)->pos())){

            Vector2D posPass = wm.ourPlayer(i)->pos()+wm.ourPlayer(i)->vel();

            if( wm.ourPlayer(i)->pos().x  > offsideX )
                continue;

            if(posPass.x < -35 && fabs(posPass.y)<15)
                continue;

            double score = flagScoring(agent,(posPass),(agentPos));
            double score2 = calcScore(agent,posPass);

            if( agentScore > score && agentScore2 > score2 )
                continue;

            spd A=getDirectSpeed(agentPos,posPass);

            for (int j = 1; j <= 3; j++)
            {

                points[k].pos= posPass;

                points[k].num=i;

    //            if(wm.ourPlayer(i)->bodyCount()<2)
    //                points[k].pos+=VecPosition(0.5,wm.ourPlayer(i)->body().degree(),POLAR);



                Body_SmartKick smart(posPass,A.max,A.min,j);

                if(!smart.execute(agent))
                    continue;
                if( j != smart.sequence().pos_list_.size() )
                    continue;

    //            points[k].can=true;


                points[k].conf=getCount(wm,posPass,calculate_PM(wm,agentPos,posPass,smart.sequence().speed_,0.15));

                if(  points[k].conf>1  ){

                    continue;
                }
                points[k].step = j;



                Simul2 a;
                setKick(wm,points[k].kick,smart);
                points[k].start=startKick(wm,smart);
                points[k].maxSpeed=smart.sequence().speed_;
    //            points[k].step=stepKick(smart);
                points[k].rand=kickRand(agent,smart);
                //points[k].conf=getCount(wm,wm.ourPlayer(i)->pos(),5);
    //            cout<<"max speed"<<points[k].start.getDistanceTo(points[k].pos)<<endl;
    //            cout<<"n : "<<a.N(points[k].start.getDistanceTo(points[k].pos),points[k].maxSpeed,0.94,true)<<endl;
                points[k].score=score*pow(XFACTOR,-2+a.N(points[k].start.dist(points[k].pos),points[k].maxSpeed,0.94,true));

                if( agentScore > points[k].score && agentScore2 > score2 )
                    continue;



                k++;
            }
        }
    }




    for (int i = 0; i < k; i++)
        for (int j=i+1; j < k; j++)
            if (points[i].score<points[j].score){
//        boolSwap(points[i].can,points[j].can);
        intSwap(points[i].num,points[j].num);
        intSwap(points[i].step,points[j].step);
        intSwap(points[i].conf,points[j].conf);
        vecSwap(points[i].pos,points[j].pos);
        vecSwap(points[i].start,points[j].start);
        doubleSwap(points[i].score,points[j].score);
        doubleSwap(points[i].rand,points[j].rand);
        doubleSwap(points[i].maxSpeed,points[j].maxSpeed);
        kickSwap(points[i].kick,points[j].kick);
    }


    for (int i = 0; i < k; i++)
    {
//        if( better==false || agentScore<points[i].score ||
//                agentScore2 < calcScore(agent,points[i].pos))
        {
            if( points[i].pos.dist(agentPos)>3 /** set && points[i].oppInCircle==0*/ ){

                if(/*points[i].can &&*/
                    ( ( Shoot().OCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,1,0)&& (cout<<"yes 11111111111111"<<endl) )  ||
                      ( OCI(agent,toVecPos(points[i].start),toVecPos(points[i].pos),points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,1) && (cout<<"yes 2222222222222"<<endl) )  ||
                        ( IOOCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,points[i].kick,6,0.3,points[i].step,points[i].rand/2,true,1) && (cout<<"yes boyyyyyyyyyyyyyy"<<endl) )) ){
                    kicking=points[i].kick;
                    kicking.reciver = points[i].num;
                    kicking.reciverUnum = points[i].num;
                    kicking.reciverVector = points[i].pos - wm.ourPlayer(points[i].num)->pos();


                    nr=points[i].num;
                    return true;
                }

            }
        }
    }


    return false;
}


/// /////////////////

bool    MostafPass::leadingPassSabok(PlayerAgent * agent,NewKick &kicking,int &nr){

    const WorldModel & wm = agent->world();

//    if(wm.ball().pos().x<-30)
//        return false;
    Vector2D posBall=wm.ball().pos();
    Vector2D posAgent=wm.self().pos();
    double	agentScore=flagScoring(agent,(posBall),(posAgent));
    double  agentScore2=calcScore(agent,posAgent)*SIXFACTOR;




    struct structer{
//        bool			can;
        Vector2D		pos;
        Vector2D        start;
        double			maxSpeed;
        int			step;
        int			conf;
        int			num;
        double      score;
        double      rand;
        NewKick     kick;
    } points[300];

    int k=0;


    double offsideX = wm.offsideLineX() + 1.5 ;

    if (wm.offsideLineCount() > 5 )
        offsideX = 52.0;

    /// player Unum
    for(int i=2 ; i<12 ; i++){

        if( posBall.x > 36 ){
            i = 5 ;
            continue;

        }


        if( !wm.ourPlayer(i) || i==wm.self().unum() || wm.ourPlayer(i)->seenPosCount() >0 || getCount(wm,wm.ourPlayer(i)->pos(),5) > 1 /*|| (wm.ourPlayer(i)->bodyCount()!=0 && wm.ourPlayer(i)->pos().dist(wm.self().pos())<40)*/)
            continue;

        if( wm.ourPlayer(i)->pos().x  > offsideX )
            continue;

        int av=1;

        if(i<7)
            av=2;

        Vector2D posTeammate = wm.ourPlayer(i)->pos() + wm.ourPlayer(i)->vel();

        if(posTeammate.x < -30 && posTeammate.absY() <18 )
            continue;

        double dir=0;
        if(wm.ourPlayer(i)->bodyCount()==0)
            dir=wm.ourPlayer(i)->body().degree();
        else
            av++;

        for(int j=0;j<10;j++){

            Vector2D posPass = posTeammate+Vector2D::polar2vector(j+1,dir);

            if( isInField(posPass,48,31)==false   ){

                continue;
            }
            double score=flagScoring(agent,(posPass),(posAgent));
            double score2=calcScore(agent,posPass);


            if( agentScore > score && agentScore2 > score2 )
                continue;

            /// step
            for(int s=1;s<=3;s++){



                spd A=getLeadSpeed(agent,i,posAgent,posPass,s,av);
                Body_SmartKick smart(posPass,A.max,A.min,s);

                if(!smart.execute(agent))
                    continue;

                if( s != smart.sequence().pos_list_.size() )
                    continue;

                if(smart.sequence().speed_ < 0.9 )
                    continue;

                points[k].conf=getCount(wm,posPass,calculate_PM(wm,posAgent,posPass,smart.sequence().speed_,0.15));

                if(  points[k].conf!=0  ){

                    continue;
                }

                points[k].num=i;
                points[k].pos=posPass;
                points[k].step=s;


                setKick(wm,points[k].kick,smart);
                points[k].start=startKick(wm,smart);
                points[k].maxSpeed=smart.sequence().speed_;



                points[k].rand=kickRand(agent,smart);
                Simul2 a;
                points[k].score=score*pow(XFACTOR,max(-5+a.N(points[k].start.dist(posPass),points[k].maxSpeed,0.94,true),0));

                if( agentScore > points[k].score && agentScore2 > calcScore(agent,posPass) )
                    continue;

                k++;
            }
        }
    }


    for (int i = 0; i < k; i++)
        for (int j=i+1; j < k; j++)
            if (points[i].score<points[j].score){

        intSwap(points[i].num,points[j].num);
        intSwap(points[i].step,points[j].step);
        intSwap(points[i].conf,points[j].conf);
        vecSwap(points[i].pos,points[j].pos);
        vecSwap(points[i].start,points[j].start);
        doubleSwap(points[i].score,points[j].score);
        doubleSwap(points[i].rand,points[j].rand);
        doubleSwap(points[i].maxSpeed,points[j].maxSpeed);
        kickSwap(points[i].kick,points[j].kick);
    }

    for (int i = 0; i < k; i++)
    {
//        if( better==false || agentScore<points[i].score||
//                agentScore2 < calcScore(agent,points[i].pos))
        {
            if( points[i].pos.dist(posAgent)>3 /** set && points[i].oppInCircle==0*/ ){

                if(/*points[i].can &&*/
                    ( ( Shoot().OCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,1,2) && (cout<<"yes 222222222222222"<<endl) ) ||
                      ( OCI(agent,toVecPos(points[i].start),toVecPos(points[i].pos),points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,3) ) && (cout<<"yes 22222222222222"<<endl)  ||
                        ( IOOCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,points[i].kick,6,0.3,points[i].step,points[i].rand/2,true,1) && (cout<<"yes boyyyyyyyyyyyyyy"<<endl) )) ){

                    kicking=points[i].kick;
                    kicking.reciver = points[i].num;
                    kicking.reciverUnum = points[i].num;
                    kicking.reciverVector = points[i].pos - wm.ourPlayer(points[i].num)->pos();


                    nr=points[i].num;
                    return true;
                }

            }
        }
    }


        return false;
}


/// //////////////////////////
bool        MostafPass::throughPassSabok(PlayerAgent * agent,NewKick &kicking,int &nr){

    const WorldModel & wm = agent->world();

    Vector2D posAgent=wm.self().pos();
    Vector2D posBall=wm.ball().pos();
    if(posAgent.x > 32 || posAgent.x < -32 )
        return false;

    double	agentScore=flagScoring(agent,(posBall),(posAgent));


    double  agentScore2=calcScore(agent,posAgent)*SIXFACTOR;


    struct structer{

//        bool			can;
        Vector2D		pos;
        Vector2D        start;
        double			maxSpeed;
        int             step;
        int             conf;
        int             num;
        double			score;
        double          rand;
        NewKick         kick;

    } points[486];

    int k=0;

    double  TG = tanDeg(15);

    double offsideX = wm.offsideLineX() + 1.5 ;

    if (wm.offsideLineCount() > 5 )
        offsideX = 52.0;

    /// player Unum
    for(int i=9 ; i<12 ; i++){


        if(!wm.ourPlayer(i) || i == wm.self().unum() || wm.ourPlayer(i)->posCount() > 4 || wm.ourPlayer(i)->pos().x > 45 )
            continue;

        if( wm.ourPlayer(i)->pos().x  > offsideX )
            continue;

        Vector2D posTeammate=wm.ourPlayer(i)->pos();


        int time = 1;


        for(int j=0;j<18;j++){

            for(int f=0;f<3;f++){

                Vector2D posPass = posTeammate + Vector2D( j*1.666666667/**5.0*/ + 10.0 , (TG-(TG*f)) * (j*/*5.0*/1.666666667 + 10.0) );
                if(!isInField(posPass,45.0,33))
                    continue;

//                    Simul2 a;
                double score=flagScoring(agent,posPass,posAgent)/**pow(XFACTOR,-2+a.N(points[k].start.getDistanceTo(points[k].pos),points[k].maxSpeed,0.94,true))*/;
//                    points[k].timing = timeToReach(smart.sequence().speed_ , points[k].pos.getDistanceTo(posAgent),time);
//cout<<"                                "<<points[k].timing<<endl;


                if( agentScore > score && agentScore2 > calcScore(agent,posPass) )
                        continue;


                /// step
                for(int s=1;s<=3;s++){

                    points[k].pos = posPass;

                    spd A = getThroughSpeed(agent,i,posAgent,posPass,s,time,0);

                    Body_SmartKick smart(posPass ,A.max,A.min, s);


                    if(!smart.execute(agent))
                        continue;

                    if( s != smart.sequence().pos_list_.size() )
                        continue;


                    points[k].conf = getCount (wm,posPass,calculate_PM(wm,posAgent,posPass,smart.sequence().speed_,0.15));


                    if(  points[k].conf>2  ){

                        continue;
                    }

                    setKick(wm,points[k].kick,smart);

                    points[k].step = s;
                    points[k].num = i;
                    points[k].start=startKick(wm,smart);
                    points[k].maxSpeed=smart.sequence().speed_;

                    points[k].rand=kickRand(agent,smart);

    //                    Simul2 a;
                    points[k].score = score/**pow(XFACTOR,-2+a.N(points[k].start.getDistanceTo(points[k].pos),points[k].maxSpeed,0.94,true))*/;
    //                    points[k].timing = timeToReach(smart.sequence().speed_ , points[k].pos.getDistanceTo(posAgent),time);
    //cout<<"                                "<<points[k].timing<<endl;


//                    if( agentScore > points[k].score && agentScore2 > calcScore(agent,points[k].pos) )
//                        continue;

                    k++;
                }
            }
        }
    }

    for (int i = 0; i < k; i++)
        for (int j=i+1; j < k; j++)
            if (points[i].score<points[j].score){
//        boolSwap(points[i].can,points[j].can);
        intSwap(points[i].num,points[j].num);
        intSwap(points[i].step,points[j].step);
        intSwap(points[i].conf,points[j].conf);
        vecSwap(points[i].pos,points[j].pos);
        vecSwap(points[i].start,points[j].start);
        doubleSwap(points[i].score,points[j].score);
        doubleSwap(points[i].rand,points[j].rand);
        doubleSwap(points[i].maxSpeed,points[j].maxSpeed);
        kickSwap(points[i].kick,points[j].kick);
//        intSwap(points[i].timing ,points[j].timing );
    }

    for (int i = 0; i < k; i++)
    {
//        if( better==false || agentScore<points[i].score||
//                agentScore2 < calcScore(agent,points[i].pos))
        {
            if( points[i].pos.dist(posAgent)>3 /** set && points[i].oppInCircle==0*/ ){
                if(/*points[i].can &&*/
                    ( ( Shoot().OCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,0,2) && (cout<<"yes 111111111111111"<<endl) ) ||
                      ( OCI(agent,toVecPos(points[i].start),toVecPos(points[i].pos),points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,2)) && (cout<<"yes 22222222222222222"<<endl)  ||
                        ( IOOCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,points[i].kick,6,0.3,points[i].step,points[i].rand/2,true,5) && (cout<<"yes boyyyyyyyyyyyyyy"<<endl) )) ){

                    kicking=points[i].kick;
                    kicking.reciver = points[i].num;
                    kicking.reciverUnum = points[i].num;
                    kicking.reciverVector = points[i].pos - wm.ourPlayer(points[i].num)->pos();

                    nr=points[i].num;
                    return true;
                }

            }
        }
    }

        return false;
}
