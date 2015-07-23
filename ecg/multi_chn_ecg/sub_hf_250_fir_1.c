/******************************************************************************

    �������� sub_hf_250_fir_1()
	�﷨�� int sub_hf_250_fir_1(int datum, int init)
		������ ȥ��Ƶ��������
	       ��ַ��̣�Y(n) =��1/4) * [ X(n) + 2 * X(n-1) + X(n-2))]
	�ο����ģ� ��Ҫ������������Online digital filters for biological signals 
	                           some fast designs for a small computer
	���ã� ��
	�����ã����������ϲ㺯��
	������������˲��źż���ʼ����־��
	��������� ��
	����ֵ�� ���˲��ź�
	������ �ҵ���һֱ��matlab�汾��һ�µ�ԭ��matlab������м����Ǹ����ͣ�
	       ���ڴξ�Ϊint�ͣ�����ѡ�������ͻ�Ҫ���ǣ�����������
		   sub_hf_250_fir_1����������sub_hf_250_fir_2�����ϲ���Ϊһ������
    ע�⣺ ����ע������
	       1���˺�������matlab�汾һ�������Դ�����������ʵ��źţ�
		   ����ֻ�ܴ��������Ϊ250Hz������Ƶ��Ϊ62.5Hz
		   2������������������⣬�м����϶�Ϊ�������ͣ��������������������
		   ������ǰ�����˳���ۺϿ���
	 
******************************************************************************/


/*
double sub_hf_250_fir_1(int datum, int init)
{
	static int data[3], ptr;
	double fdatum = 0;//���������
	int intfdatum = 0;//���������
	double subPrecision = 0;//��߾��ȵ��м���
	int ptrMinusOne = 0;
	int ptrMinusTwo = 0;//����
	double medineOne = 0;
	double medineTwo = 0, medineThree = 0;
	//��ʼ��
	if (init)
	{
		for (ptr = 0; ptr < 3; ++ptr)
		{
			data[ptr] = 0;
		}
		ptr = 0;
		return 0;
	}

	data[ptr] = datum;
	ptrMinusOne = ptr - 1;
	if (ptrMinusOne < 0)
	{
		ptrMinusOne = ptrMinusOne + 3;
	}
	ptrMinusTwo = ptr - 2;
	if (ptrMinusTwo < 0)
	{
		ptrMinusTwo = ptrMinusTwo + 3;
	}

	medineOne = (1  * (double)data[ptr] / 4);
	medineTwo = (2  * (double)data[ptrMinusOne] / 4);
	medineThree = (1  * (double)data[ptrMinusTwo] / 4);
	fdatum = medineOne + medineTwo + medineThree;//��ַ���

	++ptr;
	if (ptr == 3)
	{
		ptr = 0;
	}

	intfdatum = (int)(fdatum);
	//��������Ͷ���Ϊint��ʱ���ʵ�����������
	subPrecision = fdatum - intfdatum;
	if (subPrecision >= 0.5000)
	{
		intfdatum = intfdatum + 1;
	}
	if (subPrecision <= -0.5000)
	{
		intfdatum = intfdatum - 1;
	}
	return fdatum;
}*/



int sub_hf_250_fir_1(int datum, int init)
{
	static int data[3], ptr;
	double fdatum = 0;//���������
	int intfdatum = 0;//���������
	double subPrecision = 0;//��߾��ȵ��м���
	int ptrMinusOne = 0;
	int ptrMinusTwo = 0;//����
	double medineOne = 0;
	double medineTwo = 0, medineThree = 0;
	//��ʼ��
	if (init)
	{
		for (ptr = 0; ptr < 3; ++ptr)
		{
			data[ptr] = 0;
		}
		ptr = 0;
		return 0;
	}

	data[ptr] = datum;
	ptrMinusOne = ptr - 1;
	if (ptrMinusOne < 0)
	{
		ptrMinusOne = ptrMinusOne + 3;
	}
	ptrMinusTwo = ptr - 2;
	if (ptrMinusTwo < 0)
	{
		ptrMinusTwo = ptrMinusTwo + 3;
	}

	medineOne = (1  * (double)data[ptr] / 4);
	medineTwo = (2  * (double)data[ptrMinusOne] / 4);
	medineThree = (1  * (double)data[ptrMinusTwo] / 4);
	fdatum = medineOne + medineTwo + medineThree;//��ַ���

	++ptr;
	if (ptr == 3)
	{
		ptr = 0;
	}

	intfdatum = (int)(fdatum);
	//��������Ͷ���Ϊint��ʱ���ʵ�����������
	subPrecision = fdatum - intfdatum;
	if (subPrecision >= 0.5000)
	{
		intfdatum = intfdatum + 1;
	}
	if (subPrecision <= -0.5000)
	{
		intfdatum = intfdatum - 1;
	}
	return intfdatum;
}