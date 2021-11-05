#include "Bus_Fsm.h"
#include "can_fsm.h"
#include "can_input.h"
#include "can_cfg.h"


static CCanIdle     fsm_can_idle;
static CCanStart    fsm_can_start;
static CCanArb      fsm_can_arb;
static CCanCtl      fsm_can_ctl;
static CCanData     fsm_can_data;
static CCanCrc      fsm_can_crc;
static CCanAck      fsm_can_ack;
static CCanEnd      fsm_can_end;
static CCanError    fsm_can_err;
static CCanOverload fsm_can_overload;

static unsigned int crc15Poly = 0x4599;//CRC_15
static unsigned int crc17Poly = 0x1685B;//CRC_17      CRC��λȡ��1
static unsigned int crc21Poly = 0x102899;//CRC_21

static unsigned int crc15Width = 15;
static unsigned int crc17Width = 17;
static unsigned int crc21Width = 21;


static unsigned int crc15Mask = 0x7fff;
static unsigned int crc17Mask = 0x1ffff;
static unsigned int crc21Mask = 0x1fffff;

static unsigned int crc15MaxBit = 1 << (crc15Width - 1);
static unsigned int crc17MaxBit = 1 << (crc17Width - 1);
static unsigned int crc21MaxBit = 1 << (crc21Width - 1);

uint CCanDecoder::mRTR;
uint CCanDecoder::mBRS;
uint CCanDecoder::mEDL;
uint CCanDecoder::mDLC;
uint CCanDecoder::mDLCTemp;
uint CCanDecoder::mIDE;
uint CCanDecoder::mIdentifier;
int  CCanDecoder::mCheckStart;
int  CCanDecoder::mFillBit;
long CCanDecoder::mBeginT;
uint  CCanDecoder::crc15;
uint  CCanDecoder::crc17;
uint  CCanDecoder::crc21;

uint CCanDecoder::m_u32Err;         //rhl--for CAN FD
uint CCanDecoder::m_u32StufCnt;

CCanCfg  CCanDecoder::mpCanCfg;
CFrame   CCanDecoder::mFrame;
/******************************************************************************
* ������ �� CCanDecoder
* ����   �� ����״̬��
* ��������� 
* ���������
* ����ֵ  ��
* ˵��   �� 
* ����   ��
* ����   ��
* ***************************************************************************/
CCanDecoder::CCanDecoder()
{
	mpNow = &fsm_can_idle;
	fsm_can_idle.setNextStat(&fsm_can_start);
	fsm_can_start.setNextStat(&fsm_can_arb);
	fsm_can_arb.setNextStat(&fsm_can_ctl);
	fsm_can_ctl.setNextStat(&fsm_can_data);
	fsm_can_data.setNextStat(&fsm_can_crc);
	fsm_can_crc.setNextStat(&fsm_can_ack);
	fsm_can_ack.setNextStat(&fsm_can_idle);

	fsm_can_end.setNextStat(&fsm_can_overload);
	fsm_can_err.setNextStat(&fsm_can_idle);
	setErr(&fsm_can_err);
	setIdle(&fsm_can_idle);
	
}
CCanDecoder::~CCanDecoder()
{

}

//! rst to idle
void CCanDecoder::rst()
{
	mpNow = &fsm_can_idle;


}
/******************************************************************************
* ������ �� checkFillBit
* ����   �� ����Ƿ���Ҫbit���
* ��������� bitֵ
* ���������
* ����ֵ  ��1 or 0
* ˵��   �� ����bit Normal��ֵ��������ߵ͵�ƽ���� Ϊ1
* ����   ��
* ����   ��
* ***************************************************************************/
int CCanDecoder::checkFillBit(int bit)
{
	int b = BitNormal;

	mFillBit = mFillBit & 0x1f;

	if (mCheckStart >= 5 &&
		(mFillBit == 0x1f || mFillBit == 0))
	{
		b = BitFilled;
		if ((mFillBit & 1) == bit)//
		{
			b = BitError;
		}
	}
	mFillBit = (mFillBit << 1) | bit;
	mCheckStart++;


	return b;
}

/******************************************************************************
* ������ �� crc
* ����   �� Can ֡CRCУ�����
* ��������� bitֵ
* ��������� ��
* ����ֵ  ��
* ˵��   ��
* ����   ��
* ����   ��
* �޸ļ�¼����CRC15��17��21�ֿ�����
* �޸�ʱ�䣺2020-3-5
* �޸�ԭ��Can��Can-FD CRC���㣬������Դ��ͬ
* ***************************************************************************/
void CCanDecoder::crc(int bit) {

	uint crcNext = (crc15 & crc15MaxBit) != 0;
	crcNext = crcNext ^ bit;

	crc15 = crc15 << 1;
	crc15 = crc15 & crc15Mask;


	if (crcNext) {
		crc15 = crc15 ^ crc15Poly;
	}
}
/******************************************************************************
* ������    �� crc_FD
* ����        �� Can-FD 17/21 bit CRC����
* ��������� bitֵ
* ���������
* ����ֵ    ��
* ˵��        ��
* ����        �� 2020-3-5
* ����        �� rhl
* �޸ļ�¼��
* ***************************************************************************/
void CCanDecoder::crc_FD(int bit)
{
	uint crcNext = (crc17 & crc17MaxBit) != 0;
	crcNext = crcNext ^ bit;
	crc17 = crc17 << 1;
	crc17 = crc17 & crc17Mask;
	if (crcNext)
	{
		crc17 = crc17 ^ crc17Poly;
	}

	crcNext = (crc21 & crc21MaxBit) != 0;
	crcNext = crcNext ^ bit;
	crc21 = crc21 << 1;
	crc21 = crc21 & crc21Mask;
	if (crcNext)
	{
		crc21 = crc21 ^ crc21Poly;
	}
}

/******************************************************************************
* ������ �� decode
* ����   �� Can-Fd֡�����ຯ��
* ��������� ͨ��������ָ�롢����
* ��������� 
* ����ֵ  ��false or true
* ˵��   ��
* ����   ��
* ����   ��
* �޸ļ�¼��
* �޸�ʱ�䣺
* �޸�ԭ����ֲƽ̨
* ***************************************************************************/

bool CCanDecoder::decode(bool *pdata,long size)
{
	int last_dat, next_dat;
	
	int count = 0;

	mBeginT = 0;
	if(!pdata)
	{
		return false;
	}
	
	last_dat = *pdata;
	now()->onEnter(last_dat);
	for (mBeginT = (mpCanCfg.mSamplePosUser / 100.0* mpCanCfg.getSamplePerBaud()); mBeginT < size; mBeginT++)
	{

		count++;

		if (mpCanCfg.mSignalType == SIGTYPE_RX || mpCanCfg.mSignalType == SIGTYPE_CAN_H)
		{
			*(pdata + mBeginT) = !(*(pdata + mBeginT));
		}

		next_dat = *(pdata + mBeginT);

		if (next_dat > last_dat)
		{

			now()->serialIn(RISE, next_dat);
			last_dat = next_dat;

		}
		else if (next_dat < last_dat)
		{

			now()->serialIn(FALL, next_dat);
			last_dat = next_dat;

		}

		if (count == mpCanCfg.mSamplePerBaudUser)
		{
			count = 0;
			now()->serialIn(SAMP, next_dat);

		}
	}

	return true;
}

