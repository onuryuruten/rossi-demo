//============================================================================
// Name        : Table Calibration Mono.cpp
// Author      : Yigit
// Version     :
// Copyright   : You dont have right to copy
// Description : Ansi-stylaaa yeah
//============================================================================

#include <stdio.h>
#include <signal.h>

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/GazeControl.h>
#include <yarp/dev/PolyDriver.h>
#include <math.h>
#include <yarp/os/Network.h>
#include <yarp/math/Math.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <opencv/cxcore.h>

#include "ros/ros.h"
#include "cv_bridge/cv_bridge.h"
#include "cv_bridge/CvBridge.h"
#include "tabletop_2D_segmentation/Perception2D.h"

#include "sensor_msgs/Image.h"
#include "sensor_msgs/RegionOfInterest.h"

#include "aff_msgs/ExperimentState.h"
#include "aff_msgs/Features.h"

// ++Onur: inclusion for data logging

#include "tabletop_2D_segmentation/DataLogger.h"

// --Onur

const int N_BINS = 10;
const std::string WORKSPACE_2D_SEGMENTATION_SRV_NAME =
		"/tabletop_2D_segmentation";

YARP_DECLARE_DEVICES(icubmod)
;

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace std;
//using namespace cv;
#define PI 3.1415926535898

struct Packet {
	IplImage* raw_image;
	IplImage* filtered_image;
	std::vector<CvRect> boundingBoxes;
	std::vector<int> segmented_areas;

	bool faceDetected;
	std::vector<CvHistogram*> redHists;
	std::vector<CvHistogram*> blueHists;
	std::vector<CvHistogram*> greenHists;
};

bool first_srv_callback = false;

void initializePacket(Packet *willBeSent);
int givePercentage(IplImage* overlay);
void Allocate_imgs();
int cvOverlayImage(IplImage* src, IplImage* overlay, CvPoint location,
		bool flag);
bool detect(IplImage* img, CvPoint* pt1, CvPoint *pt2);
void drawWireFrame(IplImage *image, bool willBeFilled[][4], float aboveLimit,
		IplImage *colourImage);//,int motion);
float getEuclidianDistance(CvPoint2D32f p1, CvPoint2D32f p2);
int findEdgeColorForIdentification(CvPoint2D32f p1, CvPoint2D32f p2,
		IplImage* image);
int findEdgeColorForVerification(CvPoint2D32f p1, CvPoint2D32f p2,
		IplImage* image);
double findDistanceToLine(CvPoint2D32f p1, CvPoint2D32f p2, CvPoint2D32f point);
int* determineWays(CvPoint2D32f* singleCorners, int indexAndDegrees[][9],
		int index);
int* determineOpenSide(CvPoint2D32f* singleCorners, int indexAndDegrees[][9],
		int index);
int findExtensionColor(CvPoint2D32f source, CvPoint2D32f destination,
		IplImage* image);
void tagOtherCorners(int* ways, int x);
void tagMainCorner(bool selections[], int verhor[], int x, int *ways);
void releaseMemory();
void initializeTheGlobalVars();
void findAreasAndCorners(int indexAndDegrees[][9]);
int floodFill(int a, int b, int *minX, int *minY, int *maxX, int *maxY);
int getImageSize(IplImage *overlay);
bool floodFillCenterBased(int x, int y, int xPixelOffset, int yPixelOffset);

IplImage *img = NULL, *imgResult, *copyBgr, *Image2, *imgRed, *imgGreen,
		*imgBlue;
int mR_val = 255, mG_val = 0, mB_val = 0, MAR_val = 256, MAG_val = 104,
		MAB_val = 157;//default green .ctrl BLUE to find color
char fileName[100];
char positions[4][4][100];
char areas[3][3][100];
CvPoint2D32f places[4][4];
bool detectedFace = false;
CvPoint* pt1, *pt2;
unsigned long total[3];
bool newFace = false;

PolyDriver *clientGaze;
PolyDriver *clientTorso;
IGazeControl *igaze;
IEncoders *ienc;
IPositionControl *ipos;
IPositionControl *iposGaze;

int diff, mean[3], temp[3];
CvScalar color, color1;
CvScalar value11;
float threshold = 100.0;//200

IplImage * hue; // hue channel
IplImage * hue1; // Hue upper bound
IplImage * hue2; // Hue lower bound
IplImage * hue3; // hue color filtering
IplImage * hsvImage; // hsv image

int HuethresH = 70,//155
		HuethresL = 0,//51
		SatthresL = 0, SatthresH = 158,//158
		ValthresL = 0, ValthresH = 0, erosionCount = 0, blurSize = 0;
int c = 0;
int d = 0;

BufferedPort<Bottle> in;
Bottle *inBot;

//int universalUpDeg = 0;
//int universalDownDeg = 0;
//int universalLeftDeg = 0;
//int universalRightDeg = 0;
int* ways;
bool* identified;
int** identificationIndex;
bool willBeFilled[4][4];
int setOrReset[3][3];
int countForIdentified = 0;
CvPoint2D32f* singleCorners;
CvPoint2D32f* features;
CvPoint2D32f* newCornerFeatures;

int countForPixels;

yarp::sig::Vector angles(3);

BufferedPort<ImageOf<PixelBgr> > *portImgOut, *portDenemeOut, *portDenemeOut1;

// Create memory for calculations
static CvMemStorage* storage = 0;

// Create a new Haar classifier
static CvHaarClassifierCascade* cascade = 0;

// Create a string that contains the cascade name
const char* cascade_name = "/home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/icub_aff_learning/tabletop_2D_segmentation/bin/haarcascade_frontalface_alt.xml";

IplImage* images[9];
bool seenOrNot[9];
CvPoint* wireFramePoints = new CvPoint[9];

int locationAndSize[9][4];
Packet willBeSent;
Packet willBeSentOld;

int counter = 0;
IplImage* tempImg;
bool motionDone;
int perceptTrying;

//++Kadir
sensor_msgs::CvBridge* bridge_;
ros::ServiceServer srv_perception_2D;
ros::Subscriber sub_exp_state_;
ros::Publisher pub_tabletop_2d_features_;
aff_msgs::ExperimentState exp_state_;
IplImage* raw_image;
IplImage* filtered_image;
IplImage *rawForDisappear;
IplImage *filteredForDisappear;
cv_bridge::CvImagePtr cv_raw_image_ptr;
ros::NodeHandle* nh;
int oldRegionId = -2;

DataLogger 	*imageLogger = new DataLogger("image","./",-1),
		*imageEffectLogger = new DataLogger("effect_image","./",-1); // TODO: be sure count is of no use or go for something other than -1
ImageFeatureTuple imgTuple, imgEffectTuple; // TODO delete/add if necessary

// ++Onur: Logging the retrieved information
void imageLoggingOp(ImageFeatureTuple &imgTuple, int region_id,sensor_msgs::Image::Ptr raw, sensor_msgs::Image::Ptr filtered)
{		
		/*
			The information to store are as follows:
			
			sensor_msgs/Image raw_image
			sensor_msgs/Image filtered_image
			sensor_msgs/RegionOfInterest rois
			bool face_detected
			int16 ooi_area
			int8[] ooi_color_r_hist
			int8[] ooi_color_g_hist
			int8[] ooi_color_b_hist
		*/		

		// raw image
		/*imgTuple.rawimg_header_seq = raw->header.seq;
		imgTuple.rawimg_header_stamp = raw->header.stamp;
		imgTuple.rawimg_header_frame_id = raw->header.frame_id;*/
		
		imgTuple.rawimg_height = raw->height;
		imgTuple.rawimg_width = raw->width;		
		imgTuple.rawimg_step = raw->step;

		if(imgTuple.rawimg_data == NULL)
		{
			imgTuple.rawimg_data = new int[raw->height*raw->step];
		}

		for(int i = 0; i < raw->height*raw->step; i++)
		{
			imgTuple.rawimg_data[i] = (int)raw->data[i];
		}
		/*imgTuple.rawimg_is_bigendian = raw->is_bigendian;
		imgTuple.rawimg_encoding = raw->encoding;*/
					
		// filtered image
		/*imgTuple.filimg_header_seq = filtered->header.seq;
		imgTuple.filimg_header_stamp = filtered->header.stamp;
		imgTuple.filimg_header_frame_id = filtered->header.frame_id;*/

		
		imgTuple.filimg_height = filtered->height;
		imgTuple.filimg_width = filtered->width;		
		imgTuple.filimg_step = filtered->step;

		if(imgTuple.filimg_data == NULL)
		{
			imgTuple.filimg_data = new int[filtered->height*filtered->step];
		}

		for(int i = 0; i < filtered->height*filtered->step; i++)
		{
			imgTuple.filimg_data[i] = (int)filtered->data[i];
		}
				//(int*)&filtered->data[0];
		/*imgTuple.filimg_is_bigendian = filtered->is_bigendian;
		imgTuple.filimg_encoding = filtered->encoding;*/
		
		// region of interest information
		imgTuple.roi_width = willBeSent.boundingBoxes[region_id].width;
		imgTuple.roi_height = willBeSent.boundingBoxes[region_id].height;
		imgTuple.roi_x_offset = willBeSent.boundingBoxes[region_id].x;
		imgTuple.roi_y_offset = willBeSent.boundingBoxes[region_id].y;


		// Whether a face was detected at this instance:
		imgTuple.face_detected = willBeSent.faceDetected;
				
		// ooi_area
		imgTuple.ooi_area = willBeSent.segmented_areas[region_id];
		
		imgTuple.ooi_color_r_hist.resize(N_BINS);
		imgTuple.ooi_color_g_hist.resize(N_BINS);
		imgTuple.ooi_color_b_hist.resize(N_BINS);
		
				
		for (uint8_t i = 0; i < N_BINS; i++) 
		{
			imgTuple.ooi_color_r_hist[i]
					= cvQueryHistValue_1D(willBeSent.redHists[region_id], i);
			imgTuple.ooi_color_g_hist[i]
					= cvQueryHistValue_1D(willBeSent.greenHists[region_id], i);
			imgTuple.ooi_color_b_hist[i]
					= cvQueryHistValue_1D(willBeSent.blueHists[region_id], i);
		}		
		
		// log the data using void DataLogger::logSingleData(ImageFeatureTuple *currentImgInfo, int label);
		
}



// Uses imgTuple as a reference to initial features of the object
void computeEffect()
{
		// TODO: Discuss for whether the size changes!!!!!


		if(imgEffectTuple.rawimg_height * imgEffectTuple.rawimg_step != imgTuple.rawimg_height * imgTuple.rawimg_step)
		{
			std::cout << "The data sizes for raw img of init & final img tuples do not match" << endl;
		}
		int i;
		for(i = 0; i < imgEffectTuple.rawimg_height * imgEffectTuple.rawimg_step; i++)
		{
			imgEffectTuple.rawimg_data[i] -= imgTuple.rawimg_data[i];
		}

		imgEffectTuple.rawimg_height 	-= imgTuple.rawimg_height;
		imgEffectTuple.rawimg_width 	-= imgTuple.rawimg_width;		
		imgEffectTuple.rawimg_step 	-= imgTuple.rawimg_step;
		
		// TODO: Discuss for whether the size changes!!!!!

		if(imgEffectTuple.filimg_height * imgEffectTuple.filimg_step != imgTuple.filimg_height * imgTuple.filimg_step)
		{
			std::cout << "The data sizes for filtered img of init & final img tuples do not match" << endl;
		}
		
		for(i = 0; i < imgEffectTuple.filimg_height * imgEffectTuple.filimg_step; i++)
		{
			imgEffectTuple.filimg_data[i] -= imgTuple.filimg_data[i];
		}		
		
		imgEffectTuple.filimg_height 	-= imgTuple.filimg_height;
		imgEffectTuple.filimg_width 	-= imgTuple.filimg_width;		
		imgEffectTuple.filimg_step 	-= imgTuple.filimg_step;
		
		// region of interest information
		imgEffectTuple.roi_width 	-= imgTuple.roi_width;
		imgEffectTuple.roi_height 	-= imgTuple.roi_height;
		imgEffectTuple.roi_x_offset 	-= imgTuple.roi_x_offset;
		imgEffectTuple.roi_y_offset 	-= imgTuple.roi_y_offset;

		imgEffectTuple.face_detected = imgEffectTuple.face_detected ^ imgTuple.face_detected;
					
		imgEffectTuple.ooi_area = imgEffectTuple.ooi_area - imgTuple.ooi_area; // i.e., 0 or 1
				
		for (uint8_t i = 0; i < N_BINS; i++) 
		{
			imgEffectTuple.ooi_color_r_hist[i]
					-= imgTuple.ooi_color_r_hist[i];
			imgEffectTuple.ooi_color_g_hist[i]
					-= imgTuple.ooi_color_g_hist[i];
			imgEffectTuple.ooi_color_b_hist[i]
					-= imgTuple.ooi_color_b_hist[i];
		}	

}

//--Onur

void segmentTabletopObjects();

void percept(int region_id,
		tabletop_2D_segmentation::Perception2D::Request& request,
		tabletop_2D_segmentation::Perception2D::Response& response) {

	//		std::cout<<(int)request.arg<<" "<<(int)request.arg2<<" "<<region_id<<std::endl;

	//		percept(region_id);
	//		cv_raw_image_ptr->image = cv::Mat(raw_image);
	//		sensor_msgs::CvBridge::fromIpltoRosImage(willBeSent.raw_image, response.raw_image, "bgr8");
	bool found = false;
	cout<<" hede hodoooo: "<<willBeSent.segmented_areas[region_id]<<endl;
	if(willBeSent.segmented_areas[region_id] != -1)
		found = true;

	while (region_id != -1 && willBeSent.segmented_areas[region_id] == -1
			&& nh->ok() && perceptTrying < 5) {
		cout << "width: " << copyBgr->width << " , height: " << copyBgr->height
				<< endl;
		perceptTrying++;
		segmentTabletopObjects();

		sensor_msgs::Image::Ptr img_ptr = bridge_->cvToImgMsg(
				willBeSent.raw_image, "bgr8");

		//cvShowImage("asd", bridge_->imgMsgToCv((sensor_msgs::Image::ConstPtr)img_ptr, "bgr8"));
		//		cvWaitKey(0);
		response.raw_image = *img_ptr;
		img_ptr = bridge_->cvToImgMsg(willBeSent.filtered_image, "bgr8");
		response.filtered_image = *img_ptr;

		response.rois.resize(9);
		for (uint8_t i = 0; i < willBeSent.boundingBoxes.size(); i++) {
			sensor_msgs::RegionOfInterest roi;
			roi.width = willBeSent.boundingBoxes[i].width;
			roi.height = willBeSent.boundingBoxes[i].height;
			roi.x_offset = willBeSent.boundingBoxes[i].x;
			roi.y_offset = willBeSent.boundingBoxes[i].y;
			response.rois[i] = roi;
		}

		response.face_detected = willBeSent.faceDetected;
		response.ooi_area = willBeSent.segmented_areas[region_id];
		response.ooi_color_r_hist.resize(N_BINS);
		response.ooi_color_g_hist.resize(N_BINS);
		response.ooi_color_b_hist.resize(N_BINS);

		for (uint8_t i = 0; i < N_BINS; i++) {
			response.ooi_color_r_hist[i]
					= cvQueryHistValue_1D(willBeSent.redHists[region_id], i);
			response.ooi_color_g_hist[i]
					= cvQueryHistValue_1D(willBeSent.greenHists[region_id], i);
			response.ooi_color_b_hist[i]
					= cvQueryHistValue_1D(willBeSent.blueHists[region_id], i);
		}
		if (willBeSent.segmented_areas[region_id] != -1)
			found = true;
		ros::spinOnce();
	}
	while (perceptTrying > 4 && !found && region_id != -1 && nh->ok()) {
		cout << "viy viy" << endl;
		found = floodFillCenterBased(request.arg, request.arg2, 0, 0);
		cout<<"center flood fill sonucu: "<<found<<endl;
		int countForRect = 0;
		if (!found) {
			for (int x = -100; x <= 100; x++) {
				for (int y = -100; y <= 100; y++) {
					countForRect++;
					if (copyBgr->height / 2 + y < copyBgr->height
							&& copyBgr->height / 2 + y > 0 && copyBgr->width
							/ 2 + x < copyBgr->width && copyBgr->width / 2 + x
							> 0) {
						found = floodFillCenterBased(request.arg, request.arg2,
								x, y);
						if (found) {
							x = 101;
							y = 101;
						}
					}
					//cvSet2D(src, y+location.y - overlay->height/2, x+location.x- overlay->width/2, over);
				}
			}
		}
		cout<<"countForRect: "<<countForRect<<endl;
		ros::spinOnce();
	}
	if (region_id == -1 && request.task
			== tabletop_2D_segmentation::Perception2D::Request::EXTRACT_EFFECT) {
		cout << "region -1" << endl;

		cout << "SIZES:" << willBeSentOld.segmented_areas[oldRegionId]
				<< "and " << willBeSent.segmented_areas[region_id] << endl;
		cout << "REGION x:" << willBeSentOld.boundingBoxes[oldRegionId].x
				<< "and " << willBeSent.boundingBoxes[region_id].x << endl;
	}
	if (found && request.task
			== tabletop_2D_segmentation::Perception2D::Request::EXTRACT_EFFECT) {

		cout << "SIZES:" << willBeSentOld.segmented_areas[oldRegionId]
				<< "and " << willBeSent.segmented_areas[region_id] << endl;
		cout << "REGION x:" << willBeSentOld.boundingBoxes[oldRegionId].x
				<< "and " << willBeSent.boundingBoxes[region_id].x << endl;
	}
	if (request.task
			== tabletop_2D_segmentation::Perception2D::Request::DO_PERCEPT
			&& found) {
		cout<<"Found and Copied"<<endl;
		oldRegionId = region_id;
		//memcpy(&willBeSentOld, &willBeSent, sizeof(Packet));
		willBeSentOld.faceDetected = willBeSent.faceDetected;

		cvCopyImage(willBeSentOld.raw_image,willBeSent.raw_image);
		cvCopyImage(willBeSentOld.filtered_image,willBeSent.filtered_image);
		for(int x = 0;x < willBeSentOld.blueHists.size();x++)
		{
			willBeSentOld.blueHists[x] = willBeSent.blueHists[x];
		}
		for(int x = 0;x < willBeSentOld.segmented_areas.size();x++)
		{
			willBeSentOld.segmented_areas[x] = willBeSent.segmented_areas[x];
		}
		for(int x = 0;x < willBeSentOld.redHists.size();x++)
		{
			willBeSentOld.redHists[x] = willBeSent.redHists[x];
		}

		for(int x = 0;x < willBeSentOld.greenHists.size();x++)
		{
			willBeSentOld.greenHists[x] = willBeSent.greenHists[x];
		}

		for(int x = 0;x < willBeSentOld.boundingBoxes.size();x++)
		{
			willBeSentOld.boundingBoxes[x] = willBeSent.boundingBoxes[x];
		}

	}
	while (!found && region_id != -1 && nh->ok()) {
		cout << "Region ID:" << region_id << endl;
		cout << "bulamadim hic bisi" << endl;
		ros::spinOnce();
	}

}

void expStateCallback(aff_msgs::ExperimentState::ConstPtr exp_state) {
	first_srv_callback = true;
	exp_state_ = *exp_state;
}

bool perception2DCallback(
		tabletop_2D_segmentation::Perception2D::Request& request,
		tabletop_2D_segmentation::Perception2D::Response& response) {

	std::cout << "**********************" << std::endl;
	std::cout << " service called" << std::endl;
	std::cout << "**********************" << std::endl;
	if (request.task
			== tabletop_2D_segmentation::Perception2D::Request::DO_PERCEPT) {
		int region_id = request.arg * 3 + request.arg2;
		perceptTrying = 0;
		percept(region_id, request, response);
		sensor_msgs::Image::Ptr raw_ptr = bridge_->cvToImgMsg(willBeSentOld.raw_image, "bgr8");
		sensor_msgs::Image::Ptr filt_ptr = bridge_->cvToImgMsg(willBeSentOld.filtered_image, "bgr8");
		// ++Onur
		cout<<"REGION ID:"<<region_id<<endl;
		cout<<"FACEEEE:"<<willBeSentOld.faceDetected<<endl;
		std::cout << "T2D@ DO_PERCEPT: Logging started" << std::endl;
		imageLoggingOp(imgTuple,region_id,raw_ptr, filt_ptr);
		imageLogger->logSingleData(&imgTuple,region_id);
		std::cout << "Logging done" << std::endl;
		
		// --Onur
	} else if (request.task
			== tabletop_2D_segmentation::Perception2D::Request::EXTRACT_EFFECT) {
		int region_id = request.arg * 3 + request.arg2;
		bool objPresent = true;
	cout<<"REGION ID:"<<region_id<<endl;
		if (region_id != -1) {
			perceptTrying = 0;
			percept(region_id, request, response);


		} else {
			
			objPresent = false;
			perceptTrying = 0;
			newFace = willBeSent.faceDetected;
			cvCopyImage(willBeSent.raw_image,rawForDisappear);
			cvCopyImage(willBeSent.filtered_image,filteredForDisappear);
			initializePacket(&willBeSent);
			willBeSent.faceDetected =newFace;
			cvCopyImage(rawForDisappear,willBeSent.raw_image);
			cvCopyImage(filteredForDisappear,willBeSent.filtered_image);
			percept(region_id, request, response);
			region_id = oldRegionId;
		}
		cout<<"FACEEEE:"<<willBeSent.faceDetected<<endl;
		std::cout << "T2D@ EXTRACT_EFFECT: Final Logging started" << std::endl;
		// ++Onur: Final features & effect logging
		
		if(willBeSent.raw_image == NULL)
		{
			std::cout << "Houston; we can not have a null image. willBeSentOld has null raw_image" << std::endl;
		}
		if(willBeSent.filtered_image == NULL)
		{
			std::cout << "Houston; we can not have a null image. willBeSentOld has null filtered_image" << std::endl;
		}	
		
		
			
		sensor_msgs::Image::Ptr raw_ptr = bridge_->cvToImgMsg(willBeSent.raw_image, "bgr8");
		sensor_msgs::Image::Ptr filt_ptr = bridge_->cvToImgMsg(willBeSent.filtered_image, "bgr8");

		if(region_id < 0)
		{
			region_id = oldRegionId;
		}

		std::cout << "T2D@ EXTRACT_EFFECT: Before imageLoggingOp with region id: " << region_id << std::endl;
		imageLoggingOp(imgEffectTuple,region_id,raw_ptr, filt_ptr);
		
		std::cout << "T2D@ EXTRACT_EFFECT: imageLoggingOp passed." << std::endl;
		imageLogger->logSingleData(&imgEffectTuple,region_id);
		std::cout << "T2D@ EXTRACT_EFFECT: logging..." << std::endl;
//		if(region_id >= 0)
//			imageLogger->logSingleData(&imgTuple,region_id);
//		else
//			std::cout << "T2D@ EXTRACT_EFFECT: disappear?..." << std::endl;
		//Extract Features between willbesent and willbesentOld neye yollayacagim
		
		// The effect logging will commence here, i.e., when the object is properly found...
		std::cout << "T2D@ EXTRACT_EFFECT: Effect Logging started" << std::endl;
		if(objPresent)
		{
			computeEffect();
		}
		imageEffectLogger->logSingleData(&imgEffectTuple,region_id);

		//raw_ptr = 0;
		//filt_ptr = 0;
		// --Onur
	}
	std::cout<<"******** CIKTIM ***************"<<std::endl;
	return true;
}

int countt = 0;
int motion;
bool flag = true;
ImageOf<PixelBgr> *frameLeft;
CvFont font;
ImageOf<PixelBgr> imgOut, denemeOut, denemeOut1;
BufferedPort<ImageOf<PixelBgr> > imagePortLeft, imagePortRight;
BufferedPort<Bottle> motionSense;
IplImage* tempImgG;
IplImage* tempImg1;
IplImage* tempImgForROI;
IplImage* wireFrame;
IplImage* tmp_image;
IplImage* eig_image;
IplImage *ROIGrayAndBinary;
IplImage *ROI;
IplImage *destination;
IplImage *maskImage;
IplImage *copyOfGlobBgr;


void segmentTabletopObjects() {
	frameLeft = imagePortLeft.read();
	if (frameLeft != NULL) {
		countt++;
		cout << countt << endl;
		IplImage *bgr_frameLeft = NULL;
		if (bgr_frameLeft != NULL) {
			cvReleaseImage(&bgr_frameLeft);
			bgr_frameLeft = NULL;
		}
		bgr_frameLeft = (IplImage*) frameLeft->getIplImage();
		countForIdentified = 0;
		cvCvtColor(bgr_frameLeft, tempImgG, CV_BGR2GRAY);
		cvCopyImage(bgr_frameLeft,tempImg);

		//cvSmooth(tempImgG, tempImgG, CV_GAUSSIAN, 3, 3, 0, 0);
		cvThreshold(tempImgG, imgResult, 250, 255, CV_THRESH_BINARY);

		//cvErode(imgResult, imgResult, NULL,1);
		igaze->getAngles(angles);

		//inBot = motionSense.read();
		//if (inBot == NULL) continue;

		//motion = inBot->get(0).asInt();
		//cout<<"motion: "<<motion<<endl;
		//igaze->checkMotionDone(&motionDone);
		//cout<<"Motion Done: "<<motionDone<<endl;

		if (angles[1] < -26 && !(exp_state_.experiment_state
		 == aff_msgs::ExperimentState::LET_HUMAN_ACT || exp_state_.experiment_state
		 == aff_msgs::ExperimentState::ACTION))//&& motion != 1)//Internal motion da eklenecek//exp_state_
		{
			releaseMemory();
			//cvShowImage("original image",bgr_frameLeft);
			//cvWaitKey(0);
			cvCopyImage(bgr_frameLeft,willBeSent.raw_image);
			cvCopyImage(bgr_frameLeft,copyOfGlobBgr);
			//cout<<"aney"<<endl;
			//Feature Detection

			CvSize img_sz = cvGetSize(imgResult);
			if (tmp_image != NULL) {
				cvReleaseImage(&tmp_image);
				tmp_image = NULL;
			}
			tmp_image = cvCreateImage(img_sz, IPL_DEPTH_32F, 1);
			if (eig_image != NULL) {
				cvReleaseImage(&eig_image);
				eig_image = NULL;
			}
			eig_image = cvCreateImage(img_sz, IPL_DEPTH_32F, 1);
			//cout<<"aney1"<<endl;
			int win_size = 20;
			int corner_count = 100;
			features = new CvPoint2D32f[corner_count];
			cvGoodFeaturesToTrack(imgResult, eig_image, tmp_image, features,
					&corner_count, 0.1, 5, 0, 4, 0, 0.04);



//			cvShowImage("edges",imgResult);
//						cvWaitKey(0);
			//cvGoodFeaturesToTrack(imgResult, eig_image, tmp_image, features,
			//&corner_count, 0.5, 5, 0, 3,1, 0.04);
			//				cvFindCornerSubPix(imgResult, features, corner_count, cvSize(win_size,
			//						win_size), cvSize(-1, -1), cvTermCriteria(CV_TERMCRIT_ITER
			//						| CV_TERMCRIT_EPS, 20, 0.03));
//							cvCopyImage(bgr_frameLeft,tempImg);
//							for (int i = 0; i < corner_count; i++) {
//								cvCircle(tempImg, cvPoint(features[i].x, features[i].y), 4, cvScalar(0,
//										255, 0), -1, 8);
//							}
//							cvShowImage("all features",tempImg);
//							cvWaitKey(0);
			//				sprintf(fileName,"/home/yigit/images/features%d.jpg",c);
			//							  cvSaveImage(fileName,tempImg);
			//							  c++;

			cvDilate(imgResult, imgResult, NULL, 3);
//			cvShowImage("imgResult",imgResult);
//					cvWaitKey(0);

			//				sprintf(fileName,"/home/yigit/images/white%d.jpg",c);
			//				  cvSaveImage(fileName,imgResult);
			//				  c++;
			int validationNos[corner_count];
			double degrees[corner_count][2];
			for (int i = 0; i < corner_count; i++) {
				validationNos[i] = 0;
			}
			int white;
			double deg = 0.0;
			int count = 0;
			for (int i = 0; i < corner_count; i++) {
				for (int j = 0; j < corner_count; j++) {
					if (i != j && (getEuclidianDistance(features[i],
							features[j]) > 60 + features[i].y * 0.05)) {//80 and 0.05

						white = findEdgeColorForVerification(features[i],
								features[j], imgResult);
						if (white == 1) {
							deg = (int) ((atan2(
									(features[i].y - features[j].y),
									(features[i].x - features[j].x)) * 180)
									/ PI + 360) % 360;

							if (validationNos[i] == 0) {

								degrees[i][validationNos[i]] = deg;
								validationNos[i]++;
							} else {
								int degreeDiff = abs(
										degrees[i][validationNos[i] - 1] - deg);
								if ((degreeDiff > 45 && degreeDiff < 135)
										|| (degreeDiff > 225 && degreeDiff
												< 315)) {

									degrees[i][validationNos[i]] = deg;

									validationNos[i]++;
									count++;

								}

							}

						}
						white = 0;

					}
					if (validationNos[i] == 2)
						break;

				}

			}
			newCornerFeatures = new CvPoint2D32f[count];
			int index = 0;
			for (int i = 0; i < corner_count; i++) {
				if (validationNos[i] == 2) {
					newCornerFeatures[index].x = features[i].x;
					newCornerFeatures[index].y = features[i].y;
					index++;
				}

			}

			singleCorners = new CvPoint2D32f[count];
			for (int i = 0; i < count; i++) {
				singleCorners[i].x = 0;
				singleCorners[i].y = 0;
			}
			int grouping[count];
			for (int i = 0; i < count; i++) {
				grouping[i] = -1;
			}

			counter = 0;
			int no = 1;
			for (int i = 0; i < count; i++) {
				int group = counter;
				if (grouping[i] == -1) {
					grouping[i] = group;
					singleCorners[counter].x += (int) newCornerFeatures[i].x;
					singleCorners[counter].y += (int) newCornerFeatures[i].y;

					for (int x = i + 1; x < count; x++) {

						if (grouping[x] == -1 && getEuclidianDistance(
								newCornerFeatures[i], newCornerFeatures[x])
								< 30 + newCornerFeatures[i].y * 0.05) {//)+ (newCornerFeatures[i].y/480)*90 ) {//35 and 0.13
							//cout<<"Icerde"<<endl;
							grouping[x] = group;
							singleCorners[counter].x
									+= (int) newCornerFeatures[x].x;
							singleCorners[counter].y
									+= (int) newCornerFeatures[x].y;
							no++;
						}
					}
					singleCorners[counter].x = (int) (singleCorners[counter].x
							/ no);
					singleCorners[counter].y = (int) (singleCorners[counter].y
							/ no);
					no = 1;
					counter++;
				}
			}

			cvCopyImage(bgr_frameLeft,tempImg);
			for (int i = 0; i < counter; i++) {
				cvCircle(tempImg, cvPoint(singleCorners[i].x,
						singleCorners[i].y), 4, cvScalar(0, 255, 0), -1, 8);
			}
//							cvShowImage("Corners",tempImg);
//							cvWaitKey(0);
			//					sprintf(fileName,"/home/yigit/images/corners%d.jpg",d);
			//					  cvSaveImage(fileName,tempImg);
			//					  d++;

			//cvCopyImage(bgr_frameLeft,tempImg);

			int indexAndDegrees[counter][9];
			for (int i = 0; i < counter; i++) {
				indexAndDegrees[i][0] = 0;
				for (int j = 1; j < 9; j++) {
					indexAndDegrees[i][j] = -1;
				}
			}

			for (int i = 0; i < counter; i++) {
				//					cvCopyImage(bgr_frameLeft,tempImg);
				//					cvCircle(tempImg, cvPoint(singleCorners[i].x, singleCorners[i].y), 4,
				//													cvScalar(255, 255, 0), -1, 8);
				//					cvShowImage("Neighbours",tempImg);
				//					cvWaitKey(0);
				for (int j = 0; j < counter; j++) {
					if (i != j) {

						white = findEdgeColorForVerification(singleCorners[i],
								singleCorners[j], imgResult);

						if (white == 1) {
							//								cvCircle(tempImg, cvPoint(singleCorners[j].x, singleCorners[j].y), 4,
							//										cvScalar(0, 255, 0), -1, 8);
							//								cvShowImage("Neighbours",tempImg);
							//													cvWaitKey(0);

							int degree = (int) ((atan2((singleCorners[i].y
									- singleCorners[j].y), (singleCorners[i].x
									- singleCorners[j].x)) * 180) / PI + 360)
									% 360;
							int minIndex = j;
							int minDegree = degree;
							double minValue = getEuclidianDistance(
									singleCorners[i], singleCorners[j]);
							bool flag = false;
							if ((degree > 45 && degree <= 135) || (degree > 205
									&& degree < 325)) {
								if (minValue < 150)//220
									flag = true;
							} else if ((degree > 315 || degree < 45) || (degree
									> 135 && degree < 215)) {
								if (minValue < 220)//300
									flag = true;
							}

							//cout<<"Uzaklik "<<minValue<<endl;
							int degreeToCompare = -1;

							for (int k = 0; k < counter; k++) {
								if (!(i == k || j == k)) {
									//cout<<"KLAR"<<endl;

									white = findEdgeColorForVerification(
											singleCorners[i], singleCorners[k],
											imgResult);
									if (white == 1) {
										degreeToCompare = (int) ((atan2(
												(singleCorners[i].y
														- singleCorners[k].y),
												(singleCorners[i].x
														- singleCorners[k].x))
												* 180) / PI + 360) % 360;
										//cout<<"compare Edilecek Uzaklik"<<getEuclidianDistance(singleCorners[i],
										//singleCorners[k])<<endl;
										if (degreeToCompare > 45
												&& degreeToCompare < 315
												&& degree > 45 && degree < 315
												&& degreeToCompare < degree
														+ 10 && degreeToCompare
												> degree - 10) {

											if (getEuclidianDistance(
													singleCorners[i],
													singleCorners[k])
													< minValue
													&& ((degreeToCompare > 35
															&& degreeToCompare
																	<= 145)
															|| (degreeToCompare
																	> 205
																	&& degreeToCompare
																			< 325))
													&& getEuclidianDistance(
															singleCorners[i],
															singleCorners[k])
															< 150) {//220
												minValue
														= getEuclidianDistance(
																singleCorners[i],
																singleCorners[k]);
												minIndex = k;
												minDegree = degreeToCompare;
												//cout<<"Degisti"<<endl;

											} else if (getEuclidianDistance(
													singleCorners[i],
													singleCorners[k])
													< minValue
													&& (degreeToCompare > 135
															&& degreeToCompare
																	< 215)
													&& getEuclidianDistance(
															singleCorners[i],
															singleCorners[k])
															< 220) {//300
												minValue
														= getEuclidianDistance(
																singleCorners[i],
																singleCorners[k]);
												minIndex = k;
												minDegree = degreeToCompare;
												//cout<<"Degisti"<<endl;
											}

										} else if ((degreeToCompare > 315
												|| degreeToCompare < 45)
												&& (degree < 45 || degree > 315)) {
											if (getEuclidianDistance(
													singleCorners[i],
													singleCorners[k])
													< minValue
													&& getEuclidianDistance(
															singleCorners[i],
															singleCorners[k])
															< 220) {//300
												minValue
														= getEuclidianDistance(
																singleCorners[i],
																singleCorners[k]);
												minIndex = k;
												minDegree = degreeToCompare;
												//cout<<"Degisti"<<endl;

											}
										}
									}
								}

							}
							int noOfCorner = indexAndDegrees[i][0];
							//cout<<"no of corner: "<<noOfCorner<<endl;
							if (noOfCorner != 4) {
								for (int x = 0; x < noOfCorner + 1; x++) {

									if (indexAndDegrees[i][2 * x + 1]
											== minIndex) {
										//cout<<"zaten var"<<endl;
										break;
									} else if (indexAndDegrees[i][2 * x + 1]
											== -1 && flag) {
										indexAndDegrees[i][2 * x + 1]
												= minIndex;
										indexAndDegrees[i][2 * x + 2]
												= minDegree;
										indexAndDegrees[i][0]++;
										//cout<<"Kaydedildi"<<endl;

									}
								}
							}

						}

					}

				}
			}

//							cout<<"Neighbours:"<<endl;
//							for (int y = 0; y < counter; y++) {
//								cout<<indexAndDegrees[y][0] <<endl;
//								cvCopyImage(bgr_frameLeft,tempImg);
//								cvCircle(tempImg, cvPoint(singleCorners[y].x, singleCorners[y].y), 4,cvScalar(255, 255, 0), -1, 8);
//								for (int i = 0; i < indexAndDegrees[y][0]; i++) {
//									cvCircle(tempImg, cvPoint(singleCorners[indexAndDegrees[y][2*i+1]].x, singleCorners[indexAndDegrees[y][2*i+1]].y), 4,
//											cvScalar(0, 255, 0), -1, 8);
//								}
//								cvShowImage("Neighbours",tempImg);
//								cvWaitKey(0);
//
//							}
//							cout<<"Neighbours finish"<<endl;

			identificationIndex = new int*[counter];
			for (int y = 0; y < counter; y++) {
				identificationIndex[y] = new int[2];
			}

			identified = new bool[counter];
			for (int x = 0; x < counter; x++) {
				identified[x] = false;
			}

			for (int x = 0; x < counter; x++) {
				for (int y = 0; y < 2; y++) {
					identificationIndex[x][y] = -1;
				}
			}

			for (int x = 0; x < 4; x++) {
				for (int y = 0; y < 4; y++) {
					willBeFilled[x][y] = false;
				}
			}

			/*
			 * START LABELING
			 */
			findAreasAndCorners(indexAndDegrees);
			/*
			 * FINISH LABELING
			 */

			cvCopyImage(bgr_frameLeft,tempImg);
			cvCopyImage(bgr_frameLeft,tempImg1);
			for (int x = 0; x < counter; x++) {
				if (identified[x]) {
					cvCircle(tempImg1, cvPoint(singleCorners[x].x,
							singleCorners[x].y), 4, cvScalar(0, 255, 0), -1, 8);
					cvPutText(
							tempImg1,
							positions[identificationIndex[x][0]][identificationIndex[x][1]],
							cvPoint(singleCorners[x].x, singleCorners[x].y),
							&font, cvScalar(0, 255, 0, 0));
				}
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {

					if (willBeFilled[x][y] && willBeFilled[x + 1][y + 1]
							&& (places[x][y].x + places[x + 1][y + 1].x) / 2
									< tempImg->width && (places[x][y].x
							+ places[x + 1][y + 1].x) / 2 > 0
							&& (places[x][y].y + places[x + 1][y + 1].y) / 2
									< tempImg->height && (places[x][y].y
							+ places[x + 1][y + 1].y) / 2 > 0) {

						cvCircle(tempImg, cvPoint((places[x][y].x + places[x
								+ 1][y + 1].x) / 2, (places[x][y].y + places[x
								+ 1][y + 1].y) / 2), 4, cvScalar(0, 255, 0),
								-1, 8);
						cvPutText(tempImg, areas[x][y], cvPoint((places[x][y].x
								+ places[x + 1][y + 1].x) / 2, (places[x][y].y
								+ places[x + 1][y + 1].y) / 2), &font,
								cvScalar(0, 255, 0, 0));
					} else if (willBeFilled[x][y + 1] && willBeFilled[x + 1][y]
							&& (places[x][y + 1].x + places[x + 1][y].x) / 2
									< tempImg->width && (places[x][y + 1].x
							+ places[x + 1][y].x) / 2 > 0
							&& (places[x][y + 1].y + places[x + 1][y].y) / 2
									< tempImg->height && (places[x][y + 1].y
							+ places[x + 1][y].y) / 2 > 0) {

						cvCircle(tempImg, cvPoint((places[x][y + 1].x
								+ places[x + 1][y].x) / 2, (places[x][y + 1].y
								+ places[x + 1][y].y) / 2), 4, cvScalar(0, 255,
								0), -1, 8);
						cvPutText(tempImg, areas[x][y], cvPoint((places[x][y
								+ 1].x + places[x + 1][y].x) / 2, (places[x][y
								+ 1].y + places[x + 1][y].y) / 2), &font,
								cvScalar(0, 255, 0, 0));
					}

				}
			}

			//Prepare the Image
			int width = bgr_frameLeft->width;
			int height = bgr_frameLeft->height;
			if (width > 0 && height > 0) {
				if (copyBgr != NULL) {
					cvReleaseImage(&copyBgr);
					copyBgr = NULL;
				}
				copyBgr = cvCreateImage(cvSize(bgr_frameLeft->width,
						bgr_frameLeft->height), IPL_DEPTH_8U, 3);
				cvCopyImage(bgr_frameLeft,copyBgr);
				if (ROIGrayAndBinary != NULL) {
					cvReleaseImage(&ROIGrayAndBinary);
					ROIGrayAndBinary = NULL;
				}
				ROIGrayAndBinary = cvCreateImage(cvSize(width, height),
						IPL_DEPTH_8U, 1);
				cvCvtColor(copyBgr, ROIGrayAndBinary, CV_BGR2GRAY);
				//cvSaveImage("/home/yigit/withWhite1.jpg",copyBgr);
				cvThreshold(ROIGrayAndBinary, ROIGrayAndBinary, 210, 255,
						CV_THRESH_BINARY );
				cvSmooth(ROIGrayAndBinary, ROIGrayAndBinary, CV_GAUSSIAN, 3, 3,
						0, 0);
				cvDilate(ROIGrayAndBinary, ROIGrayAndBinary, NULL, 4);

				CvScalar ptr;
				CvScalar ptr1;
				for (int a = 0; a < width; a++) {
					for (int b = 0; b < height; b++) {
						if (b < ROIGrayAndBinary->height && b > 0 && a
								< ROIGrayAndBinary->width && a > 0) {
							ptr = cvGet2D(ROIGrayAndBinary, b, a);
							if (ptr.val[0] > 200.0 && b < copyBgr->height && b
									> 0 && a < copyBgr->width && a > 0)
								cvSet2D(copyBgr, b, a, cvScalarAll(0));
						}
					}

				}

				//cvSaveImage("/home/yigit/withoutWhite12.jpg",copyBgr);

				//++Ygt
				hsvImage
						= cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
				hue = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
				hue1 = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
				hue2 = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
				hue3 = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);

				cvCvtColor(copyBgr, hsvImage, CV_BGR2HSV);

				cvSetImageCOI(hsvImage, 1);
				cvCopy(hsvImage, hue);

				CvScalar hsv_min = cvScalar(5, 0, 0, 0);
				CvScalar hsv_max = cvScalar(40, 0, 0, 0);
				cvInRangeS(hue, hsv_min, hsv_max, hue1);

				CvScalar hsv_min1 = cvScalar(70, 0, 0, 0);
				CvScalar hsv_max1 = cvScalar(120, 0, 0, 0);
				cvInRangeS(hue, hsv_min1, hsv_max1, hue2);

				//cvThreshold (hue,hue1,HuethresL,180, CV_THRESH_BINARY); // get lower bound
				cvSmooth(hue1, hue1, CV_MEDIAN, 9, 9, 0, 0);
				//cvShowImage("hue1",hue1);
				//cvWaitKey(0);
				//cvThreshold (hue, hue2,HuethresH,180, CV_THRESH_BINARY_INV); // get upper bound
				cvSmooth(hue2, hue2, CV_MEDIAN, 9, 9, 0, 0);
				//cvShowImage("hue2",hue2);
				//cvWaitKey(0);
				for (int a = 0; a < width; a++) {
					for (int b = 0; b < height; b++) {
						if (b < hue1->height && b > 0 && a < hue1->width && a
								> 0 && b < hue2->height && b > 0 && a
								< hue2->width && a > 0) {
							ptr = cvGet2D(hue1, b, a);
							ptr1 = cvGet2D(hue2, b, a);
							int g = ((int) ptr.val[0] | (int) ptr1.val[0]);
							if (b < hue3->height && b > 0 && a < hue3->width
									&& a > 0)
								cvSet2D(hue3, b, a, cvScalarAll(g));
						}
					}

				}
				//cvInRangeS()()
				cvSmooth(hue3, hue3, CV_MEDIAN, 9, 9, 0, 0);

				for (int a = 0; a < width; a++) {
					for (int b = 0; b < height; b++) {
						//cout<<x<<" "<<y<<endl;
						if (b < copyBgr->height && b > 0 && a < copyBgr->width
								&& a > 0) {
							ptr = cvGet2D(hue3, b, a);
							if (ptr.val[0] == 0 && b < copyBgr->height && b > 0
									&& a < copyBgr->width && a > 0)
								cvSet2D(copyBgr, b, a, cvScalarAll(255));
						}
					}

				}
				//cvShowImage("Captures",copyBgr);
				//cvWaitKey(0);
				//					IplImage *gray=cvCreateImage(cvGetSize(copyBgr), IPL_DEPTH_8U, 1);
				//					cvCvtColor(copyBgr,gray,CV_BGR2GRAY);
				//					cvThreshold(gray, gray, 200, 255, CV_THRESH_BINARY_INV);
				//					cvShowImage("Captures",gray);
				//										cvWaitKey(0);
				//					IplImage *labelImg=cvCreateImage(cvGetSize(gray), IPL_DEPTH_LABEL, 1);
				//					cvb::CvBlobs blobs;
				//					unsigned int result=cvLabel(gray, labelImg, blobs);
				//					cout<<"result: "<<result<<endl;
				//					cvRenderBlobs(labelImg, blobs, copyBgr, copyBgr);
				//					cvShowImage("Captures",copyBgr);
				//										cvWaitKey(0);
				//					for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
				//					{
				//						it->second->
				//					  cout << "Blob #" << it->second->label << ": Area=" << it->second->area << ", Centroid=(" << it->second->centroid.x << ", " << it->second->centroid.y << ")" << endl;
				//					}
				//					//cvShowImage("Captures",labelImg);
				//					cvWaitKey(0);
				cvReleaseImage(&hsvImage);
				cvReleaseImage(&hue);
				cvReleaseImage(&hue1);
				cvReleaseImage(&hue2);
				hsvImage = NULL;
				hue = NULL;
				hue1 = NULL;
				hue2 = NULL;

			}

			//cvDilate(copyBgr, copyBgr, NULL,3);
			//cvSaveImage("/home/kadir/Desktop/segmentedWhole.jpg",copyBgr);

//												cvShowImage("Filtered",copyBgr);
//												cvWaitKey(0);
			if (Image2 != NULL) {
				cvReleaseImage(&Image2);
				Image2 = NULL;
			}
			Image2 = cvCreateImage(cvSize(copyBgr->width, copyBgr->height),
					IPL_DEPTH_8U, 3);
			cvZero(Image2);
			int result = 0;
			cvCopyImage(copyBgr,willBeSent.filtered_image);

			//cout<<"abbolo"<<endl;
			int minY, maxY, minX, maxX;
			for (int x = 2; x >= 0; x--) {
				for (int y = 2; y >= 0; y--) {
					//cout<<"x: "<<x<<" , y: "<<y<<endl;
					if (willBeFilled[x][y] && willBeFilled[x + 1][y + 1]) {
						//cout<<"girebildim1"<<endl;
						minY = 480, maxY = 0, minX = 640, maxX = 0;
						total[0] = 0;
						total[1] = 0;
						total[2] = 0;
						diff = 0;
						countForPixels = 0;
						cvZero(Image2);
						result = floodFill((places[x][y].x + places[x + 1][y
								+ 1].x) / 2, (places[x][y].y + places[x + 1][y
								+ 1].y) / 2, &minX, &minY, &maxX, &maxY);
						//cout<<minX<<","<<minY<<","<<maxX<<","<<maxY<<endl;
						if (result == 3) {
							if (minX < copyBgr->width && minX >= 0 && maxX
									< copyBgr->width && maxX >= 0 && minY
									< copyBgr->height && minY >= 0 && maxY
									< copyBgr->height && maxY >= 0) {
								if (x == 2) {
									if (places[x][y].y - minY
											> (places[x + 1][y + 1].y
													- places[x][y].y) / 4) {
										setOrReset[x][y] = 1;
									} else {
										setOrReset[x][y] = 0;
									}
								} else if (x == 1) {
									//cout<<"1. 1 deyim"<<endl;
									if (setOrReset[x + 1][y] == -1
											|| setOrReset[x + 1][y] == 0) {
										if (places[x][y].y - minY > (places[x
												+ 1][y + 1].y - places[x][y].y)
												/ 4) {
											setOrReset[x][y] = 1;
										} else if (setOrReset[x + 1][y] == -1
												&& (maxY
														- places[x + 1][y + 1].y
														> (places[x + 1][y + 1].y
																- places[x][y].y)
																/ 4)) {
											setOrReset[x][y] = -1;
										} else if (setOrReset[x + 1][y] != 1) {
											setOrReset[x][y] = 0;
										}
									}

								} else {
									if (setOrReset[x + 1][y] == -1 && (maxY
											- places[x + 1][y + 1].y
											> (places[x + 1][y + 1].y
													- places[x][y].y) / 4)) {
										setOrReset[x][y] = -1;
									} else if (setOrReset[x + 1][y] != 1)
										setOrReset[x][y] = 0;

								}
								//cout<<"girebildim1"<<endl;
								//IplImage *ROI = cvCreateImage(cvSize(maxX-minX, maxY-minY), IPL_DEPTH_8U,3);
								if (images[y + x * 3] != NULL) {
									cvReleaseImage(&images[y + x * 3]);
									images[y + x * 3] = NULL;

								}
								images[y + x * 3] = cvCreateImage(cvSize(maxX
										- minX, maxY - minY), copyBgr->depth,
										copyBgr->nChannels);
								//									if( x != 2 && setOrReset[x+1][y] != 1)
								//									{
								//										willBeSent.ROIs[y + x*3] = cvCreateImage( cvSize( maxX-minX,  maxY-minY),copyBgr->depth, copyBgr->nChannels );
								//										willBeSent.locationAndSize[y + x*3][0] = minX;
								//										willBeSent.locationAndSize[y + x*3][1] = minY;
								//										willBeSent.locationAndSize[y + x*3][2] = maxX-minX;
								//										willBeSent.locationAndSize[y + x*3][3] = maxY-minY;
								//									}
								if (x != 2 && setOrReset[x + 1][y] != 1) {
									willBeSent.boundingBoxes[y + x * 3]
											= cvRect(minX, minY, maxX - minX,
													maxY - minY);

								} else if (x == 2) {
									willBeSent.boundingBoxes[y + x * 3]
											= cvRect(minX, minY, maxX - minX,
													maxY - minY);
								}

								cvSetImageROI(copyBgr, cvRect(minX, minY, maxX
										- minX, maxY - minY));
								if (cvGetImageROI(copyBgr).width == maxX - minX
										&& cvGetImageROI(copyBgr).height
												== maxY - minY) {

									if (x != 2 && setOrReset[x + 1][y] != 1) {
										cvCopy(copyBgr, images[y + x * 3]);
										//											cvShowImage("Captures",copyBgr);
										//															cvWaitKey(0);
										if (!seenOrNot[y + x * 3])
											seenOrNot[y + x * 3] = true;
										//cvCopy(copyBgr, willBeSent.ROIs[y + x*3]);
									} else if (x == 2) {
										cvCopy(copyBgr, images[y + x * 3]);
										if (!seenOrNot[y + x * 3])
											seenOrNot[y + x * 3] = true;
									}
								}

								cvResetImageROI(copyBgr);
							}

						} else if (result == -1 || result == -2) {
							if (seenOrNot[y + x * 3]) {
								//TODO: Yigit
								willBeSent.boundingBoxes[y + x * 3] = cvRect(0,
										0, 0, 0);
								willBeSent.segmented_areas[y + x * 3] = -1;
								cvClearHist(willBeSent.blueHists[y + x * 3]);
								cvClearHist(willBeSent.redHists[y + x * 3]);
								cvClearHist(willBeSent.greenHists[y + x * 3]);
								setOrReset[x][y] = -1;
								seenOrNot[y + x * 3] = false;
								if (images[y + x * 3] != NULL) {
									cvReleaseImage(&images[y + x * 3]);
									images[y + x * 3] = NULL;
								}

							}
						}
					} else if (willBeFilled[x][y + 1] && willBeFilled[x + 1][y]) {

						minY = 480, maxY = 0, minX = 640, maxX = 0;
						total[0] = 0;
						total[1] = 0;
						total[2] = 0;
						diff = 0;
						countForPixels = 0;
						cvZero(Image2);
						result = floodFill((places[x][y + 1].x
								+ places[x + 1][y].x) / 2, (places[x][y + 1].y
								+ places[x + 1][y].y) / 2, &minX, &minY, &maxX,
								&maxY);
						if (result == 3) {
							if (minX < copyBgr->width && minX >= 0 && maxX
									< copyBgr->width && maxX >= 0 && minY
									< copyBgr->height && minY >= 0 && maxY
									< copyBgr->height && maxY >= 0) {

								if (x == 2) {
									if (places[x][y + 1].y - minY > (places[x
											+ 1][y].y - places[x][y + 1].y) / 4) {
										setOrReset[x][y] = 1;
									} else {
										setOrReset[x][y] = 0;
									}
								} else if (x == 1) {
									//cout<<"2. 1 deyim"<<endl;
									if (setOrReset[x + 1][y] == -1
											|| setOrReset[x + 1][y] == 0) {
										if (places[x][y + 1].y - minY
												> (places[x + 1][y].y
														- places[x][y + 1].y)
														/ 4) {
											setOrReset[x][y] = 1;
										} else if (setOrReset[x + 1][y] == -1
												&& (maxY - places[x + 1][y].y
														> (places[x + 1][y].y
																- places[x][y
																		+ 1].y)
																/ 4)) {
											setOrReset[x][y] = -1;
										} else if (setOrReset[x + 1][y] != 1) {
											setOrReset[x][y] = 0;
										}
									}

								} else {
									if (setOrReset[x + 1][y] == -1
											&& (maxY - places[x + 1][y].y
													> (places[x + 1][y].y
															- places[x][y + 1].y)
															/ 4))// ||setOrReset[x+1][y] == -1)
									{
										setOrReset[x][y] = -1;
									} else if (setOrReset[x + 1][y] != 1)
										setOrReset[x][y] = 0;
								}

								//IplImage *ROI = cvCreateImage(cvSize(maxX-minX, maxY-minY), IPL_DEPTH_8U,3);
								if (images[y + x * 3] != NULL) {
									cvReleaseImage(&images[y + x * 3]);
									images[y + x * 3] = NULL;

								}
								images[y + x * 3] = cvCreateImage(cvSize(maxX
										- minX, maxY - minY), copyBgr->depth,
										copyBgr->nChannels);
								//									if( x != 2 && setOrReset[x+1][y] != 1)
								//									{
								//										willBeSent.ROIs[y + x*3] = cvCreateImage( cvSize( maxX-minX,  maxY-minY),copyBgr->depth, copyBgr->nChannels );
								//										willBeSent.locationAndSize[y + x*3][0] = minX;
								//										willBeSent.locationAndSize[y + x*3][1] = minY;
								//										willBeSent.locationAndSize[y + x*3][2] = maxX-minX;
								//										willBeSent.locationAndSize[y + x*3][3] = maxY-minY;
								//									}
								if (x != 2 && setOrReset[x + 1][y] != 1) {
									willBeSent.boundingBoxes[y + x * 3]
											= cvRect(minX, minY, maxX - minX,
													maxY - minY);

								} else if (x == 2) {
									willBeSent.boundingBoxes[y + x * 3]
											= cvRect(minX, minY, maxX - minX,
													maxY - minY);
								}

								cvSetImageROI(copyBgr, cvRect(minX, minY, maxX
										- minX, maxY - minY));
								if (cvGetImageROI(copyBgr).width == maxX - minX
										&& cvGetImageROI(copyBgr).height
												== maxY - minY) {

									if (x != 2 && setOrReset[x + 1][y] != 1) {
										cvCopy(copyBgr, images[y + x * 3]);
										if (!seenOrNot[y + x * 3])
											seenOrNot[y + x * 3] = true;
										//cvCopy(copyBgr, willBeSent.ROIs[y + x*3]);
									} else if (x == 2) {
										cvCopy(copyBgr, images[y + x * 3]);
										if (!seenOrNot[y + x * 3])
											seenOrNot[y + x * 3] = true;
									}
								}

								cvResetImageROI(copyBgr);

							}

						} else if (result == -1 || result == -2) {
							if (seenOrNot[y + x * 3]) {
								willBeSent.boundingBoxes[y + x * 3] = cvRect(0,
										0, 0, 0);
								willBeSent.segmented_areas[y + x * 3] = -1;
								cvClearHist(willBeSent.blueHists[y + x * 3]);
								cvClearHist(willBeSent.redHists[y + x * 3]);
								cvClearHist(willBeSent.greenHists[y + x * 3]);
								setOrReset[x][y] = -1;
								seenOrNot[y + x * 3] = false;
								if (images[y + x * 3] != NULL) {
									cvReleaseImage(&images[y + x * 3]);
									images[y + x * 3] = NULL;

								}
							}
						}
					}
				}
			}
			//cvReleaseImage(&copyBgr);
			//cvReleaseImage(&Image2);
			//cout<<"abbo"<<endl;
			//cout<<"8.5"<<endl;
			//igaze->getAngles(angles);
			//cout<<"8.6"<<endl;
			//cout<<angles[0]<<endl;
			//cout<<angles[1]<<endl;
			//cout<<angles[2]<<endl;
			cvCopyImage(bgr_frameLeft,tempImgForROI);
			//cout<<"8.7"<<endl;
			//Output Wireframe
			drawWireFrame(wireFrame, willBeFilled, angles[1], tempImgForROI);//,motion);
//			cvShowImage("wire",wireFrame);
//															cvWaitKey(0);
			imgOut.wrapIplImage(wireFrame);
			portImgOut->prepare() = imgOut;
			portImgOut->write();
			//cout<<"8.8"<<endl;
			
			//cout<<"red"<<endl;
			//for(int a = 0;a < 10;a++)
			//{
				//cout<<cvQueryHistValue_1D(willBeSent.redHists[4],a)<<",";
			//}
			//cout<<endl;
			
			//cout<<"blue"<<endl;
			//for(int a = 0;a < 10;a++)
			//{
				//cout<<cvQueryHistValue_1D(willBeSent.blueHists[4],a)<<",";
			//}
			//cout<<endl;
			
			//cout<<"green"<<endl;
			//for(int a = 0;a < 10;a++)
			//{
				//cout<<cvQueryHistValue_1D(willBeSent.greenHists[4],a)<<",";
			//}
			//cout<<willBeSent.segmented_areas[4]<<endl;
			
			//cout<<"Packet Information"<<endl;
			//				cout<<"Size of whole Image w:"<<willBeSent.raw_image->width<<" ,h:"<<willBeSent.raw_image->height<<endl;
			//				for(int x = 0;x<9;x++)
			//				{
			//					if(willBeSent != NULL)
			//					{
			//						if(willBeSent.ROIs[x] != NULL)
			//							{
			//								cout<<"ROI NO: "<<x<<" is found"<<endl;
			//								cout<<"Properties are:"<<endl;
			//								cout<<"x and y:"<<willBeSent.locationAndSize[x][0]<<", "<<willBeSent.locationAndSize[x][1]<<endl;
			//								cout<<"w and h:"<<willBeSent.locationAndSize[x][2]<<", "<<willBeSent.locationAndSize[x][3]<<endl;
			//								cout<<"size:"<<willBeSent.segmented_areas[x]<<endl;
			//
															//cout<<"bhist bin 0 :"<<cvQueryHistValue_1D(willBeSent.blueHists[x],0)<<endl;
															//cout<<"ghist bin 0:"<<cvQueryHistValue_1D(willBeSent.greenHists[x],0)<<endl;
															//cout<<"rhist bin 0:"<<cvQueryHistValue_1D(willBeSent.redHists[x],0)<<endl;
			//							}
			//					}
			//
			//				}
			//cout<<"patlaaar"<<endl;
			//PACKET WILL BE SENT FROM HERE
			//PACKET WILL BE SENT FROM HERE
			//PACKET WILL BE SENT FROM HERE
			//cvShowImage("Areas",tempImg);
			//cvWaitKey(0);
//			cvShowImage("deneme",tempImg);
//																		cvWaitKey(0);
			denemeOut.wrapIplImage(tempImg);
			portDenemeOut->prepare() = denemeOut;
			portDenemeOut->write();
			//cout<<"8.9"<<endl;

			//cvShowImage("Captures",tempImg1);
			//cvShowImage("Areas",tempImg1);
			//cvWaitKey(0);
//			cvShowImage("deneme1",tempImg1);
//																					cvWaitKey(0);
			denemeOut1.wrapIplImage(tempImg1);
			portDenemeOut1->prepare() = denemeOut1;
			portDenemeOut1->write();
			//cvShowImage("Captures", imgResult);
			//cout<<"9"<<endl;
			//			cvReleaseImage(&tempImg);
			//			cvReleaseImage(&imgResult);
			//src = (IplImage*)imgOut.getIplImage();
			//cvWaitKey(0);
		} else {
			//cout<<"burdayim gencler"<<endl;
			cvCopyImage(bgr_frameLeft,tempImgForROI);
			drawWireFrame(wireFrame, willBeFilled, angles[1], tempImgForROI);//,motion);

			//PACKET WILL BE SENT FROM HERE
			//PACKET WILL BE SENT FROM HERE
			//PACKET WILL BE SENT FROM HERE
			imgOut.wrapIplImage(wireFrame);
			portImgOut->prepare() = imgOut;
			portImgOut->write();
			cvCopyImage(bgr_frameLeft,tempImg);

			cvCopyImage(bgr_frameLeft,tempImg1);

			//cvShowImage("Points",tempImg);
			//cvWaitKey(0);
			denemeOut.wrapIplImage(tempImg);
			portDenemeOut->prepare() = denemeOut;
			portDenemeOut->write();
			//cout<<"8.9"<<endl;
			//cvShowImage("Areas",tempImg1);
			//cvWaitKey(0);
			denemeOut1.wrapIplImage(tempImg1);
			portDenemeOut1->prepare() = denemeOut1;
			portDenemeOut1->write();
		}
	}
}

//--Kadir

int main(int argc, char** argv) {

	ros::init(argc, argv, "tabletop_2D_segmentation");
	nh = new ros::NodeHandle();

	srv_perception_2D = nh->advertiseService(
			WORKSPACE_2D_SEGMENTATION_SRV_NAME, perception2DCallback);

	sub_exp_state_ = nh->subscribe<aff_msgs::ExperimentState> (
			"/experiment_state", 10, expStateCallback);

	pub_tabletop_2d_features_ = nh->advertise<aff_msgs::Features>("/tabletop_2D_features", 10);

	YARP_REGISTER_DEVICES(icubmod);

	pt1 = new CvPoint;
	pt2 = new CvPoint;
	bridge_ = new sensor_msgs::CvBridge();

	portImgOut = new BufferedPort<ImageOf<PixelBgr> > ;

	Network::init();

	Property optGaze("(device gazecontrollerclient)");
	optGaze.put("remote", "/iKinGazeCtrl");
	optGaze.put("local", "/gaze_client");

	clientGaze = new PolyDriver;
	if (!clientGaze->open(optGaze)) {
		delete clientGaze;
		return false;
	}

	// open the view
	clientGaze->view(igaze);
	//clientGaze->view(iposGaze);

	// put the gaze in tracking mode, so that
	// when the torso moves, the gaze controller
	// will compensate for it
	igaze->setTrackingMode(false);
	igaze->setEyesTrajTime(1);
	igaze->setNeckTrajTime(1.2);
	//	Property optTorso("(device remote_controlboard)");
	//	optTorso.put("remote","/icub/torso");
	//	optTorso.put("local","/torso_client");
	//
	//	clientTorso=new PolyDriver;
	//	if (!clientTorso->open(optTorso))
	//	{
	//		delete clientTorso;
	//		return false;
	//	}
	//
	//	// open the view
	//	clientTorso->view(ienc);
	//	clientTorso->view(ipos);


	//	Vector fp(3);
	//	fp[0]=-0.50;    // x-component [m]
	//	fp[1]=+0.00;    // y-component [m]
	//	fp[2]=+0.35;    // z-component [m]
	//
	//
	////	Vector fp(3);
	////		fp[0]=-0.50;    // x-component [m]
	////		fp[1]=+0.00;    // y-component [m]
	////		fp[2]=-0.10;    // z-component [m]
	//
	//	igaze->lookAtFixationPoint(fp);     // retrieve the current fixation point
	//	bool done = false;
	//	while (!done) {
	//	   igaze->checkMotionDone(&done);
	//	   Time::delay(0.04);   // or any suitable delay
	//	}
	//cout << "finish" << endl;
	//cout<<"x: "<<x[0]<<", y: "<<x[1]<<",z: "<<x[2]<<endl;

	motionSense.open("/ext_motion_detection:i");
	Network::connect("/ext_motion_detection:o", "/ext_motion_detection:i");

	portImgOut = new BufferedPort<ImageOf<PixelBgr> > ;
	portImgOut->open("/video/imageLeft/out");

	portDenemeOut = new BufferedPort<ImageOf<PixelBgr> > ;
	portDenemeOut->open("/video/imageDeneme/out");

	portDenemeOut1 = new BufferedPort<ImageOf<PixelBgr> > ;
	portDenemeOut1->open("/video/imageDeneme1/out");

	Network::connect("/video/imageLeft/out", "/ed");
	Network::connect("/video/imageDeneme/out", "/deneme");
	Network::connect("/video/imageDeneme1/out", "/deneme1");

	imagePortLeft.open("/video/imageLeft/in");
	Network::connect("/icub/camCalib/left/out", "/video/imageLeft/in");
	//Network::connect("/icubSim/cam/left","/video/imageLeft/in");
	//
	//	imagePortRight.open("/video/imageRight/in");
	//	Network::connect("/icub/camCalib/right/out","/video/imageRight/in");
	//cvNamedWindow("Captures", CV_WINDOW_AUTOSIZE);
	cout << "aaa" << endl;
	wireFramePoints[0].x = 280;
	wireFramePoints[0].y = 174;
	wireFramePoints[1].x = 527;
	wireFramePoints[1].y = 174;
	wireFramePoints[2].x = 780;
	wireFramePoints[2].y = 174;
	wireFramePoints[3].x = 254;
	wireFramePoints[3].y = 358;
	wireFramePoints[4].x = 525;
	wireFramePoints[4].y = 358;
	wireFramePoints[5].x = 788;
	wireFramePoints[5].y = 358;
	wireFramePoints[6].x = 217;
	wireFramePoints[6].y = 598;
	wireFramePoints[7].x = 515;
	wireFramePoints[7].y = 598;
	wireFramePoints[8].x = 825;
	wireFramePoints[8].y = 598;

	sprintf(positions[0][0], "Top Left Corner");
	sprintf(positions[0][1], "Top Left Side");
	sprintf(positions[0][2], "Top Right Side");
	sprintf(positions[0][3], "Top Right Corner");
	sprintf(positions[1][0], "Left Up Side");
	sprintf(positions[1][1], "Middle Up Left");
	sprintf(positions[1][2], "Middle Up Right");
	sprintf(positions[1][3], "Right Up Side");
	sprintf(positions[2][0], "Left Down Side");
	sprintf(positions[2][1], "Middle Down Left");
	sprintf(positions[2][2], "Middle Down Right");
	sprintf(positions[2][3], "Right Down Side");
	sprintf(positions[3][0], "Down Left Corner");
	sprintf(positions[3][1], "Down Left Side");
	sprintf(positions[3][2], "Down Right Side");
	sprintf(positions[3][3], "Down Right Corner");

	sprintf(areas[0][0], "Left Up Corner");
	sprintf(areas[0][1], "Top Middle");
	sprintf(areas[0][2], "Right Up Corner");
	sprintf(areas[1][0], "Left Side");
	sprintf(areas[1][1], "Middle");
	sprintf(areas[1][2], "Right Side");
	sprintf(areas[2][0], "Left Down Corner");
	sprintf(areas[2][1], "Down Middle");
	sprintf(areas[2][2], "Right Down Corner");

	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 0.5, CV_AA);
	imgResult = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
	tempImgG = cvCreateImage(cvGetSize(imgResult), IPL_DEPTH_8U, 1);
	tempImg = cvCreateImage(cvGetSize(imgResult), IPL_DEPTH_8U, 3);
	tempImg1 = cvCreateImage(cvGetSize(imgResult), IPL_DEPTH_8U, 3);
	tempImgForROI = cvCreateImage(cvGetSize(imgResult), IPL_DEPTH_8U, 3);
	wireFrame = cvCreateImage(cvSize(1024, 768), IPL_DEPTH_8U, 3);

	initializeTheGlobalVars();
	initializePacket(&willBeSent);
	initializePacket(&willBeSentOld);

	//	while (!first_srv_callback && nh->ok())
	//		ros::spinOnce();

	while (flag) {
		if (!nh->ok())
			break;
		segmentTabletopObjects();
		ros::spinOnce();
	}
}

float getEuclidianDistance(CvPoint2D32f p1, CvPoint2D32f p2) {
	return sqrt(pow((p1.y - p2.y), 2) + pow((p1.x - p2.x), 2));
}

double findDistanceToLine(CvPoint2D32f p1, CvPoint2D32f p2, CvPoint2D32f point) {
	double a = p2.y - p1.y;
	double b = p1.x - p2.x;
	double c = p1.y * p2.x - p2.y * p2.x + p2.x * p2.y - p2.y * p1.x;

	return abs(a * point.x + b * point.y + c) / sqrt(pow(a, 2) + pow(b, 2));
}

int findExtensionColor(CvPoint2D32f source, CvPoint2D32f destination,
		IplImage* image) {

	CvPoint2D32f newPoint;
	//cout<<"source x and y: "<<source.x<<", "<<source.y<<endl;
	//cout<<"destination x and y: "<<destination.x<<", "<<destination.y<<endl;
	double a = source.y - destination.y;
	double b = destination.x - source.x;
	double c = destination.y * source.x - source.y * source.x + source.x
			* source.y - source.y * destination.x;
	double xNew;
	if (destination.x > source.x) {
		xNew = destination.x +  (destination.x - source.x) / 2;//2x/5
		if (xNew < 640) {
			newPoint.x = xNew;
			//cout<<"Xnew: "<<xNew<<endl;
		} else {
			//cout<<"xNew disarida"<<endl;
			return -2;
		}

	} else {
		xNew = destination.x - (source.x - destination.x) / 2; //2x/5
		if (xNew > 0) {
			newPoint.x = xNew;
			//cout<<"Xnew: "<<xNew<<endl;
		} else {
			//cout<<"xNew disarida"<<endl;
			return -2;
		}

	}

	newPoint.y = (-1 * c - a * xNew) / b;
	if (newPoint.y < 480 && newPoint.y >= 0)
	{
//		cvCopyImage(copyOfGlobBgr,tempImg);
//		cvCircle(tempImg, cvPoint(source.x, source.y), 4,cvScalar(0, 255, 0), -1, 8);
//		cvCircle(tempImg, cvPoint(destination.x, destination.y), 4,cvScalar(255, 255, 0), -1, 8);
//		cvCircle(tempImg, cvPoint(newPoint.x, newPoint.y), 4,cvScalar(0, 0, 255), -1, 8);
//		cvShowImage("4neigh",tempImg);
//		cvWaitKey(0);
		return findEdgeColorForIdentification(destination, newPoint, image);
	}
	else {
		//cout<<"yNew disarida"<<endl;
		return -2;
	}
}

int findEdgeColorForVerification(CvPoint2D32f p1, CvPoint2D32f p2,
		IplImage* image) {
	int interval = 100;
	int counter = 0;
	double deltaX = (p2.x - p1.x) / interval;
	double deltaY = (p2.y - p1.y) / interval;
	CvScalar ptr;
	for (int i = 0; i < interval; i++) {
		if (p1.y + i * deltaY < image->height && p1.x + i * deltaX
				< image->width) {
			if (p1.y + i * deltaY < image->height && p1.y + i * deltaY > 0
					&& p1.x + i * deltaX < image->width && p1.x + i * deltaX
					> 0) {
				ptr = cvGet2D(image, p1.y + i * deltaY, p1.x + i * deltaX);
				if (ptr.val[0] == 255.0) {

					counter++;
				}
			}

		}

	}
	if (interval - counter < 10)
		return 1;
	else
		return 0;

}

int findEdgeColorForIdentification(CvPoint2D32f p1, CvPoint2D32f p2,
		IplImage* image) {
	int interval = 100;
	int counter = 0;
	//int white = false;
	double deltaX = (p2.x - p1.x) / interval;
	double deltaY = (p2.y - p1.y) / interval;
	CvScalar ptr;
	for (int i = 0; i < interval; i++) {
		if (p1.y + i * deltaY < image->height && p1.x + i * deltaX
				< image->width) {
			if (p1.y + i * deltaY < image->height && p1.y + i * deltaY > 0
					&& p1.x + i * deltaX < image->width && p1.x + i * deltaX
					> 0) {
				ptr = cvGet2D(image, p1.y + i * deltaY, p1.x + i * deltaX);
				//cout<<p1.y + i * deltaY<<"   "<<p1.x + i * deltaX<<endl;
				if (ptr.val[0] == 255.0) {

					counter++;
				}
			}

		}

	}
	//cout<<"interval - counter = "<<interval - counter<<endl;
	int noOfBlack = interval - counter;
	int noOfWhite = counter;
	//cout<<"noOfBlack"<<noOfBlack<<endl;
	//cout<<"noOfWhite"<<noOfWhite<<endl;
	if (noOfWhite > 70)//70
		return 1;
	else if (noOfBlack > 70)//70
		return 0;
	else
		return -1;
}

// 0->left, 1->down, 2->right, 3->up
int* determineWays(CvPoint2D32f* singleCorners, int indexAndDegrees[][9],
		int index) {
	int* ways = new int[4];
	double distance = 0;
	double minHor = 10000;
	int minHorIndex = -1;
	double minVer = 10000;
	int maxHorIndex = -1;
	double maxHor = 0;
	int maxVerIndex = -1;
	double maxVer = 0;
	int minVerIndex = -1;

	for (int x = 0; x < indexAndDegrees[index][0]; x++) {
		distance = singleCorners[indexAndDegrees[index][2 * x + 1]].y
				- singleCorners[index].y;
		if (distance > maxVer) {
			maxVer = distance;
			maxVerIndex = indexAndDegrees[index][2 * x + 1];
			//universalDownDeg = indexAndDegrees[index][2 * x + 2];
		}
		if (distance < minVer) {
			minVer = distance;
			minVerIndex = indexAndDegrees[index][2 * x + 1];
			//universalUpDeg = indexAndDegrees[index][2 * x + 2];
		}

	}
	ways[1] = maxVerIndex;
	ways[3] = minVerIndex;
	for (int x = 0; x < indexAndDegrees[index][0]; x++) {
		distance = singleCorners[indexAndDegrees[index][2 * x + 1]].x
				- singleCorners[index].x;
		if (distance > maxHor) {
			maxHor = distance;
			maxHorIndex = indexAndDegrees[index][2 * x + 1];
			//universalRightDeg = indexAndDegrees[index][2 * x + 2];
		}
		if (distance < minHor) {
			minHor = distance;
			minHorIndex = indexAndDegrees[index][2 * x + 1];
			//universalLeftDeg = indexAndDegrees[index][2 * x + 2];
		}

	}
	ways[0] = minHorIndex;
	ways[2] = maxHorIndex;
	return ways;
}

void tagMainCorner(bool selections[], int verhor[], int x, int* ways) {
	if (selections[0] && selections[1]) {

		identificationIndex[x][0] = 2 - verhor[0];
		identificationIndex[x][1] = 1 + verhor[1];

	} else if (selections[1] && selections[2]) {
		identificationIndex[x][0] = 2 - verhor[0];
		identificationIndex[x][1] = 2 - verhor[1];
	} else if (selections[2] && selections[3]) {
		identificationIndex[x][0] = 1 + verhor[0];
		identificationIndex[x][1] = 2 - verhor[1];
	} else if (selections[3] && selections[0]) {
		identificationIndex[x][0] = 1 + verhor[0];
		identificationIndex[x][1] = 1 + verhor[1];
	}
	identified[x] = true;
	willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]] = true;
	places[identificationIndex[x][0]][identificationIndex[x][1]]
			= singleCorners[x];
	countForIdentified++;
	//cout<<"Ana koordinatlari x: "<<identificationIndex[x][0]<<", y: "<<identificationIndex[x][1]<<endl;
	//cvWaitKey(0);
	//cout<<"TAGGED AS: "<<endl;
	tagOtherCorners(ways, x);

}

void tagOtherCorners(int* ways, int x) {

	if (ways[0] != -1) {
		identificationIndex[ways[0]][0] = identificationIndex[x][0];
		identificationIndex[ways[0]][1] = identificationIndex[x][1] - 1;
	}
	if (ways[1] != -1) {
		identificationIndex[ways[1]][0] = identificationIndex[x][0] + 1;
		identificationIndex[ways[1]][1] = identificationIndex[x][1];
	}
	if (ways[2] != -1) {
		identificationIndex[ways[2]][0] = identificationIndex[x][0];
		identificationIndex[ways[2]][1] = identificationIndex[x][1] + 1;
	}

	if (ways[3] != -1) {
		identificationIndex[ways[3]][0] = identificationIndex[x][0] - 1;
		identificationIndex[ways[3]][1] = identificationIndex[x][1];
	}

	for (int a = 0; a < 4; a++) {

		if (ways[a] != -1 && !identified[ways[a]]
				&& identificationIndex[ways[a]][0] >= 0
				&& identificationIndex[ways[a]][0] < 4
				&& identificationIndex[ways[a]][1] >= 0
				&& identificationIndex[ways[a]][1] < 4) {
			identified[ways[a]] = true;
			//cout<<"ways["<<a<<"]:"<<endl;
			//cout<<"koordinatlari x: "<<identificationIndex[ways[a]][0]<<", y: "<<identificationIndex[ways[a]][1]<<endl;
			willBeFilled[identificationIndex[ways[a]][0]][identificationIndex[ways[a]][1]]
					= true;
			places[identificationIndex[ways[a]][0]][identificationIndex[ways[a]][1]]
					= singleCorners[ways[a]];
			countForIdentified++;
		}

	}

}

void drawWireFrame(IplImage *image, bool willBeFilled[][4], float aboveLimit,
		IplImage *colourImage)//,int motion)
{
//	cout<<"Wireframe Started"<<endl;
//	for(int x = 0;x < 3;x++)
//	{
//		for(int y = 0;y < 3;y++)
//		{
//			cout<<willBeFilled[x][y]<<""<<endl;
//		}
//		cout<<endl;
//	}
//	cout<<"Wireframe Finished"<<endl;
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 0.5, CV_AA);
	cvZero(image);
	cvDrawLine(image, cvPoint(175, 101), cvPoint(875, 101), cvScalar(255, 255,
			255), 2);
	cvDrawLine(image, cvPoint(875, 101), cvPoint(990, 733), cvScalar(255, 255,
			255), 2);
	cvDrawLine(image, cvPoint(990, 733), cvPoint(37, 733), cvScalar(255, 255,
			255), 2);
	cvDrawLine(image, cvPoint(37, 733), cvPoint(175, 101), cvScalar(255, 255,
			255), 2);
	cvDrawLine(image, cvPoint(142, 254), cvPoint(902, 254), cvScalar(255, 255,
			255), 2);
	cvDrawLine(image, cvPoint(95, 470), cvPoint(941, 470), cvScalar(255, 255,
			255), 2);
	cvDrawLine(image, cvPoint(399, 101), cvPoint(344, 733), cvScalar(255, 255,
			255), 2);
	cvDrawLine(image, cvPoint(649, 101), cvPoint(681, 733), cvScalar(255, 255,
			255), 2);
	//cvPutText(image, "Face Not Detected", cvPoint(10,30), &font, cvScalar(0, 255, 0,0));

	//cout<<aboveLimit<<endl;
	if (aboveLimit < -26) {
		//cout<<"not in"<<endl;
		if (detectedFace == true) {

			//cvPutText(image, "Face Detected", cvPoint(10,30), &font, cvScalar(0, 255, 0,0));
			cvOverlayImage(image, destination, cvPoint(image->width / 2
					- destination->width / 2, 0), true);
		} else
			cvPutText(image, "Face Not Detected", cvPoint(10, 30), &font,
					cvScalar(0, 255, 0, 0));

		//		for (int x = 0; x < 3; x++) {
		//			for (int y = 0; y < 3; y++) {
		//				cout<<setOrReset[x][y]<<" ";
		//			}
		//			cout<<endl;
		//		}
		int numBins = N_BINS;
		float range[] = { 0, 255 };
		float *ranges[] = { range };

		for (int x = 0; x < 3; x++) {
			for (int y = 0; y < 3; y++) {
				//if(y + x * 3 == 4)
							//	cvSaveImage("/home/kadir/Desktop/segmented.jpg",images[y + x * 3]);
				if ((willBeFilled[x][y] && willBeFilled[x + 1][y + 1])
						|| seenOrNot[y + x * 3]) {
					if (images[y + x * 3] != NULL && (setOrReset[x][y] == 1
							|| setOrReset[x][y] == 0)) {
						
						int size = cvOverlayImage(image, images[y + x * 3],
								wireFramePoints[y + x * 3], false);
						
						if (x != 2 && setOrReset[x + 1][y] != 1) {

							willBeSent.segmented_areas[y + x * 3] = size;

							//							willBeSent.blueHists[y + x * 3] = cvCreateHist(1,
							//									&numBins, CV_HIST_ARRAY, ranges, 1);
							//							willBeSent.redHists[y + x * 3] = cvCreateHist(1,
							//									&numBins, CV_HIST_ARRAY, ranges, 1);
							//							willBeSent.greenHists[y + x * 3] = cvCreateHist(1,
							//									&numBins, CV_HIST_ARRAY, ranges, 1);
							cvClearHist(willBeSent.blueHists[y + x * 3]);
							cvClearHist(willBeSent.redHists[y + x * 3]);
							cvClearHist(willBeSent.greenHists[y + x * 3]);
							imgRed = cvCreateImage(
									cvGetSize(images[y + x * 3]), 8, 1);
							imgGreen = cvCreateImage(cvGetSize(
									images[y + x * 3]), 8, 1);
							imgBlue = cvCreateImage(
									cvGetSize(images[y + x * 3]), 8, 1);

							cvSplit(images[y + x * 3], imgBlue, imgGreen,
									imgRed, NULL);

							cvCalcHist(&imgRed, willBeSent.redHists[y + x * 3],
									0, 0);
							cvCalcHist(&imgGreen, willBeSent.greenHists[y + x
									* 3], 0, 0);
							cvCalcHist(&imgBlue,
									willBeSent.blueHists[y + x * 3], 0, 0);

							cvReleaseImage(&imgRed);
							cvReleaseImage(&imgGreen);
							cvReleaseImage(&imgBlue);
							imgRed = NULL;
							imgGreen = NULL;
							imgBlue = NULL;

						} else if (x == 2) {
							willBeSent.segmented_areas[y + x * 3] = size;

							//							willBeSent.blueHists[y + x * 3] = cvCreateHist(1,
							//									&numBins, CV_HIST_ARRAY, ranges, 1);
							//							willBeSent.redHists[y + x * 3] = cvCreateHist(1,
							//									&numBins, CV_HIST_ARRAY, ranges, 1);
							//							willBeSent.greenHists[y + x * 3] = cvCreateHist(1,
							//									&numBins, CV_HIST_ARRAY, ranges, 1);
							cvClearHist(willBeSent.blueHists[y + x * 3]);
							cvClearHist(willBeSent.redHists[y + x * 3]);
							cvClearHist(willBeSent.greenHists[y + x * 3]);
							imgRed = cvCreateImage(
									cvGetSize(images[y + x * 3]), 8, 1);
							imgGreen = cvCreateImage(cvGetSize(
									images[y + x * 3]), 8, 1);
							imgBlue = cvCreateImage(
									cvGetSize(images[y + x * 3]), 8, 1);

							cvSplit(images[y + x * 3], imgBlue, imgGreen,
									imgRed, NULL);

							cvCalcHist(&imgRed, willBeSent.redHists[y + x * 3],
									0, 0);
							cvCalcHist(&imgGreen, willBeSent.greenHists[y + x
									* 3], 0, 0);
							cvCalcHist(&imgBlue,
									willBeSent.blueHists[y + x * 3], 0, 0);

							cvReleaseImage(&imgRed);
							cvReleaseImage(&imgGreen);
							cvReleaseImage(&imgBlue);
							imgRed = NULL;
							imgGreen = NULL;
							imgBlue = NULL;

						}

					}

				} else if ((willBeFilled[x][y + 1] && willBeFilled[x + 1][y])
						|| seenOrNot[y + x * 3]) {
					cout<<"abov"<<endl;
					if (images[y + x * 3] != NULL && (setOrReset[x][y] == 1
							|| setOrReset[x][y] == 0)) {
						cout<<"abov1"<<endl;
						int size = cvOverlayImage(image, images[y + x * 3],
								wireFramePoints[y + x * 3], false);
						if (x != 2 && setOrReset[x + 1][y] != 1) {

							willBeSent.segmented_areas[y + x * 3] = size;

							//							willBeSent.blueHists[y + x * 3] = cvCreateHist(1,
							//									&numBins, CV_HIST_ARRAY, ranges, 1);
							//							willBeSent.redHists[y + x * 3] = cvCreateHist(1,
							//									&numBins, CV_HIST_ARRAY, ranges, 1);
							//							willBeSent.greenHists[y + x * 3] = cvCreateHist(1,
							//									&numBins, CV_HIST_ARRAY, ranges, 1);
							cvClearHist(willBeSent.blueHists[y + x * 3]);
							cvClearHist(willBeSent.redHists[y + x * 3]);
							cvClearHist(willBeSent.greenHists[y + x * 3]);
							imgRed = cvCreateImage(
									cvGetSize(images[y + x * 3]), 8, 1);
							imgGreen = cvCreateImage(cvGetSize(
									images[y + x * 3]), 8, 1);
							imgBlue = cvCreateImage(
									cvGetSize(images[y + x * 3]), 8, 1);

							cvSplit(images[y + x * 3], imgBlue, imgGreen,
									imgRed, NULL);

							cvCalcHist(&imgRed, willBeSent.redHists[y + x * 3],
									0, 0);
							cvCalcHist(&imgGreen, willBeSent.greenHists[y + x
									* 3], 0, 0);
							cvCalcHist(&imgBlue,
									willBeSent.blueHists[y + x * 3], 0, 0);

							cvReleaseImage(&imgRed);
							cvReleaseImage(&imgGreen);
							cvReleaseImage(&imgBlue);
							imgRed = NULL;
							imgGreen = NULL;
							imgBlue = NULL;

						} else if (x == 2) {
							willBeSent.segmented_areas[y + x * 3] = size;

							//							willBeSent.blueHists[y + x * 3] = cvCreateHist(1,
							//									&numBins, CV_HIST_ARRAY, ranges, 1);
							//							willBeSent.redHists[y + x * 3] = cvCreateHist(1,
							//									&numBins, CV_HIST_ARRAY, ranges, 1);
							//							willBeSent.greenHists[y + x * 3] = cvCreateHist(1,
							//									&numBins, CV_HIST_ARRAY, ranges, 1);
							cvClearHist(willBeSent.blueHists[y + x * 3]);
							cvClearHist(willBeSent.redHists[y + x * 3]);
							cvClearHist(willBeSent.greenHists[y + x * 3]);
							imgRed = cvCreateImage(
									cvGetSize(images[y + x * 3]), 8, 1);
							imgGreen = cvCreateImage(cvGetSize(
									images[y + x * 3]), 8, 1);
							imgBlue = cvCreateImage(
									cvGetSize(images[y + x * 3]), 8, 1);

							cvSplit(images[y + x * 3], imgBlue, imgGreen,
									imgRed, NULL);

							cvCalcHist(&imgRed, willBeSent.redHists[y + x * 3],
									0, 0);
							cvCalcHist(&imgGreen, willBeSent.greenHists[y + x
									* 3], 0, 0);
							cvCalcHist(&imgBlue,
									willBeSent.blueHists[y + x * 3], 0, 0);

							cvReleaseImage(&imgRed);
							cvReleaseImage(&imgGreen);
							cvReleaseImage(&imgBlue);
							imgRed = NULL;
							imgGreen = NULL;
							imgBlue = NULL;

						}

					}

				}

			}
		}
		//cvShowImage("WireFrame",image);
		//cvWaitKey(0);
	} else if (aboveLimit > -26 && detect(colourImage, pt1, pt2)) {
		cout << "detected" << endl;
		//cout<<"10"<<endl;
		//cout<<r->width<<endl;
		if (pt2->x > 0 && pt2->y > 0) {
			if (ROI != NULL) {
				cvReleaseImage(&ROI);
				ROI = NULL;
			}
			ROI = cvCreateImage(cvSize(pt2->x, pt2->y), IPL_DEPTH_8U, 3);
			//cout<<"1a"<<endl;
			for (int a = 0; a < ROI->width; a++) {

				for (int b = 0; b < ROI->height; b++) {
					//cout<<"For ROI"<<a<<" "<<b<<endl;
					//cout<<"For BGR"<<places[x+1][y].x + a<<" "<<places[x][y+1].y + b<<endl;
					if (pt1->y + b < colourImage->height && pt1->y + b > 0
							&& pt1->x + a < colourImage->width && pt1->x + a
							> 0) {
						CvScalar over = cvGet2D(colourImage, pt1->y + b, pt1->x
								+ a);
						if (b < ROI->height && b > 0 && a < ROI->width && a > 0) {
							cvSet2D(ROI, b, a, over);
						}
					}
				}
			}
			//cvSetImageROI(colourImage, cvRect(10,30, pt2->x,pt2->y));
			cout << "1b" << endl;
			//cvCopy(colourImage, ROI);
			//cout<<"1c"<<endl;
			//cvResetImageROI(colourImage);
			//cout<<"1d"<<endl;
			//cvPutText(image, "Face Detected", cvPoint(10,30), &font, cvScalar(0, 255, 0,0));
			if ((int) ((ROI->width * 50) / 100) > 0
					&& (int) ((ROI->height * 50) / 100) > 0) {
				if (destination != NULL) {
					cvReleaseImage(&destination);
					destination = NULL;
				}
				destination = cvCreateImage(cvSize(75, 75), ROI->depth,
						ROI->nChannels);
				cvResize(ROI, destination);
				cvOverlayImage(image, destination, cvPoint(image->width / 2
						- destination->width / 2, 0), true);
				detectedFace = true;

				//cvReleaseImage(&destination);
			}
		}
		cout << "bb" << endl;
		int numBins = N_BINS;
		float range[] = { 0, 255 };
		float *ranges[] = { range };
		if (willBeFilled != NULL) {
			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {

					if ((willBeFilled[x][y] && willBeFilled[x + 1][y + 1])
							|| seenOrNot[y + x * 3]) {
						if (images[y + x * 3] != NULL && (setOrReset[x][y] == 1
								|| setOrReset[x][y] == 0)) {
							int size = cvOverlayImage(image, images[y + x * 3],
									wireFramePoints[y + x * 3], false);
							if (x != 2 && setOrReset[x + 1][y] != 1) {
								willBeSent.segmented_areas[y + x * 3] = size;

								//							willBeSent.blueHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.redHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.greenHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								cvClearHist(willBeSent.blueHists[y + x * 3]);
								cvClearHist(willBeSent.redHists[y + x * 3]);
								cvClearHist(willBeSent.greenHists[y + x * 3]);
								imgRed = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgGreen = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgBlue = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);

								cvSplit(images[y + x * 3], imgBlue, imgGreen,
										imgRed, NULL);

								cvCalcHist(&imgRed, willBeSent.redHists[y + x
										* 3], 0, 0);
								cvCalcHist(&imgGreen, willBeSent.greenHists[y
										+ x * 3], 0, 0);
								cvCalcHist(&imgBlue, willBeSent.blueHists[y + x
										* 3], 0, 0);

								cvReleaseImage(&imgRed);
								cvReleaseImage(&imgGreen);
								cvReleaseImage(&imgBlue);
								imgRed = NULL;
								imgGreen = NULL;
								imgBlue = NULL;

							} else if (x == 2) {
								willBeSent.segmented_areas[y + x * 3] = size;

								//							willBeSent.blueHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.redHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.greenHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								cvClearHist(willBeSent.blueHists[y + x * 3]);
								cvClearHist(willBeSent.redHists[y + x * 3]);
								cvClearHist(willBeSent.greenHists[y + x * 3]);
								imgRed = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgGreen = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgBlue = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);

								cvSplit(images[y + x * 3], imgBlue, imgGreen,
										imgRed, NULL);

								cvCalcHist(&imgRed, willBeSent.redHists[y + x
										* 3], 0, 0);
								cvCalcHist(&imgGreen, willBeSent.greenHists[y
										+ x * 3], 0, 0);
								cvCalcHist(&imgBlue, willBeSent.blueHists[y + x
										* 3], 0, 0);

								cvReleaseImage(&imgRed);
								cvReleaseImage(&imgGreen);
								cvReleaseImage(&imgBlue);
								imgRed = NULL;
								imgGreen = NULL;
								imgBlue = NULL;

							}

						}

					} else if ((willBeFilled[x][y + 1]
							&& willBeFilled[x + 1][y]) || seenOrNot[y + x * 3]) {
						if (images[y + x * 3] != NULL && (setOrReset[x][y] == 1
								|| setOrReset[x][y] == 0)) {
							int size = cvOverlayImage(image, images[y + x * 3],
									wireFramePoints[y + x * 3], false);
							if (x != 2 && setOrReset[x + 1][y] != 1) {
								willBeSent.segmented_areas[y + x * 3] = size;

								//							willBeSent.blueHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.redHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.greenHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								cvClearHist(willBeSent.blueHists[y + x * 3]);
								cvClearHist(willBeSent.redHists[y + x * 3]);
								cvClearHist(willBeSent.greenHists[y + x * 3]);
								imgRed = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgGreen = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgBlue = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);

								cvSplit(images[y + x * 3], imgBlue, imgGreen,
										imgRed, NULL);

								cvCalcHist(&imgRed, willBeSent.redHists[y + x
										* 3], 0, 0);
								cvCalcHist(&imgGreen, willBeSent.greenHists[y
										+ x * 3], 0, 0);
								cvCalcHist(&imgBlue, willBeSent.blueHists[y + x
										* 3], 0, 0);

								cvReleaseImage(&imgRed);
								cvReleaseImage(&imgGreen);
								cvReleaseImage(&imgBlue);
								imgRed = NULL;
								imgGreen = NULL;
								imgBlue = NULL;

							} else if (x == 2) {
								willBeSent.segmented_areas[y + x * 3] = size;

								//							willBeSent.blueHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.redHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.greenHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								cvClearHist(willBeSent.blueHists[y + x * 3]);
								cvClearHist(willBeSent.redHists[y + x * 3]);
								cvClearHist(willBeSent.greenHists[y + x * 3]);
								imgRed = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgGreen = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgBlue = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);

								cvSplit(images[y + x * 3], imgBlue, imgGreen,
										imgRed, NULL);

								cvCalcHist(&imgRed, willBeSent.redHists[y + x
										* 3], 0, 0);
								cvCalcHist(&imgGreen, willBeSent.greenHists[y
										+ x * 3], 0, 0);
								cvCalcHist(&imgBlue, willBeSent.blueHists[y + x
										* 3], 0, 0);

								cvReleaseImage(&imgRed);
								cvReleaseImage(&imgGreen);
								cvReleaseImage(&imgBlue);
								imgRed = NULL;
								imgGreen = NULL;
								imgBlue = NULL;

							}

						}

					}

				}
			}
		}

		//cvShowImage("WireFrame",image);
		//cvWaitKey(0);

	} else if (aboveLimit > -26 && !detect(colourImage, pt1, pt2)) {
		cout << "not detected" << endl;
		pt1->x = -1;
		pt1->y = -1;
		pt2->x = -1;
		pt2->y = -1;
		cvPutText(image, "Face Not Detected", cvPoint(10, 30), &font, cvScalar(
				0, 255, 0, 0));
		detectedFace = false;
		int numBins = N_BINS;
		float range[] = { 0, 255 };
		float *ranges[] = { range };

		if (willBeFilled != NULL) {
			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {

					if ((willBeFilled[x][y] && willBeFilled[x + 1][y + 1])
							|| seenOrNot[y + x * 3]) {
						if (images[y + x * 3] != NULL && (setOrReset[x][y] == 1
								|| setOrReset[x][y] == 0)) {
							int size = cvOverlayImage(image, images[y + x * 3],
									wireFramePoints[y + x * 3], false);
							if (x != 2 && setOrReset[x + 1][y] != 1) {
								willBeSent.segmented_areas[y + x * 3] = size;

								//							willBeSent.blueHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.redHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.greenHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								cvClearHist(willBeSent.blueHists[y + x * 3]);
								cvClearHist(willBeSent.redHists[y + x * 3]);
								cvClearHist(willBeSent.greenHists[y + x * 3]);
								imgRed = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgGreen = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgBlue = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);

								cvSplit(images[y + x * 3], imgBlue, imgGreen,
										imgRed, NULL);

								cvCalcHist(&imgRed, willBeSent.redHists[y + x
										* 3], 0, 0);
								cvCalcHist(&imgGreen, willBeSent.greenHists[y
										+ x * 3], 0, 0);
								cvCalcHist(&imgBlue, willBeSent.blueHists[y + x
										* 3], 0, 0);

								cvReleaseImage(&imgRed);
								cvReleaseImage(&imgGreen);
								cvReleaseImage(&imgBlue);
								imgRed = NULL;
								imgGreen = NULL;
								imgBlue = NULL;

							} else if (x == 2) {
								willBeSent.segmented_areas[y + x * 3] = size;

								//							willBeSent.blueHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.redHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.greenHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								cvClearHist(willBeSent.blueHists[y + x * 3]);
								cvClearHist(willBeSent.redHists[y + x * 3]);
								cvClearHist(willBeSent.greenHists[y + x * 3]);
								imgRed = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgGreen = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgBlue = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);

								cvSplit(images[y + x * 3], imgBlue, imgGreen,
										imgRed, NULL);

								cvCalcHist(&imgRed, willBeSent.redHists[y + x
										* 3], 0, 0);
								cvCalcHist(&imgGreen, willBeSent.greenHists[y
										+ x * 3], 0, 0);
								cvCalcHist(&imgBlue, willBeSent.blueHists[y + x
										* 3], 0, 0);

								cvReleaseImage(&imgRed);
								cvReleaseImage(&imgGreen);
								cvReleaseImage(&imgBlue);
								imgRed = NULL;
								imgGreen = NULL;
								imgBlue = NULL;

							}

						}

					} else if ((willBeFilled[x][y + 1]
							&& willBeFilled[x + 1][y]) || seenOrNot[y + x * 3]) {
						if (images[y + x * 3] != NULL && (setOrReset[x][y] == 1
								|| setOrReset[x][y] == 0)) {
							int size = cvOverlayImage(image, images[y + x * 3],
									wireFramePoints[y + x * 3], false);
							if (x != 2 && setOrReset[x + 1][y] != 1) {
								willBeSent.segmented_areas[y + x * 3] = size;

								//							willBeSent.blueHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.redHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.greenHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								cvClearHist(willBeSent.blueHists[y + x * 3]);
								cvClearHist(willBeSent.redHists[y + x * 3]);
								cvClearHist(willBeSent.greenHists[y + x * 3]);
								imgRed = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgGreen = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgBlue = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);

								cvSplit(images[y + x * 3], imgBlue, imgGreen,
										imgRed, NULL);

								cvCalcHist(&imgRed, willBeSent.redHists[y + x
										* 3], 0, 0);
								cvCalcHist(&imgGreen, willBeSent.greenHists[y
										+ x * 3], 0, 0);
								cvCalcHist(&imgBlue, willBeSent.blueHists[y + x
										* 3], 0, 0);

								cvReleaseImage(&imgRed);
								cvReleaseImage(&imgGreen);
								cvReleaseImage(&imgBlue);
								imgRed = NULL;
								imgGreen = NULL;
								imgBlue = NULL;

							} else if (x == 2) {
								willBeSent.segmented_areas[y + x * 3] = size;

								//							willBeSent.blueHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.redHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								//							willBeSent.greenHists[y + x * 3] = cvCreateHist(1,
								//									&numBins, CV_HIST_ARRAY, ranges, 1);
								cvClearHist(willBeSent.blueHists[y + x * 3]);
								cvClearHist(willBeSent.redHists[y + x * 3]);
								cvClearHist(willBeSent.greenHists[y + x * 3]);
								imgRed = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgGreen = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);
								imgBlue = cvCreateImage(cvGetSize(images[y + x
										* 3]), 8, 1);

								cvSplit(images[y + x * 3], imgBlue, imgGreen,
										imgRed, NULL);

								cvCalcHist(&imgRed, willBeSent.redHists[y + x
										* 3], 0, 0);
								cvCalcHist(&imgGreen, willBeSent.greenHists[y
										+ x * 3], 0, 0);
								cvCalcHist(&imgBlue, willBeSent.blueHists[y + x
										* 3], 0, 0);

								cvReleaseImage(&imgRed);
								cvReleaseImage(&imgGreen);
								cvReleaseImage(&imgBlue);
								imgRed = NULL;
								imgGreen = NULL;
								imgBlue = NULL;

							}

						}

					}

				}
			}
		}

		//cvShowImage("WireFrame",image);
		//cvWaitKey(0);
	}

	willBeSent.faceDetected = detectedFace;
}

//Function to detect and draw any faces that is present in an image
bool detect(IplImage* img, CvPoint* pt1, CvPoint *pt2) {
	bool detected = false;

	// Allocate the memory storage
	storage = cvCreateMemStorage(0);

	// Load the HaarClassifierCascade
	cascade = (CvHaarClassifierCascade*) cvLoad(cascade_name, 0, 0, 0);
	// Create two points to represent the face locations
	int i;

	// Clear the memory storage which was used before
	cvClearMemStorage(storage);
	// Find whether the cascade is loaded, to find the faces. If yes, then:
	if (cascade) {

		// There can be more than one face in an image. So create a growable sequence of faces.
		// Detect the objects and store them in the sequence
		CvSeq* faces = cvHaarDetectObjects(img, cascade, storage, 1.1, 3,
				CV_HAAR_DO_CANNY_PRUNING, cvSize(10, 10));

		// Loop the number of faces found.
		for (i = 0; i < faces->total; i++) {
			// Create a new rectangle for drawing the face
			CvRect* r = (CvRect*) cvGetSeqElem(faces, i);

			detected = true;

			// Find the dimensions of the face,and scale it if necessary
			pt1->x = r->x;
			pt1->y = r->y;
			pt2->x = r->width;
			pt2->y = r->height;

			//cvRectangle( img, pt2, pt1, CV_RGB(255,255,255), 3, 8, 0 );
			r = NULL;

		}
		faces = NULL;
	}
	return detected;

}

int cvOverlayImage(IplImage* src, IplImage* overlay, CvPoint location,
		bool flag) {
	int sizeOfObject = 0;
	for (int x = 0; x < overlay->width; x++) {

		for (int y = 0; y < overlay->height; y++) {

			if (y < overlay->height && y > 0 && x < overlay->width && x > 0) {
				CvScalar over = cvGet2D(overlay, y, x);
				if (y + location.y < src->height && y + location.y > 0 && x
						+ location.x < src->width && x + location.x > 0 && flag) {
					cvSet2D(src, y + location.y, x + location.x, over);
				} else if (y + location.y < src->height && y + location.y > 0
						&& x + location.x < src->width && x + location.x > 0
						&& !flag) {

					if (over.val[0] > 5.0 && over.val[0] < 250.0) {
						cvSet2D(src, y + location.y - overlay->height / 2, x
								+ location.x - overlay->width / 2, over);
						sizeOfObject++;
					}

				}
			}
			//cvSet2D(src, y+location.y - overlay->height/2, x+location.x- overlay->width/2, over);
		}
	}

	return sizeOfObject;
}

int givePercentage(IplImage* overlay) {

	int counterForBlack = 0;
	for (int x = 0; x < overlay->width; x++) {

		for (int y = 0; y < overlay->height; y++) {

			if (y < overlay->height && y > 0 && x < overlay->width && x > 0) {

				CvScalar over = cvGet2D(overlay, y, x);
				if (over.val[0] < 5) {
					counterForBlack++;
				}

			}
			//cvSet2D(src, y+location.y - overlay->height/2, x+location.x- overlay->width/2, over);
		}
	}
	double percentage = (counterForBlack * 100) / (overlay->width
			* overlay->height);

	return (int) percentage;

}

int floodFill(int a, int b, int *minX, int *minY, int *maxX, int *maxY) {

	if ((a < copyBgr->width - 2) && (a > 1) && (b < copyBgr->height - 2) && (b
			> 1)) {
		//cout<<"aa b"<<endl;
		CvScalar color1 = cvGet2D(copyBgr, b, a);
		//cout<<"aa a"<<endl;
		//cout<<"hop10.5"<<endl;
		if ((color1.val[0] != 255 || color1.val[1] != 255 || color1.val[2]
				!= 255)) {
			if (maskImage != NULL) {
				cvReleaseImage(&maskImage);
				maskImage = NULL;
			}
			maskImage = cvCreateImage(cvSize(copyBgr->width + 2,
					copyBgr->height + 2), copyBgr->depth, 1);
			cvZero(maskImage);
			cvFloodFill(copyBgr, cvPoint(a, b), cvScalarAll(255), cvScalarAll(
					20), cvScalarAll(20), NULL, (255 << 8)
					+CV_FLOODFILL_MASK_ONLY + 4, maskImage);
			cvSmooth(maskImage, maskImage, CV_MEDIAN, 9, 9, 0, 0);
			//cvShowImage("Captures",maskImage);
			//cvWaitKey(0);
			//From left
			//cout<<"1"<<endl;
			for (int x = 4; x < maskImage->width - 4; x++) {
				for (int y = 4; y < maskImage->height - 4; y++) {
					color1 = cvGet2D(maskImage, y, x);
					if (color1.val[0] == 255) {
						*minX = x - 1;
						x = maskImage->width;
						y = maskImage->height;
					}
				}
			}
			//From right
			//cout<<"2"<<endl;
			for (int x = maskImage->width - 4; x >= 4; x--) {
				for (int y = maskImage->height - 4; y >= 4; y--) {
					//cout<<"x: "<<x<<", y: "<<y<<endl;
					color1 = cvGet2D(maskImage, y, x);
					if (color1.val[0] == 255) {
						*maxX = x - 1;
						x = 0;
						y = 0;
					}
				}
			}
			//From up
			//cout<<"3"<<endl;
			for (int y = 4; y < maskImage->height - 4; y++) {
				for (int x = 4; x < maskImage->width - 4; x++) {
					color1 = cvGet2D(maskImage, y, x);
					if (color1.val[0] == 255) {
						*minY = y - 1;
						x = maskImage->width;
						y = maskImage->height;
					}
				}
			}
			//From down
			//cout<<"5"<<endl;
			for (int y = maskImage->height - 4; y >= 4; y--) {
				for (int x = maskImage->width - 4; x >= 4; x--) {
					color1 = cvGet2D(maskImage, y, x);
					if (color1.val[0] == 255) {
						*maxY = y - 1;
						x = 0;
						y = 0;
					}
				}
			}
			//cout<<"5"<<endl;
			cvReleaseImage(&maskImage);
			maskImage = NULL;
			return 3;
		} else {
			return -1;
		}
	} else
		return -2;

}

void initializePacket(Packet *willBeSent) {

	willBeSent->redHists.resize(9);
	willBeSent->greenHists.resize(9);
	willBeSent->blueHists.resize(9);

	int numBins = N_BINS;
	float range[] = { 0, 255 };
	float *ranges[] = { range };

	for (uint i = 0; i < willBeSent->redHists.size(); i++) {
		willBeSent->blueHists[i] = cvCreateHist(1, &numBins, CV_HIST_ARRAY,
				ranges, 1);
		willBeSent->redHists[i] = cvCreateHist(1, &numBins, CV_HIST_ARRAY,
				ranges, 1);
		willBeSent->greenHists[i] = cvCreateHist(1, &numBins, CV_HIST_ARRAY,
				ranges, 1);
		cvClearHist(willBeSent->redHists[i]);
		cvClearHist(willBeSent->greenHists[i]);
		cvClearHist(willBeSent->blueHists[i]);
	}

	willBeSent->boundingBoxes.resize(9);
	for (int a = 0; a < willBeSent->boundingBoxes.size(); a++) {
		willBeSent->boundingBoxes[a] = cvRect(0, 0, 0, 0);
	}

	willBeSent->segmented_areas.resize(9);
	for (int a = 0; a < willBeSent->segmented_areas.size(); a++) {
		willBeSent->segmented_areas[a] = -1;
	}

	//willBeSent->raw_image = NULL;
	willBeSent->raw_image = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
	willBeSent->filtered_image = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U,
			3);
	willBeSent->faceDetected = false;

}
void initializeTheGlobalVars() {

	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			setOrReset[x][y] = -1;
		}
	}

	for (int x = 0; x < 9; x++) {
		images[x] = NULL;
	}

	for (int x = 0; x < 9; x++) {
		seenOrNot[x] = false;
	}

	rawForDisappear = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
	filteredForDisappear = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
	copyOfGlobBgr = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);

}

void releaseMemory() {

	if (features != NULL) {
		delete[] features;
		features = NULL;
	}

	if (newCornerFeatures != NULL) {
		delete[] newCornerFeatures;
		newCornerFeatures = NULL;
	}

	if (singleCorners != NULL) {
		delete[] singleCorners;
		singleCorners = NULL;
	}

	for (int i = 0; i < counter; i++) {
		if (identificationIndex[i] != NULL) {
			delete[] identificationIndex[i];
			identificationIndex[i] = NULL;
		}

	}

	if (identificationIndex != NULL) {
		delete[] identificationIndex;
		identificationIndex = NULL;
	}

	if (identified != NULL) {
		delete[] identified;
		identified = NULL;
	}

	//	cout<<"hiiiii"<<endl;
	if (newCornerFeatures != NULL) {
		delete[] newCornerFeatures;
		newCornerFeatures = NULL;
	}

	if (singleCorners != NULL) {
		delete[] singleCorners;
		singleCorners = NULL;
	}

}

bool floodFillCenterBased(int x, int y, int xPixelOffset, int yPixelOffset) {
	cout << "width: " << copyBgr->width << " , height: " << copyBgr->height
			<< endl;
	if (Image2 != NULL) {
		cvReleaseImage(&Image2);
		Image2 = NULL;
	}
	Image2 = cvCreateImage(cvSize(copyBgr->width, copyBgr->height),
			IPL_DEPTH_8U, 3);

	cvZero(Image2);
	int result = 0;
	//	willBeSent.filtered_image = cvCreateImage(cvSize(copyBgr->width,
	//			copyBgr->height), IPL_DEPTH_8U, 3);
	//	cvCopyImage(copyBgr,willBeSent.filtered_image);
	cout << "flood 1" << endl;
	int minY, maxY, minX, maxX;
	minY = 480, maxY = 0, minX = 640, maxX = 0;
	total[0] = 0;
	total[1] = 0;
	total[2] = 0;
	diff = 0;
	countForPixels = 0;
	cvZero(Image2);
	result = floodFill(copyBgr->width / 2 + xPixelOffset, copyBgr->height / 2
			+ yPixelOffset, &minX, &minY, &maxX, &maxY);
	cout << "flood 2" << endl;
	if (result == 3) {
		if (minX < copyBgr->width && minX >= 0 && maxX < copyBgr->width && maxX
				>= 0 && minY < copyBgr->height && minY >= 0 && maxY
				< copyBgr->height && maxY >= 0) {

			if (images[y + x * 3] != NULL) {
				cvReleaseImage(&images[y + x * 3]);
				images[y + x * 3] = NULL;

			}
			images[y + x * 3] = cvCreateImage(cvSize(maxX - minX, maxY - minY),
					copyBgr->depth, copyBgr->nChannels);

			if (x != 2 && setOrReset[x + 1][y] != 1) {
				willBeSent.boundingBoxes[y + x * 3] = cvRect(minX, minY, maxX
						- minX, maxY - minY);

			} else if (x == 2) {
				willBeSent.boundingBoxes[y + x * 3] = cvRect(minX, minY, maxX
						- minX, maxY - minY);
			}
			cout << "flood 3" << endl;
			cvSetImageROI(copyBgr, cvRect(minX, minY, maxX - minX, maxY - minY));
			if (cvGetImageROI(copyBgr).width == maxX - minX && cvGetImageROI(
					copyBgr).height == maxY - minY) {

				if (x != 2 && setOrReset[x + 1][y] != 1) {
					cvCopy(copyBgr, images[y + x * 3]);

					if (!seenOrNot[y + x * 3])
						seenOrNot[y + x * 3] = true;
				} else if (x == 2) {
					cvCopy(copyBgr, images[y + x * 3]);
					if (!seenOrNot[y + x * 3])
						seenOrNot[y + x * 3] = true;
				}
			}
			cout << "flood 4" << endl;
			cvResetImageROI(copyBgr);
			willBeSent.segmented_areas[y + x * 3] = getImageSize(images[y + x
					* 3]);
			cvClearHist(willBeSent.blueHists[y + x * 3]);
			cvClearHist(willBeSent.redHists[y + x * 3]);
			cvClearHist(willBeSent.greenHists[y + x * 3]);
			imgRed = cvCreateImage(cvGetSize(images[y + x * 3]), 8, 1);
			imgGreen = cvCreateImage(cvGetSize(images[y + x * 3]), 8, 1);
			imgBlue = cvCreateImage(cvGetSize(images[y + x * 3]), 8, 1);

			cvSplit(images[y + x * 3], imgBlue, imgGreen, imgRed, NULL);

			cvCalcHist(&imgRed, willBeSent.redHists[y + x * 3], 0, 0);
			cvCalcHist(&imgGreen, willBeSent.greenHists[y + x * 3], 0, 0);
			cvCalcHist(&imgBlue, willBeSent.blueHists[y + x * 3], 0, 0);

			cvReleaseImage(&imgRed);
			cvReleaseImage(&imgGreen);
			cvReleaseImage(&imgBlue);
			imgRed = NULL;
			imgGreen = NULL;
			imgBlue = NULL;
			cout << "flood 5" << endl;
			return true;
		} else
			return false;

	} else if (result == -1 || result == -2) {
		return false;
	}
}

int getImageSize(IplImage *overlay) {
	int sizeOfObject = 0;
	for (int x = 0; x < overlay->width; x++) {

		for (int y = 0; y < overlay->height; y++) {

			if (y < overlay->height && y > 0 && x < overlay->width && x > 0) {
				CvScalar over = cvGet2D(overlay, y, x);
				if (over.val[0] > 5.0 && over.val[0] < 250.0) {
					sizeOfObject++;
				}

			}
			//cvSet2D(src, y+location.y - overlay->height/2, x+location.x- overlay->width/2, over);
		}
	}

	return sizeOfObject;
}

void findAreasAndCorners(int indexAndDegrees[][9]) {
	for (int x = 0; x < counter; x++) {
		if (indexAndDegrees[x][0] == 4 && !identified[x]) {
			//cout<<"4lu"<<endl;
//			cvCopyImage(copyOfGlobBgr,tempImg);
//											cvCircle(tempImg, cvPoint(singleCorners[x].x, singleCorners[x].y), 4,cvScalar(0, 255, 0), -1, 8);
//											cvShowImage("4neigh",tempImg);
//											cvWaitKey(0);
			ways = determineWays(singleCorners, indexAndDegrees, x);
			bool leftSelected = false;
			bool rightSelected = false;
			bool upSelected = false;
			bool downSelected = false;
			int horizontal;
			int vertical;
			if (abs(singleCorners[ways[0]].x - 320) < abs(
					singleCorners[ways[2]].x - 320)) {
				leftSelected = true;
				horizontal = findExtensionColor(singleCorners[x],
						singleCorners[ways[0]], imgResult);
				//cout<<"leftSelected"<<endl;
			} else {
				rightSelected = true;
				horizontal = findExtensionColor(singleCorners[x],
						singleCorners[ways[2]], imgResult);
				//cout<<"rightSelected"<<endl;
			}
			if (abs(singleCorners[ways[1]].y - 240) < abs(
					singleCorners[ways[3]].y - 240)) {
				downSelected = true;
				vertical = findExtensionColor(singleCorners[x],
						singleCorners[ways[1]], imgResult);
				//cout<<"downSelected"<<endl;
			} else {
				upSelected = true;
				vertical = findExtensionColor(singleCorners[x],
						singleCorners[ways[3]], imgResult);
				//cout<<"upSelected"<<endl;
			}

			bool selections[4];
			selections[0] = leftSelected;
			selections[1] = downSelected;
			selections[2] = rightSelected;
			selections[3] = upSelected;
			int verhor[2];
			verhor[0] = vertical;
			verhor[1] = horizontal;
			//cout<<"verhor[0]: "<<verhor[0]<<" and verhor[1]: "<<verhor[1]<<endl;
			if (verhor[0] != -2 && verhor[1] != -2 && verhor[0] != -1
					&& verhor[1] != -1) {
				tagMainCorner(selections, verhor, x, ways);
			}

			if (ways != NULL) {
				delete[] ways;
				ways = NULL;
			}
		}

	}

	for (int x = 0; x < counter; x++) {
		if (indexAndDegrees[x][0] == 3 && !identified[x]) {
//			cvCopyImage(copyOfGlobBgr,tempImg);
//														cvCircle(tempImg, cvPoint(singleCorners[x].x, singleCorners[x].y), 4,cvScalar(0, 255, 0), -1, 8);
//														cvShowImage("4neigh",tempImg);
//														cvWaitKey(0);
			int *ways = new int[4];
			for (int i = 0; i < 4; i++) {
				ways[i] = -1;
			}
			for (int i = 0; i < indexAndDegrees[x][0]; i++) {

				if (indexAndDegrees[x][2 * i + 2] > 135 && indexAndDegrees[x][2
						* i + 2] <= 225) {
					ways[2] = indexAndDegrees[x][2 * i + 1];
				} else if (indexAndDegrees[x][2 * i + 2] > 45
						&& indexAndDegrees[x][2 * i + 2] <= 135) {
					ways[3] = indexAndDegrees[x][2 * i + 1];
				} else if (indexAndDegrees[x][2 * i + 2] > 315
						|| indexAndDegrees[x][2 * i + 2] <= 45) {
					ways[0] = indexAndDegrees[x][2 * i + 1];
				} else if (indexAndDegrees[x][2 * i + 2] > 225
						&& indexAndDegrees[x][2 * i + 2] <= 315) {
					ways[1] = indexAndDegrees[x][2 * i + 1];
				}

			}

			bool selections[4];
			selections[0] = false;
			selections[1] = false;
			selections[2] = false;
			selections[3] = false;
			int verhor[2];
			verhor[0] = false;
			verhor[1] = false;
			int whiteOrBlack;
			//cout<<"-1 kontrolu"<<endl;
			if (ways[0] == -1) {
				//cout<<"SOl bos"<<endl;
				whiteOrBlack = findExtensionColor(singleCorners[ways[2]],
						singleCorners[x], imgResult);
				if (abs(singleCorners[ways[1]].y - 240) < abs(
						singleCorners[ways[3]].y - 240)) {

					selections[1] = true;
					verhor[0] = findExtensionColor(singleCorners[x],
							singleCorners[ways[1]], imgResult);
				} else {
					selections[3] = true;
					verhor[0] = findExtensionColor(singleCorners[x],
							singleCorners[ways[3]], imgResult);
				}

				if (whiteOrBlack == 1) {
					//cout<<"beyazim"<<endl;
					selections[2] = true;
					verhor[1] = findExtensionColor(singleCorners[x],
							singleCorners[ways[2]], imgResult);

					if (verhor[0] != -2 && verhor[1] != -2 && verhor[0] != -1
							&& verhor[1] != -1) {
						tagMainCorner(selections, verhor, x, ways);

					}
				} else if (whiteOrBlack == 0) {
					//cout<<"siyahim"<<endl;
					if ((selections[1] && verhor[0] == 1) || (selections[3]
							&& verhor[0] == 0)) {
						identificationIndex[x][0] = 1;
						identificationIndex[x][1] = 0;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					} else if ((selections[1] && verhor[0] == 0)
							|| (selections[3] && verhor[0] == 1)) {
						identificationIndex[x][0] = 2;
						identificationIndex[x][1] = 0;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					}
				}
				//else
				//cout<<"belirsiz"<<endl;
				//cvWaitKey(0);

			} else if (ways[1] == -1) {
				//cout<<"asagi bos"<<endl;
				whiteOrBlack = findExtensionColor(singleCorners[ways[3]],
						singleCorners[x], imgResult);
				if (abs(singleCorners[ways[0]].x - 320) < abs(
						singleCorners[ways[2]].x - 320)) {
					selections[0] = true;

					verhor[1] = findExtensionColor(singleCorners[x],
							singleCorners[ways[0]], imgResult);
				} else {
					selections[2] = true;

					verhor[1] = findExtensionColor(singleCorners[x],
							singleCorners[ways[2]], imgResult);
				}

				if (whiteOrBlack == 1) {
					selections[3] = true;
					verhor[0] = findExtensionColor(singleCorners[x],
							singleCorners[ways[3]], imgResult);
					if (verhor[0] != -2 && verhor[1] != -2 && verhor[0] != -1
							&& verhor[1] != -1) {
						tagMainCorner(selections, verhor, x, ways);
					}

				} else if (whiteOrBlack == 0) {
					if ((selections[0] && verhor[1] == 1) || (selections[2]
							&& verhor[1] == 0)) {
						identificationIndex[x][0] = 3;
						identificationIndex[x][1] = 2;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					} else if ((selections[0] && verhor[1] == 0)
							|| (selections[2] && verhor[1] == 1)) {
						identificationIndex[x][0] = 3;
						identificationIndex[x][1] = 1;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					}
				}

			} else if (ways[2] == -1) {
				//cout<<"sag bos"<<endl;
				whiteOrBlack = findExtensionColor(singleCorners[ways[0]],
						singleCorners[x], imgResult);
				if (abs(singleCorners[ways[1]].y - 240) < abs(
						singleCorners[ways[3]].y - 240)) {

					selections[1] = true;
					verhor[0] = findExtensionColor(singleCorners[x],
							singleCorners[ways[1]], imgResult);
				} else {
					selections[3] = true;
					verhor[0] = findExtensionColor(singleCorners[x],
							singleCorners[ways[3]], imgResult);
				}

				if (whiteOrBlack == 1) {
					selections[0] = true;
					verhor[1] = findExtensionColor(singleCorners[x],
							singleCorners[ways[0]], imgResult);

					if (verhor[0] != -2 && verhor[1] != -2 && verhor[0] != -1
							&& verhor[1] != -1) {
						//cout<<"TAGGING"<<endl;
						tagMainCorner(selections, verhor, x, ways);
					}

				} else if (whiteOrBlack == 0) {
					if ((selections[1] && verhor[0] == 1) || (selections[3]
							&& verhor[0] == 0)) {
						identificationIndex[x][0] = 1;
						identificationIndex[x][1] = 3;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					} else if ((selections[1] && verhor[0] == 0)
							|| (selections[3] && verhor[0] == 1)) {
						identificationIndex[x][0] = 2;
						identificationIndex[x][1] = 3;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					}
				}

			} else if (ways[3] == -1) {
				//cout<<"ust bos"<<endl;
				whiteOrBlack = findExtensionColor(singleCorners[ways[1]],
						singleCorners[x], imgResult);
				if (abs(singleCorners[ways[0]].x - 320) < abs(
						singleCorners[ways[2]].x - 320)) {
					selections[0] = true;
					verhor[1] = findExtensionColor(singleCorners[x],
							singleCorners[ways[0]], imgResult);
				} else {
					selections[2] = true;
					verhor[1] = findExtensionColor(singleCorners[x],
							singleCorners[ways[2]], imgResult);
				}

				if (whiteOrBlack == 1) {
					selections[1] = true;
					verhor[0] = findExtensionColor(singleCorners[x],
							singleCorners[ways[1]], imgResult);

					if (verhor[0] != -2 && verhor[1] != -2 && verhor[0] != -1
							&& verhor[1] != -1) {
						tagMainCorner(selections, verhor, x, ways);
					}
				} else if (whiteOrBlack == 0) {
					if ((selections[0] && verhor[1] == 1) || (selections[2]
							&& verhor[1] == 0)) {
						identificationIndex[x][0] = 0;
						identificationIndex[x][1] = 2;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					} else if ((selections[0] && verhor[1] == 0)
							|| (selections[2] && verhor[1] == 1)) {
						identificationIndex[x][0] = 0;
						identificationIndex[x][1] = 1;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					}
				}

			}

			if (ways != NULL) {
				delete[] ways;
				ways = NULL;
			}
		}
	}
//
	for (int x = 0; x < counter; x++) {
		if (indexAndDegrees[x][0] == 2 && !identified[x]) {
//			cvCopyImage(copyOfGlobBgr,tempImg);
//																	cvCircle(tempImg, cvPoint(singleCorners[x].x, singleCorners[x].y), 4,cvScalar(0, 255, 0), -1, 8);
//																	cvShowImage("2neigh",tempImg);
//																	cvWaitKey(0);
			int *ways = new int[4];
			for (int i = 0; i < 4; i++) {
				ways[i] = -1;
			}
			for (int i = 0; i < indexAndDegrees[x][0]; i++) {

				if (indexAndDegrees[x][2 * i + 2] > 135 && indexAndDegrees[x][2
						* i + 2] <= 225) {
					ways[2] = indexAndDegrees[x][2 * i + 1];
				} else if (indexAndDegrees[x][2 * i + 2] > 45
						&& indexAndDegrees[x][2 * i + 2] <= 135) {
					ways[3] = indexAndDegrees[x][2 * i + 1];
				} else if (indexAndDegrees[x][2 * i + 2] > 315
						|| indexAndDegrees[x][2 * i + 2] <= 45) {
					ways[0] = indexAndDegrees[x][2 * i + 1];
				} else if (indexAndDegrees[x][2 * i + 2] > 225
						&& indexAndDegrees[x][2 * i + 2] <= 315) {
					ways[1] = indexAndDegrees[x][2 * i + 1];
				}

			}

			bool selections[4];
			selections[0] = false;
			selections[1] = false;
			selections[2] = false;
			selections[3] = false;
			int verhor[2];
			verhor[0] = -1;
			verhor[1] = -1;
			bool whiteOrBlack;
			int control[2];
			if (ways[3] == -1 && ways[2] == -1) {
				//cout<<"3 ve 2 yok"<<endl;

				control[0] = findExtensionColor(singleCorners[ways[1]],
						singleCorners[x], imgResult);
				control[1] = findExtensionColor(singleCorners[ways[0]],
						singleCorners[x], imgResult);
				//				cout<<"control[0]"<<control[0]<<endl;
				//				cout<<"control[1]"<<control[1]<<endl;
				if (control[0] == 0 && control[1] == 0) {
					identificationIndex[x][0] = 0;
					identificationIndex[x][1] = 3;
					identified[x] = true;
					willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
							= true;
					places[identificationIndex[x][0]][identificationIndex[x][1]]
							= singleCorners[x];
				} else if (control[0] == 0 && control[1] == 1) {

					int horizontal = findExtensionColor(singleCorners[x],
							singleCorners[ways[0]], imgResult);
					if (horizontal == 1) {
						identificationIndex[x][0] = 0;
						identificationIndex[x][1] = 2;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					} else if (horizontal == 0) {
						identificationIndex[x][0] = 0;
						identificationIndex[x][1] = 1;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					}

				} else if (control[0] == 1 && control[1] == 0) {
					int vertical = findExtensionColor(singleCorners[x],
							singleCorners[ways[1]], imgResult);
					if (vertical == 1) {
						identificationIndex[x][0] = 1;
						identificationIndex[x][1] = 3;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					} else if (vertical == 0) {
						identificationIndex[x][0] = 2;
						identificationIndex[x][1] = 3;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					}
				} else if (control[0] == 1 && control[1] == 1) {
					selections[1] = true;
					selections[0] = true;
					verhor[0] = findExtensionColor(singleCorners[x],
							singleCorners[ways[1]], imgResult);
					verhor[1] = findExtensionColor(singleCorners[x],
							singleCorners[ways[0]], imgResult);

					if (verhor[0] != -2 && verhor[1] != -2 && verhor[0] != -1
							&& verhor[1] != -1) {
						tagMainCorner(selections, verhor, x, ways);
					}
				}

			} else if (ways[0] == -1 && ways[3] == -1) {
				//cout<<"0 ve 3 yok"<<endl;
				control[0] = findExtensionColor(singleCorners[ways[2]],
						singleCorners[x], imgResult);
				control[1] = findExtensionColor(singleCorners[ways[1]],
						singleCorners[x], imgResult);
				//				cout<<"control[0]"<<control[0]<<endl;
				//				cout<<"control[1]"<<control[1]<<endl;
				if (control[0] == 0 && control[1] == 0) {
					identificationIndex[x][0] = 0;
					identificationIndex[x][1] = 0;
					identified[x] = true;
					willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
							= true;
					places[identificationIndex[x][0]][identificationIndex[x][1]]
							= singleCorners[x];
				} else if (control[0] == 0 && control[1] == 1) {

					int vertical = findExtensionColor(singleCorners[x],
							singleCorners[ways[1]], imgResult);
					if (vertical == 1) {
						identificationIndex[x][0] = 1;
						identificationIndex[x][1] = 0;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					} else if (vertical == 0) {
						identificationIndex[x][0] = 2;
						identificationIndex[x][1] = 0;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					}

				} else if (control[0] == 1 && control[1] == 0) {
					int horizontal = findExtensionColor(singleCorners[x],
							singleCorners[ways[2]], imgResult);
					if (horizontal == 1) {
						identificationIndex[x][0] = 0;
						identificationIndex[x][1] = 1;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					} else if (horizontal == 0) {
						identificationIndex[x][0] = 0;
						identificationIndex[x][1] = 2;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					}
				} else if (control[0] == 1 && control[1] == 1) {
					selections[1] = true;
					selections[2] = true;
					verhor[0] = findExtensionColor(singleCorners[x],
							singleCorners[ways[1]], imgResult);
					verhor[1] = findExtensionColor(singleCorners[x],
							singleCorners[ways[2]], imgResult);

					if (verhor[0] != -2 && verhor[1] != -2 && verhor[0] != -1
							&& verhor[1] != -1) {
						//cout<<"TAGGING"<<endl;
						tagMainCorner(selections, verhor, x, ways);
					}
				}
			} else if (ways[1] == -1 && ways[2] == -1) {
				//cout<<"1 ve 2 yok"<<endl;
				control[0] = findExtensionColor(singleCorners[ways[3]],
						singleCorners[x], imgResult);
				control[1] = findExtensionColor(singleCorners[ways[0]],
						singleCorners[x], imgResult);
				//				cout<<"control[0]"<<control[0]<<endl;
				//				cout<<"control[1]"<<control[1]<<endl;
				if (control[0] == 0 && control[1] == 0) {
					identificationIndex[x][0] = 3;
					identificationIndex[x][1] = 3;
					identified[x] = true;
					willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
							= true;
					places[identificationIndex[x][0]][identificationIndex[x][1]]
							= singleCorners[x];
				} else if (control[0] == 0 && control[1] == 1) {

					int horizontal = findExtensionColor(singleCorners[x],
							singleCorners[ways[0]], imgResult);
					if (horizontal == 1) {
						identificationIndex[x][0] = 3;
						identificationIndex[x][1] = 2;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					} else if (horizontal == 0) {
						identificationIndex[x][0] = 3;
						identificationIndex[x][1] = 1;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					}

				} else if (control[0] == 1 && control[1] == 0) {
					int vertical = findExtensionColor(singleCorners[x],
							singleCorners[ways[3]], imgResult);
					if (vertical == 1) {
						identificationIndex[x][0] = 2;
						identificationIndex[x][1] = 3;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					} else if (vertical == 0) {
						identificationIndex[x][0] = 1;
						identificationIndex[x][1] = 3;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					}
				} else if (control[0] == 1 && control[1] == 1) {
					selections[3] = true;
					selections[0] = true;
					verhor[0] = findExtensionColor(singleCorners[x],
							singleCorners[ways[3]], imgResult);
					verhor[1] = findExtensionColor(singleCorners[x],
							singleCorners[ways[0]], imgResult);

					if (verhor[0] != -2 && verhor[1] != -2 && verhor[0] != -1
							&& verhor[1] != -1) {
						tagMainCorner(selections, verhor, x, ways);
					}
				}
			} else if (ways[0] == -1 && ways[1] == -1) {
				//cout<<"0 ve 1 yok"<<endl;
				control[0] = findExtensionColor(singleCorners[ways[2]],
						singleCorners[x], imgResult);
				control[1] = findExtensionColor(singleCorners[ways[3]],
						singleCorners[x], imgResult);
				//				cout<<"control[0]"<<control[0]<<endl;
				//				cout<<"control[1]"<<control[1]<<endl;
				if (control[0] == 0 && control[1] == 0) {
					identificationIndex[x][0] = 3;
					identificationIndex[x][1] = 0;
					identified[x] = true;
					willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
							= true;
					places[identificationIndex[x][0]][identificationIndex[x][1]]
							= singleCorners[x];
				} else if (control[0] == 0 && control[1] == 1) {

					int vertical = findExtensionColor(singleCorners[x],
							singleCorners[ways[3]], imgResult);
					if (vertical == 1) {
						identificationIndex[x][0] = 2;
						identificationIndex[x][1] = 0;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					} else if (vertical == 0) {
						identificationIndex[x][0] = 1;
						identificationIndex[x][1] = 0;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					}

				} else if (control[0] == 1 && control[1] == 0) {
					int horizontal = findExtensionColor(singleCorners[x],
							singleCorners[ways[2]], imgResult);
					if (horizontal == 1) {
						identificationIndex[x][0] = 3;
						identificationIndex[x][1] = 1;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					} else if (horizontal == 0) {
						identificationIndex[x][0] = 3;
						identificationIndex[x][1] = 2;
						identified[x] = true;
						willBeFilled[identificationIndex[x][0]][identificationIndex[x][1]]
								= true;
						places[identificationIndex[x][0]][identificationIndex[x][1]]
								= singleCorners[x];
					}
				} else if (control[0] == 1 && control[1] == 1) {
					selections[3] = true;
					selections[2] = true;
					verhor[0] = findExtensionColor(singleCorners[x],
							singleCorners[ways[3]], imgResult);
					verhor[1] = findExtensionColor(singleCorners[x],
							singleCorners[ways[2]], imgResult);

					if (verhor[0] != -2 && verhor[1] != -2 && verhor[0] != -1
							&& verhor[1] != -1) {
						tagMainCorner(selections, verhor, x, ways);
					}
				}
			}

			if (ways != NULL) {
				delete[] ways;
				ways = NULL;
			}
		}
	}
//
	for (int g = 0; g < counter; g++) {
		if (!identified[g]) {

			for (int y = 0; y < indexAndDegrees[g][0]; y++) {
				if (identified[indexAndDegrees[g][2 * y + 1]]) {
					//cvCopyImage(bgr_frameLeft,tempImg);
					//					cvCircle(tempImg, cvPoint(singleCorners[g].x, singleCorners[g].y), 4,cvScalar(0, 255, 0), -1, 8);
					//					cvShowImage("Captures",tempImg);
					//cvWaitKey(0);
					if ((indexAndDegrees[g][2 * y + 2] <= 225)
							&& (indexAndDegrees[g][2 * y + 2] > 135)) {
						//						cout<<"sagda tanimli"<<endl;
						//						cvCircle(tempImg, cvPoint(singleCorners[indexAndDegrees[g][2*y +1]].x, singleCorners[indexAndDegrees[g][2*y +1]].y), 4,cvScalar(0, 255, 0), -1, 8);
						//												cvShowImage("Captures",tempImg);
						//	cvWaitKey(0);
						identificationIndex[g][0]
								= identificationIndex[indexAndDegrees[g][2 * y
										+ 1]][0];
						identificationIndex[g][1]
								= identificationIndex[indexAndDegrees[g][2 * y
										+ 1]][1] - 1;
						//cout<<"koordinatlari x: "<<identificationIndex[g][0]<<", y: "<<identificationIndex[g][1]<<endl;

						if (identificationIndex[g][0] >= 0
								&& identificationIndex[g][0] < 4
								&& identificationIndex[g][1] >= 0
								&& identificationIndex[g][1] < 4) {
							identified[g] = true;
							willBeFilled[identificationIndex[g][0]][identificationIndex[g][1]]
									= true;
							places[identificationIndex[g][0]][identificationIndex[g][1]]
									= singleCorners[g];
							break;
						} else {
							identificationIndex[g][0] = -1;
							identificationIndex[g][1] = -1;
						}

					} else if ((indexAndDegrees[g][2 * y + 2] > 315)
							|| (indexAndDegrees[g][2 * y + 2] <= 45)) {

						//						cout<<"solda tanimli"<<endl;
						//						cvCircle(tempImg, cvPoint(singleCorners[indexAndDegrees[g][2*y +1]].x, singleCorners[indexAndDegrees[g][2*y +1]].y), 4,cvScalar(0, 255, 0), -1, 8);
						//												cvShowImage("Captures",tempImg);
						//												//cvWaitKey(0);
						identificationIndex[g][0]
								= identificationIndex[indexAndDegrees[g][2 * y
										+ 1]][0];
						identificationIndex[g][1]
								= identificationIndex[indexAndDegrees[g][2 * y
										+ 1]][1] + 1;
						//cout<<"koordinatlari x: "<<identificationIndex[g][0]<<", y: "<<identificationIndex[g][1]<<endl;
						if (identificationIndex[g][0] >= 0
								&& identificationIndex[g][0] < 4
								&& identificationIndex[g][1] >= 0
								&& identificationIndex[g][1] < 4) {
							identified[g] = true;
							willBeFilled[identificationIndex[g][0]][identificationIndex[g][1]]
									= true;
							places[identificationIndex[g][0]][identificationIndex[g][1]]
									= singleCorners[g];
							break;
						} else {
							identificationIndex[g][0] = -1;
							identificationIndex[g][1] = -1;
						}
					} else if ((indexAndDegrees[g][2 * y + 2] <= 315)
							&& (indexAndDegrees[g][2 * y + 2] > 225)) {
						//cout<<"asagida tanimli"<<endl;
						//						cvCircle(tempImg, cvPoint(singleCorners[indexAndDegrees[g][2*y +1]].x, singleCorners[indexAndDegrees[g][2*y +1]].y), 4,cvScalar(0, 255, 0), -1, 8);
						//												cvShowImage("Captures",tempImg);
						//cvWaitKey(0);
						identificationIndex[g][0]
								= identificationIndex[indexAndDegrees[g][2 * y
										+ 1]][0] - 1;
						identificationIndex[g][1]
								= identificationIndex[indexAndDegrees[g][2 * y
										+ 1]][1];
						//cout<<"koordinatlari x: "<<identificationIndex[g][0]<<", y: "<<identificationIndex[g][1]<<endl;
						if (identificationIndex[g][0] >= 0
								&& identificationIndex[g][0] < 4
								&& identificationIndex[g][1] >= 0
								&& identificationIndex[g][1] < 4) {
							identified[g] = true;
							willBeFilled[identificationIndex[g][0]][identificationIndex[g][1]]
									= true;
							places[identificationIndex[g][0]][identificationIndex[g][1]]
									= singleCorners[g];
							break;
						} else {
							identificationIndex[g][0] = -1;
							identificationIndex[g][1] = -1;
						}
					} else if ((indexAndDegrees[g][2 * y + 2] <= 135)
							&& (indexAndDegrees[g][2 * y + 2] > 45)) {
						//cout<<"ustte tanimli"<<endl;
						cvCircle(tempImg,
								cvPoint(singleCorners[indexAndDegrees[g][2 * y
										+ 1]].x,
										singleCorners[indexAndDegrees[g][2 * y
												+ 1]].y), 4,
								cvScalar(0, 255, 0), -1, 8);
						cvShowImage("Captures", tempImg);
						//cvWaitKey(0);
						identificationIndex[g][0]
								= identificationIndex[indexAndDegrees[g][2 * y
										+ 1]][0] + 1;
						identificationIndex[g][1]
								= identificationIndex[indexAndDegrees[g][2 * y
										+ 1]][1];
						//cout<<"koordinatlari x: "<<identificationIndex[g][0]<<", y: "<<identificationIndex[g][1]<<endl;
						if (identificationIndex[g][0] >= 0
								&& identificationIndex[g][0] < 4
								&& identificationIndex[g][1] >= 0
								&& identificationIndex[g][1] < 4) {
							identified[g] = true;
							willBeFilled[identificationIndex[g][0]][identificationIndex[g][1]]
									= true;
							places[identificationIndex[g][0]][identificationIndex[g][1]]
									= singleCorners[g];
							break;
						} else {
							identificationIndex[g][0] = -1;
							identificationIndex[g][1] = -1;
						}
					}
				}
			}
		}
	}
}

