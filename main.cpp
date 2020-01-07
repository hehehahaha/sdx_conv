#include <iostream>
#include <stdlib.h>
#include <stdint.h>

#include "sds_lib.h"
#include "conv.h"

#ifndef NUM_TESTS
#define NUM_TESTS 1
#endif

#define Max(A,B) ((A>B)?A:B)

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

static void init_arrays(haha *A, haha *B, haha *conv1out, haha *pooling1out, haha *W, haha *conv2out, haha *pooling2out, haha *W3, haha *W4, haha *D_sw, haha *D)//A输入， B卷积核1， D硬件输出， D_sw软件输出， W卷积核2
{
	//读取输入
	FILE *fp0;
	float buffer0[IN_CH_NUM_Conv * IN_HEIGHT_Conv * IN_WIDTH_Conv];
	fp0 = fopen("/mnt/inp.txt", "r");
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
	fp = fopen("/mnt/conv1.txt", "r");
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
	fp2 = fopen("/mnt/conv2.txt", "r");
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
	fp3 = fopen("/mnt/fc1.txt", "r");
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
	fp4 = fopen("/mnt/fc2.txt", "r");
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

	for (int i = 0; i < FC2_OUT; i++) {
		D_sw[i] = 0;
		D[i] = 0;
	}
}

void conv1(haha *A, haha *B, haha *conv1out) {
	haha tempA[IN_CH_NUM_Conv][IN_HEIGHT_Conv][IN_WIDTH_Conv];
	haha tempB[OUT_CH_NUM_Conv/2][IN_CH_NUM_Conv][Conv_MAP_SIZE][Conv_MAP_SIZE];
	haha tempconv1out[OUT_CH_NUM_Conv/2][OUT_HEIGHT_Conv][OUT_WIDTH_Conv] = {0};

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

	for (int ii = 0; ii < OUT_HEIGHT_Conv2 / 2; ii++) {
		for (int jj = 0; jj < OUT_WIDTH_Conv2 / 2; jj++) {
			for (int kk = 0; kk < OUT_CH_NUM_Conv2; kk++) {
				templc[(ii * OUT_WIDTH_Conv2 / 2 + jj) * OUT_CH_NUM_Conv2 + kk] = temppooling2out[kk][ii][jj];
			}
		}
	}
	memcpy((void*)pooling2out, (const void*)templc, sizeof(haha) * 16 * 5 * 5);
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
}

int mmult_test(haha *A, haha *B, haha *W, haha *W3, haha *W4, haha *D_sw, haha *D)
{
     std::cout << "Testing " << NUM_TESTS << " iterations of " << IN_CH_NUM_Conv << "x" << IN_CH_NUM_Conv
               << " floating point mmultadd..." << std::endl;

 	haha *conv1out, *pooling1out, *conv2out, *pooling2out;
 	conv1out = (haha *)malloc(OUT_CH_NUM_Conv * OUT_HEIGHT_Conv * OUT_WIDTH_Conv * sizeof(haha));
 	pooling1out = (haha *)malloc(OUT_CH_NUM_Conv * OUT_HEIGHT_Conv / 2 * OUT_WIDTH_Conv / 2 * sizeof(haha));

 	conv2out = (haha *)malloc(OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 * OUT_WIDTH_Conv2 * sizeof(haha));
 	pooling2out = (haha *)malloc(OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_WIDTH_Conv2 / 2 * sizeof(haha));

 	perf_counter hw_ctr, sw_ctr;

     for (int i = 0; i < NUM_TESTS; i++)
     {
    	 init_arrays(A, B, conv1out, pooling1out, W, conv2out, pooling2out, W3, W4, D_sw, D);//A输入， B卷积核1， D硬件输出， D_sw软件输出， W卷积核2

         // hw_ctr.start();
          conv1_hw(A, B, conv1out);
          poolingrelu1_hw(conv1out, pooling1out);
          conv2_hw(pooling1out, W, conv2out);
          poolingrelu2_hw(conv2out, pooling2out);
          //fc12_hw(pooling2out, W3, W4, D);
         // hw_ctr.stop();

          //sw_ctr.start();
          //conv1(A, B, conv1out);
          //poolingrelu1(conv1out, pooling1out);
          //conv2(pooling1out, W, conv2out);
          //poolingrelu2(conv2out, pooling2out);
          fc12(pooling2out, W3, W4, D_sw);
          //sw_ctr.stop();

          //输出
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

     uint64_t sw_cycles = sw_ctr.avg_cpu_cycles();
     uint64_t hw_cycles = hw_ctr.avg_cpu_cycles();
     double speedup = (double) sw_cycles / (double) hw_cycles;

     std::cout << "Average number of CPU cycles running mmultadd in software: "
               << sw_cycles << std::endl;
     std::cout << "Average number of CPU cycles running mmultadd in hardware: "
               << hw_cycles << std::endl;
     std::cout << "Speed up: " << speedup << std::endl;

     free(conv1out);
     free(pooling1out);
     free(conv2out);
     free(pooling2out);
     return 0;
}

int main(int argc, char* argv[]){
     int test_passed = 0;
     haha *A, *B, *W, *W3, *W4, *D_sw, *D;
//read txt

     A = (haha *)sds_alloc(IN_CH_NUM_Conv * IN_HEIGHT_Conv * IN_WIDTH_Conv * sizeof(haha));
     B = (haha *)sds_alloc(OUT_CH_NUM_Conv * IN_CH_NUM_Conv * Conv_MAP_SIZE * Conv_MAP_SIZE * sizeof(haha));
 	 W = (haha *)malloc(OUT_CH_NUM_Conv2 * OUT_CH_NUM_Conv * Conv_MAP_SIZE2 * Conv_MAP_SIZE2 * sizeof(haha));
 	 W3 = (haha *)malloc(OUT_CH_NUM_Conv2 * OUT_HEIGHT_Conv2 / 2 * OUT_HEIGHT_Conv2 / 2 * FC1_OUT * sizeof(haha));
 	 W4 = (haha *)malloc(FC1_OUT * FC2_OUT * sizeof(haha));
 	 D_sw = (haha *)malloc(FC2_OUT * sizeof(haha));
 	 D = (haha *)malloc(FC2_OUT * sizeof(haha));

     if (!A || !B || !D || !D_sw || !W || !W3|| !W4) {
          if (A) sds_free(A);
          if (B) sds_free(B);
          if (D) sds_free(D);
          if (W) sds_free(W);
          if (D_sw) free(D_sw);
          if (W3) sds_free(W3);
          if (W4) sds_free(W4);
          return 2;
     }

     test_passed = mmult_test(A, B, W, W3, W4, D_sw, D);

     std::cout << "TEST " << (test_passed ? "FAILED" : "PASSED") << std::endl;

     /*sds_free(A);
     sds_free(B);
     sds_free(D);
     free(D_sw);*/

     return (test_passed ? -1 : 0);
}

