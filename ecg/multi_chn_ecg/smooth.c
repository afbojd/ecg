/******************************************************************************

    �������� smooth()
	�﷨�� int smooth(FilterDataStruct *FilterDate, int datum)
	������ ȥ��Ƶ��������
	       ��ַ��̣�y(n) = b(1)*x(n) + b(2)*x(n-1) + ... + b(nb+1)*x(n-nb)
	                                  - a(2)*y(n-1) - ... - a(na+1)*y(n-na)
	�ο����ģ� ��Ҫ������������Online digital filters for biological signals 
	                           some fast designs for a small computer
	���ã� ��
	�����ã����������ϲ㺯��
	����������˲����ṹ������ʹ��˲�����ֵ
	��������� ��
	����ֵ�� ���˲��ź�
	������ �˲���������FIR��
    ע�⣺
	 
******************************************************************************/
#include <string.h>
#include "qrsdet.h"
#include "filter.h"
#include "qrsdet.h"


#include "smoothCof_250.h"


int smooth(FilterDataStruct *FilterDate, int datum)
{
	int fdatum = 0;//����˲�ֵ
	real tmp = 0;//�м���
	int loop = 0;//ѭ������
	real subPrecision = 0;//����˲�ֵ�õ��ı���

	//�˲���y(n) = b(1)*x(n) + b(2)*x(n-1) + ... + b(nb+1)*x(n-nb)
	//                       - a(2)*y(n-1) - ... - a(na+1)*y(n-na)
	tmp = smB[0] * (real)datum;
	
	for (loop = 1; loop < smoothBL; ++loop)
	{
		tmp += smB[loop] * (real)FilterDate->smoothDataIn[loop - 1];
	}
	

	//�м�ֵ���滻
	memmove(&FilterDate->smoothDataIn[1],
		    &FilterDate->smoothDataIn, (smoothBL - 1) * sizeof(int));
	FilterDate->smoothDataIn[0] = datum;


	//��������Ͷ���Ϊint��ʱ���ʵ�����������
	fdatum = (int)(tmp);
	subPrecision = tmp - fdatum;
	if (subPrecision >= 0.5000)
	{
		fdatum = fdatum + 1;
	}
	if (subPrecision <= -0.5000)
	{
		fdatum = fdatum - 1;
	}

	return fdatum;
}