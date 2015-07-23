/******************************************************************************

    �ļ���: filter.h
    ������ ��ͷ�ļ������˲�����
	���ã�
	�����ã� 
	������ ��
	�����б� ��

******************************************************************************/
#ifndef filter_h__
#define filter_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "predefinefilter.h"

int hpFilter(FilterDataStruct *FilterDate, int datum);
int lpFilter(FilterDataStruct *FilterDate, int datum);
int smooth(FilterDataStruct *FilterDate, int datum);
int subpowerline(FilterDataStruct *FilterDate, int datum);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif // filter_h__