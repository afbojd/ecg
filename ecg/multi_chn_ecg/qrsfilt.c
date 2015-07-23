/******************************************************************************
    �ļ���: qrsfilt.c
	������ ���ļ��а�������Ϊ���QRS��������˲�����������qrsfilt()��
	deriv1()�����ᱻ�����������ã��������������ڲ�����
	�����б�
	1��qrsfilter() ���ļ�����������������������������ĵ��źŴ���������ֵ
	2��lpfilt()    ��ͨ�˲�����
	3��hpfilt()    ��ͨ�˲�����
	4��deriv1()    ��ֺ���
	5��deriv2()    ��ֺ���
	6��mvwint()    ���ִ���ͺ���
	7��abs()       �����Դ�����ֵ����
******************************************************************************/

#include <math.h>
#include "qrsdet.h"
#include <stdio.h>
#include <stdlib.h>
// �ڲ���������
int lpfilt( int datum ,int init );
int hpfilt( int datum, int init );
int deriv1( int x0, int init );
int deriv2( int x0, int init );
int mvwint( int datum, int init);

/******************************************************************************

    �������� qrsfilter()
	�﷨�� int qrsfilter(int datum, int init)
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

int qrsfilter( int datum, int init )
{
	int fdatum ;

	//��ʼ���˲���
	if(init)
	{
	    hpfilt( 0, 1 ) ;
		lpfilt( 0, 1 ) ;
		mvwint( 0, 1 ) ;
		deriv1( 0, 1 ) ;
		deriv2( 0, 1 ) ;
	}

	fdatum = lpfilt( datum, 0 );    //��ͨ�˲�
	fdatum = hpfilt( fdatum, 0 );    //��ͨ�˲�
	fdatum = deriv2( fdatum, 0 );    //����˲�
	fdatum = abs( fdatum );    //ȡ����ֵ
	fdatum = mvwint( fdatum, 0 );    //���ִ����

	return(fdatum);
}


//������Ϊ�˿�����ֵ����һЩ��������������ļ���������Щ������
//int qrsfilter( int datum, int init )
//{
//	int fdatum ;
//	FILE *hp, *lp, *de, *ab, *mv;
//	int ihp, ilp, ide, iab, imv;
//	ihp = 0;
//	ilp = 0;
//	ide = 0;
//	iab = 0;
//	imv = 0;
//	//��ʼ���˲���
//	if(init)
//	{
//		ihp = hpfilt( 0, 1 ) ;
//		hp = fopen( "hp.txt", "a+" );
//		fprintf(hp, "%d\n", ihp);
//		fclose( hp );
//
//		ilp = lpfilt( 0, 1 ) ;
//		lp = fopen( "lp.txt", "a+" );
//		fprintf(lp, "%d\n", ilp);
//		fclose( lp );
//
//		imv = mvwint( 0, 1 ) ;
//		mv = fopen( "mv.txt", "a+" );
//		fprintf(hp, "%d\n", imv);
//		fclose( mv );
//
//		deriv1( 0, 1 ) ;
//
//		ide = deriv2( 0, 1 ) ;
//		de = fopen( "de.txt", "a+" );
//		fprintf(de, "%d\n", ide);
//		fclose( de );
//	}
//
//	ilp = lpfilt( datum, 0 );    //��ͨ�˲�
//	lp = fopen( "lp.txt", "a+" );
//	fprintf(lp, "%d\n", ilp);
//	fclose( lp );
//
//	ihp = hpfilt( ilp, 0 );    //��ͨ�˲�
//	hp = fopen( "hp.txt", "a+" );
//	fprintf(hp, "%d\n", ihp);
//	fclose( hp );
//
//	ide = deriv2( ihp, 0 );    //����˲�
//	de = fopen( "de.txt", "a+" );
//	fprintf(de, "%d\n", ide);
//	fclose( de );
//
//	iab = abs( ide );    //ȡ����ֵ
//	ab = fopen( "ab.txt", "a+" );
//	fprintf(ab, "%d\n", iab);
//	fclose( ab );
//
//	imv = mvwint( iab, 0 );    //���ִ����
//	mv = fopen( "mv.txt", "a+" );
//	fprintf(hp, "%d\n", imv);
//	fclose( mv );
//
//	fdatum = imv;
//
//	return(fdatum) ;
//}

/******************************************************************************

    �������� lpfilt()
	�﷨: int lpfilt( int datum, int init)
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

int lpfilt( int datum, int init)
{
    static long y1 = 0, y2 = 0 ;
	static int data[LPBUFFER_LGTH], ptr = 0 ;
	long y0 ;
	int output, halfPtr ;
	if (init)
	{
		for(ptr = 0; ptr < LPBUFFER_LGTH; ++ptr)
		{
			data[ptr] = 0;
		}

		y1 = y2 = 0 ;
		ptr = 0 ;
	}
	halfPtr = ptr-(LPBUFFER_LGTH/2) ;	// Use halfPtr to index

	if (halfPtr < 0)							// to x[n-6].
	{
		halfPtr += LPBUFFER_LGTH;
	}

	//y[n] = 2*y[n-1] - y[n-2] + x[n] - 2*x[t-24 ms] + x[t-48 ms]������仰
	//�ķ��룬����ĳ˳�����ͨ����Ϊʵ�ֵ�
	y0 = (y1 << 1) - y2 + datum - (data[halfPtr] << 1) + data[ptr] ;
	y2 = y1;
	y1 = y0;
	output = y0 / ((LPBUFFER_LGTH*LPBUFFER_LGTH)/4);
	data[ptr] = datum ;			// Stick most recent sample into

	if (++ptr == LPBUFFER_LGTH)	// the circular buffer and update
	{
		ptr = 0;                 // the buffer pointer.
	}

	return(output) ;
}


/******************************************************************************

    �������� hpfilt()
	�﷨�� int hpfilt( int datum, int init )
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

int hpfilt( int datum, int init )
{
	static long y=0 ;
	static int data[HPBUFFER_LGTH], ptr = 0 ;
	int z, halfPtr ;

	if (init)
	{
		for(ptr = 0; ptr < HPBUFFER_LGTH; ++ptr)
		{
			data[ptr] = 0;
		}

		ptr = 0 ;
		y = 0 ;
	}

	y += datum - data[ptr];
	halfPtr = ptr - (HPBUFFER_LGTH / 2) ;

	if (halfPtr < 0)
	{
		halfPtr += HPBUFFER_LGTH;
	}
	z = data[halfPtr] - (y / HPBUFFER_LGTH);
	data[ptr] = datum ;

	if (++ptr == HPBUFFER_LGTH)
	{
		ptr = 0;
	}

	return( z );
}

/******************************************************************************

    �������� deriv1()��deriv2()
	�﷨�� int deriv1( int x, int init ) or int deriv2( int x, int init )
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

int deriv1( int x, int init )
{
	static int derBuff[DERIV_LENGTH], derI = 0 ;
	int y ;

	if (init != 0)
	{
		for(derI = 0; derI < DERIV_LENGTH; ++derI)
		{
			derBuff[derI] = 0;
		}

		derI = 0 ;
		return(0) ;
	}

	y = x - derBuff[derI] ;
	derBuff[derI] = x ;

	if (++derI == DERIV_LENGTH)
	{
		derI = 0;
	}

	return(y) ;
}

int deriv2( int x, int init )
{
	static int derBuff[DERIV_LENGTH], derI = 0 ;
	int y ;

	if (init != 0)
	{
		for(derI = 0; derI < DERIV_LENGTH; ++derI)
		{
			derBuff[derI] = 0;
		}

		derI = 0 ;
		return(0) ;
	}

	y = x - derBuff[derI] ;
	derBuff[derI] = x ;

	if (++derI == DERIV_LENGTH)
	{
		derI = 0;
	}

	return(y) ;
}




/******************************************************************************

    �������� mvwint()
	�﷨�� int mvwint( int datum, int init )
	������ ���ִ���ͺ���
	���ã� ��
	�����ã� qrsfilter()
	��������� ����ֵ���Ƿ��ʼ������
	��������� �����˲���õ����ź�ֵ
	����ֵ�� �����˲���õ����ź�ֵ
	������

******************************************************************************/

int mvwint( int datum, int init )
{
	static long sum = 0 ;
	static int data[WINDOW_WIDTH], ptr = 0 ;
	int output;

	if (init)
	{
		for (ptr = 0; ptr < WINDOW_WIDTH ; ++ptr)
		{
			data[ptr] = 0;
		}
		sum = 0 ;
		ptr = 0 ;
	}

	sum += datum ;
	sum -= data[ptr] ;
	data[ptr] = datum ;                 //������һ��ѭ��

	if (++ptr == WINDOW_WIDTH)
	{
		ptr = 0 ;
	}

	if ((sum / WINDOW_WIDTH) > 32000)    //������һ����ֹ���/���������ж�����
	{
			output = 32000 ;
	}
	else
	{
		output = sum / WINDOW_WIDTH ;
	}

	return(output) ;
}
