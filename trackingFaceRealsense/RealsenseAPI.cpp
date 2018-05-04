#include"RealsenseAPI.h"
#include<RealSense\Session.h>
#include<RealSense\SampleReader.h>
#include<opencv2\opencv.hpp>

using namespace Intel::RealSense;
using namespace Intel::RealSense::Face;
using namespace cv;
using std::vector;

bool RealsenseAPI::initialize(int width, int height, int fps)
{
	this->width = width;
	this->height = height;
	this->fps = fps;
	this->senseManager = SenseManager::CreateInstance();
	if (senseManager == nullptr) {
		return false;
	}
	this->senseManager->EnableStream(Capture::STREAM_TYPE_COLOR, width, height, fps);
	senseManager->EnableFace(); // FaceModule機能を有効化
	this->senseManager->Init();
	Capture::Device* device = this->senseManager->QueryCaptureManager()->QueryDevice();
	if (device == nullptr) {
		return false;
	}
	device->ResetProperties(Capture::STREAM_TYPE_ANY);
	device->SetMirrorMode(Capture::Device::MirrorMode::MIRROR_MODE_HORIZONTAL);

	fmod = senseManager->QueryFace(); // FaceModuleの取得
	fdata = fmod->CreateOutput(); // FaceModuleの出力先の取得

	fconfig = fmod->CreateActiveConfiguration(); // FaceModuleの設定
	fconfig->detection.isEnabled = true; // 顔検出を有効化
	fconfig->detection.maxTrackedFaces = faceNum; // 最大の顔追跡数 // TODO: 1?
	fconfig->landmarks.isEnabled = true; // 顔特徴点の検出を有効化
	landmarkPoints = new FaceData::LandmarkPoint[fconfig->landmarks.numLandmarks]; // 顔特徴点を入れる配列を用意
	GazeConfiguration* gazeConfig = fconfig->QueryGaze();
	gazeConfig->isEnabled = true;

	fconfig->ApplyChanges(); // 設定を適用
	return true;

}

bool RealsenseAPI::initializeWebCam(int width, int height, int fps)
{
	this->width = width;
	this->height = height;
	this->fps = fps;
	this->senseManager = SenseManager::CreateInstance();
	if (senseManager == nullptr)
	{
		return false;
	}
	this->senseManager->EnableStream(Capture::STREAM_TYPE_COLOR, width, height, fps);
}

RealsenseAPI::~RealsenseAPI()
{
	delete landmarkPoints;
	if (this->senseManager != nullptr)
	{
		this->senseManager->Release();
		this->senseManager = nullptr;
	}
}

int RealsenseAPI::getFPS()
{
	return this->fps;
}

int RealsenseAPI::getHeight()
{
	return this->height;
}

int RealsenseAPI::getWidth()
{
	return this->width;
}

int RealsenseAPI::getFaceNum() {
	return this->faceNum;
}

void RealsenseAPI::getColorImage(cv::Mat& inputMat)
{
	if (inputMat.rows != this->height || inputMat.cols != this->width || inputMat.type() != CV_8UC3)
	{
		inputMat = cv::Mat(this->height, this->width, CV_8UC3);
	}


	Status status = senseManager->AcquireFrame(false);
	if (status < Status::STATUS_NO_ERROR)
	{
		std::cout << "err" << std::endl;;
		return;
	}

	const Capture::Sample *sample = senseManager->QuerySample();

	if (sample->color)
	{
		Image::ImageData data = {}; //={}構造体の初期化方法;
		Image::Rotation rotation = sample->color->QueryRotation();
		status = sample->color->AcquireAccess(Image::ACCESS_READ, Image::PIXEL_FORMAT_RGB24, rotation, Image::OPTION_ANY, &data);
		if (status >= Status::STATUS_NO_ERROR)
		{
			memcpy(inputMat.data, data.planes[0], data.pitches[0] * this->height);
			sample->color->ReleaseAccess(&data);
			senseManager->ReleaseFrame();
		}
	}

}

void RealsenseAPI::update() {
	this->fdata->Update();
}

void RealsenseAPI::getFaceData(vector<vector<FaceData::LandmarkPoint>>& facelandmarkVector) {
	fdata->Update();
	facelandmarkVector.clear();
	for (int i = 0, n = fdata->QueryNumberOfDetectedFaces(); i < n; i++) {
		FaceData::Face *face = fdata->QueryFaceByIndex(i);
		if (face != nullptr) {
			//Intel::RealSense::RectI32* rectI32 = ;
			//face->QueryDetection()->QueryBoundingRect(rectI32);

			FaceData::LandmarksData *lmdata = face->QueryLandmarks(); // RealSenseの特徴点データを取得

			if (lmdata != nullptr) {
				lmdata->QueryPoints(landmarkPoints);

				if (20 < landmarkPoints->confidenceImage) {
					vector<FaceData::LandmarkPoint> tmpLandmarkVec(&landmarkPoints[0], &landmarkPoints[fconfig->landmarks.numLandmarks]);
					facelandmarkVector.push_back(tmpLandmarkVec);
				}
			}
		}
	}
}

void RealsenseAPI::getGazePoint(vector<Point>& gazePointVector) {
	gazePointVector.clear();
	for (int i = 0, n = fdata->QueryNumberOfDetectedFaces(); i < n; i++) {
		FaceData::Face* face = fdata->QueryFaceByIndex(i);
		if (face != nullptr) {
			FaceData::GazeData* gazeData = face->QueryGaze();
			if (gazeData != nullptr) {
				GazePoint gazePoint = gazeData->QueryGazePoint();
				int x = gazePoint.screenPoint.x;
				int y = gazePoint.screenPoint.y;
				Point p(x, y);
				gazePointVector.push_back(p);
			}

		}
	}
}


void RealsenseAPI::drawFaceLandmarks(Mat& img, vector<vector<FaceData::LandmarkPoint>>& facelandmarkVector) {
	for (int i = 0, n = facelandmarkVector.size(); i < n; i++) {
		for (int j = 0, m = facelandmarkVector[i].size(); j < m; j++) {
			auto alias = facelandmarkVector[i][j].source.alias;
			int x = facelandmarkVector[i][j].image.x;
			int y = facelandmarkVector[i][j].image.y;
			cv::circle(img, Point(x, y), 5, Scalar(255, 0, 0), -1);
		}
	}
}


void RealsenseAPI::drawGazepoints(cv::Mat& img, std::vector<cv::Point>& gazePoint) {
	for (int i = 0, n = gazePoint.size(); i < n; i++) {
		cv::circle(img, gazePoint[i], 5, Scalar(0, 0, 255), -1);
	}
}