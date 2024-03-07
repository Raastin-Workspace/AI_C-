#ifndef ATKPOSITIONING_H
#define ATKPOSITIONING_H

#include <rcsc/geom/vector_2d.h>
#include <rcsc/player/soccer_action.h>
#include"Oci.h"
#include <rcsc/action/body_go_to_point.h>

#define DIST_DEFAULT 10
#define ANG_DEFAULT 20
#define STEP_DEFAULT 3
#define DIST_FRONT_DEFUALT 20

using namespace rcsc;


class Positioning
    : public rcsc::SoccerBehavior {
private:

public:
    double min(double a,double b);
    double max(double a,double b);

bool		isBallTheirs        ( PlayerAgent * agent ,double = 0.7, VecPosition = VecPosition( -1000, -1000 ) );
bool		isBallOurs          ( PlayerAgent * agent ,bool b = false                                );
int         getOppInCircle(PlayerAgent * agent ,Circle cir);
int         getTtInCircle(PlayerAgent * agent ,Circle cir);
bool        goTo(PlayerAgent *agent,Vector2D posTo,double bodyAng,double maxDashPower,double disWhenTrue=0.5);
bool        goTo2(PlayerAgent *agent,Vector2D posTo,double bodyAng,double maxDashPower,double disWhenTrue=0.5);

VecPosition targetPos(PlayerAgent * agent ,VecPosition center,int  number);
int         positioningRadius(VecPosition center);
int         coneScore(PlayerAgent * agent ,VecPosition center,VecPosition target,int minConf=10);
VecPosition circlePositioning(PlayerAgent * agent ,VecPosition center);
double      getConeDist(VecPosition pos,VecPosition start,VecPosition end);

bool        offenseMode(PlayerAgent * agent);
bool        newAtkPositioning(PlayerAgent *agent);


};

#endif
