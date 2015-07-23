/******************************************************************************

    �������� hpFilter()
	�﷨�� int hpFilter(int datum, int init)
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
	������ �˲���������IIR����һ�֣��Զ���ΪI�ͣ�����250Hzʱ���˲����˵õĽ��
	       �����˲������ȶ������⣬����ϵ����ΪFIR�͵ģ�����������Է�ĸ��һ
	       �ж�������ʹ�ô��˲���Ҳ����FIR�����˲���
    ע�⣺
	 
******************************************************************************/
#include <string.h>
#include "qrsdet.h"
#include "filter.h"

#include "hpFilterCof_250.h"


int hpFilter(FilterDataStruct *FilterDate, int datum)
{
	int fdatum = 0;//����˲�ֵ
	real tmp = 0;//�м���
	int loop = 0;//ѭ������
	real subPrecision = 0;//����˲�ֵ�õ��ı���

	//�˲���y(n) = b(1)*x(n) + b(2)*x(n-1) + ... + b(nb+1)*x(n-nb)
	//                       - a(2)*y(n-1) - ... - a(na+1)*y(n-na)
	tmp = hpNum[0] * (real)datum;

	for (loop = 1; loop < hpNL; ++loop)
	{
		tmp += hpNum[loop] * (real)FilterDate->hpDataIn[loop - 1];
	}

	

	//�м�ֵ���滻
	memmove(&FilterDate->hpDataIn[1],
		&FilterDate->hpDataIn, (hpNL - 1) * sizeof(int));
	FilterDate->hpDataIn[0] = datum;

	if (hpDL > 1)
	{
		for (loop = 1; loop < hpDL; ++loop)
		{
			tmp -= hpDen[loop] * FilterDate->hpDataOut[loop - 1];
		}
		memmove(&FilterDate->hpDataOut[1], 
			&FilterDate->hpDataOut, (hpDL - 1) * sizeof(real));
		FilterDate->hpDataOut[0] = tmp;
	}

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