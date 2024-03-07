// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef TOKYOTEC_BHV_BASIC_OFFENSIVE_KICK_H
#define TOKYOTEC_BHV_BASIC_OFFENSIVE_KICK_H

#include <rcsc/player/soccer_action.h>
#include"Pass.h"
#include"strategy.h"
static NewKick   Kicking;



#include<Simul.h>

class Bhv_BasicOffensiveKick
    : public rcsc::SoccerBehavior {
private:

public:

    bool    setKicking(const WorldModel & wm,Body_SmartKick a){

        Kicking.cycle=wm.time().cycle();
        Kicking.step=a.sequence().pos_list_.size();
        Kicking.end=a.M_target_point;
        Kicking.M_speed=a.M_first_speed_thr;
        Kicking.speed=a.M_first_speed;
    };

    bool execute( rcsc::PlayerAgent * agent );




};

#endif
