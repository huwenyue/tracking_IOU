
#include "putText.h"
#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

#pragma warning(disable: 4267)

PutText::PutText()
{
    ;
}

PutText::~PutText()
{
    ;
}



void PutText::GetStringSize(HDC hDC, const char* str, int* w, int* h)
{
    SIZE size;
    GetTextExtentPoint32A(hDC, str, strlen(str), &size);
    if (w != 0) *w = size.cx;
    if (h != 0) *h = size.cy;
}

void PutText::putTextZH(cv::Mat &dst, const char* str, cv::Point org, cv::Scalar color, int fontSize, const char* fn, bool italic, bool underline)
{
    CV_Assert(dst.data != 0 && (dst.channels() == 1 || dst.channels() == 3));

    int x, y, r, b;
    if (org.x > dst.cols || org.y > dst.rows) return;
    x = org.x < 0 ? -org.x : 0;
    y = org.y < 0 ? -org.y : 0;

    LOGFONTA lf;
    lf.lfHeight = -fontSize;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = 5;
    lf.lfItalic = italic;
    lf.lfUnderline = underline;
    lf.lfStrikeOut = 0;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = 0;
    lf.lfClipPrecision = 0;
    lf.lfQuality = PROOF_QUALITY;
    lf.lfPitchAndFamily = 0;
    strcpy_s(lf.lfFaceName, fn);

    HFONT hf = CreateFontIndirectA(&lf);
    HDC hDC = CreateCompatibleDC(0);
    HFONT hOldFont = (HFONT)SelectObject(hDC, hf);

    int strBaseW = 0, strBaseH = 0;
    int singleRow = 0;
    char buf[1 << 12];
    strcpy_s(buf, str);
    char *bufT[1 << 12];
    {
        int nnh = 0;
        int cw, ch;

        const char* ln = strtok_s(buf, "\n", bufT);
        while (ln != 0)
        {
            GetStringSize(hDC, ln, &cw, &ch);
            strBaseW = __max(strBaseW, cw);
            strBaseH = __max(strBaseH, ch);

            ln = strtok_s(0, "\n", bufT);
            nnh++;
        }
        singleRow = strBaseH;
        strBaseH *= nnh;
    }

    if (org.x + strBaseW < 0 || org.y + strBaseH < 0)
    {
        SelectObject(hDC, hOldFont);
        DeleteObject(hf);
        DeleteObject(hDC);
        return;
    }

    r = org.x + strBaseW > dst.cols ? dst.cols - org.x - 1 : strBaseW - 1;
    b = org.y + strBaseH > dst.rows ? dst.rows - org.y - 1 : strBaseH - 1;
    org.x = org.x < 0 ? 0 : org.x;
    org.y = org.y < 0 ? 0 : org.y;

    BITMAPINFO bmp = { 0 };
    BITMAPINFOHEADER& bih = bmp.bmiHeader;
    int strDrawLineStep = strBaseW * 3 % 4 == 0 ? strBaseW * 3 : (strBaseW * 3 + 4 - ((strBaseW * 3) % 4));

    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = strBaseW;
    bih.biHeight = strBaseH;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = BI_RGB;
    bih.biSizeImage = strBaseH * strDrawLineStep;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;

    void* pDibData = 0;
    HBITMAP hBmp = CreateDIBSection(hDC, &bmp, DIB_RGB_COLORS, &pDibData, 0, 0);

    HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hBmp);

    SetTextColor(hDC, RGB(255, 255, 255));
    SetBkColor(hDC, 0);

    strcpy_s(buf, str);
    const char* ln = strtok_s(buf, "\n", bufT);
    int outTextY = 0;
    while (ln != 0)
    {
        TextOutA(hDC, 0, outTextY, ln, strlen(ln));
        outTextY += singleRow;
        ln = strtok_s(0, "\n", bufT);
    }
    uchar* dstData = (uchar*)dst.data;
    int dstStep = dst.step / sizeof(dstData[0]);
    unsigned char* pImg = (unsigned char*)dst.data + org.x * dst.channels() + org.y * dstStep;
    unsigned char* pStr = (unsigned char*)pDibData + x * 3;
    for (int tty = y; tty <= b; ++tty)
    {
        unsigned char* subImg = pImg + (tty - y) * dstStep;
        unsigned char* subStr = pStr + (strBaseH - tty - 1) * strDrawLineStep;
        for (int ttx = x; ttx <= r; ++ttx)
        {
            for (int n = 0; n < dst.channels(); ++n) {
                double vtxt = subStr[n] / 255.0;
                int cvv = vtxt * color.val[n] + (1 - vtxt) * subImg[n];
                subImg[n] = cvv > 255 ? 255 : (cvv < 0 ? 0 : cvv);
            }

            subStr += 3;
            subImg += dst.channels();
        }
    }

    SelectObject(hDC, hOldBmp);
    SelectObject(hDC, hOldFont);
    DeleteObject(hf);
    DeleteObject(hBmp);
    DeleteDC(hDC);
}
void PutText::drawid(cv::Mat img, vector<string>sentence, int disx, int disy)
{
    int length;
    if (sentence.size() != 0)
        length = sentence[0].length();
    for (int i = 0; i < sentence.size(); i++)
    {
        if (length < sentence[i].length())
        {
            length = sentence[i].length();
        }

    }
    // cout << length;
    cv::rectangle(img, Rect(disx, disy, length * 14, 20 * sentence.size() + 10), Scalar(100, 95, 85), 1, 1);
    for (int i = 0; i < sentence.size(); i++)
    {
        putTextZH(img, sentence[i].data(), Point(disx, 20 * i + 0 + disy), Scalar(255, 255, 255), 25, "Arial");
    }

}

void PutText::drawhistogram(cv::Mat img, int a, int b, int gailv, int t, int dis, int disx, cv::Scalar col)
{
    cv::rectangle(img, Rect(a + 20 + t*dis + disx*t, b - gailv, dis, gailv), col, 2, 1);

}
void PutText::drawline(cv::Mat img, int a, int b, int gailv1, int gailv2, int t, int dis, int disx, cv::Scalar col)
{

    Point p1(a + 20 + t * dis + disx * t + dis / 2, b - gailv1);
    Point p2(a + 20 + t * dis + disx * t + dis / 2 + dis + disx, b - gailv2);
    line(img, p1, p2, col, 1);


}

void PutText::draw(cv::Mat img, vector<int>a, int x, int y, int dis, int disx, cv::Scalar col, cv::Scalar col1, int n)
{//xy:coordinate//dis:Bar chart width,disx Distance between bar //col line color//col1 bar color//vector<int>a: Visitors flowrate

    int start = 0, end, height = 200, xinterval = 1, time = 20, count = 0;

    for (int i = 8; i < time + 1; i = i + xinterval)
    {
        putTextZH(img, to_string(i).data(), Point((dis + 2 * dis * count + 2 * disx * count) / 2 + 20 + x - 8, y), Scalar(255, 255, 255), 20, "Arial");
        count++;
    }
    end = dis + dis * count + disx * count;
    putTextZH(img, "ͳ number/h", Point((end - start) / 2 - 50, y + 20), Scalar(255, 255, 255), 20, "Arial");
    //IplImage *p = &IplImage(img);


    //DrawTransRec(p, x, y - height, end, height, Scalar(255, 255, 255), 1);
    Point p1(x, y);//
    Point p2(x + end, y);
    Point p3(x, y - height);

    Point p4(x + end - 5, y - 3);
    Point p5(x + end - 5, y + 3);

    Point p6(x + 3, y - height + 5);
    Point p7(x - 3, y - height + 5);

    line(img, p1, p2, Scalar(255, 255, 255), 3);
    line(img, p1, p3, Scalar(255, 255, 255), 3);

    line(img, p2, p4, Scalar(255, 255, 255), 3);
    line(img, p2, p5, Scalar(255, 255, 255), 3);


    line(img, p3, p6, Scalar(255, 255, 255), 3);
    line(img, p3, p7, Scalar(255, 255, 255), 3);

    vector<int>::iterator pos;
    int i = 0;
    for (pos = a.begin(); pos < a.end(); pos++)
    {
        drawhistogram(img, x, y, *pos / n, i, dis, disx, Scalar(255, 255, 255));
        putTextZH(img, to_string(*pos).data(), Point((dis + 2 * dis * i + 2 * disx * i) / 2 + 20 + x - 18, y - *pos / n - 20), Scalar(255, 255, 255), 20, "Arial");
        i++;
    }
    if (a.size() > 1)
    {

        vector<int>::iterator pos;
        int i = 0;
        for (pos = a.begin(); pos < a.end() - 1; ++pos)
        {

            drawline(img, x, y, *pos / n, *(pos + 1) / n, i, dis, disx, col);
            i++;
        }
    }
}
void PutText::DrawTransRec(IplImage* img, int x, int y, int width, int height, CvScalar color, double alpha)//Background transparency
{
    IplImage * rec = cvCreateImage(cvSize(width, height), img->depth, img->nChannels);
    cvRectangle(rec, cvPoint(x, y), cvPoint(width, height), color, -1);
    cvSetImageROI(img, cvRect(x, y, width, height));
    cvAddWeighted(img, alpha, rec, 1 - alpha, 0.0, img);
    cvResetImageROI(img);

}

void PutText::draw_student_info(cv::Mat &img, std::vector<cv::Rect> rects, std::vector<std::string> names, std::vector<std::string> Emotions, std::vector<std::string> IsRaiseHands)
{
	for(int i=0; i<rects.size(); i++){
		rectangle(img, rects[i], Scalar{ 0 ,0,255 });
		vector<string> res;
		res.push_back(names[i]);
		res.push_back(Emotions[i]);
		res.push_back(IsRaiseHands[i]);
		drawid(img, res, rects[i].x, rects[i].y);
	}
}

