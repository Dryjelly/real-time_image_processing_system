#ifdef OPENCV

#include "stdio.h"
#include "stdlib.h"
#include "opencv2/opencv.hpp"
#include "image.h"

using namespace cv;

extern "C" {

IplImage *image_to_ipl(image im)
{
    int x,y,c;
    IplImage *disp = cvCreateImage(cvSize(im.w,im.h), IPL_DEPTH_8U, im.c);
    int step = disp->widthStep;
    for(y = 0; y < im.h; ++y){
        for(x = 0; x < im.w; ++x){
            for(c= 0; c < im.c; ++c){
                float val = im.data[c*im.h*im.w + y*im.w + x];
                disp->imageData[y*step + x*im.c + c] = (unsigned char)(val*255);
            }
        }
    }
    return disp;
}

image ipl_to_image(IplImage* src)
{
    int h = src->height;
    int w = src->width;
    int c = src->nChannels;
    image im = make_image(w, h, c);
    unsigned char *data = (unsigned char *)src->imageData;
    int step = src->widthStep;
    int i, j, k;

    for(i = 0; i < h; ++i){
        for(k= 0; k < c; ++k){
            for(j = 0; j < w; ++j){
                im.data[k*w*h + i*w + j] = data[i*step + j*c + k]/255.;
            }
        }
    }
    return im;
}

Mat image_to_mat(image im)
{
    image copy = copy_image(im);
    constrain_image(copy);
    if(im.c == 3) rgbgr_image(copy);

    IplImage *ipl = image_to_ipl(copy);
    Mat m = cvarrToMat(ipl, true);
    cvReleaseImage(&ipl);
    free_image(copy);
    return m;
}

/*
mean shift 추가함
*/

// 아래 줄을 주석처리하면 비디오 영상에 대해 동작
//#define WEBCAM
/*
bool mouse_is_pressing = false;
int start_x, start_y, end_x, end_y;
int step = 0;
Mat img_color;
Rect roi;


void swap(int* v1, int* v2) {
	int temp = *v1;
	*v1 = *v2;
	*v2 = temp;
}


void mouse_callback(int event, int x, int y, int flags, void* userdata)
{
	Mat img_result = img_color.clone();


	if (event == EVENT_LBUTTONDOWN) {
		step = 1;

		mouse_is_pressing = true;
		start_x = x;
		start_y = y;



	}
	else if (event == EVENT_MOUSEMOVE) {

		if (mouse_is_pressing) {

			end_x = x;
			end_y = y;

			step = 2;
		}

	}
	else if (event == EVENT_LBUTTONUP) {

		mouse_is_pressing = false;

		end_x = x;
		end_y = y;

		step = 3;
	}
}


int main()
{
	Mat img_hsv, img_mask, img_ROI;
	Mat objectHistogram;


	int channels[] = { 0 };
	int hsize[] = { 64};
	float range1[] = { 0, 180 };
	const float* histRange[] = { range1 };


#ifdef WEBCAM
	VideoCapture cap(0);
#else
	VideoCapture cap("note.mp4");
#endif

	if (!cap.isOpened()) {
		cerr << "video 에러 - 카메라 또는 영상을 열 수 없습니다.\n";
		return -1;
	}

	namedWindow("Color", 1);
	setMouseCallback("Color", mouse_callback);

#ifndef WEBCAM
	Mat img_sceen;
	cap.read(img_sceen);
#endif


	while (1)
	{

#ifdef WEBCAM
		cap.read(img_color);
#else
		if (step == 4)
			cap.read(img_color);
		else
			img_sceen.copyTo(img_color);
#endif

		if (img_color.empty()) {
			cerr << "빈 영상이 캡쳐되었습니다.\n";
			break;
		}

		switch (step)
		{

		case 1:
			circle(img_color, Point(start_x, start_y), 10, Scalar(0, 255, 0), -1);

			break;

		case 2:
			rectangle(img_color, Point(start_x, start_y), Point(end_x, end_y), Scalar(0, 255, 0), 3);

			break;

		case 3:

			if (start_x > end_x) {
				swap(&start_x, &end_x);
				swap(&start_y, &end_y);
			}

			roi = Rect(start_x, start_y, end_x - start_x, end_y - start_y);
			cvtColor(img_color, img_hsv, COLOR_BGR2HSV);
			img_ROI = Mat(img_hsv, roi);

			inRange(img_ROI, Scalar(0., 60., 60.), Scalar(180., 255., 255.), img_mask);

			imshow("ROI", img_ROI);

			calcHist(&img_ROI, 1, channels, img_mask, objectHistogram, 1, hsize, histRange);
			normalize(objectHistogram, objectHistogram, 0, 255, NORM_MINMAX);

			step++;

			break;

		case 4:

			Mat bp;
			cvtColor(img_color, img_hsv, COLOR_BGR2HSV);
			calcBackProject(&img_hsv, 1, channels, objectHistogram, bp, histRange);

			// Tracking
			meanShift(bp, roi, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

			rectangle(img_color, roi, Scalar(0, 0, 255), 2);
			break;

		}


		//if (step < 4)
		cout << step << endl;

		imshow("Color", img_color);


		if (waitKey(25) >= 0)
			break;
	}


	return 0;
}
*/
/*
mean shift 추가함
*/

box mean_shift(image im_prev, image im, box b)
{
    Mat img_prev, img, img_hsv_1, img_hsv_2, img_mask, img_ROI;
	Mat objectHistogram;

	int channels[] = { 0 };
	int hsize[] = { 64};
	float range1[] = { 0, 180 };
	const float* histRange[] = { range1 };
	Rect roi;
    box output;

	img_prev = image_to_mat(im_prev);
	img = image_to_mat(im);

	roi = Rect(b.x, b.y, b.w, b.h);
	cvtColor(img_prev, img_hsv_1, COLOR_BGR2HSV);
	img_ROI = Mat(img_hsv_1, roi);

	inRange(img_ROI, Scalar(0., 60., 60.), Scalar(180., 255., 255.), img_mask);

    //imshow("ROI", img_ROI);

	calcHist(&img_ROI, 1, channels, img_mask, objectHistogram, 1, hsize, histRange);
	normalize(objectHistogram, objectHistogram, 0, 255, NORM_MINMAX);

	Mat bp;
	cvtColor(img, img_hsv_2, COLOR_BGR2HSV);
	calcBackProject(&img_hsv_2, 1, channels, objectHistogram, bp, histRange);

	// Tracking
	meanShift(bp, roi, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

	output.x = roi.x;
	output.y = roi.y;
	output.w = roi.width;
	output.h = roi.height;

	return output;
}

box mean_shift_v2(image im, image hist, box b)
{
    Mat img, img_hsv, img_mask, img_ROI;
	Mat objectHistogram;

	int channels[] = { 0 };
	int hsize[] = { 64};
	float range1[] = { 0, 180 };
	const float* histRange[] = { range1 };
	Rect roi;
    box output;

    roi = Rect(b.x, b.y, b.w, b.h);

	img = image_to_mat(im);
	objectHistogram = image_to_mat(hist);

	Mat bp;
	cvtColor(img, img_hsv, COLOR_BGR2HSV);
	calcBackProject(&img_hsv, 1, channels, objectHistogram, bp, histRange);

	// Tracking
	meanShift(bp, roi, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

	output.x = roi.x;
	output.y = roi.y;
	output.w = roi.width;
	output.h = roi.height;
	printf("%f %f %f %f\n", b.x, b.y, b.w, b.h);
    printf("%lf,%lf,%lf,%lf\n",output.x,output.y,output.w,output.h);
	return output;
}

image mat_to_image(Mat m)
{
    IplImage ipl = m;
    image im = ipl_to_image(&ipl);
    rgbgr_image(im);
    return im;
}

image CalHistogram (image im, box b)
{
    Mat img, img_hsv, img_mask, img_ROI;
	Mat objectHistogram;

	int channels[] = { 0 };
	int hsize[] = { 64};
	float range1[] = { 0, 180 };
	const float* histRange[] = { range1 };
	Rect roi;
    image output;

	img = image_to_mat(im);

	roi = Rect(b.x, b.y, b.w, b.h);
	cvtColor(img, img_hsv, COLOR_BGR2HSV);
	img_ROI = Mat(img_hsv, roi);

	inRange(img_ROI, Scalar(0., 60., 60.), Scalar(180., 255., 255.), img_mask);

	calcHist(&img_ROI, 1, channels, img_mask, objectHistogram, 1, hsize, histRange);
	normalize(objectHistogram, objectHistogram, 0, 255, NORM_MINMAX);

	output = mat_to_image(objectHistogram);

	return output;
}

void *open_video_stream(const char *f, int c, int w, int h, int fps)
{
    VideoCapture *cap;
    if(f) cap = new VideoCapture(f);
    else cap = new VideoCapture(c);
    if(!cap->isOpened()) return 0;
    if(w) cap->set(CV_CAP_PROP_FRAME_WIDTH, w);
    if(h) cap->set(CV_CAP_PROP_FRAME_HEIGHT, w);
    if(fps) cap->set(CV_CAP_PROP_FPS, w);
    return (void *) cap;
}

image get_image_from_stream(void *p)
{
    VideoCapture *cap = (VideoCapture *)p;
    Mat m;
    *cap >> m;
    if(m.empty()) return make_empty_image(0,0,0);
    return mat_to_image(m);
}

image load_image_cv(char *filename, int channels)
{
    int flag = -1;
    if (channels == 0) flag = -1;
    else if (channels == 1) flag = 0;
    else if (channels == 3) flag = 1;
    else {
        fprintf(stderr, "OpenCV can't force load with %d channels\n", channels);
    }
    Mat m;
    m = imread(filename, flag);
    if(!m.data){
        fprintf(stderr, "Cannot load image \"%s\"\n", filename);
        char buff[256];
        sprintf(buff, "echo %s >> bad.list", filename);
        system(buff);
        return make_image(10,10,3);
        //exit(0);
    }
    image im = mat_to_image(m);
    return im;
}

int show_image_cv(image im, const char* name, int ms)
{
    Mat m = image_to_mat(im);
    imshow(name, m);
    int c = waitKey(ms);
    if (c != -1) c = c%256;
    return c;
}

void make_window(char *name, int w, int h, int fullscreen)
{
    namedWindow(name, WINDOW_NORMAL); 
    if (fullscreen) {
        setWindowProperty(name, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    } else {
        resizeWindow(name, w, h);
        if(strcmp(name, "Demo") == 0) moveWindow(name, 0, 0);
    }
}

}

#endif
