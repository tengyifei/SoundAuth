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

#define TABLE_RESOLUTION 5000
#define PI 3.1415926535897932384626433832795f
#define P2I 6.283185307179586476925286766559f
#define Halfpi 1.5707963267948966192313216916398f
#define table_over_pi 1591.5494309189533576888376337251f

//float *sine_table=NULL;//, **sine_table_diff=NULL;
//float sine_table[TABLE_RESOLUTION+1][2];
//bool populated=false;

JNIEXPORT void JNICALL 
Java_tyfcxy_SoundAuth_AudioIn_Populate_1Table(JNIEnv * env, jclass  obj)
{
    struct timeval thetime;
    gettimeofday(&thetime, NULL);
    srand ( thetime.tv_sec*1000+thetime.tv_usec );
    
    /*if (populated) return;
	//sine_table=new float[TABLE_RESOLUTION+1][2];//(float*)malloc((TABLE_RESOLUTION+1)*sizeof(float*));
	//sine_table_diff=//(float*)malloc((TABLE_RESOLUTION+1)*sizeof(float*));
	for (int i=0; i<TABLE_RESOLUTION+1; i++){
        sine_table[i][0]=sin(((float)i)*((float)(PI))/((float)(TABLE_RESOLUTION)));
    }
    for (int i=0; i<TABLE_RESOLUTION; i++){
        //sine_table_diff[i]=sine_table[i+1]-sine_table[i];
        sine_table[i][1]=sine_table[i+1][0]-sine_table[i][0];
    }
    populated=true;*/
}

/*static inline float xsin (float x) {
  x=x/Halfpi;
  float x2 = x * x;
  return ((((.00015148419f * x2
             - .00467376557f) * x2
            + .07968967928f) * x2
           - .64596371106f) * x2
          + 1.57079631847f) * x;
}*/

/*float sin_acc2(float num){
       num=fmod(num, P2I);
       if ((-Halfpi<num)&&(num<Halfpi)) return xsin(num);
       return 0;
       /*if (num>0){
       }else{
       }   */       
}*/

/*float sin_acc(float num) {
       float sign, diff, rawtarget, remainder;
       int target;
       
       if (num<0) {num=-num; sign=-1;}else{sign=1;}
       remainder=fmod(num, P2I);
       
       rawtarget=remainder*table_over_pi;
       
       if (remainder>PI) {
           rawtarget-=TABLE_RESOLUTION;
           sign=-sign;
       }
       
       target=(int)(rawtarget);     				                    //position of array index
       //diff=sine_table_diff[target]*(rawtarget-(float)target);       //linear interpolation
       
       return sign*( sine_table[target][0]+(sine_table[target][1]*(rawtarget-(float)target)) );
}

JNIEXPORT double JNICALL 
Java_tyfcxy_SoundAuth_AudioIn_DFT1(JNIEnv * env, jclass  obj)
{
    float temp;
    
    struct timeval start, end;

    long mtime, seconds, useconds;    
    gettimeofday(&start, NULL);
    for (int i=0; i<10000000; i++){
        temp+=sin((((float)((rand()%32767)*32767+(rand()%32767)))/((float)500000)));
    }
    gettimeofday(&end, NULL);
    
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    
    return ((((int)temp)%5)/10000)+mtime;
}

JNIEXPORT double JNICALL 
Java_tyfcxy_SoundAuth_AudioIn_DFT2(JNIEnv * env, jclass  obj)
{
    float temp;
    
    struct timeval start, end;

    long mtime, seconds, useconds;    
    gettimeofday(&start, NULL);
    for (int i=0; i<10000000; i++){
        temp+=sin_acc( (((float)((rand()%32767)*32767+(rand()%32767)))/((float)500000)) );
    }
    gettimeofday(&end, NULL);
    
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    
    return ((((int)temp)%5)/10000)+mtime;
}*/
