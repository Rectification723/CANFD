
#include "Bus_Fsm.h"
#include "can_fsm.h"
#include "can_cfg.h"

#include <iostream>

using namespace std;


//! CCanIdle
CCanIdle::CCanIdle()
{}
CCanIdle::~CCanIdle()
{}
/******************************************************************************
* 功能   ： 初始化配置信息、设置采样率、采样位置
* 输入参数：
* 输出参数：
* 返回值  ：
* 说明   ：
* 日期   ：
* 作者   ：
* ***************************************************************************/
void CCanIdle::onEnter(int /*bitVal*/)
{

	mpCanCfg.reset();
	mpCanCfg.setSamplePerBaud(10000000000);
	mpCanCfg.mSamplePerBaudUser = mpCanCfg.getSamplePerBaud();
	mpCanCfg.mSamplePosUser = mpCanCfg.mSamplePos;

	bEOF = false;
}
/******************************************************************************
* 功能   ： 检测7个隐性电平检测开始
* 输入参数：事件、电平
* 输出参数：
* 返回值  ：
* 说明   ：
* 日期   ：
* 作者   ：
* ***************************************************************************/
void CCanIdle::serialIn(int event, int bitVal)
{
	if (event == SAMP)
	{
		mData = (mData << 1) | bitVal;
		mCount++;
		if (mCount >= 7 && (mData & CAN_EOF) == CAN_EOF)
		{
			bEOF = true;
		}
		else if (m_u32Err != BitNormal)//add by rhl for Err Process
		{
			bEOF = true;
		}
	}
	else if (event == FALL && bEOF)
	{
		toNext(bitVal);
		//! clock start samp        
	}

}

//! CCanStart
CCanStart::CCanStart()
{}
CCanStart::~CCanStart()
{}

void CCanStart::onEnter(int /*bitVal*/)
{
	mFillBit = 0;
	mCheckStart = 0;
	crc15 = 0;
	crc17 = 0x10000;
	crc21 = 0x100000;

	m_u32StufCnt = 0;
	m_u32Err = BitNormal;
}

/******************************************************************************
* 功能   ： 检测1个显性电平确认开始
* 输入参数：事件、电平
* 输出参数：
* 返回值  ：
* 说明   ：
* 日期   ：
* 作者   ：
* ***************************************************************************/
void CCanStart::serialIn(int event, int bitVal)
{

	if (event == SAMP)
	{
		if (bitVal == 0)
		{
			if (BitFilled == checkFillBit(bitVal)) //if fill bit then get next
			{
				return;
			}
			crc(bitVal);
			crc_FD(bitVal);
			toNext(bitVal);
		}
		else
		{
			toPrev(bitVal);
		}
	}
}

void CCanStart::onExit(int /*bitVal*/)
{
}



//! CCanArb
CCanArb::CCanArb()
{}
CCanArb::~CCanArb()
{}

void CCanArb::onEnter(int /*bitVal*/)
{
	mErr = false;
	mCount = 0;
	mData = 0;
	mRTR = RECESSIVE;
	mIDE = CAN_STD;
	mEDL = CAN_STD;
	mBRS = ERRFLAG;
	mExtCount = 0;
	mIdentifier = 0;
	mFrame.setBegin(mBeginT);

}
/******************************************************************************
* 功能   ： 检测ARB域区别 标准扩展帧 标准帧与CAN FD帧
* 输入参数：事件、电平
* 输出参数：
* 返回值  ：
* 说明   ：
* 日期   ：
* 作者   ：
* ***************************************************************************/
void CCanArb::serialIn(int event, int bitVal)
{
	if (event == SAMP)
	{


		if (mCount == 0)
		{

		}

		//rhl--add CAN-FD CRC Check
		crc_FD(bitVal);

		int bit = checkFillBit(bitVal);
		//check bit fill
		if (bit == BitFilled)
		{
			m_u32StufCnt++; //rhl--2020-3-5 STUFF Count
			return;
		}
		else if (bit == BitError)//add by rhl
		{
			m_u32Err = BitError;
			toErr(bitVal);

			return;
		}

		crc(bitVal);

		if (mIDE == CAN_STD)
		{
			stdFrame(bitVal);
		}
		else if (mIDE == CAN_EXT)
		{
			extFrame(bitVal);
		}
	}
}

void CCanArb::stdFrame(int bitVal)
{
	mCount++;
	if (mCount <= 11)
	{
		if (mCount == 1)
		{
			mFrame.CID.mBegintime = mBeginT;
		}

		mIdentifier = (mIdentifier << 1) | bitVal;
		if (mCount == 7)
		{
			if ((mIdentifier & CAN_EOF) == CAN_EOF)
			{
				mErr = true;
			}
		}
		return;
	}
	if (mCount == 12)
	{
		mFrame.CID.mLen = 11;
		mFrame.CID.mEndtime = mBeginT;
		mR = bitVal;

	}

	if (mCount == 13)
	{
		mIDE = bitVal;
		if (mIDE == CAN_EXT)
		{
			mExtCount = 0;
		}
		mRTR = mR;
	}
	if (mCount == 14)
	{

		mEDL = bitVal;

		if (mEDL == CAN_STD)
		{
			toNext(bitVal);
		}
	}
	if (mCount == 16)
	{
		mBRS = bitVal;
		if (mBRS)                // 修改间隔采样率
		{
			mpCanCfg.mSamplePerBaudUser = mpCanCfg.getFDSamplePeriod();
			mpCanCfg.mSamplePosUser = mpCanCfg.mFDSamplePos;
		}
	}
	if (mCount == 17)
	{
		mFrame.CID.mLen = 11;
		mESI = bitVal;
		toNext(bitVal);
	}
}

void CCanArb::extFrame(int bitVal)
{
	mExtCount++;
	if (mExtCount <= 18)
	{
		mIdentifier = (mIdentifier << 1) | bitVal;
	}

	else
	{


		if (mExtCount == 19)
		{
			mRTR = bitVal;
			mFrame.CID.mEndtime = mBeginT;
			mFrame.CID.mLen = 29;
		}
		if (mExtCount == 20)
		{
			mEDL = bitVal;
		}

		if (mExtCount == 21)
		{


			if (mEDL == CAN_STD)
			{
				toNext(bitVal);
			}
		}

		if (mExtCount == 22)
		{
			mBRS = bitVal;
			if (mBRS)
			{
				mpCanCfg.mSamplePerBaudUser = mpCanCfg.getFDSamplePeriod();
				mpCanCfg.mSamplePosUser = mpCanCfg.mFDSamplePos;
			}

		}

		if (mExtCount == 23)
		{

			mESI = bitVal;
			mFrame.CID.mLen = 29;
			toNext(bitVal);
		}
	}
}

void CCanArb::onExit(int /*bitVal*/)
{

	mFrame.CID.mDataFrame = mIdentifier;


}

//! CCanCtl
CCanCtl::CCanCtl()
{}
CCanCtl::~CCanCtl()
{}
void CCanCtl::onEnter(int /*bitVal*/)
{
	mCount = 0;
	mErr = false;
	mDLC = 0;
	mDLCTemp = 0;
	mFrame.CDLC.mDataFrame = 0;
	mFrame.CDLC.mLen = 0;
	mFrame.CDLC.mEndtime = 0;
}
/******************************************************************************
* 功能   ： 检测DLC长度
* 输入参数：事件、电平
* 输出参数：
* 返回值  ：
* 说明   ：
* 日期   ：
* 作者   ：
* ***************************************************************************/

void CCanCtl::serialIn(int event, int bitVal)
{

	if (event == SAMP)
	{

		if (mCount == 0)
		{
			mFrame.CDLC.mBegintime = mBeginT;
		}

		crc_FD(bitVal);

		int bit = checkFillBit(bitVal);

		if (bit == BitFilled)
		{
			m_u32StufCnt++;
			return;
		}
		else if (bit == BitError)
		{
			m_u32Err = BitError;
			toErr(bitVal);
			return;
		}

		crc(bitVal);
		mDLC = (mDLC << 1) | bitVal;
		mCount++;
		if (mCount == 4)
		{
			
			mFrame.CDLC.mEndtime = mBeginT;
			if (mDLC >= 9 && mDLC <= 12)
			{
				mDLC = (mDLC - 8) * 4 + 8;
			}
			if (mDLC == 13)
			{
				mDLC = 32;
			}
			if (mDLC == 14)
			{
				mDLC = 48;
			}
			if (mDLC == 15)
			{
				mDLC = 64;
			}

			if (mDLC == 4)
			{
			}
			toNext(bitVal);
			if (mRTR == CAN_FRAME_RMT || mDLC == 0)
			{
				now()->toNext(bitVal);
			}
		}
	}
}

void CCanCtl::onExit(int /*bitVal*/)
{
	mDLCTemp = mDLC;
	mFrame.CDLC.mDataFrame = mDLC;
	mFrame.CDLC.mLen = 4;
	
}

//! CCanData
CCanData::CCanData()
{}
CCanData::~CCanData()
{}
/******************************************************************************
* 功能   ： 根据DLC长度读入数据	
* 输入参数：事件、电平
* 输出参数：
* 返回值  ：
* 说明   ：
* 日期   ：
* 作者   ：
* ***************************************************************************/
void CCanData::onEnter(int /*bitVal*/)
{
	mCount = 0;
	mData = 0;
	mErr = false;
	DataTemp.mBegintime = mBeginT;

}

void CCanData::serialIn(int event, int bitVal)
{

	if (event == SAMP)
	{
		crc_FD(bitVal);
		int bit = checkFillBit(bitVal);
		if (bit == BitFilled)
		{
			m_u32StufCnt++;
			return;
		}
		else if (bit == BitError)
		{
			mCount = 0;
			m_u32Err = BitError;
			toErr(bitVal);

			return;
		}

		crc(bitVal);
		mData = (mData << 1) | bitVal;
		mCount++;

		if (mCount == 8)
		{
			mDLCTemp--;
			if (mDLCTemp == 0)
			{
				toNext(bitVal);
			}
			else if (mDLCTemp > 0)
			{
				toSelf(bitVal);
			}
		}
	}
}

void CCanData::onExit(int bitVal)
{
	
	DataTemp.mDataFrame = mData;
	DataTemp.mLen = mCount;
	DataTemp.mEndtime = mBeginT;
	mFrame.CData.push_back(DataTemp);
}
CCanCrc::CCanCrc()
{
	u32aGrayCode[7] = 0x09;
	u32aGrayCode[6] = 0x0a;
	u32aGrayCode[5] = 0x0f;
	u32aGrayCode[4] = 0x0c;
	u32aGrayCode[3] = 0x05;
	u32aGrayCode[2] = 0x06;
	u32aGrayCode[1] = 0x03;
	u32aGrayCode[0] = 0x00;
}
CCanCrc::~CCanCrc()
{
}
void CCanCrc::onEnter(int bitVal)
{
	mCount = 0;
	mData = 0;
	mErr = false;
	m_s32LastBit = bitVal;
	m_s32Polarity = 0;
	m_s32FixStfCnt = 0;

}

/******************************************************************************
* 函数名    ： CCanCrc::serialIn
* 功能        ： Can-FD/Can CRC field process
* 输入参数： int event; int bitVal
* 输出参数：
* 返回值    ：
* 说明        ：
* 日期        ：
* 作者        ：
* 修改记录：修改CAN-FD CRC处理，
*                    增加STUFF Count 字段(4bit)，
*                    修填充规则(CAN-FD使用固定填充位)
* 修改日期：2020-3-5
* 修改人    ：rhl
* ***************************************************************************/
void CCanCrc::serialIn(int event, int bitVal)
{
	if (event == SAMP)
	{

		if (mCount == 0) 
		{
			mFrame.CCrc.mBegintime = mBeginT;
		}
		if (mEDL == CAN_STD)//CAN
		{
			CanCrc(bitVal);
		}
		else {
			CanFDCrc(bitVal);
		}
	}
}

/******************************************************************************
* 函数名    ： CCanCrc::CanCrc
* 功能        ： Can CRC field process
* 输入参数： int bitVal
* 输出参数：
* 返回值    ：
* 说明        ：
* 日期        ：
* 作者        ：
* 修改记录：将CAN与CAN-FD CRC计算独立封装
* 修改日期：2020-3-5
* 修改人    ：rhl
* ***************************************************************************/
void CCanCrc::CanCrc(int bitVal)
{
	int bit = checkFillBit(bitVal);
	//check bit fill
	if (bit == BitFilled)
	{
		return;
	}
	else if (bit == BitError)
	{
		m_u32Err = BitError;
		toErr(bitVal);

		return;
	}

	mData = (mData << 1) | bitVal;
	mCount++;

	//with delimiter bit. for bug1950 by hxh
	if (mCount == 16)
	{
		mData = mData >> 1;
		mCount--;
		mErr = (crc15 != mData);
		toNext(bitVal);
	}
}


/******************************************************************************
* 函数名    ： CCanCrc::CanFDCrc
* 功能        ： Can-FD CRC field process
* 输入参数： int bitVal
* 输出参数：
* 返回值    ：
* 说明        ：
* 日期        ：
* 作者        ：
* 修改记录：修改CAN-FD CRC处理，
*                    增加STUFF Count 字段(4bit)，
*                    修填充规则(CAN-FD使用固定填充位)
* 修改日期：2020-3-5
* 修改人    ：rhl
* ***************************************************************************/
void CCanCrc::CanFDCrc(int bitVal)
{
	if (mCount % 5 == 0)
	{
		m_s32FixStfCnt++;
		if (bitVal == m_s32LastBit) {

			m_u32Err = BitError;
			toErr(bitVal);
		}
	}
	else
	{
		m_s32LastBit = bitVal;
		mData = (mData << 1) | bitVal;
	}
	if (mCount > 0 && mCount <= 4)
	{
		m_s32Polarity += bitVal;
		crc_FD(bitVal);
		if (mCount == 4)
		{
			if (m_s32Polarity % 2 != 0)
			{
				m_u32Err = e_ParityErr;
				toErr(bitVal);

			}
			else if (mData != u32aGrayCode[m_u32StufCnt % 8])
			{
				m_u32Err = e_StuffCntErr;
				toErr(bitVal);

			}
		}
	}

	mCount++;


	if (mDLC <= 16 && mCount == 28)
	{
		mData = mData >> 1;
		mCount -= (m_s32FixStfCnt + 1);

		mErr = (crc17 != (mData & 0x1ffff));
		toNext(bitVal);
	}
	else if (mDLC >= 17 && mDLC <= 64 && mCount == 33)
	{
		mData = mData >> 1;
		mCount -= (m_s32FixStfCnt + 1);

		mErr = (crc21 != (mData & 0x1fffff));
		toNext(bitVal);
	}
}

void CCanCrc::onExit(int /*bitVal*/)
{

	mpCanCfg.mSamplePerBaudUser = mpCanCfg.getSamplePerBaud();
	mpCanCfg.mSamplePosUser = mpCanCfg.mSamplePos;

	mFrame.CCrc.mDataFrame = mData;

	mFrame.CCrc.mLen = mCount;
	mFrame.CCrc.mEndtime = mBeginT;

}

//! CCanAck
CCanAck::CCanAck()
{}
CCanAck::~CCanAck()
{}
void CCanAck::onEnter(int /*bitVal*/)
{
	mCount = 0;
	mData = 0;
	mErr = false;

}
/******************************************************************************
* 功能   ：检测ack 
* 输入参数：事件、电平
* 输出参数：
* 返回值  ：
* 说明   ：当ack为1时说明没有应答 显示错误
* 日期   ：
* 作者   ：
* ***************************************************************************/
void CCanAck::serialIn(int event, int bitVal)
{
	if (event == SAMP)
	{
		if (mCount==0)
		{
			mFrame.CAck.mBegintime = mBeginT;
		}
		mData = (mData << 1) | bitVal;
		mCount++;
		if (mCount == 2)
		{

			toNext(bitVal);//to end
		}
	}
}


void CCanAck::onExit(int /*bitVal*/)
{
	mFrame.CAck.mDataFrame = mData;

	mFrame.CAck.mLen = mCount;
	mFrame.CAck.mEndtime = mBeginT;
}

//! CCanEnd
CCanEnd::CCanEnd()
{}
CCanEnd::~CCanEnd()
{}
void CCanEnd::onEnter(int /*bitVal*/)
{
	mCount = 0;
	mData = 0;
	mErr = false;

}


void CCanEnd::serialIn(int event, int bitVal)
{

	switch (event)
	{
	case SAMP:
		mData = (mData << 1) | bitVal;
		mCount++;
		break;


	case FALL:
		if (mCount == 7)
		{
			toNext(bitVal);
		}
		break;
	default:
		break;
	}
	if (mCount == 8)
	{
		toIdle(bitVal);
	}
}

void CCanEnd::onExit(int /*bitVal*/)
{

}



CCanError::CCanError()
{}
CCanError::~CCanError()
{}
void CCanError::onEnter(int /*bitVal*/)
{
	mCount = 0;
	mData = 0;

}

void CCanError::serialIn(int event, int bitVal)
{
	if (event == SAMP)
	{
		mData = (mData << 1) | bitVal;
		mCount++;
		if (mCount >= 7 && (mData & CAN_EOF) == CAN_EOF)
		{
			toIdle(bitVal);
		}
	}
}

void CCanError::onExit(int /*bitVal*/)
{



}


//! CCanOverload
CCanOverload::CCanOverload()
{}
CCanOverload::~CCanOverload()
{}

void CCanOverload::onEnter(int /*bitVal*/)
{
	mCount = 0;
	mData = 0;

}

void CCanOverload::serialIn(int event, int bitVal)
{
	switch (event)
	{
	case SAMP:
		mData = (mData << 1) | bitVal;
		mCount++;
		break;
	case FALL:
		if (mCount == 7)
		{
			toSelf(bitVal);
		}
		break;
	default:
		break;
	}
	if (mCount == 8)
	{
		toIdle(bitVal);
	}
}

void CCanOverload::onExit(int /*bitVal*/)
{
}
