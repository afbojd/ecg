/******************************************************************************

    �������� statfun()
	�﷨�� int statfun( ChannelAnalysisStruct* ChanAnaStr, int lastqrsLOc )
	������ ͳ�ƺ�����ͳ����run��֮ǰ���õĺ����õ����ĵ����������Դ��ݸ�
	       assessHeart����ʹ�ã���֮ǰ�汾��run�ж������뷢չ����������run
		   �����Ĵ�����
	ԭ�� 
	���ã���׼���е�һЩ����
	�����ã�int run(TotalAnalysisStruct* TotalStruct, 
	                int* proArray, int lenArray)
	��������� ��һQRS���ϲ�������
	��������� �ṹoutStruct�����Ͻṹ���嶨���run.h
	����ֵ����������δ��ȷ����
	������ �Ժ󻹿ɶԷ���ֵ���и���ϸ�Ķ���
    ע�⣺ 
	 
******************************************************************************/
#include <string.h>
#include "qrsdet.h"
#include "mult_run.h"

int statfun( ChannelAnalysisStruct* ChanAnaStr, int lastqrsLOc )
{
	int lastHRate = 0;
	real rrInterval = 0;
	real rrInter = 0;
	real heartRate = 0;
	int HRate = 0;
	const int memmovelen = (HrArLen - 1) * sizeof(int);
	int zeroFlag = 0, flagCtr = 0;
	int sumHRate = 0, plusTimes = 0;
	
	//���������
	if (lastqrsLOc && ChanAnaStr->qrsLoc)
	{
		rrInterval = (real)(ChanAnaStr->qrsLoc - lastqrsLOc);
		rrInter = rrInterval / SAMPLE_RATE;
		heartRate = 60 / rrInter;
		HRate = (int)heartRate;
		if (ChanAnaStr->lastHRate == 0)
		{
			ChanAnaStr->lastHRate = HRate;
		} 
		else if (((0.7 * ChanAnaStr->heartRate <= HRate) 
			&& (HRate <= 1.4 * ChanAnaStr->heartRate)) 
			|| (ChanAnaStr->heartRate == 0) || (HRate <= 201 && HRate >= 40))
		{
			ChanAnaStr->lastHRate = HRate;
		}
		memmove(ChanAnaStr->HrArray, 
			&(ChanAnaStr->HrArray[1]), memmovelen);
		ChanAnaStr->HrArray[HrArLen - 1] = ChanAnaStr->lastHRate;
		zeroFlag = 0;//����ʼֵ
		for (flagCtr = 0; flagCtr < HrArLen; ++flagCtr)
		{
			if (ChanAnaStr->HrArray[flagCtr] == 0)
			{
				zeroFlag = zeroFlag + 1;
			}
		}
		if (zeroFlag >= 5)
		{
			ChanAnaStr->heartRate = 0;
		} 
		else
		{
			sumHRate = 0;//����ʼֵ
			plusTimes = 0;//����ʼֵ
			for (; zeroFlag < 5; ++zeroFlag)
			{
				++plusTimes;
				sumHRate = sumHRate + ChanAnaStr->HrArray[zeroFlag];
			}
			ChanAnaStr->heartRate = sumHRate / plusTimes;
		}
	}
	else
	{
		ChanAnaStr->heartRate = 0;
	}
	

	return 0;
}