#ifndef _MMULTADD_H_
#define _MMULTADD_H_
#include <ap_int.h>

//typedef ap_fixed<4,4, AP_RND, AP_SAT> haha;
typedef int haha;

#define IN_CH_NUM_Conv 1
#define IN_HEIGHT_Conv 28
#define IN_WIDTH_Conv 28

#define OUT_CH_NUM_Conv 8
#define OUT_HEIGHT_Conv 24
#define OUT_WIDTH_Conv 24

#define OUT_CH_NUM_Conv2 16
#define OUT_HEIGHT_Conv2 10
#define OUT_WIDTH_Conv2 10

#define FC1_OUT 64
#define FC2_OUT 10

#define Conv_MAP_SIZE 5

#define Conv_MAP_SIZE2 3

void conv1(haha *A, haha *B, haha *conv1out);
void poolingrelu1(haha *conv1out, haha *pooling1out);
void conv2(haha *pooling1out, haha *W, haha *conv2out);
void poolingrelu2(haha *conv2out, haha *pooling2out);
void fc12(haha *pooling2out, haha *W3, haha *W4, haha *D_sw);

#pragma SDS data zero_copy(A[0:IN_CH_NUM_Conv * IN_HEIGHT_Conv * IN_WIDTH_Conv], B[0:UT_CH_NUM_Conv*IN_CH_NUM_Conv*Conv_MAP_SIZE*Conv_MAP_SIZE], conv1out[0:OUT_CH_NUM_Conv * OUT_HEIGHT_Conv * OUT_WIDTH_Conv])
void conv1_hw(haha *A, haha *B, haha *conv1out);
#pragma SDS data zero_copy(conv1out[0:OUT_CH_NUM_Conv * OUT_HEIGHT_Conv * OUT_WIDTH_Conv], pooling1out[0:OUT_CH_NUM_Conv * OUT_HEIGHT_Conv / 2 * OUT_WIDTH_Conv / 2])
void poolingrelu1_hw(haha *conv1out, haha *pooling1out);
#pragma SDS data zero_copy(pooling1out[0:OUT_CH_NUM_Conv * OUT_HEIGHT_Conv / 2 * OUT_WIDTH_Conv / 2], W[0:OUT_CH_NUM_Conv2 * OUT_CH_NUM_Conv * Conv_MAP_SIZE2 * Conv_MAP_SIZE2], conv2out[0:OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2])
void conv2_hw(haha *pooling1out, haha *W, haha *conv2out);
#pragma SDS data zero_copy(conv2out[0:OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2], pooling2out[0:OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_WIDTH_Conv2 / 2])
void poolingrelu2_hw(haha *conv2out, haha *pooling2out);
void fc12_hw(haha *pooling2out, haha *W3, haha *W4, haha *D);

//#pragma SDS data access_pattern(A:SEQUENTIAL, B:SEQUENTIAL, C:SEQUENTIAL)
//#pragma SDS data sys_port(A:ACP, B:AFI)

#endif /* _MMULTADD_H_ */


