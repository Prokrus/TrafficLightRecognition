/*
* 55.cpp
*
*  Created on: 2018年7月18日
*      Author: tielang2253
*/

#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>
#include <string>
#include <time.h>
//#include <unistd.h>
//#include "RingBuffer.h"
//#include "libsvmClassifier.h"
#include "detector.h"
#include <fstream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <cstdio>

using namespace std;
using namespace cv;
//using namespace Geely_ssg_cv;



/*****主函数*****/


int main(void *ptr)
{




	string modelPath = "D:\\01_data\\models\\localication_models\\detection_model_201816.model";

	/*! Load detection model*/
	const char* modelPath_ = modelPath.c_str();
	struct svm_model *SVMModel;
	if ((SVMModel = svm_load_model(modelPath_)) == 0)
	{
		cout << "Can't load SVM model" << endl;
		return 0;
	}
		
	/*! generate imagepath*/
	vector<string> fileNames;
	string imagesDir = "D:\\01_data\\images\\2018.08.30am_only_lights\\";
	string pCmd = "dir /B " + imagesDir;
	FILE* pipe = _popen(pCmd.c_str(), "rt");
	char buf[256];
	while (!feof(pipe))
	{
		if (fgets(buf, 256, pipe) != NULL)
		{
			fileNames.push_back(string(buf));
			fileNames.back().resize(fileNames.back().size() - 1);
		}
	}
	_pclose(pipe);
		
	/*! process every image*/
	vector<vector<Rect>> lights;
	int imagesNum = fileNames.size();
	int time_sum = 0;
	vector<int> lights_num;
	for (int imagesIndex = 0; imagesIndex < imagesNum; imagesIndex++)
	{ 
		//if(imagesIndex >700)
		{ 
		string imagePath = imagesDir + fileNames[imagesIndex];
		cout << fileNames[imagesIndex] << endl;
		//string imagePath = "25846.png";
		Geely_ssg_detector BM(imagePath, SVMModel);
		vector<Rect>  rect;
		rect = BM.detection();
		lights.push_back(rect);
		time_sum += BM.time_temp;
		lights_num.push_back(rect.size());
		}
	}
	cout << "average processing time is: " << time_sum / imagesNum << endl;


	/*! save number of lights in each image in txt file*/
	
	//ofstream detected_num("detected_lights_num.txt");
	//if (detected_num.is_open())
	//{
	//	vector<int>::iterator numIt = lights_num.begin();
	//	for (numIt; numIt != lights_num.end(); ++numIt)
	//	{
	//		detected_num << (*numIt) << " ";
	//	}
	//}
	//detected_num.close();
	//cout << "number  of detected lights in each image has been saved." << endl;

	/*! save rects in txt file*/
	/*
	ofstream detectedLights("preprocessedLights.txt");
	if(detectedLights.is_open())
	{ 
		vector<vector<Rect>>::iterator imageIt = lights.begin();
		for (imageIt; imageIt != lights.end(); ++imageIt)
		{
			vector<Rect>::iterator lightIt = (*imageIt).begin();
			for (lightIt; lightIt != (*imageIt).end(); ++lightIt)
			{
				detectedLights << (*lightIt).x << " ";
				detectedLights << (*lightIt).y << " ";
				detectedLights << (*lightIt).x+ (*lightIt).width << " ";
				detectedLights << (*lightIt).y + (*lightIt).height << " ";
			}
			detectedLights << 0;
			detectedLights << "\n";
		}
		detectedLights.close();
	}
	cout << "rects have been saved" << endl;
	*/
	return 0;
}



