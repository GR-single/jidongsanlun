#include "pkbehaviors.h"


////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
/////// GOALIE
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

PKGoalieBehavior::
PKGoalieBehavior( const std::string teamName,
                  int uNum,
                  const map<string, string>& namedParams_,
                  const string& rsg_)
    : NaoBehavior( teamName,
                   uNum,
                   namedParams_,
                   rsg_) {
}

void PKGoalieBehavior::
beam( double& beamX, double& beamY, double& beamAngle ) {
    beamX = -HALF_FIELD_X+.5;
    beamY = 0;
    beamAngle = 0;
}




SkillType PKGoalieBehavior::selectSkill() {
    double distance,angle;
    getTargetDistanceAndAngle(ball,distance,angle);
    isPKGoal = true;
    return NaoBehavior::selectKeeperSkills(ball,distance,angle);
}





////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
/////// SHOOTER
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
PKShooterBehavior::
PKShooterBehavior( const std::string teamName,
                   int uNum,
                   const map<string, string>& namedParams_,
                   const string& rsg_ )
    : NaoBehavior( teamName,
                   uNum,
                   namedParams_,
                   rsg_ ) {
}

void PKShooterBehavior::
beam( double& beamX, double& beamY, double& beamAngle ) {
    beamX = -0.5;
    beamY = 0;
    beamAngle = 0;
}

SkillType PKShooterBehavior::
selectSkill() {
    //获得守门员位置信息
    WorldObject* opponent=worldModel->getWorldObject(WO_OPPONENT1);

    //判断球员的击球位置
    double middle;//击球位置
    if(opponent->pos.getY()>=0&&ball.getY()<=opponent->pos.getY()){
        middle=-0.9;
    }else if(opponent->pos.getY()>=0&&ball.getY()>opponent->pos.getY()){
        middle=0.9;
    }else if(opponent->pos.getY()<0&&ball.getY()<=opponent->pos.getY()){
        middle=-0.9;
    }else{
        middle=0.9;
    }


    /*double middle;//第二种可行办法
        if(opponent->pos.getY()>=0){
            middle=-0.8;
        }else{
            middle=0.8;
        }*/

    //一些球场位置的定义
    VecPosition target_begin(HALF_FIELD_X,0,0);//足球起始位置
    VecPosition target(HALF_FIELD_X,middle,0);//目标击球点
    VecPosition temptarget_begin(0,0,0);//长踢目标点
    VecPosition temptarget_end(11.8,0,0);//球员击球点
    int changchuan=0;

    //判断球员在不同位置下的不同运球状态
    if(ball.getDistanceTo(temptarget_begin)<=0.5){//两个长踢节省路上时间
        return kickBall(KICK_IK,target_begin);
    }
    else if(ball.getDistanceTo(temptarget_end)>0.5&&ball.getDistanceTo(temptarget_end)>5){//两个长踢节省路上时间
        return kickBall(KICK_IK,target_begin);
    }
    else if(ball.getDistanceTo(temptarget_end)<0.5){//到达击球点
        return kickBall(KICK_FORWARD,target);
    }
    else if(ball.getX()>13.8&&ball.getY()<3&&ball.getY()>-3){//在禁区不做行动
        return SKILL_STAND;
    }
    else{                                               //带球
        if(ball.getDistanceTo(me)<0.2)
            return kickBall(KICK_DRIBBLE,temptarget_end);
        else
            return goToTarget(ball);
    }

}
