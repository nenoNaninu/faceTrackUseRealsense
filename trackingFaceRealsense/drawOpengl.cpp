#include"lib.h"
#include"myUtils.h"
#include<Windows.h>
#include<opencv2\opencv.hpp>
#include"RealsenseAPI.h"
#include<GL\freeglut.h>
#include<RealSense\Face\FaceModule.h>
#include<RealSense\Face\FaceData.h>
#include<RealSense\Face\FaceConfiguration.h>


RealsenseAPI* realsenseAPI = nullptr;

void display();

void glShow(cv::Mat& img);

void keyboard(unsigned char key, int x, int y);

void idle();

void idle2();

void faceTracking();


int main(int argc, char *argv[])
{
	int width = 640;
	int height = 480;
	int fps = 60;

	realsenseAPI = new RealsenseAPI();
	bool check = realsenseAPI->initialize(width, height, fps);

	if (!check) {
		std::cout << "error" << std::endl;
		std::getchar();
		return -1;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("RealsenseImg");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, width, height, 0.0);
	glViewport(0, 0, width, height);

	glutDisplayFunc(faceTracking);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(faceTracking);

	glutMainLoop();
	
	delete realsenseAPI;
	return 0;
}


void display()
{
	cv::Mat img;
	realsenseAPI -> getColorImage(img);
	cv::flip(img, img, 0);
	cv::cvtColor(img,img, CV_BGR2RGB);

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(img.cols, img.rows,
		GL_RGB, GL_UNSIGNED_BYTE, img.data);
	glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
	case 'Q':
	case '\033':
		glutLeaveMainLoop();
	default:
		break;
	}
}

void idle()
{
	cv::Mat mat = cv::imread("test.jpg");
	//realsenseAPI->getColorImage(mat);
	cv::flip(mat, mat, 0);
	cv::cvtColor(mat, mat, CV_BGR2RGB);

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(mat.cols, mat.rows,
		GL_RGB, GL_UNSIGNED_BYTE, mat.data);
	glFlush();
	printFPS();
}

void idle2()
{
	static cv::Mat mat = cv::imread("test.jpg");
	static bool first = true;
	if (first)
	{
		first = false;
		cv::resize(mat, mat, cv::Size(realsenseAPI->getWidth(), realsenseAPI->getHeight()));
		//realsenseAPI->getColorImage(mat);
		cv::flip(mat, mat, 0);
		cv::cvtColor(mat, mat, CV_BGR2RGB);
	}

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(mat.cols, mat.rows,
		GL_RGB, GL_UNSIGNED_BYTE, mat.data);
	glFlush();
	printFPS();
}



void faceTracking() {
	if (realsenseAPI == nullptr) {
		std::cout << "error" << std::endl;
		return;
	}

	cv::Mat img;
	std::vector<std::vector<Intel::RealSense::Face::FaceData::LandmarkPoint>> facelandmarkVector;
	std::vector<cv::Point> gazePointVector;
	realsenseAPI->update();
	realsenseAPI->getColorImage(img);
	realsenseAPI->getFaceData(facelandmarkVector);
	realsenseAPI->getGazePoint(gazePointVector);
	realsenseAPI->drawFaceLandmarks(img,facelandmarkVector);
	realsenseAPI->drawGazepoints(img, gazePointVector);
	glShow(img);
	printFPS();
}


void glShow(cv::Mat& img) {
	cv::flip(img, img, 0);
	cv::cvtColor(img, img, CV_BGR2RGB);

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(img.cols, img.rows,
		GL_RGB, GL_UNSIGNED_BYTE, img.data);
	glFlush();
}