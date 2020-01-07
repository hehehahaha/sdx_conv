#include <iostream>
#include <stdlib.h>
#include <stdint.h>

#include "sds_lib.h"
#include "conv.h"

#define Max(A,B) ((A>B)?A:B)

void conv1_hw(haha *A, haha *B, haha *conv1out) {
	haha tempA[IN_CH_NUM_Conv][IN_HEIGHT_Conv][IN_WIDTH_Conv];
#pragma HLS array_partition variable = tempA complete dim = 1
	haha tempB[OUT_CH_NUM_Conv/2][IN_CH_NUM_Conv][Conv_MAP_SIZE][Conv_MAP_SIZE];
#pragma HLS array_partition variable = tempB complete dim = 1
#pragma HLS array_partition variable = tempB complete dim = 2
	haha tempconv1out[OUT_CH_NUM_Conv/2][OUT_HEIGHT_Conv][OUT_WIDTH_Conv] = {0};
//#pragma HLS INTERFACE axis port=A
#pragma HLS array_partition variable = tempconv1out complete dim = 1

	for (int cyc = 0; cyc < OUT_CH_NUM_Conv; cyc += 4)
	{
		memcpy((void*)tempA, (const void*)A, sizeof(haha) * 28 * 28);
		memcpy((void*)tempB, B + 5 * 5 * 1 * 4 * cyc / 4, sizeof(haha) * 5 * 5 * 1 * 4);

		for (int kr = 0; kr < Conv_MAP_SIZE; kr++)//5
		{
			for (int kc = 0; kc < Conv_MAP_SIZE; kc++)//5
			{
				for (int r = 0; r < OUT_HEIGHT_Conv; r++)//24
				{
					for (int c = 0; c < OUT_WIDTH_Conv; c++)//24
					{
#pragma HLS PIPELINE
						for (int cho = 0; cho < OUT_CH_NUM_Conv/2; cho++)//8
						{
							for (int chi = 0; chi < IN_CH_NUM_Conv; chi++)//1
							{
								tempconv1out[cho][r][c] += tempA[chi][r + kr][c + kc] * tempB[cho][chi][kr][kc];
							}
						}
					}
				}
			}
		}
		memcpy(conv1out + 4 * 24 * 24 * cyc / 4, (const void*)tempconv1out, sizeof(haha) * 4 * 24 * 24);
		for (int i = 0; i < OUT_CH_NUM_Conv / 2; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv; k++) {
					tempconv1out[i][j][k] = 0;
				}
			}
		}
	}
}

void poolingrelu1_hw(haha *conv1out, haha *pooling1out) {
	haha tempconv1out[OUT_CH_NUM_Conv][OUT_HEIGHT_Conv][OUT_WIDTH_Conv];
	haha temppooling1[OUT_CH_NUM_Conv][OUT_HEIGHT_Conv / 2][OUT_WIDTH_Conv / 2] = { 0 };
	haha temppooling1out[OUT_CH_NUM_Conv][OUT_HEIGHT_Conv / 2][OUT_WIDTH_Conv / 2] = { 0 };
	memcpy((void*)tempconv1out, (const void*)conv1out, sizeof(haha) * 8 * 24 * 24);
//#pragma HLS INTERFACE axis port=conv1out
//#pragma HLS INTERFACE axis port=pooling1out
	for (int i = 0; i < OUT_CH_NUM_Conv; i++) {
		for (int j = 0; j < OUT_HEIGHT_Conv / 2; j++) {
			for (int k = 0; k < OUT_WIDTH_Conv / 2; k++) {
				temppooling1[i][j][k] = Max(Max(tempconv1out[i][2 * j][2 * k], tempconv1out[i][2 * j][2 * k + 1]), Max(tempconv1out[i][2 * j + 1][2 * k], tempconv1out[i][2 * j + 1][2 * k + 1]));
				temppooling1out[i][j][k] = (temppooling1[i][j][k] > 0) ? temppooling1[i][j][k] : ((haha)0.0);
			}
		}
	}
	memcpy((void*)pooling1out, (const void*)temppooling1out, sizeof(haha) * 8 * 12 * 12);
}

void conv2_hw(haha *pooling1out, haha *W, haha *conv2out) {
	haha temppooling1out[OUT_CH_NUM_Conv][OUT_HEIGHT_Conv / 2][OUT_WIDTH_Conv / 2];
#pragma HLS array_partition variable = temppooling1out complete dim = 1
	haha tempW[OUT_CH_NUM_Conv2][OUT_CH_NUM_Conv][Conv_MAP_SIZE2][Conv_MAP_SIZE2];
#pragma HLS array_partition variable = tempW complete dim = 1
#pragma HLS array_partition variable = tempW complete dim = 2
	haha tempconv2out[OUT_CH_NUM_Conv2][OUT_HEIGHT_Conv2][OUT_WIDTH_Conv2] = { 0 };
#pragma HLS array_partition variable = tempconv2out complete dim = 1
//#pragma HLS INTERFACE axis port=pooling1out
//#pragma HLS INTERFACE axis port=W
//#pragma HLS INTERFACE axis port=conv2out
	memcpy((void*)temppooling1out, (const void*)pooling1out, sizeof(haha) * 8 * 12 * 12);
	memcpy((void*)tempW, (const void*)W, sizeof(haha) * 16 * 8 * 3 * 3);

	for (int kr = 0; kr < Conv_MAP_SIZE2; kr++)
	{
		for (int kc = 0; kc < Conv_MAP_SIZE2; kc++)
		{
			for (int r = 0; r < OUT_HEIGHT_Conv2; r++)
			{
				for (int c = 0; c < OUT_WIDTH_Conv2; c++)
				{
#pragma HLS PIPELINE
					for (int cho = 0; cho < OUT_CH_NUM_Conv2; cho++)
					{
						for (int chi = 0; chi < OUT_CH_NUM_Conv; chi++)
						{
							tempconv2out[cho][r][c] += temppooling1out[chi][r + kr][c + kc] * tempW[cho][chi][kr][kc];
						}
					}
				}
			}
		}
	}
	memcpy((void*)conv2out, (const void*)tempconv2out, sizeof(haha) * 16 * 10 * 10);
}

void poolingrelu2_hw(haha *conv2out, haha *pooling2out) {
	haha tempconv2out[OUT_CH_NUM_Conv2][OUT_HEIGHT_Conv2][OUT_WIDTH_Conv2];
	haha temppooling2[OUT_CH_NUM_Conv2][OUT_HEIGHT_Conv2 / 2][OUT_WIDTH_Conv2 / 2] = { 0 };
	haha temppooling2out[OUT_CH_NUM_Conv2][OUT_HEIGHT_Conv2 / 2][OUT_WIDTH_Conv2 / 2] = { 0 };
//#pragma HLS INTERFACE axis port=conv2out
//#pragma HLS INTERFACE axis port=pooling2out
	haha templc[OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_HEIGHT_Conv2 / 2] = { 0 };
	memcpy((void*)tempconv2out, (const void*)conv2out, sizeof(haha) * 16 * 10 * 10);

	for (int i = 0; i < OUT_CH_NUM_Conv2; i++) {
		for (int j = 0; j < OUT_HEIGHT_Conv2 / 2; j++) {
			for (int k = 0; k < OUT_WIDTH_Conv2 / 2; k++) {
				temppooling2[i][j][k] = Max(Max(tempconv2out[i][2 * j][2 * k], tempconv2out[i][2 * j][2 * k + 1]), Max(tempconv2out[i][2 * j + 1][2 * k], tempconv2out[i][2 * j + 1][2 * k + 1]));
				temppooling2out[i][j][k] = (temppooling2[i][j][k] > 0) ? temppooling2[i][j][k] : ((haha)0.0);
			}
		}
	}

	for (int ii = 0; ii < OUT_HEIGHT_Conv2 / 2; ii++) {
		for (int jj = 0; jj < OUT_WIDTH_Conv2 / 2; jj++) {
			for (int kk = 0; kk < OUT_CH_NUM_Conv2; kk++) {
				templc[(ii * OUT_WIDTH_Conv2 / 2 + jj) * OUT_CH_NUM_Conv2 + kk] = temppooling2out[kk][ii][jj];
			}
		}
	}
	memcpy((void*)pooling2out, (const void*)templc, sizeof(haha) * 16 * 5 * 5);
}

void fc12_hw(haha *pooling2out, haha *W3, haha *W4, haha *D){
	haha temppooling2out[OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_HEIGHT_Conv2 / 2];
	haha tempW3[OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_HEIGHT_Conv2 / 2][FC1_OUT];
	haha tempW4[FC1_OUT][FC2_OUT];
	haha tempD_sw[FC2_OUT] = { 0 };
//#pragma HLS INTERFACE axis port=pooling2out
//#pragma HLS INTERFACE axis port=W3
//#pragma HLS INTERFACE axis port=W4
//#pragma HLS INTERFACE axis port=D
	haha Q[FC1_OUT] = { 0 };
	haha temprelu3[FC1_OUT] = { 0 };

	memcpy((void*)temppooling2out, (const void*)pooling2out, sizeof(haha) * 16 * 5 * 5);
	memcpy((void*)tempW3, (const void*)W3, sizeof(haha) * 16 * 5 * 5 * 64);
	memcpy((void*)tempW4, (const void*)W4, sizeof(haha) * 64 * 10);

	for (int j = 0; j < OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_HEIGHT_Conv2 / 2; j++) {
		for (int i = 0; i < FC1_OUT; i++) {
			Q[i] += temppooling2out[j] * tempW3[j][i];
		}
	}
	//relu3
	for (int i = 0; i < FC1_OUT; i++) {
		temprelu3[i] = (Q[i] > 0) ? Q[i] / 100000 : ((haha)0.0);
		//printf("%d ", temprelu3[i]);
	}

	for (int j = 0; j < FC1_OUT; j++) {
		for (int i = 0; i < FC2_OUT; i++) {
			tempD_sw[i] += temprelu3[j] * tempW4[j][i];
		}
	}
	memcpy((void*)D, (const void*)tempD_sw, sizeof(haha) * 10);
}
