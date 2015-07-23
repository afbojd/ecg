/******************************************************************************

    �ļ���: smoothCof_250.h
	������ ƽ���˲��������������õ��ľ���4��ƽ����250Hz��ת����Ƶ�����൱��
	       -3dB��ֹƵ��Ϊ30Hz����һ���ݲ�Ƶ��Ϊ62.5Hz�ĵ�ͨ�˲���
	������ �˲���������FIR��
	�����б� ��

******************************************************************************/
#ifndef smoothCof_250_h__
#define smoothCof_250_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "qrsdet.h"
#include "smoothorder_250.h"

const real smB[smoothBL] = {0.250000000f,    0.250000000f,    
	                        0.250000000f,    0.250000000f};

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif // smoothCof_250_h__

