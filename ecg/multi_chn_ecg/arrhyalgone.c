/******************************************************************************

    �������� arrhyalgone()
	�﷨�� int arrhyalgone( int qrsposition, int qrswide,
	                                        char *type, int *rhythm, int init)
	������ ����RR���ڼ�QRS����ȼ�����ɲ���
	       �ο����ģ�
		   1�����ھ������붯̬��ֵ��Ƕ��ʽ�ĵ�໤ϵͳ
		   2������Ƕ��ʽ΢���ı�Я�ĵ�໤�����
	���ã� ��׼���е�memcpy���ַ������ƣ�����
	�����ã��ϲ㺯������������
	��������� QRS���ϲ�λ�ã�QRS����ȼ��Ƿ��ʼ������
	��������� ���������ĺ����ֽ�������
	           ��һʱ��QRS������������
	           N���������ģ���V�������粫���ģ���O�������粫�����緿�ԡ�
			   �����Եȣ�
			   ����������
			   (N(��������)��(T(�Ķ�����)��(B(�Ķ�����)��(A(ͣ��)
	����ֵ�� ����ֵ���廹δ����, ��ʱδ���쳣�жϣ����ж�ָ���Ƿ�Ϊ�գ�
	         �ռ��С�Ƿ��㹻�ȵȺ������
	������ ��init��0ʱ��һЩ������ʼֵ
	       �������ɲ������Ϊ���ļ������ɼ�⣬���Կ��������������
		   ��������ɣ����ڽ���һ�㶼��Ҫ�ж�����ģ��������ӳٲ�����
		   ��ν�����������Ҫ����
		   ��������������ģ������������粫���������粫
		   ������ֽ��ɣ��������Ķ����١��Ķ�������ͣ��
		   ͣ�����������Ļ��ǽ��ɣ���û��ǵ�������
		   ��������RR�������б�ͣ���ķ��������Ǻܺã�����һ�������������ĳһ
		   ʱ����޲�������Ϊ������ô˺�������ʱ�����б�ͣ����
		   ��ý�����������ݿ������ƣ�������ÿ�����Ķ������������֮ǰ����
		   ��һ��ʱ���������������������п���
		   ��������RR���ڵ��滻���Ľ�����1����ǰ������������һ����Ҳ��������
		   2�����©������ǰRR����С���������ھ�ֵ��1.6������ʱ�������滻��
		   �Է���©�����ģ�ʹ����һ�����жϳ��粫����
	���Գ��򣺾���Ч���û�������ԭ��1��rr���ڵľ���ϵ��,�����Ŀǰ�����Ӱ��
	          ���Ǻܴ������100�źţ�û�ҵ��쳣������������Ҫ��ϵ����0.8��Ϊ
			  0.83���Ҽ��ɣ�������Կ���ת��Ϊ0.85���ң�Ӧ�ö�û��ʲô����
			  ��������������Ѿ������
		      2��Ŀǰ������źž�Ϊԭʼ�źţ����Բ��ҵ�QRS����Ȳ�׼ȷ������
			  Ӱ��

			  ÿ�õ�һ��qrs��õ���ʱ�̵���������

******************************************************************************/

#include <string.h>
#include "qrsdet.h"

int arrhyalgone( int qrsposition, int qrswide,
	                                  char *type, int *rhythm, int init)
{
	static int rr1 = 0, rr2 = 0, rr3 = 0, rr4 = 0, rr5 = 0;//RR����
	static int arr1 = 0, arr2 = 0, arr3 = 0, arr4 = 0;//�������ĵ�RR����
	static int ave_rr = 0, cur_rr = 0;//ƽ��RR���ں͵�ǰRR����
	static int qrsp1 = 0, qrsp2 = 0, qrsp3 = 0,
		                        qrsp4 = 0, qrsp5 = 0, qrsp6 = 0;//QRS��λ��
	static int ta_flag = 0, br_flag = 0;//�Ķ����ټ�������־λ
	static char lastype = '\0', thirdtype = '\0', fourthtype = '\0';
	static char fifthtype = '\0', sixthtype = '\0';
	int qrswidth = 0;//QRS�����
	int intRR = 0;
	real realRR = 0;
//	int testone = 0;
/*
	int va_flag = 0;//�����Ķ����ٱ�־
	int aa_flag = 0;//�����Ķ����ٱ�־
	int vb_flag = 0;//���Զ����ɱ�־
	int vt_flag = 0;//���������ɱ�־
	int pp_flag = 0;//�ɶ�PVCs��־
	int missbeat_flag = 0;//���ͣ����־
	int pva_flag = 0;//�����粫��־
	int atr_flag = 0;//�����粫��־
	int normal_flag = 0;//�������ʱ�־*/
	int rhythm_flag = 0;

	if (init)
	{
		rr1 = rr2 = rr3 = rr4 = 0;

		ave_rr = arr1 = arr2 = arr3 = arr4 = cur_rr = 0;
		qrsp1 = qrsp2 = qrsp3 = qrsp4 = qrsp5 = qrsp6 = 0;
		ta_flag = br_flag = 0;
		lastype = 'N';
		thirdtype = 'N';
		fourthtype = 'N';
		fifthtype = 'N';
		sixthtype = 'N';
		*rhythm = 7;
	}
	qrsp1 = qrsp2;
	qrsp2 = qrsp3;
	qrsp3 = qrsp4;
	qrsp4 = qrsp5;
	qrsp5 = qrsp6;
	qrsp6 = qrsposition;
	qrswidth = qrswide;
	rr1 = qrsp2 - qrsp1;
	rr2 = qrsp3 - qrsp2;
	rr3 = qrsp4 - qrsp3;
	rr4 = qrsp5 - qrsp4;
	rr5 = qrsp6 - qrsp5;

	if (qrsp1 && qrsp2 && qrsp3 && qrsp4 && qrsp5 && qrsp6)
	{
		cur_rr = rr5;
		//��������
		*type = 'N';
		//���ڿ�Ȳ���̫�������Ŀ�Ȳ��Һ������鷳
		//����������������϶�����Ҫ���������ȸ���ֵMS120ΪMS135
		intRR = ((int)(0.855 * ave_rr + 0.56));

		realRR = (0.855 * ave_rr + 0.56) - intRR;
		if (realRR >= 0.5000)
		{
			intRR = intRR + 1;
		}
		if (realRR <= -0.5000)
		{
			intRR = intRR - 1;
		}
//		testone = MS135;
		if (cur_rr <= intRR && (qrswidth >= MS135))
		{
			*type = 'V';
		}
		if (cur_rr <= intRR && (qrswidth < MS135))
		{
			*type = 'O';
		}

		//����ave_rr
		if ((*type == 'N') && (lastype == 'N') &&
			               (cur_rr < 1.5 * ave_rr) && (cur_rr > 0.7 * ave_rr))
		{
			arr1 = arr2;
			arr2 = arr3;
			arr3 = arr4;
			arr4 = cur_rr;
			ave_rr = (arr1 + arr2 + arr3 + arr4) / 4;
		}


		//����ϸ�ָ����ͳ���
		if ((thirdtype == 'V') && (lastype == 'V') && (*type == 'V'))
		{
			rhythm_flag = 15;
		}
		else if((thirdtype == 'O') && (lastype == 'O') && (*type == 'O'))
		{
			rhythm_flag = 14;
		}
		else if((fourthtype == 'N') && (thirdtype == 'V')
			                        && (lastype == 'N') && (*type == 'V'))
		{
			rhythm_flag = 13;
		}
		else if((sixthtype == 'N') && (fifthtype == 'N') &&
			    (fourthtype == 'V') && (thirdtype == 'N') &&
				(lastype == 'N') && (*type == 'V'))
		{
			rhythm_flag = 12;
		}
		else if((sixthtype == 'V') && (fifthtype == 'V') &&
			    (fourthtype == 'N') && (thirdtype == 'V') &&
			    (lastype == 'V') && (*type == 'N'))
		{
			rhythm_flag = 12;
		}
		else if((lastype == 'V') && (*type == 'V'))
		{
			rhythm_flag = 11;
		}
		else if(cur_rr >= MS1500)
		{
			rhythm_flag = 10;
		}
		else if(*type == 'V')
		{
			rhythm_flag = 9;
		}
		else if(*type == 'O')
		{
			rhythm_flag = 8;
		}
		else
		{
			rhythm_flag = 7;
		}

		if (rhythm_flag > (*rhythm))
		{
			*rhythm = rhythm_flag;
		}

		sixthtype = fifthtype;
		fifthtype = fourthtype;
		fourthtype = thirdtype;
		thirdtype = lastype;
		lastype = *type;

/*
		//��������
		memcpy(rhythm,"(N",4);
		if (cur_rr <= MS600)
		{
			++ta_flag;
		}
		else
		{
			ta_flag = 0;
		}
		if ((cur_rr >= MS1200) && (cur_rr < MS2400))
		{
			++br_flag;
		}
		else
		{
			br_flag = 0;
		}
		if (ta_flag > 3)
		{
			memcpy(rhythm,"(T",4);
		}
		if (br_flag > 3)
		{
			memcpy(rhythm,"(B",4);
		}
		if (cur_rr >= MS2400)
		{
			memcpy(rhythm,"(A",4);
		}*/

	}
	else
	{
		arr1 = rr2;
		arr2 = rr3;
		arr3 = rr4;
		arr4 = rr5;
		ave_rr = (arr1 + arr2 + arr3 + arr4) / 4;
		sixthtype = fifthtype;
		fifthtype = fourthtype;
		fourthtype = thirdtype;
		thirdtype = lastype;
		lastype = *type;
		*type = 'N';
		*rhythm = 7;
	}
	return(0);
}
