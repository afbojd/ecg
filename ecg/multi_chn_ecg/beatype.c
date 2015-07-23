/******************************************************************************

    �������� beatype()
	�﷨�� char beatype( int qrsposition, int init )
	������ ����RR���ڽ����ķ���
	       �ο����ģ�
		   1��An arrhythmia classification system based on the
		      RR-interval signal
		   2��Arrhythmia classification using the RR-interval duration signal
	���ã� ��׼���е�abs������ֵ������     
	�����ã��ϲ㺯������������
	��������� QRS���ϲ�λ�ü��Ƿ��ʼ������
	��������� ��һʱ��QRS������������
	����ֵ�� ��һʱ��QRS������������
	         N���������ģ���V�������粫���ģ���A�������粫���ģ�
	������ ��init��0ʱ��һЩ������ʼֵ
	 
******************************************************************************/

#include <math.h>
#include "qrsdet.h"

char beatype( int qrsposition, int init ) 
{
	static int rr1 = 0, rr2 = 0, rr3 = 0; //RR����
	static int qrsp1 = 0, qrsp2 = 0, qrsp3 = 0, qrsp4 = 0; //QRS��λ��
	int condone, condtwo, condthree; //����
	int condfour, condfive, condsix;
	char beatcategory; //��������

	if (init)
	{
		rr1 = rr2 = rr3 = 0;
		qrsp1 = qrsp2 = qrsp3 = qrsp4 = 0;
	}

	qrsp1 = qrsp2;
	qrsp2 = qrsp3;
	qrsp3 = qrsp4;
	qrsp4 = qrsposition;
	
	if (qrsp1 && qrsp2 && qrsp3 && qrsp4)
	{
		rr1 = qrsp2 - qrsp1;
		rr2 = qrsp3 - qrsp2;
		rr3 = qrsp4 - qrsp3;
		condone = (rr2 <= (0.88 * rr1)) && ((0.95 * rr3) >= rr1);
		condtwo = (rr2 + rr3) < 2 * rr1;  
		condthree = (rr2 + rr3) >= 2 * rr1;

		condfour = (abs(rr2 - rr1) < MS200) && ((rr1 < MS750) 
			&& (rr2 < MS750)) && (rr3 > 0.6 * (rr1 + rr2));
		condfive = (abs(rr2 - rr3) < MS200) && ((rr3 < MS750)
			&& (rr2 < MS750)) && (rr1 > 0.6 * (rr3 + rr2));
		condsix = (rr2 < MS600) && ((1.8 * rr2 < rr1) || (1.8 * rr2 < rr3));

		//���������粫���ȼ����ڷ����粫
		beatcategory = 'N';
		if (condone && condthree)
		{
			beatcategory = 'V';
		}
		if (condone && condtwo)
		{
				beatcategory = 'A';
		}
		if (condfour || condfive || condsix)
		{
				beatcategory = 'V';
		}

	}
	else
	{
		beatcategory = 'Q';
	}

	return(beatcategory);
}