/******************************************************************************

    �������� sub_dc_offset_250()
	�﷨�� int sub_dc_offset_250(int datum, int init)
	������ ȥ��Ƶ��������
	       ��ַ��̣�Y(n) = Y(n-1) + ��249 / 250�� * X(n) - X(n-1) + 
		   ��1 / 250�� * X(n-250)
	�ο����ģ� ��Ҫ������������Digital Filters for Real-Time ECG Signal 
               Processing Using Microprocessors.
	���ã� ��
	�����ã����������ϲ㺯��
	������������˲��źż���ʼ����־��
	��������� ��
	����ֵ�� ���˲��ź�
	������ �ҵ���һֱ��matlab�汾��һ�µ�ԭ��matlab������м����Ǹ����ͣ�
	       ���ڴξ�Ϊint�ͣ�����ѡ�������ͻ�Ҫ���ǣ�����������
    ע�⣺ ����ע������
	       1���˺�������matlab�汾һ�������Դ�����������ʵ��źţ�
		   ����ֻ�ܴ��������Ϊ250Hz���ź�
		   ���ADS1298-FE�������²ɼ����ź�,�˴��Ľ�ֹƵ��Ϊ0.5Hz
		   2������������������⣬�м����϶�Ϊ�������ͣ��������������������
		   ������ǰ�����˳���ۺϿ���
	 
******************************************************************************/

int sub_dc_offset_250(int datum, int init)
{
	static double lastFdatum;//��Y(n-1)
	static int data[251], ptr;
	double fdatum = 0;//���������
	int intfdatum = 0;//���������
	double subPrecision = 0;//��߾��ȵ��м���
	int ptrMinusOne = 0, ptrMinusOrder = 0;//����
	double medineOne = 0, medineTwo = 0;
	//��ʼ��
	if (init)
	{
		lastFdatum = 0;
		for (ptr = 0; ptr < 251; ++ptr)
		{
			data[ptr] = 0;
		}
		ptr = 0;
		return 0;
	}

	data[ptr] = datum;
	ptrMinusOne = ptr - 1;
	ptrMinusOrder = ptr - 250;
	if (ptrMinusOne < 0)
	{
		ptrMinusOne = ptrMinusOne + 251;
	}
	if (ptrMinusOrder < 0)
	{
		ptrMinusOrder = ptrMinusOrder + 251;
	}

	medineOne = (249  * (double)data[ptr] / 250);
	medineTwo = (1  * (double)data[ptrMinusOrder] / 250);
	fdatum = lastFdatum + medineOne
	         - data[ptrMinusOne] + medineTwo;//��ַ���
	lastFdatum = fdatum;

	++ptr;
	if (ptr == 251)
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