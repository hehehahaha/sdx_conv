#include <iostream>
#include <stdlib.h>
#include <stdint.h>

#include "sds_lib.h"
#include "conv.h"

#ifndef NUM_TESTS
#define NUM_TESTS 1
#endif

#define Max(A,B) ((A>B)?A:B)

void conv1(haha *A, haha *W1, haha *conv1out);
void bnrelu1(haha *conv1out, haha *gama1, haha *beta1, haha *burelu1out);
void conv2(haha *burelu1out, haha *W2, haha *conv2out);
void bnrelu2(haha *conv2out, haha *gama2, haha *beta2, haha *burelu2out);
void pooling1(haha *burelu2out, haha *pooling1out);

void conv3(haha *D_sw, haha *W3, haha *conv3out);
void bnrelu3(haha *conv3out, haha *gama3, haha *beta3, haha *burelu3out);
void conv4(haha *burelu3out, haha *W4, haha *conv4out);
void bnrelu4(haha *conv4out, haha *gama4, haha *beta4, haha *burelu4out);
void pooling2(haha *burelu4out, haha *pooling2out);

class perf_counter
{
public:
     uint64_t tot, cnt, calls;
     perf_counter() : tot(0), cnt(0), calls(0) {};
     inline void reset() { tot = cnt = calls = 0; }
     inline void start() { cnt = sds_clock_counter(); calls++; };
     inline void stop() { tot += (sds_clock_counter() - cnt); };
     inline uint64_t avg_cpu_cycles() { return ((tot+(calls>>1)) / calls); };
};

static void init_arrays(haha *A, haha *W1, haha *conv1out, haha *gama1, haha *beta1, haha *burelu1out, haha *W2, haha *conv2out, haha *gama2, haha *beta2, haha *burelu2out, haha *D_sw)//A输入， B卷积核1， D硬件输出， D_sw软件输出， W卷积核2
{
	//读取输入
	FILE *fp;
	float bufferA[IN_CH_NUM_Conv * IN_HEIGHT_Conv * IN_WIDTH_Conv];
	fp = fopen("/mnt/inp.txt", "r");
	if (fp == NULL) {
		printf("sadioj111111111gsaoig");
	}
	for (int i = 0; i < IN_CH_NUM_Conv * IN_HEIGHT_Conv * IN_WIDTH_Conv; i++) {
		fscanf(fp, "%f", &bufferA[i]);
	}
	fclose(fp);

	//读取权值1
	FILE *fp1;
	float bufferW1[Conv_MAP_SIZE * Conv_MAP_SIZE * IN_CH_NUM_Conv * OUT_CH_NUM_Conv];
	fp1 = fopen("/mnt/conv1.txt", "r");
	if (fp1 == NULL) {
		printf("sadiojg222222sao1ig");
	}
	for (int i = 0; i < Conv_MAP_SIZE * Conv_MAP_SIZE * IN_CH_NUM_Conv * OUT_CH_NUM_Conv; i++) {
		fscanf(fp1, "%f", &bufferW1[i]);
	}
	/* for (int k = 0; k < 5*5*8; k++){
	  printf("%f", buffer1[k]);
	 }*/
	fclose(fp1);

	//读取gama1
	FILE *fp2;
	float buffergama1[OUT_CH_NUM_Conv];
	fp2 = fopen("/mnt/gama1.txt", "r");
	if (fp2 == NULL) {
		printf("sadiojgs333333ao1ig");
	}
	for (int i = 0; i < OUT_CH_NUM_Conv; i++) {
		fscanf(fp2, "%f", &buffergama1[i]);
	}
	/* for (int k = 0; k < 5*5*8; k++){
	  printf("%f", buffer1[k]);
	 }*/
	fclose(fp2);

	//读取beta1
	FILE *fp3;
	float bufferbeta1[OUT_CH_NUM_Conv];
	fp3 = fopen("/mnt/beta1.txt", "r");
	if (fp3 == NULL) {
		printf("sadiojgs4444444444ao1ig");
	}
	for (int i = 0; i < OUT_CH_NUM_Conv; i++) {
		fscanf(fp3, "%f", &bufferbeta1[i]);
	}
	/* for (int k = 0; k < 5*5*8; k++){
	  printf("%f", buffer1[k]);
	 }*/
	fclose(fp3);

	//读取权值2
	FILE *fp4;
	float bufferW2[Conv_MAP_SIZE * Conv_MAP_SIZE * OUT_CH_NUM_Conv2 * OUT_CH_NUM_Conv];
	fp4 = fopen("/mnt/conv2.txt", "r");
	if (fp4 == NULL) {
		printf("sadio555555555jgs4aoig");
	}
	for (int i = 0; i < Conv_MAP_SIZE * Conv_MAP_SIZE * OUT_CH_NUM_Conv2 * OUT_CH_NUM_Conv; i++) {
		fscanf(fp4, "%f", &bufferW2[i]);
	}
	/* for (int k = 0; k < 3*3*16*8; k++){
	  printf("%f", buffer2[k]);
	 }*/
	fclose(fp4);

	//读取gama2
	FILE *fp5;
	float buffergama2[OUT_CH_NUM_Conv2];
	fp5 = fopen("/mnt/gama2.txt", "r");
	if (fp5 == NULL) {
		printf("sadioj666666gsao1ig");
	}
	for (int i = 0; i < OUT_CH_NUM_Conv2; i++) {
		fscanf(fp5, "%f", &buffergama2[i]);
	}
	/* for (int k = 0; k < 5*5*8; k++){
	  printf("%f", buffer1[k]);
	 }*/
	fclose(fp5);

	//读取beta2
	FILE *fp6;
	float bufferbeta2[OUT_CH_NUM_Conv2];
	fp6 = fopen("/mnt/beta2.txt", "r");
	if (fp6 == NULL) {
		printf("sadiojg777777777sao1ig");
	}
	for (int i = 0; i < OUT_CH_NUM_Conv2; i++) {
		fscanf(fp6, "%f", &bufferbeta2[i]);
	}
	/* for (int k = 0; k < 5*5*8; k++){
	  printf("%f", buffer1[k]);
	 }*/
	fclose(fp6);

	//初始化

	for (int i = 0; i < IN_CH_NUM_Conv * IN_HEIGHT_Conv * IN_WIDTH_Conv; i++) {
		A[i] = bufferA[i];
		//printf("%f ", A[i]);
	}

	for (int i = 0; i < OUT_CH_NUM_Conv * IN_CH_NUM_Conv * Conv_MAP_SIZE * Conv_MAP_SIZE; i++) {
		W1[i] = bufferW1[i];
	}

	for (int i = 0; i < OUT_CH_NUM_Conv * OUT_HEIGHT_Conv * OUT_WIDTH_Conv; i++) {
		conv1out[i] = 0;
	}

	for (int i = 0; i < OUT_CH_NUM_Conv; i++) {
		gama1[i] = buffergama1[i];
	}

	for (int i = 0; i < OUT_CH_NUM_Conv; i++) {
		beta1[i] = bufferbeta1[i];
	}

	for (int i = 0; i < OUT_CH_NUM_Conv * OUT_HEIGHT_Conv * OUT_WIDTH_Conv; i++) {
		burelu1out[i] = 0;
	}

	for (int i = 0; i < OUT_CH_NUM_Conv2 * OUT_CH_NUM_Conv * Conv_MAP_SIZE * Conv_MAP_SIZE; i++) {
		W2[i] = bufferW2[i];
	}

	for (int i = 0; i < OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2; i++) {
		conv2out[i] = 0;
	}

	for (int i = 0; i < OUT_CH_NUM_Conv2; i++) {
		gama2[i] = buffergama2[i];
	}

	for (int i = 0; i < OUT_CH_NUM_Conv2; i++) {
		beta2[i] = bufferbeta2[i];
	}

	for (int i = 0; i < OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2; i++) {
		burelu2out[i] = 0;
	}

	for (int i = 0; i < 128 * 28 * 28 / 4; i++) {
		D_sw[i] = 0;
	}
}

static void init_arrays2(haha *W3, haha *conv3out, haha *gama3, haha *beta3, haha *burelu3out, haha *W4, haha *conv4out, haha *gama4, haha *beta4, haha *burelu4out, haha *D_sw2)//A输入， B卷积核1， D硬件输出， D_sw软件输出， W卷积核2
{
	//读取W3
	FILE *fp7;
	fp7 = fopen("E:/毕业论文/sdsoc code/Project2/mnt/conv3.txt", "r");
	if (fp7 == NULL) {
		printf("sadioj111111111gsaoig");
	}
	for (int i = 0; i < OUT_CH_NUM_Conv3 * OUT_CH_NUM_Conv2 * Conv_MAP_SIZE * Conv_MAP_SIZE; i++) {
		fscanf(fp7, "%f", &W3[i]);
	}
	fclose(fp7);

	//读取gama3
	FILE *fp8;
	float buffergama3[OUT_CH_NUM_Conv3];
	fp8 = fopen("E:/毕业论文/sdsoc code/Project2/mnt/gama3.txt", "r");
	if (fp8 == NULL) {
		printf("sadiojg222222sao1ig");
	}
	for (int i = 0; i < OUT_CH_NUM_Conv3; i++) {
		fscanf(fp8, "%f", &buffergama3[i]);
	}

	fclose(fp8);

	//读取beta3
	FILE *fp9;
	float bufferbeta3[OUT_CH_NUM_Conv3];
	fp9 = fopen("E:/毕业论文/sdsoc code/Project2/mnt/beta3.txt", "r");
	if (fp9 == NULL) {
		printf("sadiojgs333333ao1ig");
	}
	for (int i = 0; i < OUT_CH_NUM_Conv3; i++) {
		fscanf(fp9, "%f", &bufferbeta3[i]);
	}

	fclose(fp9);

	//读取W4
	FILE *fp10;
	fp10 = fopen("E:/毕业论文/sdsoc code/Project2/mnt/conv4.txt", "r");
	if (fp10 == NULL) {
		printf("sadiojgs4444444444ao1ig");
	}
	for (int i = 0; i < OUT_CH_NUM_Conv4 * OUT_CH_NUM_Conv3 * Conv_MAP_SIZE * Conv_MAP_SIZE; i++) {
		fscanf(fp10, "%f", &W4[i]);
	}

	fclose(fp10);

	//读取gama4
	FILE *fp11;
	float buffergama4[OUT_CH_NUM_Conv4];
	fp11 = fopen("E:/毕业论文/sdsoc code/Project2/mnt/gama4.txt", "r");
	if (fp11 == NULL) {
		printf("sadio555555555jgs4aoig");
	}
	for (int i = 0; i < OUT_CH_NUM_Conv4; i++) {
		fscanf(fp11, "%f", &buffergama4[i]);
	}

	fclose(fp11);

	//读取beta4
	FILE *fp12;
	float bufferbeta4[OUT_CH_NUM_Conv4];
	fp12 = fopen("E:/毕业论文/sdsoc code/Project2/mnt/beta4.txt", "r");
	if (fp12 == NULL) {
		printf("sadioj666666gsao1ig");
	}
	for (int i = 0; i < OUT_CH_NUM_Conv4; i++) {
		fscanf(fp12, "%f", &bufferbeta4[i]);
	}

	fclose(fp12);

	//初始化

	for (int i = 0; i < OUT_CH_NUM_Conv3; i++) {
		gama3[i] = buffergama3[i];
	}

	for (int i = 0; i < OUT_CH_NUM_Conv3; i++) {
		beta3[i] = bufferbeta3[i];
	}

	for (int i = 0; i < OUT_CH_NUM_Conv4; i++) {
		gama4[i] = buffergama4[i];
	}

	for (int i = 0; i < OUT_CH_NUM_Conv4; i++) {
		beta4[i] = bufferbeta4[i];
	}

	for (int i = 0; i < OUT_CH_NUM_Conv3 * OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3; i++) {
		conv3out[i] = 0;
	}

	for (int i = 0; i < OUT_CH_NUM_Conv3 * OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3; i++) {
		burelu3out[i] = 0;
	}

	for (int i = 0; i < OUT_CH_NUM_Conv4 * OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4; i++) {
		conv4out[i] = 0;
	}

	for (int i = 0; i < OUT_CH_NUM_Conv4 * OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4; i++) {
		burelu4out[i] = 0;
	}

	for (int i = 0; i < 256 * 5 * 5; i++) {
		D_sw2[i] = 0;
	}
}

#pragma SDS data zero_copy(A[0:IN_CH_NUM_Conv*IN_HEIGHT_Conv*IN_WIDTH_Conv], W1[0:OUT_CH_NUM_Conv / 16 *IN_CH_NUM_Conv*Conv_MAP_SIZE*Conv_MAP_SIZE], conv1out[0:OUT_CH_NUM_Conv*OUT_HEIGHT_Conv*OUT_WIDTH_Conv / 16])
void conv1(haha *A, haha *W1, haha *conv1out) {
	int times = 16;
	haha tempA[IN_CH_NUM_Conv][IN_HEIGHT_Conv][IN_WIDTH_Conv];
	haha tempB[OUT_CH_NUM_Conv / 16][IN_CH_NUM_Conv][Conv_MAP_SIZE][Conv_MAP_SIZE];
	haha tempconv1out[OUT_CH_NUM_Conv / 16][OUT_HEIGHT_Conv][OUT_WIDTH_Conv] = { 0 };

	for (int cyc = 0; cyc < OUT_CH_NUM_Conv; cyc += OUT_CH_NUM_Conv / times)
	{
		memcpy((void*)tempA, (const void*)A, sizeof(haha) * IN_CH_NUM_Conv * IN_HEIGHT_Conv * IN_WIDTH_Conv);
		/*for (int i = 0; i < IN_CH_NUM_Conv; i++) {
			for (int j = 0; j < IN_HEIGHT_Conv; j++) {
				for (int k = 0; k < IN_WIDTH_Conv; k++) {
					printf("%f ", tempA[i][j][k]);
				}
			}
		}*/
		memcpy((void*)tempB, W1 + IN_CH_NUM_Conv * Conv_MAP_SIZE * Conv_MAP_SIZE * cyc, sizeof(haha) * OUT_CH_NUM_Conv * IN_CH_NUM_Conv * Conv_MAP_SIZE * Conv_MAP_SIZE / times);
		/*for (int i = 0; i < OUT_CH_NUM_Conv; i++) {
			for (int j = 0; j < IN_CH_NUM_Conv; j++) {
				for (int k = 0; k < Conv_MAP_SIZE; k++) {
					for (int l = 0; l < Conv_MAP_SIZE; l++) {
						printf("%f ", tempB[i][j][k][l]);
					}
				}
			}
		}*/
		for (int kr = 0; kr < Conv_MAP_SIZE; kr++)//3
		{
			for (int kc = 0; kc < Conv_MAP_SIZE; kc++)//3
			{
				for (int r = 0; r < OUT_HEIGHT_Conv; r++)//30
				{
					for (int c = 0; c < OUT_WIDTH_Conv; c++)//30
					{
						for (int cho = 0; cho < OUT_CH_NUM_Conv / times; cho++)//128
						{
							for (int chi = 0; chi < IN_CH_NUM_Conv; chi++)//3
							{
								tempconv1out[cho][r][c] += tempA[chi][r + kr][c + kc] * tempB[cho][chi][kr][kc];
							}
						}
					}
				}
			}
		}
		memcpy(conv1out + cyc * OUT_HEIGHT_Conv * OUT_WIDTH_Conv, (const void*)tempconv1out, sizeof(haha) * OUT_CH_NUM_Conv * OUT_HEIGHT_Conv * OUT_WIDTH_Conv / times);

		/*for (int i = 0; i < OUT_CH_NUM_Conv; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv; k++) {
					printf("%f ", tempconv1out[i][j][k]);
				}
			}
		}*/
		for (int i = 0; i < OUT_CH_NUM_Conv / times; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv; k++) {
					tempconv1out[i][j][k] = 0;
				}
			}
		}
	}
}

#pragma SDS data zero_copy(conv1out[0:OUT_CH_NUM_Conv*OUT_HEIGHT_Conv*OUT_WIDTH_Conv / 16], gama1[0:OUT_CH_NUM_Conv / 16], beta1[0:OUT_CH_NUM_Conv / 16], burelu1out[0:OUT_CH_NUM_Conv*OUT_HEIGHT_Conv*OUT_WIDTH_Conv / 16])
void bnrelu1(haha *conv1out, haha *gama1, haha *beta1, haha *burelu1out) {
	haha tempconv1out[OUT_CH_NUM_Conv / 16][OUT_HEIGHT_Conv][OUT_WIDTH_Conv];
	haha tempgama1[OUT_CH_NUM_Conv / 16];
	haha tempbeta1[OUT_CH_NUM_Conv / 16];
	haha tempbn1[OUT_CH_NUM_Conv / 16][OUT_HEIGHT_Conv][OUT_WIDTH_Conv] = { 0 };
	//haha tempburelu1out[OUT_CH_NUM_Conv][OUT_HEIGHT_Conv][OUT_WIDTH_Conv] = { 0 };
	for (int cyc = 0; cyc < OUT_CH_NUM_Conv; cyc += OUT_CH_NUM_Conv / 16)
	{
		memcpy((void*)tempconv1out, conv1out + OUT_HEIGHT_Conv * OUT_WIDTH_Conv * cyc, sizeof(haha) * OUT_CH_NUM_Conv * OUT_HEIGHT_Conv * OUT_WIDTH_Conv / 16);
		memcpy((void*)tempgama1, gama1 + cyc, sizeof(haha) * OUT_CH_NUM_Conv / 16);
		memcpy((void*)tempbeta1, beta1 + cyc, sizeof(haha) * OUT_CH_NUM_Conv / 16);

		for (int i = 0; i < OUT_CH_NUM_Conv / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv; k++) {
					tempbn1[i][j][k] = tempgama1[i] * tempconv1out[i][j][k] + tempbeta1[i];
					tempbn1[i][j][k] = (tempbn1[i][j][k] > 0) ? tempbn1[i][j][k] : ((haha)0);
				}
			}
		}
		memcpy(burelu1out + cyc * OUT_HEIGHT_Conv * OUT_WIDTH_Conv, (const void*)tempbn1, sizeof(haha) * OUT_CH_NUM_Conv * OUT_HEIGHT_Conv * OUT_WIDTH_Conv / 16);



		for (int i = 0; i < OUT_CH_NUM_Conv / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv; k++) {
					tempbn1[i][j][k] = 0;
				}
			}
		}
	}
}

#pragma SDS data zero_copy(burelu1out[0:OUT_CH_NUM_Conv*OUT_HEIGHT_Conv*OUT_WIDTH_Conv], W2[0:OUT_CH_NUM_Conv2 / 16 * OUT_CH_NUM_Conv * Conv_MAP_SIZE * Conv_MAP_SIZE], conv2out[0:OUT_CH_NUM_Conv2 / 16 *OUT_HEIGHT_Conv2*OUT_WIDTH_Conv2])
void conv2(haha *burelu1out, haha *W2, haha *conv2out) {
	haha tempburelu1out[OUT_CH_NUM_Conv][OUT_HEIGHT_Conv][OUT_WIDTH_Conv];
	haha tempW2[OUT_CH_NUM_Conv2 / 16][OUT_CH_NUM_Conv][Conv_MAP_SIZE][Conv_MAP_SIZE];
	haha tempconv2out[OUT_CH_NUM_Conv2 / 16][OUT_HEIGHT_Conv2][OUT_WIDTH_Conv2] = { 0 };

	for (int cyc = 0; cyc < OUT_CH_NUM_Conv2; cyc += OUT_CH_NUM_Conv2 / 16)
	{
		memcpy((void*)tempburelu1out, (const void*)burelu1out, sizeof(haha) * OUT_CH_NUM_Conv * OUT_HEIGHT_Conv * OUT_WIDTH_Conv);
		/*for (int i = 0; i < OUT_CH_NUM_Conv; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv; k++) {
					printf("%f ", tempburelu1out[i][j][k]);
				}
			}
		}*/
		memcpy((void*)tempW2, W2 + Conv_MAP_SIZE * Conv_MAP_SIZE * OUT_CH_NUM_Conv * cyc , sizeof(haha) * OUT_CH_NUM_Conv2 * OUT_CH_NUM_Conv * Conv_MAP_SIZE * Conv_MAP_SIZE / 16);

		for (int kr = 0; kr < Conv_MAP_SIZE; kr++)//3
		{
			for (int kc = 0; kc < Conv_MAP_SIZE; kc++)//3
			{
				for (int r = 0; r < OUT_HEIGHT_Conv2; r++)//28
				{
					for (int c = 0; c < OUT_WIDTH_Conv2; c++)//28
					{
						for (int cho = 0; cho < OUT_CH_NUM_Conv2 / 16; cho++)//64
						{
							for (int chi = 0; chi < OUT_CH_NUM_Conv; chi++)//128
							{
								tempconv2out[cho][r][c] += tempburelu1out[chi][r + kr][c + kc] * tempW2[cho][chi][kr][kc];
							}
						}
					}
				}
			}
		}
		memcpy(conv2out + cyc * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2, (const void*)tempconv2out, sizeof(haha) * OUT_CH_NUM_Conv2 / 16 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2);
		/*for (int i = 0; i < OUT_CH_NUM_Conv2 / 8; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv2; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv2; k++) {
					printf("%f ", tempconv2out[i][j][k]);
				}
			}
		}*/
		for (int i = 0; i < OUT_CH_NUM_Conv2 / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv2; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv2; k++) {
					tempconv2out[i][j][k] = 0;
				}
			}
		}
	}

}

#pragma SDS data zero_copy(conv2out[0:OUT_CH_NUM_Conv2*OUT_HEIGHT_Conv2*OUT_WIDTH_Conv2 / 16], gama2[0:OUT_CH_NUM_Conv2 / 16], beta2[0:OUT_CH_NUM_Conv2 / 16], burelu2out[0:OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2 / 16])
void bnrelu2(haha *conv2out, haha *gama2, haha *beta2, haha *burelu2out) {
	haha tempconv2out[OUT_CH_NUM_Conv2 / 16][OUT_HEIGHT_Conv2][OUT_WIDTH_Conv2];
	//haha tempburelu2out[OUT_CH_NUM_Conv2][OUT_HEIGHT_Conv2][OUT_WIDTH_Conv2] = { 0 };
	haha tempgama2[OUT_CH_NUM_Conv2 / 16];
	haha tempbeta2[OUT_CH_NUM_Conv2 / 16];
	haha tempbn2[OUT_CH_NUM_Conv2 / 16][OUT_HEIGHT_Conv2][OUT_WIDTH_Conv2] = { 0 };
	for (int cyc = 0; cyc < OUT_CH_NUM_Conv2; cyc += OUT_CH_NUM_Conv2 / 16)
	{
		memcpy((void*)tempconv2out, conv2out + OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2 * cyc, sizeof(haha) * OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2 / 16);
		memcpy((void*)tempgama2, gama2 + cyc, sizeof(haha) * OUT_CH_NUM_Conv2 / 16);
		memcpy((void*)tempbeta2, beta2 + cyc, sizeof(haha) * OUT_CH_NUM_Conv2 / 16);

		for (int i = 0; i < OUT_CH_NUM_Conv2 / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv2; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv2; k++) {
					tempbn2[i][j][k] = tempgama2[i] * tempconv2out[i][j][k] + tempbeta2[i];
					tempbn2[i][j][k] = (tempbn2[i][j][k] > 0) ? tempbn2[i][j][k] : ((haha)0);
				}
			}
		}
		memcpy(burelu2out + cyc * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2, (const void*)tempbn2, sizeof(haha) * OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2 / 16);
		for (int i = 0; i < OUT_CH_NUM_Conv2 / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv2; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv2; k++) {
					tempbn2[i][j][k] = 0;
				}
			}
		}
	}
}

#pragma SDS data zero_copy(burelu2out[0:OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2 / 16], pooling1out[0:OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_WIDTH_Conv2 / 2 / 16])
void pooling1(haha *burelu2out, haha *pooling1out) {
	haha tempburelu2out[OUT_CH_NUM_Conv2 / 16][OUT_HEIGHT_Conv2][OUT_WIDTH_Conv2];
	haha temppooling1out[OUT_CH_NUM_Conv2 / 16][OUT_HEIGHT_Conv2 / 2][OUT_WIDTH_Conv2 / 2] = { 0 };

	for (int cyc = 0; cyc < OUT_CH_NUM_Conv2; cyc += OUT_CH_NUM_Conv2 / 16)
	{
		memcpy((void*)tempburelu2out, burelu2out + cyc * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2, sizeof(haha) * OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2 / 16);

		for (int i = 0; i < OUT_CH_NUM_Conv2 / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv2 / 2; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv2 / 2; k++) {
					temppooling1out[i][j][k] = Max(Max(tempburelu2out[i][2 * j][2 * k], tempburelu2out[i][2 * j][2 * k + 1]), Max(tempburelu2out[i][2 * j + 1][2 * k], tempburelu2out[i][2 * j + 1][2 * k + 1]));
				}
			}
		}

		/*  展开
		for (int ii = 0; ii < OUT_HEIGHT_Conv2 / 2; ii++) {
			for (int jj = 0; jj < OUT_WIDTH_Conv2 / 2; jj++) {
				for (int kk = 0; kk < OUT_CH_NUM_Conv2; kk++) {
					templc[(ii * OUT_WIDTH_Conv2 / 2 + jj) * OUT_CH_NUM_Conv2 + kk] = temppooling2out[kk][ii][jj];
				}
			}
		}*/
		memcpy(pooling1out + cyc * OUT_HEIGHT_Conv2 / 2 * OUT_WIDTH_Conv2 / 2, (const void*)temppooling1out, sizeof(haha) * OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_WIDTH_Conv2 / 2 / 16);

		for (int i = 0; i < OUT_CH_NUM_Conv2 / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv2 / 2; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv2 / 2; k++) {
					//printf("%f ", temppooling1out[i][j][k]);
					temppooling1out[i][j][k] = 0;
				}
			}
		}
	}
}

#pragma SDS data zero_copy(pooling1out[0:OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_WIDTH_Conv2 / 2 / 16], W3[0:OUT_CH_NUM_Conv3 / 16 * OUT_CH_NUM_Conv2 * Conv_MAP_SIZE * Conv_MAP_SIZE], conv3out[0:OUT_CH_NUM_Conv3 / 16 * OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3])
void conv3(haha *pooling1out, haha *W3, haha *conv3out) {
	int times = 16;
	haha temppooling1out[OUT_CH_NUM_Conv2][OUT_HEIGHT_Conv2 / 2][OUT_WIDTH_Conv2 / 2];
	haha tempW3[OUT_CH_NUM_Conv3 / 16][OUT_CH_NUM_Conv2][Conv_MAP_SIZE][Conv_MAP_SIZE];
	haha tempconv3out[OUT_CH_NUM_Conv3 / 16][OUT_HEIGHT_Conv3][OUT_WIDTH_Conv3] = { 0 };

	for (int cyc = 0; cyc < OUT_CH_NUM_Conv3; cyc += OUT_CH_NUM_Conv3 / times)
	{
		memcpy((void*)temppooling1out, (const void*)pooling1out, sizeof(haha) * OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_WIDTH_Conv2 / 2);
		/*for (int i = 0; i < IN_CH_NUM_Conv; i++) {
			for (int j = 0; j < IN_HEIGHT_Conv; j++) {
				for (int k = 0; k < IN_WIDTH_Conv; k++) {
					printf("%f ", tempA[i][j][k]);
				}
			}
		}*/
		memcpy((void*)tempW3, W3 + OUT_CH_NUM_Conv2 * Conv_MAP_SIZE * Conv_MAP_SIZE * cyc, sizeof(haha) * OUT_CH_NUM_Conv3 / times * OUT_CH_NUM_Conv2 * Conv_MAP_SIZE * Conv_MAP_SIZE);
		/*for (int i = 0; i < OUT_CH_NUM_Conv; i++) {
			for (int j = 0; j < IN_CH_NUM_Conv; j++) {
				for (int k = 0; k < Conv_MAP_SIZE; k++) {
					for (int l = 0; l < Conv_MAP_SIZE; l++) {
						printf("%f ", tempB[i][j][k][l]);
					}
				}
			}
		}*/
		for (int kr = 0; kr < Conv_MAP_SIZE; kr++)//3
		{
			for (int kc = 0; kc < Conv_MAP_SIZE; kc++)//3
			{
				for (int r = 0; r < OUT_HEIGHT_Conv3; r++)//12
				{
					for (int c = 0; c < OUT_WIDTH_Conv3; c++)//12
					{
						for (int cho = 0; cho < OUT_CH_NUM_Conv3 / times; cho++)//256/4
						{
							for (int chi = 0; chi < OUT_CH_NUM_Conv2; chi++)//128
							{
								tempconv3out[cho][r][c] += temppooling1out[chi][r + kr][c + kc] * tempW3[cho][chi][kr][kc];
							}
						}
					}
				}
			}
		}
		memcpy(conv3out + cyc * OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3, (const void*)tempconv3out, sizeof(haha) * OUT_CH_NUM_Conv3 * OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3 / times);

		/*for (int i = 0; i < OUT_CH_NUM_Conv; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv; k++) {
					printf("%f ", tempconv1out[i][j][k]);
				}
			}
		}*/
		for (int i = 0; i < OUT_CH_NUM_Conv3 / times; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv3; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv3; k++) {
					//printf("%f ", tempconv3out[i][j][k]);
					tempconv3out[i][j][k] = 0;
				}
			}
		}
	}
}

#pragma SDS data zero_copy(conv3out[0:OUT_CH_NUM_Conv3 / 16 * OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3], gama3[0:OUT_CH_NUM_Conv3 / 16], beta3[0:OUT_CH_NUM_Conv3 / 16], burelu3out[0:OUT_CH_NUM_Conv3 / 16 * OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3])
void bnrelu3(haha *conv3out, haha *gama3, haha *beta3, haha *burelu3out) {
	haha tempconv3out[OUT_CH_NUM_Conv3 / 16][OUT_HEIGHT_Conv3][OUT_WIDTH_Conv3];
	haha tempgama3[OUT_CH_NUM_Conv3 / 16];
	haha tempbeta3[OUT_CH_NUM_Conv3 / 16];
	haha tempbn3[OUT_CH_NUM_Conv3 / 16][OUT_HEIGHT_Conv3][OUT_WIDTH_Conv3] = { 0 };
	//haha tempburelu1out[OUT_CH_NUM_Conv][OUT_HEIGHT_Conv][OUT_WIDTH_Conv] = { 0 };
	for (int cyc = 0; cyc < OUT_CH_NUM_Conv3; cyc += OUT_CH_NUM_Conv3 / 16)
	{
		memcpy((void*)tempconv3out, conv3out + OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3 * cyc, sizeof(haha) * OUT_CH_NUM_Conv3 * OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3 / 16);
		memcpy((void*)tempgama3, gama3 + cyc, sizeof(haha) * OUT_CH_NUM_Conv3 / 16);
		memcpy((void*)tempbeta3, beta3 + cyc, sizeof(haha) * OUT_CH_NUM_Conv3 / 16);

		for (int i = 0; i < OUT_CH_NUM_Conv3 / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv3; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv3; k++) {
					tempbn3[i][j][k] = tempgama3[i] * tempconv3out[i][j][k] + tempbeta3[i];
					tempbn3[i][j][k] = (tempbn3[i][j][k] > 0) ? tempbn3[i][j][k] : ((haha)0);
				}
			}
		}
		memcpy(burelu3out + cyc * OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3, (const void*)tempbn3, sizeof(haha) * OUT_CH_NUM_Conv3 * OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3 / 16);

		/*for (int i = 0; i < OUT_CH_NUM_Conv3 / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv3; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv3; k++) {
					printf("%f ", tempbn3[i][j][k]);
				}
			}
		}*/

		for (int i = 0; i < OUT_CH_NUM_Conv3 / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv3; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv3; k++) {
					tempbn3[i][j][k] = 0;
				}
			}
		}
	}
}

#pragma SDS data zero_copy(burelu3out[0:OUT_CH_NUM_Conv3 / 16 * OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3], W4[0:OUT_CH_NUM_Conv4 / 16 * OUT_CH_NUM_Conv3 * Conv_MAP_SIZE * Conv_MAP_SIZE], conv4out[0:OUT_CH_NUM_Conv4 / 16 * OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4])
void conv4(haha *burelu3out, haha *W4, haha *conv4out) {
	int times = 16;
	haha tempburelu3out[OUT_CH_NUM_Conv3][OUT_HEIGHT_Conv3][OUT_WIDTH_Conv3];
	haha tempW4[OUT_CH_NUM_Conv4 / 16][OUT_CH_NUM_Conv3][Conv_MAP_SIZE][Conv_MAP_SIZE];
	haha tempconv4out[OUT_CH_NUM_Conv4 / 16][OUT_HEIGHT_Conv4][OUT_WIDTH_Conv4] = { 0 };

	for (int cyc = 0; cyc < OUT_CH_NUM_Conv4; cyc += OUT_CH_NUM_Conv4 / times)
	{
		memcpy((void*)tempburelu3out, (const void*)burelu3out, sizeof(haha) * OUT_CH_NUM_Conv3 * OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3);
		/*for (int i = 0; i < OUT_CH_NUM_Conv3; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv3; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv3; k++) {
					printf("%f ", tempburelu3out[i][j][k]);
				}
			}
		}*/
		memcpy((void*)tempW4, W4 + OUT_CH_NUM_Conv3 * Conv_MAP_SIZE * Conv_MAP_SIZE * cyc, sizeof(haha) * OUT_CH_NUM_Conv4 / times * OUT_CH_NUM_Conv3 * Conv_MAP_SIZE * Conv_MAP_SIZE);
		/*for (int i = 0; i < OUT_CH_NUM_Conv; i++) {
			for (int j = 0; j < IN_CH_NUM_Conv; j++) {
				for (int k = 0; k < Conv_MAP_SIZE; k++) {
					for (int l = 0; l < Conv_MAP_SIZE; l++) {
						printf("%f ", tempB[i][j][k][l]);
					}
				}
			}
		}*/
		for (int kr = 0; kr < Conv_MAP_SIZE; kr++)//3
		{
			for (int kc = 0; kc < Conv_MAP_SIZE; kc++)//3
			{
				for (int r = 0; r < OUT_HEIGHT_Conv4; r++)//10
				{
					for (int c = 0; c < OUT_WIDTH_Conv4; c++)//10
					{
						for (int cho = 0; cho < OUT_CH_NUM_Conv4 / times; cho++)//256/4
						{
							for (int chi = 0; chi < OUT_CH_NUM_Conv3; chi++)//128
							{
								tempconv4out[cho][r][c] += tempburelu3out[chi][r + kr][c + kc] * tempW4[cho][chi][kr][kc];
							}
						}
					}
				}
			}
		}
		memcpy(conv4out + cyc * OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4, (const void*)tempconv4out, sizeof(haha) * OUT_CH_NUM_Conv4 * OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4 / times);

		for (int i = 0; i < OUT_CH_NUM_Conv4 / times; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv4; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv4; k++) {
					//printf("%f ", tempconv4out[i][j][k]);
					tempconv4out[i][j][k] = 0;
				}
			}
		}
	}
}

#pragma SDS data zero_copy(conv4out[0:OUT_CH_NUM_Conv4 / 16 * OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4], gama3[0:OUT_CH_NUM_Conv4 / 16], beta3[0:OUT_CH_NUM_Conv4 / 16], burelu4out[0:OUT_CH_NUM_Conv4 / 16 * OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4])
void bnrelu4(haha *conv4out, haha *gama4, haha *beta4, haha *burelu4out) {
	haha tempconv4out[OUT_CH_NUM_Conv4 / 16][OUT_HEIGHT_Conv4][OUT_WIDTH_Conv4];
	haha tempgama4[OUT_CH_NUM_Conv4 / 16];
	haha tempbeta4[OUT_CH_NUM_Conv4 / 16];
	haha tempbn4[OUT_CH_NUM_Conv4 / 16][OUT_HEIGHT_Conv4][OUT_WIDTH_Conv4] = { 0 };
	//haha tempburelu1out[OUT_CH_NUM_Conv][OUT_HEIGHT_Conv][OUT_WIDTH_Conv] = { 0 };
	for (int cyc = 0; cyc < OUT_CH_NUM_Conv4; cyc += OUT_CH_NUM_Conv4 / 16)
	{
		memcpy((void*)tempconv4out, conv4out + OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4 * cyc, sizeof(haha) * OUT_CH_NUM_Conv4 * OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4 / 16);
		memcpy((void*)tempgama4, gama4 + cyc, sizeof(haha) * OUT_CH_NUM_Conv4 / 16);
		memcpy((void*)tempbeta4, beta4 + cyc, sizeof(haha) * OUT_CH_NUM_Conv4 / 16);

		for (int i = 0; i < OUT_CH_NUM_Conv4 / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv4; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv4; k++) {
					tempbn4[i][j][k] = tempgama4[i] * tempconv4out[i][j][k] + tempbeta4[i];
					tempbn4[i][j][k] = (tempbn4[i][j][k] > 0) ? tempbn4[i][j][k] : ((haha)0);
				}
			}
		}
		memcpy(burelu4out + cyc * OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4, (const void*)tempbn4, sizeof(haha) * OUT_CH_NUM_Conv4 * OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4 / 16);

		/*for (int i = 0; i < OUT_CH_NUM_Conv4 / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv4; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv4; k++) {
					printf("%f ", tempbn4[i][j][k]);
				}
			}
		}*/

		for (int i = 0; i < OUT_CH_NUM_Conv4 / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv4; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv4; k++) {
					tempbn4[i][j][k] = 0;
				}
			}
		}
	}
}

//#pragma SDS data zero_copy(burelu4out[0:OUT_CH_NUM_Conv4 / 16 * OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4], pooling2out[0:OUT_CH_NUM_Conv4 / 16 * OUT_HEIGHT_Conv4 / 2 * OUT_WIDTH_Conv4 / 2])
void pooling2(haha *burelu4out, haha *pooling2out) {
	haha tempburelu4out[OUT_CH_NUM_Conv4 / 16][OUT_HEIGHT_Conv4][OUT_WIDTH_Conv4];
	haha temppooling2out[OUT_CH_NUM_Conv4 / 16][OUT_HEIGHT_Conv4 / 2][OUT_WIDTH_Conv4 / 2] = { 0 };

	for (int cyc = 0; cyc < OUT_CH_NUM_Conv4; cyc += OUT_CH_NUM_Conv4 / 16)
	{
		memcpy((void*)tempburelu4out, burelu4out + OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4 * cyc, sizeof(haha) * OUT_CH_NUM_Conv4 * OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4 / 16);

		for (int i = 0; i < OUT_CH_NUM_Conv4 / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv4 / 2; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv4 / 2; k++) {
					temppooling2out[i][j][k] = Max(Max(tempburelu4out[i][2 * j][2 * k], tempburelu4out[i][2 * j][2 * k + 1]), Max(tempburelu4out[i][2 * j + 1][2 * k], tempburelu4out[i][2 * j + 1][2 * k + 1]));
				}
			}
		}

		/*  展开
		for (int ii = 0; ii < OUT_HEIGHT_Conv2 / 2; ii++) {
			for (int jj = 0; jj < OUT_WIDTH_Conv2 / 2; jj++) {
				for (int kk = 0; kk < OUT_CH_NUM_Conv2; kk++) {
					templc[(ii * OUT_WIDTH_Conv2 / 2 + jj) * OUT_CH_NUM_Conv2 + kk] = temppooling2out[kk][ii][jj];
				}
			}
		}*/
		memcpy(pooling2out + OUT_HEIGHT_Conv4 / 2 * OUT_WIDTH_Conv4 / 2 * cyc, (const void*)temppooling2out, sizeof(haha) * OUT_CH_NUM_Conv4 * OUT_HEIGHT_Conv4 / 2 * OUT_WIDTH_Conv4 / 2 / 16);

		for (int i = 0; i < OUT_CH_NUM_Conv4 / 16; i++) {
			for (int j = 0; j < OUT_HEIGHT_Conv4 / 2; j++) {
				for (int k = 0; k < OUT_WIDTH_Conv4 / 2; k++) {
					printf("%f ", temppooling2out[i][j][k]);
					temppooling2out[i][j][k] = 0;
				}
			}
		}
	}
}

int mmult_test(haha *A, haha *W1, haha *gama1, haha *beta1, haha *W2, haha *gama2, haha *beta2, haha *D_sw,
				haha *W3, haha *gama3, haha *beta3, haha *W4, haha *gama4, haha *beta4, haha *D_sw2)
{
	std::cout << "Testing " << NUM_TESTS << " iterations of " << IN_CH_NUM_Conv << "x" << IN_CH_NUM_Conv
		<< " floating point mmultadd..." << std::endl;

	haha *conv1out, *burelu1out, *conv2out, *burelu2out;
	conv1out = (haha *)sds_alloc(OUT_CH_NUM_Conv * OUT_HEIGHT_Conv * OUT_WIDTH_Conv * sizeof(haha));
	burelu1out = (haha *)sds_alloc(OUT_CH_NUM_Conv * OUT_HEIGHT_Conv * OUT_WIDTH_Conv * sizeof(haha));

	conv2out = (haha *)sds_alloc(OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2 * sizeof(haha));
	burelu2out = (haha *)sds_alloc(OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2 * sizeof(haha));

	haha *conv3out, *burelu3out, *conv4out, *burelu4out;
	conv3out = (haha *)malloc(OUT_CH_NUM_Conv3 * OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3 * sizeof(haha));
	burelu3out = (haha *)malloc(OUT_CH_NUM_Conv3 * OUT_HEIGHT_Conv3 * OUT_WIDTH_Conv3 * sizeof(haha));

	conv4out = (haha *)malloc(OUT_CH_NUM_Conv4 * OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4 * sizeof(haha));
	burelu4out = (haha *)malloc(OUT_CH_NUM_Conv4 * OUT_HEIGHT_Conv4 * OUT_WIDTH_Conv4 * sizeof(haha));

	for (int i = 0; i < NUM_TESTS; i++)
	{
		init_arrays(A, W1, conv1out, gama1, beta1, burelu1out, W2, conv2out, gama2, beta2, burelu2out, D_sw);//A输入， B卷积核1， D硬件输出， D_sw软件输出， W卷积核2
		init_arrays2(W3, conv3out, gama3, beta3, burelu3out, W4, conv4out, gama4, beta4, burelu4out, D_sw2);
		conv1(A, W1, conv1out);
		bnrelu1(conv1out, gama1, beta1, burelu1out);
		conv2(burelu1out, W2, conv2out);
		bnrelu2(conv2out, gama2, beta2, burelu2out);
		pooling1(burelu2out, D_sw);

		conv3(D_sw, W3, conv3out);
		bnrelu3(conv3out, gama3, beta3, burelu3out);
		conv4(burelu3out, W4, conv4out);
		bnrelu4(conv4out, gama4, beta4, burelu4out);
		pooling2(burelu4out, D_sw2);
		/*printf("%f", D_sw[0]);*/
		/*
		float aa = D_sw[0];
		int temp = 0;
		for (int i = 0; i < 10; i++) {
			printf("%d\n", D_sw[i]);
			if (D_sw[i] > aa) {
				temp = i;
				aa = D_sw[i];
			}
		}
		printf("%d", temp);*/
	}
	sds_free(conv1out);
	sds_free(burelu1out);
	sds_free(conv2out);
	sds_free(burelu2out);
	sds_free(conv3out);
	sds_free(burelu3out);
	sds_free(conv4out);
	sds_free(burelu4out);
	return 0;
}

int main(int argc, char* argv[]){
	int test_passed = 0;
    haha *A, *W1, *gama1, *beta1, *W2, *gama2, *beta2, *D_sw;
//read txt

 	A = (haha *)sds_alloc(IN_CH_NUM_Conv * IN_HEIGHT_Conv * IN_WIDTH_Conv * sizeof(haha));
 	W1 = (haha *)sds_alloc(OUT_CH_NUM_Conv * IN_CH_NUM_Conv * Conv_MAP_SIZE * Conv_MAP_SIZE * sizeof(haha));
 	gama1 = (haha *)sds_alloc(OUT_CH_NUM_Conv * sizeof(haha));
 	beta1 = (haha *)sds_alloc(OUT_CH_NUM_Conv * sizeof(haha));
 	W2 = (haha *)sds_alloc(OUT_CH_NUM_Conv2 * OUT_CH_NUM_Conv * Conv_MAP_SIZE * Conv_MAP_SIZE * sizeof(haha));
 	gama2 = (haha *)sds_alloc(OUT_CH_NUM_Conv2 * sizeof(haha));
 	beta2 = (haha *)sds_alloc(OUT_CH_NUM_Conv2 * sizeof(haha));
 	D_sw = (haha *)sds_alloc(128*28*28/4 * sizeof(haha));

	//第二组
 	haha *W3, *gama3, *beta3, *W4, *gama4, *beta4, *D_sw2;
	W3 = (haha *)sds_alloc(OUT_CH_NUM_Conv3 * OUT_CH_NUM_Conv2 * Conv_MAP_SIZE * Conv_MAP_SIZE * sizeof(haha));
	gama3 = (haha *)sds_alloc(OUT_CH_NUM_Conv3 * sizeof(haha));
	beta3 = (haha *)sds_alloc(OUT_CH_NUM_Conv3 * sizeof(haha));
	W4 = (haha *)sds_alloc(OUT_CH_NUM_Conv4 * OUT_CH_NUM_Conv3 * Conv_MAP_SIZE * Conv_MAP_SIZE * sizeof(haha));
	gama4 = (haha *)sds_alloc(OUT_CH_NUM_Conv4 * sizeof(haha));
	beta4 = (haha *)sds_alloc(OUT_CH_NUM_Conv4 * sizeof(haha));
	D_sw2 = (haha *)sds_alloc(256 * 5 * 5 * sizeof(haha));

 	if (!A || !W1 || !gama1 || !beta1 || !W2 || !gama2 || !beta2 || !D_sw) {
 			if (A) sds_free(A);
 			if (W1) sds_free(W1);
 			if (gama1) sds_free(gama1);
 			if (beta1) sds_free(beta1);
 			if (W2) sds_free(W2);
 			if (gama2) sds_free(gama2);
 			if (beta2) sds_free(beta2);
 			if (D_sw) sds_free(D_sw);
 			return 2;
 		}

	if (!W3 || !gama3 || !beta3 || !W4 || !gama4 || !beta4 || !D_sw2) {
		if (W3) free(W3);
		if (gama3) free(gama3);
		if (beta3) free(beta3);
		if (W4) free(W4);
		if (gama4) free(gama4);
		if (beta4) free(beta4);
		if (D_sw2) free(D_sw2);
		return 2;
	}

 	test_passed = mmult_test(A, W1, gama1, beta1, W2, gama2, beta2, D_sw, W3, gama3, beta3, W4, gama4, beta4, D_sw2);

 	std::cout << "TEST " << (test_passed ? "FAILED" : "PASSED") << std::endl;
     /*sds_free(A);
     sds_free(B);
     sds_free(D);
     free(D_sw);*/

    return (test_passed ? -1 : 0);
}

