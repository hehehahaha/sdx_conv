Row:
for (int r = 0; r < OUT_HEIGHT_Conv; r++)   //24
{
  Column:
  for (int c = 0; c < OUT_WIDTH_Conv; c++)   //24
  {
    Kernel_Row:
    for (int kr = 0; kr < Conv_MAP_SIZE; kr++)    //5
    {
      Kernel_Column:
      for (int kc = 0; kc < Conv_MAP_SIZE; kc++)    //5
      {
        Out_Channel:
        for (int cho = 0; cho < OUT_CH_NUM_Conv; cho++)    //32
	{
          Input_Channel:
	  for (int chi = 0; chi < IN_CH_NUM_Conv; chi++)     //1
	  {
             ImgOut[cho][r][c] += ImgIn[chi][r + kr][c + kc] * weight[cho][chi][kr][kc];
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

Row:
for (int r = 0; r < OUT_HEIGHT_Conv; r++)   //24
{
  Column:
  for (int c = 0; c < OUT_WIDTH_Conv; c++)   //24
  {
    Kernel_Row:
    for (int kr = 0; kr < Conv_MAP_SIZE; kr++)    //5
    {
      Kernel_Column:
      for (int kc = 0; kc < Conv_MAP_SIZE; kc++)    //5
      {
        Out_Channel:
        for (int cho = 0; cho < OUT_CH_NUM_Conv; cho++)    //32
	{
#pragma HLS UNROLL
          Input_Channel:
	  for (int chi = 0; chi < IN_CH_NUM_Conv; chi++)     //1
	  {
#pragma HLS UNROLL
             ImgOut[cho][r][c] += ImgIn[chi][r + kr][c + kc] * weight[cho][chi][kr][kc];
	  }
        }
      }  
    }
  }
}



