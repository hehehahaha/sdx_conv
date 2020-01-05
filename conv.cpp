#include <iostream>
#include <stdlib.h>
#include <stdint.h>

#ifndef NUM_TESTS
#define NUM_TESTS 1
#endif

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

#define Max(A,B) ((A>B)?A:B)

void conv1(haha *A, haha *B, haha *conv1out);
void poolingrelu1(haha *conv1out, haha *pooling1out);
void conv2(haha *pooling1out, haha *W, haha *conv2out);
void poolingrelu2(haha *conv2out, haha *pooling2out);
void fc12(haha *pooling2out, haha *W3, haha *W4, haha *D_sw);

static void init_arrays(haha *A, haha *B, haha *conv1out, haha *pooling1out, haha *W, haha *conv2out, haha *pooling2out, haha *W3, haha *W4, haha *D_sw)//A输入， B卷积核1， D硬件输出， D_sw软件输出， W卷积核2
{
	//读取输入
	FILE *fp0;
	float buffer0[IN_CH_NUM_Conv * IN_HEIGHT_Conv * IN_WIDTH_Conv];
	fp0 = fopen("E:/毕业论文/sdsoc code/Project1/mnt/inp.txt", "r");
	if (fp0 == NULL) {
		printf("sadiojgsaoig");
	}
	for (int i = 0; i < IN_CH_NUM_Conv * IN_HEIGHT_Conv * IN_WIDTH_Conv; i++) {
		fscanf(fp0, "%f", &buffer0[i]);
	}
	fclose(fp0);

	//读取权值1
	FILE *fp;
	float buffer1[Conv_MAP_SIZE * Conv_MAP_SIZE * IN_CH_NUM_Conv * OUT_CH_NUM_Conv];
	fp = fopen("E:/毕业论文/sdsoc code/Project1/mnt/conv1.txt", "r");
	if (fp == NULL) {
		printf("sadiojgsao1ig");
	}
	for (int i = 0; i < Conv_MAP_SIZE * Conv_MAP_SIZE * IN_CH_NUM_Conv * OUT_CH_NUM_Conv; i++) {
		fscanf(fp, "%f", &buffer1[i]);
	}
	/* for (int k = 0; k < 5*5*8; k++){
	  printf("%f", buffer1[k]);
	 }*/
	fclose(fp);

	//读取权值2
	FILE *fp2;
	float buffer2[Conv_MAP_SIZE2 * Conv_MAP_SIZE2 * OUT_CH_NUM_Conv2 * OUT_CH_NUM_Conv];
	fp2 = fopen("E:/毕业论文/sdsoc code/Project1/mnt/conv2.txt", "r");
	if (fp2 == NULL) {
		printf("sadiojgs4aoig");
	}
	for (int i = 0; i < Conv_MAP_SIZE2 * Conv_MAP_SIZE2 * OUT_CH_NUM_Conv2 * OUT_CH_NUM_Conv; i++) {
		fscanf(fp2, "%f", &buffer2[i]);
	}
	/* for (int k = 0; k < 3*3*16*8; k++){
	  printf("%f", buffer2[k]);
	 }*/
	fclose(fp2);

	//读取权值3
	FILE *fp3;
	float buffer3[OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_HEIGHT_Conv2 / 2 * FC1_OUT];
	fp3 = fopen("E:/毕业论文/sdsoc code/Project1/mnt/fc1.txt", "r");
	if (fp3 == NULL) {
		printf("sadiojgs4aoig");
	}
	for (int i = 0; i < OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_HEIGHT_Conv2 / 2 * FC1_OUT; i++) {
		fscanf(fp3, "%f", &buffer3[i]);
	}
	/* for (int k = 0; k < 3*3*16*8; k++){
	  printf("%f", buffer2[k]);
	 }*/
	fclose(fp3);
	
	//读取权值4
	FILE *fp4;
	float buffer4[FC1_OUT * FC2_OUT];
	fp4 = fopen("E:/毕业论文/sdsoc code/Project1/mnt/fc2.txt", "r");
	if (fp4 == NULL) {
		printf("sadiojgs4aoig");
	}
	for (int i = 0; i < FC1_OUT * FC2_OUT; i++) {
		fscanf(fp4, "%f", &buffer4[i]);
	}
	fclose(fp4);
	
	//初始化
	for (int i = 0; i < IN_CH_NUM_Conv * IN_HEIGHT_Conv * IN_WIDTH_Conv; i++) {
		A[i] = buffer0[i];
	}

	for (int i = 0; i < OUT_CH_NUM_Conv*IN_CH_NUM_Conv*Conv_MAP_SIZE*Conv_MAP_SIZE; i++) {
		B[i] = buffer1[i];
	}

	for (int i = 0; i < OUT_CH_NUM_Conv * OUT_HEIGHT_Conv * OUT_WIDTH_Conv; i++) {
		conv1out[i] = 0;
	}

	for (int i = 0; i < OUT_CH_NUM_Conv * OUT_HEIGHT_Conv / 2 * OUT_WIDTH_Conv / 2; i++) {
		pooling1out[i] = 0;
	}

	for (int i = 0; i < OUT_CH_NUM_Conv2*OUT_CH_NUM_Conv*Conv_MAP_SIZE2*Conv_MAP_SIZE2; i++) {
		W[i] = buffer2[i];
	}

	for (int i = 0; i < OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2; i++) {
		conv2out[i] = 0;
	}

	for (int i = 0; i < OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_WIDTH_Conv2 / 2; i++) {
		pooling2out[i] = 0;
	}
	
	for (int i = 0; i < OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_HEIGHT_Conv2 / 2 * FC1_OUT; i++) {
		W3[i] = buffer3[i];
		//printf("%d ", W3[i]);
	}

	for (int i = 0; i < FC1_OUT * FC2_OUT; i++) {
		W4[i] = buffer4[i];
	}

	for (int i = 0; i < OUT_CH_NUM_Conv * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2 / 4; i++) {
		D_sw[i] = 0;
	}
}

void conv1(haha *A, haha *B, haha *conv1out) {
	haha tempA[IN_CH_NUM_Conv][IN_HEIGHT_Conv][IN_WIDTH_Conv];
	haha tempB[OUT_CH_NUM_Conv][IN_CH_NUM_Conv][Conv_MAP_SIZE][Conv_MAP_SIZE];
	haha tempconv1out[OUT_CH_NUM_Conv][OUT_HEIGHT_Conv][OUT_WIDTH_Conv] = {0};

	memcpy((void*)tempA, (const void*)A, sizeof(haha) * 28 * 28);
	memcpy((void*)tempB, (const void*)B, sizeof(haha) * 5 * 5 * 1 * 8);

	for (int cho = 0; cho < OUT_CH_NUM_Conv; cho++)//8
	{
		for (int chi = 0; chi < IN_CH_NUM_Conv; chi++)//1
		{
			for (int kr = 0; kr < Conv_MAP_SIZE; kr++)//5
			{
				for (int kc = 0; kc < Conv_MAP_SIZE; kc++)//5
				{
					for (int r = 0; r < OUT_HEIGHT_Conv; r++)//24
					{
						for (int c = 0; c < OUT_WIDTH_Conv; c++)//24
						{
							tempconv1out[cho][r][c] += tempA[chi][r + kr][c + kc] * tempB[cho][chi][kr][kc];
						}
					}
				}
			}

		}
	}
	memcpy((void*)conv1out, (const void*)tempconv1out, sizeof(haha) * 8 * 24* 24);

}

void poolingrelu1(haha *conv1out, haha *pooling1out) {
	haha tempconv1out[OUT_CH_NUM_Conv][OUT_HEIGHT_Conv][OUT_WIDTH_Conv];
	haha temppooling1[OUT_CH_NUM_Conv][OUT_HEIGHT_Conv / 2][OUT_WIDTH_Conv / 2] = { 0 };
	haha temppooling1out[OUT_CH_NUM_Conv][OUT_HEIGHT_Conv / 2][OUT_WIDTH_Conv / 2] = { 0 };
	memcpy((void*)tempconv1out, (const void*)conv1out, sizeof(haha) * 8 * 24 * 24);

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

void conv2(haha *pooling1out, haha *W, haha *conv2out) {
	haha temppooling1out[OUT_CH_NUM_Conv][OUT_HEIGHT_Conv / 2][OUT_WIDTH_Conv / 2];
	haha tempW[OUT_CH_NUM_Conv2][OUT_CH_NUM_Conv][Conv_MAP_SIZE2][Conv_MAP_SIZE2];
	haha tempconv2out[OUT_CH_NUM_Conv2][OUT_HEIGHT_Conv2][OUT_WIDTH_Conv2] = { 0 };

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

void poolingrelu2(haha *conv2out, haha *pooling2out) {
	haha tempconv2out[OUT_CH_NUM_Conv2][OUT_HEIGHT_Conv2][OUT_WIDTH_Conv2];
	haha temppooling2[OUT_CH_NUM_Conv2][OUT_HEIGHT_Conv2 / 2][OUT_WIDTH_Conv2 / 2] = { 0 };
	haha temppooling2out[OUT_CH_NUM_Conv2][OUT_HEIGHT_Conv2 / 2][OUT_WIDTH_Conv2 / 2] = { 0 };

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
	/*for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 5; j++) {
			for (int k = 0; k < 5; k++) {
				printf("%d ", temppooling2out[i][j][k]);
			}
		}
	}*/
	for (int ii = 0; ii < OUT_HEIGHT_Conv2 / 2; ii++) {
		for (int jj = 0; jj < OUT_WIDTH_Conv2 / 2; jj++) {
			for (int kk = 0; kk < OUT_CH_NUM_Conv2; kk++) {
				templc[(ii * OUT_WIDTH_Conv2 / 2 + jj) * OUT_CH_NUM_Conv2 + kk] = temppooling2out[kk][ii][jj];
			}
		}
	}
	memcpy((void*)pooling2out, (const void*)templc, sizeof(haha) * 16 * 5 * 5);
	//free(tempconv2out);
}

void fc12(haha *pooling2out, haha *W3, haha *W4, haha *D_sw){
	haha temppooling2out[OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_HEIGHT_Conv2 / 2];
	haha tempW3[OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_HEIGHT_Conv2 / 2][FC1_OUT];
	haha tempW4[FC1_OUT][FC2_OUT];
	haha tempD_sw[FC2_OUT] = { 0 };

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
	memcpy((void*)D_sw, (const void*)tempD_sw, sizeof(haha) * 10);
	//free(temppooling2out);
	//free(tempW3);
	//free(tempW4);
}

int mmult_test(haha *A, haha *B, haha *W, haha *W3, haha *W4, haha *D_sw)
{
	std::cout << "Testing " << NUM_TESTS << " iterations of " << IN_CH_NUM_Conv << "x" << IN_CH_NUM_Conv
		<< " floating point mmultadd..." << std::endl;

	haha *conv1out, *pooling1out, *conv2out, *pooling2out;
	conv1out = (haha *)malloc(OUT_CH_NUM_Conv * OUT_HEIGHT_Conv * OUT_WIDTH_Conv * sizeof(haha));
	pooling1out = (haha *)malloc(OUT_CH_NUM_Conv * OUT_HEIGHT_Conv / 2 * OUT_WIDTH_Conv / 2 * sizeof(haha));

	conv2out = (haha *)malloc(OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2 * sizeof(haha));
	pooling2out = (haha *)malloc(OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_WIDTH_Conv2 / 2 * sizeof(haha));

	for (int i = 0; i < NUM_TESTS; i++)
	{
		init_arrays(A, B, conv1out, pooling1out, W, conv2out, pooling2out, W3, W4, D_sw);//A输入， B卷积核1， D硬件输出， D_sw软件输出， W卷积核2
		conv1(A, B, conv1out);
		/*for (int i = 0; i < OUT_CH_NUM_Conv * OUT_HEIGHT_Conv * OUT_WIDTH_Conv; i++) {
			printf("%d ", conv1out[i]);
		}*/
		poolingrelu1(conv1out, pooling1out);
		conv2(pooling1out, W, conv2out);
		poolingrelu2(conv2out, pooling2out);
		fc12(pooling2out, W3, W4, D_sw);

		float aa = D_sw[0];
		int temp = 0;
		for (int i = 0; i < 10; i++) {
			printf("%d\n", D_sw[i]);
			if (D_sw[i] > aa) {
				temp = i;
				aa = D_sw[i];
			}
		}
		printf("%d", temp);
	}

	return 0;
}

int main(int argc, char* argv[]) {
	int test_passed = 0;
	haha *A, *B, *W, *W3, *W4, *D_sw;

	A = (haha *)malloc(IN_CH_NUM_Conv * IN_HEIGHT_Conv * IN_WIDTH_Conv * sizeof(haha));
	B = (haha *)malloc(OUT_CH_NUM_Conv * IN_CH_NUM_Conv * Conv_MAP_SIZE * Conv_MAP_SIZE * sizeof(haha));
	W = (haha *)malloc(OUT_CH_NUM_Conv2 * OUT_CH_NUM_Conv * Conv_MAP_SIZE2 * Conv_MAP_SIZE2 * sizeof(haha));
	W3 = (haha *)malloc(OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_HEIGHT_Conv2 / 2 * FC1_OUT * sizeof(haha));
	W4 = (haha *)malloc(FC1_OUT * FC2_OUT * sizeof(haha));
	D_sw = (haha *)malloc(FC2_OUT * sizeof(haha));

	if (!A || !B || !D_sw || !W || !W3 || !W4) {
		if (A) free(A);
		if (B) free(B);
		if (W) free(W);
		if (D_sw) free(D_sw);
		if (W3) free(W3);
		if (W4) free(W4);
		return 2;
	}

	test_passed = mmult_test(A, B, W, W3, W4, D_sw);

	std::cout << "TEST " << (test_passed ? "FAILED" : "PASSED") << std::endl;

	
	 free(A);
	 free(B);
	 free(W);
	 free(D_sw);
	 free(W3);
	 free(W4);

	system("pause");
	return 0;
	//return (test_passed ? -1 : 0);
}


