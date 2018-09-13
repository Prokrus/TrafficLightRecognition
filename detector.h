//#include "stdafx.h"
#ifndef LIBSVMCLASSIFIER_H
#define LIBSVMCLASSIFIER_H
#include "svm.h"
//#include "svm.cpp"
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/ml/ml.hpp"
#include <iostream>
#include <time.h>
#include <windows.h>
#undef max
#undef min

#define BOXSIZE 32
#define CELLSIZE 4
#define	PROBTHRESHOLD 0.9


#define MINAREA  2*2
#define MAXAREA  60 * 60
#define WHRATIO  2.0
#define MINPOINTS  4
#define MAXPOINTS  200
#define AREARATIO 0.3
#define GRAYTHESHOLD 128
#define ERODEDILATESIZE 2
#define REDLOWH1 0
#define REDHIGH1 9
#define REDLOWH2 167
#define REDHIGH2 180
#define REDLOWS 76
#define LOWV 46
#define YELLOWLOWH 5
#define YELLOWHIGHH 27
#define YELLOWLOWS 76
#define GREENLOWH 35
#define GREENHIGHH 77
#define GREENLOWS 38
#define METRICTHRESHOLD 0.5
#define EXTENDL 1.5
#define EXTENDR 1.5
#define EXTENDT 1.5
#define EXTENDB 1.5
#define REGIONRATIO 2/3
using namespace std;
using namespace cv;

struct condidatLight
{
	Rect rect;
	double confidence;
};

class Geely_ssg_detector
{
public:
	//double* predic;
	Geely_ssg_detector(string imgPath, struct svm_model* model);
//	Mat loadImage(string imgPath);
	vector<Rect> detection();
	//Rect determineLight(vector<condidatLight>);
	int time_temp;
private:
	//vector<double> distances;
	string imgPath;
	struct svm_model* model;
};
#endif