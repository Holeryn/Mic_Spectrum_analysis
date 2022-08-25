#include "transform.h"

#if DEBUG
#include <stdio.h>
#endif

volatile void transform(float *amplitudesS, float *frequencyS,int N){
  float complex temp[N];

  for(int i = 0; i <= N - 1; i++){
    temp[i] = 0;

    for(int j = 0; j <= N - 1; j++){
      //amplitudesS[j] = 0.62 - 0.48*abs(j/N - 1/2) - 0.38*cos(2*M_PI*j/N);

      temp[i] = CMPLX(creal(temp[i]) + amplitudesS[j]*(cos(((2*M_PI)/(N))*i*j)),
   	                  cimag(temp[i]) + amplitudesS[j]*(sin(((2*M_PI)/(N))*i*j)));

#if DEBUG
      printf("-- %f %f %f\n",creal(temp[i]),cimag(temp[i],amplitudesS[i]);
#endif
    }

        frequencyS[i] = cabs(temp[i]) > 100 ? 0 : cabs(temp[i]);
#if DEBUG
    printf("%f %f\n",frequencyS[i],amplitudesS[i]);
#endif
  }

}
