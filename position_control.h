
// position_control.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CpositioncontrolApp:
// このクラスの実装については、position_control.cpp を参照してください
//

class CpositioncontrolApp : public CWinApp
{
public:
	CpositioncontrolApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CpositioncontrolApp theApp;

void CreateConsole(void);
