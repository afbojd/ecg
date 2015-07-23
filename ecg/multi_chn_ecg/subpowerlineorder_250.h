/******************************************************************************

    �ļ���: subpowerlineorder_250.h
    ������ ȥ��Ƶ�������������
	������ 
	�����б� ��
	�ο����ģ� ��Ҫ����������ƪ���ģ����Ļ���һ��
	           1��Subtraction of 50 Hz interference from the
	           electrocardiogram
	           2��New approach to the digital elimination of
	           50 Hz interference from the electrocardiogram
	
	������ ����һ�ַ������˲����������⼸�ֲ�ͬ�ĵط�ֻ����subpowerlineOrder.h
	       ͷ�ļ�������ϵ��ͷ�ļ���������.c�ļ��л����ý���ͷ�ļ���
	       �ڽ���ͷ�ļ��ж���ĺ�������˲����࣬��Щ
	       ����Ҫ��Ե���50Hz��Ƶ���ţ�Ŀǰ����ȥ��60Hz��Ƶ����
	ע�⣺ �ڲ�ͬ�Ĳ���Ƶ�ʺ͹�Ƶ�����¶���ĺ���Ҫ����Ӧ�ļ��¸���

******************************************************************************/
#ifndef subpowerlineorder_250_h__
#define subpowerlineorder_250_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//�������ݴ洢���鳤��
#define datainlen 7
//��Ƶ�������洢���鳤��
#define interferencelen 5
//һ�����ڲ���������ʵ��Ϊ����Ƶ�ʳ��Թ�Ƶ�������ã�
#define numOfCircle 5
//�������ڲ���������ʵ��Ϊ������Ƶ�ʳ��Թ�Ƶ���� - 1��/2��
#define halfNumOfCir 2
//���������ݴ洢��������Ҫ�õ����±�ƫ��ֵ
#define realptrShift 2

#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif // subpowerlineorder_250_h__




