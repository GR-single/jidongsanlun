#include "naobehavior.h"
#include "../rvdraw/rvdraw.h"
#include <cmath>
#include <vector>

//如需要其他功能请自己include相应头文件

//持球者策略实现
SkillType NaoBehavior::onball()
{   
    /**************************
     ******参赛者自己编写*******
     **************************/
    float canShootDistance;
    VecPosition goal=VecPosition(15,0,0);
    VecPosition cttg=worldModel->getTeammate(findClosetTeamateToGoal());//离球门最近的队友
    VecPosition cttm=worldModel->getTeammate(findClosetTeamateToMe());//离当前球员最近的队友
    VecPosition cotb=worldModel->getOpponent(findClosetOpponentToball());//离球最近的对手球员
    if(worldModel->getOpponent(WO_OPPONENT1).getY()>0){//根据守门员的位置判断射上半球门或下半球门
      goal.setY(goal.getY()-0.3);
    }
    else{
      goal.setY(goal.getY()+0.3);
    }
    int thisPlayer=worldModel->getUNum();
    if(thisPlayer!=7&&thisPlayer!=8&&thisPlayer!=11){
      canShootDistance=10;
    }
    else{
      canShootDistance=6;
    }
    if(me.getDistanceTo(goal)<=canShootDistance)//能射就射
        return LongKick(goal);
    if(thisPlayer==findClosetTeamateToGoal()){//当前球员离球门最近时
      return kickBall(KICK_DRIBBLE,VecPosition(15,0,0));//不能射带球
    }
    else{
      if(me.getDistanceTo(cttg)<2)//传给cttg时判断传球距离
        return ShortKick(VecPosition(cttg.getX()+0.3,cttg.getY(),0));
      else if(me.getDistanceTo(cttg)<7) return kickBall(KICK_FORWARD,VecPosition(cttg.getX()+0.3,cttg.getY(),0));
      else return LongKick(VecPosition(cttg.getX()+0.3,cttg.getY(),0));
    }
    
    
    if (ball.getX()<14){  //示例，有需要请删掉
      return LongKick();
    }
    else{
      return kickBall(KICK_FORWARD,VecPosition(15,0,0));
    }	
}
