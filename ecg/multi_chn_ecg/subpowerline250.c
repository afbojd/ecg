/******************************************************************************

    �������� subpowerline250()
	�﷨�� int subpowerline250(int datum, int init)
	������ ȥ��Ƶ��������
	�ο����ģ� ��Ҫ����������ƪ���ģ����Ļ���һ��
	           1��Subtraction of 50 Hz interference from the
	              electrocardiogram
	           2��New approach to the digital elimination of
	              50 Hz interference from the electrocardiogram
	���ã� ��׼���е�abs������ֵ������
	�����ã����������ϲ㺯��
	������������˲��źż���ʼ����־��
	��������� ��
	����ֵ�� ���˲��ź�
	������ �ҵ���һֱ��matlab�汾��һ�µ�ԭ��matlab������м����Ǹ����ͣ�
	       ���ڴξ�Ϊint�ͣ�����ѡ�������ͻ�Ҫ���ǣ�����������
		   ʵ��֤�ɶ���������ȵĹ�Ƶ���ţ�������ˣ����źŽ����˲�����
		   ��Ч������
		   �ӳ�5��

		   ����ĺ������Ϊ���͵ĺ���������ĺ������Ϊdouble�͵ĺ���

    ע�⣺ ����ע������
	       1���˺�������matlab�汾һ�������Դ�����������ʺ����ֹ�Ƶ����
		   ��50��60�����źţ����ﴦ�������Ϊ500Hz����Ƶ����Ϊ50Hz���ź�
		   ΪADS1298-FE�ɼ����ź�
		   2��δ����ǰ���ͻ��
	 
******************************************************************************/
#include <math.h>
/*#define ABS(x) (x>=0?x:-x)*/

int subpowerline250(int datum, int init)
{
	int intfdatum = 0;//���ֵ
	double fdatum = 0;//���ֵ
	double subPrecision = 0;//Ϊ��߾��ȵ��м�ֵ
	static int data[7], ptr;
	static double interference[5];//���������
	static int jVar, ctrVar;//�����������������������
	int realPtr = 0;
	int numOfCircle = 5;//һ�����ڲ�������
	int halfNumOfCir = 2;
	int crit = 3000;//����ֵ��δ���嶨��
	double sumOfdata = 0;
	int sumLoop = 0, sumPtr = 0;
	double alterError = 0;
	int	alterPtr = 0;
	int diffOne = 0, diffTwo = 0, criterion = 0;
	int diffOnePtr = 0, diffTwoPtr = 0, ptrMinusOne = 0;

	if (init)
	{
		for (ptr = 0; ptr < 7; ++ptr)
		{
			data[ptr] = 0;
		}
		for (ptr = 0; ptr < 5; ++ptr)
		{
			interference[ptr] = 0;
		}
		ptr = 0;
		jVar = 0;
		ctrVar = 1;
		return 0;
	}

	data[ptr] = datum;
	if (jVar == numOfCircle)
	{
		jVar = 0;
	}

	diffOnePtr = ptr - numOfCircle;
	diffTwoPtr = ptr - numOfCircle - 1;
	ptrMinusOne = ptr - 1;
	if (diffOnePtr < 0)
	{
		diffOnePtr = diffOnePtr + 7;
	}
	if (diffTwoPtr < 0)
	{
		diffTwoPtr = diffTwoPtr + 7;
	}
	if (ptrMinusOne < 0)
	{
		ptrMinusOne = ptrMinusOne + 7;
	}

	diffOne = data[diffOnePtr] - data[ptr];
	diffTwo = data[diffTwoPtr] - data[ptrMinusOne];

	criterion = abs(diffOne - diffTwo);
	if (criterion < crit)
	{
		ctrVar = ctrVar - 1;
		if (ctrVar == 0)
		{
			ctrVar = 1;
			for (sumLoop = 1; sumLoop <= numOfCircle; ++sumLoop)
			{
				sumPtr = ptr - sumLoop;
				if (sumPtr < 0)
				{
					sumPtr = sumPtr + 7;
				}
				sumOfdata = sumOfdata + data[sumPtr];
			}

			alterPtr = ptr - numOfCircle;
			if (alterPtr < 0)
			{
				alterPtr = alterPtr + 7;
			}
			alterError = (data[ptr] - data[alterPtr]) / 2;
			sumOfdata = sumOfdata - alterError;
			fdatum = sumOfdata / numOfCircle;

			realPtr = ptr - 2;
			if (realPtr < 0)
			{
				realPtr = realPtr + 7;
			}
			//�Ƿ�Ӧ��������������Ǹ�һ��
			interference[jVar] = data[realPtr] - fdatum;
		} 
		else
		{
			realPtr = ptr - 2;
			if (realPtr < 0)
			{
				realPtr = realPtr + 7;
			}
			fdatum = data[realPtr] - interference[jVar];
		}
	} 
	else
	{
		ctrVar = numOfCircle;
		realPtr = ptr - 2;
		if (realPtr < 0)
		{
			realPtr = realPtr + 7;
		}
		fdatum = data[realPtr] - interference[jVar];
	}
	jVar = jVar + 1;
	++ptr;
	if (ptr == 7)
	{
		ptr = 0;
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


//#include <math.h>
//
//double subpowerline250(int datum, int init)
//{
//	int intfdatum = 0;//���ֵ
//	double fdatum = 0;//���ֵ
//	double subPrecision = 0;//Ϊ��߾��ȵ��м�ֵ
//	static int data[7], ptr;
//	static double interference[5];//���������
//	static int jVar, ctrVar;//�����������������������
//	int realPtr = 0;
//	int numOfCircle = 5;//һ�����ڲ�������
//	int halfNumOfCir = 2;
//	int crit = 3000;//����ֵ��δ���嶨��
//	double sumOfdata = 0;
//	int sumLoop = 0, sumPtr = 0;
//	double alterError = 0;
//	int	alterPtr = 0;
//	int diffOne = 0, diffTwo = 0, criterion = 0;
//	int diffOnePtr = 0, diffTwoPtr = 0, ptrMinusOne = 0;
//
//	if (init)
//	{
//		for (ptr = 0; ptr < 7; ++ptr)
//		{
//			data[ptr] = 0;
//		}
//		for (ptr = 0; ptr < 5; ++ptr)
//		{
//			interference[ptr] = 0;
//		}
//		ptr = 0;
//		jVar = 0;
//		ctrVar = 1;
//		return 0;
//	}
//
//	data[ptr] = datum;
//	if (jVar == numOfCircle)
//	{
//		jVar = 0;
//	}
//
//	diffOnePtr = ptr - numOfCircle;
//	diffTwoPtr = ptr - numOfCircle - 1;
//	ptrMinusOne = ptr - 1;
//	if (diffOnePtr < 0)
//	{
//		diffOnePtr = diffOnePtr + 7;
//	}
//	if (diffTwoPtr < 0)
//	{
//		diffTwoPtr = diffTwoPtr + 7;
//	}
//	if (ptrMinusOne < 0)
//	{
//		ptrMinusOne = ptrMinusOne + 7;
//	}
//
//	diffOne = data[diffOnePtr] - data[ptr];
//	diffTwo = data[diffTwoPtr] - data[ptrMinusOne];
//
//	criterion = abs(diffOne - diffTwo);
//	if (criterion < crit)
//	{
//		ctrVar = ctrVar - 1;
//		if (ctrVar == 0)
//		{
//			ctrVar = 1;
//			for (sumLoop = 1; sumLoop <= numOfCircle; ++sumLoop)
//			{
//				sumPtr = ptr - sumLoop;
//				if (sumPtr < 0)
//				{
//					sumPtr = sumPtr + 7;
//				}
//				sumOfdata = sumOfdata + data[sumPtr];
//			}
//
//			alterPtr = ptr - numOfCircle;
//			if (alterPtr < 0)
//			{
//				alterPtr = alterPtr + 7;
//			}
//			alterError = (data[ptr] - data[alterPtr]) / 2;
//			sumOfdata = sumOfdata - alterError;
//			fdatum = sumOfdata / numOfCircle;
//
//			realPtr = ptr - 2;
//			if (realPtr < 0)
//			{
//				realPtr = realPtr + 7;
//			}
//			//�Ƿ�Ӧ��������������Ǹ�һ��
//			interference[jVar] = data[realPtr] - fdatum;
//		} 
//		else
//		{
//			realPtr = ptr - 2;
//			if (realPtr < 0)
//			{
//				realPtr = realPtr + 7;
//			}
//			fdatum = data[realPtr] - interference[jVar];
//		}
//	} 
//	else
//	{
//		ctrVar = numOfCircle;
//		realPtr = ptr - 2;
//		if (realPtr < 0)
//		{
//			realPtr = realPtr + 7;
//		}
//		fdatum = data[realPtr] - interference[jVar];
//	}
//	jVar = jVar + 1;
//	++ptr;
//	if (ptr == 7)
//	{
//		ptr = 0;
//	}
//	intfdatum = (int)fdatum;
//	subPrecision = fdatum - intfdatum;
//	if (subPrecision >= 0.5000)
//	{
//		intfdatum = intfdatum + 1;
//	}
//	if (subPrecision < -0.5000)
//	{
//		intfdatum = intfdatum - 1;
//	}
//	return(fdatum);
//}