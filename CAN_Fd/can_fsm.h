#ifndef _CAN_FSM_H
#define _CAN_FSM_H


#include "Bus_Fsm.h"
#include "bus_decoder.h"
#include "can_cfg.h"






#define ERRFLAG             3

#define CAN_EOF             (0x7F)

#define CAN_STD             (0)
#define CAN_EXT             (1)

//CAN with Flexible Data-rate
#define CAN_FD              (1)

#define DOMINANT            (0)
#define RECESSIVE           (1)

#define CAN_FRAME_DATA      (0)
#define CAN_FRAME_RMT       (1)

enum
{
	BitNormal,
	BitFilled,
	BitError,         //stuff Err

	e_ParityErr,    //add by rhl for CanFD
	e_StuffCntErr
};

//add by rhl for toErr func
enum
{
	e_Start,
	e_Arb,
	e_Ctrl,
	e_Data,
	e_Crc,
	e_Ack
};


class CCanDecoder : public CDecoder, public CBusState
{
public:
	CCanDecoder();
	virtual ~CCanDecoder();

public:
	void rst();

public:

	static CCanCfg mpCanCfg;
	static CFrame mFrame;
	static uint mIdentifier;
	static uint mRTR;
	static uint mBRS;
	static uint mEDL;
	static uint mDLC;
	static uint mDLCTemp;
	static uint mIDE;
	static long mBeginT;



	static uint  crc15;
	static uint  crc17;
	static uint  crc21;

	static uint m_u32StufCnt;
	static uint m_u32Err;

	static int  mFillBit;
	static int  mCheckStart;

	static int  checkFillBit(int);
	static void crc(int);


	static void crc_FD(int);

public:
	bool decode( bool *mdata, long size);
};

class CCanIdle : public CCanDecoder
{
public:
	CCanIdle();
	virtual ~CCanIdle();

public:
	void onEnter(int bitVal);
	void serialIn(int event, int bitVal);
private:
	bool bEOF;
};

class CCanStart : public CCanDecoder
{
public:
	CCanStart();
	virtual ~CCanStart();


public:
	void onEnter(int bitVal);
	void serialIn(int event, int bitVal);
	void onExit(int bitVal);
};

//Arbitration field. with ext,fd
class CCanArb : public CCanDecoder
{
public:
	CCanArb();
	virtual ~CCanArb();

protected:
	uint mR;
	uint mESI;
	bool mErr;
	uint mExtCount;

public:
	void onEnter(int bitVal);
	void onExit(int bitVal);
	void serialIn(int event, int bitVal);

	void stdFrame(int bitVal);
	void extFrame(int bitVal);
};

class CCanCtl : public CCanDecoder
{
public:
	CCanCtl();
	virtual ~CCanCtl();

protected:
	bool mErr;
public:
	void onEnter(int bitVal);
	void onExit(int bitVal);
	void serialIn(int event, int bitVal);
};

class CCanData : public CCanDecoder
{
public:
	CCanData();
	virtual ~CCanData();

protected:
	CFiled DataTemp;
	uint mDataLen;
	bool mErr;
public:
	void onEnter(int bitVal);
	void onExit(int bitVal);
	void serialIn(int event, int bitVal);
};

class CCanCrc : public CCanDecoder
{
public:
	CCanCrc();
	virtual ~CCanCrc();

protected:
	bool mErr;
	int m_s32LastBit;             //add by rhl for fixed stuff bit
	int m_s32Polarity;
	int m_s32FixStfCnt;           //CRC固定填充位计数
	unsigned int u32aGrayCode[8]; //前3bit为0-7对应格雷码，后1bit为格雷码奇偶校验位

public:
	void onEnter(int bitVal);
	void onExit(int bitVal);
	void serialIn(int event, int bitVal);

	//add by rhl for Can/CanFD CRC Process
	void CanCrc(int bitVal);
	void CanFDCrc(int bitVal);
};

class CCanAck : public CCanDecoder
{
public:
	CCanAck();
	virtual ~CCanAck();
protected:
	bool mErr;

public:
	void onEnter(int bitVal);
	void onExit(int bitVal);
	void serialIn(int event, int bitVal);
};

class CCanEnd : public CCanDecoder
{
public:
	CCanEnd();
	virtual ~CCanEnd();
protected:
	bool mErr;

public:
	void onEnter(int bitVal);
	void onExit(int bitVal);
	void serialIn(int event, int bitVal);
};

class CCanError : public CCanDecoder
{
public:
	CCanError();
	virtual ~CCanError();

public:
	void onEnter(int bitVal);
	void onExit(int bitVal);
	void serialIn(int event, int bitVal);
};

class CCanOverload : public CCanDecoder
{
public:
	CCanOverload();
	virtual ~CCanOverload();

public:
	void onEnter(int bitVal);
	void onExit(int bitVal);
	void serialIn(int event, int bitVal);
};

#endif // BUS_CAN_FSM_H

