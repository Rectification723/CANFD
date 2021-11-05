#ifndef BUS_DECODER_H
#define BUS_DECODER_H






/***********************************************
*!!!!!! Limit the count of the Decoder result!!!!!!
************************************************/
#define uint unsigned int


class CServiceBus;
class CLineView;
class CBlockFilter;

class CDecoder
{
public:
	CDecoder();
	~CDecoder();


	virtual void     rst() = 0;
public:
	static uint mData;
	static uint mLatch;
	static uint mCount;
	static uint mPosition;
	static uint mSpan;



};

#endif // BUS_DECODER_H

