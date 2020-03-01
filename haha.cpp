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
	  for (int chi = 0; chi < IN_CH_NUM_Conv; chi++)    // IN_CH_NUM_Conv = 1
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


int times = 4
for (int cyc = 0; cyc < OUT_CH_NUM_Conv; cyc += OUT_CH_NUM_Conv / times)
 {	
  memcpy(ImgIn, In, sizeof(float) * 28 * 28);
  memcpy(Weight, W + 5 * 5 * 1 * cyc, sizeof(float) * 5 * 5 * 1 * 32 / times);
  for (int r = 0; r < OUT_HEIGHT_Conv; r++)   // OUT_HEIGHT_Conv = 24
  {
    for (int c = 0; c < OUT_WIDTH_Conv; c++)   // OUT_WIDTH_Conv = 24
    {
      for (int kr = 0; kr < Conv_MAP_SIZE; kr++)    // Conv_MAP_SIZE = 5
      {
        for (int kc = 0; kc < Conv_MAP_SIZE; kc++)    // Conv_MAP_SIZE = 5
        {
#pragma HLS PIPELINE
          for (int cho = 0; cho < OUT_CH_NUM_Conv / times; cho++)    // OUT_CH_NUM_Conv / times = 32/4
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
  memcpy(Out + 24 * 24 * cyc, ImgOut, sizeof(float) * 24 * 24 * 32 / times);
  for (int i = 0; i < OUT_CH_NUM_Conv / times; i++) {
    for (int j = 0; j < OUT_HEIGHT_Conv; j++) {
      for (int k = 0; k < OUT_WIDTH_Conv; k++) {
	ImgOut[i][j][k] = 0;
      }
    }
  }


	
	
th1 = 0.5
th2 = 1   
Regularization_conv1_weights = tf.reduce_sum(tf.where(
			       tf.greater(conv1_weights, th1), 
	                       tf.square(conv1_weights - th2),tf.where(
			       tf.greater(conv1_weights, -th1),
                               tf.square(conv1_weights), 
			       tf.square(conv1_weights + th2))))

