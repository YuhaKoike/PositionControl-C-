#pragma once


// analysis ダイアログ

class analysisDlg : public CDialogEx
{
	DECLARE_DYNAMIC(analysisDlg)

public:
	analysisDlg(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~analysisDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_analysisDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	void Init();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnChangeBox();
	CMFCEditBrowseCtrl browse_box;
	CButton button_start;
	afx_msg void OnBnClickedStart();
	afx_msg void OnClose();
	CSliderCtrl slider_th1;
	afx_msg void OnNMCustomdrawTh2after(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawTh1after(NMHDR* pNMHDR, LRESULT* pResult);
	CSliderCtrl slider_th2;
	CEdit edit_th1;
	CEdit edit_th2;
	CButton b_roi_reset;
	afx_msg void OnBnClickedRoiResetafter();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	bool flag_end;
	int flag_init;
};
