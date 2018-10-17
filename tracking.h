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
{  //����Ϊdet.txt�ļ������Ϊbbox�������id��txt�ļ�
public:
	std::string tracking(std::string file_det);  //����det.txt�ļ�����������id��det.txt ��·��
	std::vector<recId> tracking_id(std::vector<Rec> bbox);

private:
	std::vector<Rec> trackers_;
	std::vector<recId> trackers_ids_;  //��N-1֡��bbox�����е�id
	std::vector<std::vector<double> > stt_det;    //�������det������
	std::vector<std::vector<double> > readTxt_det(std::string txt_det);   //����det�ļ�
	std::set<int> iset;    //����֡��
	std::vector<Rec> bbox_nFrame_det(int k);
	double Pose_IOU_rate(Rec a, Rec b);

};