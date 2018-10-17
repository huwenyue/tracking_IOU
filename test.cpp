#include "evaluation_interface.h"
#include "tracking.h"
#include "rec.h"
#include <fstream>
#include <sstream>
#include <opencv2\opencv.hpp>
#include "putText.h"

using namespace cv;
using namespace std;

//寻址路径
const string address = "D:\\Downloads\\MOT17_data\\train\\MOT17-04-SDP\\";
//储存路径
const string csv_store = "D:\\Downloads\\MOT17_data\\train\\MOT17-04-SDP.csv";



int main() {
	TrackingFunc tracker;
	Evaluation eval;
	string det_path = address + "det\\det.txt";
	string outfile_path = tracker.tracking(det_path);  //读入det文件并返回bbox和id的txt文件
	//string outfile_path = "D:/Downloads/MOT17_data/train/detection-04-FRCNN.txt";
	string gt_path = address + "gt\\gt.txt";
	eval.evaluation(gt_path, outfile_path);
	double Fp = eval.getFP();
	double Mt = eval.getMT();
	double Ml = eval.getML();
	int size = eval.size_; 

	for (int r = 1; r <= eval.iset_size; r++) {
		//读入每一帧的图片
		string str;
		if (r < 10) {
			str = address + "img1\\00000";

		}
		else if (r < 100) {
			str = address + "img1\\0000";
		}
		else {
			str = address + "img1\\000";
		}
		str += std::to_string(r);
		str += ".jpg";
		cout << str << endl;

		//gt
		//read image
		Mat img_gt = imread(str);
		for (int i = 0; i < eval.st_gt.size(); i++) {
			if (eval.st_gt[i][0] == r ) {
				//找到gt中对应帧数与r相同则画框，帧数从1开始
				Rect bb;
				bb.x = eval.st_gt[i][2];
				bb.y = eval.st_gt[i][3];
				bb.width = eval.st_gt[i][4];
				bb.height = eval.st_gt[i][5];
				rectangle(img_gt, bb, Scalar(255, 0, 0));    //blue
															 //imshow("test", img_gt);
/*				PutText ob_id;
				ob_id.drawid(img_gt, { to_string(int(eval.st_gt[i][1])) }, bb.x, bb.y);	*/										 //waitKey(0);
			}
		}
		//imshow("test", img_gt);
		//waitKey(0);
		// restore labeled gt
		//string str_gt = address + "img_gt\\img_gt_";
		//str_gt += std::to_string(r);
		//str_gt += ".jpg";
/*		cout << str_gt << endl;
		imwrite(str_gt, img_gt);*/   //restore


		//det
		//read image
		Mat img_det = imread(str);
		int count = 0;
		for (int j = 0; j < eval.st_det.size(); j++) {
			if (eval.st_det[j][0] == r ) {
				//找到gt中对应帧数与r相同则画框，帧数从1开始
				count++;
				Rect b;
				b.x = eval.st_det[j][2];
				b.y = eval.st_det[j][3];
				b.width = eval.st_det[j][4];
				b.height = eval.st_det[j][5];
				rectangle(img_gt, b, Scalar(0, 0, 255));    //red

				//PutText ob_id;
				//ob_id.drawid(img_gt, {to_string(int(eval.st_det[j][1]))}, b.x + b.width, b.y + b.height);
			}
		}

		
		
		cout << "det bbox in frame : " << count << endl;
		//label in one image
		//imshow("det", img_gt);
		//waitKey(1);
		// restore labeled det
		string str_det = address + "img_det\\img_det_";
		str_det += std::to_string(r);
		str_det += ".jpg";
		cout << str_det << endl;
		//imshow("tmp", img_gt);
		//waitKey(100);
		imwrite(str_det, img_gt);   //restore
	}


	//输出为csv文件
	ofstream outfile;
	outfile.open(csv_store);
	outfile << "fp" << ',' << "mt" << ',' << "ml" << ',' <<"size" << endl;
	outfile << Fp << ',' << Mt << ',' << Ml <<','<<size<<endl;
	outfile.close();

	cout << "evaluation succeed! " << endl;
	getchar();

	//void fileEmpty(const string fileName)
	//{
	//	fstream file(fileName, ios::out);
	//}

	

	return 0;
}