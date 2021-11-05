// DecStat.cpp: implementation of the CDecStat class.
//
//////////////////////////////////////////////////////////////////////
#include "Bus_Fsm.h"




CBusState*  CBusState::mpNow;
CBusState*  CBusState::mpError;
CBusState*  CBusState::mpIdle;

CBusState::CBusState()
{
	mpNext = 0;
}

CBusState::~CBusState()
{
}

void CBusState::setNextStat(CBusState *pNxt)
{
	mpNext = pNxt;
	mpNext->mpPrev = this;
}

void CBusState::switchTo(CBusState *pNext, int bitVal)
{
	onExit(bitVal);
	pNext->onEnter(bitVal);
	mpNow = pNext;
}

void CBusState::toPrev(int bitVal)
{
	switchTo(mpPrev, bitVal);
}

void CBusState::toNext(int bitVal)
{
	switchTo(mpNext, bitVal);
}

void CBusState::toNextNext(int bitVal)
{
	switchTo(mpNext->mpNext, bitVal);
}

void CBusState::toSelf(int bitVal)
{
	switchTo(this, bitVal);
}

void CBusState::toErr(int bitVal)
{
	switchTo(mpError, bitVal);
}

void CBusState::toIdle(int bitVal)
{
	switchTo(mpIdle, bitVal);
}

void CBusState::setIdle(CBusState *idle)
{
	mpIdle = idle;
}

void CBusState::setErr(CBusState *err)
{
	mpError = err;
}

CBusState* CBusState::now()
{
	return mpNow;
}



void CBusState::onEnter(int /*bitVal*/)
{}
void CBusState::onExit(int /*bitVal*/)
{}

void CBusState::serialIn(int /*cmd*/, int /*bitval*/)
{}
