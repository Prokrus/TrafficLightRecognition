#include "detector.h"

Geely_ssg_detector::Geely_ssg_detector(string imagePath, struct svm_model* model)
{
	this->imgPath = imagePath;
	this->model = model;
}



vector<Rect> Geely_ssg_detector::detection()
{
	vector<Rect> allLights;

	DWORD tStart, tEnd;
	tStart = GetTickCount();

	/*! load test image*/
	Mat originalImage;
	originalImage = imread(imgPath, 1);
	if (originalImage.empty())
	{
		cout << "load image failed" << endl;
		return allLights;
	}
	//imshow("original image", originalImage);

	/*! generate gray Image*/
	Mat grayImage;
	cvtColor(originalImage, grayImage, COLOR_BGR2GRAY);

	/*! generate binary image*/
	Mat binaryImage;
	//threshold(grayImage, binaryImage, GRAYTHESHOLD, 255, CV_THRESH_BINARY);
	//imshow("binary image", binaryImage);
	//waitKey(0);

	
	//Mat binaryImage;
	int block_size = 11;
	double C = 0;
	cv::adaptiveThreshold(grayImage, binaryImage, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, block_size, C);
	imshow("adaptive threshold", binaryImage);
	//waitKey(0);
	

	/*! convert to HSV image*/
	Mat hsvImg;
	cvtColor(originalImage, hsvImg, COLOR_BGR2HSV);

	/*! Define color ranges*/
	const Scalar hsvRedLo(REDLOWH1, REDLOWS, LOWV);
	const Scalar hsvRedHi(REDHIGH1, 255, 255);

	const Scalar hsvRedLo2(REDLOWH2, REDLOWS, LOWV);
	const Scalar hsvRedHi2(REDHIGH2, 255, 255);

	const Scalar hsvYellowLo(YELLOWLOWH, YELLOWLOWS, LOWV);
	const Scalar hsvYellowHi(YELLOWHIGHH, 255, 255);

	const Scalar hsvGreenLo(GREENLOWH, GREENLOWS, LOWV);
	const Scalar hsvGreenHi(GREENHIGHH, 255, 255);

	/*! convert image to binary based on color*/
	Mat redImage,redImage2, greenImage, yellowImage, binImage, thresholdedImage;
	inRange(hsvImg, hsvRedLo, hsvRedHi, redImage);
	inRange(hsvImg, hsvRedLo2, hsvRedHi2, redImage2);
	inRange(hsvImg, hsvGreenLo, hsvGreenHi, greenImage);
	inRange(hsvImg, hsvYellowLo, hsvYellowHi, yellowImage);

	binImage = max(redImage, greenImage);
	binImage = max(binImage, redImage2);
	thresholdedImage = max(binImage, yellowImage);
	//imshow("threshold image", thresholdedImage);
	//waitKey(0);

	/*! dilate and erode*/ 
	Mat element = getStructuringElement(MORPH_RECT, Size(ERODEDILATESIZE, ERODEDILATESIZE));
	Mat dilateImage;
	dilate(thresholdedImage, dilateImage, element);

	Mat erodeImage;
	erode(dilateImage, erodeImage, element);

	//imshow("dilate image", dilateImage);

	/*! median filter*/
	//Mat medianImage;
	//medianBlur(thresholdedImage, medianImage, 3);
	//imshow("median filtered image", medianImage);
	//waitKey(0);

	/*1 generate preprocessed image*/
	Mat preprocessedImage;
	preprocessedImage = min(dilateImage, binaryImage);
	imshow("preprocessed image ", preprocessedImage);

	/*! find contours*/
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(preprocessedImage, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	/*! draw all contours*/
	Mat originalImageTemp2 = originalImage.clone();
	for (int i = 0; i < (int)contours.size(); i++)
	{
		drawContours(originalImageTemp2, contours, i, Scalar(255, 0, 0), 1, 8);
	}
	//imshow("all contours", originalImageTemp2);
	//waitKey(0);

	/*! select suitable rects*/
	vector<Rect> boundRect;
	for (int i = 0; i < contours.size(); i++)
	{

		Rect tempRect;
		tempRect = boundingRect(Mat(contours[i]));

		//vector<Point> poly;
		//double contourLength = arcLength(contours[i], true);
		//double epsilon =0.01 * contourLength;
		//approxPolyDP(Mat(contours[i]), poly, epsilon, true);
		//int corners = poly.size();

		if((tempRect.y+tempRect.height) <=(int)(originalImage.rows*REGIONRATIO))
		{
			if (tempRect.area() >= MINAREA & tempRect.area() <= MAXAREA)
			{
				if ((1.0 * (tempRect.width / tempRect.height) <= WHRATIO) &
					((1.0*tempRect.height / tempRect.width) <= WHRATIO))
				{
					//if(contourArea(contours[i])>= MINCONAREA &contourArea(contours[i]) <= MAXCONAREA)
					{
						if(contours[i].size()>= MINPOINTS && contours[i].size() <= MAXPOINTS)
						{
							//if (fabs(contourArea(contours[i])) >= AREARATIO * tempRect.area())
							{
								uchar binaryValue;
								int centerX = max(min(tempRect.x + tempRect.width / 2, originalImage.cols),1);
								int centerY = max(min(tempRect.y + tempRect.height / 2, originalImage.rows),1);
								binaryValue = preprocessedImage.at<uchar>(centerY, centerX);
								if(255 == binaryValue)
								{								
									double metric = 4 * 3.14 * fabs(contourArea(contours[i])) /(arcLength(contours[i], true)*arcLength(contours[i], true));
									if(metric>METRICTHRESHOLD)
									{ 
										boundRect.push_back(tempRect);
									}
								}
							}
						}
					}
				}
			}
		}

	}

	/*! show detected rects*/
	//Mat originalImageTemp1 = originalImage.clone();
	//for (int i = 0; i < boundRect.size(); i++)
	{
	//	rectangle(originalImageTemp1, boundRect[i], Scalar(255, 0, 0), 1, 8, 0);
	}
	//imshow("contour image", originalImageTemp1);
	//cv::waitKey(0);

	/*! extend boxes */
	int rectNum = boundRect.size();
	for (int boxIndex = 0; boxIndex < rectNum; boxIndex++)
	{
		Rect rectTemp;
		rectTemp.x = max(boundRect[boxIndex].x - (int)(boundRect[boxIndex].width * EXTENDL), 1);
		rectTemp.y = max(boundRect[boxIndex].y - (int)(boundRect[boxIndex].height * EXTENDT), 1);
		int x2 = min(boundRect[boxIndex].br().x + (int)(boundRect[boxIndex].width * EXTENDR), originalImage.cols);
		int y2 = min(boundRect[boxIndex].br().y + (int)(boundRect[boxIndex].height * EXTENDB), originalImage.rows);
		rectTemp.width = x2 - rectTemp.x;
		rectTemp.height = y2 - rectTemp.y;
		boundRect[boxIndex] = rectTemp;
	}


	/*! draw detected light*/
	Mat originalImageTemp4 = originalImage.clone();
	for (int i = 0; i < boundRect.size(); i++)
	{
		rectangle(originalImageTemp4, boundRect[i], Scalar(255, 0, 0), 1, 8, 0);
	}
	imshow("big box image", originalImageTemp4);
	//waitKey(0);

	/*! svm prediction */
	//vector<Rect> allLights;
	vector<double> confidences;
	for (int rectIndex = 0; rectIndex < rectNum; rectIndex++)
	{
		/*! Load and reshape image*/
		Mat fullImageTemp = originalImage.clone();
		Mat rectImage = fullImageTemp(boundRect[rectIndex]);
		cv::resize(rectImage, rectImage, cv::Size(BOXSIZE, BOXSIZE), 0, 0, cv::INTER_LINEAR);

		/*! generate hog feature*/
		HOGDescriptor* hog = new HOGDescriptor(Size(BOXSIZE, BOXSIZE), Size(BOXSIZE, BOXSIZE), Size(1, 1), Size(CELLSIZE, CELLSIZE), 31);
		vector<float> hogFeature;
		hog->compute(rectImage, hogFeature, Size(1, 1), Size(0, 0));

		/*! Generate svm_node for prediction*/
		struct svm_node *svmVec;
		int hogNum = hogFeature.size();
		svmVec = (struct svm_node *)malloc((hogNum + 1)*sizeof(struct svm_node));
		for (int hogIndex = 0; hogIndex < hogNum; hogIndex++)
		{
			svmVec[hogIndex].index = hogIndex + 1;
			svmVec[hogIndex].value = hogFeature[hogIndex];
		}
		svmVec[hogNum].index = -1;

		/*! Prediction processing*/
		double predictions;
		double prob_est[2];  // Probability estimation
		if (svm_check_probability_model(model))
		{
			predictions = svm_predict_probability(model, svmVec, prob_est);
			//cout << predictions << prob_est[0] << prob_est[1] << endl;
		}
		else
		{
			predictions = svm_predict(model, svmVec);
			//cout << predictions << endl;
		}

		if ((int)predictions == 1 && prob_est[0]>PROBTHRESHOLD)
		{
				//TrafficLight light;
			//light.locationXXYY[0] = boundRect[rectIndex].x;
			//light.locationXXYY[1] = boundRect[rectIndex].y;
			//light.locationXXYY[2] = boundRect[rectIndex].x + boundRect[rectIndex].width;
			//light.locationXXYY[3] = boundRect[rectIndex].y + boundRect[rectIndex].height;
			//condidatLight lightTemp;
			//lightTemp.rect = boundRect[rectIndex];
			allLights.push_back(boundRect[rectIndex]);
			confidences.push_back(prob_est[0]);
			//lights.push_back(light);
		}
	}

	/*! find the only one light on imagev */
	Rect determined_rect;
	if (!confidences.empty())

	{

		vector<double>::iterator maximumIt = max_element(confidences.begin(), confidences.end());

		int maximum_index = std::distance(confidences.begin(), maximumIt);

		determined_rect = allLights[maximum_index];

		Mat determined_image = originalImage.clone();

		rectangle(determined_image, determined_rect, Scalar(255, 0, 0), 1, 8, 0);

		// imshow("determined light",determined_image);
		 //waitKey(0);
	}


	/*! draw the detected box */
	
	Mat originalImageTemp3 = originalImage.clone();
	for (int i = 0; i < allLights.size(); i++)
	{
		rectangle(originalImageTemp3, allLights[i], Scalar(255, 0, 0), 1, 8, 0);
	}
	imshow("locations image", originalImageTemp3);

	tEnd = GetTickCount();
	//cout << tEnd - tStart << endl;
	time_temp = tEnd - tStart;
	waitKey(0);
	
	return boundRect;

}

