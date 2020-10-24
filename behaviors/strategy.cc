#include "naobehavior.h"
#include "../rvdraw/rvdraw.h"
#include "../worldmodel/worldmodel.h"
#include "beam_save.h"

#include<iostream>
#include<fstream>
#include<cstdio>
#include<cmath>
#include<string>
#include<algorithm>
#include<vector>
#include<climits>
#include<float.h>
#include<string.h>
#include<ctime>
#include <boost/cstdint.hpp>

typedef uint64_t uint64;

extern int agentBodyType;
vector<VecPosition> positions;
vector<int> players;
/*
 * Real game beaming.
 * Filling params x y angle
 * 开场阵型
 */
void NaoBehavior::beam( double& beamX, double& beamY, double& beamAngle ) {
    //示例，有需要请自行修改
    if(worldModel->getUNum()==WO_TEAMMATE1){
        beamX = -14.8;
        beamY = 0;
        beamAngle = 0;
       }
    else if(worldModel->getUNum()==WO_TEAMMATE7){
        beamX = -2.2;
        beamY = 0;
        beamAngle = 0;
       }
    else{
        switch(worldModel->getUNum()){
            case WO_TEAMMATE2:{
                beamX = -13;
                beamY = -3;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE3:{
                beamX = -13;
                beamY = 3;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE4:{
                beamX = -8.5;
                beamY = 0;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE5:{
                beamX = -6;
                beamY = -2.7;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE11:{
                beamX = -2;
                beamY = 2;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE9:{
                beamX = -3.5;
                beamY = 0;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE6:{
                beamX = -1;
                beamY =-5;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE10:{
                beamX = -2;
                beamY = -3.5;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE8:{
                beamX = -6;
                beamY = 2.7;
                beamAngle = 0;
                break;
            }
        }
    }
}


SkillType NaoBehavior::selectSkill()
{
    //return SKILL_STAND;
    //return myTry();
    //示例，有需要请自行修改
    //worldModel->getRVSender()->clear(); // erases drawings from previous cycle
    //worldModel->getRVSender()->clearStaticDrawings();


    int Nearest2ball = findClosestPlayer2Ball();
    double distance,angle;
    getTargetDistanceAndAngle(ball,distance,angle);
    //画出球的位置
    //worldModel->getRVSender()->drawPoint("ball",ball.getX(),ball.getY(),20.0f, RVSender::PINK);

    //开球模式
    if(worldModel->getPlayMode() == PM_KICK_OFF_LEFT || worldModel->getPlayMode() == PM_KICK_OFF_RIGHT)
    {
        return kickOff();
    }

    //PLAY_ON mode正常对局
    else
    {
	//角色分配
        if(Nearest2ball == worldModel->getUNum() && worldModel->getUNum() > 1)
        {
            return onball();	//持球者角色
        }
        //守门员
        else if(worldModel->getUNum() == 1)
        {
            isPKGoal = false;
            if(worldModel->getPlayMode() == PM_CORNER_KICK_LEFT || worldModel->getPlayMode() == PM_CORNER_KICK_RIGHT)
            {
                return goToTargetRelative(worldModel->g2l(VecPosition(-14.5,0,0)),angle);
            }
            else
            {
                return NaoBehavior::selectKeeperSkills(ball,distance,angle);
            }
        }
        //other players
        else
        {
            if(ball.getX()>=0)
                current_beam = demoMode_1(ball);
            else
                current_beam = demoMode_2(ball);
            last_beam.clear();  //delete the data before
            for(int i = WO_TEAMMATE2; i < WO_TEAMMATE1+NUM_AGENTS; ++i)
            {
                if(i != Nearest2ball)
                {
                    last_beam.push_back(i);         //确定要走阵型的机器人球员的Unum
                    if(last_beam.size() == 9)
                    {
                        break;
                    }
                }
            }
            for(int i=0;i<(int)last_beam.size();i++)
            {
                if(last_beam[i] == worldModel->getUNum())
                {
                    VecPosition target = collisionAvoidance(true,true,false,1,0.5,current_beam[i],true);   //假设Unum为i，则把阵型点集合中第i个坐标点设为相应机器人的目标点
                   
                    if(me.getDistanceTo(target) > 0.25)
                    {
                        // Far away from the ball so walk toward target offset from the ball
                        return goToTarget(target);
                    }
                    //面向球
                    double rot,dis;
                    getTargetDistanceAndAngle(ball,dis,rot);    //面向球
                    return goToTargetRelative(target,rot);
                }
            }
        }
        //return goToTarget(ball);
    }
}
SkillType NaoBehavior::myTry(){
    VecPosition goalVecpos;
    int nearestPlayer=findClosestPlayer2Ball();
    int thisPlayer=worldModel->getUNum();
    if(thisPlayer==nearestPlayer){
        return ShortKick();
    }
    else{
        return SKILL_STAND;
    }
}
//开球站位
SkillType NaoBehavior::kickOff()
{
    //我方开球
    if((worldModel->getSide() == SIDE_LEFT && worldModel->getPlayMode() == PM_KICK_OFF_LEFT) || (worldModel->getSide() == SIDE_RIGHT && worldModel->getPlayMode() == PM_KICK_OFF_RIGHT))
    {
	/***************************************
	*******我方开球总体的站位或走位***********
	****************************************/
        int OpponentNum=0;
        if(OpponentNum!=0)
        OpponentNum=0;
        for(int i=1;i<12;i++)
        {
            if(ball.getDistanceTo(worldModel->getOpponent(i))<2.8)
            OpponentNum++;
        }
        if(OpponentNum<3)
        {
            positions.clear();
            /*以下是球员开球以及进攻时每个球员的站位*/
            positions.push_back(VecPosition(-14.8,0,0));//1
            positions.push_back(VecPosition(-13.5,-2.5,0));//2
            positions.push_back(VecPosition(-13.5,2.5,0));//3
            positions.push_back(VecPosition(-11,0,0));//4
            positions.push_back(VecPosition(-2,-7,0));//5
            positions.push_back(VecPosition(-2,2,0));//8
            positions.push_back(VecPosition(-1,-1.5,0));//9
            positions.push_back(VecPosition(-0.5,-3,0));//7
            positions.push_back(VecPosition(-0.5,-4.5,0));//10
            positions.push_back(VecPosition(-0.5,-6.5,0));//6

            players.clear();
            /*以下是对应站位的球员编号*/
            players.push_back(WO_TEAMMATE1);//1
            players.push_back(WO_TEAMMATE2);//2
            players.push_back(WO_TEAMMATE3);//3
            players.push_back(WO_TEAMMATE4);//4
            players.push_back(WO_TEAMMATE5);//5
            players.push_back(WO_TEAMMATE8);//8
            players.push_back(WO_TEAMMATE9);//9
            players.push_back(WO_TEAMMATE7);//7
            players.push_back(WO_TEAMMATE10);//10
            players.push_back(WO_TEAMMATE6);//6

            /*开始走位*/
            if(worldModel->getUNum()!=WO_TEAMMATE11)
            {
                for(int i=0;i<(int)players.size();i++)
                {
                    if(players[i] == worldModel->getUNum())
                    {
                        VecPosition target = collisionAvoidance(true,true,true,1,0.5,positions[i],true);   //假设Unum为i，则把阵型点集合中第i个坐标点设为相应机器人的目标点
                   
                        if(me.getDistanceTo(target) > 0.25)
                        {
                            // Far away from the ball so walk toward target offset from the ball
                            return goToTarget(target);
                        }
                        //面向球
                        double rot,dis;
                        getTargetDistanceAndAngle(ball,dis,rot);    //面向球
                        return goToTargetRelative(target,rot);
                    }
                }
            }
            /*else if(worldModel->getTeammate(WO_OPPONENT9).getDistanceTo(ball)>0.3)
            {
                return goToTarget(VecPosition(0,0.5,0));
            }*/
            else 
            return kickBall(KICK_FORWARD,VecPosition(7.5,-7.5,0));
        }
        else
        {
            positions.clear();
            /*以下是球员开球以及进攻时每个球员的站位*/
            positions.push_back(VecPosition(-14.8,0,0));//1
            positions.push_back(VecPosition(-13.5,-2.5,0));//2
            positions.push_back(VecPosition(-13.5,2.5,0));//3
            positions.push_back(VecPosition(-11,0,0));//4
            positions.push_back(VecPosition(-2,-7,0));//5
            positions.push_back(VecPosition(-2,2,0));//8
            positions.push_back(VecPosition(-2.2,0,0));//9
            positions.push_back(VecPosition(-1,-1,0));//7
            positions.push_back(VecPosition(-1.5,-5.5,0));//10
            positions.push_back(VecPosition(-0.5,-6.5,0));//6

            players.clear();
            /*以下是对应站位的球员编号*/
            players.push_back(WO_TEAMMATE1);//1
            players.push_back(WO_TEAMMATE2);//2
            players.push_back(WO_TEAMMATE3);//3
            players.push_back(WO_TEAMMATE4);//4
            players.push_back(WO_TEAMMATE5);//5
            players.push_back(WO_TEAMMATE8);//8
            players.push_back(WO_TEAMMATE9);//9
            players.push_back(WO_TEAMMATE7);//7
            players.push_back(WO_TEAMMATE10);//10
            players.push_back(WO_TEAMMATE6);//6

            /*开始走位*/
            if(worldModel->getUNum()!=WO_TEAMMATE11)
            {
                for(int i=0;i<(int)players.size();i++)
                {
                    if(players[i] == worldModel->getUNum())
                    {
                        VecPosition target = collisionAvoidance(true,true,true,1,0.5,positions[i],true);   //假设Unum为i，则把阵型点集合中第i个坐标点设为相应机器人的目标点
                   
                        if(me.getDistanceTo(target) > 0.25)
                        {
                            // Far away from the ball so walk toward target offset from the ball
                            return goToTarget(target);
                        }
                        //面向球
                        double rot,dis;
                        getTargetDistanceAndAngle(ball,dis,rot);    //面向球
                        return goToTargetRelative(target,rot);
                    }
                }
            }
            /*else if(worldModel->getTeammate(WO_OPPONENT11).getDistanceTo(ball)>0.3)
            {
                return goToTarget(VecPosition(0,0.2,0));
            }*/
            else 
            return kickBall(KICK_FORWARD,VecPosition(-0.1,-5,0));
        }
        
        
    }

    //对方开球
    else if((worldModel->getSide() == SIDE_LEFT && worldModel->getPlayMode() == PM_KICK_OFF_RIGHT) || (worldModel->getSide() == SIDE_RIGHT && worldModel->getPlayMode() == PM_KICK_OFF_LEFT))
    {
	/***************************************
	*****对方开球时我方的总体的站位或走位*****
	****************************************/
        positions.clear();
        /*以下是对面开球以及防守时每个球员的站位*/
        positions.push_back(VecPosition(-14.8,0,0));//1
        positions.push_back(VecPosition(-13.5,-2,0));//2
        positions.push_back(VecPosition(-13.5,2,0));//3
        positions.push_back(VecPosition(-5.5,0,0));//4
        positions.push_back(VecPosition(-4,-4.5,0));//5
        positions.push_back(VecPosition(-0.8,-2.5,0));//6
        positions.push_back(VecPosition(-2.5,0,0));//7
        positions.push_back(VecPosition(-4,4.5,0));//8
        positions.push_back(VecPosition(-1.8,1.8,0));//9
        positions.push_back(VecPosition(-1.8,-1.8,0));//11
        positions.push_back(VecPosition(-0.8,2.5,0));//10

        players.clear();
        /*以下是对应站位的球员编号*/
        players.push_back(WO_TEAMMATE1);//1
        players.push_back(WO_TEAMMATE2);//2
        players.push_back(WO_TEAMMATE3);//3
        players.push_back(WO_TEAMMATE4);//4
        players.push_back(WO_TEAMMATE5);//5
        players.push_back(WO_TEAMMATE6);//6
        players.push_back(WO_TEAMMATE7);//7
        players.push_back(WO_TEAMMATE8);//8
        players.push_back(WO_TEAMMATE9);//9
        players.push_back(WO_TEAMMATE10);//10
        players.push_back(WO_TEAMMATE11);//11

        /*开始走位*/
        for(int i=0;i<(int)players.size();i++)
            {
                if(players[i] == worldModel->getUNum())
                {
                    VecPosition target = collisionAvoidance(true,true,true,1,0.5,positions[i],true);   //假设Unum为i，则把阵型点集合中第i个坐标点设为相应机器人的目标点
                   
                    if(me.getDistanceTo(target) > 0.25)
                    {
                        // Far away from the ball so walk toward target offset from the ball
                        return goToTarget(target);
                    }
                    //面向球
                    double rot,dis;
                    getTargetDistanceAndAngle(ball,dis,rot);    //面向球
                    return goToTargetRelative(target,rot);
                }
            }
        
    }
    return SKILL_STAND;
}


//找出离球最近的球员当持球者
//guorui 10/17 19:54
int NaoBehavior::findClosestPlayer2Ball()
{
    	/***************************************
	************参赛者自己编写***************
	****************************************/
    VecPosition tp;
    int minNum=1;
    double maxDistance=100;
    for(int i=2;i<12;i++){
        if(worldModel->getUNum()==i){
            tp=worldModel->getMyPosition();
        }
        else{
            tp=worldModel->getTeammate(i);
        }
        if(!worldModel->getFallenTeammate(i)&&tp.getDistanceTo(ball)<maxDistance){
            minNum=i;
            maxDistance=tp.getDistanceTo(worldModel->getBall());
        }
    }
    
    return minNum;	//示例，返回11号，有需要请自行修改；
}
//找到离当前球员最近的队友
int NaoBehavior::findClosetTeamateToMe(){
    int thisPlayer=worldModel->getUNum();
    int minNum=thisPlayer;
    float minDistance=100;
    for(int i=WO_TEAMMATE2;i<=WO_TEAMMATE11;i++){
        if(i!=thisPlayer){
            float tDis=me.getDistanceTo(worldModel->getTeammate(i));
            float x = worldModel->getTeammate(i).getX();
            if(x>me.getX()&&tDis<minDistance){
                minNum=i;
                minDistance=tDis;
            }
        }
    }
    return minNum;
}

//找到离对面球门最近的队友
int NaoBehavior::findClosetTeamateToGoal(){
    VecPosition goal = VecPosition(15,0,0);
    int maxNum = 10;
    // float minDistance = 100;
    float tx=-15;
    for(int i=WO_TEAMMATE11;i>=WO_TEAMMATE2;i--){
        // float tDis = worldModel->getTeammate(i).getDistanceTo(goal);
        // if(tDis<minDistance){
        //     minNum=i;
        //     minDistance=tDis;
        // }
        float x=worldModel->getTeammate(i).getX();
        if(x>tx){
            maxNum=i;
            tx=x;
        }

    }
    return maxNum;
}
//找到能射门的队友
int NaoBehavior::findCanShootTeamate(){
    float shootDistance=6;
    for(int i=WO_TEAMMATE11;i>=WO_TEAMMATE2;i--){
        if(isCanLongKick(i)){
            shootDistance=10;
        }
        if(worldModel->getTeammate(i).getDistanceTo(VecPosition(15,0,0))<=shootDistance){
            return i;
        }
    }
    return 0;
}
//找到对手离球最近的球员
int NaoBehavior::findClosetOpponentToball(){
    int minNum = 12;
    float minDistance = 100;
    for(int i=WO_OPPONENT1;i<=WO_OPPONENT11;i++){
        float tDis = worldModel->getOpponent(i).getDistanceTo(ball);
        if(tDis<minDistance){
            minNum=i;
            minDistance=tDis;
        }
    }
    return minNum;
}
int NaoBehavior::findHowmanydiinourhalf(){
    int numofdi=0;
    for(int i=WO_OPPONENT1;i<=WO_OPPONENT11;i++){
        float xpos = worldModel->getOpponent(i).getX();
        if(xpos<0){
            numofdi++;
        }
    }
    return numofdi;

}
VecPosition NaoBehavior::findMidPoint(VecPosition a,VecPosition b){
    return VecPosition((a.getX()+b.getX())/2,(a.getY()+b.getY())/2,0);
}

void NaoBehavior::fangShouGuoXian(vector<VecPosition> & a){
    for(int i=WO_OPPONENT2;i<=WO_OPPONENT11;i++){
        VecPosition o=worldModel->getOpponent(i);
        if(o.getX()<-5){
            a[i]=(VecPosition(o.getX()+0.5,o.getY(),0));
        }
    }
}

void NaoBehavior::tiaoZhengGoal(VecPosition & goal){
    goal.setX(goal.getX()+1);
    goal.setY(goal.getY()-1);
}

bool NaoBehavior::isCanLongKick(int player){
    return (player!=5&&player!=6&&player!=11);
}

bool NaoBehavior::isGetBall(){
    VecPosition woOnBall=worldModel->getTeammate(findClosestPlayer2Ball());
    VecPosition diOnBall=worldModel->getOpponent(findClosetOpponentToball());
    if(woOnBall.getDistanceTo(ball)<diOnBall.getDistanceTo(ball)){
        return true;
    }
    return false;

}
//示例阵型

bool NaoBehavior::isDrible(){
    int count=0;
    for(int i=WO_OPPONENT2;i<WO_OPPONENT11;i++){
        if(me.getDistanceTo(worldModel->getOpponent(i))<1.5){
            count++;
        }
    }
    return count>=2;
}

vector<VecPosition> NaoBehavior::demoMode_1(VecPosition ball)
{
    //示例，有需要请自行修改
    deam_position.clear();
    float x = ball.getX();
    float y = ball.getY();
    VecPosition getonball = worldModel->getTeammate(findClosestPlayer2Ball());
    float tx=getonball.getX();
    float ty=getonball.getY();
    float diXian = -10;
    deam_position.push_back(VecPosition(diXian,y-5,0));
    deam_position.push_back(VecPosition(diXian+3,y,0));
    deam_position.push_back(VecPosition(diXian,y+5,0));
    deam_position.push_back(VecPosition(tx-1,ty+1.5,0));
    deam_position.push_back(VecPosition(tx-1,ty-1.5,0));   
    deam_position.push_back(VecPosition(x+3,y+3,0));
    deam_position.push_back(VecPosition(x+4,y-3,0));
    deam_position.push_back(VecPosition(12,-7.5,0));
    deam_position.push_back(VecPosition(tx-5,ty,0));
    if(!isGetBall()){
        deam_position[3]=ball;
    }
    
    //围圈
    // deam_position.push_back(VecPosition(diXian,y-3,0));
    // deam_position.push_back(VecPosition(diXian,y+3,0));
    // deam_position.push_back(VecPosition(x-1.5,y-1,0));
    // deam_position.push_back(VecPosition(x,y+1.5,0));
    // deam_position.push_back(VecPosition(x+1.5,y,0));   
    // deam_position.push_back(VecPosition(x,y-1.5,0));
    // deam_position.push_back(VecPosition(x+6,7.5,0));
    // deam_position.push_back(VecPosition(x+6,-7.5,0));
    // deam_position.push_back(VecPosition(x-1.5,y+1,0));
    

    //检测是否越界
    for(int i=0;i<=(int)deam_position.size();i++)
    {
        if(deam_position[i].getY() >= 0.0)
        {
            if(deam_position[i].getX()  >= 14.5 && deam_position[i].getY() <10.0)
            {
                deam_position[i].setX(14.0);
            }
            else if(deam_position[i].getX()  < 14.5 && deam_position[i].getY() >=10.0)
            {
                deam_position[i].setY(9.5);
            }
            else if(deam_position[i].getX()  >= 14.5 && deam_position[i].getY() >=10.0)
            {
                deam_position[i].setX(14.0);
                deam_position[i].setY(9.5);
            }
        }
        else
        {
            if(deam_position[i].getX()  >= 14.5 && deam_position[i].getY() <=10.0)
            {
                deam_position[i].setX(14.0);
            }
            else if(deam_position[i].getX()  < 14.5 && deam_position[i].getY() <=-10.0)
            {
                deam_position[i].setY(-9.5);
            }
            else if(deam_position[i].getX()  >= 14.5 && deam_position[i].getY() <=-10.0)
            {
                deam_position[i].setX(14.0);
                deam_position[i].setY(-9.5);
            }
        }
    }
    //drawing beam
    worldModel->getRVSender()->clearStaticDrawings();
    if(worldModel->getUNum() == findClosestPlayer2Ball())  //离球最近的人定位的阵型
    {
        worldModel->getRVSender()->clear();
        worldModel->getRVSender()->clearStaticDrawings();
        for(int j=0;j<9;j++)
        {
            worldModel->getRVSender()->drawPoint(deam_position[j].getX(),deam_position[j].getY(),6.0f,RVSender::YELLOW);
        }
    }
    return deam_position;
}


vector<VecPosition> NaoBehavior::demoMode_2(VecPosition ball)
{
    //示例，有需要请自行修改
    deam_position.clear();
    float x = ball.getX();
    float y = ball.getY();
    VecPosition oob =worldModel->getOpponent(findClosetOpponentToball());
    float diXian = -12.5;
    if(isGetBall()==true && findHowmanydiinourhalf()<=3){
    deam_position.push_back(findMidPoint(oob,VecPosition(-15,0,0)));
    deam_position.push_back(VecPosition(x-7,y+3,0));
    deam_position.push_back(VecPosition(x-7,y-3,0));
    deam_position.push_back(VecPosition(x-8,y+2.5,0));
    deam_position.push_back(VecPosition(x-8,y-2.5,0));
    deam_position.push_back(VecPosition(x+2,y+3,0));
    deam_position.push_back(VecPosition(x+2,y-3,0));
    deam_position.push_back(VecPosition(x+1.5,y-1.5,0));
    deam_position.push_back(findMidPoint(ball,VecPosition(15,0,0)));

    }   
    else if(findHowmanydiinourhalf()<7){
    deam_position.push_back(findMidPoint(oob,VecPosition(-15,0,0)));
    deam_position.push_back(VecPosition(diXian,y-4,0));
    deam_position.push_back(VecPosition(diXian+2,y-3,0));
    deam_position.push_back(VecPosition(diXian+3,y,0));
    deam_position.push_back(VecPosition(diXian+2,y+3,0));
    deam_position.push_back(VecPosition(diXian,y+4,0));
    deam_position.push_back(findMidPoint(ball,VecPosition(15,0,0)));
    deam_position.push_back(VecPosition(x+7,y,0));
    deam_position.push_back(VecPosition(x+10,y,0));}
    else if(findHowmanydiinourhalf()>=7 && findHowmanydiinourhalf()<=11){
    deam_position.push_back(findMidPoint(oob,VecPosition(-15,0,0)));
    deam_position.push_back(VecPosition(diXian,y-4,0));
    deam_position.push_back(VecPosition(diXian+2,y-3,0));
    deam_position.push_back(VecPosition(diXian+3,y,0));
    deam_position.push_back(VecPosition(diXian+2,y+3,0));
    deam_position.push_back(VecPosition(diXian,y+4,0));
    deam_position.push_back(VecPosition(x-1,y,0));
    deam_position.push_back(VecPosition(x+3,y+1.5,0));
    deam_position.push_back(VecPosition(x+3,y-1.5,0));}
    //检测是否越界
    for(int i=0;i<=(int)deam_position.size();i++)
    {
        if(deam_position[i].getY() >= 0.0)
        {
            if(deam_position[i].getX()  >= 15.0 && deam_position[i].getY() <10.0)
            {
                deam_position[i].setX(14.0);
            }
            else if(deam_position[i].getX()  < 15.0 && deam_position[i].getY() >=10.0)
            {
                deam_position[i].setY(9.5);
            }
            else if(deam_position[i].getX()  >= 15.0 && deam_position[i].getY() >=10.0)
            {
                deam_position[i].setX(14.0);
                deam_position[i].setY(9.5);
            }
        }
        else
        {
            if(deam_position[i].getX()  >= 15.0 && deam_position[i].getY() <=10.0)
            {
                deam_position[i].setX(14.0);
            }
            else if(deam_position[i].getX()  < 15.0 && deam_position[i].getY() <=-10.0)
            {
                deam_position[i].setY(-9.5);
            }
            else if(deam_position[i].getX()  >= 15.0 && deam_position[i].getY() <=-10.0)
            {
                deam_position[i].setX(14.0);
                deam_position[i].setY(-9.5);
            }
        }
    }
    //drawing beam
    worldModel->getRVSender()->clearStaticDrawings();
    if(worldModel->getUNum() == findClosestPlayer2Ball())  //离球最近的人定位的阵型
    {
        worldModel->getRVSender()->clear();
        worldModel->getRVSender()->clearStaticDrawings();
        for(int j=0;j<9;j++)
        {
            worldModel->getRVSender()->drawPoint(deam_position[j].getX(),deam_position[j].getY(),6.0f,RVSender::YELLOW);
        }
    }
    return deam_position;
}
