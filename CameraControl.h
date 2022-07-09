#pragma once


// CameraControl ダイアログ

class CameraControl : public CDialogEx
{
	DECLARE_DYNAMIC(CameraControl)

public:
	CameraControl(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CameraControl();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CameraControl };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void Init(int i);
	CSliderCtrl SliderExposureCam[2];
	CSliderCtrl SliderGainCam[2];
	CSliderCtrl SliderFramerateCam[2];
	CEdit EditExposureCam[2];
	CEdit EditGainCam[2];
	CEdit EditFramerateCam[2];
	afx_msg void OnNMCustomdrawExposureCam1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawGainCam1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawFramerateCam1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawExposureCam2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawGainCam2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawFramerateCam2(NMHDR* pNMHDR, LRESULT* pResult);
};
