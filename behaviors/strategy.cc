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
    else if(worldModel->getUNum()==WO_TEAMMATE11){
        beamX = -2.3;
        beamY = 0;
        beamAngle = 0;
       }
    else{
        switch(worldModel->getUNum()){
            case WO_TEAMMATE2:{
                beamX = -12;
                beamY = -2;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE3:{
                beamX = -12;
                beamY = 2;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE4:{
                beamX = -10;
                beamY = -4;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE5:{
                beamX = -10;
                beamY = 4;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE6:{
                beamX = -7;
                beamY = -5;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE7:{
                beamX = -7;
                beamY = 5;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE8:{
                beamX = -4;
                beamY = -6;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE9:{
                beamX = -4;
                beamY = 6;
                beamAngle = 0;
                break;
            }
            case WO_TEAMMATE10:{
                beamX = -6;
                beamY = 0;
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
                    VecPosition target = collisionAvoidance(true,true,true,1,0.5,current_beam[i],true);   //假设Unum为i，则把阵型点集合中第i个坐标点设为相应机器人的目标点
                   
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
        return LongKick();
    }
    else{
        return goToTarget(VecPosition(worldModel->getBall().getX()-5,worldModel->getMyPosition().getY(),0));
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
	**************参赛者自己编写*************
	****************************************/
    if(worldModel->getUNum() == findClosestPlayer2Ball())
        return kickBall(KICK_IK,VecPosition(-5,5,0));
    else
        return SKILL_STAND;	//示例，站着不动，有需要请自行修改
    }
    //对方开球
    else if((worldModel->getSide() == SIDE_LEFT && worldModel->getPlayMode() == PM_KICK_OFF_RIGHT) || (worldModel->getSide() == SIDE_RIGHT && worldModel->getPlayMode() == PM_KICK_OFF_LEFT))
    {
	/***************************************
	*****对方开球时我方的总体的站位或走位*****
	**************参赛者自己编写*************
	****************************************/
    return SKILL_STAND;	//示例，站着不动，有需要请自行修改
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
    for(int i=1;i<12;i++){
        if(worldModel->getUNum()==i){
            tp=worldModel->getMyPosition();
        }
        else{
            tp=worldModel->getTeammate(i);
        }
        if(tp.getDistanceTo(worldModel->getBall())<maxDistance){
            minNum=i;
            maxDistance=tp.getDistanceTo(worldModel->getBall());
        }
    }
    return minNum;	//示例，返回11号，有需要请自行修改；
}
//找到离当前球员最近的队友
int NaoBehavior::findClosetTeamateToMe(){
    int thisPlayer=worldModel->getUNum();
    int minNum=1;
    float minDistance=100;
    for(int i=WO_TEAMMATE1;i<=WO_TEAMMATE11;i++){
        if(i!=thisPlayer){
            float tDis=me.getDistanceTo(worldModel->getTeammate(i));
            if(tDis<minDistance){
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
    int minNum = 1;
    float minDistance = 100;
    for(int i=WO_TEAMMATE1;i<=WO_TEAMMATE11;i++){
        float tDis = worldModel->getTeammate(i).getDistanceTo(goal);
        if(tDis<minDistance){
            minNum=i;
            minDistance=tDis;
        }
    }
    return minNum;
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

//示例阵型

vector<VecPosition> NaoBehavior::demoMode_1(VecPosition ball)
{
    //示例，有需要请自行修改
    deam_position.clear();
    float x = ball.getX();
    float y = ball.getY();
    float diXian = -12.5;
    deam_position.push_back(VecPosition(diXian,y-4,0));
    deam_position.push_back(VecPosition(diXian+2,y-3,0));
    deam_position.push_back(VecPosition(diXian+3,y,0));
    deam_position.push_back(VecPosition(diXian+2,y+3,0));
    deam_position.push_back(VecPosition(diXian,y+4,0));
    deam_position.push_back(VecPosition(x-4,y+2,0));
    deam_position.push_back(VecPosition(x-3,y-2,0));
    deam_position.push_back(VecPosition(x+3,y+2,0));
    deam_position.push_back(VecPosition(x+4,y-2,0));

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


vector<VecPosition> NaoBehavior::demoMode_2(VecPosition ball)
{
    //示例，有需要请自行修改
    deam_position.clear();
    float x = ball.getX();

    for(int i=0;i<9;i++){
        deam_position.push_back(VecPosition(-7.5,-4+i,0));
    }

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












