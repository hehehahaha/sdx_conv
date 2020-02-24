Row:
for (int r = 0; r < OUT_HEIGHT_Conv; r++)   // OUT_HEIGHT_Conv = 24
{
  Column:
  for (int c = 0; c < OUT_WIDTH_Conv; c++)   // OUT_WIDTH_Conv = 24
  {
    Kernel_Row:
    for (int kr = 0; kr < Conv_MAP_SIZE; kr++)    // Conv_MAP_SIZE = 5
    {
      Kernel_Column:
      for (int kc = 0; kc < Conv_MAP_SIZE; kc++)    // Conv_MAP_SIZE = 5
      {
        Out_Channel:
        for (int cho = 0; cho < OUT_CH_NUM_Conv; cho++)    // OUT_CH_NUM_Conv = 32
	{
          Input_Channel:
	  for (int chi = 0; chi < IN_CH_NUM_Conv; chi++)     // IN_CH_NUM_Conv = 1
	  {
             ImgOut[cho][r][c] += ImgIn[chi][r + kr][c + kc] * Weight[cho][chi][kr][kc];
	  }
        }
      }  
    }
  }
}


#pragma HLS array_partition variable = ImgIn complete dim = 1
#pragma HLS array_partition variable = ImgOut complete dim = 1
#pragma HLS array_partition variable = weight complete dim = 1
#pragma HLS array_partition variable = weight complete dim = 2

for (int r = 0; r < OUT_HEIGHT_Conv; r++)   // OUT_HEIGHT_Conv = 24
{
  for (int c = 0; c < OUT_WIDTH_Conv; c++)   // OUT_WIDTH_Conv = 24
  {
    for (int kr = 0; kr < Conv_MAP_SIZE; kr++)    // Conv_MAP_SIZE = 5
    {
      for (int kc = 0; kc < Conv_MAP_SIZE; kc++)    // Conv_MAP_SIZE = 5
      {
        for (int cho = 0; cho < OUT_CH_NUM_Conv; cho++)    // OUT_CH_NUM_Conv = 32
	{
#pragma HLS UNROLL
	  for (int chi = 0; chi < IN_CH_NUM_Conv; chi++)     // IN_CH_NUM_Conv = 1
	  {
#pragma HLS UNROLL
             ImgOut[cho][r][c] += ImgIn[chi][r + kr][c + kc] * Weight[cho][chi][kr][kc];
	  }
        }
      }  
    }
  }
}



#pragma HLS array_partition variable = ImgIn complete dim = 1
#pragma HLS array_partition variable = ImgOut complete dim = 1
#pragma HLS array_partition variable = weight complete dim = 1
#pragma HLS array_partition variable = weight complete dim = 2

for (int r = 0; r < OUT_HEIGHT_Conv; r++)   // OUT_HEIGHT_Conv = 24
{
  for (int c = 0; c < OUT_WIDTH_Conv; c++)   // OUT_WIDTH_Conv = 24
  {
    for (int kr = 0; kr < Conv_MAP_SIZE; kr++)    // Conv_MAP_SIZE = 5
    {
      for (int kc = 0; kc < Conv_MAP_SIZE; kc++)    // Conv_MAP_SIZE = 5
      {
#pragma HLS PIPELINE
        for (int cho = 0; cho < OUT_CH_NUM_Conv; cho++)    // OUT_CH_NUM_Conv = 32
	{
	  for (int chi = 0; chi < IN_CH_NUM_Conv; chi++)     // IN_CH_NUM_Conv = 1
	  {
             ImgOut[cho][r][c] += ImgIn[chi][r + kr][c + kc] * Weight[cho][chi][kr][kc];
	  }
        }
      }  
    }
  }
}



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

