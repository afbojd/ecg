#ifndef __RUN_H__
#define __RUN_H__

#include "qrsdet.h"
#ifdef __cplusplus
extern "C" {
#endif
struct AnalysisData 
{
	int qrsLoc;
	int qrsOnLoc;
	int qrsOffLoc;
	int qrsWide;
	int stDif;
	char beaType;
	int rhyme;
	int filterEcgValue;
	int delay;
	//�����˲�������
	int maintainrcgvalue;

	int heartRate;
	int HrArray[5];
	int lastHRate;

	int qrsnum;

	int sumRR;
	int sumRefStVa;
	int refStNum;
	int sumStValue;
	int sumStNum;
	int abnorBeatNum;
	int delayflag;//��ʱ��δ�ҵ�QRS���ϲ���־λ
	int asystoleflag;

// 	int qrsArray[4000];
// 	int stDifArray[4000];
// 	char beaTypeArray[4000];
// 	int heartRateArray[4000];
};

struct Ecgqueue
{
	//����˲����˲�ֵ��ѭ������
	int ecgbuf[bufferlength];
	//�浽��ǰ�źŵ���ѭ�������е��±�
	int ecgptr;
	//��ǰ�źŲ�����������ʱ����delay��
	int ecgnumber;
	//��һQRS���ϲ�delay������
	int lastecgnumber;
	//��һ��QRS���ϲ��յ���delay������֮���ƫ����
	int lastOffSft;
	//�ϴ�֮�ϴ�QRS���ϲ��յ���delay������֮���ƫ����
	int thiOffSft;
};

//��ŵ���ʾ����
/*
struct Displaydata
{
	int displaybuf[displaylength];
	int saveptr;
	int displayptr;
	int delayflag;
	int lastdelayflag;
	int oncedelayflag;
	int bufferlengthflag;
	int judgeflag;
	int flagOfdefg;  //flag of delayflag
	//ά���±꣬Ŀ����ά����ʾ�����������displaybufѭ��������±�
	int maintainptr;
};*/

struct IndexHeart
{
	int score;
	int avrHeart;
	int avrStValue;
	int healthLev;

	int heartJudge;
	int typeJudge;
	int stJudge;
	int detailtype;
};

int initvar(struct Ecgqueue* queue, struct AnalysisData* dataout, 
            struct IndexHeart* heartIndex);

int run(struct AnalysisData* outStruct, int* proArray,
	    int lenArray, struct Ecgqueue* queue);

#ifdef __cplusplus
}  /* extern "C" */
#endif
#endif /* __RUN_H__ */
