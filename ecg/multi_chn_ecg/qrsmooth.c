/******************************************************************************

    �������� qrsonset()
	�﷨�� int qrsmooth(int *buf)
	������ ����5��ƽ������,���鳤���ǹ̶���MS280
	�ο����ģ� ��matlab��smooth�е�moving��������
	���ã� ��
	�����ã�qrsandonoffset()����
	��������� ��qrsandonoffset���������е�qrsbuffer
	��������� �˲����qrsbuffer
	����ֵ�� 0��ʾ��������ʱδ���쳣�жϣ����ж�ָ���Ƿ�Ϊ�գ��ռ��С
	         �Ƿ��㹻�ȵȺ������
	������ ������������ͳ�ֵ��const int�������Ѽ�����������������ܣ�
	       ���Դ˺���ֻ������������������������������
	 
******************************************************************************/

#include "qrsdet.h"

int qrsmooth(int *buf)
{
	int tmp_buf[MS280] = {0};
	int liv_loop = 2;
	for (; liv_loop < MS280 - 2; ++liv_loop)
	{
		tmp_buf[liv_loop] = (buf[liv_loop - 2] + buf[liv_loop -1] 
		          + buf[liv_loop] + buf[liv_loop + 1] + buf[liv_loop + 2]) / 5;
	}
	tmp_buf[0] = buf[0];
	tmp_buf[1] = buf[1];
	tmp_buf[liv_loop] = buf[liv_loop];
	++liv_loop;
	tmp_buf[liv_loop] = buf[liv_loop];
	for (liv_loop = 0; liv_loop < MS280; ++liv_loop)
	{
		buf[liv_loop]= tmp_buf[liv_loop];
	}

	return(0);
}