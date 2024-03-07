// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
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

#ifndef BHV_ATK_MOVE_H
#define BHV_ATK_MOVE_H

#include <rcsc/geom/vector_2d.h>
#include <rcsc/player/soccer_action.h>

class Bhv_AttackerOffensiveMove
    : public rcsc::SoccerBehavior {
private:
    const rcsc::Vector2D M_home_pos;
    const bool M_forward_player;
public:
    Bhv_AttackerOffensiveMove( const rcsc::Vector2D & home_pos,
                               const bool forward_player )
        : M_home_pos( home_pos )
        , M_forward_player( forward_player )
      { }

    bool execute( rcsc::PlayerAgent * agent );

private:
    bool doForestall( rcsc::PlayerAgent * agent );
    bool doIntercept( rcsc::PlayerAgent * agent );
    bool doAvoidMarker( rcsc::PlayerAgent * agent );

    double getDashPower( const rcsc::PlayerAgent * agent,
                         const rcsc::Vector2D & target_point );
};

///
// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
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


#include <rcsc/geom/vector_2d.h>
#include <rcsc/player/soccer_action.h>

namespace rcsc {
class WorldModel;
}

class Body_ForestallBlock
    : public rcsc::BodyAction {
private:
    static int S_count_no_move;

    const rcsc::Vector2D M_home_position;

public:
    explicit
    Body_ForestallBlock( const rcsc::Vector2D & home_pos )
        : M_home_position( home_pos )
      { }

    bool execute( rcsc::PlayerAgent * agent );

private:

    rcsc::Vector2D getTargetPointSimple( const rcsc::WorldModel & wm );
    rcsc::Vector2D getTargetPoint( const rcsc::WorldModel & wm );

};


// -*-c++-*-

/*!
  \file body_intercept2008.h
  \brief ball chasing action including smart planning.
*/

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#include <rcsc/player/soccer_action.h>
#include <rcsc/player/intercept_table.h>
#include <rcsc/common/player_type.h>
#include <rcsc/geom/vector_2d.h>

namespace rcsc {

/*!
  \class Body_Intercept2008
  \brief ball chasing action including smart planning.
*/
class Body_Intercept2008
    : public BodyAction {
private:
    //! if true, agent must save recovery parameter
    const bool M_save_recovery;
    //! facing target target point. if specified this, plaeyr try to turn neck to this point.
    const Vector2D M_face_point;
public:
    /*!
      \brief construct with all parameters
      \param save_recovery if true, agent must save recovery parameter
    */
    explicit
    Body_Intercept2008( const bool save_recovery = true,
                        const Vector2D & face_point = Vector2D::INVALIDATED )
        : M_save_recovery( save_recovery )
        , M_face_point( face_point )
      { }

    /*!
      \brief execute action
      \param agent pointer to the agent itself
      \return true if action is performed
    */
    bool execute( PlayerAgent * agent );


private:

    /*!
      \brief if kickable opponent exists, perform special action
      \param agent pointer to the agent itself
      \return true if action is performed
    */
    bool doKickableOpponentCheck( PlayerAgent * agent );

    /*!
      \brief calculate best interception point using cached table
      \param wm const refefence to the WorldModel
      \param table const pointer to the cached table
      \return interception info object
    */
    //InterceptInfo getBestIntercept( const WorldModel & wm,
    //                                const InterceptTable * table ) const;

    InterceptInfo getBestIntercept( const WorldModel & wm,
                                    const InterceptTable * table ) const;

    /*!
      \brief try to perform ball wait action
      \param agent pointer to the agent itself
      \param target_point intercept target ball point
      \param info interception info that is considered now
      \return true if action is performed
    */
    bool doWaitTurn( PlayerAgent * agent,
                     const Vector2D & target_point,
                     const InterceptInfo & info );

    /*!
      \brief adjutment dash action. if possible try to perform turn action.
      \param agent pointer to agent itself
      \param target_point intercept target ball point
      \param info interception info that is considered now
      \return true if action is performed
    */
    bool doInertiaDash( PlayerAgent * agent,
                        const Vector2D & target_point,
                        const InterceptInfo & info );

};

}


#endif

