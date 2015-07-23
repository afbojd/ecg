/******************************************************************************

    �ļ���: fundecla.h
    ������ fundecla--function declare��������ͷ�ļ������ã�ʹ������ֻ�ܿ���
	       run.h�еĺ����������ܿ���������ʹ�õ���������
	���ã� 1��mult_run.h
	�����ã� mult_run.c
	������ ��
	�����б� ��

******************************************************************************/
#ifndef fundecla_h__
#define fundecla_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "mult_run.h"
//QRS���ϲ��������ú���
int multlpfilt( qrsFilterStruct* qrsFilterStr, int datum );
int multhpfilt( qrsFilterStruct* qrsFilterStr, int datum );
int multderiv1( qrsFilterStruct* qrsFilterStr, int datum );
int multderiv2( qrsFilterStruct* qrsFilterStr, int datum );
int multmvwint( qrsFilterStruct* qrsFilterStr, int datum );
int multqrsfilter( qrsFilterStruct* qrsFilterStr, int datum );
int multqrsdet( qrsDetStruct* qrsDetStr, int datum );

//��ȷλ�ò�����غ�����ST�κ���
int accurloc( ChannelAnalysisStruct* ChanAnaStr, int delay );
//ͳ�ƺ���
int statfun( ChannelAnalysisStruct* ChanAnaStr, int lastqrsLOc );

#ifdef __cplusplus
}  /* extern "C" */
#endif
#endif // fundecla_h__