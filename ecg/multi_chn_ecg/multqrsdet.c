/******************************************************************************
    �ļ���: qrsdet.c
	������ ���ļ��а�������Ϊ���QRS���ļ�⺯�����Լ���⺯��������Ӻ���
	qrsdet()�����ᱻ�����������ã��������������ڲ�����,�����������ⲿ����
	�����б� 
	    1��qrsdet()     ���ļ�����������������������������⵽һ��QRS���ϲ�
		                �������ӳ�
	    2��multpeak()       ����ֵ����
	    3��multmedian()     ��ֵ����
	    4��multthresh()     ��ֵ����
	    5��multblscheck()   ����Ư�Ƽ�⺯��
	    6��qrsfilter()  �˲��������ⲿ����
	    7��deriv1()     ��ֺ������ⲿ����
	�����ã� 
	��������� 
	��������� 
	����ֵ�� 
	������ multmedian����һ����Ϊ��ҪѰ�ҵ���ֵ���鳤�ȣ���Ϊ������Ĳ����㷨
	       ����һ��ռ�
	ע�⣺2013_1_29�շ��֣������޾ܾ��߼�T��������
******************************************************************************/
#include <string.h>		// memmove�������ڿ��ļ�
#include <math.h>
#include "qrsdet.h"
#include "fundecla.h"


// �ڲ���������
int multpeak( qrsDetStruct* qrsDetStr ,int datum );
int multmedian(int *array, int datnum);
int multthresh(int qmedian, int nmedian);
int multblscheck(int *dBuf,int dbPtr);    //baseline shift check����ƫ�Ƽ��

real multTH = 0.4f; //��ֵϵ��

int multddBuffer[DER_DELAY] = {0}, multddPtr = 0;	//���ԭʼ�źŲ�ֵ�Ļ���
int multDly = 0;

const int multmemmovelen = (arrayLen - 1) * sizeof(int);

/******************************************************************************

    �������� qrsdet()
	�﷨�� int qrsdet( qrsDetStruct* qrsDetStr, int datum )
	������ �˲��ܺ��������ø��˲������õ������˲�������ź�ֵ
	���ã� 
	       1��qrsfilter()   �˲�����
	       2��deriv1()      ��ֵ����
	       3��multpeak()        ��ֵ��⺯��
	       4��multmedian()      ��ֵ����
	       5��multthresh()      ��ֵ����
	       6��multblscheck()    ����Ư�Ƽ�⺯��
	�����ã��ϲ㺯������������
	��������� ԭʼ�ź������Ƿ��ʼ������
	��������� ����⵽һ��QRS��������������ʱ��δ��⵽�������0ֵ
	����ֵ�� ����⵽һ��QRS��������������ʱ��δ��⵽�������0ֵ
	������ 
	 
******************************************************************************/
int multqrsdet( qrsDetStruct* qrsDetStr, int datum )
{
	int fdatum, qrsDelay = 0;
	int i, newPeak, aPeak;

	fdatum = multqrsfilter(&qrsDetStr->qrsFilterStr, datum);	//��ԭʼ�����˲�
	aPeak = multpeak(qrsDetStr, fdatum);        //��ֵ���

	// ��κ��������������������Ĳ�Ӧ�ڣ���MS200Ϊ��Ӧ��
	newPeak = 0;
	if (aPeak && !qrsDetStr->preBlankCnt)// ��200ms��Ӧ���ѵ��Ҳ���һ��multpeakֵ
	{								     // ��洢��multpeakֵ�����ò�Ӧ�ڼ�������
		qrsDetStr->tempPeak = aPeak;
		qrsDetStr->preBlankCnt = PRE_BLANK;			
	}
	else if(!aPeak && qrsDetStr->preBlankCnt)//����Ӧ���ѹ�������multpeak����ʱ
	{								         //���Ÿ�ֵnewpeak����Ϊ�п��ܲ���
		if(--qrsDetStr->preBlankCnt == 0)    //QRS����Ҳ��ΪʲôFILTER_DELAY��
		{                                    //������PRE_BLANK��Ե��
			newPeak = qrsDetStr->tempPeak;
		}
	}
	else if (aPeak)					   // ����ڲ�Ӧ���������²������µ�multpeak
	{								   
		if (aPeak > qrsDetStr->tempPeak)    // ����multpeakֵ����ǰ���multpeakֵ
		{                                   // ���ظ�һ������ĵĲ���
			qrsDetStr->tempPeak = aPeak;
			qrsDetStr->preBlankCnt = PRE_BLANK;        
		}
		else if (--qrsDetStr->preBlankCnt == 0)
		{
			newPeak = qrsDetStr->tempPeak;
		}
	}

	//�洢ԭʼ�źŵ�б�ʣ���ŵ����鼴���߼����������
	multddBuffer[multddPtr] = multderiv1( &qrsDetStr->qrsFilterStr, datum );
	if (++multddPtr == DER_DELAY)
	{
		multddPtr = 0;
	}

	//ǰ8����ʵ��Ϊ8s����Ϊ����MS1000ʱqpkcnt��1��
	if ( qrsDetStr->qpkcnt < 8 )
	{
		++qrsDetStr->countnum;

		if(newPeak > 0) 
		{
			qrsDetStr->countnum = WINDOW_WIDTH;
		}

		if(++qrsDetStr->initBlank == MS1000)
		{
			qrsDetStr->initBlank = 0;
			qrsDetStr->qrsbuf[qrsDetStr->qpkcnt] = qrsDetStr->initMax;
			qrsDetStr->initMax = 0;
			++qrsDetStr->qpkcnt;

			if(qrsDetStr->qpkcnt == 8)
			{
				qrsDetStr->qmedian = multmedian(qrsDetStr->qrsbuf, 8);
				qrsDetStr->nmedian = 0;
				qrsDetStr->rrmedian = MS1000;
				qrsDetStr->sbcount = MS1500 + MS150;
				qrsDetStr->det_thresh = 
					       multthresh(qrsDetStr->qmedian, qrsDetStr->nmedian);
			}
		}

		if ( newPeak > qrsDetStr->initMax )
		{
			qrsDetStr->initMax = newPeak;
		}
	}

	else	// ��ʱ�ſ�ʼ���QRS��
	{
		++qrsDetStr->countnum;
		if (newPeak > 0)
		{
			// ���multpeak�Ƿ����ڼ���ƫ�����𣬲�������Ϊ�п��ܲ���QRS��
			if (!multblscheck(multddBuffer, multddPtr))
			{

				// ��multpeakֵ����̽����ֵ������Ϊ�п��ܲ���QRS��
				if (newPeak > qrsDetStr->det_thresh)
				{
					memmove(&qrsDetStr->qrsbuf[1], 
						                qrsDetStr->qrsbuf, multmemmovelen);
					qrsDetStr->qrsbuf[0] = newPeak;
					qrsDetStr->qmedian = multmedian(qrsDetStr->qrsbuf, 8);
					qrsDetStr->det_thresh = 
						       multthresh(qrsDetStr->qmedian, qrsDetStr->nmedian);
					memmove(&qrsDetStr->rrbuf[1], 
						                qrsDetStr->rrbuf, multmemmovelen);
					qrsDetStr->rrbuf[0] = qrsDetStr->countnum - WINDOW_WIDTH;
					qrsDetStr->rrmedian = multmedian(qrsDetStr->rrbuf, 8);
					qrsDetStr->sbcount = qrsDetStr->rrmedian + 
						             (qrsDetStr->rrmedian >> 1) + WINDOW_WIDTH;
					qrsDetStr->countnum = WINDOW_WIDTH;
					qrsDetStr->sbpeak = 0;
					qrsDelay = WINDOW_WIDTH + FILTER_DELAY;
					qrsDetStr->initBlank = 
						       qrsDetStr->initMax = qrsDetStr->rsetCount = 0;
				}

				// ��Ϊ�����������multpeak�����п�������֮ǰ
				// ��ƥ��QRS������С������
				else
				{
					memmove(&qrsDetStr->noise[1], 
						     qrsDetStr->noise, multmemmovelen);
					qrsDetStr->noise[0] = newPeak;
					qrsDetStr->nmedian = multmedian(qrsDetStr->noise, 8);
					qrsDetStr->det_thresh = 
						       multthresh(qrsDetStr->qmedian, qrsDetStr->nmedian);

					// Don't include early peaks (which might be T-waves)
					// in the search back process.  A T-wave can mask
					// a small following QRS.

					if ((newPeak > qrsDetStr->sbpeak) && 
						((qrsDetStr->countnum - WINDOW_WIDTH) >= MS360))
					{
						qrsDetStr->sbpeak = newPeak;
						qrsDetStr->sbloc = qrsDetStr->countnum - WINDOW_WIDTH;
					}
				}

				//���о�ȥ��
				/*if ((qrsDetStr->countnum > qrsDetStr->sbcount) && 
					(qrsDetStr->sbpeak > (qrsDetStr->det_thresh >> 1)))
				{
				memmove(&qrsDetStr->qrsbuf[1], qrsDetStr->qrsbuf, multmemmovelen);
				qrsDetStr->qrsbuf[0] = qrsDetStr->sbpeak;
				qrsDetStr->qmedian = multmedian(qrsDetStr->qrsbuf, 8);
				qrsDetStr->det_thresh = 
					       multthresh(qrsDetStr->qmedian, qrsDetStr->nmedian);
				memmove(&qrsDetStr->rrbuf[1], qrsDetStr->rrbuf, multmemmovelen);
				qrsDetStr->rrbuf[0] = qrsDetStr->sbloc;
				qrsDetStr->rrmedian = multmedian(qrsDetStr->rrbuf, 8);
				qrsDetStr->sbcount = qrsDetStr->rrmedian + 
					                 (qrsDetStr->rrmedian >> 1) + WINDOW_WIDTH;
				qrsDelay = qrsDetStr->countnum = 
					       qrsDetStr->countnum - qrsDetStr->sbloc;
				qrsDelay += FILTER_DELAY;
				qrsDetStr->sbpeak = 0;
				qrsDetStr->initBlank = 
					       qrsDetStr->initMax = qrsDetStr->rsetCount = 0;
				}*/
			}
		}

		//���ݻ���
		if ((qrsDetStr->countnum > qrsDetStr->sbcount) && 
			(qrsDetStr->sbpeak > (qrsDetStr->det_thresh >> 1)))
		{
			memmove(&qrsDetStr->qrsbuf[1], qrsDetStr->qrsbuf, multmemmovelen);
			qrsDetStr->qrsbuf[0] = qrsDetStr->sbpeak;
			qrsDetStr->qmedian = multmedian(qrsDetStr->qrsbuf, 8);
			qrsDetStr->det_thresh = 
				       multthresh(qrsDetStr->qmedian, qrsDetStr->nmedian);
			memmove(&qrsDetStr->rrbuf[1], qrsDetStr->rrbuf, multmemmovelen);
			qrsDetStr->rrbuf[0] = qrsDetStr->sbloc;
			qrsDetStr->rrmedian = multmedian(qrsDetStr->rrbuf, 8);

			// sbcount = 1.5 * RR����
			qrsDetStr->sbcount = qrsDetStr->rrmedian + 
				                 (qrsDetStr->rrmedian >> 1) + WINDOW_WIDTH;
			qrsDelay = qrsDetStr->countnum = 
				       qrsDetStr->countnum - qrsDetStr->sbloc;
			qrsDelay += FILTER_DELAY;
			qrsDetStr->sbpeak = 0;
			qrsDetStr->initBlank = 
				       qrsDetStr->initMax = qrsDetStr->rsetCount = 0;
		}
	}

	//����8s��δ��⵽QRS�������³�ʼ��һЩֵ��qrsbuf��ֵΪÿ1s�����ֵ
/*
	if ( qrsDetStr->qpkcnt == 8 )
	{
		if (++qrsDetStr->initBlank == MS1000)
		{
			qrsDetStr->initBlank = 0;
			qrsDetStr->rsetBuff[qrsDetStr->rsetCount] = qrsDetStr->initMax;
			qrsDetStr->initMax = 0;
			++qrsDetStr->rsetCount;

			//����8s����
			if (qrsDetStr->rsetCount == 8)
			{
				for (i = 0; i < 8; ++i)
				{
					qrsDetStr->qrsbuf[i] = qrsDetStr->rsetBuff[i];
					qrsDetStr->noise[i] = 0;
				}
				qrsDetStr->qmedian = multmedian( qrsDetStr->rsetBuff, 8 );
				qrsDetStr->nmedian = 0;
				qrsDetStr->rrmedian = MS1000;
				qrsDetStr->sbcount = MS1500 + MS150;
				qrsDetStr->det_thresh = 
					       multthresh(qrsDetStr->qmedian, qrsDetStr->nmedian);
				qrsDetStr->initBlank = 
					       qrsDetStr->initMax = qrsDetStr->rsetCount = 0;
                qrsDetStr->sbpeak = 0;
			}
		}

		if ( newPeak > qrsDetStr->initMax )
		{
			qrsDetStr->initMax = newPeak;
		}
	}*/

	return(qrsDelay);
}


/******************************************************************************

    �������� multpeak()
	�﷨: int multpeak( qrsDetStruct* qrsDetStr, int datum )
	����: ��ֵ��⺯��������Ԥ�������źţ����ź����ϸ��źţ�
	      �Լ����ֵ�Ƚϣ����������������ֵ�����浱ǰ�ź�ֵ��
	      ����0���������������ֵ��һ�룬�������ֵ��multDly Ϊ0��
	      �����ж���ǰ�ź�λ�õ����ֵ���룬����95ms��
	      �������ֵ��multDly = 3������֮�⣬���� 0��
	���ã� ��
	�����ã�  qrsdet()
	��������� Ԥ�������źż��Ƿ��ʼ������
	��������� ��ֵ
	����ֵ�� ��ֵ��multDly
	������ multDlyδ��������

******************************************************************************/
int multpeak( qrsDetStruct* qrsDetStr, int datum )
{
	int pk = 0;	
	if (qrsDetStr->qrsPeakStr.timeSinceMax > 0)
	{
		++qrsDetStr->qrsPeakStr.timeSinceMax;
	}
	if ((datum > qrsDetStr->qrsPeakStr.lastDatum) && 
		                              (datum > qrsDetStr->qrsPeakStr.maxvalue))
	{
		qrsDetStr->qrsPeakStr.maxvalue = datum;

		//���������������Ĵ�����ʱ�Ÿ�ֵtimeSinceMax
		if (qrsDetStr->qrsPeakStr.maxvalue > 2)  
		{
			qrsDetStr->qrsPeakStr.timeSinceMax = 1;
		}
	}
	else if (datum < (qrsDetStr->qrsPeakStr.maxvalue >> 1))
	{
		pk = qrsDetStr->qrsPeakStr.maxvalue;
		qrsDetStr->qrsPeakStr.maxvalue = 0;
		qrsDetStr->qrsPeakStr.timeSinceMax = 0;
		multDly = 0;
	}
	else if (qrsDetStr->qrsPeakStr.timeSinceMax > MS95)
	{
		pk = qrsDetStr->qrsPeakStr.maxvalue;
		qrsDetStr->qrsPeakStr.maxvalue = 0;
		qrsDetStr->qrsPeakStr.timeSinceMax = 0;
		multDly = 3;
	}

	qrsDetStr->qrsPeakStr.lastDatum = datum;
	return(pk);
}


/******************************************************************************

    �������� multmedian()
	�﷨: int multmedian(int *array, int datnum)
	����: ��ֵ����
	���ã� ��
	�����ã�  qrsdet()
	��������� һ���鼰���鳤��
	��������� ��������ֵ
	����ֵ�� ��������ֵ
	������ ��

******************************************************************************/
int multmedian(int *array, int datnum)
{
	int i, j, k, temp, sort[20];

	for (i = 0; i < datnum; ++i)
	{
		sort[i] = array[i];
	}

	for (i = 0; i < datnum; ++i)
	{
		temp = sort[i];

		for (j = 0; (temp < sort[j]) && (j < i) ; ++j);

		for (k = i - 1 ; k >= j ; --k)
		{
			sort[k + 1] = sort[k];
		}

		sort[j] = temp;
	}
	return(sort[datnum >> 1]);
}


/******************************************************************************

    �������� multthresh()
	�﷨: int multthresh(int qmedian, int nmedian)
	����: Detection_Threshold = Median_Noise_Peak + 
	                               multTH*(Median_QRS_Peak  - Median_Noise_Peak)  
	���ã� ��
	�����ã�  qrsdet()
	��������� ������QRS����ֵ��������������ֵ
	��������� ��ֵ
	����ֵ�� ��ֵ
	������ multTHΪ��ֵϵ�������ļ������ж���

******************************************************************************/
int multthresh(int qmedian, int nmedian)
{
	int thrsh, dmed;
	real temp;
	dmed = qmedian - nmedian;
	temp = dmed;
	temp *= multTH;
	dmed = temp ;
	thrsh = nmedian + dmed; //��ʽ����
	return(thrsh);
}


/******************************************************************************

    �������� multblscheck()
	�﷨: int multblscheck(int *dBuf,int dbPtr)
	����: ��220ms�Ĵ��ڲ������ֵ����Сֵ����������ֵ����Сֵ��ָԭ�źŵ�
	      ���ֵ����б�ʣ������ֵ����Сֵ���򣨲�����һ��������������150ms��
		  ��������Ϊ�п�����QRS������
	���ã� ��
	�����ã�  qrsdet()
	��������� ԭʼ�ź�б�ʻ������飬��ǰб�ʵ�����
	��������� ��������ʱ����0������������ʱΪ1
	����ֵ�� ��������ʱ����0������������ʱΪ1
	������ ��

******************************************************************************/
int multblscheck(int *dBuf, int dbPtr)
{
	int max, min, maxt, mint, t, x;
	max = min = 0;

	return 0; //�ڴ�ֱ�ӷ���0ֵ��������������ˣ���ʱse��������ߣ�pe����½�
	          //һЩ��������һЩ������bls�������Ż���������

	for (t = 0; t < MS220; ++t)
	{
		x = dBuf[dbPtr];

		if (x > max)
		{
			maxt = t ;
			max = x ;
		}
		else if (x < min)
		{
			mint = t ;
			min = x;
		}

		if (--dbPtr <= 0)
		{
			dbPtr = DER_DELAY - 1;
		}
	}

	min = -min;
	
    //����150ms�ڳ�������б������Ϊ�п�����QRS������	   
	if ((max > (min>>3)) && (min > (max>>3)) &&
		(abs(maxt - mint) < MS150))
	{
		return(0);
	}
	else
	{
		return(1);
	}
	
}