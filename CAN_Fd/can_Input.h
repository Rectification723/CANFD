#ifndef _CAN_INPPUT_H
#define _CAN_INPPUT_H

#include<iostream>
#include "Bus_Fsm.h"
#include "can_fsm.h"
#include <fstream>
#include <sstream>
#include<iomanip>
#include <vector>
#include <string>
#include <windows.h>
#include <stdlib.h>
#include "can_cfg.h"

using namespace std;

class CTest
{
public:
	CTest(string name);

	long getDataSize();
	bool historical_filter(double val, float H_Thre, float L_Thre);
	bool * CTest::getData();

	~CTest();



public:
	string Filename;
	long size;
private:

};


#endif

