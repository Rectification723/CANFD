#include "can_input.h"







int main(void)
{
	CTest     Test("RigolDS0.bin");
	cout <<Test.getDataSize()<<endl;

	
	CCanDecoder Candec;


	Candec.decode(Test.getData(),Test.getDataSize() );

	cout << "Betime :"<< Candec.mFrame.mBegin << endl;
	cout << "data :" << Candec.mFrame.CID.mDataFrame << endl;
	cout << "ID  Lenth:" << Candec.mFrame.CID.mLen << endl;
	cout << "Endtime :" << Candec.mFrame.CID.mEndtime << endl;

	cout << "DLC data :" << Candec.mFrame.CDLC.mBegintime << endl;
	cout << "DLC data :" << Candec.mFrame.CDLC.mDataFrame << endl;
	cout << "DLC len :" << Candec.mFrame.CDLC.mLen << endl;
	cout << "DLC EndTime :" << Candec.mFrame.CDLC.mEndtime << endl;

	cout << "All Data Lenth is " << Candec.mFrame.CData.size() << endl;
	for (int i = 0; i < Candec.mFrame.CData.size(); i++)
	{
		cout << "Data is ";
		cout << Candec.mFrame.CData[i].mDataFrame;
		cout << " Len is " << Candec.mFrame.CData[i].mLen << endl;
	}


	system("pause");
	return 0;

}
