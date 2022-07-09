
// position_controlDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "position_control.h"
#include "position_controlDlg.h"
#include "afxdialogex.h"

//add by YK
#include "lib_YK.h"
#include <thread>

queue <int> record_num, save_num[2];
queue <CString> print_log;
string folder;
string video_name;

//csv
bool csv_write = false;
int csv_start = -1;	//-1: init, 0: start, 1: end, 2: saving
system_clock::time_point tm_start, tm_end, tm_lap, voltage_start;
string filename, pathname;
ofstream csv_file;
int csv_n = 0;

//angle
auto elapsed_bef = 0;
double re_angle_bef[2] = { 0.0,0.0 };

UINT ThreadSaveCsv(LPVOID p);
UINT ThreadSaveVideo(LPVOID p);
UINT ThreadDA(LPVOID p);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CpositioncontrolDlg ダイアログ

CpositioncontrolDlg::CpositioncontrolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_POSITION_CONTROL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CpositioncontrolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, Edit_Log, edit_log);
	DDX_Control(pDX, CB_Da_Enable, Da_Enable);
	DDX_Control(pDX, B_Rec_Start, Record_Start);
	DDX_Control(pDX, B_Rec_End, Record_End);
	DDX_Control(pDX, Combo_Da, combo_da);
	DDX_Control(pDX, Text_Record_num, text_record_num);
	DDX_Control(pDX, Text_Saved_num, text_saved_num);
	DDX_Control(pDX, Edit_MaxVoltage, edit_maxvoltage);
	DDX_Control(pDX, Edit_MinVoltage, edit_minvoltage);
	DDX_Control(pDX, Edit_Frequency, edit_frequency);
	DDX_Control(pDX, B_Capture, b_capture);
	DDX_Control(pDX, Edit_Kp, edit_kp);
	DDX_Control(pDX, Edit_Kd, edit_kd);
	DDX_Control(pDX, Edit_Ki, edit_ki);
	DDX_Control(pDX, IDC_EDIT2, edit_target_angle);
	DDX_Control(pDX, IDC_EDIT3, edit_prec_angle);
	DDX_Control(pDX, CB_X_axis, cb_x_axis);
	DDX_Control(pDX, CB_Y_axis, cb_y_axis);
	DDX_Control(pDX, Browse_Box, browse_box);
	DDX_Control(pDX, Edit_Precision, edit_prec_angle);
	DDX_Control(pDX, Edit_Target_Angle, edit_target_angle);
	DDX_Control(pDX, Now_Angle, now_angle_hol);
	DDX_Control(pDX, Now_Angle2, now_angle_ver);
	DDX_Control(pDX, Radio_Horizontal, radio_horizontal);
	DDX_Control(pDX, Radio_Vertical, radio_vertical);
	DDX_Control(pDX, CB_CCW, cb_ccw);
	DDX_Control(pDX, Edit_Kp_Ver, edit_kp_ver);
	DDX_Control(pDX, Edit_Ki_Ver, edit_ki_ver);
	DDX_Control(pDX, Edit_Kd_Ver, edit_kd_ver);
	DDX_Control(pDX, Edit_Target_Angle2, edit_target_angle2);
	DDX_Control(pDX, Edit_Precision2, edit_prec_angle2);
	DDX_Control(pDX, B_Record_Reset, b_rec_reset);
	DDX_Control(pDX, Text_Saved_num2, text_saved_num2);
}


BEGIN_MESSAGE_MAP(CpositioncontrolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(CB_Da_Enable, &CpositioncontrolDlg::OnCheck_DA)
	ON_BN_CLICKED(B_Rec_Start, &CpositioncontrolDlg::OnBnClickedRecStart)
	ON_BN_CLICKED(B_Rec_End, &CpositioncontrolDlg::OnBnClickedRecEnd)
	ON_WM_TIMER()
	ON_EN_CHANGE(Edit_MaxVoltage, &CpositioncontrolDlg::OnEnChangeMaxvoltage)
	ON_EN_CHANGE(Edit_MinVoltage, &CpositioncontrolDlg::OnEnChangeMinvoltage)
	ON_EN_CHANGE(Edit_Frequency, &CpositioncontrolDlg::OnEnChangeFrequency)
	ON_BN_CLICKED(B_Capture, &CpositioncontrolDlg::OnBnClickedCapture)
	ON_EN_CHANGE(Browse_Box, &CpositioncontrolDlg::OnEnChangeBox)
	ON_BN_CLICKED(CB_X_axis, &CpositioncontrolDlg::OnBnClickedXaxis)
	ON_BN_CLICKED(CB_Y_axis, &CpositioncontrolDlg::OnBnClickedYaxis)
	ON_EN_CHANGE(Edit_Precision, &CpositioncontrolDlg::OnEnChangePrecision)
	ON_EN_CHANGE(Edit_Target_Angle, &CpositioncontrolDlg::OnEnChangeTargetAngle)
	ON_EN_CHANGE(Edit_Kp, &CpositioncontrolDlg::OnEnChangeKp)
	ON_EN_CHANGE(Edit_Kd, &CpositioncontrolDlg::OnEnChangeKd)
	ON_EN_CHANGE(Edit_Ki, &CpositioncontrolDlg::OnEnChangeKi)
	ON_CBN_SELCHANGE(Combo_Da, &CpositioncontrolDlg::OnCbnCloseupDa)
	ON_BN_CLICKED(CB_CCW, &CpositioncontrolDlg::OnBnClickedCcw)
	ON_EN_CHANGE(Edit_Kp_Ver, &CpositioncontrolDlg::OnEnChangeKpVer)
	ON_EN_CHANGE(Edit_Kd_Ver, &CpositioncontrolDlg::OnEnChangeKdVer)
	ON_EN_CHANGE(Edit_Ki_Ver, &CpositioncontrolDlg::OnEnChangeKiVer)
	ON_EN_CHANGE(Edit_Target_Angle2, &CpositioncontrolDlg::OnEnChangeTargetAngle2)
	ON_EN_CHANGE(Edit_Precision2, &CpositioncontrolDlg::OnEnChangePrecision2)
	ON_BN_CLICKED(B_Record_Reset, &CpositioncontrolDlg::OnBnClickedRecordReset)
	ON_COMMAND(ID_end, &CpositioncontrolDlg::Onend)
	ON_COMMAND(ID_analysis, &CpositioncontrolDlg::Onanalysis)
	ON_COMMAND(ID_opencamlist, &CpositioncontrolDlg::Onopencamlist)

END_MESSAGE_MAP()


// CpositioncontrolDlg メッセージ ハンドラー

BOOL CpositioncontrolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	CString str;

	// コンボボックスに項目を追加します。
	combo_da.AddString(_T("Manual"));
	combo_da.AddString(_T("Auto"));
	combo_da.SetCurSel(1);

	str.Format(_T("%d"), freq);
	edit_frequency.SetWindowTextW(str);
			
	str.Format(_T("%.2f"), max_voltage);
	edit_maxvoltage.SetWindowTextW(str);

	str.Format(_T("%.2f"), min_voltage);
	edit_minvoltage.SetWindowTextW(str);


	str.Format(_T("%2.8f"), Kp);
	edit_kp.SetWindowTextW(str);
	str.Format(_T("%2.8f"), Ki);
	edit_ki.SetWindowTextW(str);
	str.Format(_T("%2.8f"), Kd);
	edit_kd.SetWindowTextW(str);

	str.Format(_T("%2.8f"), Kp_ver);
	edit_kp_ver.SetWindowTextW(str);
	str.Format(_T("%2.8f"), Ki_ver);
	edit_ki_ver.SetWindowTextW(str);
	str.Format(_T("%2.8f"), Kd_ver);
	edit_kd_ver.SetWindowTextW(str);

	str.Format(_T("%.2f"), t_angle[0]);
	edit_target_angle.SetWindowTextW(str);

	str.Format(_T("%.2f"), t_angle[1]);
	edit_target_angle2.SetWindowTextW(str);

	str.Format(_T("%.2f"), prec_angle[0]);
	edit_prec_angle.SetWindowTextW(str);

	str.Format(_T("%.2f"), prec_angle[1]);
	edit_prec_angle2.SetWindowTextW(str);

	radio_horizontal.EnableWindow(FALSE);
	radio_vertical.EnableWindow(FALSE);

	b_rec_reset.EnableWindow(FALSE);
	//browse_box.SetWindowTextW(L"A");

	SetTimer(1, 1, NULL);
	AfxBeginThread(ThreadSaveVideo, this);
	AfxBeginThread(ThreadDA, this);
	AfxBeginThread(ThreadSaveCsv, this);

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CpositioncontrolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CpositioncontrolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CpositioncontrolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CpositioncontrolDlg::OnClose()
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	sw = 2;
	CDialogEx::OnClose();
}

UINT ThreadSaveCsv(LPVOID p)
{
	save_csv();
	return 0;
}

UINT ThreadSaveVideo(LPVOID p)
{
	record();
	return 0;
}

UINT ThreadDA(LPVOID p)
{
	da_output();
	return 0;
}

BOOL CpositioncontrolDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam)
		{
			if (numCameras == 0)
			{
				return FALSE;
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

void CpositioncontrolDlg::OnCheck_DA()
{
	if (Da_Enable.GetCheck())
	{
		da_enable = true;

		CString str;
		combo_da.GetWindowTextW(str);

		if (str == "Auto")
		{
			if (radio_horizontal.GetCheck())
			{
				hov = 0;
			}
			else if (radio_vertical.GetCheck())
			{
				hov = 1;
			}
			else
			{
				hov = 0;
				radio_horizontal.SetCheck(BST_CHECKED);
			}


			voltage_bef = 0.0;
			da_auto = 1;

			voltage_start = system_clock::now();
		}
		else if (str == "Manual")
		{

			if (cb_x_axis.GetCheck())
			{
				x_axis = true;
			}

			if (cb_y_axis.GetCheck())
			{
				y_axis = true;
			}

			if (cb_ccw.GetCheck())
			{
				ccw = true;
			}

			da_auto = 2;
			voltage_bef = -1;
		}
		//edit_log.SetWindowTextW(str);
		return;

	}
	else if (!Da_Enable.GetCheck())
	{
		da_enable = false;
		da_auto = 0;
		voltage_bef = -1;
		x_axis = false;
		y_axis = false;
		ccw = false;
		hov = -1;
		init_vol = false;
	}
}


void CpositioncontrolDlg::OnBnClickedXaxis()
{
	if (cb_x_axis.GetCheck())
	{
		x_axis = true;
	}
	else if (!cb_x_axis.GetCheck())
	{
		x_axis = false;
	}
	voltage_bef = -1;
}

void CpositioncontrolDlg::OnBnClickedYaxis()
{
	if (cb_y_axis.GetCheck())
	{
		y_axis = true;
	}
	else if (!cb_y_axis.GetCheck())
	{
		y_axis = false;
	}
	voltage_bef = -1;
}

void CpositioncontrolDlg::OnBnClickedRecStart()
{
	struct tm l_time;

	time_t t = time(NULL);
	localtime_s(&l_time, &t);
	const tm* localTime = &l_time;
	stringstream s;

	s << "20" << localTime->tm_year - 100;
	// setw(),setfill()で0詰め
	s << setw(2) << setfill('0') << localTime->tm_mon + 1;
	s << setw(2) << setfill('0') << localTime->tm_mday;
	s << setw(2) << setfill('0') << localTime->tm_hour;
	s << setw(2) << setfill('0') << localTime->tm_min;
	s << setw(2) << setfill('0') << localTime->tm_sec;
	// std::stringにして値を返す
	filename = s.str();

	if (sw == 1)
	{
		//fps = ptrFramerate->GetValue();
		for (int i = 0; i < numCameras; i++)
		{
			video_start[i] = 1;
		}
		CString str;
		str = "record start";
		print_log.push(str);
	}

	if (radio_horizontal.GetCheck())
	{
		hov = 0;
	}
	else if (radio_vertical.GetCheck())
	{
		hov = 1;
	}
	else
	{
		hov = 0;
		radio_horizontal.SetCheck(BST_CHECKED);
	}

	if (folder == "")
	{
		csv_file.open("..\\saved_file\\" + filename + ".csv");
	}
	else
	{
		csv_file.open(folder + "\\" + filename + ".csv");
	}
	
	for (int i = 0; i < numCameras; i++)
	{
		csv_info csv;
		csv.elapsed = 0;
		csv.voltage = voltage;
		csv.voltage_time = 0;
		csv.reangle = re_angle[i];
		csv.quadrant = quadrant[i];
		csv.angular_speed = 0.0;

		csv_vec[i].push_back(csv);
	}
	


	if (numCameras == 2)
	{
		csv_file << "Bottom Veiw,,,,,,," << "Side View" << endl;
		csv_file << "Time [ms]," << "Voltage Applied Time [ms]," << "Angle [°]," << "Quadrant," << "Voltage [V]," << "Angular Speed [°/s],,";
		csv_file << "Voltage Applied Time [ms]," << " Angle [°]," << "Quadrant," << "Voltage [V]," << "Angular Speed [°/s]," << "p1x,p1y,p2x,p2y" << endl;
	}
	else
	{
		csv_file << "Bottom View" << endl;
		csv_file << "Time [ms]," << "Voltage Applied Time [ms]," << "Angle [°]," << "Quadrant," << "Voltage [V]," << "Angular Speed [°/s]," << "p1x,p1y,p2x,p2y" << endl;
	}

	for (int i = 0; i < 2; i++)
	{
		re_angle_bef[i] = re_angle[i];
	}


	tm_start = system_clock::now();
	tm_lap = tm_start;
	csv_start = 0;

	CString str;
	str = "0";
	text_record_num.SetWindowTextW(str);
	text_saved_num.SetWindowTextW(str);
	text_saved_num2.SetWindowTextW(str);

	Record_Start.EnableWindow(FALSE);
}

void CpositioncontrolDlg::OnBnClickedRecEnd()
{
	for (int i = 0; i < numCameras; i++)
	{
		video_start[i] = 2;
	}
	csv_start = 1;

	if (numCameras != 0)
	{
		b_rec_reset.EnableWindow(TRUE);
	}

	Record_Start.EnableWindow(TRUE);

	CString str;
	str.Format(_T("%d"), csv_vec[0].size());
	print_log.push(CString("CSV Size: ") + str);

}
system_clock::time_point start, t_end;

void CpositioncontrolDlg::OnTimer(UINT_PTR nIDEvent)
{
	start = system_clock::now();
	if (print_log.empty() == false)
	{
		CString str = print_log.front();
		print_log.pop();
		
		edit_log.ReplaceSel(str + L"\r\n");

		if (str == "record end")
		{
			b_rec_reset.EnableWindow(FALSE);
		}
	}
	
	if (record_num.empty() == false)
	{
		CString str;
		str.Format(_T("%d"), record_num.front());
		record_num.pop();
		text_record_num.SetWindowTextW(str);
	}

	for (int j = 0; j < numCameras; j++)
	{
		if (save_num[j].empty() == false)
		{
			CString str;
			str.Format(_T("%d"), save_num[j].front());
			save_num[j].pop();
			if (j == 0)
			{
				text_saved_num.SetWindowTextW(str);
			}
			else if (j == 1)
			{
				text_saved_num2.SetWindowTextW(str);
			}

		}
	}


	if (Da_Enable.GetCheck())
	{
		CString str;
		str.Format(_T("%3.2f"), voltage);

	}
	
	if (radio_horizontal.GetCheck())
	{
		CString str;
		str.Format(_T("%3.2f"), re_angle[0]);
		now_angle_hol.SetWindowTextW(str);
	}

	if (radio_vertical.GetCheck())
	{
		CString str;
		str.Format(_T("%3.2f"), re_angle[1]);
		now_angle_ver.SetWindowTextW(str);
	}

	if (da_enable == false)
	{
		if (Da_Enable.GetCheck())
		{
			Da_Enable.SetCheck(BST_UNCHECKED);
			da_enable = false;
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CpositioncontrolDlg::OnEnChangeMaxvoltage()
{
	CString str;
	edit_maxvoltage.GetWindowTextW(str);
	double vol = _tstof(str);
	if (str != "")
	{
		if (0.0 < vol && vol < 5.0)
		{
			max_voltage = vol;
		}
		else
		{
			max_voltage = 4.0;
			edit_maxvoltage.SetWindowTextW(L"4.0");
		}
	}

}


void CpositioncontrolDlg::OnEnChangeMinvoltage()
{
	CString str;
	edit_minvoltage.GetWindowTextW(str);

	min_voltage = _tstof(str);
}


void CpositioncontrolDlg::OnEnChangeFrequency()
{
	if (!Da_Enable.GetCheck())
	{
		CString str;
		edit_frequency.GetWindowTextW(str);

		freq = _tstof(str);
	}

}


void CpositioncontrolDlg::OnBnClickedCapture()
{
	if (sw == 1)
	{
		if (capture == false)
		{
			capture = true;
		}
	}

}


void CpositioncontrolDlg::OnEnChangeBox()
{
	CString str;
	str = "saved folder changed for";
	print_log.push(str);
	browse_box.GetWindowTextW(str);
	print_log.push(str);
	wstring wstr = str.GetBuffer();
	string s(wstr.begin(), wstr.end());
	folder = s;
}

void CpositioncontrolDlg::OnEnChangePrecision()
{
	CString str;
	edit_prec_angle.GetWindowTextW(str);
	double angle = _tstof(str);

	if (str != "")
	{
		if (0.0 <= angle && angle <= 180.0)
		{
			prec_angle[0] = angle;
		}
		else
		{
			prec_angle[0] = 0.0;
			edit_prec_angle.SetWindowTextW(L"0.0");
		}
	}
	else
	{
		prec_angle[0] = 0.0;
		edit_prec_angle.SetWindowTextW(L"0.0");
	}
}

void CpositioncontrolDlg::OnEnChangeTargetAngle()
{
	CString str;
	edit_target_angle.GetWindowTextW(str);
	double angle = _tstof(str);
	if (str != "")
	{
		if (0.0 <= angle && angle <= 360.0)
		{
			if (0.0 <= angle && angle <= 180.0)
			{
				t_angle[0] = angle;
			}
			else if (180.0 < angle && angle < 360)
			{
				t_angle[0] = angle - 360.0;
			}
		}
		else
		{
			t_angle[0] = 0.0;
			edit_target_angle.SetWindowTextW(L"0.0");
		}

	}
	else
	{
		t_angle[0] = 0.0;
		edit_target_angle.SetWindowTextW(L"0.0");
	}
}


void CpositioncontrolDlg::OnEnChangeKp()
{
	CString str;
	edit_kp.GetWindowTextW(str);
	if (str != "")
	{
		Kp = _tstof(str);
	}
	else
	{
		Kp = 0.0;
		edit_kp.SetWindowTextW(L"0.0");
	}
}


void CpositioncontrolDlg::OnEnChangeKd()
{
	CString str;
	edit_kd.GetWindowTextW(str);
	if (str != "")
	{
		Kd = _tstof(str);
	}
	else
	{
		Kd = 0.0;
		edit_kd.SetWindowTextW(L"0.0");
	}
}


void CpositioncontrolDlg::OnEnChangeKi()
{
	CString str;
	edit_ki.GetWindowTextW(str);
	if (str != "")
	{
		Ki = _tstof(str);
	}
	else
	{
		Ki = 0.0;
		edit_ki.SetWindowTextW(L"0.0");
	}
}

void CpositioncontrolDlg::OnCbnCloseupDa()
{
	CString str;
	combo_da.GetWindowTextW(str);

	if (str != "Auto")
	{
		radio_horizontal.EnableWindow(TRUE);
		radio_vertical.EnableWindow(TRUE);
	}
	else
	{
		radio_horizontal.EnableWindow(FALSE);
		radio_vertical.EnableWindow(FALSE);
	}

}

void CpositioncontrolDlg::OnBnClickedCcw()
{
	if (cb_ccw.GetCheck())
	{
		ccw = true;
	}
	else if (!cb_ccw.GetCheck())
	{
		ccw = false;
	}
	voltage_bef = -1;
}

void CpositioncontrolDlg::OnEnChangeKpVer()
{
	CString str;
	edit_kp_ver.GetWindowTextW(str);
	if (str != "")
	{
		Kp_ver = _tstof(str);
	}
	else
	{
		Kp_ver = 0.0;
		edit_kp_ver.SetWindowTextW(L"0.0");
	}
}


void CpositioncontrolDlg::OnEnChangeKdVer()
{
	CString str;
	edit_kd_ver.GetWindowTextW(str);
	if (str != "")
	{
		Kd_ver = _tstof(str);
	}
	else
	{
		Kd_ver = 0.0;
		edit_kd_ver.SetWindowTextW(L"0.0");
	}
}


void CpositioncontrolDlg::OnEnChangeKiVer()
{
	CString str;
	edit_ki_ver.GetWindowTextW(str);
	if (str != "")
	{
		Ki_ver = _tstof(str);
	}
	else
	{
		Ki_ver = 0.0;
		edit_ki_ver.SetWindowTextW(L"0.0");
	}
}


void CpositioncontrolDlg::OnEnChangeTargetAngle2()
{
	CString str;
	edit_target_angle2.GetWindowTextW(str);
	double angle = _tstof(str);
	if (str != "")
	{
		if (0.0 <= angle && angle <= 360.0)
		{
			if (0.0 <= angle && angle <= 180.0)
			{
				t_angle[1] = angle;
			}
			else if (180.0 < angle && angle < 360)
			{
				t_angle[1] = angle - 360.0;
			}
		}
		else
		{
			t_angle[1] = 0.0;
			edit_target_angle2.SetWindowTextW(L"0.0");
		}

	}
	else
	{
		t_angle[1] = 0.0;
		edit_target_angle2.SetWindowTextW(L"0.0");
	}
}


void CpositioncontrolDlg::OnEnChangePrecision2()
{
	CString str;
	edit_prec_angle2.GetWindowTextW(str);
	double angle = _tstof(str);

	if (str != "")
	{
		if (0.0 <= angle && angle <= 180.0)
		{
			prec_angle[1] = angle;
		}
		else
		{
			prec_angle[1] = 0.0;
			edit_prec_angle2.SetWindowTextW(L"0.0");
		}
	}
	else
	{
		prec_angle[1] = 0.0;
		edit_prec_angle2.SetWindowTextW(L"0.0");
	}
}


void CpositioncontrolDlg::OnBnClickedRecordReset()
{
	for (int j = 0; j < numCameras; j++)
	{
		CString str;
		str = "record end";
		print_log.push(str);
		video_start[j] = 0;
		setup_video[j] = false;
		rec_num[j] = 0;
		recordImage[j].clear();

		str = "save csv file end";
		print_log.push(str);

		csv_start = -1;
		csv_file.close();
		for (int i = 0; i < 2; i++)
		{
			csv_vec[i].clear();
		}
	}

	b_rec_reset.EnableWindow(FALSE);

}


void CpositioncontrolDlg::Onend()
{
	sw = 2;
	CDialog::EndDialog(0);
}



void CpositioncontrolDlg::Onanalysis()
{
	analysisDlg* dlg;
	dlg = new analysisDlg(this);
	//CWnd* wind = &dlg;
	//dlg->DoModal();
	
	dlg->Initialize();
	dlg->Create(IDD_analysisDlg, this);
	dlg->ShowWindow(SW_SHOW);
	dlg->Init();
	//AfxBeginThread(ThreadAnalysisFeature, this);
}



void CpositioncontrolDlg::Onopencamlist()
{
	cameralistDlg* dlg;
	dlg = new cameralistDlg(this);

	dlg->Initialize();
	dlg->Create(IDD_cameralistDlg, this);
	dlg->ShowWindow(SW_SHOW);
	dlg->Init();
}
