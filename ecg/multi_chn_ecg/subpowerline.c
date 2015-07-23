/******************************************************************************

    �������� subpowerline()
	�﷨�� int subpowerline(FilterDataStruct *FilterDate, int datum)
	������ ȥ��Ƶ��������
	�ο����ģ� ��Ҫ����������ƪ���ģ����Ļ���һ��
	           1��Subtraction of 50 Hz interference from the
	              electrocardiogram
	           2��New approach to the digital elimination of
	              50 Hz interference from the electrocardiogram
	���ã� ��׼���е�abs������ֵ������
	�����ã����������ϲ㺯��
	����������˲����ṹ������ʹ��˲�����ֵ
	��������� ��
	����ֵ�� ���˲��ź�
	������ ����һ�ַ������˲����������⼸�ֲ�ͬ�ĵط�ֻ����subpowerlineOrder.h
	       ͷ�ļ�������ϵ��ͷ�ļ���������.c�ļ��л����ý���ͷ�ļ���
		   �ڽ���ͷ�ļ��ж���ĺ�������˲����࣬��Щ
		   ����Ҫ��Ե���50Hz��Ƶ���ţ�Ŀǰ����ȥ��60Hz��Ƶ����
    ע�⣺

******************************************************************************/
#include <math.h>
#include <string.h>
#include "qrsdet.h"
#include "filter.h"


#include "subpowerlineorder_250.h"


int subpowerline(FilterDataStruct *FilterDate, int datum)
{
	int intfdatum = 0;//���ֵ
	real fdatum = 0;//���ֵ
	real subPrecision = 0;//Ϊ��߾��ȵ��м�ֵ
	int realPtr = 0;
	real sumOfdata = 0;
	int sumLoop = 0, sumPtr = 0;
	real alterError = 0;
	int	alterPtr = 0;
	int crit = 0;
	int diffOne = 0, diffTwo = 0, criterion = 0;
	int diffOnePtr = 0, diffTwoPtr = 0, ptrMinusOne = 0;

	crit = (int)(150 * FilterDate->subPL.crit);
	FilterDate->subPL.dataIn[FilterDate->subPL.ptr] = datum;

	if (FilterDate->subPL.jVar == numOfCircle)
	{
		FilterDate->subPL.jVar = 0;
	}

	diffOnePtr = FilterDate->subPL.ptr - numOfCircle;
	diffTwoPtr = FilterDate->subPL.ptr - numOfCircle - 1;
	ptrMinusOne = FilterDate->subPL.ptr - 1;
	if (diffOnePtr < 0)
	{
		diffOnePtr = diffOnePtr + datainlen;
	}
	if (diffTwoPtr < 0)
	{
		diffTwoPtr = diffTwoPtr + datainlen;
	}
	if (ptrMinusOne < 0)
	{
		ptrMinusOne = ptrMinusOne + datainlen;
	}

	diffOne = FilterDate->subPL.dataIn[diffOnePtr] -
		      FilterDate->subPL.dataIn[FilterDate->subPL.ptr];
	diffTwo = FilterDate->subPL.dataIn[diffTwoPtr] -
		      FilterDate->subPL.dataIn[ptrMinusOne];
	criterion = abs(diffOne - diffTwo);

	if (criterion < crit)
	{
		FilterDate->subPL.ctrVar = FilterDate->subPL.ctrVar - 1;
		if (FilterDate->subPL.ctrVar == 0)
		{
			FilterDate->subPL.ctrVar = 1;
			for (sumLoop = 1; sumLoop <= numOfCircle; ++sumLoop)
			{
				sumPtr = FilterDate->subPL.ptr - sumLoop;
				if (sumPtr < 0)
				{
					sumPtr = sumPtr + datainlen;
				}
				sumOfdata = sumOfdata + FilterDate->subPL.dataIn[sumPtr];
			}

			alterPtr = FilterDate->subPL.ptr - numOfCircle;
			if (alterPtr < 0)
			{
				alterPtr = alterPtr + datainlen;
			}
			alterError = (FilterDate->subPL.dataIn[FilterDate->subPL.ptr] -
				          FilterDate->subPL.dataIn[alterPtr]) / 2;
			sumOfdata = sumOfdata - alterError;
			fdatum = sumOfdata / numOfCircle;

			realPtr = FilterDate->subPL.ptr - realptrShift;
			if (realPtr < 0)
			{
				realPtr = realPtr + datainlen;
			}
			//�Ƿ�Ӧ��������������Ǹ�һ��
			FilterDate->subPL.interference[FilterDate->subPL.jVar] =
				        FilterDate->subPL.dataIn[realPtr] - fdatum;
		}
		else
		{
			realPtr = FilterDate->subPL.ptr - realptrShift;
			if (realPtr < 0)
			{
				realPtr = realPtr + datainlen;
			}
			fdatum = FilterDate->subPL.dataIn[realPtr] -
				     FilterDate->subPL.interference[FilterDate->subPL.jVar];
		}
	}
	else
	{
		FilterDate->subPL.ctrVar = numOfCircle;
		realPtr = FilterDate->subPL.ptr - realptrShift;
		if (realPtr < 0)
		{
			realPtr = realPtr + datainlen;
		}
		fdatum = FilterDate->subPL.dataIn[realPtr] -
			     FilterDate->subPL.interference[FilterDate->subPL.jVar];
	}

	FilterDate->subPL.jVar = FilterDate->subPL.jVar + 1;
	++FilterDate->subPL.ptr;
	if (FilterDate->subPL.ptr == datainlen)
	{
		FilterDate->subPL.ptr = 0;
	}

	intfdatum = (int)fdatum;
	subPrecision = fdatum - intfdatum;

	if (subPrecision >= 0.5000)
	{
		intfdatum = intfdatum + 1;
	}
	if (subPrecision < -0.5000)
	{
		intfdatum = intfdatum - 1;
	}
	return(intfdatum);
}
