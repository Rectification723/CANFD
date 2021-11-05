#ifndef _CAN_CFG_
#define _CAN_CFG_

#include <iostream>
#include < vector> 
#include "Bus_Fsm.h"
using namespace std;

/******************************************************************************
* 功能   ： CAN配置结构体
* 输入参数：
* 输出参数：
* 返回值  ：
* 说明   ：
* 日期   ：2021.9.10
* 作者   ：
* ***************************************************************************/
struct CCanCfg
{
	Chan mSrc; 
	int mBaud;//标准can波特率
	int mFDBaud;//CAN-FD波特率
	int mSignalType;//输入数据类型
	int mSamplePerBaud;//CAN每波特采样点数
	int mFDSamplePeriod;//CAN FD每波特采样点数
	int mSamplePos;//CAN 采样位置
	int mFDSamplePos;//CAN-FD 采样位置
	int mSamplePosUser;//程序使用
	int mSamplePerBaudUser;//同

	void  reset();


	int   getSamplePerBaud(void);
	int   getFDSamplePeriod(void);
	void  setSamplePerBaud(unsigned long long  sample); //sample / baud
	void  setCanSamplePos(int  pos);
	void  setCanFDSamplePos(int  pos);
	void  setCanBaud(int baud);
	void  setFDBaud(int baud);

};

struct CFiled
{
	unsigned int mDataFrame;
	int  mLen;
	long  mEndtime;
	long  mBegintime;
};

struct CFrame
{
	long mBegin; 

	CFiled  CID; 

	CFiled  CDLC; 

	vector  <CFiled> CData; 

	CFiled   CCrc;

	CFiled   CAck; 

	void setBegin(long pos);
	long getBeigin();

};


enum EBaudRate
{
	BaudRate_50 = 50,
	BaudRate_75 = 75,
	BaudRate_110 = 110,
	BaudRate_134 = 134,
	BaudRate_150 = 150,
	BaudRate_300 = 300,
	BaudRate_600 = 600,
	BaudRate_1000 = 1000,
	BaudRate_1200 = 1200,
	BaudRate_1800 = 1800,
	BaudRate_2000 = 2000,
	BaudRate_2400 = 2400,
	BaudRate_4800 = 4800,
	BaudRate_9600 = 9600,
	BaudRate_10000 = 10000,
	BaudRate_20000 = 20000,
	BaudRate_19200 = 19200,
	BaudRate_33300 = 33300,
	BaudRate_38400 = 48400,
	BaudRate_50000 = 50000,
	BaudRate_57600 = 57600,
	BaudRate_62500 = 62500,
	BaudRate_83300 = 83300,
	BaudRate_100000 = 100000,
	BaudRate_115200 = 115200,
	BaudRate_125000 = 125000,
	BaudRate_230400 = 230400,
	BaudRate_250000 = 250000,
	BaudRate_460800 = 460800,
	BaudRate_500000 = 500000,
	BaudRate_800000 = 800000,
	BaudRate_921600 = 921600,
	BaudRate_1Mbps = 1000000,
	BaudRate_2Mbps = 2000000,
	BaudRate_3Mbps = 3000000,
	BaudRate_4Mbps = 4000000,
	BaudRate_5Mbps = 5000000,
	BaudRate_6Mbps = 6000000,
	BaudRate_7Mbps = 7000000,
	BaudRate_8Mbps = 8000000,
	BaudRate_9Mbps = 9000000,
	BaudRate_10Mbps = 10000000,
	BaudRate_20Mbps = 20000000,
	BaudRate_Is_User = 1,
	BaudRate_All = 2
};

enum SIGTYPECAN
{
	SIGTYPE_CAN_H = 0,
	SIGTYPE_CAN_L = 1,
	SIGTYPE_RX = 2,
	SIGTYPE_TX = 3,
	SIGTYPE_CSNZ_H_L = 4
};


#endif // !_CAN_CFG_
