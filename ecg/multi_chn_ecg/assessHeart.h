#ifndef assessHeart_h__
#define assessHeart_h__

#include "run.h"

/*
������judge���ĺ���������£�
heartJudge��1����������
            2�������Ի�
			3�������Կ�
			4�����ʹ���
			5�����ʹ���
			6���˴μ��������

typeJudge��1��δ�������ɲ���
           2���������ɲ���
		   3�����ɲ�������Ժܴ�
		   4�����ɲ���
		   6���˴μ��������

detailtype:6���˴μ��������
           7��δ�������ɲ���
           8�����ַ����粫
           9�����������粫
           10�����ͣ��
           11���ɶ������粫
           12�������粫������
           13�������粫������
           14�������Ķ�����
           15�������Ķ�����

stJudge��1��ST�ε�ѹ����
         2��ST�ε�ѹ���̧��
		 3��ST�ε�ѹ���ѹ��
		 4��ST�ε�ѹ̧��
		 5��ST�ε�ѹѹ��
		 6���˴μ��������

������healthLevֵ�ĺ���������£�
healthLev��3�����׷�������ǰ��ҽԺ����һ�����
           4�����׷��������˶������������泩
		   5�����׷��������˶�����������
		   6���ź��������ã��˴μ�������壬�����õ缫���¼��

*/
#ifdef __cplusplus
extern "C" {
#endif



int assessHeart(struct IndexHeart* heartIndex, struct AnalysisData* outStruct);


#ifdef __cplusplus
}  /* extern "C" */
#endif
#endif // assessHeart_h__