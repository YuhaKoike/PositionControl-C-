#ifndef FLIRCAM_YK
#define FLIRCAM_YK

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace Spinnaker::Video;
using namespace std;
using namespace cv;
/*
void count_cam();
void acquition_cam();
void acquition_cam2();
*/

class FLIRCam
{
public:
	FLIRCam();
	~FLIRCam();

public:
	void FLIRtoCV(CameraPtr cam, Mat& outimg);
	void AcquireSetup(CameraPtr cam, INodeMap& nodeMap);
	void getCameraInfo(INodeMap& nodeMap, int i);

private:

};
#endif //