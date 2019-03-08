#include "naobehavior.h"
#include "../rvdraw/rvdraw.h"

extern int agentBodyType;

/*
 * Real game beaming.
 * Filling params x y angle
 */
void NaoBehavior::beam( double& beamX, double& beamY, double& beamAngle ) {
    beamX = -HALF_FIELD_X + worldModel->getUNum();
    beamY = 0;
    beamAngle = 0;
}


SkillType NaoBehavior::selectSkill() {
    // My position and angle
    //cout << worldModel->getUNum() << ": " << worldModel->getMyPosition() << ",\t" << worldModel->getMyAngDeg() << "\n";

    // Position of the ball
    //cout << worldModel->getBall() << "\n";

    // Example usage of the roboviz drawing system and RVSender in rvdraw.cc.
    // Agents draw the position of where they think the ball is
    // Also see example in naobahevior.cc for drawing agent position and
    // orientation.
    /*
    worldModel->getRVSender()->clear(); // erases drawings from previous cycle
    worldModel->getRVSender()->drawPoint("ball", ball.getX(), ball.getY(), 10.0f, RVSender::MAGENTA);
    */

    // ### Demo Behaviors ###

    // Walk in different directions
    //return goToTargetRelative(VecPosition(1,0,0), 0); // Forward
    //return goToTargetRelative(VecPosition(-1,0,0), 0); // Backward
    //return goToTargetRelative(VecPosition(0,1,0), 0); // Left
    //return goToTargetRelative(VecPosition(0,-1,0), 0); // Right
    //return goToTargetRelative(VecPosition(1,1,0), 0); // Diagonal
    //return goToTargetRelative(VecPosition(0,1,0), 90); // Turn counter-clockwise
    //return goToTargetRelative(VecPdosition(0,-1,0), -90); // Turn clockwise
    //return goToTargetRelative(VecPosition(1,0,0), 15); // Circle

    //Walk to the ball
    //return goToTarget(ball);
    WorldObject* opp = worldModel->getWorldObject( WO_OPPONENT1 );
    double closestDistanceToBall = 10000;VecPosition closest =  opp->pos;
    for(int i = WO_OPPONENT1; i < WO_OPPONENT1+4; ++i) 
    {
        VecPosition temp;
        int playerNum = i - WO_OPPONENT1 + 1;
        WorldObject* opponent = worldModel->getWorldObject( i );
        if (opponent->validPosition) {
            temp = opponent->pos;
        } 
        else 
        {
            continue;
        }
        
        temp.setZ(0);

        double distanceToBall = temp.getDistanceTo(ball);
        if (distanceToBall < closestDistanceToBall) 
        {
            closest=opponent->pos;
            closestDistanceToBall = distanceToBall;
        }
    }
    // Turn in place to face ball
    double distance, angle;
    getTargetDistanceAndAngle(ball, distance, angle);
    VecPosition myPos=worldModel->getMyPosition();
    double distanceToGoal=myPos.getDistanceTo(VecPosition(HALF_FIELD_X, 0, 0));
    //if (abs(angle) > 20) 
        //return goToTargetRelative(VecPosition(), angle);
    /*if(closestDistanceToBall<1)
    {
        if(angle*angle>100)
            return goToTargetRelative(VecPosition(), angle);
        return goToTarget(ball);
    }*/
    cout << "X= " <<myPos.getX()-closest.getX()<<"     CLOSEST = "<<closestDistanceToBall<<"    GOAL = "<<distanceToGoal<<endl;
    //cout << "OPP X= " <<closest.getX()<<" OPP Y= "<<closest.getY()<<endl;
    if(distanceToGoal<2)
        return kickBall(KICK_IK, VecPosition(HALF_FIELD_X*1.04, 0, 0));
    if(myPos.getX()+1.5<closest.getX()&&closestDistanceToBall<2.5)
    {
        if(myPos.getY()-closest.getY()>0)
            return kickBall(KICK_DRIBBLE, VecPosition(HALF_FIELD_X, myPos.getY()+3, 0));
        return kickBall(KICK_DRIBBLE, VecPosition(HALF_FIELD_X, myPos.getY()-3, 0));
    }
    return kickBall(KICK_DRIBBLE, VecPosition(HALF_FIELD_X*1.05, 0, 0));
    if(distanceToGoal<3&&closestDistanceToBall>2.5)
        return kickBall(KICK_IK, VecPosition(HALF_FIELD_X, 0, 0));
    if(distance>2.5)
    { 
        if(angle*angle>100)
            return goToTargetRelative(VecPosition(), angle);
        return goToTarget(ball);
    }
     //else {
      //return SKILL_STAND;
    //}
    // Walk to ball while always facing forward
    //return goToTargetRelative(worldModel->g2l(ball), -worldModel->getMyAngDeg());
    //if(me.getDistanceTo(VecPosition(HALF_FIELD_X, 0, 0))<0.25)
        //return kickBall(KICK_IK, VecPosition(HALF_FIELD_X, 0, 0));

    // Dribble ball toward opponent's goal
    //if(angle*angle>100)
        //return goToTargetRelative(VecPosition(), angle);
    return kickBall(KICK_IK, VecPosition(HALF_FIELD_X, 0, 0)); // IK kick

    // Kick ball toward opponent's goal
    //return kickBall(KICK_FORWARD, VecPosition(HALF_FIELD_X, 0, 0)); // Basic kick
    //return kickBall(KICK_IK, VecPosition(HALF_FIELD_X, 0, 0)); // IK kick

    // Just stand in place
    //return SKILL_STAND;

    // Demo behavior where players form a rotating circle and kick the ball
    // back and forth
    //return demoKickingCircle();
    /*for(int i = WO_OPPONENT1; i <= WO_OPPONENT11; ++i) {
        WorldObject* opponent = worldModel->getWorldObject( i );
        if (opponent->validPosition == true) {
            VecPosition temp = opponent->pos;
            temp.setZ(0);
            if (abs(me.getAngleBetweenPoints(target, temp)) < 90.0 &&
                    me.getDistanceTo(temp) < me.getDistanceTo(target)) {
                double distance = me.getDistanceTo(temp);
                if (distance < closestObjDistance) {
                    closestObjDistance = distance;
                    closestObjPos = temp;
                }
            }
        }
    }*/
    
}


/*
 * Demo behavior where players form a rotating circle and kick the ball
 * back and forth
 */
SkillType NaoBehavior::demoKickingCircle() {
    // Parameters for circle
    VecPosition center = VecPosition(-HALF_FIELD_X/2.0, 0, 0);
    double circleRadius = 5.0;
    double rotateRate = 2.5;

    // Find closest player to ball
    int playerClosestToBall = -1;
    double closestDistanceToBall = 10000;
    for(int i = WO_TEAMMATE1; i < WO_TEAMMATE1+NUM_AGENTS; ++i) {
        VecPosition temp;
        int playerNum = i - WO_TEAMMATE1 + 1;
        if (worldModel->getUNum() == playerNum) {
            // This is us
            temp = worldModel->getMyPosition();
        } else {
            WorldObject* teammate = worldModel->getWorldObject( i );
            if (teammate->validPosition) {
                temp = teammate->pos;
            } else {
                continue;
            }
        }
        temp.setZ(0);

        double distanceToBall = temp.getDistanceTo(ball);
        if (distanceToBall < closestDistanceToBall) {
            playerClosestToBall = playerNum;
            closestDistanceToBall = distanceToBall;
        }
    }

    if (playerClosestToBall == worldModel->getUNum()) {
        // Have closest player kick the ball toward the center
        return kickBall(KICK_FORWARD, center);
    } else {
        // Move to circle position around center and face the center
        VecPosition localCenter = worldModel->g2l(center);
        SIM::AngDeg localCenterAngle = atan2Deg(localCenter.getY(), localCenter.getX());

        // Our desired target position on the circle
        // Compute target based on uniform number, rotate rate, and time
        VecPosition target = center + VecPosition(circleRadius,0,0).rotateAboutZ(360.0/(NUM_AGENTS-1)*(worldModel->getUNum()-(worldModel->getUNum() > playerClosestToBall ? 1 : 0)) + worldModel->getTime()*rotateRate);

        // Adjust target to not be too close to teammates or the ball
        target = collisionAvoidance(true /*teammate*/, false/*opponent*/, true/*ball*/, 1/*proximity thresh*/, .5/*collision thresh*/, target, true/*keepDistance*/);

        if (me.getDistanceTo(target) < .25 && abs(localCenterAngle) <= 10) {
            // Close enough to desired position and orientation so just stand
            return SKILL_STAND;
        } else if (me.getDistanceTo(target) < .5) {
            // Close to desired position so start turning to face center
            return goToTargetRelative(worldModel->g2l(target), localCenterAngle);
        } else {
            // Move toward target location
            return goToTarget(target);
        }
    }
}


