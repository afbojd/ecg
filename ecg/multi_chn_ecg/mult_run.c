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
#include "mult_run.h"
#include "filter.h"
#include "fundecla.h"
#include <stdio.h>

int mult_run(TotalAnalysisStruct* TotalStruct, int* proArray, int lenArray)
{
	int ecgvalue = 0;
	int delay = 0;
	int lastqrsLOc = 0;
	int liv_loopctrone = 0;

	/*FILE *values;
	char filename[50] = {"threeasystolefil1.txt"};
	FILE *qrsLocfil;
	char qrsLocfilename[50] = {"threeasystoleqrsLoc21.txt"};*/

	for (liv_loopctrone = 0; liv_loopctrone < lenArray; ++liv_loopctrone)
	{
		//���µ�ǰ��ͨ���Ĳ�������ֵ
		++(TotalStruct->ChannelAnalysis[liv_loopctrone].Ecgqueue.ecgnumber);
		//�˲�����Ҫ�жϸ�����������
		ecgvalue = proArray[liv_loopctrone];
		ecgvalue = hpFilter(&TotalStruct->ChannelAnalysis[liv_loopctrone].FilterData, ecgvalue);
		ecgvalue = lpFilter(&TotalStruct->ChannelAnalysis[liv_loopctrone].FilterData, ecgvalue);
		ecgvalue = subpowerline(&TotalStruct->ChannelAnalysis[liv_loopctrone].FilterData, ecgvalue);
		ecgvalue = smooth(&TotalStruct->ChannelAnalysis[liv_loopctrone].FilterData, ecgvalue);

		TotalStruct->ChannelAnalysis[liv_loopctrone].OutDate.filterEcgValue = ecgvalue;
		TotalStruct->ChannelAnalysis[liv_loopctrone].Ecgqueue.ecgbuf[TotalStruct->ChannelAnalysis[liv_loopctrone].Ecgqueue.ecgptr] = ecgvalue;
		if (++(TotalStruct->ChannelAnalysis[liv_loopctrone].Ecgqueue.ecgptr) >= bufferlength)
		{
			TotalStruct->ChannelAnalysis[liv_loopctrone].Ecgqueue.ecgptr = 0;
		}

		/*if (liv_loopctrone == 0)
		{
			values = fopen(filename, "a+");
			fprintf(values, "%d", ecgvalue);
			fprintf(values, ",");
			fclose(values);
		} 
		else if (liv_loopctrone == 1)
		{
			values = fopen(filename, "a+");
			fprintf(values, "%d", ecgvalue);
			fprintf(values, ",");
			fclose(values);
		}
		else
		{
			values = fopen(filename, "a+");
			fprintf(values, "%d\n", ecgvalue);
			fclose(values);
		}*/

		//�����㷨ͨ�������жϾ���
		if (TotalStruct->ChannelAnalysis[liv_loopctrone].algoChan)
		{
			//����������ڴ��ж������
			delay = multqrsdet( &TotalStruct->ChannelAnalysis[liv_loopctrone].qrsDetStr, ecgvalue );
			if (delay != 0)
			{
				TotalStruct->ChannelAnalysis[liv_loopctrone].delayflag = 0;
				lastqrsLOc = TotalStruct->ChannelAnalysis[liv_loopctrone].qrsLoc;
				accurloc( &TotalStruct->ChannelAnalysis[liv_loopctrone], delay );	
				
				++TotalStruct->ChannelAnalysis[liv_loopctrone].qrsnum;
				statfun( &TotalStruct->ChannelAnalysis[liv_loopctrone], lastqrsLOc );

				/*qrsLocfil = fopen(qrsLocfilename, "a+");
				fprintf(qrsLocfil, "%d", TotalStruct->ChannelAnalysis[liv_loopctrone].qrsLoc);
				fprintf(qrsLocfil, ",");
				fclose(qrsLocfil);*/
			}
			++TotalStruct->ChannelAnalysis[liv_loopctrone].delayflag;
			if (TotalStruct->ChannelAnalysis[liv_loopctrone].delayflag >= MS3000)
			{
				TotalStruct->ChannelAnalysis[liv_loopctrone].heartRate = 0;
			}

			
		}

		
		delay = 0;//ÿ��ѭ���󽫴�ֵ���㣬���������ļ���Ҳ���з���ֵ
	}

	return 0;
}