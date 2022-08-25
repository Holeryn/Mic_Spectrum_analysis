// This program use portaudio library and the gnuplot directly.
// You can read a documentation for the portaudio library at: http://files.portaudio.com/docs/v19-doxydocs/
// A good part of this code is just a modified version of the patest_read_rector.c in portaudio/test
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include "portaudio.h"
#include "transform.h"

#define G_SEND_COMMAND(m) \
  fprintf(gnuplotPipe,"%s \n",m);

#define ERROR(m)     									\
  	Pa_Terminate(); 									\
	perror("Error:"#m);                            		\
	return 1;

/* #define SAMPLE_RATE  (17932) // Test failure to open with this value. */
#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (1024)
#define NUM_SECONDS     (0.1)
#define NUM_CHANNELS    (2)
/* #define DITHER_FLAG     (paDitherOff)  */
#define DITHER_FLAG     (0) /**/

/* Select sample format. */
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif

// Not a good practise
// but my machine, my rules...
FILE *temp;
PaStream *stream;

static void
handler(int sig){
  PaError err;

   	/* FILE *sfd; */
    /* char text[6*lines]; */
    /* sfd = fopen("saved.temp","wb"); */

    /* temp = fopen("data.temp","w"); */
    /* fread(sfd,sizeof(char)*6,lines,temp); */
    /* perror("prova"); */
    /* printf("%s\n",text); */

    err = Pa_CloseStream( stream );
    if( err != paNoError ) {
      Pa_Terminate();
      perror("closing the stream");
      exit(1);
    }

    fclose(temp);

    // delete data.temp
    remove("data.temp");

    exit(0);
}

int main(void){
  	PaStreamParameters inputParameters;
  	PaError err;
  	int i;
    int totalFrames;
  	int numSamples;
  	int numBytes;
    SAMPLE *recordedSamples;

    time_t *tloc;


    totalFrames = NUM_SECONDS * SAMPLE_RATE; /* Record NUM_SECONDS seconds. */
    numSamples = totalFrames * NUM_CHANNELS;

    float frequencyS[totalFrames]; // Frequency spectrum

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if(sigaction(SIGINT,&sa,NULL) == -1){
      ERROR(sigaction);
    }


    // Open the pipe
    // the -peristent comand is for avoid that gnuplot terminate together with the software
    FILE *gnuplotPipe = popen("gnuplot -persistent","w");
    G_SEND_COMMAND("set title \"Mic monitor\"");
    G_SEND_COMMAND("set title \"Mic monitor\"");
    G_SEND_COMMAND("set style data lines");
    G_SEND_COMMAND("set grid");

    // in "data.temp" will be saved the coordinates of the points in x y format
    temp = fopen("data.temp","w");
    if(temp == NULL){
      ERROR(opening_data);
    }

    numBytes = numSamples * sizeof(SAMPLE);
    recordedSamples = (SAMPLE *) calloc(numSamples,numBytes );
    if( recordedSamples == NULL )
    {
        printf("Could not allocate record array.\n");
        exit(1);
    }
    for( i=0; i<numSamples; i++ ) recordedSamples[i] = 0;

    err = Pa_Initialize();
    if( err != paNoError ) {
      ERROR(1);
    }

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        ERROR(2);
    }
    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    /* Record some audio. -------------------------------------------- */
    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,                  /* &outputParameters, */
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              NULL, /* no callback, use blocking API */
              NULL ); /* no callback, so no callback userData */

    if( err != paNoError ){
      ERROR(3);
    }

    err = Pa_StartStream( stream );
    if( err != paNoError ){
      ERROR(4);
    }


    for(double ms = 0.0; 1 ; ms += NUM_SECONDS){

    err = Pa_ReadStream( stream, recordedSamples, totalFrames );
    if( err != paNoError ) {
      ERROR(5);
    }

    transform(recordedSamples, frequencyS,totalFrames);

    for(int i = 0; i <= totalFrames; i++){
      fprintf(temp, "%lf %lf\n",(float)clock()/CLOCKS_PER_SEC,frequencyS[i]);
    }

    fflush(temp);

    #if !DEBUG
    G_SEND_COMMAND("plot 'data.temp'");
    fflush(gnuplotPipe);
    #endif

    }

    return 0;
}
