#include "main.h"
#include <QApplication>
#include <QFileDialog>
//#include <cmath>
//#include <QPainter>
#include <QDebug>
//#include <chrono>

Window:: Window()
{
    setupUi(this);
    QHBoxLayout *layout = new QHBoxLayout(scrollAreaWidgetContents);
    layout->setContentsMargins(0, 0, 0, 0);
    canvas = new Canvas(this);
    layout->addWidget(canvas);
    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0,100);
    statusbar->addPermanentWidget(slider);
    connectSignals();
}

void
Window:: connectSignals()
{
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(actionWhiten_Skin, SIGNAL(triggered()), this, SLOT(whitenSkin()));
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(onHScroll(int)));
}

void
Window:: openFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Image", "",
                "Image files (*.jpg *.png );;All files (*.*)" );
    if (!filename.isEmpty())
        openFile(filename);
}

void
Window:: openFile(QString filename)
{
    QImage new_image(filename);
    if (new_image.isNull()) return;

    image = QImage2Mat(new_image);
    old_img = image.clone();
    rub_img = image.clone();
    canvas->setImage(new_image);
}

void
Window:: whitenSkin()
{
    if (canvas->pixmap()->isNull()) return;
	face_detect(image);
	int window_size = 11;
	int height = image.size().height;
	int width = image.size().width;
    qDebug() << "Total faces =" << faces.size();
	for (int i = 0; i < (int)faces.size(); i++)
	{
		int sx = faces[i].x;
		int sy = faces[i].y;
		int ex = faces[i].x + faces[i].width;
		int ey = faces[i].y + faces[i].height;
		if (image.channels() == 3)
		{
			for (int row = sy; row < ey; row++)
			{
				uchar *data = rub_img.ptr<uchar>(row);
				for (int col = sx; col < ex; col++)
				{
					double r = 0, g = 0, b = 0;
					double wr = 0, wg = 0, wb = 0;
					for (int i = row - window_size / 2; i <= row + window_size / 2; i++)
					for (int j = col - window_size / 2; j <= col + window_size / 2; j++)
					{
						if (i >= 0 && i < height &&
							j >= 0 && j < width)
						{
							uchar *old_data = old_img.ptr<uchar>(i);
							double tb = wij(i, j, row, col, old_data[j * 3], data[col * 3]);
							double tg = wij(i, j, row, col, old_data[j * 3 + 1], data[col * 3 + 1]);
							double tr = wij(i, j, row, col, old_data[j * 3 + 2], data[col * 3 + 2]);
							b += old_data[j * 3] * tb;
							g += old_data[j * 3 + 1] * tg;
							r += old_data[j * 3 + 2] * tr;
							wb += tb;
							wg += tg;
							wr += tr;
						}
					}
					data[col * 3] = b / wb;
					data[col * 3 + 1] = g / wg;
					data[col * 3 + 2] = r / wr;
				}
			}
		}
    }
}

void
Window:: onHScroll(int pos)
{
	double rub_value = pos / 100.0;
	double white_value = rub_value;
	//int height = image.size().height;
	//int width = image.size().width;

	// Apply a Bilateral Filter for Skin Blur
	for (int i = 0; i < (int)faces.size(); i++)
	{
		int sx = faces[i].x;
		int sy = faces[i].y;
		int ex = faces[i].x + faces[i].width;
		int ey = faces[i].y + faces[i].height;

		//int size = faces[i].width * faces[i].height;

		for (int row = sy; row < ey; row++)
		{
			uchar *old_data = old_img.ptr<uchar>(row);
			uchar *data = image.ptr<uchar>(row);
			uchar *rub_data = rub_img.ptr<uchar>(row);

			for (int col = sx; col < ex; col++)
			{
				double ratio = Mask(col, row, (sx + ex) / 2, (sy + ey) / 2,
					ex - sx, ey - sy);
				double rv = rub_value * ratio;

				if (image.channels() == 3)
				{
					data[col * 3] = (1 - rv) * old_data[col * 3]
						+ rv * rub_data[col * 3];
					data[col * 3 + 1] = (1 - rv) * old_data[col * 3 + 1]
						+ rv * rub_data[col * 3 + 1];
					data[col * 3 + 2] = (1 - rv) * old_data[col * 3 + 2]
						+ rv * rub_data[col * 3 + 2];
				}
				else
				{
					data[col] = (1 - rv) * old_data[col]
						+ rv * rub_data[col];
				}
			}
		}
        // Whiten Skin by increasing saturation/luminance
		cv::Mat hsv_white_img = image.clone();
		if (image.channels() == 3)
			cv::cvtColor(image, hsv_white_img, CV_BGR2HSV);
		for (int row = sy; row < ey; row++)
		{
			uchar *data = hsv_white_img.ptr<uchar>(row);
			for (int col = sx; col < ex; col++)
			{
				double ratio = Mask(col, row, (sx + ex) / 2, (sy + ey) / 2,
					ex - sx, ey - sy);
				double wv = white_value * ratio;
				double value = 1 - 0.1 * wv;
				double li = 1 + 0.1 * wv;

				if (image.channels() == 3)
				{
					data[col * 3 + 1] = data[col * 3 + 1] * value;
					double t = data[col * 3 + 2] * li;
					data[col * 3 + 2] = t > 255 ? 255 : t;
				}
				else
				{
					double t = data[col] * li;
					data[col] = t > 255 ? 255 : t;
				}
			}
		}
		if (image.channels() == 3)
			cv::cvtColor(hsv_white_img, image, CV_HSV2BGR);
	}
	canvas->setImage(Mat2QImage(image));
}

void
Window:: face_detect(const cv::Mat &frame)
{
	cv::CascadeClassifier face_cascade;
	if (!face_cascade.load("haarcascade_frontalface_alt.xml")) return;

	if (frame.cols > 0 && frame.rows > 0)
	{
		cv::Mat frame_gray;
		if (frame.channels() == 3)
			cvtColor(frame, frame_gray, CV_BGR2GRAY);
		else
			frame_gray = frame.clone();
		equalizeHist(frame_gray, frame_gray);
		face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
	}
}

double wij(int i, int j, int x, int y, uchar gij, uchar gxy)
{
	double tds = 400;
	double tdr = 500;
	int ix = (i - x)*(i - x);
	int jy = (j - y)*(j - y);
	int gg = (gij - gxy)*(gij - gxy);
	double w = exp(-(ix + jy)/tds - gg/tdr);
	return w;
}

inline double Mask(int x, int y, int cx, int cy, int w, int h)
{
	double xt = x - cx;
	double yt = y - cy;
	yt = yt * w / (double)h;
	double r = (xt*xt + yt*yt) / (w*w/4);
	if (r <= 0.7)
		return 1;
	else if (r <= 1)
		return 1 - 10 * (r - 0.7)/3;
	else
		return 0;
}

cv::Mat QImage2Mat(QImage &img)
{
    if (img.format()!=QImage::Format_RGB888)
        img = img.convertToFormat(QImage::Format_RGB888);

    cv::Mat tmp(img.height(), img.width(), CV_8UC3, (uchar*)img.bits(), img.bytesPerLine());
    cv::Mat mat;
    cvtColor(tmp, mat ,CV_BGR2RGB);
    return mat;
}

QImage Mat2QImage(cv::Mat const& mat)
{
    QImage img((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    return img.rgbSwapped();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Window *win = new Window();
    win->resize(640, 600);
    win->openFile("/home/subha/Arindam.jpg");
    win->show();
    return app.exec();
}
