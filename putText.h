
#include <windows.h>
#include <opencv2/opencv.hpp>

class PutText {
public:
	void draw_student_info(cv::Mat &img, std::vector<cv::Rect> rects, std::vector<std::string> names, std::vector<std::string> Emotions, std::vector<std::string> IsRaiseHands);
	
public:
    void GetStringSize(HDC hDC, const char* str, int* w, int* h);
    void putTextZH(cv::Mat &dst, const char* str, cv::Point org, cv::Scalar color, int fontSize,
        const char *fn = "Arial", bool italic = false, bool underline = false);
    //void drawrectangle(cv::Mat img, int a, int b, int c);
    void drawid(cv::Mat img, std::vector<std::string>sentence, int disx, int disy);
    void drawhistogram(cv::Mat img, int a, int b, int gailv, int t, int dis, int disx, cv::Scalar col);
    void drawline(cv::Mat img, int a, int b, int gailv1, int gailv2, int t, int dis, int disx, cv::Scalar col);
    void draw(cv::Mat img, std::vector<int>a, int x = 50, int y = 600, int dis = 30, int disx = 10, cv::Scalar col = cv::Scalar(255, 255, 0), cv::Scalar col1 = cv::Scalar(89, 90, 90), int n = 10);
    void DrawTransRec(IplImage* img, int x, int y, int width, int height, CvScalar color, double alpha);
    PutText();
    ~PutText();
};

