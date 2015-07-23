/******************************************************************************
    �ļ���: multqrsfilter.c
	�﷨�� int multqrsfilter( qrsFilterStruct* qrsFilterStr, int datum )
	������ ���ļ��а�������Ϊ���QRS��������˲�����������qrsfilt()��
	deriv1()�����ᱻ�����������ã��������������ڲ�����
	�����б� 
	    1��multqrsfilter() ���ļ�����������������������������ĵ��źŴ���������ֵ
	    2��multlpfilt()    ��ͨ�˲�����
	    3��multhpfilt()    ��ͨ�˲�����
	    4��multderiv1()    ��ֺ���
	    5��multderiv2()    ��ֺ���
	    6��multmvwint()    ���ִ���ͺ���
	    7��abs()       �����Դ�����ֵ����
	�����ã� qrsdet.c
	��������� 
	��������� 
	����ֵ�� 
	������ 
	ע�⣺ 
******************************************************************************/
#include <math.h>
#include "qrsdet.h"
#include "qrsdetstruct.h"

/******************************************************************************

    �������� qrsfilter()
	�﷨�� int qrsfilter(qrsFilterStruct* qrsFilterStr, int datum)
	������ �˲��ܺ��������ø��˲������õ������˲�������ź�ֵ
	���ã� 
	       1��lpfilt()    ��ͨ�˲�����
	       2��hpfilt()    ��ͨ�˲�����
	       3��deriv1()    ��ֺ���
	       4��deriv2()    ��ֺ���
	       5��mvwint()    ���ִ���ͺ���
	       6��abs()       �����Դ�����ֵ����
	�����ã� qrsdet()     QRS����⺯��
	��������� ԭʼ�ź������Ƿ��ʼ������
	��������� �����˲���õ����ź�ֵ
	����ֵ�� �����˲���õ����ź�ֵ
	������ ��init��0ʱ���˲������漰��̬����������
	 
******************************************************************************/

int multqrsfilter( qrsFilterStruct* qrsFilterStr, int datum )
{
	int fdatum ;

	fdatum = multlpfilt( qrsFilterStr, datum );   //��ͨ�˲�
	fdatum = multhpfilt( qrsFilterStr, fdatum );  //��ͨ�˲�
	fdatum = multderiv2( qrsFilterStr, fdatum );  //����˲�
	fdatum = abs( fdatum );    //ȡ����ֵ
	fdatum = multmvwint( qrsFilterStr, fdatum );  //���ִ����
	
	return(fdatum);
}


/******************************************************************************

    �������� lpfilt()
	�﷨: int lpfilt( qrsFilterStruct* qrsFilterStr, int datum)
	����: ��ͨ�˲�������ʱ���ֱ��ʽΪ
	      y[n] = 2*y[n-1] - y[n-2] + x[n] - 2*x[t-24 ms] + x[t-48 ms]
		  y[n] = y[n] / ((LPBUFFER_LGTH * LPBUFFER_LGTH) / 4)
	���ã� ��
	�����ã�  qrsfilter()
	��������� ����ֵ���Ƿ��ʼ������
	��������� �˲����ź���
	����ֵ�� �˲����ź���
	������ �˲��ӳ�Ϊ��(LPBUFFER_LGTH / 2) - 1������

******************************************************************************/

int multlpfilt( qrsFilterStruct* qrsFilterStr, int datum )
{
	int y0 ;
	int output, halfPtr ;
	
	// Use halfPtr to index
	halfPtr = qrsFilterStr->qrsLpStr.lptr - (LPBUFFER_LGTH / 2) ;	
	
	if (halfPtr < 0)							// to x[n-6].
	{
		halfPtr += LPBUFFER_LGTH;
	}

	//y[n] = 2*y[n-1] - y[n-2] + x[n] - 2*x[t-24 ms] + x[t-48 ms]������仰
	//�ķ��룬����ĳ˳�����ͨ����Ϊʵ�ֵ�
	y0 = (qrsFilterStr->qrsLpStr.dataoutone << 1) - 
		  qrsFilterStr->qrsLpStr.dataouttwo + datum - 
		  (qrsFilterStr->qrsLpStr.datain[halfPtr] << 1) + 
		  qrsFilterStr->qrsLpStr.datain[qrsFilterStr->qrsLpStr.lptr] ; 

	qrsFilterStr->qrsLpStr.dataouttwo = qrsFilterStr->qrsLpStr.dataoutone;                                                         
	qrsFilterStr->qrsLpStr.dataoutone = y0;
	output = y0 / ((LPBUFFER_LGTH * LPBUFFER_LGTH) / 4);

	// Stick most recent sample into
	qrsFilterStr->qrsLpStr.datain[qrsFilterStr->qrsLpStr.lptr] = datum ;			
	// the circular buffer and update
	if (++qrsFilterStr->qrsLpStr.lptr == LPBUFFER_LGTH)	
	{
		qrsFilterStr->qrsLpStr.lptr = 0;             // the buffer pointer.
	}	
	return(output);
}

/******************************************************************************

    �������� hpfilt()
	�﷨�� int hpfilt( qrsFilterStruct* qrsFilterStr, int datum )
	������ ��ͨ�˲�������ʱ���ֱ��ʽΪ
	       y[n] = y[n-1] + x[n] - x[n-128 ms] 
           z[n] = x[n-64 ms] - y[n]/HPBUFFER_LGTH
	���ã� ��
	�����ã� qrsfilter()
	��������� ����ֵ���Ƿ��ʼ������
	��������� �����˲���õ����ź�ֵ
	����ֵ�� �����˲���õ����ź�ֵ
	������ �˲��ӳ�Ϊ��(HPBUFFER_LGTH - 1) / 2������

******************************************************************************/

int multhpfilt( qrsFilterStruct* qrsFilterStr, int datum )
{
	int z, halfPtr ;

	qrsFilterStr->qrsHpStr.dataoutone += datum - 
		          qrsFilterStr->qrsHpStr.datain[qrsFilterStr->qrsHpStr.hptr];

	halfPtr = qrsFilterStr->qrsHpStr.hptr - (HPBUFFER_LGTH / 2) ;
	
	if (halfPtr < 0)
	{
		halfPtr += HPBUFFER_LGTH;
	}
	z = qrsFilterStr->qrsHpStr.datain[halfPtr] - 
		           (qrsFilterStr->qrsHpStr.dataoutone / HPBUFFER_LGTH);

	qrsFilterStr->qrsHpStr.datain[qrsFilterStr->qrsHpStr.hptr] = datum ;
	
	if (++qrsFilterStr->qrsHpStr.hptr == HPBUFFER_LGTH)
	{
		qrsFilterStr->qrsHpStr.hptr = 0;
	}

	return(z);
}


/******************************************************************************

    �������� deriv1()��deriv2() 
	�﷨�� int deriv1( qrsFilterStruct* qrsFilterStr, int datum ) or 
	       int deriv2( qrsFilterStruct* qrsFilterStr, int datum )
	������ ��ֺ�����������ʵ�ֹ���һ����֮����д����������Ϊderiv1�ᱻ��������
	       ���ã���ֹ�ڲ���̬�����ֵ��������������
		   ʱ���ֱ��ʽΪ:y[n] = x[n] - x[n - 10ms]
	���ã� ��
	�����ã� deriv2()��qrsfilter()���ã�deriv1()��qrsdet()����
	��������� ����ֵ���Ƿ��ʼ������
	��������� �����˲���õ����ź�ֵ
	����ֵ�� �����˲���õ����ź�ֵ
	������ �˲��ӳ�Ϊ��DERIV_LENGTH / 2������

******************************************************************************/

int multderiv1( qrsFilterStruct* qrsFilterStr, int datum )
{
	int y;

	y = datum - 
		qrsFilterStr->qrsDerStr[0].datain[qrsFilterStr->qrsDerStr[0].derptr];

	qrsFilterStr->qrsDerStr[0].datain[qrsFilterStr->qrsDerStr[0].derptr] = 
		                                                         datum;
	
	if (++qrsFilterStr->qrsDerStr[0].derptr == DERIV_LENGTH)
	{
		qrsFilterStr->qrsDerStr[0].derptr = 0;
	}
	return(y) ;
}

int multderiv2( qrsFilterStruct* qrsFilterStr, int datum )
{
	int y;

	y = datum - 
		qrsFilterStr->qrsDerStr[1].datain[qrsFilterStr->qrsDerStr[1].derptr];

	qrsFilterStr->qrsDerStr[1].datain[qrsFilterStr->qrsDerStr[1].derptr] = 
		datum;

	if (++qrsFilterStr->qrsDerStr[1].derptr == DERIV_LENGTH)
	{
		qrsFilterStr->qrsDerStr[1].derptr = 0;
	}
	return(y) ;
}


/******************************************************************************

    �������� mvwint()
	�﷨�� int mvwint( qrsFilterStruct* qrsFilterStr, int datum )
	������ ���ִ���ͺ���
	���ã� ��
	�����ã� qrsfilter()
	��������� ����ֵ���Ƿ��ʼ������
	��������� �����˲���õ����ź�ֵ
	����ֵ�� �����˲���õ����ź�ֵ
	������ 

******************************************************************************/

int multmvwint( qrsFilterStruct* qrsFilterStr, int datum )
{
	int output;

	qrsFilterStr->qrsMvStr.sum += datum;
	qrsFilterStr->qrsMvStr.sum -= 
		        qrsFilterStr->qrsMvStr.datain[qrsFilterStr->qrsMvStr.mvptr];
	//������һ��ѭ��
	qrsFilterStr->qrsMvStr.datain[qrsFilterStr->qrsMvStr.mvptr] = datum;                 

	if (++qrsFilterStr->qrsMvStr.mvptr == WINDOW_WIDTH)
	{
		qrsFilterStr->qrsMvStr.mvptr = 0;
	}

	//������һ����ֹ���/���������ж�����
	if ((qrsFilterStr->qrsMvStr.sum / WINDOW_WIDTH) > 32000)    
	{
			output = 32000;
	}
	else
	{
		output = qrsFilterStr->qrsMvStr.sum / WINDOW_WIDTH ;
	}

	return(output);
}