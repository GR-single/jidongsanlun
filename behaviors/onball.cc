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
    VecPosition cotb=worldModel->getOpponent(findClosetOpponentToball());//离球最近的对手球员
    int canShootT=findCanShootTeamate();//可以射门的队友编号
    if(isCanLongKick(thisPlayer)){
      shootDistance=10;
    }
    else{
      shootDistance=6;
    }
    // if(ball.getX()<0&&ball.getX()>-3){
    //   tempGoal=collisionAvoidance(true,true,false,1,1,cttg,true);
    //   return kickBall(KICK_DRIBBLE,VecPosition(12,-7.5,0));
    // }
    // if(ball.getX()<-3){
    //   return LongKick(VecPosition(12,-7.5,0));
    // }
    // if(worldModel->getOpponent(WO_OPPONENT1).getY()>0){
    //   goal.setY(goal.getY()-0.3);
    // }
    // else{
    //   goal.setY(goal.getY()+0.3);
    // }
    //判断是否射门
    


    if(isDrible()){
      if(isGetBall()){
        tempGoal=collisionAvoidance(true,true,false,1,0.5,goal,true);
      }
      else{
        tempGoal=goal;
      }
      return kickBall(KICK_DRIBBLE,tempGoal);
    }else if(me.getDistanceTo(goal)<shootDistance){
      return LongKick(goal);
    }

    if(ball.getX()<0&&findHowmanydiinourhalf()>=6){
      return LongKick();
    }
    // if(canShootT!=0){
    //   VecPosition canShootTV=worldModel->getTeammate(canShootT);//可以射门的队友
    //   if(me.getDistanceTo(canShootTV)<4){
    //     return ShortKick(canShootTV);
    //   }
    //   else if(me.getDistanceTo(canShootTV)<8){
    //     return kickBall(KICK_FORWARD,canShootTV);
    //   }
    //   else if(me.getDistanceTo(canShootTV)<11&&isCanLongKick(thisPlayer)){
    //     return LongKick(canShootTV);
    //   }
    //   if(me.getDistanceTo(cotb)<1.5){
    //     tempGoal=collisionAvoidance(true,true,false,1,1,canShootTV,true);
    //     return kickBall(KICK_DRIBBLE,tempGoal);
    //   }
    // }

    //判断是否向离球门最近的队友传球
    if(thisPlayer!=findClosetTeamateToGoal()){
      if(me.getDistanceTo(cttg)<4){
        return ShortKick(cttg);
      }
      else if(me.getDistanceTo(cttg)<8){
        return kickBall(KICK_FORWARD,cttg);
      }
      else if(me.getDistanceTo(cttg)<11&&isCanLongKick(thisPlayer)){
        return LongKick(cttg);
      }
      if(me.getDistanceTo(cotb)<1.5){
        tempGoal=collisionAvoidance(true,true,false,1,1,goal,true);
        return kickBall(KICK_DRIBBLE,tempGoal);
      }
    }
    tempGoal=collisionAvoidance(true,true,false,1,1,VecPosition(12,-7.5,0),true);
    return kickBall(KICK_DRIBBLE,tempGoal);
      
      
    
    
    

    
    


    if (ball.getX()<14){  //示例，有需要请删掉
      return LongKick();
    }
    else{
      return kickBall(KICK_FORWARD,VecPosition(15,0,0));
    }	
}
