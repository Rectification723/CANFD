#include "can_input.h"



/*****************************************************************************
* 功能   ： 读取二进制文件并计算数据大小
* 输入参数： 文件的相对路径
* 输出参数： 无
* 返回值  ：
* 说明   ：
* 日期   ：
* 作者   ：
* 修改记录：
* 修改时间：
* 修改原因：
* ***************************************************************************/
CTest::CTest(string name)
{
	Filename = name;
	ifstream inF(Filename, ios::binary);
	////////////////计算SIZE//////////////////////
	long l = inF.tellg();
	inF.seekg(0, ios::end);
	long m = inF.tellg();
	size = (m - l) / 4; // float32 类型数据 
	inF.close();
}
/*****************************************************************************
* 功能   ： 返回二进制文件的有效数据大小 单位字节
* 输入参数： 无
* 输出参数： 无
* 返回值  ：
* 说明   ：
* 日期   ：
* 作者   ：
* 修改记录：
* 修改时间：
* 修改原因：
* ***************************************************************************/
long CTest::getDataSize()
{
	return size - 41; //去除头部
}
/*****************************************************************************
* 函数名 ： historical_filter
* 功能   ： 判断电平高于高的为1小于小的为0 之间的电平保持不变
* 输入参数： 浮点数电平值、高阈值、低阈值
* 输出参数： 无
* 返回值  ：
* 说明   ：
* 日期   ：
* 作者   ：
* 修改记录：
* 修改时间：
* 修改原因：
* ***************************************************************************/
bool CTest::historical_filter(double val, float H_Thre, float L_Thre)
{
	static bool bit; //bit生命周期要长  当电平不在范围内时保持上一个电平
	bit = (val >= H_Thre) ? 1 : bit;
	bit = (val <= L_Thre) ? 0 : bit;

	return bit;
}
/*****************************************************************************
* 函数名 ： getData
* 功能   ： 返回数据的首地址
* 输入参数： 无
* 输出参数： 无
* 返回值  ：
* 说明   ：
* 日期   ：
* 作者   ：
* 修改记录：
* 修改时间：
* 修改原因：
* ***************************************************************************/
bool * CTest::getData()
{
	ifstream inF(Filename, ios::binary);
	inF.open(Filename, ios::binary);
	inF.clear();
	float* data = new float[size];
	bool * mLogic_Vol = new bool[size - 41];
	inF.read(reinterpret_cast<char*>(data), sizeof(float) * (size));
	inF.close();
	for (int i = 41; i < size; i++)  //去除头部41字节
	{
		mLogic_Vol[i - 41] = historical_filter(data[i], 0.21, 0.15);
	}
	return mLogic_Vol;
}



CTest::~CTest()
{


}