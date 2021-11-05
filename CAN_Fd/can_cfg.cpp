#include "can_cfg.h"


void CCanCfg::reset()
{
	mSrc = chan1;
	mBaud = BaudRate_500000;
	mFDBaud = BaudRate_1Mbps;
	mSignalType = SIGTYPE_CAN_L;
	mSamplePos = 50;
	mFDSamplePos = 50;

}
void CCanCfg::setSamplePerBaud(unsigned long long sample)
{

	////////////////CAN-STD config/////////////////////
	if (mBaud > 0)
	{
		mSamplePerBaud = sample / mBaud;
	}
	else
	{
		mSamplePerBaud = 8;
	}

	if (mSamplePerBaud < 3)
	{
		mSamplePerBaud = 3;
	}

	////////////////CAN-FD config/////////////////////
	if (mFDBaud > 0)
	{
		mFDSamplePeriod = sample / mFDBaud;
	}
	else
	{
		mFDSamplePeriod = 8;
	}

	if (mFDSamplePeriod < 3)
	{
		mFDSamplePeriod = 3;
	}

}
int CCanCfg::getSamplePerBaud()
{
	return mSamplePerBaud;
}

int CCanCfg::getFDSamplePeriod()
{
	return mFDSamplePeriod;
}
void CCanCfg::setCanSamplePos(int  pos)
{
	mSamplePos = pos;
}

void CCanCfg::setCanFDSamplePos(int  pos)
{
	mFDSamplePos = pos;
}
void CCanCfg::setCanBaud(int baud)
{
	mBaud = baud;
}
void CCanCfg::setFDBaud(int baud)
{
	mFDBaud = baud;
}
void CFrame::setBegin(long pos)
{
	mBegin = pos;
}
long CFrame::getBeigin()
{
	return mBegin;
}
