#include "StdAfx.h"
#include "EpgDataCap3Main.h"


CEpgDataCap3Main::CEpgDataCap3Main(void)
{
	InitializeCriticalSection(&this->utilLock);

	decodeUtilClass.SetEpgDB(&(this->epgDBUtilClass));
}

CEpgDataCap3Main::~CEpgDataCap3Main(void)
{
	DeleteCriticalSection(&this->utilLock);
}

class CBlockLock
{
public:
	CBlockLock(CRITICAL_SECTION* lock_) : lock(lock_) { EnterCriticalSection(lock); }
	~CBlockLock() { LeaveCriticalSection(lock); }
private:
	CRITICAL_SECTION* lock;
};

//DLL�̏�����
//�߂�l�F
// �G���[�R�[�h
//�����F
// asyncMode		[IN]TRUE:�񓯊����[�h�AFALSE:�������[�h
DWORD CEpgDataCap3Main::Initialize(
	BOOL asyncFlag
	)
{
	return NO_ERR;
}

//DLL�̊J��
//�߂�l�F
// �G���[�R�[�h
DWORD CEpgDataCap3Main::UnInitialize(
	)
{
	return NO_ERR;
}

//��͑Ώۂ�TS�p�P�b�g�P��ǂݍ��܂���
//�߂�l�F
// �G���[�R�[�h
// data		[IN]TS�p�P�b�g�P��
// size		[IN]data�̃T�C�Y�i188�A192������ɂȂ�͂��j
DWORD CEpgDataCap3Main::AddTSPacket(
	BYTE* data,
	DWORD size
	)
{
	if( size < 188 ){
		return ERR_INVALID_ARG;
	}

	CBlockLock lock(&this->utilLock);

	DWORD stratPos = 0;
	if( size > 188 ){
		if( data[0] != 0x47 ){
			if( data[size-188] == 0x47 ){
				stratPos = size-188;
			}else{
				for( DWORD i=0; i<size-188; i++ ){
					if( data[i] == 0x47 ){
						break;
					}else{
						stratPos++;
					}
				}
			}
		}
	}
	DWORD err = this->decodeUtilClass.AddTSData(data+stratPos, 188);
	return err;
}

//��̓f�[�^�̌��݂̃X�g���[���h�c���擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// originalNetworkID		[OUT]���݂�originalNetworkID
// transportStreamID		[OUT]���݂�transportStreamID
DWORD CEpgDataCap3Main::GetTSID(
	WORD* originalNetworkID,
	WORD* transportStreamID
	)
{
	CBlockLock lock(&this->utilLock);
	DWORD err = this->decodeUtilClass.GetTSID(originalNetworkID, transportStreamID);
	return err;
}

//���X�g���[���̃T�[�r�X�ꗗ���擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// serviceListSize			[OUT]serviceList�̌�
// serviceList				[OUT]�T�[�r�X���̃��X�g�iDLL���Ŏ����I��delete����B���Ɏ擾���s���܂ŗL���j
DWORD CEpgDataCap3Main::GetServiceListActual(
	DWORD* serviceListSize,
	SERVICE_INFO** serviceList
	)
{
	CBlockLock lock(&this->utilLock);
	DWORD err = this->decodeUtilClass.GetServiceListActual(serviceListSize, serviceList);
	return err;
}

//�~�ς��ꂽEPG���̂���T�[�r�X�ꗗ���擾����
//SERVICE_EXT_INFO�̏��͂Ȃ��ꍇ������
//�߂�l�F
// �G���[�R�[�h
//�����F
// serviceListSize			[OUT]serviceList�̌�
// serviceList				[OUT]�T�[�r�X���̃��X�g�iDLL���Ŏ����I��delete����B���Ɏ擾���s���܂ŗL���j
DWORD CEpgDataCap3Main::GetServiceListEpgDB(
	DWORD* serviceListSize,
	SERVICE_INFO** serviceList
	)
{
	CBlockLock lock(&this->utilLock);
	DWORD err = this->epgDBUtilClass.GetServiceListEpgDB(serviceListSize, serviceList);
	return err;
}

//�w��T�[�r�X�̑SEPG�����擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// originalNetworkID		[IN]�擾�Ώۂ�originalNetworkID
// transportStreamID		[IN]�擾�Ώۂ�transportStreamID
// serviceID				[IN]�擾�Ώۂ�ServiceID
// epgInfoListSize			[OUT]epgInfoList�̌�
// epgInfoList				[OUT]EPG���̃��X�g�iDLL���Ŏ����I��delete����B���Ɏ擾���s���܂ŗL���j
DWORD CEpgDataCap3Main::GetEpgInfoList(
	WORD originalNetworkID,
	WORD transportStreamID,
	WORD serviceID,
	DWORD* epgInfoListSize,
	EPG_EVENT_INFO** epgInfoList
	)
{
	CBlockLock lock(&this->utilLock);
	DWORD err = this->epgDBUtilClass.GetEpgInfoList(originalNetworkID, transportStreamID, serviceID, epgInfoListSize, epgInfoList);
	return err;
}

//�w��T�[�r�X�̌���or����EPG�����擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// originalNetworkID		[IN]�擾�Ώۂ�originalNetworkID
// transportStreamID		[IN]�擾�Ώۂ�transportStreamID
// serviceID				[IN]�擾�Ώۂ�ServiceID
// nextFlag					[IN]TRUE�i���̔ԑg�j�AFALSE�i���݂̔ԑg�j
// epgInfo					[OUT]EPG���iDLL���Ŏ����I��delete����B���Ɏ擾���s���܂ŗL���j
DWORD CEpgDataCap3Main::GetEpgInfo(
	WORD originalNetworkID,
	WORD transportStreamID,
	WORD serviceID,
	BOOL nextFlag,
	EPG_EVENT_INFO** epgInfo
	)
{
	CBlockLock lock(&this->utilLock);
	SYSTEMTIME nowTime;
	if( this->decodeUtilClass.GetNowTime(&nowTime) != NO_ERR ){
		GetLocalTime(&nowTime);
	}
	DWORD err = this->epgDBUtilClass.GetEpgInfo(originalNetworkID, transportStreamID, serviceID, nextFlag, nowTime, epgInfo);
	return err;
}

//�w��C�x���g��EPG�����擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// originalNetworkID		[IN]�擾�Ώۂ�originalNetworkID
// transportStreamID		[IN]�擾�Ώۂ�transportStreamID
// serviceID				[IN]�擾�Ώۂ�ServiceID
// EventID					[IN]�擾�Ώۂ�EventID
// pfOnlyFlag				[IN]p/f����̂݌������邩�ǂ���
// epgInfo					[OUT]EPG���iDLL���Ŏ����I��delete����B���Ɏ擾���s���܂ŗL���j
DWORD CEpgDataCap3Main::SearchEpgInfo(
	WORD originalNetworkID,
	WORD transportStreamID,
	WORD serviceID,
	WORD eventID,
	BYTE pfOnlyFlag,
	EPG_EVENT_INFO** epgInfo
	)
{
	CBlockLock lock(&this->utilLock);

	DWORD err = this->epgDBUtilClass.SearchEpgInfo(originalNetworkID, transportStreamID, serviceID, eventID, pfOnlyFlag, epgInfo);
	return err;
}

//EPG�f�[�^�̒~�Ϗ�Ԃ����Z�b�g����
void CEpgDataCap3Main::ClearSectionStatus()
{
	CBlockLock lock(&this->utilLock);
	this->epgDBUtilClass.ClearSectionStatus();
	return ;
}

//EPG�f�[�^�̒~�Ϗ�Ԃ��擾����
//�߂�l�F
// �X�e�[�^�X
//�����F
// l_eitFlag		[IN]L-EIT�̃X�e�[�^�X���擾
EPG_SECTION_STATUS CEpgDataCap3Main::GetSectionStatus(BOOL l_eitFlag)
{
	CBlockLock lock(&this->utilLock);
	EPG_SECTION_STATUS status = this->epgDBUtilClass.GetSectionStatus(l_eitFlag);
	return status;
}

//PC���v�����Ƃ����X�g���[�����ԂƂ̍����擾����
//�߂�l�F
// ���̕b��
int CEpgDataCap3Main::GetTimeDelay(
	)
{
	CBlockLock lock(&this->utilLock);
	int delay = this->decodeUtilClass.GetTimeDelay();
	return delay;
}
