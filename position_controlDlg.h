
// position_controlDlg.h : ヘッダー ファイル
//

#pragma once


// CpositioncontrolDlg ダイアログ
class CpositioncontrolDlg : public CDialogEx
{
// コンストラクション
public:
	CpositioncontrolDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POSITION_CONTROL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit edit_log;
	CButton Da_Enable;
	afx_msg void OnClose();
	afx_msg void OnCheck_DA();
	CButton Record_Start;
	afx_msg void OnBnClickedRecStart();
	CButton Record_End;
	afx_msg void OnBnClickedRecEnd();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CComboBox combo_da;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CStatic text_record_num;
	CStatic text_saved_num;
	CEdit edit_maxvoltage;
	CEdit edit_minvoltage;
	CEdit edit_frequency;
	afx_msg void OnEnChangeMaxvoltage();
	afx_msg void OnEnChangeMinvoltage();
	afx_msg void OnEnChangeFrequency();
	CButton b_capture;
	afx_msg void OnBnClickedCapture();
	CEdit edit_kp;
	CEdit edit_kd;
	CEdit edit_ki;
	CButton cb_x_axis;
	CButton cb_y_axis;
	CMFCEditBrowseCtrl browse_box;
	afx_msg void OnEnChangeBox();
	afx_msg void OnBnClickedXaxis();
	afx_msg void OnBnClickedYaxis();
	afx_msg void OnEnChangePrecision();
	afx_msg void OnEnChangeTargetAngle();
	CEdit edit_prec_angle;
	CEdit edit_target_angle;
	afx_msg void OnEnChangeKp();
	afx_msg void OnEnChangeKd();
	afx_msg void OnEnChangeKi();
	CStatic now_angle_hol;
	CStatic now_angle_ver;
	CButton radio_horizontal;
	CButton radio_vertical;
	afx_msg void OnCbnCloseupDa();
	CButton cb_ccw;
	afx_msg void OnBnClickedCcw();
	CEdit edit_kp_ver;
	CEdit edit_ki_ver;
	CEdit edit_kd_ver;
	CEdit edit_target_angle2;
	CEdit edit_prec_angle2;
	afx_msg void OnEnChangeKpVer();
	afx_msg void OnEnChangeKdVer();
	afx_msg void OnEnChangeKiVer();
	afx_msg void OnEnChangeTargetAngle2();
	afx_msg void OnEnChangePrecision2();
	CButton b_rec_reset;
	afx_msg void OnBnClickedRecordReset();
	CStatic text_saved_num2;
	afx_msg void Onend();
	afx_msg void Onanalysis();
	afx_msg void Onopencamlist();
};
