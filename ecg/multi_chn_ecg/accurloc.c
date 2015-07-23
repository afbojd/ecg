/******************************************************************************

    �������� accurloc()
	�﷨�� int accurloc( ChannelAnalysisStruct* ChanAnaStr,
	                                      int delay, QrsBufStruct* QrsBufStr)
	������ accurloc--accurate location����ȷλ�ò��Һ�����
	       ����qrsdet�����õ����ӳ٣��Լ��������Ӧ������׼ȷ�õ�QRS����
	       ����λ���Լ�QRS��������յ�
	ԭ�� ��һ����Χ�ڣ��ҵ����ֵ����Сֵ�����ǵ����꣬Ϊ����Ǳ��������
	       �����ȷֱ������������һ��б�ʣ�б��֮����Ϊ�Ǹ�ʱ�����߲ſ���
		   ����Ǳ�������㣬�ڱȽ�����MS20���б�ʺͣ����Ϊ����
		   ԭ���������ӣ��ѵ��Ǿ�����ֵ��ѡȡ
	���ã� ��׼���е�abs������ֵ������
	�����ã�int run(TotalAnalysisStruct* TotalStruct,
	                                     int* proArray, int lenArray)
	��������� �㷨ͨ���ṹ�壬��ͨ��QRS����ӳ٣��м�ṹ��
	��������� �㷨ͨ���ṹ�壬��ͨ��QRS����ӳ٣��м�ṹ��
	����ֵ�� ��������δ��ȷ����
	������ �����Ǵ����һ��ѭ�������У����ڸ��������Ҫ���Ǻñ߽�����
	       ��������Ĭ��qrsdet��һ��delay��������һ��QRS��
    ע�⣺ �������д��ʱ��������һֱ�����⣬�������ԣ������������±�����
	       ��������liv_max_locat_right == bufferlength��
		   ��Ϊ��liv_max_locat_right >= bufferlength������
		   ���ҷ�ΧΪ�������ݣ�������Ӧ���е��ĵ����ݣ�����Ժ��Ǽ�ǿ
		   Ŀǰ��ѡȡ����ֵ����Ӧ����100��������

******************************************************************************/
#include <string.h>
#include <math.h>
#include "qrsdet.h"
#include "mult_run.h"
#include <stdlib.h>

int accurloc( ChannelAnalysisStruct* ChanAnaStr, int delay )
{
	int liv_ptr = 0;//���ҵ���������
	int liv_begin = 0;
	int liv_control = 0;

	int liv_max = 0, liv_max_locat = 0;//���ֵ��(��ecgbuffer�е�)λ��
	int liv_max_locat_left = 0, liv_max_locat_right = 0;//����һ������
	int liv_max_locat_left_slope = 0, liv_max_locat_right_slope = 0;
	int liv_max_locat_mult_slope = 0;//����һ��б��֮��
	int liv_max_locat_left_MS20 = 0, liv_max_locat_right_MS20 = 0;//����
	int liv_max_locat_left_MS20_slope = 0, liv_max_locat_right_MS20_slope = 0;
	int liv_max_locat_MS20_plus_slope = 0;//����MS20��б�ʾ���ֵ֮��

	int liv_min = 0, liv_min_locat = 0;//��Сֵ��(��ecgbuffer�е�)λ��
	int liv_min_locat_left = 0, liv_min_locat_right = 0;//����һ������
	int liv_min_locat_left_slope = 0, liv_min_locat_right_slope = 0;
	int liv_min_locat_mult_slope = 0;//����һ��б��֮��
	int liv_min_locat_left_MS20 = 0, liv_min_locat_right_MS20 = 0;//����
	int liv_min_locat_left_MS20_slope = 0, liv_min_locat_right_MS20_slope = 0;
	int liv_min_locat_MS20_plus_slope = 0;//����MS20��б�ʾ���ֵ֮��

	int liv_max_or_min_loc = 0;//buffer�У�����ƫ����
	int qrs_shift = 0;//QRS��ƫ����
//	int qrs_position = 0;//QRS��λ��

//	int ecgbufferbegin = 0;//QRS������ecgbuffer�е�����
//	int liv_loop = 0;//��ֵqrsbuffer���������

	//int pbufferbegin = 0;//P����������
	//int pbuf_loop = 0;//��ֵpbuffer���������

	/*int newecgbuf[bufferlength] = {0};
	int offsetPtrShift = 0;
	int onsetPtrShift = 0;
	int lastOffsetPtrShift = 0;
	int bufLen = 0;
	int newecgbufbegin = 0;
	int newecgbuf_loop = 0;
	int onsetFlag = 0;*/

	//������������
	liv_ptr = ChanAnaStr->Ecgqueue.ecgptr - delay - MS70;//liv_ptr = bufferptr - delay - MS10;
	if (liv_ptr < 0)
	{
		liv_ptr = liv_ptr + bufferlength;
	}

	liv_begin = liv_ptr - MS80;//MS70
	if (liv_begin < 0)
	{
		liv_begin = liv_begin + bufferlength;
	}

	//Ѱ�������Сֵ������
	liv_max = liv_min = ChanAnaStr->Ecgqueue.ecgbuf[liv_begin];
	//2013_5_3 13:29����bug����Сֵ����Ӧ���ȸ�ֵΪliv_begin������
	//û��������������liv_max_locat * liv_min_locat = 0
	liv_max_locat = liv_min_locat = liv_begin;

	for (liv_control = 0; liv_control < 2 * MS100; ++liv_control)
	{
		if (++liv_begin >= bufferlength)
		{
			liv_begin =0;
		}

		if (liv_max < ChanAnaStr->Ecgqueue.ecgbuf[liv_begin])
		{
			liv_max = ChanAnaStr->Ecgqueue.ecgbuf[liv_begin];
			liv_max_locat = liv_begin;
		}

		if (liv_min > ChanAnaStr->Ecgqueue.ecgbuf[liv_begin])
		{
			liv_min = ChanAnaStr->Ecgqueue.ecgbuf[liv_begin];
			liv_min_locat = liv_begin;
		}
	}

	//�����ﴦ��˭������
	//�Ƚ�����һ�����궨�ã�Ŀ������б�����һ�Ϊ������
	liv_max_locat_left = liv_max_locat - 1;
	if (liv_max_locat_left < 0)
	{
		liv_max_locat_left = liv_max_locat_left + bufferlength;
	}

	liv_max_locat_right = liv_max_locat + 1;
	if (liv_max_locat_right >= bufferlength)
	{
		liv_max_locat_right = 0;
	}

	liv_min_locat_left = liv_min_locat - 1;
	if (liv_min_locat_left < 0)
	{
		liv_min_locat_left = liv_min_locat_left + bufferlength;
	}

	liv_min_locat_right = liv_min_locat + 1;
	if (liv_min_locat_right >= bufferlength)
	{
		liv_min_locat_right = 0;
	}

	//������MS20�����궨��,Ŀ��б��ȷ������
	liv_max_locat_left_MS20 = liv_max_locat - MS20;
	if (liv_max_locat_left_MS20 < 0)
	{
		liv_max_locat_left_MS20 = liv_max_locat_left_MS20 + bufferlength;
	}

	liv_max_locat_right_MS20 = liv_max_locat + MS20;
	if (liv_max_locat_right_MS20 >= bufferlength)
	{
		liv_max_locat_right_MS20 = 0;
	}

	liv_min_locat_left_MS20 = liv_min_locat - MS20;
	if (liv_min_locat_left_MS20 < 0)
	{
		liv_min_locat_left_MS20 = liv_min_locat_left_MS20 + bufferlength;
	}

	liv_min_locat_right_MS20 = liv_min_locat + MS20;
	if (liv_min_locat_right_MS20 >= bufferlength)
	{
		liv_min_locat_right_MS20 = 0;
	}

	//��������һ��б��
	liv_max_locat_left_slope = ChanAnaStr->Ecgqueue.ecgbuf[liv_max_locat] -
		                       ChanAnaStr->Ecgqueue.ecgbuf[liv_max_locat_left];
	liv_max_locat_right_slope =
		                ChanAnaStr->Ecgqueue.ecgbuf[liv_max_locat_right] -
		                ChanAnaStr->Ecgqueue.ecgbuf[liv_max_locat];
	liv_max_locat_mult_slope = liv_max_locat_left_slope *
		                       liv_max_locat_right_slope;

	liv_min_locat_left_slope = ChanAnaStr->Ecgqueue.ecgbuf[liv_min_locat] -
		                       ChanAnaStr->Ecgqueue.ecgbuf[liv_min_locat_left];
	liv_min_locat_right_slope =
		                ChanAnaStr->Ecgqueue.ecgbuf[liv_min_locat_right] -
						ChanAnaStr->Ecgqueue.ecgbuf[liv_min_locat];
	liv_min_locat_mult_slope = liv_min_locat_left_slope  *
		                       liv_min_locat_right_slope;

	//��������MS20��б��
	liv_max_locat_left_MS20_slope =
		           abs(ChanAnaStr->Ecgqueue.ecgbuf[liv_max_locat] -
					   ChanAnaStr->Ecgqueue.ecgbuf[liv_max_locat_left_MS20]);
	liv_max_locat_right_MS20_slope =
		           abs(ChanAnaStr->Ecgqueue.ecgbuf[liv_max_locat_right_MS20] -
					   ChanAnaStr->Ecgqueue.ecgbuf[liv_max_locat]);
	liv_max_locat_MS20_plus_slope = liv_max_locat_left_MS20_slope +
		                            liv_max_locat_right_MS20_slope;

	liv_min_locat_left_MS20_slope =
		           abs(ChanAnaStr->Ecgqueue.ecgbuf[liv_min_locat] -
				       ChanAnaStr->Ecgqueue.ecgbuf[liv_min_locat_left_MS20]);
	liv_min_locat_right_MS20_slope =
		           abs(ChanAnaStr->Ecgqueue.ecgbuf[liv_min_locat_right_MS20] -
				       ChanAnaStr->Ecgqueue.ecgbuf[liv_min_locat]);
	liv_min_locat_MS20_plus_slope = liv_min_locat_left_MS20_slope +
		                            liv_min_locat_right_MS20_slope;

	//һ�������£���������
	if ((liv_max_locat_mult_slope <= 0) && (liv_min_locat_mult_slope <= 0))
	{
		if (liv_max_locat_MS20_plus_slope >=
			                       0.80 * liv_min_locat_MS20_plus_slope)
		{
			liv_max_or_min_loc = liv_max_locat;
		}
		else
		{
			liv_max_or_min_loc = liv_min_locat;
		}
	}
	else if (liv_max_locat_mult_slope <= 0)
	{
		liv_max_or_min_loc = liv_max_locat;
	}
	else if (liv_min_locat_mult_slope <= 0)
	{
		liv_max_or_min_loc = liv_min_locat;
	}
	else
	{
		liv_max_or_min_loc = liv_max_locat;
	}

	//�˴��õ�QRS��������λ��
	qrs_shift = ChanAnaStr->Ecgqueue.ecgptr - liv_max_or_min_loc;
	if (qrs_shift < 0)
	{
		qrs_shift = qrs_shift + bufferlength;
	}
	//QRS��λ��
	ChanAnaStr->qrsLoc = ChanAnaStr->Ecgqueue.ecgnumber - qrs_shift + 1;

	return 0;
}
