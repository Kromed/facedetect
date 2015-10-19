#include <iostream>
#include <windows.h>
#include <strsafe.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/cudaobjdetect.hpp"
#include "opencv2/cudawarping.hpp"

using namespace std;
using namespace cv;
using namespace cv::cuda;

Mat image;
string CascadeName = "haarcascade_frontalface_alt2.xml";
cv::CascadeClassifier cascade_cpu;
Ptr<cuda::CascadeClassifier> cascade_gpu;
bool findLargestObject = false;  //parameter of cascade classifier if true - just one(largest) face; false -multifaces
bool filterRects = true;		 //parameter of cascade classifier.if false - very many concentic rects

bool useGPU = false;
bool writeImage = false;		
bool showImage = false;

int max_width  = 0.8*GetSystemMetrics(SM_CXSCREEN);	// width limit of displaying image - 80% by screen resolation
int max_height = 0.8*GetSystemMetrics(SM_CYSCREEN);	//height limit of displaying image - 80% by screen resolation


void detect_faces(cv::Mat& image, TCHAR ImageName[MAX_PATH]){
	Mat gray, resz_image;
	vector<Rect> faces;

	if (image.channels() == 3){ cv::cvtColor(image, gray, CV_BGR2GRAY); }//convert image to gray
	else{ image.copyTo(gray); }
	equalizeHist(gray, gray);		//equalise by histogram - recommended for best result

	double timer = (double)cvGetTickCount();
	if(useGPU){ 
		GpuMat gray_gpu(gray);
		GpuMat facesBuf_gpu;
		cascade_gpu->setFindLargestObject(findLargestObject);
		cascade_gpu->setScaleFactor(1.2);
		cascade_gpu->setMinNeighbors((filterRects || findLargestObject) ? 4 : 0);
		cascade_gpu->detectMultiScale(gray_gpu, facesBuf_gpu);
		cascade_gpu->convert(facesBuf_gpu, faces);
		gray_gpu.release();
		facesBuf_gpu.release();
	}
	else { 
		cascade_cpu.detectMultiScale(gray, faces, 1.2, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
	}
	timer = (double)cvGetTickCount() - timer;

	if (!faces.empty())	{	//rect drawing
		for (size_t i = 0; i < faces.size(); ++i){ rectangle(image, faces[i], cv::Scalar(255)); }
	}
	cout << timer / cv::getTickFrequency() << "sec" << " detected " << faces.size() << " face(s)" << endl;
	if (writeImage){ imwrite(ImageName, image); }	//write image with drawn rects
	if (showImage) {								//checking size of image before show it
		float Scale = image.rows;
		if (image.rows > max_height || image.cols > max_width){	//if greater then limit - resize
			Scale /=image.cols;					//find greatest overlimited dimension
			if (Scale >= 0.5625){					//by aspect ratio 16:9
				Scale = image.rows;
				Scale /= max_height;
				Size sz(cvRound(image.cols / Scale), cvRound(image.rows / Scale));
				cv::resize(image, resz_image, sz);
				imshow(ImageName, resz_image);
			}
			else{
				Scale = image.cols;
				Scale /= max_width;
				Size sz(cvRound(image.cols / Scale), cvRound(image.rows / Scale));
				cv::resize(image, resz_image, sz);
				imshow(ImageName, resz_image);
			}
		}
		else{ imshow(ImageName, image); }
		cvWaitKey(0);
	}
}

int main(int argc, char * argv[]){
	WIN32_FIND_DATA ffd;
	TCHAR szDir[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
	size_t length_of_path;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char path[MAX_PATH];
	string input;

	cout << "Simple facedetector for scan all images in directory (version with CUDA)" << endl;
	if (!cascade_cpu.load(CascadeName))	{ cout << "Cascade file not exist or damaged! Place it nearby program and restart" << endl; system("pause"); return 0; }
	else								{ cout << "Haarcascade load sucessfully" << endl; }

	cout << "Use GPY? (y/n): "; cin >> input; if (input == "y" || input == "Y"){ useGPU = true; }
	if (useGPU){
		cv::cuda::printShortCudaDeviceInfo(cv::cuda::getDevice());
		cascade_gpu = cuda::CascadeClassifier::create(CascadeName);
	}
	cout << "Overwrite images with detected faces? (y/n): ";		cin >> input; if (input == "y" || input == "Y"){ writeImage = true; }
	cout << "Show images after it scanned? (y/n): ";				cin >> input; if (input == "y" || input == "Y"){ showImage = true; }
	cout << "Select directory for scan:  ";
	cin >> path;

	// Check that the input path plus 3 is not longer than MAX_PATH. Three characters are for the "\*" plus NULL appended below.
	StringCchLength(path, MAX_PATH, &length_of_path);
	if (length_of_path > (MAX_PATH - 3)){ cout << "Directory path is too long!" << endl;	system("pause"); return 0; }
	// Prepare string for use with FindFile functions.  First, copy the string to a buffer, then append '\*' to the directory name.
	StringCchCopy(szDir, MAX_PATH, path);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));
	// Find the first file in the directory.
	hFind = FindFirstFile(szDir, &ffd);
	if (INVALID_HANDLE_VALUE == hFind) { cout << "No such directory!" << endl; system("pause"); return 0; }
	//All files, except folders, processing in this while
	do
	{
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
			//Prepare full filename - with path
			StringCchCopy(szFileName, MAX_PATH, path);
			StringCchCat(szFileName, MAX_PATH, TEXT("\\"));
			StringCchCat(szFileName, MAX_PATH, TEXT(ffd.cFileName));
			//check image and call facedetect function
			cout << ffd.cFileName << " ";
			image = imread(szFileName);
			if (!image.empty()){detect_faces(image, szFileName);}
			else { cout << "not read" << endl; }
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);
	cout << "Scan completed." << endl;

	system("pause");
	return 0;
}