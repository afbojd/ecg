#ifndef mult_run_h__
#define mult_run_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "qrsdet.h"
#include "predefinefilter.h"
#include "qrsdetstruct.h"

//ʹ��ʵʱ���ʲ���仯̫�󣬲��ԣ������ȶ���Ϊ��
#define HrArLen 5

//����ÿͨ���ĵ�ADת�����ȼ�
//�㷨ͨ����
//�ڳ�ʼ��ʱ���룬�ɵ����߸�ֵ���˽ṹ��
typedef struct
{
	//�ĵ�ADת������
	real precision[8];
	//�㷨ͨ����
	int algoChanArray[8];
}InitVarStruct;


//ά���ĵ����ݵĽṹ�壬ʵ��Ϊһ����
typedef struct 
{
	//����˲����˲�ֵ��ѭ������
	int ecgbuf[bufferlength];
	//�浽��ǰ�źŵ���ѭ�������е��±�
	int ecgptr;
	//��ǰ�źŲ�����������ʱ����delay��
	int ecgnumber;
	////��һQRS���ϲ�delay������
	//int lastecgnumber;
	////��һ��QRS���ϲ��յ���delay������֮���ƫ����
	//int lastOffSft;
	////�ϴ�֮�ϴ�QRS���ϲ��յ���delay������֮���ƫ����
	//int thiOffSft;
}EcgqueueStruct;


//��ʾ������Ľ���ṹ��
typedef struct 
{
	int filterEcgValue;
}OutDataStruct;


typedef struct  
{
	int qrsLoc;

	int heartRate;
	int HrArray[HrArLen];//Ŀ����ʹ��ʵʱ���ʲ���仯̫�󣬲��� 
	int lastHRate;
	int qrsnum;

	int delayflag;

	int algoChan;

	EcgqueueStruct Ecgqueue;
	OutDataStruct OutDate;
	//Displaydatastruct Displaydata;
	FilterDataStruct FilterData;
	qrsDetStruct qrsDetStr;

}ChannelAnalysisStruct;

typedef struct
{
	ChannelAnalysisStruct *ChannelAnalysis;
	int channelNum;
}TotalAnalysisStruct;

int mult_initvar(TotalAnalysisStruct* TotalStruct, 
	ChannelAnalysisStruct* ChannelStruct, int channelNum, InitVarStruct* InitVarStr);

int mult_run(TotalAnalysisStruct* TotalStruct, int* proArray, int lenArray);


#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif // mult_run_h__
