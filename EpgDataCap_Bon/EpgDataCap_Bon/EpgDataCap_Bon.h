
// EpgDataCap_Bon.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#include "resource.h"		// メイン シンボル


// CEpgDataCap_BonApp:
// このクラスの実装については、EpgDataCap_Bon.cpp を参照してください。
//

class CEpgDataCap_BonApp
{
public:
	CEpgDataCap_BonApp();

public:
	BOOL InitInstance();
};

BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int value, LPCTSTR lpFileName);

extern CEpgDataCap_BonApp theApp;