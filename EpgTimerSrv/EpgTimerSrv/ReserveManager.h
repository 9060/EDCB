#pragma once

#include "../../Common/Util.h"
#include "../../Common/EpgTimerUtil.h"
#include "../../Common/PathUtil.h"
#include "../../Common/StringUtil.h"
#include "../../Common/ParseTextInstances.h"

#include "TwitterManager.h"

#include "NotifyManager.h"
#include "ReserveInfo.h"
#include "TunerManager.h"
#include "BatManager.h"

class CReserveManager
{
public:
	CReserveManager(CNotifyManager& notifyManager_, CEpgDBManager& epgDBManager_);
	~CReserveManager(void);
	void ChangeRegist();
	void ReloadSetting();

	//�\����̓ǂݍ��݂��s��
	//�߂�l�F
	// TRUE�i�����j�AFALSE�i���s�j
	BOOL ReloadReserveData();

	//�\������擾����
	//�����F
	// reserveList		[OUT]�\����ꗗ
	void GetReserveDataAll(
		vector<RESERVE_DATA>* reserveList
		);

	//�`���[�i�[���̗\������擾����
	//�����F
	// reserveList		[OUT]�\����ꗗ
	void GetTunerReserveAll(
		vector<TUNER_RESERVE_INFO>* list
		);

	//�\������擾����
	//�߂�l�F
	// TRUE�i�����j�AFALSE�i���s�j
	//�����F
	// id				[IN]�\��ID
	// reserveData		[OUT]�\����
	BOOL GetReserveData(
		DWORD id,
		RESERVE_DATA* reserveData
		);

	//�\�����ǉ�����
	//�߂�l�F
	// TRUE�i�����j�AFALSE�i���s�j
	//�����F
	// reserveList		[IN]�\����
	BOOL AddReserveData(
		const vector<RESERVE_DATA>& reserveList,
		BOOL tweet = FALSE
		);

	//�\�����ύX����
	//�߂�l�F
	// TRUE�i�����j�AFALSE�i���s�j
	//�����F
	// reserveList		[IN]�\����
	BOOL ChgReserveData(
		const vector<RESERVE_DATA>& reserveList,
		BOOL timeChg = FALSE
		);

	//�\������폜����
	//�����F
	// reserveList		[IN]�\��ID���X�g
	void DelReserveData(
		const vector<DWORD>& reserveList
		);

	//�\��̐U�蕪�����s��
	void ReloadBankMap(BOOL notify);
	
	//�^��ςݏ��ꗗ���擾����
	//�����F
	// infoList			[OUT]�^��ςݏ��ꗗ
	void GetRecFileInfoAll(
		vector<REC_FILE_INFO>* infoList
		);

	//�^��ςݏ����폜����
	//�����F
	// idList			[IN]ID���X�g
	void DelRecFileInfo(
		const vector<DWORD>& idList
		);

	//�^��ςݏ��̃v���e�N�g��ύX����
	//�����F
	// infoList			[IN]�^��ςݏ��ꗗ�iid��protectFlag�̂ݎQ�Ɓj
	void ChgProtectRecFileInfo(
		const vector<REC_FILE_INFO>& infoList
		);


	BOOL IsEnableSuspend(
		BYTE* suspendMode,
		BYTE* rebootFlag
		);

	BOOL IsEnableReloadEPG(
		);

	BOOL IsSuspendOK(BOOL rebootFlag = FALSE);

	BOOL GetSleepReturnTime(
		LONGLONG* returnTime
		);

	BOOL StartEpgCap();
	void StopEpgCap();
	BOOL IsEpgCap();

	BOOL IsFindReserve(
		WORD ONID,
		WORD TSID,
		WORD SID,
		WORD eventID
		);

	static void SendNotifyChgReserve(DWORD notifyId, const RESERVE_DATA& oldInfo, const RESERVE_DATA& newInfo, CNotifyManager& notifyManager);

	BOOL GetTVTestChgCh(
		LONGLONG chID,
		TVTEST_CH_CHG_INFO* chInfo
		);

	BOOL SetNWTVCh(
		const SET_CH_INFO& chInfo
		);

	BOOL CloseNWTV(
		);

	void SetNWTVMode(
		DWORD mode
		);

	void SendTweet(
		SEND_TWEET_MODE mode,
		void* param1,
		void* param2,
		void* param3
		);

	BOOL GetRecFilePath(
		DWORD reserveID,
		wstring& filePath,
		DWORD* ctrlID,
		DWORD* processID
		);

	//�\��ǉ��\���`�F�b�N����
	BOOL ChkAddReserve(const RESERVE_DATA& chkData, WORD* status);

	//6���ȓ��̘^�挋�ʂɓ����ԑg�����邩�`�F�b�N����
	BOOL IsFindRecEventInfo(const EPGDB_EVENT_INFO& info, WORD chkDay);
	void ChgAutoAddNoRec(const EPGDB_EVENT_INFO& info);

	BOOL IsRecInfoChg();
protected:
	CRITICAL_SECTION managerLock;

	CNotifyManager& notifyManager;
	CEpgDBManager& epgDBManager;

	HANDLE bankCheckThread;
	HANDLE bankCheckStopEvent;

	WORD notifyStatus;

	CParseReserveText reserveText;
	map<DWORD, CReserveInfo*> reserveInfoMap; //�L�[�@reserveID
	CParseRecInfoText recInfoText;
	CParseRecInfo2Text recInfo2Text;
	wstring recInfo2RegExp;
	int recInfo2DropChk;

	CParseChText5 chUtil;

	CTunerManager tunerManager;
	CBatManager batManager;
	CTwitterManager* twitterManager;

	typedef struct _BANK_WORK_INFO{
		CReserveInfo* reserveInfo;
		LONGLONG startTime;//�}�[�W���l�������J�n����
		LONGLONG endTime;//�}�[�W���l�������I������
		BYTE priority;
		wstring sortKey;
		DWORD reserveID;
		DWORD chID;		//originalNetworkID<<16 | transportStreamID
		DWORD preTunerID;
		DWORD useTunerID;
		WORD ONID;
		WORD TSID;
		WORD SID;
	}BANK_WORK_INFO;
	typedef struct _BANK_INFO{
		DWORD tunerID;
		map<DWORD, BANK_WORK_INFO*> reserveList; //�L�[ �\��ID
	}BANK_INFO;
	map<DWORD, BANK_INFO*> bankMap; //�L�[ �`���[�i�[ID
	map<DWORD, BANK_WORK_INFO*> NGReserveMap;

	int defStartMargine;
	int defEndMargine;

	BOOL backPriorityFlag;
	BOOL sameChPriorityFlag;

	map<DWORD, CTunerBankCtrl*> tunerBankMap; //�L�[ bonID<<16 | tunerID

	BYTE enableSetSuspendMode;
	BYTE enableSetRebootFlag;
	BYTE enableEpgReload;

	BOOL epgCapCheckFlag;


	BOOL BSOnly;
	BOOL CS1Only;
	BOOL CS2Only;
	int ngCapMin;
	int ngCapTunerMin;
	typedef struct _EPGTIME_INFO{
		DWORD time;
		int wday;
		int basicOnlyFlags;
	}EPGTIME_INFO;
	vector<EPGTIME_INFO> epgCapTimeList;
	int wakeTime;
	BYTE defSuspendMode;
	BYTE defRebootFlag;
	int batMargin;
	vector<wstring> noStandbyExeList;
	DWORD noStandbyTime;
	BOOL ngShareFile;
	BOOL autoDel;
	vector<wstring> delExtList;
	vector<wstring> delFolderList;
	BOOL eventRelay;
	BOOL useTweet;
	BOOL useProxy;
	wstring proxySrv;
	wstring proxyID;
	wstring proxyPWD;
	BOOL recEndTweetErr;
	DWORD recEndTweetDrop;

	vector<wstring> tvtestUseBon;

	int duraChgMarginMin;
	int notFindTuijyuHour;
	int noEpgTuijyuMin;

	BOOL autoDelRecInfo;
	int autoDelRecInfoNum;
	BOOL timeSync;
	BOOL setTimeSync;

	DWORD NWTVPID;
	wstring recExePath;
	CSendCtrlCmd sendCtrlNWTV;
	BOOL NWTVUDP;
	BOOL NWTVTCP;

	BOOL ngAddResSrvCoop;

	BOOL errEndBatRun;

	int reloadBankMapAlgo;
	BOOL useRecNamePlugIn;
	wstring recNamePlugInFilePath;

	BOOL chgRecInfo;
protected:
	BOOL _AddReserveData(RESERVE_DATA reserve, BOOL tweet = FALSE);
	BOOL _ChgReserveData(RESERVE_DATA reserve, BOOL chgTime);

	void _ReloadBankMap();
	void _ReloadBankMapAlgo(BOOL do2Pass, BOOL ignoreUseTunerID, BOOL backPriority, BOOL noTuner);
	void CalcEntireReserveTime(LONGLONG* startTime, LONGLONG* endTime, const RESERVE_DATA& data);
	void CheckOverTimeReserve();
	void CreateWorkData(CReserveInfo* reserveInfo, BANK_WORK_INFO* workInfo, BOOL backPriority, DWORD reserveCount, DWORD reserveNum, BOOL noTuner = FALSE);
	DWORD ChkInsertStatus(BANK_INFO* bank, BANK_WORK_INFO* inItem, BOOL reCheck = FALSE, BOOL mustOverlap = FALSE);
	DWORD ReChkInsertStatus(BANK_INFO* bank, BANK_WORK_INFO* inItem);
	DWORD ChkInsertNGStatus(BANK_INFO* bank, BANK_WORK_INFO* inItem);
	BOOL ChangeNGReserve(BANK_WORK_INFO* inItem);
	DWORD ChkInsertSameChStatus(BANK_INFO* bank, BANK_WORK_INFO* inItem);

	void SendNotifyStatus(WORD status);
	static void SendNotifyRecEnd(const REC_FILE_INFO& item, CNotifyManager& notifyManager);
	void _DelReserveData(
		const vector<DWORD>& reserveList
	);

	static UINT WINAPI BankCheckThread(LPVOID param);
	void CheckEndReserve();
	void CheckErrReserve();
	void CheckBatWork();
	void CheckTuijyu();
	BOOL CheckEventRelay(const EPGDB_EVENT_INFO& info, const RESERVE_DATA& data, BOOL errEnd = FALSE);

	BOOL CheckChgEvent(const EPGDB_EVENT_INFO& info, RESERVE_DATA* data, BYTE* chgMode = NULL);
	BOOL CheckChgEventID(const EPGDB_EVENT_INFO& info, RESERVE_DATA* data);
	BOOL CheckNotFindChgEvent(RESERVE_DATA* data, CTunerBankCtrl* ctrl, vector<DWORD>* deleteList);
	BOOL ChgDurationChk(const EPGDB_EVENT_INFO& info);

	void EnableSuspendWork(BYTE suspendMode, BYTE rebootFlag, BYTE epgReload);
	BOOL IsFindNoSuspendExe();
	BOOL IsFindShareTSFile();

	BOOL GetNextEpgcapTime(LONGLONG* capTime, LONGLONG chkMargineMin, int* basicOnlyFlags = NULL);

	//TS�t�@�C�����폜���ĕK�v�ȋ󂫗̈�����
	static void CreateDiskFreeSpace(
		const vector<RESERVE_DATA>& chkReserve,
		const wstring& defRecFolder,
		const map<wstring, wstring>& protectFile,
		const vector<wstring>& delFolderList,
		const vector<wstring>& delExtList
		);
};

