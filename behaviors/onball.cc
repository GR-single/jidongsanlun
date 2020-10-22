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
    float shootDistance;
    int thisPlayer=worldModel->getUNum();
    VecPosition goal=VecPosition(15,0,0);
    VecPosition tempGoal=VecPosition(10,-5,0);
    VecPosition cttg=worldModel->getTeammate(findClosetTeamateToGoal());//离球门最近的队友
    VecPosition cttm=worldModel->getTeammate(findClosetTeamateToMe());//离当前球员最近的队友
    //VecPosition cotb=worldModel->getOpponent(findClosetOpponentToball());//离球最近的对手球员
    
    if(isCanLongKick(thisPlayer)){
      shootDistance=10;
    }
    else{
      shootDistance=6;
    }
    //判断是否射门
    if(me.getDistanceTo(goal)<shootDistance){
      return LongKick(goal);
    }
    
    //判断是否向离球门最近的队友传球
    if(thisPlayer!=findClosetTeamateToGoal()){
      if(me.getDistanceTo(cttg)<6){
        return kickBall(KICK_FORWARD,cttg);
      }
      else if(me.getDistanceTo(cttg)<10&&isCanLongKick(thisPlayer)){
        return LongKick(cttg);
      }
    }
    

    //判断是否向离得近的队友传球
    if(thisPlayer==findClosetTeamateToMe()){

    }
    else{
      if(me.getDistanceTo(cttm)<6){
        return kickBall(KICK_FORWARD,cttm);
      }
      else if(me.getDistanceTo(cttm)<10&&isCanLongKick(thisPlayer)) {
        return LongKick(cttm);
      }
    }
    return kickBall(KICK_DRIBBLE,tempGoal);
    

    
    


    if (ball.getX()<14){  //示例，有需要请删掉
      return LongKick();
    }
    else{
      return kickBall(KICK_FORWARD,VecPosition(15,0,0));
    }	
}
