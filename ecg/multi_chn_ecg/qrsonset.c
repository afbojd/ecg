/******************************************************************************

    �������� qrsonset()
	�﷨�� int qrsonset( int *buf, int maxvalue, int minvalue, int *isopoint )
	������ �˺���������Ѱ��QRS���ϲ�����㣬�ڲ��������нǶ���С����Ϊ��
	       ���ϲ���㣬�������б����С��Ϊ���
	�ο����ģ� ��Ҫ����������ƪ���ģ����Ļ���һ��
		   1��Electrocardiogram signal preprocessing for automatic
		      detection of QRS boundaries
		   2��Q-onset and T-end delineation: assessment of the performance
		      of an automated method with the use of a reference database
	���ã� ��׼���е�abs������ֵ��������acos�������ң�����
	�����ã�qrsandonoffset()����
	��������� qrsbuffer����ǰQRS����������Сֵ������qrsandonoffset()����
	��������� QRS���ϲ���������������ƫ�ƣ����ȵ�λ����qrsbuf�е�
	           λ��isopoint
	����ֵ�� QRS���ϲ���������������ƫ��
	������ ���뽫���ֵ����Сֵ��Ӧ������Ҳ���ݽ������Ƚ����ߴ�С���Ӷ�ȷ��
	       ���Ż���ʱ�䣬�����ǵ��ϸ��ӣ���Щ�������Ҳ��һ��ѡȡ��׼ȷ������
		   ������MS120���Ҳ��ң����ͨ������취����
		   ���ڴ�����ź���ԭʼ�źţ������˲��źţ��źŻ����ٴ��ڸ��ţ�����
		   ��������С�ǶȻ���б����С��Ч���������ر�ã�������ʱ��֪������
		   ����Ч�����ã�Ҫ���Ժ�����ǰ�����Ƚ�Ч��
		   Ŀǰ���õ���б�ʺͣ���smoothʱ��
		   ���������ƽ����������׼����Ҫ��
		   ��С�Ƕȵ����ۻ������Ǳ�б�ʺ͵ĺ�һЩ������smooth����С�Ƕȸ���
		   �յ������51
	 
******************************************************************************/

#include <math.h>
#include "qrsdet.h"

int qrsmooth(int *buf);

int qrsonset( int *buf, int maxvalue, int minvalue, int *isopoint )
{
	int crit = 0;//�Ƚϱ�׼
	int loopone = 0, looptwo = 0, loopthree = 0;//����ѭ���Ŀ����������꣩
	int conTimeOne = 0, conTimeTwo = 0;//����ʱ��
	int qLeft = 0, qRight = 0;//���ղ�������
	int leftMS10 = 0, rightMS10 = 0;//���ң���MS10�㣬��б�ʺ���������һ��
	int peakFlagone = 0, peakFlagtwo = 0;//Ѱ��peak������������������ż���ֵ��
	int slopeFlagone = 0, slopeFlagtwo = 0;//Ѱ��slope���������������ͬ�ϣ�
	//�Ƕ���ر���
	double angleBuf[MS125] = {0};//�ǶȻ���
	int angPtr = 0;//�ǶȻ�������
    double bOrdY = 0, cOrdY = 0, bcOrdY = 0; //�����������ֵ
	double edgeA = 0, edgeB = 0, edgeC = 0;//���߳�
	double angleA = 0, cosA = 0;//A�ĽǶȺ�����ֵ
	double minAngle = 0;//��С�Ƕ�Ϊһtempֵ
	int minAngleLoc = 0;//��С�Ƕ�����
	//����б�ʣ�Ѱ����С��
	int slopeOne = 0, slopeTwo = 0, slopePlus = 0;
	int slopeBuf[MS125] = {0};//б�ʺͻ���
	int slopePtr = 0;//б�ʺͻ�������
	int minSlope = 0;//��Сб�ʺ�Ϊһtempֵ
	int minSlopeLoc = 0;//��Сб�ʺ�����
	
	int loc = 0;//һ����������ֵ��һ������ѭ������ֵ
	int onsetShift = 0;//����ֵ����������ƫ����

	crit = 0.01 * (maxvalue - minvalue);//����������ɵ�

	//Ѱ��qLeft
	//Ϊ��ֹQRSΪ�����������ſ�ΪMS110
	for (loopone = 1; loopone < MS110; ++loopone)
	{
		if (abs(buf[loopone] - buf[loopone - 1]) <= crit)
		{
			conTimeOne = conTimeOne + 1;
		}
		else
		{
			conTimeOne = 0;
		}

		//����ʱ������һ���������������������
		if (conTimeOne >= MS40)
		{
			qLeft = loopone;//�Ƿ��MS15���Ժ�Ҫ��Ч��
			break;
		}
	}
	if (qLeft <= 0)
	{
		qLeft = MS50;//�����������ĵ�ʱ��ֱ�ӵ�MS50��
	}

	*isopoint = qLeft;

	//Ѱ��qRight
	for (looptwo = qLeft; looptwo < MS125; ++looptwo)
	{
		//��������break��һ����peak��һ����slope
		//��peak
		leftMS10 = looptwo - MS10;
		rightMS10 = looptwo + MS10;
		if (leftMS10 < 0)
		{
			leftMS10 = 0;
		}
		//����������ֵ
		peakFlagone = buf[looptwo] - buf[leftMS10];
		peakFlagtwo = buf[looptwo] - buf[rightMS10];
		if (peakFlagone * peakFlagtwo > 0)
		{
			peakFlagone = abs(peakFlagone);
			peakFlagtwo = abs(peakFlagtwo);
			//��׼�ɸģ������2*crit
			if ((peakFlagone >= 2 * crit) &&(peakFlagtwo >= 2 * crit))
			{
				qRight = looptwo;
				break;
			}
		}

		//��slope
		//����������ֵ
		slopeFlagone = buf[looptwo + 1] - buf[looptwo];
		slopeFlagtwo = buf[looptwo + 2] - buf[looptwo + 1];
		//����ķ��������qLeft����
		if (slopeFlagone * slopeFlagtwo > 0)
		{
			slopeFlagone = abs(slopeFlagone);
			slopeFlagtwo = abs(slopeFlagtwo);
			//��׼�ɸģ������2*crit
			if ((slopeFlagone >= 2 * crit) && (slopeFlagtwo >= 2 * crit))
			{
				conTimeTwo = conTimeTwo + 1;
			}
			else
			{
				conTimeTwo = 0;
			}
			if (conTimeTwo >= MS20)
			{
				qRight = looptwo - MS20;
				break;
			}
		}
	}
	if (qRight == 0)
	{
		qRight = MS100;//�����������ĵ�ʱ��ֱ�ӵ�MS100��
	}

	if (qLeft >= qRight)
	{
		onsetShift = MS125 - qLeft;
		return(onsetShift);
	}

	//qrsmooth(buf);
	qrsmooth(buf);
	qrsmooth(buf);
	qrsmooth(buf);
	qrsmooth(buf);

	//�ɿ����ҽǶ���С���б����С�㣬��һ���Ǹ���׼ȷ�͸��õ���
	//�ҽǶ���С��
	for (loopthree = qLeft; loopthree <= qRight; ++loopthree)
	{
		leftMS10 = loopthree - MS20;//�Ƿ��ȥMS20��׼ȷ��
		rightMS10 = loopthree + MS20;
		if (leftMS10 < 0)
		{
			leftMS10 = 0;
		}
		if (loopthree == leftMS10)
		{
			angleBuf[angPtr] = -1;
		    ++angPtr;
			continue;
		}

		//����������ͱ߳�
		bOrdY = buf[rightMS10] - buf[loopthree];
		cOrdY = buf[leftMS10] - buf[loopthree];
		bcOrdY = buf[rightMS10] - buf[leftMS10];
		bcOrdY = 4 + bcOrdY * bcOrdY;
		edgeA = sqrt(bcOrdY);
		cOrdY = 1 + cOrdY * cOrdY;
		edgeB = sqrt(cOrdY);
		bOrdY = 1 + bOrdY * bOrdY;
		edgeC = sqrt(bOrdY);
		//�������Ҷ����������ֵ
		cosA = (edgeB * edgeB + edgeC * edgeC - edgeA * edgeA)
			                                   /(2 * edgeB * edgeC);
		angleA = acos(cosA);
		angleBuf[angPtr] = angleA;
		++angPtr;
	}

	minAngle = angleBuf[0];
	minAngleLoc = 0;
	angPtr = angPtr - MS10;
	angPtr = (angPtr > 0) ? angPtr : 1;
	if (angPtr > 2)
	{
		for (loc = 2; loc < angPtr; ++loc)
		{
			if (minAngle > angleBuf[loc])
			{
				minAngle = angleBuf[loc];
				minAngleLoc = loc;
			}
		}
	}
	else
	{
		minAngleLoc = 0;
	}

	minAngleLoc = minAngleLoc + qLeft;
	//minAngleLoc = qRight;//������

	onsetShift = MS125 - minAngleLoc;
	return(onsetShift);
	
	////������б�ʺ���С�㣬ֱ���������е�һЩ����
	//for (loopthree = qLeft; loopthree <= qRight; ++loopthree)
	//{
	//	leftMS10 = loopthree - 1;
	//	rightMS10 = loopthree + 1;
	//	if (leftMS10 < 0)
	//	{
	//		leftMS10 = 0;
	//	}
	//	slopeOne = abs(buf[loopthree] - buf[leftMS10]);
	//	slopeTwo = abs(buf[rightMS10] - buf[loopthree]);
	//	slopePlus = slopeOne + slopeTwo;
	//	slopeBuf[slopePtr] = slopePlus;
	//	++slopePtr;
	//}
	//minSlope = slopeBuf[0];
	//minSlopeLoc = 0;

	//for (loc = 0; loc < slopePtr; ++loc)
	//{
	//	if (minSlope > slopeBuf[loc])
	//	{
	//		minSlope = slopeBuf[loc];
	//		minSlopeLoc = loc;
	//	}
	//}

	//minSlopeLoc = minSlopeLoc + qLeft;
	////minSlopeLoc = qLeft;//������

	//onsetShift = MS125 - minSlopeLoc;
	//return(onsetShift);

}