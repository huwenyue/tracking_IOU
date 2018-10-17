#pragma once
#include <iostream>
#include <vector>
#include<iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include "rec.h"

class TrackingFunc
{  //输入为det.txt文件，输出为bbox的坐标和id的txt文件
public:
	std::string tracking(std::string file_det);  //传入det.txt文件，输出分配好id的det.txt 的路径
	std::vector<recId> tracking_id(std::vector<Rec> bbox);

private:
	std::vector<Rec> trackers_;
	std::vector<recId> trackers_ids_;  //第N-1帧中bbox里所有的id
	std::vector<std::vector<double> > stt_det;    //保存读入det的数据
	std::vector<std::vector<double> > readTxt_det(std::string txt_det);   //读入det文件
	std::set<int> iset;    //保存帧数
	std::vector<Rec> bbox_nFrame_det(int k);
	double Pose_IOU_rate(Rec a, Rec b);

};