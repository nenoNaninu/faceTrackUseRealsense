#include<RealSense\SenseManager.h>
#include<opencv2\opencv.hpp>
#include<RealSense\Face\FaceModule.h>
#include<RealSense\Face\FaceData.h>
#include<RealSense\Face\FaceConfiguration.h>
#include<vector>

class RealsenseAPI
{
public:
	RealsenseAPI() {}
	~RealsenseAPI();
	bool initialize(int width, int height, int fps);
	bool initializeWebCam(int width, int height, int fps);
	void getColorImage(cv::Mat& inputMat);
	void getFaceData(std::vector<std::vector<Intel::RealSense::Face::FaceData::LandmarkPoint>>& facelandmarkVector);
	int getWidth();
	int getHeight();
	int getFPS();
	int getFaceNum();
	void drawGazepoints(cv::Mat& img,std::vector<cv::Point>& gazePoint);
	void drawFaceLandmarks(cv::Mat& img,std::vector<std::vector<Intel::RealSense::Face::FaceData::LandmarkPoint>>& facelandmarkVector);
	void getGazePoint(std::vector<cv::Point>& gazePointVector);
	void update();

private:

	int width, height, fps;
	int faceNum = 3;
	Intel::RealSense::SenseManager* senseManager = nullptr;
	Intel::RealSense::Face::FaceModule *fmod = nullptr;
	Intel::RealSense::Face::FaceData *fdata = nullptr;
	Intel::RealSense::Face::FaceConfiguration *fconfig = nullptr;
	Intel::RealSense::Face::FaceData::LandmarkPoint* landmarkPoints = nullptr;

};

