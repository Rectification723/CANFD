#ifndef _BUS_FSM_H_
#define _BUS_FSM_H_

//#include<iostream>
//using namespace std;

enum
{
	FALL,   //! fall
	RISE,   //! edge rise
	SAMP,   //! sampling
	START,
	STOP,
	SPITMO,
	SampleOver,
	LeftChannel,
	RightChannel,
};
enum Chan
{
	chan_none = 0,
	chan1 = 1,
	chan2 = 2,
	chan3 = 3,
	chan4 = 4,
	d0 = 5,
	d1 = 6,
	d2 = 7,
	d3 = 8,
	d4 = 9,
	d5 = 10,
	d6 = 11,
	d7 = 12,
	d8 = 13,
	d9 = 14,
	d10 = 15,
	d11 = 16,
	d12 = 17,
	d13 = 18,
	d14 = 19,
	d15 = 20,
	acline = 21,
	ext = 22,
	ext5 = 23,
	d0d7 = 24,
	d8d15 = 25,
	d0d15 = 26,
	all_trig_src = 26,
	all_pattern_src = 20,
	r1 = 27,
	r2 = 28,
	r3 = 29,
	r4 = 30,
	r5 = 31,
	r6 = 32,
	r7 = 33,
	r8 = 34,
	r9 = 35,
	r10 = 36,
	m1 = 37,
	m2 = 38,
	m3 = 39,
	m4 = 40,
	color_grade = 41,
	fft = 42,
	soft_chan = 64,
	dg1_sync = 65,
	dg2_sync = 66,
	digi_ch1 = 67,
	digi_ch2 = 68,
	digi_ch3 = 69,
	digi_ch4 = 70,
	digi_ch1_l = 71,
	digi_ch2_l = 72,
	digi_ch3_l = 73,
	digi_ch4_l = 74,
	eye_ch1 = 75,
	eye_ch2 = 76,
	eye_ch3 = 77,
	eye_ch4 = 78,
	la = 79,
	reference = 80,
	analog = 81,
	analog_la = 82,
	sch_ch = 83,
	chan_all = 87
};

class CBusState
{
public:
	CBusState();
	virtual ~CBusState();

public:
	virtual void setNextStat(CBusState *pNxt);
	virtual void setIdle(CBusState* idle);
	virtual void setErr(CBusState* err);


	virtual void switchTo(CBusState *pNext, int bitVal);
	virtual void toNext(int bitVal);
	virtual void toPrev(int bitVal);
	virtual void toSelf(int bitVal);
	virtual void toNextNext(int bitVal);

	virtual void toErr(int bitVal);
	virtual void toIdle(int bitVal);

	virtual CBusState* now();

public:
	virtual void onEnter(int bitVal);
	virtual void onExit(int bitVal);
	virtual void serialIn(int cmd, int bitval);


protected:
	static CBusState* mpNow;
private:
	CBusState* mpNext;
	CBusState* mpPrev;
	static CBusState* mpError;
	static CBusState* mpIdle;

};

#endif // 