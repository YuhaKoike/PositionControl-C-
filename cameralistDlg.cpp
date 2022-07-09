// cameralistDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "position_control.h"
#include "cameralistDlg.h"
#include "afxdialogex.h"

#include "lib_YK.h"

// cameralistDlg ダイアログ

IMPLEMENT_DYNAMIC(cameralistDlg, CDialogEx)

cameralistDlg::cameralistDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_cameralistDlg, pParent)
{

}

cameralistDlg::~cameralistDlg()
{
}

void cameralistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_camlist, camlist);
	DDX_Control(pDX, IDC_reload, B_reload);
	DDX_Control(pDX, B_clear, bclear);
	DDX_Control(pDX, B_start, bstart);
	DDX_Control(pDX, IDC_COMBO1, ComboBottom);
	DDX_Control(pDX, IDC_COMBO2, ComboSide);
	DDX_Control(pDX, IDC_TEXT_ERORR, TextError);
}


BEGIN_MESSAGE_MAP(cameralistDlg, CDialogEx)

	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_reload, &cameralistDlg::OnBnClickedreload)
	ON_BN_CLICKED(B_clear, &cameralistDlg::OnBnClickedclear)
	ON_BN_CLICKED(B_start, &cameralistDlg::OnBnClickedstart)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO1, &cameralistDlg::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, &cameralistDlg::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// cameralistDlg メッセージ ハンドラー
void cameralistDlg::Init()
{

	LVCOLUMN lvCol1;
	lvCol1.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;	// 配置, ヘッダ名, 幅
	lvCol1.fmt = LVCFMT_LEFT;
	lvCol1.pszText = _T("No.");
	lvCol1.cx = 40;
	camlist.InsertColumn(1, &lvCol1);

	LVCOLUMN lvCol2;
	lvCol2.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;	// 配置, ヘッダ名, 幅
	lvCol2.fmt = LVCFMT_CENTER;
	lvCol2.pszText = _T("Camera Name");
	lvCol2.cx = 220;
	camlist.InsertColumn(2, &lvCol2);

	LVCOLUMN lvCol3;
	lvCol3.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;	// 配置, ヘッダ名, 幅
	lvCol3.fmt = LVCFMT_CENTER;
	lvCol3.pszText = _T("ID");
	lvCol3.cx = 100;
	camlist.InsertColumn(3, &lvCol3);

	LVCOLUMN lvCol4;
	lvCol4.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol4.fmt = LVCFMT_CENTER;
	lvCol4.pszText = _T("Status");
	lvCol4.cx = 60;
	camlist.InsertColumn(4, &lvCol4);

	for (int i = 0; i < 2; i++) {
		end_flag[i] = false;
		start_flag[i] = false;
	}

	bottom = false;
	side = false;

	ComboBottom.AddString(_T("--------"));
	ComboSide.AddString(_T("--------"));
	ComboBottom.SetCurSel(0);
	ComboSide.SetCurSel(0);
}



int cameralistDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


void cameralistDlg::OnBnClickedreload()
{	
	camlist.DeleteAllItems();
	SystemPtr system = System::GetInstance();
	CameraList cameralist = system->GetCameras();

	ComboBottom.ResetContent();
	ComboSide.ResetContent();
	ComboBottom.AddString(_T("--------"));
	ComboSide.AddString(_T("--------"));
	ComboBottom.SetCurSel(0);
	ComboSide.SetCurSel(0);

	int num = cameralist.GetSize();
	
	if (num == 0)
	{
		system->ReleaseInstance();
		cameralist.Clear();
		return;
	}

	for (int i = 0; i < num; i++)
	{
		CameraPtr cam = cameralist.GetByIndex(i);
		cam->Init();
		INodeMap& nodeMap = cam->GetNodeMap();

		LVITEM lvItem1_1;
		lvItem1_1.mask = LVIF_TEXT;
		CString str;
		str.Format(_T("%d"), i);
		lvItem1_1.pszText = (LPTSTR)(LPCTSTR)str;
		lvItem1_1.iItem = i;
		lvItem1_1.iSubItem = 0;
		camlist.InsertItem(&lvItem1_1);

		LVITEM lvItem1_2;
		lvItem1_2.mask = LVIF_TEXT;
		CStringPtr node_devicename = nodeMap.GetNode("DeviceModelName");
		CString devicename = (CString)node_devicename->GetValue();
		lvItem1_2.pszText = (LPTSTR)(LPCTSTR)devicename;
		lvItem1_2.iItem = i;
		lvItem1_2.iSubItem = 1;
		camlist.SetItem(&lvItem1_2);

		LVITEM lvItem1_3;
		lvItem1_3.mask = LVIF_TEXT;
		CStringPtr node_devicenumber = nodeMap.GetNode("DeviceSerialNumber");
		int serial = stoi((String)node_devicenumber->GetValue());
		CString str_serial;
		str_serial.Format(_T("%d"), serial);
		lvItem1_3.pszText = (LPTSTR)(LPCTSTR)str_serial;
		lvItem1_3.iItem = i;
		lvItem1_3.iSubItem = 2;
		camlist.SetItem(&lvItem1_3);
		ComboBottom.AddString((LPCTSTR)str_serial);
		ComboSide.AddString((LPCTSTR)str_serial);

	}
}

void cameralistDlg::OnBnClickedclear()
{
	camlist.DeleteAllItems();
}

Mat dispImage[2], calcImage[2];
vector<Mat> recordImage[2];
mutex mt_disp, mt_calc;

void runCamera(cameralistDlg* dlg, int number, int i)
{
	Mat img;
	FLIRCam* FC;
	FC = new FLIRCam();

	SystemPtr system = System::GetInstance();
	CameraList camList = system->GetCameras();
	CameraPtr cam;

	cam = camList.GetByIndex(number);
	cam->Init();
	INodeMap& nodeMap = cam->GetNodeMap();

	FC->AcquireSetup(cam, nodeMap);
	FC->getCameraInfo(nodeMap, i);
	dlg->start_flag[i] = true;
	while (dlg->end_flag[i])
	{
		if (video_start[0] != 1)
		{
			icam[i].ptrFramerate->SetValue(icam[i].fps);
			icam[i].ptrExposure->SetValue(icam[i].exptime);
			icam[i].ptrGain->SetValue(icam[i].gain);
			icam[i].fps = icam[i].ptrFramerate->GetValue();
			icam[i].exptime = icam[i].ptrExposure->GetValue();
			icam[i].gain = icam[i].ptrGain->GetValue();
		}

		FC->FLIRtoCV(cam, img);
		lock_guard<std::mutex> lock(mt_calc);
		calcImage[i] = img.clone();

		if (video_start[i] == 1)
		{
			recordImage[i].push_back(img.clone());
		}
	}
	cam->EndAcquisition();
	cam->DeInit();
}

UINT Threadcam(LPVOID p)
{
	cameralistDlg* dlg = (cameralistDlg*)p;
	runCamera(dlg, dlg->camnumber[0], 0);
	return 0;
}

UINT ThreaddisplayImage(LPVOID p)
{
	cameralistDlg* dlg = (cameralistDlg*)p;
	displayImage(dlg, 0);
	return 0;
}

UINT ThreadcalcAngle(LPVOID p)
{
	cameralistDlg* dlg = (cameralistDlg*)p;
	calcAngle(dlg, 0);
	return 0;
}

UINT Threadcam2(LPVOID p)
{
	cameralistDlg* dlg = (cameralistDlg*)p;
	runCamera(dlg, dlg->camnumber[1], 1);
	return 0;
}

UINT ThreaddisplayImage2(LPVOID p)
{
	cameralistDlg* dlg = (cameralistDlg*)p;
	displayImage(dlg, 1);
	return 0;
}

UINT ThreadcalcAngle2(LPVOID p)
{
	cameralistDlg* dlg = (cameralistDlg*)p;
	calcAngle(dlg, 1);
	return 0;
}

void cameralistDlg::OnBnClickedstart()
{
	CString stre;
	TextError.GetWindowTextW(stre);
	if (stre == "Error")
	{
		return;
	}

	TCHAR tszText[256];
	String str = "";

	int itemnum = camlist.GetItemCount();
	//int numcam = 0;
	int i = 0;
	bool check[2] = { true, true };	//bottom, side

	CString strb, strs;
	int n = ComboBottom.GetCurSel();
	ComboBottom.GetLBText(n, strb);
	n = ComboSide.GetCurSel();
	ComboSide.GetLBText(n, strs);


	if (strb != "--------")
	{
		bottom = true;
		check[0] = false;
		numCameras++;
	}

	if (strs != "--------")
	{
		side = true;
		check[1] = false;
		numCameras++;
	}


	while (true)
	{
		camlist.GetItemText(i, 2, tszText, 256);
		str = CStringA(tszText).GetBuffer();
		if (strb == str.c_str())
		{
			camnumber[0] = i;
			check[0] = true;
		}

		if (strs == str.c_str())
		{
			camnumber[1] = i;
			check[1] = true;
		}

		i++;
		if (i > itemnum)
		{
			i = 0;
		}
		if (check[0] == true && check[1] == true)
		{
			break;
		}
	}


	if (numCameras == 0)
	{
		return;
	}
	else
	{

		if (end_flag[0] == false || end_flag[1] == false) 
		{
			sw = 1;
			
			end_flag[0] = true;
			start_flag[0] = false;

			if (bottom == true)
			{
				AfxBeginThread(Threadcam, this);
				AfxBeginThread(ThreaddisplayImage, this);
				AfxBeginThread(ThreadcalcAngle, this);
				volatile int nn = 0;
				while (!start_flag[0])
				{
					nn = 0;
				}
			}

			if (side == true)
			{
				end_flag[1] = true;
				start_flag[1] = false;

				AfxBeginThread(Threadcam2, this);
				AfxBeginThread(ThreaddisplayImage2, this);
				AfxBeginThread(ThreadcalcAngle2, this);
				volatile int nn = 0;
				while (!start_flag[1])
				{
					nn = 0;
				}
			}

			CameraControl* dlg;
			dlg = new CameraControl(this);
			dlg->Initialize();
			dlg->Create(IDD_CameraControl, this);
			dlg->ShowWindow(SW_SHOW);
			dlg->Init(numCameras);
		}
	}
}

void cameralistDlg::OnClose()
{
	for (int i = 0; i < 2; i++) {
		end_flag[i] = false;
	}
	bottom = false;
	side = false;
	numCameras = 0;
	
	CDialogEx::OnClose();
}

void cameralistDlg::OnCbnSelchangeCombo1()
{
	CString strb, strs;
	int n = ComboBottom.GetCurSel();
	ComboBottom.GetLBText(n, strb);
	n = ComboSide.GetCurSel();
	ComboSide.GetLBText(n, strs);
	if (strb == strs && strb != "--------")
	{
		TextError.SetWindowTextW(_T("Error"));
	}
	else
	{
		TextError.SetWindowTextW(_T("--------"));
	}

}

void cameralistDlg::OnCbnSelchangeCombo2()
{
	CString strb, strs;
	int n = ComboBottom.GetCurSel();
	ComboBottom.GetLBText(n, strb);
	n = ComboSide.GetCurSel();
	ComboSide.GetLBText(n, strs);
	if (strb == strs && strb != "--------")
	{
		TextError.SetWindowTextW(_T("Error"));
	}
	else
	{
		TextError.SetWindowTextW(_T("--------"));
	}
}
