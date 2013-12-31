#include <jni.h>
#include <math.h>
#include <fcntl.h>
#include <AudioIn.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

#include "android/log.h"

static const char *TAG="libs";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)

#define SPL_RATE 44100
#define AUDIO_LEN 11025
#define BUFF_LEN 220
#define SEG_COUNT 10
#define TOTAL_NOTES 32
#define PI 3.1415926535897932384626433832795
#define TWOPI 6.283185307179586476925286766559

#define NO_RESULT_YET 20
#define GOT_RESULT    30

#define BASE_FREQ 600.0
#define JUMP_FREQ 200.0

#define AMP_THRESHOLD 14000.0
#define WIDTH 0.85

double argsin[16][BUFF_LEN];				//precomputed fourier transform data
double argcos[16][BUFF_LEN];

int freq[TOTAL_NOTES*SEG_COUNT];                          //each note 10 block

short *data=NULL;                                         //sound data

struct timeval start, end;
long mtime, seconds, useconds;   

int hist=0;
int nearestF=0;
int maxF=0;

int ref_freq[32];						//data extracted from sound goes here
unsigned char c_ref_freq[16];

JNIEXPORT void JNICALL 
Java_tyfcxy_SoundAuth_AudioIn_Populate_1Table(JNIEnv * env, jclass  obj)
{
    for (int i=0; i<16; i++){
        for (int k=0; k<BUFF_LEN; k++){
            double hamming= 0.53836 - ( 0.46164 * cos( 2 * PI * k / ( BUFF_LEN ) ) ) ;
            //hamming = 1;
            
            argsin[i][k] = hamming
                           * sin(
                           ( (BASE_FREQ+((double)i)*JUMP_FREQ)*(double)BUFF_LEN/(double)SPL_RATE )
                           * TWOPI * (double)k
                           / (double)BUFF_LEN
                           );
                           
            argcos[i][k] = hamming
                           * cos(
                           ( (BASE_FREQ+((double)i)*JUMP_FREQ)*(double)BUFF_LEN/(double)SPL_RATE )
                           * TWOPI * (double)k
                           / (double)BUFF_LEN
                           );
        }
    }
}

JNIEXPORT jint JNICALL 
Java_tyfcxy_SoundAuth_AudioIn_DFT(JNIEnv * env, jclass  obj, jshortArray audiodata, jintArray result)
{
    gettimeofday(&start, NULL);
                                         
    jboolean isCopy;
    short* pAudio = (short*)env->GetShortArrayElements(audiodata, &isCopy);
    
    int max=0, min=0x7FFFFFFF;
    
    for (int i=0; i<AUDIO_LEN; i++){
        if (pAudio[i]>max) max=pAudio[i];
        if (pAudio[i]<min) min=pAudio[i];
    }
    
    int magn=max-min;
    int audioptr_base=0;
    
    bool isOdd=false;
    
    if (magn >= AMP_THRESHOLD){
       for (int i=0; i<50; i++){
           freq[hist]=-1;
           double maxfreqamp=0;
           for (int j=0; j<16; j++){
   				double sinPart=0, cosPart=0;
    			for (int k = 0; k < BUFF_LEN; k++) {
    				sinPart += (double)pAudio[k+audioptr_base] * (-1.0) * argsin[j][k];
    				cosPart += (double)pAudio[k+audioptr_base] * argcos[j][k];
    			}
    		     
    			double magn500=abs(2.0 * sqrt(sinPart*sinPart + cosPart*cosPart));
    		     
    			if (magn500>=(((double)magn)*WIDTH)){      //prominent enough
    				if (magn500>=maxfreqamp){			   //get most prominent frequency
    					maxfreqamp=magn500;
    					freq[hist]=j;
    				}
    			}
    			//freq[hist]=j;
	       }
	       audioptr_base+=BUFF_LEN;
	       if (isOdd) {audioptr_base++; isOdd=!isOdd;} else {isOdd=!isOdd;}
	       if (freq[hist]==-1) nearestF=0; else nearestF++;
	       if (nearestF>maxF) maxF=nearestF;
	       if (nearestF>=((TOTAL_NOTES*SEG_COUNT)-0)) break;
	       hist++;
           hist%=(TOTAL_NOTES*SEG_COUNT);
       }
    }else{
          nearestF=0;
          hist=0;
          maxF=0;
    }
    
    env->ReleaseShortArrayElements(audiodata, pAudio, JNI_ABORT);
    
    if (nearestF>=((TOTAL_NOTES*SEG_COUNT)-0)){   //got one
        hist++;		//rewind hist ptr
		hist%=(TOTAL_NOTES*SEG_COUNT);
		int curr_freq=freq[hist];
		int accu=0, len_freq=0, l=0;
		while (l<32)
        {
            accu=hist;
            len_freq=0;
            while ((curr_freq==freq[accu]) && (len_freq<10)){accu++;accu%=(TOTAL_NOTES*SEG_COUNT);len_freq++;}
            if (len_freq>=7){			//yes
		       ref_freq[l]=freq[hist];
               hist+=(len_freq>8)?10:(len_freq+8);
		       l++;
            }else{						//not long enough
               hist++;
            }
			hist%=(TOTAL_NOTES*SEG_COUNT);
			curr_freq=freq[hist];
		}

		for (l=0; l<16; l++){		//squeeze the results
			ref_freq[l]=(ref_freq[l*2]<<4)+ref_freq[l*2+1];
			c_ref_freq[l]=ref_freq[l];
		}
		//nearestF=0;
		//hist=0;
    }
    
    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0);
    
    int* pResult = (int*)env->GetIntArrayElements(result, NULL);
    pResult[0]=mtime;
    env->ReleaseIntArrayElements(result, pResult, 0);
    
    if (isCopy) {pResult[1]=1;} else {pResult[1]=0;}
    if (nearestF>=((TOTAL_NOTES*SEG_COUNT)-0)) {pResult[2]=1;nearestF=0;hist=0;} else pResult[2]=0;
    for (int i=0; i<16; i++){
        pResult[3+i]=c_ref_freq[i];
    }
    
    pResult[20]=nearestF;
    
    return magn;
}
