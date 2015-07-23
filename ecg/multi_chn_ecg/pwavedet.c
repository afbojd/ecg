/******************************************************************************

    �������� pwavedet()
	�﷨�� int pwavedet( int *pbuf, int *pBegin, int *pEnd, int *pLoc)
	������ �˺���������Ѱ��P������㣬�յ㣬������
	�ο����ģ� ��Ҫ����������ƪ����
		   1��һ�ֻ����������亯����ϵ�P��ʶ���㷨
		   2��Q-onset and T-end delineation: assessment of the performance
		      of an automated method with the use of a reference database
	���ã� ��׼���е�abs������ֵ������
	�����ã� ���������е���
	��������� pbuffer������qrsandonoffset()����
	��������� P����㡢�յ㼰P����ֵ����pbuf�е�����
	����ֵ�� Ŀǰδ����÷���ֵ�������汾����ã��磺����0�������ҵ���P��
	         ��Ч������1�������ҵ���Ч
	������ �˰汾�����P���Ĳ��ң�Ĭ����ΪÿһQRS������һP��
	       ���ҵ��������������յ������
	 
******************************************************************************/
#include "qrsdet.h"
#include <math.h>

int pwavedet( int *pbuf, int *pBegin, int *pEnd, int *pLoc)
{
	int loopone = 0, looptwo = 0, loopthree = 0;//ѭ����������
	int crit = 0;//��׼�������յ㣩
	int maxValue = 0, minValue = 0;//�����Сֵ
	int maxLoc = 0, minLoc = 0;//�����Сֵ����
	int maxLeft = 0, maxRight = 0;//�����������MS10��
	int minLeft = 0, minRight = 0;//��С��������MS10��
	int maxLeftValue = 0, maxRightValue = 0;//�����������MS10��ֵ
	int minLeftValue = 0, minRightValue = 0;//��С��������MS10��ֵ
	int maxValuePlus = 0, minValuePlus = 0;//б�ʺ�

	int looptwoLeft = 0;//Ѱ��beginʱ���һ��
	int conTimeTwo = 0;//����ʱ�䣨��Ŀǰ��Ϊ����MS20ʱ��Ϊ�ҵ����
	int loopthreeRight = 0;//Ѱ��endʱ�ұ�һ��
	int conTimeThree = 0;//����ʱ�䣨��Ŀǰ��Ϊ����MS20ʱ��Ϊ�ҵ��յ�

	//Ѱ�������Сֵ������
	maxValue = minValue = pbuf[0];
	for (loopone = 0; loopone < MS220; ++loopone)
	{
		if (maxValue < pbuf[loopone])
		{
			maxValue = pbuf[loopone];
			maxLoc = loopone;
		}
		if (minValue < pbuf[loopone])
		{
			minValue = pbuf[loopone];
			minLoc = loopone;
		}
	}

	crit = 0.01 * (maxValue - minValue);//����ֵ
	maxLeft = maxLoc - MS10;
	maxRight = maxLoc + MS10;
	minLeft = minLoc - MS10;
	minRight = minLoc + MS10;
	if (maxLeft < 0)
	{
		maxLeft = 0;
	}
	if (maxRight >= MS220)
	{
		maxRight = MS220 - 1;
	}
	if (minLeft < 0)
	{
		minLeft = 0;
	}
	if (minRight >= MS220)
	{
		minRight = MS220 - 1;
	}

	maxLeftValue = abs(pbuf[maxLeft] - pbuf[maxLoc]);
	maxRightValue = abs(pbuf[maxRight] - pbuf[maxLoc]);
	minLeftValue = abs(pbuf[minLeft] - pbuf[minLoc]);
	minRightValue = abs(pbuf[minRight] - pbuf[minLoc]);
	maxValuePlus = maxLeftValue + maxRightValue;
	minValuePlus = minLeftValue + minRightValue;
	if (maxValuePlus > 0.8 * minValuePlus)
	{
		*pLoc = maxLoc;
	}
	else
	{
		*pLoc = minLoc;
	}

	//Ѱ��pbegin
	for (looptwo = *pLoc; looptwo >= 0; --looptwo)
	{
		looptwoLeft = looptwo - 1;
		if (looptwoLeft < 0)
		{
			*pBegin = 0;
			break;
		}
		if (abs(pbuf[looptwo] - pbuf[looptwoLeft]) <= crit)
		{
			conTimeTwo = conTimeTwo + 1;
		}
		else
		{
			conTimeTwo = 0;
		}

		//����ʱ������һ���������������������
		if (conTimeTwo >= MS20)
		{
			*pBegin = looptwo + MS15;//�Ƿ�����Ժ�Ҫ��Ч��
			break;
		}
	}

	//Ѱ��pend
	for (loopthree = *pLoc; loopthree < MS220; ++loopthree)
	{
		loopthreeRight = loopthree + 1;
		if (loopthreeRight >= MS220)
		{
			*pEnd = MS220 - 1;
			break;
		}
		if (abs(pbuf[loopthree] - pbuf[loopthreeRight]) <= crit)
		{
			conTimeThree = conTimeThree + 1;
		}
		else
		{
			conTimeThree = 0;
		}

		//����ʱ������һ���������������������
		if (conTimeThree >= MS20)
		{
			*pEnd = loopthree - MS15;//�Ƿ�����Ժ�Ҫ��Ч��
			break;
		}
	}
return (1);
}
