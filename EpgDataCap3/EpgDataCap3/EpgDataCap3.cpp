// EpgDataCap3.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
//

#include "stdafx.h"

#include "EpgDataCap3Main.h"
#include "../../Common/ErrDef.h"
#include "../../Common/BlockLock.h"

class CInstanceManager
{
public:
	static const DWORD INVALID_ID = 0xFFFFFFFF;

	CInstanceManager()
	{
		InitializeCriticalSection(&(this->m_lock));
		this->m_nextID = 1;
	}

	~CInstanceManager()
	{
		DeleteCriticalSection(&(this->m_lock));
	}

	DWORD initialize(BOOL asyncFlag, DWORD *id)
	{
		CBlockLock lock(&(this->m_lock));

		std::shared_ptr<CEpgDataCap3Main> ptr;

		*id = INVALID_ID;

		try {
			ptr = std::make_shared<CEpgDataCap3Main>();
		} catch (std::bad_alloc &) {
			return ERR_FALSE;
		}

		DWORD err = ptr->Initialize(asyncFlag);
		if (err != NO_ERR) {
			return err;
		}

		try {
			*id = this->getNextID();
			this->m_list.insert(std::make_pair(*id,ptr));
		} catch (std::bad_alloc &) {
			*id = INVALID_ID;
			return ERR_FALSE;
		}

		return err;
	}

	DWORD uninitialize(DWORD id)
	{
		CBlockLock lock(&(this->m_lock));

		std::shared_ptr<CEpgDataCap3Main> ptr = this->find(id);
		if (ptr == NULL) {
			return ERR_NOT_INIT;
		}

		DWORD err = ptr->UnInitialize();
		this->m_list.erase(id);

		return err;
	}

	std::shared_ptr<CEpgDataCap3Main> find(DWORD id)
	{
		CBlockLock lock(&(this->m_lock));

		if (this->m_list.count(id) == 0) {
			return NULL;
		}

		return this->m_list.at(id);
	}

protected:
	std::map<DWORD, std::shared_ptr<CEpgDataCap3Main> > m_list;
	DWORD m_nextID;
	CRITICAL_SECTION m_lock;

	DWORD getNextID()
	{
		CBlockLock lock(&(this->m_lock));

		DWORD nextID = INVALID_ID;
		int count = 0;
		do {
			if (this->m_list.count(this->m_nextID) == 0) {
				nextID = this->m_nextID;
			}

			this->m_nextID += 1;
			if ((this->m_nextID == 0)|| (this->m_nextID == INVALID_ID)) {
				this->m_nextID = 1;
			}
			count += 1;

			// 65536 �񎎂��ă_����������f�O
			// 1 �v���Z�X���� (2^16) �ȏ�̃C���X�^���X�𓯎��ɍ쐬���邱�Ƃ͂Ȃ��͂�

		} while ((nextID == INVALID_ID) && (count < (1<<16))); 

		return nextID;
	}
};

CInstanceManager g_instMng;

//DLL�̏�����
//�߂�l�F
// �G���[�R�[�h
//�����F
// asyncMode		[IN]TRUE:�񓯊����[�h�AFALSE:�������[�h
// id				[OUT]����ID
DWORD WINAPI InitializeEP(
	BOOL asyncFlag,
	DWORD* id
	)
{
	if (id == NULL) {
		return ERR_INVALID_ARG;
	}

	DWORD err = g_instMng.initialize(asyncFlag, id);

	_OutputDebugString(L"EgpDataCap3.dll [InitializeEP : id=%d]\n", *id);

	return err;
}

//DLL�̊J��
//�߂�l�F
// �G���[�R�[�h
//�����F
// id		[IN]����ID InitializeEP�̖߂�l
DWORD WINAPI UnInitializeEP(
	DWORD id
	)
{
	_OutputDebugString(L"EgpDataCap3.dll [UnInitializeEP : id=%d]\n", id);

	DWORD err = g_instMng.uninitialize(id);

	return err;
}

//��͑Ώۂ�TS�p�P�b�g�P��ǂݍ��܂���
//�߂�l�F
// �G���[�R�[�h
// id		[IN]����ID InitializeEP�̖߂�l
// data		[IN]TS�p�P�b�g�P��
// size		[IN]data�̃T�C�Y�i188�A192������ɂȂ�͂��j
DWORD WINAPI AddTSPacketEP(
	DWORD id,
	BYTE* data,
	DWORD size
	)
{
	std::shared_ptr<CEpgDataCap3Main> ptr = g_instMng.find(id);
	if (ptr == NULL) {
		return ERR_NOT_INIT;
	}

	return ptr->AddTSPacket(data, size);
}

//��̓f�[�^�̌��݂̃X�g���[���h�c���擾����
//�߂�l�F
// �G���[�R�[�h
// id						[IN]����ID
// originalNetworkID		[OUT]���݂�originalNetworkID
// transportStreamID		[OUT]���݂�transportStreamID
DWORD WINAPI GetTSIDEP(
	DWORD id,
	WORD* originalNetworkID,
	WORD* transportStreamID
	)
{
	std::shared_ptr<CEpgDataCap3Main> ptr = g_instMng.find(id);
	if (ptr == NULL) {
		return ERR_NOT_INIT;
	}

	return ptr->GetTSID(originalNetworkID, transportStreamID);
}

//���X�g���[���̃T�[�r�X�ꗗ���擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// id						[IN]����ID
// serviceListSize			[OUT]serviceList�̌�
// serviceList				[OUT]�T�[�r�X���̃��X�g�iDLL���Ŏ����I��delete����B���Ɏ擾���s���܂ŗL���j
DWORD WINAPI GetServiceListActualEP(
	DWORD id,
	DWORD* serviceListSize,
	SERVICE_INFO** serviceList
	)
{
	std::shared_ptr<CEpgDataCap3Main> ptr = g_instMng.find(id);
	if (ptr == NULL) {
		return ERR_NOT_INIT;
	}

	return ptr->GetServiceListActual(serviceListSize, serviceList);
}

//�~�ς��ꂽEPG���̂���T�[�r�X�ꗗ���擾����
//SERVICE_EXT_INFO�̏��͂Ȃ��ꍇ������
//�߂�l�F
// �G���[�R�[�h
//�����F
// id						[IN]����ID
// serviceListSize			[OUT]serviceList�̌�
// serviceList				[OUT]�T�[�r�X���̃��X�g�iDLL���Ŏ����I��delete����B���Ɏ擾���s���܂ŗL���j
DWORD WINAPI GetServiceListEpgDBEP(
	DWORD id,
	DWORD* serviceListSize,
	SERVICE_INFO** serviceList
	)
{
	std::shared_ptr<CEpgDataCap3Main> ptr = g_instMng.find(id);
	if (ptr == NULL) {
		return ERR_NOT_INIT;
	}

	return ptr->GetServiceListEpgDB(serviceListSize, serviceList);
}

//�w��T�[�r�X�̑SEPG�����擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// id						[IN]����ID
// originalNetworkID		[IN]�擾�Ώۂ�originalNetworkID
// transportStreamID		[IN]�擾�Ώۂ�transportStreamID
// serviceID				[IN]�擾�Ώۂ�ServiceID
// epgInfoListSize			[OUT]epgInfoList�̌�
// epgInfoList				[OUT]EPG���̃��X�g�iDLL���Ŏ����I��delete����B���Ɏ擾���s���܂ŗL���j
DWORD WINAPI GetEpgInfoListEP(
	DWORD id,
	WORD originalNetworkID,
	WORD transportStreamID,
	WORD serviceID,
	DWORD* epgInfoListSize,
	EPG_EVENT_INFO** epgInfoList
	)
{
	std::shared_ptr<CEpgDataCap3Main> ptr = g_instMng.find(id);
	if (ptr == NULL) {
		return ERR_NOT_INIT;
	}

	return ptr->GetEpgInfoList(originalNetworkID, transportStreamID, serviceID, epgInfoListSize, epgInfoList);
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
DWORD WINAPI GetEpgInfoEP(
	DWORD id,
	WORD originalNetworkID,
	WORD transportStreamID,
	WORD serviceID,
	BOOL nextFlag,
	EPG_EVENT_INFO** epgInfo
	)
{
	std::shared_ptr<CEpgDataCap3Main> ptr = g_instMng.find(id);
	if (ptr == NULL) {
		return ERR_NOT_INIT;
	}

	return ptr->GetEpgInfo(originalNetworkID, transportStreamID, serviceID, nextFlag, epgInfo);
}

//�w��C�x���g��EPG�����擾����
//�߂�l�F
// �G���[�R�[�h
//�����F
// id						[IN]����ID
// originalNetworkID		[IN]�擾�Ώۂ�originalNetworkID
// transportStreamID		[IN]�擾�Ώۂ�transportStreamID
// serviceID				[IN]�擾�Ώۂ�ServiceID
// eventID					[IN]�擾�Ώۂ�EventID
// pfOnlyFlag				[IN]p/f����̂݌������邩�ǂ���
// epgInfo					[OUT]EPG���iDLL���Ŏ����I��delete����B���Ɏ擾���s���܂ŗL���j
DWORD WINAPI SearchEpgInfoEP(
	DWORD id,
	WORD originalNetworkID,
	WORD transportStreamID,
	WORD serviceID,
	WORD eventID,
	BYTE pfOnlyFlag,
	EPG_EVENT_INFO** epgInfo
	)
{
	std::shared_ptr<CEpgDataCap3Main> ptr = g_instMng.find(id);
	if (ptr == NULL) {
		return ERR_NOT_INIT;
	}

	return ptr->SearchEpgInfo(originalNetworkID, transportStreamID, serviceID, eventID, pfOnlyFlag, epgInfo);
}

//EPG�f�[�^�̒~�Ϗ�Ԃ����Z�b�g����
//�����F
// id						[IN]����ID
void WINAPI ClearSectionStatusEP(
	DWORD id
	)
{
	std::shared_ptr<CEpgDataCap3Main> ptr = g_instMng.find(id);
	if (ptr == NULL) {
		return;
	}

	ptr->ClearSectionStatus();
}

//EPG�f�[�^�̒~�Ϗ�Ԃ��擾����
//�߂�l�F
// �X�e�[�^�X
//�����F
// id						[IN]����ID
// l_eitFlag				[IN]L-EIT�̃X�e�[�^�X���擾
EPG_SECTION_STATUS WINAPI GetSectionStatusEP(
	DWORD id,
	BOOL l_eitFlag
	)
{
	std::shared_ptr<CEpgDataCap3Main> ptr = g_instMng.find(id);
	if (ptr == NULL) {
		return EpgNoData;
	}

	return ptr->GetSectionStatus(l_eitFlag);
}

//PC���v�����Ƃ����X�g���[�����ԂƂ̍����擾����
//�߂�l�F
// ���̕b��
//�����F
// id						[IN]����ID
int WINAPI GetTimeDelayEP(
	DWORD id
	)
{
	std::shared_ptr<CEpgDataCap3Main> ptr = g_instMng.find(id);
	if (ptr == NULL) {
		return EpgNoData;
	}

	return ptr->GetTimeDelay();
}