package tyfcxy.SoundAuth;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder.AudioSource;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public final class AudioIn implements Runnable { 
	
	public static native void Populate_Table();
	public static native int DFT(short[] buffer, int[] result);
	
	static {
	    System.loadLibrary("nativedft");
	}
	
	AudioRecord recorder = null;
	int[] result=new int[64];
	int[] gotdata=new int[16];
	
     private boolean stopped = false;
     private Handler threadHandler;

     AudioIn(Handler _threadHandler) { 
    	     threadHandler=_threadHandler;
    	     Populate_Table();
             android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_AUDIO);
          }

     public final void run() { 
            int ix = 0;
            try { // ... initialize

                  Log.i("SDAuth", "Allocating buffer size: "+(2*11025*4));
                  short[][] buffers  = new short[2][11025];
                  
                   recorder = new AudioRecord(AudioSource.MIC,
                		   					  44100,
                                              AudioFormat.CHANNEL_IN_MONO,
                                              AudioFormat.ENCODING_PCM_16BIT,
                                              22*44100);

                   Log.i("SDAuth", "AudioRecord setup");
                   
                   recorder.startRecording();
                   
                   Log.i("SDAuth", "Recording started");

                   // ... loop

                   while(!stopped) { 
                      short[] buffer = buffers[ix++ % buffers.length];

                      recorder.read(buffer,0,buffer.length);

                      process(buffer);
                  }
             } catch(Throwable x) { 
               Log.w("SDAuth","Error reading voice audio",x);
             } finally { 
               recorder.stop();
             }
             
             Log.i("SDAuth", "Recording stopped");
         }

      private void process(short[] buffer) {
    	int amplitude=DFT(buffer, result);
    	if (result[1]==1) Log.i("SDAuth", "Copied");
    	Message msg;
    	//Log.i("SDAuth", "nearestF: "+result[20]);
    	if (result[2]==1) {
    		String str="";
    		for (int i=0; i<16; i++){
    			gotdata[i]=result[3+i];
    			str+=gotdata[i]+", ";
    		}
    		Log.i("SDAuth", "Got one: "+str);
    		msg=Message.obtain(threadHandler, 1, result[0], amplitude, gotdata);
    	}else{
    		msg=Message.obtain(threadHandler, 0, result[0], amplitude);
    	}
    	threadHandler.sendMessage(msg);
	}

	public void close() {
          stopped = true;
        }
    }