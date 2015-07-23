/******************************************************************************

    �������� smooth_5_points()
	�﷨�� int smooth_5_points(int datum, int init)
	������ ȥ��Ƶ��������
	       ��ַ��̣�Y(n) = Y(n-1) + ��359 / 360�� * X(n) - X(n-1) + 
		   ��1 / 360�� * X(n-360)
	�ο����ģ� 
	���ã� ��
	�����ã����������ϲ㺯��
	������������˲��źż���ʼ����־��
	��������� ��
	����ֵ�� ���˲��ź�
	������ �ҵ���һֱ��matlab�汾��һ�µ�ԭ��matlab������м����Ǹ����ͣ�
	       ���ڴξ�Ϊint�ͣ�����ѡ�������ͻ�Ҫ���ǣ�����������
    ע�⣺ ����ע������
	       1���˺�������matlab�汾һ�������Դ�����������ʵ��źţ�
		   ����ֻ�ܴ��������Ϊ360Hz����Ƶ����Ϊ60Hz���ź�
		   �����MIT���ݿ��ź�,�˴��Ľ�ֹ
		   2������������������⣬�м����϶�Ϊ�������ͣ��������������������
		   ������ǰ�����˳���ۺϿ���
	 
******************************************************************************/

#include <string.h>

int smooth_5_points(int datum, int init)
{
	static double lastFdatum;//��Y(n-1)
	static int data[5], ptr;
	double fdatum = 0;//���������
	int intfdatum = 0;//���������
	double subPrecision = 0;//��߾��ȵ��м���
	int ptrMinusOne = 0, ptrMinusOrder = 0;//����
	double medineOne = 0, medineTwo = 0;
	int memmovelen = 4 * sizeof(int);
	//��ʼ��
	if (init)
	{
		lastFdatum = 0;
		for (ptr = 0; ptr < 5; ++ptr)
		{
			data[ptr] = 0;
		}
		ptr = 0;
		return 0;
	}
	memmove(data, &data[1], memmovelen);
	data[4] = datum;
	fdatum = data[0] + data[1] + data[2] + data[3] + data[4];//��ַ���
	fdatum = fdatum / 5;
	lastFdatum = fdatum;

	intfdatum = (int)(fdatum);
	//��������Ͷ���Ϊint��ʱ���ʵ�����������
	subPrecision = fdatum - intfdatum;
	if (subPrecision >= 0.5000)
	{
		intfdatum = intfdatum + 1;
	}
	if (subPrecision <= -0.5000)
	{
		intfdatum = intfdatum - 1;
	}
	return intfdatum;
}