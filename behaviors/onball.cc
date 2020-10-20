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
    VecPosition cttg=worldModel->getTeammate(findClosetTeamateToGoal());
    VecPosition cttm=worldModel->getTeammate(findClosetTeamateToMe());
    VecPosition cotb=worldModel->getOpponent(findClosetOpponentToball());
    if(worldModel->getOpponent(WO_OPPONENT1).getY()>0){
      goal.setY(goal.getY()-0.3);
    }
    else{
      goal.setY(goal.getY()+0.3);
    }
    int thisPlayer=worldModel->getUNum();
    if(thisPlayer==1||thisPlayer==2||thisPlayer==7||thisPlayer==8||thisPlayer==11){
      canShootDistance=10;
    }
    else{
      canShootDistance=7;
    }
    if(thisPlayer==findClosetTeamateToGoal()){
      if(me.getDistanceTo(goal)<=canShootDistance)
        return LongKick(goal);
      return kickBall(KICK_DRIBBLE,VecPosition(15,0,0));
    }
    else{
      if(me.getDistanceTo(goal)<=canShootDistance)
        return LongKick(goal);
      
      return ShortKick(VecPosition(cttg.getX()+0.3,cttg.getY(),0));
    }
    
    
    if (ball.getX()<14){  //示例，有需要请删掉
      return LongKick();
    }
    else{
      return kickBall(KICK_FORWARD,VecPosition(15,0,0));
    }	
}
