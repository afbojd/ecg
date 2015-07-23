/******************************************************************************
    �ļ���: qrsdet.c
	������ ���ļ��а�������Ϊ���QRS���ļ�⺯�����Լ���⺯��������Ӻ���
	qrsdet()�����ᱻ�����������ã��������������ڲ�����,�����������ⲿ����
	�����б�
	1��qrsdet()     ���ļ�����������������������������⵽һ��QRS���������ӳ�
	2��peak()       ����ֵ����
	3��median()     ��ֵ����
	4��thresh()     ��ֵ����
	5��blscheck()   ����Ư�Ƽ�⺯��
	6��qrsfilter()  �˲��������ⲿ����
	7��deriv1()     ��ֺ������ⲿ����
******************************************************************************/



#include <string.h>		// memmove�������ڿ��ļ�
#include <math.h>
#include "qrsdet.h"
#include <stdlib.h>

// �ⲿ��������

int qrsfilter(int datum, int init);
int deriv1( int x0, int init );

// �ڲ���������

int peak( int datum, int init );
int median(int *array, int datnum);
int thresh(int qmedian, int nmedian);
int blscheck(int *dBuf,int dbPtr);    //baseline shift check����ƫ�Ƽ��


double TH = 0.4; //��ֵϵ��

int ddBuffer[DER_DELAY], ddPtr;	//���ԭʼ�źŲ�ֵ�Ļ���
int Dly = 0;

const int memmovelen = 7 * sizeof(int);


/******************************************************************************

    �������� qrsdet()
	�﷨�� int qrsdet( int datum, int init )
	������ �˲��ܺ��������ø��˲������õ������˲�������ź�ֵ
	���ã�
	       1��qrsfilter()   �˲�����
	       2��deriv1()      ��ֵ����
	       3��peak()        ��ֵ��⺯��
	       4��median()      ��ֵ����
	       5��thresh()      ��ֵ����
	       6��blscheck()    ����Ư�Ƽ�⺯��
	�����ã��ϲ㺯������������
	��������� ԭʼ�ź������Ƿ��ʼ������
	��������� ����⵽һ��QRS��������������ʱ��δ��⵽�������0ֵ
	����ֵ�� ����⵽һ��QRS��������������ʱ��δ��⵽�������0ֵ
	������ ��init��0ʱ��һЩ������ʼֵ��qrsfilter()��peak()������ʼ��

******************************************************************************/

int qrsdet( int datum, int init )
{
	static int det_thresh, qpkcnt = 0;
	static int qrsbuf[8], noise[8], rrbuf[8];
	static int rsetBuff[8], rsetCount = 0;
	static int nmedian, qmedian, rrmedian;

	// sbpeak: search back peak
	// sbloc: search back location
	// sbcount: search back count
	static int count, sbpeak = 0, sbloc, sbcount = MS1500;
	static int initBlank, initMax;
	static int preBlankCnt, tempPeak;

	int fdatum, qrsDelay = 0;
	int i, newPeak, aPeak;

	//��ʼ��
	if ( init )
	{
		for (i = 0; i < 8; ++i)
		{
			noise[i] = 0;    	// ��ʼ����������
			rrbuf[i] = MS1000;  // ��ʼ��RR���ڻ���
		}

		qpkcnt = count = sbpeak = 0;
		initBlank = initMax = preBlankCnt = ddPtr = 0;
		sbcount = MS1500;
		qrsfilter(0, 1);	// ��ʼ���˲�����
		peak(0, 1);         // ��ʼ����ֵ��⺯��
	}

	fdatum = qrsfilter(datum, 0);	//��ԭʼ�����˲�
	aPeak = peak(fdatum, 0);        //��ֵ���

	// ��κ��������������������Ĳ�Ӧ�ڣ���MS200Ϊ��Ӧ��
	newPeak = 0;
	if (aPeak && !preBlankCnt)			// ��200ms��Ӧ���ѵ��Ҳ���һ��peakֵ
	{								    // ��洢��peakֵ�����ò�Ӧ�ڼ�������
		tempPeak = aPeak;
		preBlankCnt = PRE_BLANK;
	}
	else if(!aPeak && preBlankCnt)	   // ����Ӧ���ѹ�������peak����ʱ���Ÿ�ֵ
	{								   // newpeak����Ϊ�п��ܲ���QRS��
		if(--preBlankCnt == 0)         // ��Ҳ��ΪʲôFILTER_DELAY�м�����
		{                              // PRE_BLANK��Ե��
			newPeak = tempPeak;
		}
	}
	else if (aPeak)					   // ����ڲ�Ӧ���������²������µ�peak
	{
		if (aPeak > tempPeak)		   // ����peakֵ����ǰ���peakֵ
		{                              // ���ظ�һ������ĵĲ���
			tempPeak = aPeak;
			preBlankCnt = PRE_BLANK;
		}
		else if (--preBlankCnt == 0)
		{
			newPeak = tempPeak;
		}
	}

	//�洢ԭʼ�źŵ�б�ʣ���ŵ����鼴���߼����������
	ddBuffer[ddPtr] = deriv1( datum, 0 );
	if (++ddPtr == DER_DELAY)
	{
		ddPtr = 0;
	}

	//ǰ8����ʵ��Ϊ8s����Ϊ����MS1000ʱqpkcnt��1��
	if ( qpkcnt < 8 )
	{
		++count;

		if(newPeak > 0)
		{
			count = WINDOW_WIDTH;
		}
		++initBlank;
		if(initBlank == MS1000)
		{
			initBlank = 0;
			qrsbuf[qpkcnt] = initMax;
			initMax = 0;
			++qpkcnt;

			if(qpkcnt == 8)
			{
				qmedian = median(qrsbuf, 8);
				nmedian = 0;
				rrmedian = MS1000;
				sbcount = MS1500 + MS150;
				det_thresh = thresh(qmedian, nmedian);
			}
		}

		if ( newPeak > initMax )
		{
			initMax = newPeak;
		}
	}

	else	// ��ʱ�ſ�ʼ���QRS��
	{
		++count;
		if (newPeak > 0)
		{
			// ���peak�Ƿ����ڼ���ƫ�����𣬲�������Ϊ�п��ܲ���QRS��
			if (!blscheck(ddBuffer, ddPtr))
			{

				// ��peakֵ����̽����ֵ������Ϊ�п��ܲ���QRS��
				if (newPeak > det_thresh)
				{
					memmove(&qrsbuf[1], qrsbuf, memmovelen);
					qrsbuf[0] = newPeak;
					qmedian = median(qrsbuf, 8);
					det_thresh = thresh(qmedian, nmedian);
					memmove(&rrbuf[1], rrbuf, memmovelen);
					rrbuf[0] = count - WINDOW_WIDTH;
					rrmedian = median(rrbuf, 8);
					sbcount = rrmedian + (rrmedian >> 1) + WINDOW_WIDTH;
					count = WINDOW_WIDTH;
					sbpeak = 0;
					qrsDelay = WINDOW_WIDTH + FILTER_DELAY;
					initBlank = initMax = rsetCount = 0;
				}

				// ��Ϊ�����������peak�����п�������֮ǰ
				// ��ƥ��QRS������С������
				else
				{
					memmove(&noise[1], noise, memmovelen);
					noise[0] = newPeak;
					nmedian = median(noise, 8);
					det_thresh = thresh(qmedian, nmedian);

					// Don't include early peaks (which might be T-waves)
					// in the search back process.  A T-wave can mask
					// a small following QRS.

					if ((newPeak > sbpeak) &&
						             ((count - WINDOW_WIDTH) >= MS360))
					{
						sbpeak = newPeak;
						sbloc = count - WINDOW_WIDTH;
					}
				}

				//���о�ȥ��
				//if ((count > sbcount) && (sbpeak > (det_thresh >> 1)))
				//{
				//	memmove(&qrsbuf[1], qrsbuf, memmovelen);
				//	qrsbuf[0] = sbpeak;
				//	qmedian = median(qrsbuf, 8);
				//	det_thresh = thresh(qmedian, nmedian);
				//	memmove(&rrbuf[1], rrbuf, memmovelen);
				//	rrbuf[0] = sbloc;
				//	rrmedian = median(rrbuf, 8);
				//	sbcount = rrmedian + (rrmedian >> 1) + WINDOW_WIDTH;
				//	qrsDelay = count = count - sbloc;
				//	qrsDelay += FILTER_DELAY;
				//	sbpeak = 0;
				//	initBlank = initMax = rsetCount = 0;
				//}
			}
		}

		//���ݻ���
		if ((count > sbcount) && (sbpeak > (det_thresh >> 1)))
		{
			memmove(&qrsbuf[1], qrsbuf, memmovelen);
			qrsbuf[0] = sbpeak;
			qmedian = median(qrsbuf, 8);
			det_thresh = thresh(qmedian, nmedian);
			memmove(&rrbuf[1], rrbuf, memmovelen);
			rrbuf[0] = sbloc;
			rrmedian = median(rrbuf, 8);

			// sbcount = 1.5 * RR����
			sbcount = rrmedian + (rrmedian >> 1) + WINDOW_WIDTH;
			qrsDelay = count = count - sbloc;
			qrsDelay += FILTER_DELAY;
			sbpeak = 0;
			initBlank = initMax = rsetCount = 0;
		}
	}

	//����8s��δ��⵽QRS�������³�ʼ��һЩֵ��qrsbuf��ֵΪÿ1s�����ֵ
/*
	if ( qpkcnt == 8 )
	{
		if (++initBlank == MS1000)
		{
			initBlank = 0;
			rsetBuff[rsetCount] = initMax;
			initMax = 0;
			++rsetCount;

			//����8s����
			if (rsetCount == 8)
			{
				for (i = 0; i < 8; ++i)
				{
					qrsbuf[i] = rsetBuff[i];
					noise[i] = 0;
				}
				qmedian = median( rsetBuff, 8 );
				nmedian = 0;
				rrmedian = MS1000;
				sbcount = MS1500 + MS150;
				det_thresh = thresh(qmedian, nmedian);
				initBlank = initMax = rsetCount = 0;
                sbpeak = 0;
			}
		}

		if ( newPeak > initMax )
		{
			initMax = newPeak;
		}
	}*/

	return(qrsDelay);
}


/******************************************************************************

    �������� peak()
	�﷨: int peak( int datum, int init )
	����: ��ֵ��⺯��������Ԥ�������źţ����ź����ϸ��źţ�
	      �Լ����ֵ�Ƚϣ����������������ֵ�����浱ǰ�ź�ֵ��
	      ����0���������������ֵ��һ�룬�������ֵ��Dly Ϊ0��
	      �����ж���ǰ�ź�λ�õ����ֵ���룬����95ms��
	      �������ֵ��Dly = 3������֮�⣬���� 0��
	���ã� ��
	�����ã�  qrsdet()
	��������� Ԥ�������źż��Ƿ��ʼ������
	��������� ��ֵ
	����ֵ�� ��ֵ��Dly
	������ Dlyδ��������

******************************************************************************/

int peak( int datum, int init )
{
	static int max = 0, timeSinceMax = 0, lastDatum;
	int pk = 0;

	if (init)
	{
		max = timeSinceMax = 0;
	}

	if (timeSinceMax > 0)
	{
		++timeSinceMax;
	}

	if ((datum > lastDatum) && (datum > max))
	{
		max = datum;

		//���������������Ĵ�����ʱ�Ÿ�ֵtimeSinceMax
		if (max > 2)
		{
			timeSinceMax = 1;
		}
	}
	else if (datum < (max >> 1))
	{
		pk = max;
		max = 0;
		timeSinceMax = 0;
		Dly = 0;
	}
	else if (timeSinceMax > MS95)
	{
		pk = max;
		max = 0;
		timeSinceMax = 0;
		Dly = 3;
	}

	lastDatum = datum;
	return(pk);
}


/******************************************************************************

    �������� median()
	�﷨: int median(int *array, int datnum)
	����: ��ֵ����
	���ã� ��
	�����ã�  qrsdet()
	��������� һ���鼰���鳤��
	��������� ��������ֵ
	����ֵ�� ��������ֵ
	������ ��

******************************************************************************/

int median(int *array, int datnum)
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

    �������� thresh()
	�﷨: int thresh(int qmedian, int nmedian)
	����: Detection_Threshold = Median_Noise_Peak +
	                               TH*(Median_QRS_Peak  - Median_Noise_Peak)
	���ã� ��
	�����ã�  qrsdet()
	��������� ������QRS����ֵ��������������ֵ
	��������� ��ֵ
	����ֵ�� ��ֵ
	������ THΪ��ֵϵ�������ļ������ж���

******************************************************************************/

int thresh(int qmedian, int nmedian)
{
	int thrsh, dmed;
	double temp;
	dmed = qmedian - nmedian;
	temp = dmed;
	temp *= TH;
	dmed = temp ;
	thrsh = nmedian + dmed; //��ʽ����
	return(thrsh);
}


/******************************************************************************

    �������� blscheck()
	�﷨: int blscheck(int *dBuf,int dbPtr)
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

int blscheck(int *dBuf, int dbPtr)
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
