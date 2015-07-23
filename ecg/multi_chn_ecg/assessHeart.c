/******************************************************************************

    �������� assessHeart()
	�﷨�� int assessHeart(struct IndexHeart* heartIndex,
	                                          struct AnalysisData* outStruct)
	������ �����ʡ����ɼ�ST�θ���������֣����ɴ��ֵ��������ˮƽ���ж�
	ԭ�� �ο�һЩ�ĵ�֪ʶ���ĵ��Ʒ�ı�׼
	���ã� ��
	�����ã�������
	��������� outStruct�ṹ��
	��������� heartIndex�ṹ�壬���ṹ����嶨�彫���ͷ�ļ�
	����ֵ�� 0��ʾ�������
	������ ��ʱδ���쳣�жϣ����ж�ָ���Ƿ�Ϊ�գ��ռ��С�Ƿ��㹻�ȵ�
	       �������
    ע�⣺

******************************************************************************/
#include <math.h>
#include "assessHeart.h"
#include "run.h"
#include "qrsdet.h"
#include <stdlib.h>

int assessHeart(struct IndexHeart* heartIndex, struct AnalysisData* outStruct)
{
	int qrsnum = 0;
	int liv_ctr = 0;
	int refStnum = 0;
	int realRefStnum = 0;
	int sumAvrSt = 0;
	int refStValue = 0;
	int avrStValue = 0;
	int absStValue = 0;
	int avrStnum = 0;
	int avrHeart = 0;
	double avrRR = 0;
	int sumRR = 0;
	int heartScore = 0;
	int stScore = 0;
	int arrScore = 0;
	int norBeatNum = 0;
	int abnorBeatNum = 0;
	int sumScore = 0;
	int heaLevel = 0;
	int heartJudge = 0;
	int typeJudge = 0;
	int stJudge = 0;
	int detailtype = 0;


	if (outStruct->refStNum > 0)
	{
		refStValue = outStruct->sumRefStVa / outStruct->refStNum;
	}
	else
	{
		refStValue = 0;
	}

	if (outStruct->sumStNum > 0)
	{
		avrStValue = outStruct->sumStValue / outStruct->sumStNum;
	}
	else
	{
		avrStValue = 0;
		refStValue = 0;
	}

	avrStValue = avrStValue - refStValue;//��һ�����ֵ
	//������ADS129x���򾫶�Ϊ5*21����Ϊ���ݿ���Ϊ5����Ϊnerosky��Ϊ5*��3or4��
	avrStValue = avrStValue / 20;//ע��˴������������--��������

	//�ڶ������ֵ
	if (outStruct->qrsnum > 1)
	{
		avrRR = outStruct->sumRR / (outStruct->qrsnum - 1);
		avrRR = avrRR / SAMPLE_RATE;
		avrRR = 60 / avrRR;
		avrHeart = (int)avrRR;
	}
	else
	{
		avrHeart = 0;
	}

    abnorBeatNum = outStruct->abnorBeatNum;//���������ֵ


	//���´��
	//���ʷ�ֵ
	if ((avrHeart <= 100) && (avrHeart >= 60))
	{
		heartScore = 30;
		heartJudge = 1;
	}
	else if ((avrHeart <= 50) || (avrHeart >= 120))
	{
		heartScore = 0;
		if (avrHeart <= 50)
		{
			heartJudge = 4;
		}
		else
		{
			heartJudge = 5;
		}
	}
	else if ((avrHeart > 50) && (avrHeart < 60))
	{
		heartScore = 30 - 3 * (60 - avrHeart);
		heartJudge = 2;
	}
	else if ((avrHeart > 100) && (avrHeart < 120))
	{
		heartScore = 180 - 1.5 * avrHeart;
		heartJudge = 3;
	}

	//ST��ֵ
	absStValue = abs(avrStValue);
	if (absStValue <= 10)
	{
		stScore = 40;
		stJudge = 1;
	}
	else if (absStValue >= 20)
	{
		stScore = 0;
		if (avrStValue >= 20)
		{
			stJudge = 4;
		}
		else
		{
			stJudge = 5;
		}
	}
	else
	{
		stScore = 80 - 4 * absStValue;
		if (avrStValue > 10)
		{
			stJudge = 2;
		}
		else
		{
			stJudge = 3;
		}
	}
	//���ɲ�����
	//ǰ5������Ĭ��ΪN����Ҫ�ӵ�������ʼ����


	if (abnorBeatNum <= 0)
	{
		arrScore = 30;
		typeJudge = 1;
	}
	else if (abnorBeatNum >= 6)
	{
		arrScore = 0;
		typeJudge = 4;
	}
	else
	{
		arrScore = 30 - 5 * abnorBeatNum;
		if (abnorBeatNum >= 3)
		{
			typeJudge = 3;
		}
		else
		{
			typeJudge = 2;
		}
	}

	detailtype = outStruct->rhyme;

	/*************************************************************************/
	//2013��7��2����ӣ�Ŀ���Ǵ�������Ϊ0����ʱ����δ�ҵ�QRS��ʱ���ֵ����
	if (avrHeart <= 30 || outStruct->asystoleflag)
	{
		heartScore = 0;
		heartJudge = 6;
		stScore = 0;
		stJudge = 6;
		arrScore = 0;
		typeJudge = 6;
		detailtype = 6;
	}

	if (detailtype == 10)
	{
		arrScore = 20;
	}


	/*************************************************************************/

	sumScore = heartScore + stScore + arrScore;

	if (sumScore >= 90)
	{
		heaLevel = 5;
	}
	else if (sumScore <= 70)
	{
		heaLevel = 3;
	}
	else
	{
		heaLevel = 4;
	}

	/*************************************************************************/
	//2013��7��2����ӣ�Ŀ���Ǵ�������Ϊ0����ʱ����δ�ҵ�QRS��ʱ���ֵ����
	if (avrHeart <= 30 || outStruct->asystoleflag)
	{
		heaLevel = 6;
	}
	/*************************************************************************/

	heartIndex->score = sumScore;
	heartIndex->avrHeart = avrHeart;
	heartIndex->avrStValue = avrStValue;
	heartIndex->healthLev = heaLevel;
	heartIndex->heartJudge = heartJudge;
	heartIndex->typeJudge = typeJudge;
	heartIndex->stJudge = stJudge;
	heartIndex->detailtype = detailtype;

	return(0);
}
