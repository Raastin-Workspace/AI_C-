//#ifdef HAVE_CONFIG_H
//#include <config.h>
//#endif

//#include <rcsc/action/view_change_width.h>

//#include "Pass.h"
//#define ANG_TURN 1
//#define BACK_DIS_DASH 5
//using namespace std;
//using namespace rcsc;

//#define XFACTOR 0.98

//int MostafPass::getCount(const WorldModel & wm,double dir,double PM){


//       return max(wm.dirCount(dir),max(wm.dirCount(AngleDeg::normalize_angle(dir+PM)),wm.dirCount(AngleDeg::normalize_angle(dir-PM))));

//}
//int MostafPass::getCount(const WorldModel & wm,Vector2D pos,double PM){


//        double dir=(pos-wm.self().pos()).dir().degree();
//       return max(wm.dirCount(dir),max(wm.dirCount(AngleDeg::normalize_angle(dir+PM)),wm.dirCount(AngleDeg::normalize_angle(dir-PM))));

//}
//double  MostafPass::calculate_PM(const WorldModel & wm,Vector2D start,Vector2D end,double speed,double minSpeed){

//    double dis=start.dist(end);

//    if(dis>(speed-minSpeed)/0.06)
//        return 100.0;

//    double X=1-(dis*(0.06)/speed);

//    int time =ceil((log10(X))/(log10(0.94)));

//    dis=Simul().Sn(0.0,0.4,0.6,max(time-2,0));

//    Vector2D pos=end+Vector2D::polar2vector(dis,AngleDeg::normalize_angle((end-start).dir().degree()+90.0));

//    return doubleNormalize(getAngDiff((pos-start).dir().degree(),(end-start).dir().degree()),3.0,10);



//}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//bool MostafPass::OCI(PlayerAgent * agent,Vector2D start,Vector2D end,double speed,int minConf,double minSpeed,int step,double rand,bool withRand,int Ext){

//    const WorldModel & wm = agent->world();


//    struct stc{

//        Simul   opp;
//        double  edp;
//        double  edpBack;
//        int     conf;
//        double  kickableArea;
//    }info[11];

//    Simul temp(agent);

//    int nr=0;

//    int confing = max(1,getCount(wm,(end-(wm.self().pos())).dir().degree(),0))+Ext-1;

//    for(int i=1;i<12;i++){
//        if(wm.theirPlayer(i) && wm.theirPlayer(i)->seenPosCount() <= minConf){
//            info[nr].opp=temp;
//            info[nr].opp = Simul(agent,SIM_OPPONENT,i);
//            info[nr].edp=100*info[nr].opp.getPT()->dashPowerRate()*info[nr].opp.getEffort();//*dash_rate
//            info[nr].edpBack=100*info[nr].opp.getPT()->dashPowerRate()*info[nr].opp.getEffort()*info[nr].opp.getDashRate(180);
//            info[nr].conf=wm.theirPlayer(i)->posCount();
//            info[nr].kickableArea=info[nr].opp.getPT()->kickableArea();
//            if(i==wm.theirGoalieUnum())
//                info[nr].kickableArea=1.35;
//            nr++;
//            //cout<<"add on player : "<<i<<"Object : "<<(ObjectT)(OBJECT_OPPONENT_1+(i-1))<<endl;

//        }
//    }
//    if(nr==0){

//        return true;


//    }

//    double dir=(end-start).dir().degree();
//    Vector2D vel=Vector2D::polar2vector(speed,dir);

//    Vector2D RNoiseVel=vel,LNoiseVel=vel,BNoiseVel=vel;
//    if(withRand){
//        RNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir+90));
//        LNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir-90));
//        BNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir+180));
//    }

//    if(withRand==false){
//        Line2D    line=Line2D(start,end);

//        for(int j=0; j<nr ; j++){

//            Vector2D posOnLine=line.projection(info[j].opp.getPosition());
//            temp = info[j].opp;
//            for(int k=1;k<=confing;k++)
//                temp.moveToPosSim(posOnLine,5);





//            Vector2D temp1,temp2;
//            Circle2D  cir=Circle2D(temp.getPosition()+temp.getVelocity(),info[j].kickableArea);
//            if(cir.intersection(line,&temp1,&temp2)==false)
//                continue;
//            else if(temp1.isBetweenXY(start/*+vel*/,end) || temp2.isBetweenXY(start/*+vel*/,end))
//                return false;


//            temp1,temp2;
//            cir=Circle2D(temp.getPosition(),info[j].kickableArea);
//            if(cir.intersection(line,&temp1,&temp2)==false)
//                continue;
//            else if(temp1.isBetweenXY(start/*+vel*/,end) || temp2.isBetweenXY(start/*+vel*/,end))
//                return false;
//            //Vector2D posOnLine=line.projection(temp.getPosition()+temp.getVelocity());
//            //if(line.getDistanceWithPoint(temp.getPosition()+temp.getVelocity())/*-(conf/2)*/<info[j].kickableArea&& posOnLine.isBetweenXY(start+vel,end) )
//                //return false;

//        }
//    }
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
//            for(int k=1;k<=confing;k++)
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
//            for(int k=1;k<=confing;k++)
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
//            for(int k=1;k<=confing;k++)
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





//    if(withRand==false){
//        Simul   ball(agent,SIM_BALL);

//        ball.setVelocity(vel);
//        ball.setPosition(start);
//        Vector2D posBall=start;//+vel;

//        int time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

//        if(time==22771)
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
//            vel*=0.94;
//            posBall+=vel;


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
//    else{
//        //////////                    back noise
//        Simul   ball(agent,SIM_BALL);

//        ball.setVelocity(BNoiseVel);

//        Vector2D posBall=start-vel+BNoiseVel;
//        ball.setPosition(posBall);

//        int time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

//        if(time==22771)
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
//            BNoiseVel*=0.94;
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

//        if(time==22771)
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
//            RNoiseVel*=0.94;
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

//        if(time==22771)
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
//            LNoiseVel*=0.94;
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

//    return true;
////    return OCC(agent,start,end,speed,minConf,minSpeed,step,rand,withRand);
//}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//bool MostafPass::OCC(PlayerAgent * agent,Vector2D start,Vector2D end,double speed,int minConf,double minSpeed,int step,double rand,bool withRand){

//    const WorldModel & wm = agent->world();

//    Simul   opp;
//    double  edp;
//    double  edpBack;
//    int     conf;




//    if(wm.theirPlayer(wm.theirGoalieUnum()) && wm.theirPlayer(wm.theirGoalieUnum())->posCount() < minConf){
//        Simul temp(agent,SIM_OPPONENT,wm.theirGoalieUnum());
//        opp=temp;

//        edp=100*opp.getPT()->dashPowerRate()*opp.getEffort();//*dash_rate
//        edpBack=100*opp.getPT()->dashPowerRate()*opp.getEffort()*opp.getDashRate(180);
//        conf=wm.theirPlayer(wm.theirGoalieUnum())->posCount();

//    }
//    else
//        return true;


//    double dir=(end-start).dir().degree();
//    Vector2D vel=Vector2D::polar2vector(speed,dir);

//    int config = max(0,getCount(wm,(end-(wm.self().pos())).dir().degree(),5));



//    Vector2D RNoiseVel=vel,LNoiseVel=vel,BNoiseVel=vel;
//    if(withRand){
//        RNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir+90));
//        LNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir-90));
//        BNoiseVel+=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir+180));
//    }

//    if(withRand==false){
//        Line2D    line=Line2D(start,end);

//        Vector2D temp1,temp2;
//        Circle2D  cir=Circle2D(opp.getPosition()+opp.getVelocity(),1.3/*opp.getPT().dCatchableAreaL*/);
//        if(cir.intersection(line,&temp1,&temp2)==false)
//            ;
//        else if(temp1.isBetweenXY(start/*+vel*/,end) || temp2.isBetweenXY(start/*+vel*/,end))
//            return false;

//        //Vector2D posOnLine=line.projection(opp.getPosition()+opp.getVelocity());

//        //if(line.getDistanceWithPoint(opp.getPosition()+opp.getVelocity())/*-(conf/2)*/<1.3/*opp.getPT().dCatchableAreaL*/ && posOnLine.isBetweenXY(start+vel,end) )
//            //return false;



//    }
//    else{
//        Line2D    line=Line2D(start,end);

//        Vector2D temp1,temp2;
//        Circle2D  cir=Circle2D(opp.getPosition()+opp.getVelocity(),1.3/*opp.getPT().dCatchableAreaL*/);
//        if(cir.intersection(line,&temp1,&temp2)==false)
//            ;
//        else if(temp1.isBetweenXY(start+BNoiseVel-vel,end) || temp2.isBetweenXY(start+BNoiseVel-vel,end))
//            return false;

//        //Vector2D posOnLine=line.projection(opp.getPosition()+opp.getVelocity());

//        //if(line.getDistanceWithPoint(opp.getPosition()+opp.getVelocity())/*-(conf/2)*/<1.3/*opp.getPT().dCatchableAreaL*/ && posOnLine.isBetweenXY(start+BNoiseVel,end) ){
//            //return false;

//        //}

//        Vector2D REnd=start+Vector2D::polar2vector(start.dist(end),RNoiseVel.dir().degree());
//        line=Line2D(start,REnd);

//        if(cir.intersection(line,&temp1,&temp2)==false)
//            ;
//        else if(temp1.isBetweenXY(start+RNoiseVel-vel,end) || temp2.isBetweenXY(start+RNoiseVel-vel,end))
//            return false;
//        //posOnLine=line.projection(opp.getPosition()+opp.getVelocity());

//        //if(line.getDistanceWithPoint(opp.getPosition()+opp.getVelocity())/*-(conf/2)*/<1.3/*opp.getPT().dCatchableAreaL*/ && posOnLine.isBetweenXY(start+RNoiseVel,end) ){
//            //return false;

//        //}

//        Vector2D LEnd=start+Vector2D::polar2vector(start.dist(end),LNoiseVel.dir().degree());
//        line=Line2D(start,LEnd);

//        if(cir.intersection(line,&temp1,&temp2)==false)
//            ;
//        else if(temp1.isBetweenXY(start+LNoiseVel-vel,end) || temp2.isBetweenXY(start+LNoiseVel-vel,end))
//            return false;
//        //posOnLine=line.projection(opp.getPosition()+opp.getVelocity());

//        //if(line.getDistanceWithPoint(opp.getPosition()+opp.getVelocity())/*-(conf/2)*/<1.3/*opp.getPT().dCatchableAreaL*/ && posOnLine.isBetweenXY(start+LNoiseVel,end) ){
//            //return false;

//        //}


//    }

//    if(withRand==false){
//        Simul   ball(agent,SIM_BALL);

//        ball.setVelocity(vel);

//        Vector2D posBall=start/*+vel*/;
//        ball.setPosition(posBall);
//        int time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

//        if(time==22771)
//            return false;

//        Simul   temp(agent);

//        for(int i=step ; i<=time ; i++){


//            temp = opp;


//            int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


//            double dis;
//            if(i-turnTime+config/*+conf*/<=0)
//                dis=0;
//            else
//                dis=temp.Sn(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime+config/*+conf*/);

//            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
//                return false;
//            }


//            temp = opp;


//            Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);


//            turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);



//            if(i-turnTime+config/*+conf*/<=0)
//                dis=0;
//            else
//                dis=temp.Sn(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime+config/*+conf*/);

//            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
//                return false;
//            }

//            vel*=0.94;
//            posBall+=vel;

//        }
//    }
//    else{
//        //////////                    back noise

//        Simul   ball(agent,SIM_BALL);

//        ball.setVelocity(BNoiseVel);

//        Vector2D posBall=start+BNoiseVel-vel;
//        ball.setPosition(posBall);
//        int time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

//        if(time==22771)
//            return false;

//        Simul   temp(agent);
//        for(int i=step ; i<=time ; i++){


//            temp = opp;


//            int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


//            double dis;
//            if(i-turnTime+config/*+conf*/<=0)
//                dis=0;
//            else
//                dis=temp.Sn(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime+config/*+conf*/);

//            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
//                return false;
//            }


//            temp = opp;


//            Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);


//            turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);



//            if(i-turnTime+config/*+conf*/<=0)
//                dis=0;
//            else
//                dis=temp.Sn(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime+config/*+conf*/);

//            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
//                return false;
//            }

//            BNoiseVel*=0.94;
//            posBall+=BNoiseVel;

//        }
//        //////////                    right noise


//        ball.setVelocity(RNoiseVel);

//        posBall=start+RNoiseVel-vel;
//        ball.setPosition(posBall);

//        time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

//        if(time==22771)
//            return false;

////        Simul   temp(agent);
//        for(int i=step ; i<=time ; i++){


//            temp = opp;


//            int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


//            double dis;
//            if(i-turnTime+config/*+conf*/<=0)
//                dis=0;
//            else
//                dis=temp.Sn(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime+config/*+conf*/);

//            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
//                return false;
//            }


//            temp = opp;


//            Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);


//            turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);



//            if(i-turnTime+config/*+conf*/<=0)
//                dis=0;
//            else
//                dis=temp.Sn(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime+config/*+conf*/);

//            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
//                return false;
//            }

//            RNoiseVel*=0.94;
//            posBall+=RNoiseVel;

//        }
//        //////////                    left noise


//        ball.setVelocity(LNoiseVel);

//        posBall=start+LNoiseVel-vel;
//        ball.setPosition(posBall);

//        time=ball.calculateBallTimeTo(start.dist(end),minSpeed);

//        if(time==22771)
//            return false;

////        Simul   temp(agent);
//        for(int i=step ; i<=time ; i++){


//            temp = opp;


//            int turnTime=temp.turnTimeToPlusSim(posBall,ANG_TURN);


//            double dis;
//            if(i-turnTime+config/*+conf*/<=0)
//                dis=0;
//            else
//                dis=temp.Sn(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime+config/*+conf*/);

//            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
//                return false;
//            }


//            temp = opp;


//            Vector2D posBack=temp.getPosition()+(temp.getPosition()-posBall);


//            turnTime=temp.turnTimeToPlusSim(posBack,ANG_TURN);



//            if(i-turnTime+config/*+conf*/<=0)
//                dis=0;
//            else
//                dis=temp.Sn(temp.getVelocity().r()+edp,temp.getDecay(),edp,i-turnTime+config/*+conf*/);

//            if(temp.getPosition().dist(posBall)<dis+1.3/**temp.getPT().dCatchableAreaL*/ /*+(1/i)*/){
//                return false;
//            }

//            LNoiseVel*=0.94;
//            posBall+=LNoiseVel;

//        }
//    }
//    return true;
//}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//double MostafPass::oneStepRand(PlayerAgent * agent,double dPower){

//    const WorldModel & wm = agent->world();

//    double dir_diff=fabs(AngleDeg::normalize_angle(wm.self().body().degree()-(wm.ball().pos()-wm.self().pos()).dir().degree()));
//    double dist=wm.self().pos().dist(wm.ball().pos())-0.385;

//    double pos_rate = 0.5 + 0.25 * ( dir_diff/180 + dist/wm.self().playerType().kickableMargin() );    // [0.5, 1.0]

//    double speed_rate = 0.5 + 0.5 * ( wm.ball().vel().length() / ( /*ball maxSpeed*/3 * /* ballDecay */ 0.94 ) );

//    // [0, 2*kick_rand]
//    return wm.self().playerType().kickRand() * ( dPower/ /* maxPower*/ 100 ) * ( pos_rate + speed_rate );


//}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//double MostafPass::twoStepRand(PlayerAgent * agent,Body_SmartKick kick,double dPower){

//    const WorldModel & wm = agent->world();

//    int step=kick.sequence().pos_list_.size();
//    Vector2D posBall=kick.sequence().pos_list_.at(step-2);

//    Vector2D posAgent=wm.self().pos()+wm.self().vel();
//    if(step==3)
//        posAgent+=wm.self().vel()*0.4;

//    double dir_diff=fabs(AngleDeg::normalize_angle(wm.self().body().degree()-(posBall-posAgent).dir().degree()));
//    double dist=posAgent.dist(posBall)-0.385;

//    double pos_rate = 0.5 + 0.25 * ( dir_diff/180 + dist/wm.self().playerType().kickableMargin() );    // [0.5, 1.0]

//    double speed_rate = 0.75;//0.5 + 0.5 * ( wm.ball().vel().length() / ( /*ball maxSpeed*/3 * /* ballDecay */ 0.94 ) );

//    // [0, 2*kick_rand]
//    return wm.self().playerType().kickRand() * ( dPower/ /* maxPower*/ 100 ) * ( pos_rate + speed_rate );


//}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//double MostafPass::kickRand(PlayerAgent * agent,Body_SmartKick kick,double dPower){

//    const WorldModel & wm = agent->world();
//    int step=kick.sequence().pos_list_.size();
//    Vector2D posBall,posAgent;
//    double  ballSpeed;

//    if(step==1){
//        posBall=wm.ball().pos();
//        posAgent=wm.self().pos();
//        ballSpeed=wm.ball().vel().length();

//    }
//    else if(step==2){
//        posBall=kick.sequence().pos_list_.at(0);
//        posAgent=wm.self().pos()+wm.self().vel();
//        ballSpeed=/*kick.sequence().speed_*0.3*/0;

//    }
//    else if(step==3){
//        posBall=kick.sequence().pos_list_.at(1);
//        posAgent=wm.self().pos()+wm.self().vel()+wm.self().vel()*wm.self().playerType().playerDecay();
//        ballSpeed=/*kick.sequence().speed_*0.3*/0;

//    }
//    else{
//        cout<<"hey jo"<<endl;
//        return 1000;

//    }




//    double dir_diff=fabs(AngleDeg::normalize_angle(wm.self().body().degree()-(posBall-posAgent).dir().degree()));
//    double dist=posAgent.dist(posBall)-0.385;

//    double pos_rate = 0.5 + 0.25 * ( dir_diff/180 + dist/wm.self().playerType().kickableMargin() );    // [0.5, 1.0]

//    double speed_rate = 0.5 + 0.5 * ( ballSpeed / ( /*ball maxSpeed * ballDecay */ 2.82 ) );

//    // [0, 2*kick_rand]
//    return wm.self().playerType().kickRand() * ( kick.sequence().power_/ /* maxPower*/ 100 ) * ( pos_rate + speed_rate );


//}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//bool    MostafPass::isInGoal(Vector2D start,Vector2D end,double speed,double rand,double Y){

//    double  dir=(end-start).dir().degree();
//    Vector2D velKick=Vector2D::polar2vector(speed,dir);
//    Vector2D RNoise=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir+90));
//    Vector2D LNoise=Vector2D::polar2vector(rand,AngleDeg::normalize_angle(dir-90));
//    double  RDir=(velKick+RNoise).dir().degree();
//    double  LDir=(velKick+LNoise).dir().degree();

//    Line2D    RLine=Line2D(start-velKick,RDir);
//    Line2D    LLine=Line2D(start-velKick,LDir);

//    Line2D    line=Line2D(Vector2D(52.5,0),90);

//    Vector2D RPos=line.intersection(RLine);
//    Vector2D LPos=line.intersection(LLine);

//    if(RPos.y>(Y) || RPos.y<(Y*(-1)) || LPos.y>(Y) || LPos.y<(Y*(-1)))
//        return false;


//    return true;

//}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//bool    MostafPass::setKick(const WorldModel & wm,NewKick &kick,Body_SmartKick a){

//    kick.cycle=wm.time().cycle();
//    kick.step=a.sequence().pos_list_.size();
//    kick.end=a.M_target_point;
//    kick.M_speed=a.M_first_speed_thr;
//    kick.speed=a.M_first_speed;
//    return kick.step;
//};
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//Vector2D    MostafPass::startKick(const WorldModel & wm,Body_SmartKick a){

////    if(a.sequence().pos_list_.size() > 1 )
////        return a.sequence().pos_list_.at(a.sequence().pos_list_.size()-2);
////    else if(a.sequence().pos_list_.size() == 1 )
////        return wm.ball().pos();
////    else{
////        cout<<"hey jo at startkick"<<endl;
////        return Vector2D(1000,1000);
////    }

//    if(a.sequence().pos_list_.size() > 0 )
//        return a.sequence().pos_list_.at(a.sequence().pos_list_.size()-1);
//    else{
//        cout<<"hey jo at startkick"<<endl;
//        return Vector2D(1000,1000);
//    }

//}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//int    MostafPass::stepKick(Body_SmartKick a){

//        return a.sequence().pos_list_.size();

//}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//bool    MostafPass::SHOOOOOOT(PlayerAgent * agent,NewKick &kick){


//    const WorldModel & wm = agent->world();

//    Vector2D start=wm.self().pos();


//    if(start.dist(Vector2D(52.5,0.0))>20)
//        return false;


//    double y=-7.0;
//    for(int i=0;i<=56;y+=0.25 && i++){
//        Vector2D end=Vector2D(52.5,y);

//        int conf=getCount(wm,(end-start).dir().degree(),5);

//        double minSpeed=1.0+(start.dist(end)*0.06);

//        Body_SmartKick smart(end,3,minSpeed,1);





//        if(conf>3 )
//            continue;
//        if(!smart.execute(agent) )
//            continue;
//        double rand = kickRand(agent,smart);
//        if(!isInGoal((startKick(wm,smart)),(end),smart.sequence().speed_,rand,7.0))
//            continue;

//        if(OCI(agent,(startKick(wm,smart)),(end),smart.sequence().speed_,10,0.3,stepKick(smart),rand,true,0)){

//            setKick(wm,kick,smart);
//            return true;
//        }



//    }
//    y=-7.0;
//    for(int i=0;i<=56;y+=0.25 && i++){
//        Vector2D end=Vector2D(52.5,y);

//        int conf=getCount(wm,(end-start).dir().degree(),5);

//        double minSpeed=1.0+(start.dist(end)*0.06);

//        Body_SmartKick smart(end,3,minSpeed,2);





//        if(conf>3 )
//            continue;
//        if(!smart.execute(agent) )
//            continue;

//        double rand = kickRand(agent,smart);
//        if(!isInGoal((startKick(wm,smart)),(end),smart.sequence().speed_,rand,7.0))
//            continue;

//        if(OCI(agent,(startKick(wm,smart)),(end),smart.sequence().speed_,10,0.3,stepKick(smart),rand,true,0)){

//            setKick(wm,kick,smart);
//            return true;
//        }



//    }

//    y=-7.0;
//    for(int i=0;i<=56;y+=0.25 && i++){
//        Vector2D end=Vector2D(52.5,y);

//        int conf=getCount(wm,(end-start).dir().degree(),5);

//        double minSpeed=1.0+(start.dist(end)*0.06);

//        Body_SmartKick smart(end,3,minSpeed,3);





//        if(conf>3 )
//            continue;
//        if(!smart.execute(agent) )
//            continue;

//        double rand = kickRand(agent,smart);

//        if(!isInGoal((startKick(wm,smart)),(end),smart.sequence().speed_,rand,7.0))
//            continue;

//        if(OCI(agent,(startKick(wm,smart)),(end),smart.sequence().speed_,10,0.3,stepKick(smart),rand,true,0)){

//            setKick(wm,kick,smart);
//            return true;
//        }



//    }


//    return false;

//}


//double      MostafPass::flagScoring(PlayerAgent * agent,Vector2D    pos,Vector2D posFrom,int minConf){

//    const WorldModel & wm = agent->world();

//    Vector2D posOpp[11];
//    int nr=0;

//    for (int i = 1; i < 12; i++)
//        if(wm.theirPlayer(i) && wm.theirPlayer(i)->posCount() <= minConf){

//            posOpp[nr]=(wm.theirPlayer(i)->pos());
//            nr++;
//        }

//    double Rad=reRate(pos.x,-52.5,60.5,7,15,RRM_REVERSE);

//    if(posFrom.x<30){

//        Vector2D	score=Vector2D(0.0,0.0);

//        for (int j = 0; j < nr; j++)
//        {



//                Vector2D	R=posOpp[j]-pos;

//                double  dir=0,dis=0;
//                if(R.r()>Rad)
//                    continue;
//                dis=reRate(Rad-R.r(),0,Rad,0,10);
//                int s=(R.dir().degree())/fabs(R.dir().degree());
//                dir=fabs(R.dir().degree());
//                dir=reRate(dir,0,180,0,60,RRM_DIRECT);
//                dir*=s;

//                score+=Vector2D::polar2vector(dis,AngleDeg::normalize_angle(dir+180));


//        }

//        double offScore=reRate(wm.offsideLineX()-pos.x,-8,50,0,33,RRM_REVERSE);
//        Vector2D Z=Vector2D::polar2vector(offScore,0);
//        score+=Z;
//        Vector2D pos=Vector2D::polar2vector(10,0);
//        double X=dakheli(score,pos)/(pow(pos.r(),2));
//        pos*=X;
//        int s=1;
//        if(fabs(score.dir().degree())>90)
//            s=-1;
//        return  s*score.r() ;


//    }
//    else{

//        double dirTo=0;
//        Rad=7;
//        double offScore=20-reRate(disToLine(pos,Vector2D(51,7),Vector2D(51,-7),dirTo),0,30,0,20,RRM_DIRECT);
//        Vector2D Z=Vector2D::polar2vector(offScore,dirTo);

//        Vector2D	score=Vector2D(0.0,0.0);
//        for (int j = 0; j < 11; j++)
//        {

//                Vector2D	R=posOpp[j]-pos;

//                double  dir=0,dis=0;
//                if(R.r()>Rad)
//                    continue;
//                dis=reRate(Rad-R.r(),0,7,0,Rad);
//                dir=AngleDeg::normalize_angle(R.dir().degree()-dirTo);
//                int s=(dir)/fabs(dir);
//                dir=reRate(fabs(dir),0,180,0,60,RRM_DIRECT);
//                dir*=s;

//                score+=Vector2D::polar2vector(dis,AngleDeg::normalize_angle(dir+dirTo+180));


//        }

//        score=score*(((  1.052  )))+Z*(((   2.6   )));
//        Vector2D pos=Vector2D::polar2vector(10,dirTo);
//        double X=dakheli(score,pos)/(pow(pos.r(),2));
//        pos*=X;
//        int s=1;
//        if(fabs(AngleDeg::normalize_angle(score.dir().degree()-dirTo))>90)
//            s=-1;
//       return  s*score.r() ;


//    }


//}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//spd      MostafPass::getDirectSpeed(Vector2D start,Vector2D end){

//    spd speed;


//    // evolate speed
//    // A1 = An*q+Sn*(1-q)
//    speed.max=doubleNormalize(1.8/*finalMaxSpeed*/*0.94/*ballDecay*/+start.dist(end)*0.06/*1-ballDecay*/,0.0,3.0);

//    if(start.x > 38 )
//        speed.max=doubleNormalize(2.2/*finalMaxSpeed*/*0.94/*ballDecay*/+start.dist(end)*0.06/*1-ballDecay*/,0.0,3.0);

//    speed.min=doubleNormalize(0.4/*finalMinSpeed*/*0.94/*ballDecay*/+start.dist(end)*0.06/*1-ballDecay*/,0.0,2.4);


//    return speed;
//}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//spd      MostafPass::getLeadSpeed(PlayerAgent * agent,int nr,Vector2D start,Vector2D end,int step,int avans){

//    const WorldModel & wm = agent->world();
//    spd speed;

//    if(!wm.ourPlayer(nr)){
//        speed.max=0.0;
//        speed.min=0.0;
//        return speed;
//    }
//    Simul   sim(agent,SIM_TEAMMATE,nr);


//    double     time=sim.dashTimeTo(end)+avans/*+0.5*/;

//    //  S*(1-q)/(1-pow(q,n))

//    double a=doubleNormalize(start.dist(end)*(0.06)/(1-pow(0.94,time)),0.0,3.0);
//    double b=doubleNormalize(0.4/*finalMinSpeed*/*0.94/*ballDecay*/+start.dist(end)*0.06/*1-ballDecay*/,0.0,3.0);

//    speed.max=max(a,b);
//    speed.min=min(a,b);
//    return speed;



//}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//spd      MostafPass::getThroughSpeed(PlayerAgent * agent,int nr,Vector2D start,Vector2D end,int step,int &time,int avans){

//    const WorldModel & wm = agent->world();
//    spd speed;

//    if(!wm.ourPlayer(nr)){
//        speed.max=0.0;
//        speed.min=0.0;
//        return speed;
//    }
//    Simul   sim(agent,SIM_TEAMMATE,nr);


////    int     time;//=sim.dashTimeTo(end);

//    if(wm.ourPlayer(nr)->bodyCount()<2)
//        time=sim.timeTo(end)+avans;
//    else
//        time=sim.dashTimeTo(end)+avans;

//    //  S*(1-q)/(1-pow(q,n))

//    double a=doubleNormalize(start.dist(end)*(0.06)/(1-pow(0.94,time)),0.0,3.0);
//    double b=doubleNormalize(0.3/*MinfinalSpeed*/*0.94/*ballDecay*/+start.dist(end)*0.06/*1-ballDecay*/,0.0,3.0);

//    speed.max=max(a,b);
//    speed.min=min(a,b);
//    return speed;

//}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//bool	MostafPass::directPass(PlayerAgent * agent,NewKick &kicking,int &nr,bool better){

//    const WorldModel & wm = agent->world();

//    Vector2D	agentPos=((wm.self().pos()));
//    int		agentNum=wm.self().unum();
//    Vector2D posBall=(wm.ball().pos());
//    double	agentScore=flagScoring(agent,posBall,agentPos);

//    struct structer{
//        bool			can;
//        Vector2D		pos;
//        Vector2D             start;
//        double			maxSpeed;
//        int			step;
//        int			conf;
//        int			num;
//        double			score;
//        double                  rand;
//        NewKick                 kick;
//    } points[30];

//    int k=0;

//    for( int i=2 ; i < 12 ;	i++)
//        if(agentNum!=i && wm.ourPlayer(i) && wm.ourPlayer(i)->posCount()<3 && getCount(wm,wm.ourPlayer(i)->pos(),5)<2 && isInField((wm.ourPlayer(i)->pos()))){
//        for (int j = 1; j <= 3; j++)
//        {
//            points[k].num=i;
//            points[k].pos=(wm.ourPlayer(i)->pos()+wm.ourPlayer(i)->vel());

////            if(wm.ourPlayer(i)->bodyCount()<2)
////                points[k].pos+=Vector2D::polar2vector(0.5,wm.ourPlayer(i)->body().degree());

//            spd A=getDirectSpeed(agentPos,points[k].pos);

//            Body_SmartKick smart((points[k].pos),A.max,A.min,j);

//            points[k].can=smart.execute(agent);
//            if(!points[k].can)
//                continue;

//            points[k].conf=getCount(wm,(points[k].pos),calculate_PM(wm,agentPos,points[k].pos,smart.sequence().speed_,0.15));


//            if(  points[k].conf>1  ){
//                //points[k].can =false;
//                continue;
//            }
//            Simul a;
//            setKick(wm,points[k].kick,smart);
//            points[k].start=(startKick(wm,smart));
//            points[k].maxSpeed=smart.sequence().speed_;
//            points[k].step=stepKick(smart);
//            points[k].rand=kickRand(agent,smart);
//            //points[k].conf=getCount(wm,wm.ourPlayer(i)->pos(),5);
////            cout<<"max speed"<<points[k].start.dist(points[k].pos)<<endl;
////            cout<<"n : "<<a.N(points[k].start.dist(points[k].pos),points[k].maxSpeed,0.94,true)<<endl;
//            points[k].score=flagScoring(agent,points[k].pos,agentPos)*pow(XFACTOR,-2+a.N(points[k].start.dist(points[k].pos),points[k].maxSpeed,0.94,true));
//            k++;
//        }
//    }




//    for (int i = 0; i < k; i++)
//        for (int j=i+1; j < k; j++)
//            if (points[i].score<points[j].score){
//        typeSwap(points[i].can,points[j].can);
//        typeSwap(points[i].num,points[j].num);
//        typeSwap(points[i].step,points[j].step);
//        typeSwap(points[i].conf,points[j].conf);
//        typeSwap(points[i].pos,points[j].pos);
//        typeSwap(points[i].start,points[j].start);
//        typeSwap(points[i].score,points[j].score);
//        typeSwap(points[i].rand,points[j].rand);
//        typeSwap(points[i].maxSpeed,points[j].maxSpeed);
//        typeSwap(points[i].kick,points[j].kick);
//    }


//    for (int i = 0; i < k; i++)
//    {
//        if( better==false || agentScore<points[i].score)
//        {
//            if( points[i].pos.dist(agentPos)>3 /** set && points[i].oppInCircle==0*/ ){

//                if(points[i].can && OCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,1) ){
//                    kicking=points[i].kick;
//                    //sayPassRequest(intetnt.endPos,points[i].num);
//                    nr=points[i].num;
//                    return true;
//                }

//            }
//        }
//    }


//    return false;
//}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//bool        MostafPass::isInField(Vector2D pos){

//    if( -33<pos.y && pos.y<33 && -48<pos.x && pos.x<51.5)
//        return true;

//    return false;



//}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//void        MostafPass::setInField(Vector2D &pos,double maxX){

//    if( fabs(pos.y) > 33.0 )
//        pos.y = ( 33.0 * (fabs(pos.y )/pos.y) ) ;

//    if (fabs( pos.x ) >48.0 )
//        pos.x = ( 51.0 * (fabs(pos.x )/pos.x) ) ;

//    return ;
//}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//int MostafPass::timeToReach(double speed,double dis,int & time){


//    if(dis>(speed - 0.3)/0.06)
//        return 100.0;

//    double X=1-(dis*(0.06)/speed);

//    time =ceil((log10(X))/(log10(0.94))) - time;

//    return time;


//}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//bool        MostafPass::throughPass(PlayerAgent * agent,NewKick &kicking,int &nr,bool better){

//    const WorldModel & wm = agent->world();

//    Vector2D posAgent=(wm.self().pos());
//    Vector2D posBall=(wm.ball().pos());
//    if(posAgent.x>32)
//        return false;
//    double	agentScore=flagScoring(agent,posBall,posAgent);





//    struct structer{
//        bool			can;
//        Vector2D		pos;
//        Vector2D             start;
//        double			maxSpeed;
//        int			step;
//        int			conf;
//        int			num;
//        double			score;
//        double                  rand;
//        NewKick                 kick;
////        int             timing;
//    } points[162];

//    int k=0;
//    /*Simul agent(agent);
//    agent.getSimulFromType(wm,agent.getAgentObjectType(wm),SIM_PLAYER);
//*/
//    double  TG=tanDeg(20);
//    for(int i=9 ; i<12 ; i++){

//        if(!wm.ourPlayer(i))
//            continue;
//        if(i==wm.self().unum() || wm.ourPlayer(i)->posCount()>4)
//            continue;

//        Vector2D posTeam=(wm.ourPlayer(i)->pos());

//        for(int s=1;s<=3;s++)

//            for(int j=0;j<6;j++)
//                for(int f=0;f<3;f++){

//                    points[k].pos=posTeam+Vector2D(j*5+10,(TG-(TG*f))*(j*5+10));


////                    if( isInField(points[k].pos)==false   )
////                        continue;

//                    setInField(points[k].pos);

//                    /**make true Simul   opp(WM);
//                    opp.getSimulFromType(WM->getClosestInSetTo(OBJECT_SET_OPPONENTS,points[k].pos),SIM_PLAYER);
//                    if(opp.timeTo(points[k].pos)<agent.timeTo(points[k].pos)){
//                        points[k].can =false;
//                        continue;

//                    }*/
//                    points[k].step=s;
//                    points[k].num=i;
//                    int time = 1;
//                    spd A=getThroughSpeed(agent,points[k].num,posAgent,points[k].pos,points[k].step,time,2);

//                    Body_SmartKick smart((points[k].pos),A.max,A.min,points[k].step);

//                    points[k].can=smart.execute(agent);
//                    if(!points[k].can)
//                        continue;


//                    points[k].conf=getCount(wm,(points[k].pos),calculate_PM(wm,posAgent,points[k].pos,smart.sequence().speed_,0.15));


//                    if(  points[k].conf>2  ){
//                        //points[k].can =false;
//                        continue;
//                    }
//                    setKick(wm,points[k].kick,smart);
//                    points[k].start=(startKick(wm,smart));
//                    points[k].maxSpeed=smart.sequence().speed_;
//                    points[k].step=stepKick(smart);
//                    points[k].rand=kickRand(agent,smart);
//                    Simul a;
//                    points[k].score=flagScoring(agent,points[k].pos,posAgent)/**pow(XFACTOR,-2+a.N(points[k].start.dist(points[k].pos),points[k].maxSpeed,0.94,true))*/;
////                    points[k].timing = timeToReach(smart.sequence().speed_ , points[k].pos.dist(posAgent),time);
////cout<<"                                "<<points[k].timing<<endl;
//                    k++;
//            }
//    }

//    for (int i = 0; i < k; i++)
//        for (int j=i+1; j < k; j++)
//            if (points[i].score<points[j].score){
//        typeSwap(points[i].can,points[j].can);
//        typeSwap(points[i].num,points[j].num);
//        typeSwap(points[i].step,points[j].step);
//        typeSwap(points[i].conf,points[j].conf);
//        typeSwap(points[i].pos,points[j].pos);
//        typeSwap(points[i].start,points[j].start);
//        typeSwap(points[i].score,points[j].score);
//        typeSwap(points[i].rand,points[j].rand);
//        typeSwap(points[i].maxSpeed,points[j].maxSpeed);
//        typeSwap(points[i].kick,points[j].kick);
////        typeSwap(points[i].timing ,points[j].timing );
//    }

//    for (int i = 0; i < k; i++)
//    {
//        if( better==false || agentScore<points[i].score)
//        {
//            if( points[i].pos.dist(posAgent)>3 /** set && points[i].oppInCircle==0*/ ){
//                if(points[i].can && OCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,2) ){
//                    kicking=points[i].kick;
//                    //sayPassRequest(intetnt.endPos,points[i].num);
////                    cout<<"                                "<<points[i].timing<<endl;
//                    nr=points[i].num;
//                    return true;
//                }

//            }
//        }
//    }

//        return false;
//}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

//bool        MostafPass::leadingPass(PlayerAgent * agent,NewKick &kicking,int &nr,bool better){

//    const WorldModel & wm = agent->world();

//    if(wm.ball().pos().x<-30)
//        return false;
//    Vector2D posBall=(wm.ball().pos());
//    Vector2D posAgent=(wm.self().pos());
//    double	agentScore=flagScoring(agent,posBall,posAgent);





//    struct structer{
//        bool			can;
//        Vector2D		pos;
//        Vector2D             start;
//        double			maxSpeed;
//        int			step;
//        int			conf;
//        int			num;
//        double			score;
//        double                  rand;
//        NewKick                 kick;
//    } points[300];

//    int k=0;


//    for(int i=2 ; i<12 ; i++){

//        if(!wm.ourPlayer(i))
//            continue;
//        if(i==wm.self().unum() || wm.ourPlayer(i)->seenPosCount() > 1 || getCount(wm,wm.ourPlayer(i)->pos(),5) > 1 /*|| (wm.ourPlayer(i)->bodyCount()!=0 && wm.ourPlayer(i)->pos().dist(wm.self().pos())<40)*/)
//            continue;

//        Vector2D posTeam=(wm.ourPlayer(i)->pos()+wm.ourPlayer(i)->vel());
//        double dir=0;
//        if(wm.ourPlayer(i)->bodyCount()==0)
//            dir=wm.ourPlayer(i)->body().degree();
//        for(int s=1;s<=3;s++)

//            for(int j=0;j<10;j++){

//                points[k].pos=posTeam+Vector2D::polar2vector(j+1,dir);


//                if( isInField(points[k].pos)==false   ){
//                    //points[k].can =false;
//                    continue;
//                }
//                /**make true Simul   opp(WM);
//                opp.getSimulFromType(WM->getClosestInSetTo(OBJECT_SET_OPPONENTS,points[k].pos),SIM_PLAYER);
//                if(opp.timeTo(points[k].pos)<agent.timeTo(points[k].pos)){
//                    points[k].can =false;
//                    continue;

//                }*/
//                points[k].step=s;
//                points[k].num=i;
//                spd A=getLeadSpeed(agent,points[k].num,posAgent,points[k].pos,points[k].step,1);

//                Body_SmartKick smart((points[k].pos),A.max,A.min,points[k].step);

//                points[k].can=smart.execute(agent);
//                if(!points[k].can)
//                    continue;

//                points[k].conf=getCount(wm,(points[k].pos),calculate_PM(wm,posAgent,points[k].pos,smart.sequence().speed_,0.15));


//                if(  points[k].conf!=0  ){
//                    //points[k].can =false;
//                    continue;
//                }
//                setKick(wm,points[k].kick,smart);
//                points[k].start=(startKick(wm,smart));
//                points[k].maxSpeed=smart.sequence().speed_;
//                if(points[k].maxSpeed<0.9)
//                    continue;
//                points[k].step=stepKick(smart);
//                points[k].rand=kickRand(agent,smart);
//                Simul a;
//                points[k].score=flagScoring(agent,points[k].pos,posAgent)*pow(XFACTOR,-2+a.N(points[k].start.dist(points[k].pos),points[k].maxSpeed,0.94,true));

//                k++;
//            }
//    }


//    for (int i = 0; i < k; i++)
//        for (int j=i+1; j < k; j++)
//            if (points[i].score<points[j].score){
//        typeSwap(points[i].can,points[j].can);
//        typeSwap(points[i].num,points[j].num);
//        typeSwap(points[i].step,points[j].step);
//        typeSwap(points[i].conf,points[j].conf);
//        typeSwap(points[i].pos,points[j].pos);
//        typeSwap(points[i].start,points[j].start);
//        typeSwap(points[i].score,points[j].score);
//        typeSwap(points[i].rand,points[j].rand);
//        typeSwap(points[i].maxSpeed,points[j].maxSpeed);
//        typeSwap(points[i].kick,points[j].kick);
//    }

//    for (int i = 0; i < k; i++)
//    {
//        if( better==false || agentScore<points[i].score)
//        {
//            if( points[i].pos.dist(posAgent)>3 /** set && points[i].oppInCircle==0*/ ){

//                if(points[i].can && OCI(agent,points[i].start,points[i].pos,points[i].maxSpeed,6,0.3,points[i].step,points[i].rand,false,3) ){
//                    kicking=points[i].kick;
//                    //sayPassRequest(intetnt.endPos,points[i].num);
//                    nr=points[i].num;
//                    return true;
//                }

//            }
//        }
//    }


//        return false;
//}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

////double	MostafPass::posScoreInFieldForUs(Vector2D pos){

////    return ((110.368-Vector2D(52.5,0.0).dist(pos))+((34-fabs(pos.y))/2.5)+((pos.x+52.5)/1.12))/2.194225455;
////    //return ((110.368-Vector2D(52.5,0.0).dist(pos))+((34-fabs(pos.y))/4.5)+((pos.x+70.5)/1.0))/2.194225455;

////}
////double	MostafPass::getCofficient(double Ang){

////    double	temp=Ang/2;

////    return ((cosDeg(temp))+1)/2;

////}
////double          MostafPass::FinalMostafScoring(Vector2D pos,Vector2D inform,int minimalConf){

////    const WorldModel & wm = agent->world();

////    double scorePos = posScoreInFieldForUs(pos);

////    scorePos = WM->reRate(scorePos,0,108.520297883,0,120);


////    double dis = (inform.x+52.5);

////    dis = WM->reRate(dis,0,105,0,100);

////    dis = 110 - dis;
////    dis /= 8.33 ;

////    //dis = WM->reRate(dis , 1.2 , 9 , 3 , 12 );

////    double congScore = 0;

////    Vector2D	temp=Vector2D(55.0,0.0);
////    if(pos.x<20)temp=Vector2D(55,temp.y);

////    double dirToGoal=(temp-pos).dir().degree();

////    bool is=false;

////    for (int i = 1; i < 12; i++)
////        if((1-WM->getConfidence(WM->objOpponent(i)))*100<=minimalConf){
////        if(pos.dist(WM->posOpponent(i))<dis)
////            congScore+=dis-((pos.dist(WM->posOpponent(i)))*getCofficient(AngleDeg::normalize_angle(fabs((dirToGoal-((WM->posOpponent(i))-pos).dir().degree())))));
////        if(pos.dist((WM->posOpponent(i)))<2.0)
////            is=true;
////    }

////    congScore = WM->reRate(congScore,0,dis*0.75,0,100);

////    congScore = 100 -congScore;

////    congScore = WM->reRate(congScore,0,100,0,200);

////    if(is)congScore*=0.5;

////    return congScore + scorePos ;

////}


//Vector2D MostafPass::turnPoint( PlayerAgent * agent,int ballMinConf )
//{
//    const WorldModel & wm = agent->world();

//    Vector2D posAgent  = (wm.self().pos());
//    Vector2D posBall   = (wm.ball().pos());
//    double      bodyAngle = wm.self().body().degree();

//    double view=wm.self().viewWidth();

////    switch( wm.self().viewWidth() )
////    {
////            case ViewWidth::NARROW:
////                    view = 60.0;
////                    break;
////            case ViewWidth::NORMAL:
////                    view = 120.0;
////                    break;
////            case ViewWidth::WIDE:
////                    view = 180.0;
////                    break;
////            default:
////                    view = 60.0;
////    }

//    if( wm.ball().posCount() > ballMinConf && getAngDiff( bodyAngle, ( posBall - posAgent ).dir().degree() ) < 90.0 + view )
//    {
//            return posBall;
//    }

//    /*ObjectT oppGoal = ( WM->getSide() == SIDE_LEFT ) ? OBJECT_GOAL_R : OBJECT_GOAL_L;

//    if( WM->getClosestInSetTo( OBJECT_SET_PLAYERS, OBJECT_BALL ) == WM->getAgentObjectType() &&
//        WM->getConfidence( oppGoal ) <= ballMinConf && getAngDiff( bodyAngle, ( WM->getGlobalPosition( oppGoal ) - posAgent ).dir().degree() ) < 90.0 + view )
//    {
//            return WM->getGlobalPosition( oppGoal );
//    }*/

//    double step = 15.0;

//    double start = AngleDeg::normalize_angle( bodyAngle - 90.0 );

//    multimap <int, double> angConf;
//    multimap <int, double>::reverse_iterator rit;
//    multimap <int, double>::reverse_iterator rit2;

//    for( int i = 0; i <= (int)(180.0/step); i++ )
//    {
//            double ang = AngleDeg::normalize_angle( start + i*step );

//            angConf.insert( pair<int,double>(wm.dirCount( ang ), ang) );
//    }

//    vector <double> angs;

//    for( rit = angConf.rbegin(); rit != angConf.rend(); rit++ )
//    {
//            bool good = true;
//            bool BREAK = false;
//            for( rit2 = rit;; rit2-- )
//            {
//                    if( rit2 == angConf.rbegin() )
//                            BREAK=true;

//                    // Log.log( 250,"diff between : %lf va %lf = %lf",rit->second,rit2->second, getAngDiff( rit->second, rit2->second ) );

//                    if( getAngDiff( rit->second, rit2->second ) > view )
//                    {
//                            good = false;
//                            break;
//                    }

//                    if( BREAK ) break;
//            }

//            if( !good )
//            {
//                    break;
//            }

//            // Log.log( 250,"push_back : %lf conf : %d",rit->second,rit->first );

//            angs.push_back( rit->second );
//    }

//    sort(angs.begin(),angs.end());

//    // Log.log( 250,"front : %lf back : %lf",angs.front(),angs.back() );

//    double finalAng1 = getAngBetween( angs.front(), angs.back() );
//    double finalAng2 = getAngBetween( angs.back(), angs.front() );

//    double finalAng = ( getAngDiff( finalAng1, bodyAngle ) < getAngDiff( finalAng2, bodyAngle ) ) ? finalAng1 : finalAng2;

//    // Log.log( 250,"finalAng : %lf ",finalAng );
//    // Log.log( 250,"finalAng2 : %lf ",finalAng2 );
//    // Log.log( 250,"body : %lf ",bodyAngle );
//    // Log.log( 250,"diff1 : %lf ",getAngDiff( finalAng1, bodyAngle ) );
//    // Log.log( 250,"diff2 : %lf ",getAngDiff( finalAng2, bodyAngle ) );

//    return posAgent + Vector2D::polar2vector( 5.0, finalAng  );
//}
//double MostafPass::getAngBetween( double ang1, double ang2, double n1, double n2 )
//{
//        // clockwise

//        ang1 = AngleDeg::normalize_angle(ang1);
//        ang2 = AngleDeg::normalize_angle(ang2);

//        n1 = fabs(n1);
//        n2 = fabs(n2);


//        if( ang1 <= ang2 )
//                return AngleDeg::normalize_angle( ((ang1*n1)+(ang2*n2))/(n1+n2) );
//        else
//        {
//                ang2 += 360.0;
//                return AngleDeg::normalize_angle( ((ang1*n1)+(ang2*n2))/(n1+n2) );
//        }
//}


//Vector2D MostafPass::seen(PlayerAgent *agent){


//    const WorldModel & wm = agent->world();

//    vector <Vector2D> pos;
//    vector <int > conf;



//    double myX=min(wm.self().pos().x-7.0,33.0);
//    for(int i=2;i<12;i++)
//        if(wm.ourPlayer(i)){




//            if(Strategy::i().getPosition(i).x>=myX){
//                pos.push_back(Strategy::i().getPosition(i));
//                conf.push_back(getCount(wm,pos.at(pos.size()-1),7));

//            }

//            if(i>8){
//                pos.push_back(Strategy::i().getPosition(i)+Vector2D(12,0));
//                conf.push_back(getCount(wm,pos.at(pos.size()-1),7));

//            }
//        }



//    for (int i = 0; i < pos.size(); i++)
//        for (int j=i+1; j < pos.size(); j++)
//            if(conf.at(i)<conf.at(j)){
//                typeSwap(conf.at(i),conf.at(j));
//                typeSwap(pos.at(i),pos.at(j));
//            }

//    double bodyAng=wm.self().body().degree();
//    for (int i = 0; i < pos.size(); i++);
//        //if()


//}


//Vector2D    MostafPass::where(PlayerAgent *agent, int ballMinConf){

//    const WorldModel  & wm  = agent->world();

////    View_ChangeWidth(ViewWidth::NARROW).execute(agent);
////    if(wm.self().viewWidth()!=ViewWidth::NARROW)
////        agent->doChangeView( ViewWidth::NARROW );


//    if( wm.ball().posCount() > ballMinConf )
//        if(fabs(AngleDeg::normalize_angle(((wm.ball().pos()-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107)
//            return wm.ball().pos();

//    int player[10][7]={ {3,4,6,7,8,9,11} ,//2
//                        {2,5,6,7,8,10,11} ,//3
//                        {2,3,6,7,8,9,11} ,//4
//                        {2,3,6,7,8,10,11} ,//5
//                        {7,8,9,10,11} ,//6
//                        {6,8,9,11} ,//7
//                        {6,7,10,11} ,//8
//                        {6,7,8,10,11} ,//9
//                        {6,7,8,9,11} ,//10
//                        {7,8,9,10}//11
//                      };

//    struct stc{
//        Vector2D    pos;
//        int         conf;

//        stc(){

//            pos = Vector2D(100,100);
//            conf= 22771;
//        }
//    } info[15];

//    int selfNr = wm.self().unum() - 2 ;
//    int nr = 0 ;



//    for (int i = 0; i < 7; ++i) {

//        if(player[selfNr][i]!=0 &&player[selfNr][i]!=wm.self().unum() && wm.ourPlayer(player[selfNr][i]) ){

//            info[nr].pos = wm.ourPlayer(player[selfNr][i])->pos();

//            if(wm.ourPlayer(player[selfNr][i])->bodyCount()<2){

//                Vector2D forward = Vector2D::polar2vector(2.5,wm.ourPlayer(player[selfNr][i])->body().degree());
//                info[nr].pos += forward;

//            }

//            if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){

//                info[nr].conf= getCount( wm , info[nr].pos , 5 );
//                nr++;
//            }

//            if(player[selfNr][i] > 8 && wm.ball().pos().x < 32 && wm.ball().pos().x > -20 ){


//                info[nr].pos = wm.ourPlayer(player[selfNr][i])->pos() + Vector2D(17.50 ,0.0  );
//                if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
//                    info[nr].conf= getCount( wm , info[nr].pos, 5 );
//                    nr++;
//                }


//            }
//            if(player[selfNr][i] > 8 && wm.ball().pos().x < 15 && wm.ball().pos().x > -20 ){


//                info[nr].pos = wm.ourPlayer(player[selfNr][i])->pos() + Vector2D(30.0 ,0.0  );
//                if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
//                    info[nr].conf= getCount( wm , info[nr].pos, 5 );
//                    nr++;
//                }


//            }
//        }
//    }

//    info[nr].pos = Vector2D(52.0 ,0.0  );
//    if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
//        info[nr].conf= getCount( wm , info[nr].pos, 5 );

//        if(wm.self().pos().dist(Vector2D(52.5,0.0))<17)
//            info[nr].conf*=2.5;
//        nr++;
//    }
//    info[nr].pos = Vector2D(52.0 ,5.0  );
//    if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
//        info[nr].conf= getCount( wm , info[nr].pos, 5 );
//        if(wm.self().pos().dist(Vector2D(52.5,0.0))<17)
//            info[nr].conf*=2.5;
//        nr++;
//    }
//    info[nr].pos = Vector2D(52.0 ,-5.0  );
//    if(fabs(AngleDeg::normalize_angle(((info[nr].pos-wm.self().pos()).dir().degree())-wm.self().body().degree()))<107){
//        info[nr].conf= getCount( wm , info[nr].pos, 5 );
//        if(wm.self().pos().dist(Vector2D(52.5,0.0))<17)
//            info[nr].conf*=2.5;
//        nr++;
//    }

//    for (int i = 0; i < nr; ++i) {
//        for (int j = i+1; j < nr ; ++j) {
//            if(info[i].conf<info[j].conf){
//                typeSwap(info[i].conf,info[j].conf);
//                typeSwap(info[i].pos,info[j].pos);
//            }

//        }

//    }

////    cout<<"cycle:"<<wm.time().cycle()<<"  nr:"<<wm.self().unum()<<"  ViewAction:"<<wm.self().viewWidth()<<"  conf:"<<info[0].conf<<"  to:"<<info[0].pos.x<<" , "<<info[0].pos.y<<endl;

//    return info[0].pos;

//}



//vector < Vector2D >    MostafPass::getDir(PlayerAgent *agent,Vector2D firsPos,Vector2D endPos,int minConf){

//    const WorldModel & wm = agent->world();

//    vector <AngleDeg> dir;
//    vector <int> nr;
//    vector <Vector2D > position;

//    Vector2D    selfPos=wm.self().pos();

//    AngleDeg from = AngleDeg::normalize_angle((firsPos-selfPos).dir().degree());
//    AngleDeg to = AngleDeg::normalize_angle((endPos-selfPos).dir().degree());



//    dir.push_back(from);
//    nr.push_back(200);
//    position.push_back(firsPos);

//    for (int i = 1 ; i < 12; ++i) {

//        if(wm.theirPlayer(i) && wm.theirPlayer(i)->posCount()<minConf && (wm.theirPlayer(i)->pos()-selfPos).dir().isWithin(from,to) ){
//            dir.push_back(AngleDeg::normalize_angle((wm.theirPlayer(i)->pos()-selfPos).dir().degree()));
//            nr.push_back(1);
//            position.push_back(wm.theirPlayer(i)->pos());


//        }

//    }

//    dir.push_back(to);
//    nr.push_back(0);
//    position.push_back(endPos);

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

//    return position;


//}
