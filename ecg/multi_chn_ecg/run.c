/******************************************************************************

    �������� run()
	�﷨�� int run(struct AnalysisData* outStruct, int* proArray,
	               int lenArray, struct Ecgqueue* queue, 
				   struct Displaydata* displaydata)
	������ ����һ���źţ����ø�����������źŽ��з����������������ڽṹ
	       outStruct��
	ԭ�� 
	���ã� qrsandonoffset()��arrhyalgone()����
	�����ã�������
	��������� proArray��lenArrayΪ�ź����鼰���鳤�ȣ��ṹqueue�д�ŵĵ���
	           �������źţ���ǰѭ�������е����꼰�ܲ�����
	��������� �ṹoutStruct�����Ͻṹ���嶨���run.h
	����ֵ���������ļ�⵽�������źŵ�QRS��λ�ã���δ�ҵ��µ�QRS�����򷵻�-1 
	������ �Ժ󻹿ɶԷ���ֵ���и���ϸ�Ķ���
    ע�⣺ ����run����һ��ѭ����ע��ÿ����ѭ����һЩ����Ҫ����ʼֵ
	 
******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "run.h"
#include "detect.h"
#include "qrsdet.h"

int run(struct AnalysisData* outStruct, int* proArray,
	    int lenArray, struct Ecgqueue* queue)
{
	int qrsposition = 0;
	int onset = 0, offset = 0, qrswide = 0;
	int stVaDif = 0;
	int pbuffer[MS220] = {0};
	int loopCtr = 0;
	int ecgValue = 0;
	int delay = 0;
	int lastqrsLOc = 0, lastHRate = 0;
	double rrInterval = 0;
	double rrInter = 0;
	double heartRate = 0;
	int HRate = 0;
	int zeroFlag = 0, flagCtr = 0;
	int sumHRate = 0, plusTimes = 0;
	const int memmovelen = 4 * sizeof(int);
	//int errornum = 0;//������

	int outBuf[bufferlength] = {0};
	int lenBuf = 0;
	int bufloop = 0;
	int ecgptr = 0;
	int bufLen_loop = 0;

	/*FILE *values, *offloc;
	char offlocname[50] = {"testsinusarrqrsloc.txt"};
	char filename[50] = {"testsinusarrfil.txt"};*/

	for (loopCtr = 0; loopCtr < lenArray; ++loopCtr)
	{
		++(queue->ecgnumber);
		ecgValue = proArray[loopCtr];
		ecgValue = sub_dc_offset_250(ecgValue, 0);
		ecgValue = sub_hf_250_fir_1(ecgValue, 0);
		ecgValue = sub_hf_250_fir_2(ecgValue, 0);
		ecgValue = subpowerline250(ecgValue, 0);
		//ѭ���洢ECG�ź�
		ecgValue = smooth_5_points(ecgValue, 0);

		/*values = fopen(filename, "a+");
		fprintf(values, "%d\n", ecgValue);
		fclose(values);*/

		outStruct->filterEcgValue = ecgValue;
		queue->ecgbuf[queue->ecgptr] = ecgValue;

		if (++(queue->ecgptr) == bufferlength)
		{
			queue->ecgptr = 0;
		}

		delay = qrsdet( ecgValue, 0 );

		if (delay != 0)
		{

			/*displaydata->flagOfdefg = 0;
			displaydata->judgeflag = 0;
			displaydata->oncedelayflag = 1;
			displaydata->bufferlengthflag = 0;
			if (displaydata->lastdelayflag == 0)
			{
				displaydata->delayflag = 1;
			}
			if ((displaydata->lastdelayflag == 1) || (displaydata->lastdelayflag == 2))
			{
				displaydata->delayflag = 2;
			}*/


			//++errornum;//������
			outStruct->delayflag = 0;
			lastqrsLOc = outStruct->qrsLoc;
			lastHRate = outStruct->lastHRate;
			qrsposition = qrsandonoffset(queue->ecgbuf, delay, queue->ecgptr,  
				        queue->ecgnumber, &(queue->lastecgnumber), 
						&(queue->lastOffSft), &(queue->thiOffSft), &onset, &offset,
						&stVaDif, pbuffer, outBuf, &lenBuf);

			/*offloc = fopen(offlocname, "a+");
			fprintf(offloc, "%d\n", outStruct->qrsLoc);
			fclose(offloc);*/

			qrswide = offset - onset;
			arrhyalgone( qrsposition, qrswide, &(outStruct->beaType),
				                                         &(outStruct->rhyme), 0);
			outStruct->delay = delay;
			outStruct->qrsLoc = qrsposition;
			outStruct->qrsOnLoc = onset;
			outStruct->qrsOffLoc = offset;
			outStruct->qrsWide = qrswide;
			outStruct->stDif = stVaDif;
			++(outStruct->qrsnum);
			//���������
			if (lastqrsLOc && qrsposition)
			{
				rrInterval = qrsposition - lastqrsLOc;
				rrInter = rrInterval / SAMPLE_RATE;
				heartRate = 60 / rrInter;
				HRate = (int)heartRate;
				if (lastHRate == 0)
				{
					outStruct->lastHRate = HRate;
				} 
				else if (((0.7 * outStruct->heartRate <= HRate) 
					        && (HRate <= 1.4 * outStruct->heartRate)) 
							           || (outStruct->heartRate) == 0 
							                 || (HRate <= 201 && HRate >= 40))
				{
					outStruct->lastHRate = HRate;
				}
				memmove(outStruct->HrArray, 
					                     &(outStruct->HrArray[1]), memmovelen);
				outStruct->HrArray[4] = outStruct->lastHRate;
				zeroFlag = 0;//����ʼֵ
				for (flagCtr = 0; flagCtr < 5; ++flagCtr)
				{
					if (outStruct->HrArray[flagCtr] == 0)
					{
						zeroFlag = zeroFlag + 1;
					}
				}
				if (zeroFlag >= 5)
				{
					outStruct->heartRate = 0;
				} 
				else
				{
					sumHRate = 0;//����ʼֵ
					plusTimes = 0;//����ʼֵ
					for (; zeroFlag < 5; ++zeroFlag)
					{
						++plusTimes;
						sumHRate = sumHRate + outStruct->HrArray[zeroFlag];
					}
					outStruct->heartRate = sumHRate / plusTimes;
				}
			}
			else
			{
				outStruct->heartRate = 0;
			}
			//��ȫ��RR���ں�
			if (outStruct->qrsnum > 1)
			{
				//�����õ���RR���ںͲ�̫������Ϊ�ź�ԭ��ʱ��δ��⵽QRS��ʱ
				//��QRS֮���кܳ�һ��ʱ��
				outStruct->sumRR = outStruct->sumRR + qrsposition - lastqrsLOc;
			}
			//�ò�����������
			if (outStruct->qrsnum > 5)
			{
				if (outStruct->beaType != 'N')
				{
					++(outStruct->abnorBeatNum);
				}
			}
			//��ST��ֵ
			if ((outStruct->qrsnum > 5) && (outStruct->qrsnum <= 15))
			{
				if (outStruct->beaType == 'N')
				{
					++(outStruct->refStNum);
					outStruct->sumRefStVa = outStruct->sumRefStVa 
						                                    + outStruct->stDif;
				}
			}
			if (outStruct->qrsnum > 15)
			{
				if (outStruct->beaType == 'N')
				{
					++(outStruct->sumStNum);
					outStruct->sumStValue = outStruct->sumStValue
						                                    + outStruct->stDif;
				}
			}

			/*if ((displaydata->lastdelayflag == 1) && (displaydata->delayflag == 2))
			{
				for (bufloop = __max(queue->thiOffSft - 1, 0); bufloop < lenBuf; ++bufloop)
				{
					displaydata->displaybuf[displaydata->saveptr] = outBuf[bufloop];
					++displaydata->saveptr;
					if (displaydata->saveptr >= displaylength)
					{
						displaydata->saveptr = 0;
					}
				}
			}

			if ((displaydata->lastdelayflag == 2) && (displaydata->delayflag == 2))
			{
				for (bufloop = 0; bufloop < lenBuf; ++bufloop)
				{
					displaydata->displaybuf[displaydata->saveptr] = outBuf[bufloop];
					++displaydata->saveptr;
					if (displaydata->saveptr >= displaylength)
					{
						displaydata->saveptr = 0;
					}
				}
			}

			displaydata->lastdelayflag = displaydata->delayflag;
*/

		}

		++outStruct->delayflag;
		if (outStruct->delayflag >= MS3000)
		{
			outStruct->heartRate = 0;
			if (queue->ecgnumber >= MS20000)
			{
				outStruct->asystoleflag = 1;
			}
		}

/*
		//��ʼѧϰ�׶Σ���һ���һ��
		if ((displaydata->oncedelayflag == 0) && (displaydata->delayflag == 0) && (displaydata->lastdelayflag == 0))
		{
			displaydata->displaybuf[displaydata->saveptr] = ecgValue;
			++displaydata->saveptr;
			if (displaydata->saveptr >= displaylength)
			{
				displaydata->saveptr = 0;
			}
		}

		//displaydata.judgeflag��������ȷ�������if��else ifֻ����һ��
		//�Ƿ��Ǽ�1,�����źŷ��࣬�������Ϊ�ҵ����������������ϵ�qrs��ʱ
		if (((queue->ecgnumber - (queue->lastecgnumber - queue->lastOffSft)) >= bufferlength) && ((displaydata->delayflag == 2) || (displaydata->flagOfdefg == 1)) && (displaydata->judgeflag != 2))
		{
			displaydata->flagOfdefg = 1;
			displaydata->delayflag = 0;
			displaydata->lastdelayflag = 0;
			displaydata->judgeflag = 1;
			if (displaydata->bufferlengthflag)
			{
				displaydata->displaybuf[displaydata->saveptr] = ecgValue;
				++displaydata->saveptr;
				if (displaydata->saveptr >= displaylength)
				{
					displaydata->saveptr = 0;
				}
			}

			if (displaydata->oncedelayflag && (!displaydata->bufferlengthflag))//��ԭ�����ҵ������浽��ǰbufferlength����
			{
				displaydata->bufferlengthflag = 1;
				ecgptr = queue->ecgptr;
				for (bufLen_loop = 0; bufLen_loop < bufferlength; ++bufLen_loop)
				{
					displaydata->displaybuf[displaydata->saveptr] = queue->ecgbuf[ecgptr];
					++displaydata->saveptr;
					++ecgptr;
					if (displaydata->saveptr >= displaylength)
					{
						displaydata->saveptr = 0;
					}
					if (ecgptr >= bufferlength)
					{
						ecgptr = 0;
					}
				}
			}

		}

		//�Ƿ��Ǽ�1,�����źŷ��࣬�������Ϊ�ҵ���δ�ҵ��������������ϵ�qrs��ʱ
		//2013_5_3��
		//��(i - last_i) >= bufferlength)��Ϊ(i - last_i + 1) >= bufferlength)
		else if (((queue->ecgnumber - queue->lastecgnumber + 1) >= bufferlength) && ((displaydata->delayflag == 1) || (displaydata->flagOfdefg == 2)) && (displaydata->judgeflag != 1))//�Ƿ��Ǽ�1
		{
			displaydata->flagOfdefg = 2;
			displaydata->delayflag = 0;
			displaydata->lastdelayflag = 0;
			displaydata->judgeflag = 2;

			if (displaydata->bufferlengthflag)
			{
				displaydata->displaybuf[displaydata->saveptr] = ecgValue;
				++displaydata->saveptr;
				if (displaydata->saveptr >= displaylength)
				{
					displaydata->saveptr = 0;
				}
			}

			if (displaydata->oncedelayflag && (!displaydata->bufferlengthflag))//��ԭ�����ҵ������浽��ǰbufferlength����
			{
				displaydata->bufferlengthflag = 1;
				ecgptr = queue->ecgptr - 1;
				//�¼�2_4_11_17
				if (ecgptr < 0)
				{
					ecgptr = bufferlength - 1;
				}
				for (bufLen_loop = 0; bufLen_loop < bufferlength; ++bufLen_loop)
				{
					displaydata->displaybuf[displaydata->saveptr] = queue->ecgbuf[ecgptr];
					++displaydata->saveptr;
					++ecgptr;
					if (displaydata->saveptr >= displaylength)
					{
						displaydata->saveptr = 0;
					}
					if (ecgptr >= bufferlength)
					{
						ecgptr = 0;
					}
				}
			}

		}*/
		
/*
		if (queue->ecgnumber <= bufferlength)
		{
			outStruct->maintainrcgvalue = 0;
		} 
		else
		{
			outStruct->maintainrcgvalue = displaydata->displaybuf[displaydata->maintainptr];
			++displaydata->maintainptr;
			if (displaydata->maintainptr >= displaylength)
			{
				displaydata->maintainptr = 0;
			}
		}*/

		/*values = fopen(filename, "a+");
		fprintf(values, "%d\n", outStruct->filterEcgValue);
		fclose(values);*/

	}
	
	if (delay == 0)
	{
		return(-1);
	} 
	else
	{
		return(outStruct->qrsLoc);
	}
} 