#pragma once

// cameralistDlg ダイアログ

class cameralistDlg : public CDialogEx
{
	DECLARE_DYNAMIC(cameralistDlg)

public:
	cameralistDlg(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~cameralistDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_cameralistDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	void Init();
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CListCtrl camlist;
	CButton B_reload;
	afx_msg void OnBnClickedreload();
	CButton bclear;
	afx_msg void OnBnClickedclear();
	CButton bstart;
	afx_msg void OnBnClickedstart();
	bool end_flag[2], start_flag[2], bottom, side;
	int camnumber[2];
	afx_msg void OnClose();
	CComboBox ComboBottom;
	CComboBox ComboSide;
	CStatic TextError;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangeCombo2();
};
