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
    int thisPlayer=worldModel->getUNum();
    if(thisPlayer==1||thisPlayer==2||thisPlayer==7||thisPlayer==8||thisPlayer==11){
      canShootDistance=10;
    }
    else{
      canShootDistance=7;
    }

    
    
    if (ball.getX()<14){  //示例，有需要请删掉
      return LongKick();
    }
    else{
      return kickBall(KICK_FORWARD,VecPosition(15,0,0));
    }	
}
