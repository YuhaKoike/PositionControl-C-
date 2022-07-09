// CameraControl.cpp : 実装ファイル
//

#include "pch.h"
#include "position_control.h"
#include "CameraControl.h"
#include "afxdialogex.h"

#include "lib_YK.h"

// CameraControl ダイアログ

IMPLEMENT_DYNAMIC(CameraControl, CDialogEx)

CameraControl::CameraControl(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CameraControl, pParent)
{

}

CameraControl::~CameraControl()
{
}

void CameraControl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, Slider_Exposure_Cam1, SliderExposureCam[0]);
	DDX_Control(pDX, Slider_Gain_Cam1, SliderGainCam[0]);
	DDX_Control(pDX, Slider_Framerate_Cam1, SliderFramerateCam[0]);
	DDX_Control(pDX, Edit_Exposure_Cam1, EditExposureCam[0]);
	DDX_Control(pDX, Edit_Gain_Cam1, EditGainCam[0]);
	DDX_Control(pDX, Edit_Framerate_Cam1, EditFramerateCam[0]);
	DDX_Control(pDX, Slider_Exposure_Cam2, SliderExposureCam[1]);
	DDX_Control(pDX, Slider_Gain_Cam2, SliderGainCam[1]);
	DDX_Control(pDX, Slider_Framerate_Cam2, SliderFramerateCam[1]);
	DDX_Control(pDX, Edit_Exposure_Cam2, EditExposureCam[1]);
	DDX_Control(pDX, Edit_Gain_Cam2, EditGainCam[1]);
	DDX_Control(pDX, Edit_Framerate_Cam2, EditFramerateCam[1]);

}


BEGIN_MESSAGE_MAP(CameraControl, CDialogEx)
	ON_NOTIFY(NM_CUSTOMDRAW, Slider_Exposure_Cam1, &CameraControl::OnNMCustomdrawExposureCam1)
	ON_NOTIFY(NM_CUSTOMDRAW, Slider_Gain_Cam1, &CameraControl::OnNMCustomdrawGainCam1)
	ON_NOTIFY(NM_CUSTOMDRAW, Slider_Framerate_Cam1, &CameraControl::OnNMCustomdrawFramerateCam1)
	ON_NOTIFY(NM_CUSTOMDRAW, Slider_Exposure_Cam2, &CameraControl::OnNMCustomdrawExposureCam2)
	ON_NOTIFY(NM_CUSTOMDRAW, Slider_Gain_Cam2, &CameraControl::OnNMCustomdrawGainCam2)
	ON_NOTIFY(NM_CUSTOMDRAW, Slider_Framerate_Cam2, &CameraControl::OnNMCustomdrawFramerateCam2)
END_MESSAGE_MAP()

BOOL CameraControl::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam)
		{
			if (numCameras == 0)
			{
				return FALSE;
			}

			CString str;
			int num;

			for (int i = 0; i < numCameras; i++)
			{
				EditExposureCam[i].GetWindowTextW(str);
				num = _tstof(str);
				SliderExposureCam[i].SetPos(num);

				EditGainCam[i].GetWindowTextW(str);
				num = _tstof(str) * 100;
				SliderGainCam[i].SetPos(num);

				EditFramerateCam[i].GetWindowTextW(str);
				num = _tstof(str) * 100;
				SliderFramerateCam[i].SetPos(num);
			}

			return FALSE;
		}
		else if (VK_ESCAPE == pMsg->wParam)
		{
			sw = 2;
			OnCancel();
		}
		else
		{
			return CDialogEx::PreTranslateMessage(pMsg);
		}
	}
	else {
		return CDialogEx::PreTranslateMessage(pMsg);
	}
}

// CameraControl メッセージ ハンドラー
void CameraControl::Init(int num)
{
	for (int i = 0; i < num; i++)
	{
		icam[i].exptime_max = 0.98 * 1000.0 * 1000.0 / icam[i].fps;

		SliderExposureCam[i].SetRange(icam[i].exptime_min, icam[i].exptime_max);
		SliderGainCam[i].SetRange(icam[i].gain_min * 100, icam[i].gain_max * 100);
		SliderFramerateCam[i].SetRange(icam[i].fps_min * 100, icam[i].fps_max * 100);


		SliderExposureCam[i].SetPos(int(icam[i].exptime));
		SliderGainCam[i].SetPos(int(icam[0].gain * 100));
		SliderFramerateCam[i].SetPos(int(icam[i].fps * 100));
	}

}


void CameraControl::OnNMCustomdrawExposureCam1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	int pos = SliderExposureCam[0].GetPos();
	CString str;
	str.Format(_T("%d"), pos);
	EditExposureCam[0].SetWindowTextW(str);
	icam[0].exptime = pos;

	*pResult = 0;
}


void CameraControl::OnNMCustomdrawGainCam1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int pos = SliderGainCam[0].GetPos();
	CString str;
	str.Format(_T("%.2f"), double(pos / 100.0));
	EditGainCam[0].SetWindowTextW(str);
	icam[0].gain = double(pos / 100.0);
	*pResult = 0;
}


void CameraControl::OnNMCustomdrawFramerateCam1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int pos = SliderFramerateCam[0].GetPos();
	CString str;
	str.Format(_T("%.2f"), double(pos / 100.0));
	EditFramerateCam[0].SetWindowTextW(str);
	icam[0].fps = double(pos / 100.0);

	icam[0].exptime_max = 0.98 * 1000.0 * 1000.0 / icam[0].fps;
	SliderExposureCam[0].SetRange(icam[0].exptime_min, icam[0].exptime_max);
	SliderExposureCam[0].SetPos(int(icam[0].exptime));
	str.Format(_T("%d"), int(icam[0].exptime));
	EditExposureCam[0].SetWindowTextW(str);
	*pResult = 0;
}


void CameraControl::OnNMCustomdrawExposureCam2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	int pos = SliderExposureCam[1].GetPos();
	CString str;
	str.Format(_T("%d"), pos);
	EditExposureCam[1].SetWindowTextW(str);
	icam[1].exptime = pos;

	*pResult = 0;
}


void CameraControl::OnNMCustomdrawGainCam2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int pos = SliderGainCam[1].GetPos();
	CString str;
	str.Format(_T("%.2f"), double(pos / 100.0));
	EditGainCam[1].SetWindowTextW(str);
	icam[1].gain = double(pos / 100.0);
	*pResult = 0;
}


void CameraControl::OnNMCustomdrawFramerateCam2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int pos = SliderFramerateCam[1].GetPos();
	CString str;
	str.Format(_T("%.2f"), double(pos / 100.0));
	EditFramerateCam[1].SetWindowTextW(str);
	icam[1].fps = double(pos / 100.0);

	icam[1].exptime_max = 0.98 * 1000.0 * 1000.0 / icam[1].fps;
	SliderExposureCam[1].SetRange(icam[1].exptime_min, icam[1].exptime_max);
	SliderExposureCam[1].SetPos(int(icam[1].exptime));
	str.Format(_T("%d"), int(icam[1].exptime));
	EditExposureCam[1].SetWindowTextW(str);
	*pResult = 0;
}

