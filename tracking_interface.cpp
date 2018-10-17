#pragma once
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <time.h>
#include "tracking.h"
using namespace std;

const string address_id = "D:\\Downloads\\MOT17_data\\train\\detection-04-SDP.txt";

vector<vector<double> > TrackingFunc::readTxt_det(string txt_det) {
	//读入det文件
	vector<vector<double> > st_det;
	string buffer_det;
	ifstream Txt_det;
	Txt_det.open(txt_det);
	while (getline(Txt_det, buffer_det))   //循环读入每一行
	{
		vector<double> stu;

		double s = 0;
		stringstream sstr(buffer_det);
		string token;
		stringstream s_stream;
		while (getline(sstr, token, ',')) {   //以，分割行内数据
			s_stream.clear();
			//将string类型转换为double
			s_stream << token;
			s_stream >> s;
			stu.push_back(s);

		}
		st_det.push_back(stu);
	}
	return st_det;
}

double TrackingFunc::Pose_IOU_rate(Rec a, Rec b)
{
	double area = 0;
	double width_intersect = min(a.x + a.width, b.x + b.width) - max(a.x, b.x);
	double height_intersect = min(a.y + a.height, b.y + b.height) - max(a.y, b.y);
	double area_intersect = width_intersect * height_intersect * 1000;
	double width_union = a.width + b.width - width_intersect;
	double height_union = a.height + b.height - height_intersect;
	double area_union = width_union * height_union;
	double area_rate = area_intersect / area_union;
	if (width_intersect > 0 && height_intersect > 0)
	{
		area = area_rate;
	}
	return area;
}

vector<Rec> TrackingFunc::bbox_nFrame_det(int k) {
	//输出每一帧所有的bbox
	vector<Rec> st_bbox;
	vector<int> st_id_det;
	Rec temp;
	st_bbox.clear();
	st_id_det.clear();
	for (int kk = 0; kk < stt_det.size(); kk++) {
		//遍历st，找到第N帧的所有bbox
		if (stt_det[kk][0] == k + 1) {
			//找到第k帧（帧从1开始）
			temp.id = stt_det[kk][1];
			temp.x = stt_det[kk][2];
			temp.y = stt_det[kk][3];
			temp.width = stt_det[kk][4];
			temp.height = stt_det[kk][5];
			
			st_bbox.push_back(temp);  //输出第k帧的bbox
			st_id_det.push_back(temp.id);

		}
	}
	return st_bbox;
}

vector<recId> TrackingFunc::tracking_id(vector<Rec> bbox) {

	recId t;
	recId id_tm;
	if (trackers_.size() > 0)
	{
		//add new ids to all bbox
		//TODO:
		for (int tt = 0; tt < bbox.size(); tt++)
		{  //给bbox的id重新随机赋值
			int id_tmp = rand() % 100000;
			bbox[tt].id = id_tmp;
		}

		for (int j = 0; j< bbox.size(); j++)
		{
			double max_rate = 0;
			int max_rate_index = -1;
			for (int i = 0; i < trackers_.size(); i++)
			{
				double iou_rate = Pose_IOU_rate(bbox[j], trackers_[i]);
				if (iou_rate > max_rate)
				{
					max_rate = iou_rate;
					max_rate_index = i;
				}
			}
			if (max_rate> 800)
			{
				//add trackers[i]'s id to bbox[i]
				//TODO:
				bbox[j].id = trackers_ids_[max_rate_index].id;  //将trackers中iou最大值的id给bbox
				bbox[j].new_id = 0;  //不是new id 
			}
		}
		//update tackers_ and tracker_ids_
		trackers_.clear();
		trackers_ids_.clear();
		for (int k = 0; k < bbox.size(); k++) {
			trackers_.push_back(bbox[k]);
			t.id = bbox[k].id;
			t.new_id = bbox[k].new_id;
			trackers_ids_.push_back(t);
		}

	}
	else
	{
		//add total bbox to trackers_
		//add new id for all bbox ; update trackers_ids_
		for (int i = 0; i < bbox.size(); i++)
		{
			trackers_.push_back(bbox[i]);
			//add new ids to all 
			//TODO: trackers_ids 随机赋值
			id_tm.id = rand() % 100000;
			trackers_ids_.push_back(id_tm);
		}
	}
	return trackers_ids_;
}

string TrackingFunc::tracking(string file_det) {
	vector<Rec> bbox_det;  //第N帧中det所有bbox
	vector<recId> trackers_id;  //第N帧中所有bbox的id
	ofstream outf;
	string det_path = address_id;
	stt_det = readTxt_det(file_det);

	//outf.open(det_path, ios::ate);  //清空

	for (int i = 0; i < stt_det.size(); i++) {
		//去重，得到帧数
		iset.insert(stt_det[i][0]);
	}

	
	for (int r = 0; r < iset.size(); r++) {

		
		//对于每一帧，遍历所有BBOX
		bbox_det = bbox_nFrame_det(r);  //得到det文件中第k帧所有的bbox和id
		trackers_id = tracking_id(bbox_det);   //追踪函数得到det里bbox的id
		
		for (int k = 0; k < bbox_det.size(); k++) {
			outf.open(det_path, ios::app);
			bbox_det[k].id = trackers_id[k].id;
			bbox_det[k].new_id = trackers_id[k].new_id;
			//帧数从1开始计算
			outf <<r+1<<","<< bbox_det[k].id << "," << bbox_det[k].x << "," << bbox_det[k].y  << "," << bbox_det[k].width << "," << bbox_det[k].height <<","<<bbox_det[k].new_id<< endl;
			outf.close();
		}
	}

	return det_path;
}
