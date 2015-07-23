/******************************************************************************

    �ļ���: qrsdetstruct.h
    ������ ��ͷ�ļ�����QRS���ϲ����Һ�����Ҫ�õ����˲����Ľṹ�弰���ϲ�����
	       �����м����Ľṹ��
	���ã� qrsdet.h
	�����ã� run.h
	������ ��
	�����б� ��

******************************************************************************/
#ifndef qrsdetstruct_h__
#define qrsdetstruct_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "qrsdet.h"
//����ֵ���飨��ƽ�����飩���ȶ���Ϊ�꣬��������Ը���
#define arrayLen  8

//��ͨ�˲��ṹ��
typedef struct
{
	int dataoutone;//y��n-1������ʼ��Ϊ0
	int dataouttwo;//y��n-2������ʼ��Ϊ0
	int datain[LPBUFFER_LGTH];//����ֵ���У���ʼ��Ϊ0
	int lptr;//�����±꣬��ʼ��Ϊ0
}qrsLpStruct;

//��ͨ�˲��ṹ��
typedef struct
{
	int dataoutone;//y��n-1������ʼ��Ϊ0
	int datain[HPBUFFER_LGTH];//����ֵ���У���ʼ��Ϊ0
	int hptr;//�����±꣬��ʼ��Ϊ0
}qrsHpStruct;

//����˲��ṹ��
typedef struct
{
	int datain[DERIV_LENGTH];//����ֵ���У���ʼ��Ϊ0
	int derptr;//�����±꣬��ʼ��Ϊ0
}qrsDerStruct;

//�����˲��ṹ��
typedef struct
{
	int sum;//��ʼ��Ϊ0
	int datain[WINDOW_WIDTH];//����ֵ���У���ʼ��Ϊ0
	int mvptr;//�����±꣬��ʼ��Ϊ0
}qrsMvStruct;

//�����Ͻṹ�嶨�嵽�ܽṹ����
typedef struct
{
	qrsLpStruct qrsLpStr;
	qrsHpStruct qrsHpStr;
	qrsDerStruct qrsDerStr[2];//���Ҫ����һ��
	qrsMvStruct qrsMvStr;
}qrsFilterStruct;

//qrsdet.c��peak������ʹ�õĽṹ��
typedef struct 
{
	int maxvalue;//��ʼ��Ϊ0
	int timeSinceMax;//��ʼ��Ϊ0
	int lastDatum;//��ʼ��Ϊ0
}qrsPeakStruct;

//qrsdet.c��qrsdet������ʹ�õĽṹ��
typedef struct  
{
	int det_thresh;//��ʼ��Ϊ0
	int qpkcnt;//��ʼ��Ϊ0
	int rsetCount;//��ʼ��Ϊ0
	int nmedian;//��ʼ��Ϊ0
	int qmedian;//��ʼ��Ϊ0
	int rrmedian;//��ʼ��Ϊ0
	int countnum;//��ʼ��Ϊ0
	// sbpeak: search back peak
	int sbpeak;//��ʼ��Ϊ0
	// sbloc: search back location
	int sbloc;//��ʼ��Ϊ0
	// sbcount: search back count
	int sbcount;//��ʼ��ΪMS1500
	int initBlank;//��ʼ��Ϊ0
	int initMax;//��ʼ��Ϊ0
	int preBlankCnt;//��ʼ��Ϊ0
	int tempPeak;//��ʼ��Ϊ0
	int qrsbuf[arrayLen];//��ʼ��Ϊ0
	int noise[arrayLen];//��ʼ��Ϊ0
	int rsetBuff[arrayLen];//��ʼ��Ϊ0
	int rrbuf[arrayLen];//��ʼ��ΪMS1000
	qrsPeakStruct qrsPeakStr;
	qrsFilterStruct qrsFilterStr;
}qrsDetStruct;


#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif // qrsdetstruct_h__