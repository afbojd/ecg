/******************************************************************************

    �ļ���: predefinefilter.h
    ������ ��ͷ�ļ������˲����м�ֵ�ṹ��
	���ã�
	�����ã�
	������ ��
	�����б� ��

******************************************************************************/
#ifndef predefinefilter_h__
#define predefinefilter_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "qrsdet.h"

#include "hpfilterorder_250.h"
#include "lpfilterorder_250.h"
#include "smoothorder_250.h"
#include "subpowerlineorder_250.h"


//ȥ��Ƶ�˲������ýṹ������
typedef struct
{
	//�������ݴ洢����
	int dataIn[datainlen];
	//��Ƶ�������洢����
	int interference[interferencelen];
	//dataIn�е�ѭ���±꣬��ʼ��Ϊ0
	int ptr;
	//��ctrVar�ľ�������Ҫ�����ģ���ʼ��Ϊ0
	int jVar;
	//ע���ʼ��Ϊ1
	int ctrVar;
	//���б�׼����������M��
	real crit;
}subPLStruct;

//�˲����������ýṹ������
typedef struct
{
	//��ͨ�˲������м���
	int hpDataIn[hpNL];
	real hpDataOut[hpDL];
	//��ͨ�˲������м���
	int lpDataIn[lpNL];
	real lpDataOut[lpDL];
	//ƽ���˲������м���
	int smoothDataIn[smoothBL];
	//ȥ��Ƶ�ṹ��
	subPLStruct subPL;
}FilterDataStruct;


#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif // predefinefilter_h__

