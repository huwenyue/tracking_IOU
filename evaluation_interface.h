#pragma once
#include <string>
#include <vector>
#include <map>
#include "rec.h"
#define iou 800
#define n_s 400

class Evaluation {
public:
	int getFP();
	double getML();
	double getMT();
	int size_ ;  //id��������
	int iset_size;
	//double getHZ();
	std::vector<std::vector<double> > st_gt;  //��������gt����
	std::vector<std::vector<double> > st_det;

	void trackEvaluation(std::vector<Rec> _bbox_gt, std::vector<Rec> _bbox_det, int k);
	void evaluation(std::string txt_Gt, std::string txt_Det);  //txtdetΪ��ʼdet��txt_DetΪ�ѷ����id��det.txt
	Evaluation();

private:

	std::vector<recId> id_b;   // ��һ֡����bbox id�ı�׼ֵ
	std::vector<recId> id_t;     //��һ֡����bbox id��׷��ֵ
	std::map<int, recId> map_id_n;
	std::map<int, recId> map_id_m;
	//std::vector<Rec> id_b;   // ��N-1֡����bbox id�ı�׼ֵ
	//std::vector<Rec> id_t;     //��N-1֡����bbox id��׷��ֵ
	int n[n_s];  //��¼ÿһ֡����bbox׷�ٶԵĸ���
	int n_n[n_s];   //��¼������֡��ÿһ��id���ֵĴ���
	std::vector<double> p;       //ÿ��idƥ��ɹ��ĸ���
	std::vector<double> p_a;     //ÿ��id���ܹ켣��
	std::vector<double> id_n;  //��¼ÿ��idƥ��ɹ�����֡������ռ��ʱ��
	std::vector<Rec> bbox_nFrame_gt(int k);
	std::vector<Rec> bbox_nFrame_det(int k);

	double mt_, ml_;
	int fp;
	double ml;
	double mt;
	double hz;
	std::vector<std::vector<double> > readTxt_gt(std::string txt_gt);
	std::vector<std::vector<double> > readTxt_det(std::string txt_det);
	
	double IOU_rate(Rec a, Rec b);
	//double time_hz(string file_det);   //����det.txt�ļ�

	
};
