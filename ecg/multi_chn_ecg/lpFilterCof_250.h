/******************************************************************************

    �ļ���: lpFilterCof_250.h
    ������ ��ͨ�˲����˲�ϵ��
	       ���250Hz��-3dB��ֹƵ��Ϊ30Hz���,�½��Ƚ�ƽ��
	������ �������ΪIIR�Ͳ��ȶ������ΪFIR��
	�����б� ��

******************************************************************************/
#ifndef lpFilterCof_250_h__
#define lpFilterCof_250_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "qrsdet.h"
#include "lpfilterorder_250.h"

const real lpNum[lpNL] = {
   0.08333333582f,   0.25f,   0.3333333433f,   0.25f,  0.08333333582f
};

const real lpDen[lpDL] = {
	1
};


#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif // lpFilterCof_250_h__

