#ifndef MAIN_H
#define MAIN_H
#include "ui_mainwindow.h"
#include "canvas.h"
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

//using namespace std;
using namespace cv;

QT_BEGIN_NAMESPACE

//#define MIN(a,b) (((a)<(b))?(a):(b))
//#define MAX(a,b) (((a)>(b))?(a):(b))


class Window : public QMainWindow, Ui_MainWindow
{
    Q_OBJECT
public:
    Canvas *canvas;
    QSlider *slider;
    Window();
    void openFile(QString filename);
private slots:
    void openFile();
    void whitenSkin();
    void onHScroll(int pos);
private:
    cv::Mat image;
	cv::Mat rub_img;
	cv::Mat old_img;
	std::vector<cv::Rect> faces;

    void connectSignals();
    void face_detect(const cv::Mat &frame);
};

double wij(int i, int j, int x, int y, uchar gij, uchar gxy);
inline double Mask(int x, int y, int cx, int cy, int w, int h);
cv::Mat QImage2Mat(QImage &img);
QImage Mat2QImage(cv::Mat const& mat);

QT_END_NAMESPACE
#endif // MAIN_H