#pragma once

#include "Util.h"
#include "StructDef.h"

#include "CtrlCmdDef.h"
#include "ErrDef.h"
#include "CtrlCmdUtil.h"
#include "CtrlCmdUtil2.h"

class CSendCtrlCmd
{
public:
	CSendCtrlCmd(void);
	~CSendCtrlCmd(void);

	//�R�}���h���M���@�̐ݒ�
	//�����F
	// tcpFlag		[IN] TRUE�FTCP/IP���[�h�AFALSE�F���O�t���p�C�v���[�h
	void SetSendMode(
		BOOL tcpFlag
		);

	//���O�t���p�C�v���[�h���̐ڑ����ݒ�
	//EpgTimerSrv.exe�ɑ΂���R�}���h�͐ݒ肵�Ȃ��Ă��i�f�t�H���g�l�ɂȂ��Ă���j
	//�����F
	// eventName	[IN]�r������pEvent�̖��O
	// pipeName		[IN]�ڑ��p�C�v�̖��O
	void SetPipeSetting(
		wstring eventName,
		wstring pipeName
		);

	//TCP/IP���[�h���̐ڑ����ݒ�
	//�����F
	// ip			[IN]�ڑ���IP
	// port			[IN]�ڑ���|�[�g
	void SetNWSetting(
		wstring ip,
		DWORD port
		);

	//�ڑ��������̃^�C���A�E�g�ݒ�
	// timeOut		[IN]�^�C���A�E�g�l�i�P�ʁFms�j
	void SetConnectTimeOut(
		DWORD timeOut
		);

	//Program.txt��ǉ��ōēǂݍ��݂���
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendAddloadReserve();

	//EPG�f�[�^���ēǂݍ��݂���
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendReloadEpg();

	//�ݒ�����ēǂݍ��݂���
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendReloadSetting();

	//EpgTimerSrv.exe���I������
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendClose();

	//EpgTimerSrv.exe�̃p�C�v�ڑ�GUI�Ƃ��ăv���Z�X��o�^����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// processID			[IN]�v���Z�XID
	DWORD SendRegistGUI(DWORD processID);

	//EpgTimerSrv.exe�̃p�C�v�ڑ�GUI�o�^����������
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// processID			[IN]�v���Z�XID
	DWORD SendUnRegistGUI(DWORD processID);
	
	//EpgTimerSrv.exe��TCP�ڑ�GUI�Ƃ��ăv���Z�X��o�^����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// port					[IN]�|�[�g
	DWORD SendRegistTCP(DWORD port);

	//EpgTimerSrv.exe��TCP�ڑ�GUI�o�^����������
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// port					[IN]�|�[�g
	DWORD SendUnRegistTCP(DWORD port);

	//�\��ꗗ���擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[OUT]�\��ꗗ
	DWORD SendEnumReserve(
		vector<RESERVE_DATA>* val
		);

	//�\������擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// reserveID		[IN]�擾������̗\��ID
	// val				[OUT]�\����
	DWORD SendGetReserve(DWORD reserveID, RESERVE_DATA* val);

	//�\���ǉ�����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]�ǉ�����\��ꗗ
	DWORD SendAddReserve(vector<RESERVE_DATA>* val);

	//�\����폜����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]�폜����\��ID�ꗗ
	DWORD SendDelReserve(vector<DWORD>* val);

	//�\���ύX����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]�ύX����\��ꗗ
	DWORD SendChgReserve(vector<RESERVE_DATA>* val);

	//�`���[�i�[���Ƃ̗\��ꗗ���擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]�\��ꗗ
	DWORD SendEnumTunerReserve(vector<TUNER_RESERVE_INFO>* val);

	//�^��ςݏ��ꗗ�擾
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[OUT]�^��ςݏ��ꗗ
	DWORD SendEnumRecInfo(
		vector<REC_FILE_INFO>* val
		);
	
	//�^��ςݏ����폜����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]�폜����ID�ꗗ
	DWORD SendDelRecInfo(vector<DWORD>* val);

	//�T�[�r�X�ꗗ���擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[OUT]�T�[�r�X�ꗗ
	DWORD SendEnumService(
		vector<EPGDB_SERVICE_INFO>* val
		);

	//�T�[�r�X�w��Ŕԑg�����ꗗ���擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// service			[IN]ONID<<32 | TSID<<16 | SID�Ƃ����T�[�r�XID
	// val				[OUT]�ԑg���ꗗ
	DWORD SendEnumPgInfo(
		ULONGLONG service,
		vector<EPGDB_EVENT_INFO*>* val
		);

	//�w��C�x���g�̔ԑg�����擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// pgID				[IN]ONID<<48 | TSID<<32 | SID<<16 | EventID�Ƃ���ID
	// val				[OUT]�ԑg���
	DWORD SendGetPgInfo(
		ULONGLONG pgID,
		EPGDB_EVENT_INFO* val
		);

	//�w��L�[���[�h�Ŕԑg������������
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// key				[IN]�����L�[�i�����w�莞�͂܂Ƃ߂Č������ʂ��Ԃ�j
	// val				[OUT]�ԑg���ꗗ
	DWORD SendSearchPg(
		vector<EPGDB_SEARCH_KEY_INFO>* key,
		vector<EPGDB_EVENT_INFO*>* val
		);

	//�ԑg���ꗗ���擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[OUT]�ԑg���ꗗ
	DWORD SendEnumPgAll(
		vector<EPGDB_SERVICE_EVENT_INFO*>* val
		);

	//�����\��o�^�����ꗗ���擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[OUT]�����ꗗ
	DWORD SendEnumEpgAutoAdd(
		vector<EPG_AUTO_ADD_DATA>* val
		);

	//�����\��o�^������ǉ�����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[IN]�����ꗗ
	DWORD SendAddEpgAutoAdd(
		vector<EPG_AUTO_ADD_DATA>* val
		);

	//�����\��o�^�������폜����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[IN]�����ꗗ
	DWORD SendDelEpgAutoAdd(
		vector<DWORD>* val
		);

	//�����\��o�^������ύX����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[IN]�����ꗗ
	DWORD SendChgEpgAutoAdd(
		vector<EPG_AUTO_ADD_DATA>* val
		);

	//�����\��o�^�����ꗗ���擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[OUT]�����ꗗ	
	DWORD SendEnumManualAdd(
		vector<MANUAL_AUTO_ADD_DATA>* val
		);

	//�����\��o�^������ǉ�����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[IN]�����ꗗ
	DWORD SendAddManualAdd(
		vector<MANUAL_AUTO_ADD_DATA>* val
		);

	//�v���O�����\�񎩓��o�^�̏����폜
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[IN]�����ꗗ
	DWORD SendDelManualAdd(
		vector<DWORD>* val
		);

	//�v���O�����\�񎩓��o�^�̏����ύX
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[IN]�����ꗗ
	DWORD SendChgManualAdd(
		vector<MANUAL_AUTO_ADD_DATA>* val
		);


	DWORD SendChkSuspend();

	DWORD SendSuspend(
		WORD val
		);

	DWORD SendReboot();

	DWORD SendEpgCapNow();

	//�w��t�@�C����]������
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[IN]�t�@�C����
	// resVal		[OUT]�t�@�C���̃o�C�i���f�[�^
	// resValSize	[OUT]resVal�̃T�C�Y
	DWORD SendFileCopy(
		wstring val,
		BYTE** resVal,
		DWORD* resValSize
		);

	//PlugIn�t�@�C���̈ꗗ���擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[IN]1:ReName�A2:Write
	// resVal		[OUT]�t�@�C�����ꗗ
	DWORD SendEnumPlugIn(
		WORD val,
		vector<wstring>* resVal
		);

	//TVTest�̃`�����l���؂�ւ��p�̏����擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[IN]ONID<<32 | TSID<<16 | SID�Ƃ����T�[�r�XID
	// resVal		[OUT]�`�����l�����
	DWORD SendGetChgChTVTest(
		ULONGLONG val,
		TVTEST_CH_CHG_INFO* resVal
		);

	//�l�b�g���[�N���[�h��EpgDataCap_Bon�̃`�����l����؂�ւ�
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// chInfo				[OUT]�`�����l�����
	DWORD SendNwTVSetCh(
		SET_CH_INFO* val
		);

	//�l�b�g���[�N���[�h�ŋN������EpgDataCap_Bon���I��
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// chInfo				[OUT]�`�����l�����
	DWORD SendNwTVClose(
		);

	//�l�b�g���[�N���[�h�ŋN������Ƃ��̃��[�h
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[OUT]���[�h�i1:UDP 2:TCP 3:UDP+TCP�j
	DWORD SendNwTVMode(
		DWORD val
		);

	//�X�g���[���z�M�p�t�@�C�����J��
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]�J���t�@�C���̃T�[�o�[���t�@�C���p�X
	// resVal			[OUT]����pCtrlID
	DWORD SendNwPlayOpen(
		wstring val,
		DWORD* resVal
		);

	//�X�g���[���z�M�p�t�@�C�������
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]����pCtrlID
	DWORD SendNwPlayClose(
		DWORD val
		);

	//�X�g���[���z�M�J�n
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]����pCtrlID
	DWORD SendNwPlayStart(
		DWORD val
		);

	//�X�g���[���z�M��~
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]����pCtrlID
	DWORD SendNwPlayStop(
		DWORD val
		);

	//�X�g���[���z�M�Ō��݂̑��M�ʒu�Ƒ��t�@�C���T�C�Y���擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN/OUT]�T�C�Y���
	DWORD SendNwPlayGetPos(
		NWPLAY_POS_CMD* val
		);

	//�X�g���[���z�M�ő��M�ʒu���V�[�N����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]�T�C�Y���
	DWORD SendNwPlaySetPos(
		NWPLAY_POS_CMD* val
		);

	//�X�g���[���z�M�ő��M���ݒ肷��
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]�T�C�Y���
	DWORD SendNwPlaySetIP(
		NWPLAY_PLAY_INFO* val
		);

	//�X�g���[���z�M�p�t�@�C�����^�C���V�t�g���[�h�ŊJ��
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]�\��ID
	// resVal			[OUT]�t�@�C���p�X��CtrlID
	DWORD SendNwTimeShiftOpen(
		DWORD val,
		NWPLAY_TIMESHIFT_INFO* resVal
		);

//�R�}���h�o�[�W�����Ή���
	//�\��ꗗ���擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[OUT]�\��ꗗ
	DWORD SendEnumReserve2(
		vector<RESERVE_DATA>* val
		);

	//�\������擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// reserveID		[IN]�擾������̗\��ID
	// val				[OUT]�\����
	DWORD SendGetReserve2(DWORD reserveID, RESERVE_DATA* val);

	//�\���ǉ�����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]�ǉ�����\��ꗗ
	DWORD SendAddReserve2(vector<RESERVE_DATA>* val);

	//�\���ύX����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]�ύX����\��ꗗ
	DWORD SendChgReserve2(vector<RESERVE_DATA>* val);

	//�\��ǉ����\���m�F����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]�\����
	// resVal			[OUT]�ǉ��\���̃X�e�[�^�X
	DWORD SendAddChkReserve2(RESERVE_DATA* val, WORD* resVal);


	//EPG�f�[�^�t�@�C���̃^�C���X�^���v�擾
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]�擾�t�@�C����
	// resVal			[OUT]�^�C���X�^���v
	DWORD SendGetEpgFileTime2(wstring val, LONGLONG* resVal);

	//EPG�f�[�^�t�@�C���擾
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[IN]�t�@�C����
	// resVal		[OUT]�t�@�C���̃o�C�i���f�[�^
	// resValSize	[OUT]resVal�̃T�C�Y
	DWORD SendGetEpgFile2(
		wstring val,
		BYTE** resVal,
		DWORD* resValSize
		);

	//�����\��o�^�����ꗗ���擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[OUT]�����ꗗ
	DWORD SendEnumEpgAutoAdd2(
		vector<EPG_AUTO_ADD_DATA>* val
		);

	//�����\��o�^������ǉ�����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[IN]�����ꗗ
	DWORD SendAddEpgAutoAdd2(
		vector<EPG_AUTO_ADD_DATA>* val
		);

	//�����\��o�^������ύX����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[IN]�����ꗗ
	DWORD SendChgEpgAutoAdd2(
		vector<EPG_AUTO_ADD_DATA>* val
		);

	//�����\��o�^�����ꗗ���擾����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[OUT]�����ꗗ	
	DWORD SendEnumManualAdd2(
		vector<MANUAL_AUTO_ADD_DATA>* val
		);

	//�����\��o�^������ǉ�����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[IN]�����ꗗ
	DWORD SendAddManualAdd2(
		vector<MANUAL_AUTO_ADD_DATA>* val
		);

	//�v���O�����\�񎩓��o�^�̏����ύX
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[IN]�����ꗗ
	DWORD SendChgManualAdd2(
		vector<MANUAL_AUTO_ADD_DATA>* val
		);

	//�^��ςݏ��ꗗ�擾
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[OUT]�^��ςݏ��ꗗ
	DWORD SendEnumRecInfo2(
		vector<REC_FILE_INFO>* val
		);

	//�^��ςݏ��ꗗ�擾
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val			[OUT]�^��ςݏ��ꗗ
	DWORD SendChgProtectRecInfo2(
		vector<REC_FILE_INFO>* val
		);

//�^�C�}�[GUI�iEpgTimer_Bon.exe�j�p

	//�_�C�A���O��O�ʂɕ\��
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendGUIShowDlg(
		);

	//�\��ꗗ�̏�񂪍X�V���ꂽ
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendGUIUpdateReserve(
		);

	//EPG�f�[�^�̍ēǂݍ��݂���������
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendGUIUpdateEpgData(
		);

	//���X�V��ʒm����
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val				[IN]�ʒm���
	DWORD SendGUINotifyInfo2(NOTIFY_SRV_INFO* val);

//View�A�v���iEpgDataCap_Bon.exe�j���N��
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// exeCmd			[IN]�R�}���h���C��
	// PID				[OUT]�N������exe��PID
	DWORD SendGUIExecute(
		wstring exeCmd,
		DWORD* PID
		);

	//�X�^���o�C�A�x�~�A�V���b�g�_�E���ɓ����Ă������̊m�F�����[�U�[�ɍs��
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendGUIQuerySuspend(
		BYTE rebootFlag,
		BYTE suspendMode
		);

	//PC�ċN���ɓ����Ă������̊m�F�����[�U�[�ɍs��
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendGUIQueryReboot(
		BYTE rebootFlag
		);

	//�T�[�o�[�̃X�e�[�^�X�ύX�ʒm
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// status			[IN]�X�e�[�^�X
	DWORD SendGUIStatusChg(
		WORD status
		);


//View�A�v���iEpgDataCap_Bon.exe�j�p

	//BonDriver�̐؂�ւ�
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// bonDriver			[IN]BonDriver�t�@�C����
	DWORD SendViewSetBonDrivere(
		wstring bonDriver
		);

	//�g�p����BonDriver�̃t�@�C�������擾
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// bonDriver			[OUT]BonDriver�t�@�C����
	DWORD SendViewGetBonDrivere(
		wstring* bonDriver
		);

	//�`�����l���؂�ւ�
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// chInfo				[OUT]�`�����l�����
	DWORD SendViewSetCh(
		SET_CH_INFO* chInfo
		);

	//�����g�̎��Ԃ�PC���Ԃ̌덷�擾
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// delaySec				[OUT]�덷�i�b�j
	DWORD SendViewGetDelay(
		int* delaySec
		);

	//���݂̏�Ԃ��擾
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// status				[OUT]���
	DWORD SendViewGetStatus(
		DWORD* status
		);

	//���݂̏�Ԃ��擾
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendViewAppClose(
		);

	//���ʗpID�̐ݒ�
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// id				[IN]ID
	DWORD SendViewSetID(
		int id
		);

	//���ʗpID�̎擾
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// id				[OUT]ID
	DWORD SendViewGetID(
		int* id
		);

	//�\��^��p��GUI�L�[�v
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendViewSetStandbyRec(
		DWORD keepFlag
		);

	//�X�g���[������p�R���g���[���쐬
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// ctrlID				[OUT]����ID
	DWORD SendViewCreateCtrl(
		DWORD* ctrlID
		);

	//�X�g���[������p�R���g���[���폜
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// ctrlID				[IN]����ID
	DWORD SendViewDeleteCtrl(
		DWORD ctrlID
		);

	//����R���g���[���̐ݒ�
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val					[IN]�ݒ�l
	DWORD SendViewSetCtrlMode(
		SET_CTRL_MODE val
		);

	//�^�揈���J�n
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val					[IN]�ݒ�l
	DWORD SendViewStartRec(
		SET_CTRL_REC_PARAM val
		);

	//�^�揈���J�n
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val					[IN]�ݒ�l
	// resVal				[OUT]�h���b�v��
	DWORD SendViewStopRec(
		SET_CTRL_REC_STOP_PARAM val,
		SET_CTRL_REC_STOP_RES_PARAM* resVal
		);

	//�^�撆�̃t�@�C���p�X���擾
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val					[OUT]�t�@�C���p�X
	DWORD SendViewGetRecFilePath(
		DWORD ctrlID,
		wstring* resVal
		);

	//�^�揈���J�n
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendViewStopRecAll(
		);

	//�t�@�C���o�͂����T�C�Y���擾
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// resVal					[OUT]�t�@�C���o�͂����T�C�Y
	DWORD SendViewGetWriteSize(
		DWORD ctrlID,
		__int64* resVal
		);

	//EPG�擾�J�n
	//�߂�l�F
	// �G���[�R�[�h
	//�����F
	// val					[IN]�擾�`�����l�����X�g
	DWORD SendViewEpgCapStart(
		vector<SET_CH_INFO>* val
		);

	//EPG�擾�L�����Z��
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendViewEpgCapStop(
		);

	//EPG�f�[�^�̌���
	//�߂�l�F
	// �G���[�R�[�h
	// val					[IN]�擾�ԑg
	// resVal				[OUT]�ԑg���
	DWORD SendViewSearchEvent(
		SEARCH_EPG_INFO_PARAM* val,
		EPGDB_EVENT_INFO* resVal
		);

	//����or���̔ԑg�����擾����
	//�߂�l�F
	// �G���[�R�[�h
	// val					[IN]�擾�ԑg
	// resVal				[OUT]�ԑg���
	DWORD SendViewGetEventPF(
		GET_EPG_PF_INFO_PARAM* val,
		EPGDB_EVENT_INFO* resVal
		);

	//View�{�^���o�^�A�v���N��
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendViewExecViewApp(
		);

//TVTest�A�g�̃X�g���[�~���O�z�M��p
	//�X�g���[�~���O�z�M����ID�̐ݒ�
	//�߂�l�F
	// �G���[�R�[�h
	DWORD SendViewSetStreamingInfo(
		TVTEST_STREAMING_INFO* val
		);

protected:
	HANDLE lockEvent;

	BOOL tcpFlag;
	DWORD connectTimeOut;
	wstring eventName;
	wstring pipeName;
	wstring ip;
	DWORD port;

protected:
	//PublicAPI�r������p
	BOOL Lock(LPCWSTR log = NULL, DWORD timeOut = 60*1000);
	void UnLock(LPCWSTR log = NULL);

	DWORD SendPipe(LPCWSTR pipeName, LPCWSTR eventName, DWORD timeOut, CMD_STREAM* send, CMD_STREAM* res);
	DWORD SendTCP(wstring ip, DWORD port, DWORD timeOut, CMD_STREAM* sendCmd, CMD_STREAM* resCmd);

	DWORD SendCmdStream(CMD_STREAM* send, CMD_STREAM* res);
	DWORD SendCmdWithoutData(DWORD param, CMD_STREAM* res = NULL);
	DWORD SendCmdWithoutData2(DWORD param, CMD_STREAM* res = NULL);
	template<class T> DWORD SendCmdData(DWORD param, const T& val, CMD_STREAM* res = NULL);
	template<class T> DWORD SendCmdData2(DWORD param, const T& val, CMD_STREAM* res = NULL);
	template<class T> DWORD ReceiveCmdData(DWORD param, T* resVal);
	template<class T> DWORD ReceiveCmdData2(DWORD param, T* resVal);
	template<class T, class U> DWORD SendAndReceiveCmdData(DWORD param, const T& val, U* resVal);
	template<class T, class U> DWORD SendAndReceiveCmdData2(DWORD param, const T& val, U* resVal);
};

#if 1 //�C�����C��/�e���v���[�g��`

inline DWORD CSendCtrlCmd::SendCmdWithoutData(DWORD param, CMD_STREAM* res)
{
	CMD_STREAM send;
	send.param = param;
	return SendCmdStream(&send, res);
}

inline DWORD CSendCtrlCmd::SendCmdWithoutData2(DWORD param, CMD_STREAM* res)
{
	return SendCmdData(param, (WORD)CMD_VER, res);
}

template<class T>
DWORD CSendCtrlCmd::SendCmdData(DWORD param, const T& val, CMD_STREAM* res)
{
	CMD_STREAM send;
	send.param = param;
	send.dataSize = GetVALUESize(val);
	send.data = new BYTE[send.dataSize];

	if( WriteVALUE(val, send.data, send.dataSize, NULL) == FALSE ){
		return CMD_ERR;
	}
	return SendCmdStream(&send, res);
}

template<class T>
DWORD CSendCtrlCmd::SendCmdData2(DWORD param, const T& val, CMD_STREAM* res)
{
	WORD ver = CMD_VER;
	CMD_STREAM send;
	send.param = param;
	send.dataSize = GetVALUESize(ver) + GetVALUESize2(ver, val);
	send.data = new BYTE[send.dataSize];

	DWORD writeSize = 0;
	if( WriteVALUE(ver, send.data, send.dataSize, &writeSize) == FALSE ||
		WriteVALUE2(ver, val, send.data + writeSize, send.dataSize - writeSize, NULL) == FALSE ){
		return CMD_ERR;
	}
	return SendCmdStream(&send, res);
}

template<class T>
DWORD CSendCtrlCmd::ReceiveCmdData(DWORD param, T* resVal)
{
	CMD_STREAM res;
	DWORD ret = SendCmdWithoutData(param, &res);

	if( ret == CMD_SUCCESS ){
		if( ReadVALUE(resVal, res.data, res.dataSize, NULL) == FALSE ){
			ret = CMD_ERR;
		}
	}
	return ret;
}

template<class T>
DWORD CSendCtrlCmd::ReceiveCmdData2(DWORD param, T* resVal)
{
	CMD_STREAM res;
	DWORD ret = SendCmdWithoutData2(param, &res);

	if( ret == CMD_SUCCESS ){
		WORD ver = 0;
		DWORD readSize = 0;
		if( ReadVALUE(&ver, res.data, res.dataSize, &readSize) == FALSE ||
			ReadVALUE2(ver, resVal, res.data + readSize, res.dataSize - readSize, NULL) == FALSE ){
			ret = CMD_ERR;
		}
	}
	return ret;
}

template<class T, class U>
DWORD CSendCtrlCmd::SendAndReceiveCmdData(DWORD param, const T& val, U* resVal)
{
	CMD_STREAM res;
	DWORD ret = SendCmdData(param, val, &res);

	if( ret == CMD_SUCCESS ){
		if( ReadVALUE(resVal, res.data, res.dataSize, NULL) == FALSE ){
			ret = CMD_ERR;
		}
	}
	return ret;
}

template<class T, class U>
DWORD CSendCtrlCmd::SendAndReceiveCmdData2(DWORD param, const T& val, U* resVal)
{
	CMD_STREAM res;
	DWORD ret = SendCmdData2(param, val, &res);

	if( ret == CMD_SUCCESS ){
		WORD ver = 0;
		DWORD readSize = 0;
		if( ReadVALUE(&ver, res.data, res.dataSize, &readSize) == FALSE ||
			ReadVALUE2(ver, resVal, res.data + readSize, res.dataSize - readSize, NULL) == FALSE ){
			ret = CMD_ERR;
		}
	}
	return ret;
}

#endif
