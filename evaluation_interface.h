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
	int size_ ;  //id的总数量
	int iset_size;
	//double getHZ();
	std::vector<std::vector<double> > st_gt;  //保存读入的gt数据
	std::vector<std::vector<double> > st_det;

	void trackEvaluation(std::vector<Rec> _bbox_gt, std::vector<Rec> _bbox_det, int k);
	void evaluation(std::string txt_Gt, std::string txt_Det);  //txtdet为初始det，txt_Det为已分配好id的det.txt
	Evaluation();

private:

	std::vector<recId> id_b;   // 第一帧所有bbox id的标准值
	std::vector<recId> id_t;     //第一帧所有bbox id的追踪值
	std::map<int, recId> map_id_n;
	std::map<int, recId> map_id_m;
	//std::vector<Rec> id_b;   // 第N-1帧所有bbox id的标准值
	//std::vector<Rec> id_t;     //第N-1帧所有bbox id的追踪值
	int n[n_s];  //记录每一帧所有bbox追踪对的个数
	int n_n[n_s];   //记录在所有帧中每一个id出现的次数
	std::vector<double> p;       //每个id匹配成功的个数
	std::vector<double> p_a;     //每个id的总轨迹数
	std::vector<double> id_n;  //记录每个id匹配成功在总帧数中所占的时间
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
	//double time_hz(string file_det);   //传入det.txt文件

	
};
