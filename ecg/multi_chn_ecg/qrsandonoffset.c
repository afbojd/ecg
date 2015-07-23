/******************************************************************************

    �������� qrsandonoffset()
	�﷨�� int qrsandonoffset(int *buffer, int delay, int bufferptr,
	                          int samplepoint, int *lastSample,
	                          int *lastOffshift, int *thiOffshift,
							  int *qrsOnset,
	                          int *qrsOffset, int *stDif,
	                          int *pbuffer, int *outecgbuf, int *bufLength)
	������ ����qrsdet�����õ����ӳ٣��Լ��������Ӧ������׼ȷ�õ�QRS����
	       ����λ���Լ�QRS��������յ�
	ԭ�� ��һ����Χ�ڣ��ҵ����ֵ����Сֵ�����ǵ����꣬Ϊ����Ǳ��������
	       �����ȷֱ������������һ��б�ʣ�б��֮����Ϊ�Ǹ�ʱ�����߲ſ���
		   ����Ǳ�������㣬�ڱȽ�����MS20���б�ʺͣ����Ϊ����
		   ԭ���������ӣ��ѵ��Ǿ�����ֵ��ѡȡ
	���ã� 1��qrsonset()������QRS�������Һ���
	       2��qrsoffset()������QRS���յ���Һ���
	�����ã��ϲ㺯������������
	��������� ecgbuffer�����ecg�źţ�delay��qrsdet�������صļ����ʱ
	           bufferptr����ǰbufferԪ�ص����ꣻsamplepoint��ecg�ź�����
	��������� qrsOnset��qrsOffset��stDif��ST����ȵ�λ��Ĳ�ֵ��
	����ֵ�� QRS���ϲ�������ȷλ��
	������ �����Ǵ����һ��ѭ�������У����ڸ��������Ҫ���Ǻñ߽�����
	       ��������Ĭ��qrsdet��һ��delay��������һ��QRS��
    ע�⣺ �������д��ʱ��������һֱ�����⣬�������ԣ������������±�����
	       ��������liv_max_locat_right == bufferlength��
		   ��Ϊ��liv_max_locat_right >= bufferlength������
		   ���ҷ�ΧΪ�������ݣ�������Ӧ���е��ĵ����ݣ�����Ժ��Ǽ�ǿ
		   Ŀǰ��ѡȡ����ֵ����Ӧ����100��������

******************************************************************************/
#include "qrsdet.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
int qrsonset( int *buf, int maxvalue, int minvalue, int *isopoint );
int qrsoffset( int *buf, int maxvalue, int minvalue, int *stpoint );
int qrsmooth(int *buf);
/*int pro_newecgbuf( int *outbuf, int *buf, int bufLen, int onsetflag);*/

int qrsandonoffset(int *buffer, int delay, int bufferptr,int samplepoint,
	               int *lastSample, int *lastOffshift, int *thiOffshift, int *qrsOnset,
	               int *qrsOffset, int *stDif, int *pbuffer, int *outecgbuf, int *bufLength)
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
	int qrs_position = 0;//QRS��λ��
	int onsetShift = 0, offsetShift = 0;//QRS������յ�ƫ����

	int qrsbufferorg[MS280] = {0};//QRS���Ҷλ��棬������λ��ΪMS125��
	int qrsbuffer[MS280] = {0};//QRS���Ҷλ��棬������λ��ΪMS125��
	int qrsbuffer2[MS280] = {0};//QRS���Ҷλ��棬������λ��ΪMS125��
	int ecgbufferbegin = 0;//QRS������ecgbuffer�е�����
	int liv_loop = 0;//��ֵqrsbuffer���������

	int isoLoc = 0;//�ȵ�λ����qrsbuf�е�����
	int stLoc = 0;//ST����qrsbuf�е�����
	int liv_iso = 0;//ѭ���������꣩
	int isoTimes = 0, sumIso = 0, isoValue = 0;//�������ܺͼ�ƽ��ֵ
	int liv_st = 0;////ѭ���������꣩
	int stTimes = 0, sumSt = 0, stValue = 0;//�������ܺͼ�ƽ��ֵ

	int pbufferbegin = 0;//P����������
	int pbuf_loop = 0;//��ֵpbuffer���������


/*
	int newecgbuf[bufferlength] = {0};
	int offsetPtrShift = 0;
	int onsetPtrShift = 0;
	int lastOffsetPtrShift = 0;
	int bufLen = 0;
	int newecgbufbegin = 0;
	int newecgbuf_loop = 0;
	int onsetFlag = 0;
*/



	//������������
	liv_ptr = bufferptr - delay - MS70;//liv_ptr = bufferptr - delay - MS10;
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
	liv_max = liv_min = buffer[liv_begin];
	//2013_5_3 13:29����bug����Сֵ����Ӧ���ȸ�ֵΪliv_begin������
	//û��������������liv_max_locat * liv_min_locat = 0
	liv_max_locat = liv_min_locat = liv_begin;

	for (liv_control = 0; liv_control < 2 * MS100; ++liv_control)
	{
		if (++liv_begin >= bufferlength)
		{
			liv_begin =0;
		}

		if (liv_max < buffer[liv_begin])
		{
			liv_max = buffer[liv_begin];
			liv_max_locat = liv_begin;
		}

		if (liv_min > buffer[liv_begin])
		{
			liv_min = buffer[liv_begin];
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
	liv_max_locat_left_slope = buffer[liv_max_locat]
	                               - buffer[liv_max_locat_left];
	liv_max_locat_right_slope = buffer[liv_max_locat_right]
	                               - buffer[liv_max_locat];
	liv_max_locat_mult_slope = liv_max_locat_left_slope
		                           * liv_max_locat_right_slope;

	liv_min_locat_left_slope = buffer[liv_min_locat]
	                               - buffer[liv_min_locat_left];
	liv_min_locat_right_slope = buffer[liv_min_locat_right]
	                               - buffer[liv_min_locat];
	liv_min_locat_mult_slope = liv_min_locat_left_slope
		                           * liv_min_locat_right_slope;

	//��������MS20��б��
	liv_max_locat_left_MS20_slope = abs(buffer[liv_max_locat]
	                                - buffer[liv_max_locat_left_MS20]);
    liv_max_locat_right_MS20_slope = abs(buffer[liv_max_locat_right_MS20]
	                                - buffer[liv_max_locat]);
    liv_max_locat_MS20_plus_slope = liv_max_locat_left_MS20_slope
		                            + liv_max_locat_right_MS20_slope;

	liv_min_locat_left_MS20_slope = abs(buffer[liv_min_locat]
	                                - buffer[liv_min_locat_left_MS20]);
	liv_min_locat_right_MS20_slope = abs(buffer[liv_min_locat_right_MS20]
	                                - buffer[liv_min_locat]);
	liv_min_locat_MS20_plus_slope = liv_min_locat_left_MS20_slope
		                            + liv_min_locat_right_MS20_slope;

	//һ�������£���������
	if ((liv_max_locat_mult_slope <= 0) && (liv_min_locat_mult_slope <= 0))
	{
		if (liv_max_locat_MS20_plus_slope
			                      >=  0.80 * liv_min_locat_MS20_plus_slope)
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
	qrs_shift = bufferptr - liv_max_or_min_loc;
	if (qrs_shift < 0)
	{
		qrs_shift = qrs_shift + bufferlength;
	}
	//QRS��λ��
	qrs_position = samplepoint - qrs_shift + 1;

	//��ȡqrsbuffer
	ecgbufferbegin = liv_max_or_min_loc - MS125 + 1;
	if (ecgbufferbegin < 0)
	{
		ecgbufferbegin = ecgbufferbegin + bufferlength;
	}

	for (liv_loop = 0; liv_loop < MS280; ++liv_loop)
	{
		qrsbuffer[liv_loop] = buffer[ecgbufferbegin];
		if (++ecgbufferbegin >= bufferlength)
		{
			ecgbufferbegin = 0;
		}
	}

	for (liv_loop = 0; liv_loop < MS280; ++liv_loop)
	{
		qrsbufferorg[liv_loop] = qrsbuffer[liv_loop];
	}

	//ƽ���������Ƿ�Ч�����ã���δ��
	//���ε���smooth�����ô�ʱ��buf�������յ�����յ�
	qrsmooth(qrsbuffer);
	qrsmooth(qrsbuffer);

	//Ŀ�����ٱ���һ��ֻ�˹����ε�qrsbuf����
	for (liv_loop = 0; liv_loop < MS280; ++liv_loop)
	{
		qrsbuffer2[liv_loop] = qrsbuffer[liv_loop];
	}
	//�����
	onsetShift = qrsonset( qrsbuffer, liv_max, liv_min , &isoLoc);
	*qrsOnset = qrs_position - onsetShift + 1;
	//�յ���
	offsetShift = qrsoffset( qrsbuffer2, liv_max, liv_min, &stLoc );
	*qrsOffset = qrs_position + offsetShift;//�Ƿ�Ҫ��1��

	//��qrsbufferorg����ȵ�λ�����ֵ��ST�ε���ֵ
	//Ϊ����������Ӱ�죬����ȡһ��ֵ��ƽ��ֵ
	for (liv_iso = isoLoc - MS10;
		                    liv_iso <= isoLoc + MS10; ++liv_iso, ++isoTimes)
	{
		sumIso = sumIso + qrsbufferorg[liv_iso];
	}
	isoValue = sumIso / isoTimes;

	for (liv_st = stLoc; liv_st < MS280; ++liv_st, ++stTimes)
	{
		sumSt = sumSt + qrsbufferorg[liv_st];
	}
	stValue = sumSt / stTimes;
	*stDif = stValue - isoValue;



	/*onsetPtrShift = qrs_shift + onsetShift;//qrs����뵱ǰbufptr��ƫ��ֵ
	offsetPtrShift = qrs_shift - offsetShift;//qrs�յ��뵱ǰbufptr��ƫ��ֵ
	//��һ��qrs�յ��뵱ǰbufptr��ƫ��ֵ
	lastOffsetPtrShift = *lastOffshift + samplepoint - *lastSample;
	//��һ��qrs�յ��뵱ǰqrs����ƫ��ֵ
	onsetFlag = lastOffsetPtrShift - onsetPtrShift;

	//������qrs��̫�������
	//2013_1_25�˴�������bug��Ҫ��Ϊbufferlength-1
	if (onsetFlag >= bufferlength)
	{
		onsetFlag = bufferlength - 1;
	}
	//�ó���һ��qrs�յ��뵱ǰqrs�յ�ĳ���Ϊ��buf����Ч����
	bufLen = lastOffsetPtrShift - offsetPtrShift;//�Ƿ��1��//1706�޸�ȥ��+1

	//2013_5_6���Ĵ�������ĳ��Ⱥ�����
	//ǰ�������MS30������
	bufLen = bufLen + MS60;

	if (bufLen >= bufferlength)
	{
		bufLen = bufferlength - 1;
	}
	//�ҳ���buf�ĵ�һ���ڴ��ݽ�����ecgbuf�е�����
	newecgbufbegin = bufferptr - lastOffsetPtrShift;

	//2013_5_6���Ĵ�������ĳ��Ⱥ�����
	//��ǰһ������Ҳ���ݽ���
	newecgbufbegin = bufferptr - lastOffsetPtrShift - MS30;

	if (newecgbufbegin < 0)
	{
		newecgbufbegin = newecgbufbegin + bufferlength;
	}
	if (newecgbufbegin < 0)   //Ŀ���ǲ�ʹ��delay֮��̫�����������
	{
		newecgbufbegin = 0;
	}
	//�õ�newecgbuf
	for (newecgbuf_loop = 0; newecgbuf_loop < bufLen; ++newecgbuf_loop)
	{
		newecgbuf[newecgbuf_loop] = buffer[newecgbufbegin];
		if ( ++newecgbufbegin >= bufferlength )
		{
			newecgbufbegin = 0;
		}
	}
	//�����²�����buf
	pro_newecgbuf(outecgbuf, newecgbuf, bufLen, onsetFlag);

	//ע��Ҫ�������ȥ��ǰ���ϵ�MS60
	*bufLength = bufLen - MS60;//���˲������ݵ�������������д����
	*lastSample = samplepoint;//����ǰ����������ֵΪ��ȥ����
	*thiOffshift = *lastOffshift;//���ã�
	*lastOffshift = offsetPtrShift;//���˴�offset��ptr��ƫ�����洢����
	memset(newecgbuf, 0, sizeof(newecgbuf));//���buf
*/

	//���������pbuffer���
	//�ǲ��ǲ�Ӧ�ü�ȥMS125
	pbufferbegin = liv_max_or_min_loc  - onsetShift - MS220 + 1;
	if (pbufferbegin < 0)
	{
		pbufferbegin = pbufferbegin + bufferlength;
	}
	for (pbuf_loop = 0; pbuf_loop < MS220; ++pbuf_loop)
	{
		pbuffer[pbuf_loop] = buffer[pbufferbegin];
		if (++pbufferbegin >= bufferlength)
		{
			pbufferbegin = 0;
		}
	}

	return(qrs_position);
}
