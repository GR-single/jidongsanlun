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
    if (ball.getX()<14){  //示例，有需要请删掉
      return LongKick();
    }
    else{
      return kickBall(KICK_FORWARD,VecPosition(15,0,0));
    }	
}
