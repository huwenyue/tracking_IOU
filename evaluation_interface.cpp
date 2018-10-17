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
			//��classΪ1��classΪ7
			if (stu[8] > vis) {
				st_gt.push_back(stu);
			}
		}
	}
	return st_gt;
}

vector<vector<double> > Evaluation::readTxt_det(string txt_det) {
	//����det�ļ�
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
	return st_det; //���һ��Ϊnew_id
}

vector<Rec> Evaluation::bbox_nFrame_gt(int k) {
	//���ÿһ֡���е�bbox
	vector<Rec> st_bbox;
	vector<int> st_id_gt;
	Rec temp;
	st_bbox.clear();
	st_id_gt.clear();

	for (int kk = 0; kk < st_gt.size(); kk++) {
		//����st���ҵ���N֡������bbox
		if (st_gt[kk][0] == k + 1 ) {
			//�ҵ���k֡��֡��1��ʼ��
			temp.x = st_gt[kk][2];
			temp.y = st_gt[kk][3];
			temp.width = st_gt[kk][4];
			temp.height = st_gt[kk][5];
			temp.id = st_gt[kk][1];
			
			st_bbox.push_back(temp);  //�����k֡��bbox
			st_id_gt.push_back(temp.id);

		}
	}

	return st_bbox;
}

vector<Rec> Evaluation::bbox_nFrame_det(int k) {
	//���ÿһ֡���е�bbox
	vector<Rec> st_bbox;
	vector<int> st_id_det;
	Rec temp;
	st_bbox.clear();
	st_id_det.clear();

	for (int kk = 0; kk < st_det.size(); kk++) {
		//����st���ҵ���N֡������bbox
		if (st_det[kk][0] == k + 1) {
			//�ҵ���k֡��֡��1��ʼ��
			temp.x = st_det[kk][2];
			temp.y = st_det[kk][3];
			temp.width = st_det[kk][4];
			temp.height = st_det[kk][5];
			temp.id = st_det[kk][1];
			temp.new_id = st_det[kk][6];  

			st_bbox.push_back(temp);  //�����k֡��bbox
			st_id_det.push_back(temp.id);
			
		}
	}
	return st_bbox;
}

void Evaluation::trackEvaluation(vector<Rec> _bbox_gt, vector<Rec> _bbox_det, int k) {
	vector<recId> id_det;     //��N֡����bbox id��׷��ֵ
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
			//����iou
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
			//����������N֡��gt
			t_gt.id = _bbox_gt[i].id;
			t_gt.new_id = _bbox_gt[i].new_id;
			id_gt.push_back(t_gt);

			//����������N֡��det��bbox
			t_det.id = _bbox_det[max_rate_index].id;
			t_det.new_id = _bbox_det[max_rate_index].new_id;
			id_det.push_back(t_det);

			//��ֵӳ��
			map_id_n.insert(pair<int, recId>(t_gt.id, t_det));
			//map_id_n[t_gt] = t_det;
		}
		//��Ӧid�Ĺ켣������1,������±�Ϊ0��ʼ�洢idΪ1
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
					//��gt�ĵ�N-1֡�������N֡��ͬ��id
					if (id_b[rr].id == id_gt[r].id) {

						id_gt[r].new_id = 0;  //old id

											   //���ҵ�,��鿴det�Ƿ���ٶ�
						if (map_id_m.at(id_b[rr].id).id == map_id_n.at(id_gt[r].id).id) {
							//N-1��Nƥ��ɹ�
							if (id_gt[r].id < n_s) {
								//id��1��ʼ
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
						break;  //��N-1֡�ҵ���Ӧid������ѭ��
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
	set<int> iset;    //����֡��
	set<int> idset;   //����id�����ֵ
	
	vector<Rec> bbox_gt;  //��N֡��gt����bbox
	vector<Rec> bbox_det;  //��N֡��det����bbox

	ofstream fout;
	//fout.open(fout_add, ios::ate);  //���
	//fout.clear();

	for (int i = 0; i < stt_gt.size(); i++) {
		//ȥ�أ��õ�֡��
		iset.insert(stt_gt[i][0]);
		//ȥ�أ��õ�id���ֵ������idset��
		idset.insert(stt_gt[i][1]);
	}
	//id�ĸ���
	size_ = idset.size();
	//֡��
	iset_size = iset.size();
	cout << "id����: " << size_ << endl;

	for (int r = 0; r < iset.size(); r++) {

		//����ÿһ֡����������BBOX
		bbox_gt = bbox_nFrame_gt(r);  //�õ�gt�ļ��е�k֡���е�bbox��id
		bbox_det = bbox_nFrame_det(r);  //�õ�det�ļ��е�k֡���е�bbox��id
		trackEvaluation(bbox_gt, bbox_det, r);  // ��r��
	}
	int s = *(--idset.end());
	cout << "id���ֵ: " << s << endl;
	vector<int> i_id;
	for (int i = 0; i < s; i++) {

		if (n_n[i]>0) { //��ĸ����Ϊ0
			//ÿ��id���ܹ켣��
			p_a.push_back(n_n[i]);

			i_id.push_back(i);// gt��id

			if (n[i] > 0) {
				//ÿ��idƥ��ɹ��ĸ���
				p.push_back(double(n[i]));
			}
			else if (n[i] == 0) {
				p.push_back(0);
			}

		}
		else if (n_n[i] == 0) {
			//p_a.push_back(0);
			cout << "��ĸ����Ϊ0!" << endl;
		}
	}

	//�켣Ϊ0��id������
	fout << endl << "�켣����" << p_a.size() << endl;

	for (int j = 0; j < p_a.size(); j++)
	{
		id_n.push_back(p[j] / p_a[j]);

		fout.open(fout_add, ios::app);
		fout << "id:" << i_id[j]<< " "<< "ƥ��ɹ���:" << p[j] << " " << "��������:" << p_a[j] << " " << "���ٹ켣����:" << id_n[j] << endl;
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


