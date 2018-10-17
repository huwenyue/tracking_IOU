#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <time.h>
#include <map>
#include"evaluation_interface.h"
#define vis 0.8
using namespace std;

const string fout_add = "D:\\Downloads\\MOT17_data\\train\\04-SDP.txt";


Evaluation::Evaluation() {
	n[n_s] = { 0 };
	n_n[n_s] = { 0 };
	mt_ = 0, ml_ = 0;
}

vector<vector<double> > Evaluation::readTxt_gt(string txt_gt) {
	string buffer;
	ifstream Txt;
	Txt.open(txt_gt);
	while (getline(Txt, buffer))
	{
		vector<double> stu;

		double s = 0;
		stringstream sstr(buffer);
		string token;
		stringstream s_stream;
		while (getline(sstr, token, ',')) {
			s_stream.clear();
			s_stream << token;
			s_stream >> s;
			stu.push_back(s);

		}
		if (stu[7] == 1 || stu[7] == 7) {
			//把class为1和class为7
			if (stu[8] > vis) {
				st_gt.push_back(stu);
			}
		}
	}
	return st_gt;
}

vector<vector<double> > Evaluation::readTxt_det(string txt_det) {
	//读入det文件
	string buffer_det;
	ifstream Txt_det;
	Txt_det.open(txt_det);
	while (getline(Txt_det, buffer_det))
	{
		vector<double> stu;

		double s = 0;
		stringstream sstr(buffer_det);
		string token;
		stringstream s_stream;
		while (getline(sstr, token, ',')) {
			s_stream.clear();
			s_stream << token;
			s_stream >> s;
			stu.push_back(s);

		}
		st_det.push_back(stu);
	}
	return st_det; //最后一列为new_id
}

vector<Rec> Evaluation::bbox_nFrame_gt(int k) {
	//输出每一帧所有的bbox
	vector<Rec> st_bbox;
	vector<int> st_id_gt;
	Rec temp;
	st_bbox.clear();
	st_id_gt.clear();

	for (int kk = 0; kk < st_gt.size(); kk++) {
		//遍历st，找到第N帧的所有bbox
		if (st_gt[kk][0] == k + 1 ) {
			//找到第k帧（帧从1开始）
			temp.x = st_gt[kk][2];
			temp.y = st_gt[kk][3];
			temp.width = st_gt[kk][4];
			temp.height = st_gt[kk][5];
			temp.id = st_gt[kk][1];
			
			st_bbox.push_back(temp);  //输出第k帧的bbox
			st_id_gt.push_back(temp.id);

		}
	}

	return st_bbox;
}

vector<Rec> Evaluation::bbox_nFrame_det(int k) {
	//输出每一帧所有的bbox
	vector<Rec> st_bbox;
	vector<int> st_id_det;
	Rec temp;
	st_bbox.clear();
	st_id_det.clear();

	for (int kk = 0; kk < st_det.size(); kk++) {
		//遍历st，找到第N帧的所有bbox
		if (st_det[kk][0] == k + 1) {
			//找到第k帧（帧从1开始）
			temp.x = st_det[kk][2];
			temp.y = st_det[kk][3];
			temp.width = st_det[kk][4];
			temp.height = st_det[kk][5];
			temp.id = st_det[kk][1];
			temp.new_id = st_det[kk][6];  

			st_bbox.push_back(temp);  //输出第k帧的bbox
			st_id_det.push_back(temp.id);
			
		}
	}
	return st_bbox;
}

void Evaluation::trackEvaluation(vector<Rec> _bbox_gt, vector<Rec> _bbox_det, int k) {
	vector<recId> id_det;     //第N帧所有bbox id的追踪值
	vector<recId> id_gt;
	recId t_det;
	recId t_gt;
	vector<recId> temp_det;
	vector<recId> temp_gt;

	temp_det.clear();
	temp_gt.clear();
	id_det.clear();
	id_gt.clear();
	map_id_n.clear();

	for (int i = 0; i < _bbox_gt.size(); i++) {

		double max_rate = 0;
		int max_rate_index = -1;
		for (int j = 0; j < _bbox_det.size(); j++) {
			//计算iou
			double iou_rate = IOU_rate(_bbox_gt[i], _bbox_det[j]);
			if (iou_rate > max_rate)
			{
				max_rate = iou_rate;
				max_rate_index = j;
			}
		}
		if (max_rate > iou)
		{
			//update id_det_N and id_gt_N
			//满足条件第N帧的gt
			t_gt.id = _bbox_gt[i].id;
			t_gt.new_id = _bbox_gt[i].new_id;
			id_gt.push_back(t_gt);

			//满足条件第N帧的det的bbox
			t_det.id = _bbox_det[max_rate_index].id;
			t_det.new_id = _bbox_det[max_rate_index].new_id;
			id_det.push_back(t_det);

			//赋值映射
			map_id_n.insert(pair<int, recId>(t_gt.id, t_det));
			//map_id_n[t_gt] = t_det;
		}
		//对应id的轨迹总数加1,数组从下标为0开始存储id为1
		n_n[_bbox_gt[i].id - 1] += 1;
	}

	if (id_det.size() != 0 && id_gt.size() != 0) {
		if (id_b.size() == 0 && id_t.size() == 0) {
			//update N-1
			for (int i = 0; i < id_gt.size(); i++) {
				id_b.push_back(id_gt[i]);
				id_t.push_back(id_det[i]);
				map_id_m.insert(pair<int, recId>(id_gt[i].id, id_det[i]));
				n[id_gt[i].id - 1] += 1;
			}
		}
		else {
			for (int r = 0; r < id_gt.size(); r++) {

				for (int rr = 0; rr < id_b.size(); rr++) {
					//在gt的第N-1帧中找与第N帧相同的id
					if (id_b[rr].id == id_gt[r].id) {

						id_gt[r].new_id = 0;  //old id

											   //若找到,则查看det是否跟踪对
						if (map_id_m.at(id_b[rr].id).id == map_id_n.at(id_gt[r].id).id) {
							//N-1与N匹配成功
							if (id_gt[r].id < n_s) {
								//id从1开始
								n[id_gt[r].id - 1] += 1;

								//update N-1 , store right id
								temp_gt.push_back(id_gt[r]);
								temp_det.push_back(id_det[r]);
							}
							else {
								cout << "out size of n[i]" << endl;
							}
						}
						else {
							fp += 1;
						}
						break;  //在N-1帧找到对应id则跳出循环
					}
				}
			}

			for (int r = 0; r < id_gt.size(); r++) {
				if (id_gt[r].new_id == 1) {
					
					n[id_gt[r].id - 1] += 1;
					//update , store new id
					temp_gt.push_back(id_gt[r]);
					temp_det.push_back(id_det[r]);
				}
			}

			if (temp_det.size() != 0 && temp_gt.size() != 0) {
				id_b.clear();
				id_t.clear();
				map_id_m.clear();
				for (int k = 0; k < temp_det.size(); k++) {
					id_b.push_back(temp_gt[k]);
					id_t.push_back(temp_det[k]);
					map_id_m.insert(pair<int, recId>(temp_gt[k].id, temp_det[k]));
				}
			}

		}
	}
}

double Evaluation::IOU_rate(Rec a, Rec b)
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

//double Evaluation::time_hz(string file_det)
//{
//	double Hz = 0;
//	time_t start = 0, stop = 0;
//	TrackingFunc tracker;
//	time(&start);
//
//	for (int kk = 0; kk < 1e7; kk++) {
//
//		tracker.tracking(file_det);
//
//	}
//	time(&stop);
//	Hz = (double) 1e7 / (stop - start);
//	return Hz;
//}

void Evaluation::evaluation(string txt_Gt, string txt_Det) {

	vector<vector<double> > stt_gt = readTxt_gt(txt_Gt);
	cout << "read gt txt ready!" << endl;
	vector<vector<double> > stt_det = readTxt_det(txt_Det);
	cout << "read det txt ready!" << endl;
	set<int> iset;    //保存帧数
	set<int> idset;   //保存id的最大值
	
	vector<Rec> bbox_gt;  //第N帧中gt所有bbox
	vector<Rec> bbox_det;  //第N帧中det所有bbox

	ofstream fout;
	//fout.open(fout_add, ios::ate);  //清空
	//fout.clear();

	for (int i = 0; i < stt_gt.size(); i++) {
		//去重，得到帧数
		iset.insert(stt_gt[i][0]);
		//去重，得到id最大值储存在idset中
		idset.insert(stt_gt[i][1]);
	}
	//id的个数
	size_ = idset.size();
	//帧数
	iset_size = iset.size();
	cout << "id个数: " << size_ << endl;

	for (int r = 0; r < iset.size(); r++) {

		//对于每一帧，遍历所有BBOX
		bbox_gt = bbox_nFrame_gt(r);  //得到gt文件中第k帧所有的bbox和id
		bbox_det = bbox_nFrame_det(r);  //得到det文件中第k帧所有的bbox和id
		trackEvaluation(bbox_gt, bbox_det, r);  // 第r轮
	}
	int s = *(--idset.end());
	cout << "id最大值: " << s << endl;
	vector<int> i_id;
	for (int i = 0; i < s; i++) {

		if (n_n[i]>0) { //分母不能为0
			//每个id的总轨迹数
			p_a.push_back(n_n[i]);

			i_id.push_back(i);// gt的id

			if (n[i] > 0) {
				//每个id匹配成功的个数
				p.push_back(double(n[i]));
			}
			else if (n[i] == 0) {
				p.push_back(0);
			}

		}
		else if (n_n[i] == 0) {
			//p_a.push_back(0);
			cout << "分母不能为0!" << endl;
		}
	}

	//轨迹为0的id不考虑
	fout << endl << "轨迹个数" << p_a.size() << endl;

	for (int j = 0; j < p_a.size(); j++)
	{
		id_n.push_back(p[j] / p_a[j]);

		fout.open(fout_add, ios::app);
		fout << "id:" << i_id[j]<< " "<< "匹配成功数:" << p[j] << " " << "跟踪总数:" << p_a[j] << " " << "跟踪轨迹比例:" << id_n[j] << endl;
		fout.close();

		if (id_n[j] >= 0.8) {
			mt_ += 1;

		}
		else if (id_n[j] <= 0.2 && id_n[j] >= 0) {
			ml_ += 1;
		}
	}
	mt = mt_ / size_ * 100;
	ml = ml_ / size_ * 100;
	//hz = time_hz(txtdet);
}

int Evaluation::getFP() {
	return fp;
}

double Evaluation::getML() {
	return ml_;
}
double Evaluation::getMT() {
	return mt_;
}

//double Evaluation::getHZ() {
//	return hz;
//}


