#include "pch.h"

//add by YK
#include "lib_YK.h"

int sw = 0; //0: init, 1: ON,  2: END
int record_size = 0;
unsigned int numCameras = 0;
bool capture = false;

ImagePtr image[2] = { NULL, NULL };
vector<ImagePtr> record_image[2], cap_image[2];
CFloatPtr ptrFramerate[2], ptrExposure[2], ptrGain[2];
cam_info icam[2];

//function
void ConfigureCustomImageSettings(INodeMap& nodeMap);
void ConfigureTrigger(INodeMap& nodeMap);
void AcquireImage(CameraPtr cam, INodeMap& nodeMap);
void ImageFormat(INodeMap& nodeMap);
void Exposure(INodeMap& nodeMap);
void Gain(INodeMap& nodeMap);
void Framerate(INodeMap& nodeMap);
void Buffer(CameraPtr cam);

//FLIRCam class
FLIRCam::FLIRCam()
{

}

FLIRCam::~FLIRCam()
{

}

void FLIRCam::FLIRtoCV(CameraPtr cam, Mat& outimg)
{
	ImagePtr pImg = cam->GetNextImage();
	ImagePtr convertedImage = pImg->Convert(PixelFormat_Mono8);

	unsigned int XPadding = convertedImage->GetXPadding();
	unsigned int YPadding = convertedImage->GetYPadding();
	unsigned int rowsize = convertedImage->GetWidth();
	unsigned int colsize = convertedImage->GetHeight();

	//image data contains padding. When allocating Mat container size, you need to account for the X,Y image data padding. 
	Mat img = Mat(colsize + YPadding, rowsize + XPadding, CV_8UC1, convertedImage->GetData(), convertedImage->GetStride());
	outimg = img.clone();

}

void FLIRCam::AcquireSetup(CameraPtr cam, INodeMap& nodeMap)
{	
	ConfigureCustomImageSettings(nodeMap);
	ConfigureTrigger(nodeMap);
	Exposure(nodeMap);
	Gain(nodeMap);
	ConfigureCustomImageSettings(nodeMap);
	Framerate(nodeMap);
	Buffer(cam);
	AcquireImage(cam, nodeMap);
	return;
}

void FLIRCam::getCameraInfo(INodeMap& nodeMap, int i) {
	icam[i].ptrFramerate = nodeMap.GetNode("AcquisitionFrameRate");
	icam[i].fps_min = icam[i].ptrFramerate->GetMin();
	icam[i].fps_max = icam[i].ptrFramerate->GetMax();
	icam[i].fps = icam[i].ptrFramerate->GetValue();

	icam[i].ptrExposure = nodeMap.GetNode("ExposureTime");
	icam[i].exptime_min = icam[i].ptrExposure->GetMin();
	icam[i].exptime_max = icam[i].ptrExposure->GetMax();
	icam[i].exptime = icam[i].ptrExposure->GetValue();

	icam[i].ptrGain = nodeMap.GetNode("Gain");
	icam[i].gain_min = icam[i].ptrGain->GetMin();
	icam[i].gain_max = icam[i].ptrGain->GetMax();
	icam[i].gain = icam[i].ptrGain->GetValue();
}

void ConfigureCustomImageSettings(INodeMap& nodeMap)
{
	
	CEnumerationPtr ptrPixelFormat = nodeMap.GetNode("PixelFormat");
	if (IsAvailable(ptrPixelFormat) && IsWritable(ptrPixelFormat))
		CEnumEntryPtr ptrPixelFormatMono8 = ptrPixelFormat->GetEntryByName("Mono8");
	

	CIntegerPtr ptrOffsetX = nodeMap.GetNode("OffsetX");
	if (IsAvailable(ptrOffsetX) && IsWritable(ptrOffsetX))
		ptrOffsetX->SetValue(ptrOffsetX->GetMin());

	CIntegerPtr ptrOffsetY = nodeMap.GetNode("OffsetY");
	if (IsAvailable(ptrOffsetY) && IsWritable(ptrOffsetY))
		ptrOffsetY->SetValue(ptrOffsetY->GetMin());

	CIntegerPtr ptrWidth = nodeMap.GetNode("Width");
	if (IsAvailable(ptrWidth) && IsWritable(ptrWidth))
		ptrWidth->SetValue(ptrWidth->GetMax());

	CIntegerPtr ptrHeight = nodeMap.GetNode("Height");
	if (IsAvailable(ptrHeight) && IsWritable(ptrHeight))
		ptrHeight->SetValue(ptrHeight->GetMax());

	return;
}

void ConfigureTrigger(INodeMap& nodeMap)
{
	CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
	CEnumEntryPtr ptrTriggerModeOff = ptrTriggerMode->GetEntryByName("Off");
	ptrTriggerMode->SetIntValue(ptrTriggerModeOff->GetValue());

	return ;
}

/*
Mat ConvertToCVmat(ImagePtr pImage)
{

	ImagePtr convertedImage = pImage->Convert(PixelFormat_Mono8);

	unsigned int XPadding = convertedImage->GetXPadding();
	unsigned int YPadding = convertedImage->GetYPadding();
	unsigned int rowsize = convertedImage->GetWidth();
	unsigned int colsize = convertedImage->GetHeight();

	//image data contains padding. When allocating Mat container size, you need to account for the X,Y image data padding. 
	Mat cvimg = Mat(colsize + YPadding, rowsize + XPadding, CV_8UC1, convertedImage->GetData(), convertedImage->GetStride());


	return cvimg;

}*/

void AcquireImage(CameraPtr cam, INodeMap& nodeMap)
{
	//AcquisitionMode setting
	CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
	CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");

	ptrAcquisitionMode->SetIntValue(ptrAcquisitionModeContinuous->GetValue());

	cam->BeginAcquisition();

	return ;
}

void ImageFormat(INodeMap& nodeMap)
{
	int height = 0, width = 0;

	CIntegerPtr ptrWidth = nodeMap.GetNode("Width");
	CIntegerPtr ptrHeight = nodeMap.GetNode("Height");
	if (IsAvailable(ptrWidth) && IsWritable(ptrWidth) && IsAvailable(ptrHeight) && IsWritable(ptrHeight))
	{
		//std::cout << "Height and width length" << endl;
		//cin >> height >> width;
		if (height == 0 || width == 0)
		{
			height = 2048;
			width = 1536;
		}
		ptrWidth->SetValue(width);
		ptrHeight->SetValue(height);
	}

	return;
}

void Exposure(INodeMap& nodeMap)
{
	CEnumerationPtr ptrExposureAuto = nodeMap.GetNode("ExposureAuto");
	CEnumEntryPtr ptrExposureAutoOff = ptrExposureAuto->GetEntryByName("Off");
	ptrExposureAuto->SetIntValue(ptrExposureAutoOff->GetValue());
	return;
}

void Gain(INodeMap& nodeMap)
{
	CEnumerationPtr ptrGainAuto = nodeMap.GetNode("GainAuto");
	CEnumEntryPtr ptrGainAutoOff = ptrGainAuto->GetEntryByName("Off");
	ptrGainAuto->SetIntValue(ptrGainAutoOff->GetValue());
	return;
}

void Framerate(INodeMap& nodeMap)
{
	CBooleanPtr ptrFramerateAuto = nodeMap.GetNode("AcquisitionFrameRateEnable");
	ptrFramerateAuto->SetValue(true);
	//CEnumEntryPtr ptrTriggerModeOff = ptrFramerateAuto->GetEntryByName("ON");
	//ptrFramerateAuto->SetIntValue(ptrTriggerModeOff->GetValue());

	return;
}

void Buffer(CameraPtr cam)
{
	INodeMap& snodeMap = cam->GetTLStreamNodeMap();
	CEnumerationPtr handlingmode = snodeMap.GetNode("StreamBufferHandlingMode");
	CEnumEntryPtr handlingmode_entry = handlingmode->GetEntryByName("NewestOnly");
	handlingmode->SetIntValue(handlingmode_entry->GetValue());

	return;
}
/*
void setup_cam(CameraPtr cam, INodeMap& nodeMap, int i)
{
	ConfigureTrigger(nodeMap);
	//ImageFormat(nodeMap);
	Exposure(nodeMap);
	Gain(nodeMap);
	ConfigureCustomImageSettings(nodeMap);
	Framerate(nodeMap);
	Buffer(cam);
	AcquireImage(cam, nodeMap);

	icam[i].ptrFramerate = nodeMap.GetNode("AcquisitionFrameRate");
	icam[i].fps_min = icam[i].ptrFramerate->GetMin();
	icam[i].fps_max = icam[i].ptrFramerate->GetMax();
	icam[i].fps = icam[i].ptrFramerate->GetValue();

	icam[i].ptrExposure = nodeMap.GetNode("ExposureTime");
	icam[i].exptime_min = icam[i].ptrExposure->GetMin();
	icam[i].exptime_max = icam[i].ptrExposure->GetMax();
	icam[i].exptime = icam[i].ptrExposure->GetValue();

	icam[i].ptrGain = nodeMap.GetNode("Gain");
	icam[i].gain_min = icam[i].ptrGain->GetMin();
	icam[i].gain_max = icam[i].ptrGain->GetMax();
	icam[i].gain = icam[i].ptrGain->GetValue();

}

void acquition_cam()
{
	bool set_val = false;

	if (numCameras == 0)
	{
		sw = 2;
		return;
	}
	SystemPtr system = System::GetInstance();
	CameraList camList = system->GetCameras();
	CameraPtr cam;
	ImagePtr pImage = NULL;
	int i = 0;

	cam = camList.GetByIndex(i);
	cam->Init();
	INodeMap& nodeMap = cam->GetNodeMap();

	setup_cam(cam, nodeMap, i);
	
	if (numCameras == 1)
	{
		sw = 1;
	}
	

	while (true)
	{
		pImage = cam->GetNextImage();
		image[i] = pImage;

		if (video_start[i] == 1)
		{
			record_image[i].push_back(pImage->Convert(PixelFormat_Mono8));
		}
		else
		{
			if (set_val == false)
			{
				icam[i].ptrFramerate->SetValue(icam[i].fps);
				icam[i].ptrExposure->SetValue(icam[i].exptime);
				icam[i].ptrGain->SetValue(icam[i].gain);
			}
			else if (set_val == true)
			{
				icam[i].fps = icam[i].ptrFramerate->GetValue();
				icam[i].exptime = icam[i].ptrExposure->GetValue();
				icam[i].gain = icam[i].ptrGain->GetValue();
			}
			set_val = !set_val;
		}

		if (capture == true && cap_image[i].empty() == true)
		{
			cap_image[i].push_back(pImage->Convert(PixelFormat_Mono8));
		}

		pImage->Release();
		
		if (sw == 2) 
		{
			break;
		}
	}

	CString str;
	str = "Capture end \r\nPress Enter...";
	print_log.push(str);


	cam->EndAcquisition();
	cam->DeInit();
	
	return;
}

void acquition_cam2()
{
	bool set_val = false;

	if (numCameras != 2)
	{
		//sw = 2;
		return;
	}

	SystemPtr system = System::GetInstance();
	CameraList camList = system->GetCameras();
	CameraPtr cam;
	ImagePtr pImage = NULL;
	int i = 1;



	cam = camList.GetByIndex(i);
	cam->Init();
	INodeMap& nodeMap = cam->GetNodeMap();

	setup_cam(cam, nodeMap, i);
	
	if (sw == 0)
	{
		sw = 1;
	}

	while (true)
	{

		pImage = cam->GetNextImage();
		image[i] = pImage;


		if (video_start[i] == 1)
		{
			record_image[i].push_back(pImage->Convert(PixelFormat_Mono8));
		}
		else
		{
			if (set_val == false)
			{
				icam[i].ptrFramerate->SetValue(icam[i].fps);
				icam[i].ptrExposure->SetValue(icam[i].exptime);
				icam[i].ptrGain->SetValue(icam[i].gain);

			}
			else if (set_val == true)
			{
				icam[i].fps = icam[i].ptrFramerate->GetValue();
				icam[i].exptime = icam[i].ptrExposure->GetValue();
				icam[i].gain = icam[i].ptrGain->GetValue();

			}
			set_val = !set_val;
		}

		if (capture == true && cap_image[i].empty() == true)
		{
			cap_image[i].push_back(pImage->Convert(PixelFormat_Mono8));
		}

		pImage->Release();
		
		
		
		if (sw == 2) 
		{
			break;
		}
	}

	CString str;
	str = "Capture end \r\nPress Enter...";
	print_log.push(str);


	cam->EndAcquisition();
	cam->DeInit();
	

	
	return;
}



*/