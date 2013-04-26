#include "StdAfx.h"
#include "SendCtrlCmd.h"
/*
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "Ws2_32.lib")
*/
#include "StringUtil.h"

#include <Objbase.h>
#pragma comment(lib, "Ole32.lib")

CSendCtrlCmd::CSendCtrlCmd(void)
{
	CoInitialize(NULL);

	WSAData wsaData;
	WSAStartup(MAKEWORD(2,0), &wsaData);

	this->lockEvent = _CreateEvent(FALSE, TRUE, NULL);

	this->tcpFlag = FALSE;
	this->connectTimeOut = CONNECT_TIMEOUT;

	this->pipeName = CMD2_EPG_SRV_PIPE;
	this->eventName = CMD2_EPG_SRV_EVENT_WAIT_CONNECT;

	this->ip = L"127.0.0.1";
	this->port = 5678;

}


CSendCtrlCmd::~CSendCtrlCmd(void)
{
	if( this->lockEvent != NULL ){
		UnLock();
		CloseHandle(this->lockEvent);
		this->lockEvent = NULL;
	}
	WSACleanup();

	CoUninitialize();
}

BOOL CSendCtrlCmd::Lock(LPCWSTR log, DWORD timeOut)
{
	if( this->lockEvent == NULL ){
		return FALSE;
	}
	if( log != NULL ){
		OutputDebugString(log);
	}
	DWORD dwRet = WaitForSingleObject(this->lockEvent, timeOut);
	if( dwRet == WAIT_ABANDONED || 
		dwRet == WAIT_FAILED){
		return FALSE;
	}
	return TRUE;
}

void CSendCtrlCmd::UnLock(LPCWSTR log)
{
	if( this->lockEvent != NULL ){
		SetEvent(this->lockEvent);
	}
	if( log != NULL ){
		OutputDebugString(log);
	}
}

//�R�}���h���M���@�̐ݒ�
//�����F
// tcpFlag		[IN] TRUE�FTCP/IP���[�h�AFALSE�F���O�t���p�C�v���[�h
void CSendCtrlCmd::SetSendMode(
	BOOL tcpFlag
	)
{
	if( Lock() == FALSE ) return ;
	this->tcpFlag = tcpFlag;
	UnLock();
}

//���O�t���p�C�v���[�h���̐ڑ����ݒ�
//EpgTimerSrv.exe�ɑ΂���R�}���h�͐ݒ肵�Ȃ��Ă��i�f�t�H���g�l�ɂȂ��Ă���j
//�����F
// eventName	[IN]�r������pEvent�̖��O
// pipeName		[IN]�ڑ��p�C�v�̖��O
void CSendCtrlCmd::SetPipeSetting(
	wstring eventName,
	wstring pipeName
	)
{
	if( Lock() == FALSE ) return ;
	this->eventName = eventName;
	this->pipeName = pipeName;
	UnLock();
}

//TCP/IP���[�h���̐ڑ����ݒ�
//�����F
// ip			[IN]�ڑ���IP
// port			[IN]�ڑ���|�[�g
void CSendCtrlCmd::SetNWSetting(
	wstring ip,
	DWORD port
	)
{
	if( Lock() == FALSE ) return ;
	this->ip = ip;
	this->port = port;
	UnLock();
}

//�ڑ��������̃^�C���A�E�g�ݒ�
// timeOut		[IN]�^�C���A�E�g�l�i�P�ʁFms�j
void CSendCtrlCmd::SetConnectTimeOut(
	DWORD timeOut
	)
{
	if( Lock() == FALSE ) return ;
	this->connectTimeOut = timeOut;
	UnLock();
}

DWORD CSendCtrlCmd::SendPipe(LPCWSTR pipeName, LPCWSTR eventName, DWORD timeOut, CMD_STREAM* send, CMD_STREAM* res)
{
	if( pipeName == NULL || eventName == NULL || send == NULL || res == NULL ){
		return CMD_ERR_INVALID_ARG;
	}

	//�ڑ��҂�
	HANDLE waitEvent = _CreateEvent(FALSE, FALSE, eventName);
	if( waitEvent == NULL ){
		return CMD_ERR;
	}
	if(WaitForSingleObject(waitEvent, timeOut) != WAIT_OBJECT_0){
		CloseHandle(waitEvent);
		return CMD_ERR_TIMEOUT;
	}
	CloseHandle(waitEvent);

	//�ڑ�
	HANDLE pipe = _CreateFile( pipeName, GENERIC_READ|GENERIC_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( pipe == INVALID_HANDLE_VALUE ){
		_OutputDebugString(L"*+* ConnectPipe Err:%d\r\n", GetLastError());
		return CMD_ERR_CONNECT;
	}

	DWORD write = 0;
	DWORD read = 0;

	//���M
	DWORD head[2];
	head[0] = send->param;
	head[1] = send->dataSize;
	if( WriteFile(pipe, head, sizeof(DWORD)*2, &write, NULL ) == FALSE ){
		CloseHandle(pipe);
		return CMD_ERR;
	}
	if( send->dataSize > 0 ){
		if( send->data == NULL ){
			CloseHandle(pipe);
			return CMD_ERR_INVALID_ARG;
		}
		DWORD sendNum = 0;
		while(sendNum < send->dataSize ){
			DWORD sendSize = 0;
			if( send->dataSize - sendNum < CMD2_SEND_BUFF_SIZE ){
				sendSize = send->dataSize - sendNum;
			}else{
				sendSize = CMD2_SEND_BUFF_SIZE;
			}
			if( WriteFile(pipe, send->data + sendNum, sendSize, &write, NULL ) == FALSE ){
				CloseHandle(pipe);
				return CMD_ERR;
			}
			sendNum += write;
		}
	}

	//��M
	if( ReadFile(pipe, head, sizeof(DWORD)*2, &read, NULL ) == FALSE ){
		CloseHandle(pipe);
		return CMD_ERR;
	}
	res->param = head[0];
	res->dataSize = head[1];
	if( res->dataSize > 0 ){
		res->data = new BYTE[res->dataSize];
		DWORD readNum = 0;
		while(readNum < res->dataSize ){
			DWORD readSize = 0;
			if( res->dataSize - readNum < CMD2_RES_BUFF_SIZE ){
				readSize = res->dataSize - readNum;
			}else{
				readSize = CMD2_RES_BUFF_SIZE;
			}
			if( ReadFile(pipe, res->data + readNum, readSize, &read, NULL ) == FALSE ){
				CloseHandle(pipe);
				return CMD_ERR;
			}
			readNum += read;
		}
	}
	CloseHandle(pipe);

	return res->param;
}

DWORD CSendCtrlCmd::SendTCP(wstring ip, DWORD port, DWORD timeOut, CMD_STREAM* sendCmd, CMD_STREAM* resCmd)
{
	if( sendCmd == NULL || resCmd == NULL ){
		return CMD_ERR_INVALID_ARG;
	}

	struct sockaddr_in server;
	SOCKET sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	server.sin_family = AF_INET;
	server.sin_port = htons((WORD)port);
	string strA = "";
	WtoA(ip, strA);
	server.sin_addr.S_un.S_addr = inet_addr(strA.c_str());
	DWORD socketBuffSize = 1024*1024;
	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char*)&socketBuffSize, sizeof(socketBuffSize));
	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char*)&socketBuffSize, sizeof(socketBuffSize));

	int ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
	if( ret == SOCKET_ERROR ){
		int a= GetLastError();
		wstring aa;
		Format(aa,L"%d",a);
		OutputDebugString(aa.c_str());
		closesocket(sock);
		return CMD_ERR_CONNECT;
	}

	DWORD read = 0;
	//���M
	DWORD head[2];
	head[0] = sendCmd->param;
	head[1] = sendCmd->dataSize;
	send(sock, (char*)head, sizeof(DWORD)*2, 0 );
	if( ret == SOCKET_ERROR ){
		closesocket(sock);
		return CMD_ERR;
	}
	if( sendCmd->dataSize > 0 ){
		if( sendCmd->data == NULL ){
			closesocket(sock);
			return CMD_ERR_INVALID_ARG;
		}
		ret = send(sock, (char*)sendCmd->data, sendCmd->dataSize, 0 );
		if( ret == SOCKET_ERROR ){
			closesocket(sock);
			return CMD_ERR;
		}
	}
	//��M
	ret = recv(sock, (char*)head, sizeof(DWORD)*2, 0 );
	if( ret == SOCKET_ERROR ){
		closesocket(sock);
		return CMD_ERR;
	}
	resCmd->param = head[0];
	resCmd->dataSize = head[1];
	if( resCmd->dataSize > 0 ){
		resCmd->data = new BYTE[resCmd->dataSize];
		read = 0;
		while(ret>0){
			ret = recv(sock, (char*)(resCmd->data + read), resCmd->dataSize - read, 0);
			if( ret == SOCKET_ERROR ){
				closesocket(sock);
				return CMD_ERR;
			}else if( ret == 0 ){
				break;
			}
			read += ret;
			if( read >= resCmd->dataSize ){
				break;
			}
		}
	}
	closesocket(sock);

	return resCmd->param;
}

DWORD CSendCtrlCmd::SendAddloadReserve()
{
	return SendCmdWithoutData(CMD2_EPG_SRV_ADDLOAD_RESERVE);
}

DWORD CSendCtrlCmd::SendReloadEpg()
{
	return SendCmdWithoutData(CMD2_EPG_SRV_RELOAD_EPG);
}

DWORD CSendCtrlCmd::SendReloadSetting()
{
	return SendCmdWithoutData(CMD2_EPG_SRV_RELOAD_SETTING);
}

DWORD CSendCtrlCmd::SendClose()
{
	return SendCmdWithoutData(CMD2_EPG_SRV_CLOSE);
}

DWORD CSendCtrlCmd::SendRegistGUI(DWORD processID)
{
	return SendCmdData(CMD2_EPG_SRV_REGIST_GUI, processID);
}

DWORD CSendCtrlCmd::SendUnRegistGUI(DWORD processID)
{
	return SendCmdData(CMD2_EPG_SRV_UNREGIST_GUI, processID);
}

DWORD CSendCtrlCmd::SendRegistTCP(DWORD port)
{
	return SendCmdData(CMD2_EPG_SRV_REGIST_GUI_TCP, port);
}

DWORD CSendCtrlCmd::SendUnRegistTCP(DWORD port)
{
	return SendCmdData(CMD2_EPG_SRV_UNREGIST_GUI_TCP, port);
}

DWORD CSendCtrlCmd::SendEnumReserve(vector<RESERVE_DATA>* val)
{
	return ReceiveCmdData(CMD2_EPG_SRV_ENUM_RESERVE, val);
}

DWORD CSendCtrlCmd::SendGetReserve(DWORD reserveID, RESERVE_DATA* val)
{
	return SendAndReceiveCmdData(CMD2_EPG_SRV_GET_RESERVE, reserveID, val);
}

DWORD CSendCtrlCmd::SendAddReserve(vector<RESERVE_DATA>* val)
{
	return SendCmdData(CMD2_EPG_SRV_ADD_RESERVE, val);
}

DWORD CSendCtrlCmd::SendDelReserve(vector<DWORD>* val)
{
	return SendCmdData(CMD2_EPG_SRV_DEL_RESERVE, val);
}

DWORD CSendCtrlCmd::SendChgReserve(vector<RESERVE_DATA>* val)
{
	return SendCmdData(CMD2_EPG_SRV_CHG_RESERVE, val);
}

//�`���[�i�[���Ƃ̗\��ꗗ���擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[IN]�\��ꗗ
DWORD CSendCtrlCmd::SendEnumTunerReserve(vector<TUNER_RESERVE_INFO>* val)
{
	return ReceiveCmdData(CMD2_EPG_SRV_ENUM_TUNER_RESERVE, val);
}

//�^��ςݏ��ꗗ�擾
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[OUT]�^��ςݏ��ꗗ
DWORD CSendCtrlCmd::SendEnumRecInfo(
	vector<REC_FILE_INFO>* val
	)
{
	return ReceiveCmdData(CMD2_EPG_SRV_ENUM_RECINFO, val);
}

//�^��ςݏ����폜����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[IN]�폜����ID�ꗗ
DWORD CSendCtrlCmd::SendDelRecInfo(vector<DWORD>* val)
{
	return SendCmdData(CMD2_EPG_SRV_DEL_RECINFO, val);
}

//�T�[�r�X�ꗗ���擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[OUT]�T�[�r�X�ꗗ
DWORD CSendCtrlCmd::SendEnumService(
	vector<EPGDB_SERVICE_INFO>* val
	)
{
	return ReceiveCmdData(CMD2_EPG_SRV_ENUM_SERVICE, val);
}

//�T�[�r�X�w��Ŕԑg�����ꗗ���擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// service			[IN]ONID<<32 | TSID<<16 | SID�Ƃ����T�[�r�XID
// val				[OUT]�ԑg���ꗗ
DWORD CSendCtrlCmd::SendEnumPgInfo(
	ULONGLONG service,
	vector<EPGDB_EVENT_INFO*>* val
	)
{
	return SendAndReceiveCmdData(CMD2_EPG_SRV_ENUM_PG_INFO, service, val);
}

//�w��C�x���g�̔ԑg�����擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// pgID				[IN]ONID<<48 | TSID<<32 | SID<<16 | EventID�Ƃ���ID
// val				[OUT]�ԑg���
DWORD CSendCtrlCmd::SendGetPgInfo(
	ULONGLONG pgID,
	EPGDB_EVENT_INFO* val
	)
{
	return SendAndReceiveCmdData(CMD2_EPG_SRV_GET_PG_INFO, pgID, val);
}

//�w��L�[���[�h�Ŕԑg������������
//�߂�l�F
// �G���[�R�[�h
//�����F
// key				[IN]�����L�[�i�����w�莞�͂܂Ƃ߂Č������ʂ��Ԃ�j
// val				[OUT]�ԑg���ꗗ
DWORD CSendCtrlCmd::SendSearchPg(
	vector<EPGDB_SEARCH_KEY_INFO>* key,
	vector<EPGDB_EVENT_INFO*>* val
	)
{
	return SendAndReceiveCmdData(CMD2_EPG_SRV_SEARCH_PG, key, val);
}

//�ԑg���ꗗ���擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[OUT]�ԑg���ꗗ
DWORD CSendCtrlCmd::SendEnumPgAll(
	vector<EPGDB_SERVICE_EVENT_INFO*>* val
	)
{
	return ReceiveCmdData(CMD2_EPG_SRV_ENUM_PG_ALL, val);
}

//�����\��o�^�����ꗗ���擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[OUT]�����ꗗ
DWORD CSendCtrlCmd::SendEnumEpgAutoAdd(
	vector<EPG_AUTO_ADD_DATA>* val
	)
{
	return ReceiveCmdData(CMD2_EPG_SRV_ENUM_AUTO_ADD, val);
}

//�����\��o�^������ǉ�����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[IN]�����ꗗ
DWORD CSendCtrlCmd::SendAddEpgAutoAdd(
	vector<EPG_AUTO_ADD_DATA>* val
	)
{
	return SendCmdData(CMD2_EPG_SRV_ADD_AUTO_ADD, val);
}

//�����\��o�^�������폜����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[IN]�����ꗗ
DWORD CSendCtrlCmd::SendDelEpgAutoAdd(
	vector<DWORD>* val
	)
{
	return SendCmdData(CMD2_EPG_SRV_DEL_AUTO_ADD, val);
}

//�����\��o�^������ύX����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[IN]�����ꗗ
DWORD CSendCtrlCmd::SendChgEpgAutoAdd(
	vector<EPG_AUTO_ADD_DATA>* val
	)
{
	return SendCmdData(CMD2_EPG_SRV_CHG_AUTO_ADD, val);
}

//�����\��o�^�����ꗗ���擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[OUT]�����ꗗ	
DWORD CSendCtrlCmd::SendEnumManualAdd(
	vector<MANUAL_AUTO_ADD_DATA>* val
	)
{
	return ReceiveCmdData(CMD2_EPG_SRV_ENUM_MANU_ADD, val);
}

//�����\��o�^������ǉ�����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[IN]�����ꗗ
DWORD CSendCtrlCmd::SendAddManualAdd(
	vector<MANUAL_AUTO_ADD_DATA>* val
	)
{
	return SendCmdData(CMD2_EPG_SRV_ADD_MANU_ADD, val);
}

//�v���O�����\�񎩓��o�^�̏����폜
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[IN]�����ꗗ
DWORD CSendCtrlCmd::SendDelManualAdd(
	vector<DWORD>* val
	)
{
	return SendCmdData(CMD2_EPG_SRV_DEL_MANU_ADD, val);
}

//�v���O�����\�񎩓��o�^�̏����ύX
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[IN]�����ꗗ
DWORD CSendCtrlCmd::SendChgManualAdd(
	vector<MANUAL_AUTO_ADD_DATA>* val
	)
{
	return SendCmdData(CMD2_EPG_SRV_CHG_MANU_ADD, val);
}


DWORD CSendCtrlCmd::SendChkSuspend()
{
	return SendCmdWithoutData(CMD2_EPG_SRV_CHK_SUSPEND);
}

DWORD CSendCtrlCmd::SendSuspend(
	WORD val
	)
{
	return SendCmdData(CMD2_EPG_SRV_SUSPEND, val);
}

DWORD CSendCtrlCmd::SendReboot()
{
	return SendCmdWithoutData(CMD2_EPG_SRV_REBOOT);
}

DWORD CSendCtrlCmd::SendEpgCapNow()
{
	return SendCmdWithoutData(CMD2_EPG_SRV_EPG_CAP_NOW);
}

DWORD CSendCtrlCmd::SendFileCopy(
	wstring val,
	BYTE** resVal,
	DWORD* resValSize
	)
{
	CMD_STREAM res;
	DWORD ret = SendCmdData(CMD2_EPG_SRV_FILE_COPY, val, &res);

	if( ret == CMD_SUCCESS ){
		if( res.dataSize == 0 ){
			return CMD_ERR;
		}
		*resValSize = res.dataSize;
		*resVal = new BYTE[res.dataSize];
		memcpy(*resVal, res.data, res.dataSize);
	}
	return ret;
}

//PlugIn�t�@�C���̈ꗗ���擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[IN]1:ReName�A2:Write
// resVal		[OUT]�t�@�C�����ꗗ
DWORD CSendCtrlCmd::SendEnumPlugIn(
	WORD val,
	vector<wstring>* resVal
	)
{
	return SendAndReceiveCmdData(CMD2_EPG_SRV_ENUM_PLUGIN, val, resVal);
}

//TVTest�̃`�����l���؂�ւ��p�̏����擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[IN]ONID<<32 | TSID<<16 | SID�Ƃ����T�[�r�XID
// resVal		[OUT]�`�����l�����
DWORD CSendCtrlCmd::SendGetChgChTVTest(
	ULONGLONG val,
	TVTEST_CH_CHG_INFO* resVal
	)
{
	return SendAndReceiveCmdData(CMD2_EPG_SRV_GET_CHG_CH_TVTEST, val, resVal);
}

//�l�b�g���[�N���[�h��EpgDataCap_Bon�̃`�����l����؂�ւ�
//�߂�l�F
// �G���[�R�[�h
//�����F
// chInfo				[OUT]�`�����l�����
DWORD CSendCtrlCmd::SendNwTVSetCh(
	SET_CH_INFO* val
	)
{
	return SendCmdData(CMD2_EPG_SRV_NWTV_SET_CH, val);
}

//�l�b�g���[�N���[�h�ŋN������EpgDataCap_Bon���I��
//�߂�l�F
// �G���[�R�[�h
//�����F
// chInfo				[OUT]�`�����l�����
DWORD CSendCtrlCmd::SendNwTVClose(
	)
{
	return SendCmdWithoutData(CMD2_EPG_SRV_NWTV_CLOSE);
}

//�l�b�g���[�N���[�h�ŋN������Ƃ��̃��[�h
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[OUT]���[�h�i1:UDP 2:TCP 3:UDP+TCP�j
DWORD CSendCtrlCmd::SendNwTVMode(
	DWORD val
	)
{
	return SendCmdData(CMD2_EPG_SRV_NWTV_MODE, val);
}

//�X�g���[���z�M�p�t�@�C�����J��
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[IN]�J���t�@�C���̃T�[�o�[���t�@�C���p�X
// resVal			[OUT]����pCtrlID
DWORD CSendCtrlCmd::SendNwPlayOpen(
	wstring val,
	DWORD* resVal
	)
{
	return SendAndReceiveCmdData(CMD2_EPG_SRV_NWPLAY_OPEN, val, resVal);
}

//�X�g���[���z�M�p�t�@�C�������
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[IN]����pCtrlID
DWORD CSendCtrlCmd::SendNwPlayClose(
	DWORD val
	)
{
	return SendCmdData(CMD2_EPG_SRV_NWPLAY_CLOSE, val);
}

//�X�g���[���z�M�J�n
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[IN]����pCtrlID
DWORD CSendCtrlCmd::SendNwPlayStart(
	DWORD val
	)
{
	return SendCmdData(CMD2_EPG_SRV_NWPLAY_PLAY, val);
}

//�X�g���[���z�M��~
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[IN]����pCtrlID
DWORD CSendCtrlCmd::SendNwPlayStop(
	DWORD val
	)
{
	return SendCmdData(CMD2_EPG_SRV_NWPLAY_STOP, val);
}

//�X�g���[���z�M�Ō��݂̑��M�ʒu�Ƒ��t�@�C���T�C�Y���擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[IN/OUT]�T�C�Y���
DWORD CSendCtrlCmd::SendNwPlayGetPos(
	NWPLAY_POS_CMD* val
	)
{
	return SendAndReceiveCmdData(CMD2_EPG_SRV_NWPLAY_GET_POS, val, val);
}

//�X�g���[���z�M�ő��M�ʒu���V�[�N����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[IN]�T�C�Y���
DWORD CSendCtrlCmd::SendNwPlaySetPos(
	NWPLAY_POS_CMD* val
	)
{
	return SendCmdData(CMD2_EPG_SRV_NWPLAY_SET_POS, val);
}

//�X�g���[���z�M�ő��M���ݒ肷��
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[IN]�T�C�Y���
DWORD CSendCtrlCmd::SendNwPlaySetIP(
	NWPLAY_PLAY_INFO* val
	)
{
	return SendAndReceiveCmdData(CMD2_EPG_SRV_NWPLAY_SET_IP, val, val);
}

//�X�g���[���z�M�p�t�@�C�����^�C���V�t�g���[�h�ŊJ��
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[IN]�\��ID
// resVal			[OUT]�t�@�C���p�X��CtrlID
DWORD CSendCtrlCmd::SendNwTimeShiftOpen(
	DWORD val,
	NWPLAY_TIMESHIFT_INFO* resVal
	)
{
	return SendAndReceiveCmdData(CMD2_EPG_SRV_NWPLAY_TF_OPEN, val, resVal);
}

DWORD CSendCtrlCmd::SendEnumReserve2(vector<RESERVE_DATA>* val)
{
	return ReceiveCmdData2(CMD2_EPG_SRV_ENUM_RESERVE2, val);
}

DWORD CSendCtrlCmd::SendGetReserve2(DWORD reserveID, RESERVE_DATA* val)
{
	return SendAndReceiveCmdData2(CMD2_EPG_SRV_GET_RESERVE2, reserveID, val);
}

DWORD CSendCtrlCmd::SendAddReserve2(vector<RESERVE_DATA>* val)
{
	return SendCmdData2(CMD2_EPG_SRV_ADD_RESERVE2, val);
}

DWORD CSendCtrlCmd::SendChgReserve2(vector<RESERVE_DATA>* val)
{
	return SendCmdData2(CMD2_EPG_SRV_CHG_RESERVE2, val);
}

//�\��ǉ����\���m�F����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[IN]�\����
// resVal			[OUT]�ǉ��\���̃X�e�[�^�X
DWORD CSendCtrlCmd::SendAddChkReserve2(RESERVE_DATA* val, WORD* resVal)
{
	return SendAndReceiveCmdData2(CMD2_EPG_SRV_ADDCHK_RESERVE2, val, resVal);
}


//EPG�f�[�^�t�@�C���̃^�C���X�^���v�擾
//�߂�l�F
// �G���[�R�[�h
//�����F
// val				[IN]�擾�t�@�C����
// resVal			[OUT]�^�C���X�^���v
DWORD CSendCtrlCmd::SendGetEpgFileTime2(wstring val, LONGLONG* resVal)
{
	return SendAndReceiveCmdData2(CMD2_EPG_SRV_GET_EPG_FILETIME2, val, resVal);
}

//EPG�f�[�^�t�@�C���擾
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[IN]�t�@�C����
// resVal		[OUT]�t�@�C���̃o�C�i���f�[�^
// resValSize	[OUT]resVal�̃T�C�Y
DWORD CSendCtrlCmd::SendGetEpgFile2(
	wstring val,
	BYTE** resVal,
	DWORD* resValSize
	)
{
	CMD_STREAM res;
	DWORD ret = SendCmdData2(CMD2_EPG_SRV_GET_EPG_FILE2, val, &res);

	if( ret == CMD_SUCCESS ){
		WORD ver = 0;
		DWORD readSize = 0;
		if( ReadVALUE(&ver, res.data, res.dataSize, &readSize) == FALSE || res.dataSize <= readSize ){
			return CMD_ERR;
		}
		*resValSize = res.dataSize - readSize;
		*resVal = new BYTE[*resValSize];
		memcpy(*resVal, res.data + readSize, *resValSize);
	}
	return ret;
}

//�����\��o�^�����ꗗ���擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[OUT]�����ꗗ
DWORD CSendCtrlCmd::SendEnumEpgAutoAdd2(
	vector<EPG_AUTO_ADD_DATA>* val
	)
{
	return ReceiveCmdData2(CMD2_EPG_SRV_ENUM_AUTO_ADD2, val);
}

//�����\��o�^������ǉ�����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[IN]�����ꗗ
DWORD CSendCtrlCmd::SendAddEpgAutoAdd2(
	vector<EPG_AUTO_ADD_DATA>* val
	)
{
	return SendCmdData2(CMD2_EPG_SRV_ADD_AUTO_ADD2, val);
}

//�����\��o�^������ύX����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[IN]�����ꗗ
DWORD CSendCtrlCmd::SendChgEpgAutoAdd2(
	vector<EPG_AUTO_ADD_DATA>* val
	)
{
	return SendCmdData2(CMD2_EPG_SRV_CHG_AUTO_ADD2, val);
}

//�����\��o�^�����ꗗ���擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[OUT]�����ꗗ	
DWORD CSendCtrlCmd::SendEnumManualAdd2(
	vector<MANUAL_AUTO_ADD_DATA>* val
	)
{
	return ReceiveCmdData2(CMD2_EPG_SRV_ENUM_MANU_ADD2, val);
}

//�����\��o�^������ǉ�����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[IN]�����ꗗ
DWORD CSendCtrlCmd::SendAddManualAdd2(
	vector<MANUAL_AUTO_ADD_DATA>* val
	)
{
	return SendCmdData2(CMD2_EPG_SRV_ADD_MANU_ADD2, val);
}

//�v���O�����\�񎩓��o�^�̏����ύX
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[IN]�����ꗗ
DWORD CSendCtrlCmd::SendChgManualAdd2(
	vector<MANUAL_AUTO_ADD_DATA>* val
	)
{
	return SendCmdData2(CMD2_EPG_SRV_CHG_MANU_ADD2, val);
}

//�����\��o�^�����ꗗ���擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// val			[OUT]�����ꗗ	
DWORD CSendCtrlCmd::SendEnumRecInfo2(
	vector<REC_FILE_INFO>* val
	)
{
	return ReceiveCmdData2(CMD2_EPG_SRV_ENUM_RECINFO2, val);
}

DWORD CSendCtrlCmd::SendChgProtectRecInfo2(vector<REC_FILE_INFO>* val)
{
	return SendCmdData2(CMD2_EPG_SRV_CHG_PROTECT_RECINFO2, val);
}

//�_�C�A���O��O�ʂɕ\��
//�߂�l�F
// �G���[�R�[�h
DWORD CSendCtrlCmd::SendGUIShowDlg(
	)
{
	return SendCmdWithoutData(CMD2_TIMER_GUI_SHOW_DLG);
}

//�\��ꗗ�̏�񂪍X�V���ꂽ
//�߂�l�F
// �G���[�R�[�h
DWORD CSendCtrlCmd::SendGUIUpdateReserve(
	)
{
	return SendCmdWithoutData(CMD2_TIMER_GUI_UPDATE_RESERVE);
}

//EPG�f�[�^�̍ēǂݍ��݂���������
//�߂�l�F
// �G���[�R�[�h
DWORD CSendCtrlCmd::SendGUIUpdateEpgData(
	)
{
	return SendCmdWithoutData(CMD2_TIMER_GUI_UPDATE_EPGDATA);
}

DWORD CSendCtrlCmd::SendGUINotifyInfo2(NOTIFY_SRV_INFO* val)
{
	return SendCmdData2(CMD2_TIMER_GUI_SRV_STATUS_NOTIFY2, val);
}

//View�A�v���iEpgDataCap_Bon.exe�j���N��
//�߂�l�F
// �G���[�R�[�h
//�����F
// exeCmd			[IN]�R�}���h���C��
// PID				[OUT]�N������exe��PID
DWORD CSendCtrlCmd::SendGUIExecute(
	wstring exeCmd,
	DWORD* PID
	)
{
	return SendAndReceiveCmdData(CMD2_TIMER_GUI_VIEW_EXECUTE, exeCmd, PID);
}

//�X�^���o�C�A�x�~�A�V���b�g�_�E���ɓ����Ă������̊m�F�����[�U�[�ɍs��
//�߂�l�F
// �G���[�R�[�h
DWORD CSendCtrlCmd::SendGUIQuerySuspend(
	BYTE rebootFlag,
	BYTE suspendMode
	)
{
	return SendCmdData(CMD2_TIMER_GUI_QUERY_SUSPEND, (WORD)(rebootFlag<<8|suspendMode));
}

//PC�ċN���ɓ����Ă������̊m�F�����[�U�[�ɍs��
//�߂�l�F
// �G���[�R�[�h
DWORD CSendCtrlCmd::SendGUIQueryReboot(
	BYTE rebootFlag
	)
{
	return SendCmdData(CMD2_TIMER_GUI_QUERY_REBOOT, (WORD)(rebootFlag<<8));
}

//�T�[�o�[�̃X�e�[�^�X�ύX�ʒm
//�߂�l�F
// �G���[�R�[�h
DWORD CSendCtrlCmd::SendGUIStatusChg(
	WORD status
	)
{
	return SendCmdData(CMD2_TIMER_GUI_SRV_STATUS_CHG, status);
}

//BonDriver�̐؂�ւ�
//�߂�l�F
// �G���[�R�[�h
//�����F
// bonDriver			[IN]BonDriver�t�@�C����
DWORD CSendCtrlCmd::SendViewSetBonDrivere(
	wstring bonDriver
	)
{
	return SendCmdData(CMD2_VIEW_APP_SET_BONDRIVER, bonDriver);
}

//�g�p����BonDriver�̃t�@�C�������擾
//�߂�l�F
// �G���[�R�[�h
//�����F
// bonDriver			[OUT]BonDriver�t�@�C����
DWORD CSendCtrlCmd::SendViewGetBonDrivere(
	wstring* bonDriver
	)
{
	return ReceiveCmdData(CMD2_VIEW_APP_GET_BONDRIVER, bonDriver);
}

//�`�����l���؂�ւ�
//�߂�l�F
// �G���[�R�[�h
//�����F
// chInfo				[OUT]�`�����l�����
DWORD CSendCtrlCmd::SendViewSetCh(
	SET_CH_INFO* chInfo
	)
{
	return SendCmdData(CMD2_VIEW_APP_SET_CH, chInfo);
}

//�����g�̎��Ԃ�PC���Ԃ̌덷�擾
//�߂�l�F
// �G���[�R�[�h
//�����F
// delaySec				[OUT]�덷�i�b�j
DWORD CSendCtrlCmd::SendViewGetDelay(
	int* delaySec
	)
{
	return ReceiveCmdData(CMD2_VIEW_APP_GET_DELAY, delaySec);
}

//���݂̏�Ԃ��擾
//�߂�l�F
// �G���[�R�[�h
//�����F
// status				[OUT]���
DWORD CSendCtrlCmd::SendViewGetStatus(
	DWORD* status
	)
{
	return ReceiveCmdData(CMD2_VIEW_APP_GET_STATUS, status);
}

//���݂̏�Ԃ��擾
//�߂�l�F
// �G���[�R�[�h
DWORD CSendCtrlCmd::SendViewAppClose(
	)
{
	return SendCmdWithoutData(CMD2_VIEW_APP_CLOSE);
}

//���ʗpID�̐ݒ�
//�߂�l�F
// �G���[�R�[�h
//�����F
// id				[IN]ID
DWORD CSendCtrlCmd::SendViewSetID(
	int id
	)
{
	return SendCmdData(CMD2_VIEW_APP_SET_ID, id);
}

//���ʗpID�̎擾
//�߂�l�F
// �G���[�R�[�h
//�����F
// id				[OUT]ID
DWORD CSendCtrlCmd::SendViewGetID(
	int* id
	)
{
	return ReceiveCmdData(CMD2_VIEW_APP_GET_ID, id);
}

//�\��^��p��GUI�L�[�v
//�߂�l�F
// �G���[�R�[�h
DWORD CSendCtrlCmd::SendViewSetStandbyRec(
	DWORD keepFlag
	)
{
	return SendCmdData(CMD2_VIEW_APP_SET_STANDBY_REC, keepFlag);
}

//�X�g���[������p�R���g���[���쐬
//�߂�l�F
// �G���[�R�[�h
//�����F
// ctrlID				[OUT]����ID
DWORD CSendCtrlCmd::SendViewCreateCtrl(
	DWORD* ctrlID
	)
{
	return ReceiveCmdData(CMD2_VIEW_APP_CREATE_CTRL, ctrlID);
}

//�X�g���[������p�R���g���[���쐬
//�߂�l�F
// �G���[�R�[�h
//�����F
// ctrlID				[IN]����ID
DWORD CSendCtrlCmd::SendViewDeleteCtrl(
	DWORD ctrlID
	)
{
	return SendCmdData(CMD2_VIEW_APP_DELETE_CTRL, ctrlID);
}

//����R���g���[���̐ݒ�
//�߂�l�F
// �G���[�R�[�h
//�����F
// val					[IN]�ݒ�l
DWORD CSendCtrlCmd::SendViewSetCtrlMode(
	SET_CTRL_MODE val
	)
{
	return SendCmdData(CMD2_VIEW_APP_SET_CTRLMODE, &val);
}

//�^�揈���J�n
//�߂�l�F
// �G���[�R�[�h
//�����F
// val					[IN]�ݒ�l
DWORD CSendCtrlCmd::SendViewStartRec(
	SET_CTRL_REC_PARAM val
	)
{
	return SendCmdData(CMD2_VIEW_APP_REC_START_CTRL, &val);
}

//�^�揈���J�n
//�߂�l�F
// �G���[�R�[�h
//�����F
// val					[IN]�ݒ�l
DWORD CSendCtrlCmd::SendViewStopRec(
	SET_CTRL_REC_STOP_PARAM val,
	SET_CTRL_REC_STOP_RES_PARAM* resVal
	)
{
	return SendAndReceiveCmdData(CMD2_VIEW_APP_REC_STOP_CTRL, &val, resVal);
}

//�^�撆�̃t�@�C���p�X���擾
//�߂�l�F
// �G���[�R�[�h
//�����F
// val					[OUT]�t�@�C���p�X
DWORD CSendCtrlCmd::SendViewGetRecFilePath(
	DWORD ctrlID,
	wstring* resVal
	)
{
	return SendAndReceiveCmdData(CMD2_VIEW_APP_REC_FILE_PATH, ctrlID, resVal);
}

//�^�揈���J�n
//�߂�l�F
// �G���[�R�[�h
DWORD CSendCtrlCmd::SendViewStopRecAll(
	)
{
	return SendCmdWithoutData(CMD2_VIEW_APP_REC_STOP_ALL);
}

//�t�@�C���o�͂����T�C�Y���擾
//�߂�l�F
// �G���[�R�[�h
//�����F
// resVal					[OUT]�t�@�C���o�͂����T�C�Y
DWORD CSendCtrlCmd::SendViewGetWriteSize(
	DWORD ctrlID,
	__int64* resVal
	)
{
	return SendAndReceiveCmdData(CMD2_VIEW_APP_REC_WRITE_SIZE, ctrlID, resVal);
}

//EPG�擾�J�n
//�߂�l�F
// �G���[�R�[�h
//�����F
// val					[IN]�擾�`�����l�����X�g
DWORD CSendCtrlCmd::SendViewEpgCapStart(
	vector<SET_CH_INFO>* val
	)
{
	return SendCmdData(CMD2_VIEW_APP_EPGCAP_START, val);
}

//EPG�擾�L�����Z��
//�߂�l�F
// �G���[�R�[�h
DWORD CSendCtrlCmd::SendViewEpgCapStop(
	)
{
	return SendCmdWithoutData(CMD2_VIEW_APP_EPGCAP_STOP);
}

//EPG�f�[�^�̌���
//�߂�l�F
// �G���[�R�[�h
// val					[IN]�擾�ԑg
// resVal				[OUT]�ԑg���
DWORD CSendCtrlCmd::SendViewSearchEvent(
	SEARCH_EPG_INFO_PARAM* val,
	EPGDB_EVENT_INFO* resVal
	)
{
	return SendAndReceiveCmdData(CMD2_VIEW_APP_SEARCH_EVENT, val, resVal);
}

//����or���̔ԑg�����擾����
//�߂�l�F
// �G���[�R�[�h
// val					[IN]�擾�ԑg
// resVal				[OUT]�ԑg���
DWORD CSendCtrlCmd::SendViewGetEventPF(
	GET_EPG_PF_INFO_PARAM* val,
	EPGDB_EVENT_INFO* resVal
	)
{
	return SendAndReceiveCmdData(CMD2_VIEW_APP_GET_EVENT_PF, val, resVal);
}

//View�{�^���o�^�A�v���N��
//�߂�l�F
// �G���[�R�[�h
DWORD CSendCtrlCmd::SendViewExecViewApp(
	)
{
	return SendCmdWithoutData(CMD2_VIEW_APP_EXEC_VIEW_APP);
}

//�X�g���[�~���O�z�M����ID�̐ݒ�
//�߂�l�F
// �G���[�R�[�h
DWORD CSendCtrlCmd::SendViewSetStreamingInfo(
	TVTEST_STREAMING_INFO* val
	)
{
	return SendCmdData(CMD2_VIEW_APP_TT_SET_CTRL, val);
}

DWORD CSendCtrlCmd::SendCmdStream(CMD_STREAM* send, CMD_STREAM* res)
{
	if( Lock() == FALSE ) return CMD_ERR_TIMEOUT;
	DWORD ret = CMD_ERR;
	CMD_STREAM tmpRes;

	if( res == NULL ){
		res = &tmpRes;
	}
	if( this->tcpFlag == FALSE ){
		ret = SendPipe(this->pipeName.c_str(), this->eventName.c_str(), this->connectTimeOut, send, res);
	}else{
		ret = SendTCP(this->ip, this->port, this->connectTimeOut, send, res);
	}

	UnLock();
	return ret;
}

