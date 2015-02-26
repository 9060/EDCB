#pragma once

#include "EpgDBManager.h"
#include "ReserveManager.h"
#include "FileStreamingManager.h"
#include "NotifyManager.h"
#include "../../Common/ParseTextInstances.h"

//�e��T�[�o�Ǝ����\��̊Ǘ��������Ȃ�
//�K���I�u�W�F�N�g������Main()���c���j���̏��Ԃŗ��p���Ȃ���΂Ȃ�Ȃ�
class CEpgTimerSrvMain
{
public:
	CEpgTimerSrvMain();
	~CEpgTimerSrvMain();
	//���C�����[�v����
	//serviceFlag_: �T�[�r�X�Ƃ��Ă̋N�����ǂ���
	bool Main(bool serviceFlag_);
	//���C��������~
	void StopMain();
	//�x�~�^�X�^���o�C�Ɉڍs���č\��Ȃ��󋵂��ǂ���
	bool IsSuspendOK(); //const;
private:
	void ReloadSetting();
	//���݂̗\���Ԃɉ��������A�^�C�}���Z�b�g����
	bool SetResumeTimer(HANDLE* resumeTimer, __int64* resumeTime, DWORD marginSec);
	//�V�X�e�����V���b�g�_�E������
	static void SetShutdown(BYTE shutdownMode);
	//GUI�ɃV���b�g�_�E���\���ǂ����̖₢���킹���J�n������
	//suspendMode==0:�ċN��(���rebootFlag==1�Ƃ���)
	//suspendMode!=0:�X�^���o�C�x�~�܂��͓d���f
	bool QueryShutdown(BYTE rebootFlag, BYTE suspendMode);
	//���[�U�[��PC���g�p�����ǂ���
	bool IsUserWorking() const;
	//�}�������̃v���Z�X���N�����Ă��邩�ǂ���
	bool IsFindNoSuspendExe() const;
	bool AutoAddReserveEPG(const EPG_AUTO_ADD_DATA& data);
	bool AutoAddReserveProgram(const MANUAL_AUTO_ADD_DATA& data);
	//�O������R�}���h�֌W
	static int CALLBACK CtrlCmdCallback(void* param, CMD_STREAM* cmdParam, CMD_STREAM* resParam);

	CNotifyManager notifyManager;
	CEpgDBManager epgDB;
	//reserveManager��notifyManager��epgDB�Ɉˑ�����̂ŁA���������ւ��Ă͂����Ȃ�
	CReserveManager reserveManager;
	CFileStreamingManager streamingManager;

	CParseEpgAutoAddText epgAutoAdd;
	CParseManualAutoAddText manualAutoAdd;

	mutable CRITICAL_SECTION settingLock;
	HANDLE requestEvent;
	bool requestStop;
	bool requestResetServer;
	bool requestReloadEpgChk;
	WORD requestShutdownMode;

	bool serviceFlag;
	DWORD wakeMarginSec;
	unsigned short tcpPort;
	int autoAddHour;
	bool chkGroupEvent;
	//LOBYTE�Ƀ��[�h(1=�X�^���o�C,2=�x�~,3=�d���f,4=�Ȃɂ����Ȃ�)�AHIBYTE�ɍċN���t���O
	WORD defShutdownMode;
	DWORD ngUsePCTime;
	bool ngFileStreaming;
	vector<wstring> noSuspendExeList;
	vector<wstring> tvtestUseBon;
	bool nwtvUdp;
	bool nwtvTcp;

	vector<OLD_EVENT_INFO_DATA3> oldSearchList;
};
