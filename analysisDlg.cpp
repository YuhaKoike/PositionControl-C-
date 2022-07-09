// analysis.cpp : 実装ファイル
//

#include "pch.h"
#include "position_control.h"
#include "analysisDlg.h"
#include "afxdialogex.h"

#include "lib_YK.h"

UINT ThreadAnalysisFeature(LPVOID p);

// analysis ダイアログ

IMPLEMENT_DYNAMIC(analysisDlg, CDialogEx)

analysisDlg::analysisDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_analysisDlg, pParent)
{

}

analysisDlg::~analysisDlg()
{
}

void analysisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, Browse_Box, browse_box);
	DDX_Control(pDX, Button_Start, button_start);
	DDX_Control(pDX, Slider_Th1_after, slider_th1);
	DDX_Control(pDX, Slider_Th2_after, slider_th2);
	DDX_Control(pDX, Edit_Th1_after, edit_th1);
	DDX_Control(pDX, Edit_Th2_after, edit_th2);
	DDX_Control(pDX, B_ROI_Reset_after, b_roi_reset);
}


BEGIN_MESSAGE_MAP(analysisDlg, CDialogEx)

	ON_EN_CHANGE(Browse_Box, &analysisDlg::OnEnChangeBox)
	ON_EN_CHANGE(Browse_Box, &analysisDlg::OnEnChangeBox)
	ON_BN_CLICKED(Button_Start, &analysisDlg::OnBnClickedStart)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CUSTOMDRAW, Slider_Th2_after, &analysisDlg::OnNMCustomdrawTh2after)
	ON_NOTIFY(NM_CUSTOMDRAW, Slider_Th1_after, &analysisDlg::OnNMCustomdrawTh1after)
	ON_BN_CLICKED(B_ROI_Reset_after, &analysisDlg::OnBnClickedRoiResetafter)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// analysis メッセージ ハンドラー
void analysisDlg::Init()
{
	flag_end = false;
	flag_init = 0;
	AfxBeginThread(ThreadAnalysisFeature, this);

	slider_th1.SetRange(0, 255);
	slider_th2.SetRange(0, 255);

	slider_th1.SetPos(th_cam1[0]);
	slider_th2.SetPos(th_cam1[1]);
	
}

BOOL analysisDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_ESCAPE == pMsg->wParam)
		{
			//sw = 2;
			OnCancel();
		}
		else
		{
			return CDialogEx::PreTranslateMessage(pMsg);
		}
	}
	else 
	{
		return CDialogEx::PreTranslateMessage(pMsg);
	}
}

void analysisDlg::OnEnChangeBox()
{
	CString str;
	str = "select video name is ...";
	print_log.push(str);
	browse_box.GetWindowTextW(str);
	wstring wstr = str.GetBuffer();
	string s(wstr.begin(), wstr.end());
	int path_i = s.find_last_of("\\") + 1;	//7
	int ext_i = s.find_last_of(".");		//10

	pathname = s.substr(0, path_i + 1);
	filename = s.substr(path_i, ext_i - path_i);
	string extname = s.substr(ext_i, s.size() - ext_i);
	string name = filename + extname;
	str = name.c_str();

	print_log.push(str);

	video_name = s;
	flag_init = 1;
}


void analysisDlg::OnBnClickedStart()
{
	pre_analysis = 3;
}


void analysisDlg::OnClose()
{
	flag_end = true;
	CDialogEx::OnClose();
}



void analysisDlg::OnNMCustomdrawTh1after(NMHDR* pNMHDR, LRESULT* pResult)
{


	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int pos = slider_th1.GetPos();
	CString str;
	str.Format(_T("%d"), pos);
	edit_th1.SetWindowTextW(str);
	th_cam1[0] = pos;
	*pResult = 0;

}


void analysisDlg::OnNMCustomdrawTh2after(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int pos = slider_th2.GetPos();
	CString str;
	str.Format(_T("%d"), pos);
	edit_th2.SetWindowTextW(str);
	th_cam1[1] = pos;
	*pResult = 0;
}


void analysisDlg::OnBnClickedRoiResetafter()
{
	roi_ent[0] = false;
	roi_point[4] = 0;
}

UINT ThreadAnalysisFeature(LPVOID p)
{
	AnalysisFeature(p);
	return 0;
}

int analysisDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


