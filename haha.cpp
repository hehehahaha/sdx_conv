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



