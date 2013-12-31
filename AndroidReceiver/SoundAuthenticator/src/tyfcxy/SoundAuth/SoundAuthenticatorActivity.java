package tyfcxy.SoundAuth;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.Serializable;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Hashtable;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.AssetManager;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.view.animation.AccelerateInterpolator;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ViewFlipper;
import tyfcxy.SoundAuth.AudioIn;

public class SoundAuthenticatorActivity extends Activity{
	
	static AudioIn audiorec = null;
	static TextView xnaught;
	static TextView xtime;
	static private Handler threadHandler=null;
	static public Activity ourAct = null;
	static public Hashtable<String, UserData> user_table = null;
	
	AudioTrack AudioTrackr1=null;
	byte [][] rawmusic = new byte[2][80000];
	
	static public long authnum=0;
	static public int cntnum=0;
	
	ViewFlipper flipper;
	ImageView imgres;
	
	private class HideImageTask extends TimerTask{
		@Override
        public void run() {
    		runOnUiThread(new Runnable() {
    		    public void run() {
    		    	imgres.setVisibility(View.INVISIBLE);
    		    }
    		});
    	}
	}
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	ourAct = this;
    	
        super.onCreate(savedInstanceState);
        
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        
        setContentView(R.layout.main);
        
        imgres = (ImageView) findViewById(R.id.imageViewRes);
        imgres.setVisibility(View.INVISIBLE);
        
        xnaught = (TextView) findViewById(R.id.textViewAmplitude);
        xtime = (TextView) findViewById(R.id.textViewTime);
        
        int minSize =AudioTrack.getMinBufferSize( 22050, 
        		AudioFormat.CHANNEL_OUT_MONO, 
        		AudioFormat.ENCODING_PCM_16BIT );
        
        AudioTrackr1 = new AudioTrack(AudioManager.STREAM_MUSIC, 
        		22050, AudioFormat.CHANNEL_OUT_MONO, 
        		AudioFormat.ENCODING_PCM_16BIT, minSize*2, 
        		AudioTrack.MODE_STREAM);
        
        AssetManager mgr = getAssets();
        InputStream is1=null, is2=null;
        try {
			is1 = mgr.open("suc.raw");
			is2 = mgr.open("fai.raw");
			is1.read(rawmusic[0], 0, is1.available());
			is2.read(rawmusic[1], 0, is2.available());
		} catch (IOException e) {
			e.printStackTrace();
		}
        
        threadHandler = new Handler() {
            public void handleMessage(android.os.Message msg) {
            	if (msg.what==0){
	                xnaught.setText(Integer.toString(msg.arg2));
	                xtime.setText(Integer.toString(msg.arg1)+" ms");
            	}else{
            		if (msg.what==1){
            			if (compareResult((int[])msg.obj)){
            				Toast toast = Toast.makeText(getApplicationContext(), "Success!", Toast.LENGTH_SHORT);
            				toast.show();
            				cntnum++;
            				SharedPreferences localStorage = getSharedPreferences("SDAuth", MODE_PRIVATE);
            				Editor editor = localStorage.edit();
                            
                            editor.putInt("cnt", cntnum);
                            
                            editor.commit();
                            
                            imgres.setImageResource(R.drawable.tick);
                            imgres.setVisibility(View.VISIBLE);
                            Timer t = new Timer();
                            t.schedule(new HideImageTask(), 2000);
                            
                            Integer[] succ={0};
                            
                            audiorec.close();
                        	audiorec = null;
                        	System.gc();
                        	
                        	int minSize =AudioTrack.getMinBufferSize( 22050, 
                            		AudioFormat.CHANNEL_OUT_MONO, 
                            		AudioFormat.ENCODING_PCM_16BIT );
                            
                            AudioTrackr1 = new AudioTrack(AudioManager.STREAM_MUSIC, 
                            		22050, AudioFormat.CHANNEL_OUT_MONO, 
                            		AudioFormat.ENCODING_PCM_16BIT, minSize*2, 
                            		AudioTrack.MODE_STREAM);
                        	
                            (new ExecuteLater()).execute(succ);
                            
            			}else{
            				Toast toast = Toast.makeText(getApplicationContext(), "Fail!", Toast.LENGTH_SHORT);
            				toast.show();
            				Integer[] fail={1};
            				
            				imgres.setImageResource(R.drawable.cross);
                            imgres.setVisibility(View.VISIBLE);
            				
                            Timer t = new Timer();
                            t.schedule(new HideImageTask(), 2000);
                            
            				audiorec.close();
                        	audiorec = null;
                        	System.gc();
                        	
                        	int minSize =AudioTrack.getMinBufferSize( 22050, 
                            		AudioFormat.CHANNEL_OUT_MONO, 
                            		AudioFormat.ENCODING_PCM_16BIT );
                            
                            AudioTrackr1 = new AudioTrack(AudioManager.STREAM_MUSIC, 
                            		22050, AudioFormat.CHANNEL_OUT_MONO, 
                            		AudioFormat.ENCODING_PCM_16BIT, minSize*2, 
                            		AudioTrack.MODE_STREAM);
                        	
            				(new ExecuteLater()).execute(fail);
            			}
            		}
            	}
            }
        };
        
        SharedPreferences localStorage = getSharedPreferences("SDAuth", MODE_PRIVATE);
    	authnum = localStorage.getLong("auth", 0);
    	cntnum = localStorage.getInt("cnt", 0);
        
        user_table = new Hashtable<String, UserData>();
        
        flipper = (ViewFlipper) findViewById(R.id.flipper);
        Button button1 = (Button) findViewById(R.id.EditBtn);
        Button button2 = (Button) findViewById(R.id.ConfirmBtn);
        
        button1.setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
            	
            	EditText authstr = (EditText) findViewById(R.id.authText);
                EditText cntstr = (EditText) findViewById(R.id.CntText);
            	
            	SharedPreferences localStorage = getSharedPreferences("SDAuth", MODE_PRIVATE);
            	authstr.setText(String.valueOf(localStorage.getLong("auth", 0)));
            	cntstr.setText(String.valueOf(localStorage.getInt("cnt", 0)));
            	
                flipper.setInAnimation(inFromRightAnimation());
                flipper.setOutAnimation(outToLeftAnimation());
                flipper.showNext();
            }
        });

        button2.setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
            	
            	EditText authstr = (EditText) findViewById(R.id.authText);
                EditText cntstr = (EditText) findViewById(R.id.CntText);
            	
            	SharedPreferences localStorage = getSharedPreferences("SDAuth", MODE_PRIVATE);
            	
            	long auth = (long) Integer.parseInt(authstr.getText().toString());
                int cnt = Integer.parseInt(cntstr.getText().toString());
                
                authnum = auth;
                cntnum = cnt;
                
                Editor editor = localStorage.edit();
                
                editor.putLong("auth", auth);
                editor.putInt("cnt", cnt);
                
                editor.commit();
            	
                flipper.setInAnimation(inFromLeftAnimation());
                flipper.setOutAnimation(outToRightAnimation());
                flipper.showPrevious();
            }
        });
		
		Log.i("SDAuth", "Main app initiated");
    }
    
    void execNative(String command, String action){
    	new NativeExec().execute(command, action);
    }
    
    boolean compareResult(int[] result){
    	long l0 = (long) authnum;
        int i1 = cntnum;
        byte[] r1 = new byte[12];
        long l2 = l0;
        int i11, i12;

        for (int i3 = 0; l2 - 0L > 0; i3 = i11)
        {
            i11 = i3 + 1;
            r1[i3] = (byte) (int) (255L & l2);
            l2 = l2 >> 8;
        }

        for (int i4 = i1 , i5 = 8; i4 > 0; i5 = i12)
        {
            i12 = i5 + 1;
            r1[i5] = (byte) (i4 & 255);
            i4 = i4 >> 8;
        }

        byte[] r2;
		try {
			r2 = SHAsum(r1);
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
			return false;
		}
		
		String str="";
		for (int i=0; i<16; i++){
			int curr=((r2[i]<0)?((int)r2[i]+256):(r2[i]));
			str+=curr+", ";
		}
		Log.i("SDAuth", "Supposed: "+str);
        
        for (int i=0; i<12; i++){
        	int curr=((r2[i]<0)?((int)r2[i]+256):(r2[i]));
        	if (result[i]!=curr) return false;
        }
        return true;
    }
    
    public static byte[] SHAsum(byte[]  r0) throws java.security.NoSuchAlgorithmException
    {
        return MessageDigest.getInstance("SHA-1").digest(r0);
    }
    
    private Animation inFromRightAnimation() {

    	Animation inFromRight = new TranslateAnimation(
    	Animation.RELATIVE_TO_PARENT,  +1.0f, Animation.RELATIVE_TO_PARENT,  0.0f,
    	Animation.RELATIVE_TO_PARENT,  0.0f, Animation.RELATIVE_TO_PARENT,   0.0f
    	);
    	inFromRight.setDuration(500);
    	inFromRight.setInterpolator(new AccelerateInterpolator());
    	return inFromRight;
    	}
    	private Animation outToLeftAnimation() {
    	Animation outtoLeft = new TranslateAnimation(
    	  Animation.RELATIVE_TO_PARENT,  0.0f, Animation.RELATIVE_TO_PARENT,  -1.0f,
    	  Animation.RELATIVE_TO_PARENT,  0.0f, Animation.RELATIVE_TO_PARENT,   0.0f
    	);
    	outtoLeft.setDuration(500);
    	outtoLeft.setInterpolator(new AccelerateInterpolator());
    	return outtoLeft;
    	}

    	private Animation inFromLeftAnimation() {
    	Animation inFromLeft = new TranslateAnimation(
    	Animation.RELATIVE_TO_PARENT,  -1.0f, Animation.RELATIVE_TO_PARENT,  0.0f,
    	Animation.RELATIVE_TO_PARENT,  0.0f, Animation.RELATIVE_TO_PARENT,   0.0f
    	);
    	inFromLeft.setDuration(500);
    	inFromLeft.setInterpolator(new AccelerateInterpolator());
    	return inFromLeft;
    	}
    	private Animation outToRightAnimation() {
    	Animation outtoRight = new TranslateAnimation(
    	  Animation.RELATIVE_TO_PARENT,  0.0f, Animation.RELATIVE_TO_PARENT,  +1.0f,
    	  Animation.RELATIVE_TO_PARENT,  0.0f, Animation.RELATIVE_TO_PARENT,   0.0f
    	);
    	outtoRight.setDuration(500);
    	outtoRight.setInterpolator(new AccelerateInterpolator());
    	return outtoRight;
    	}
    
    public void onPause(){
    	super.onPause();
    	audiorec.close();
    	audiorec = null;
    	
    	execNative("/system/bin/tether stop 1", "Disable broadcast");
    }
    
    public void onResume(){
    	super.onResume();
    	audiorec = new AudioIn(threadHandler);
        new Thread(audiorec).start();
        
        execNative("/system/bin/tether start 1", "Init broadcast");
    }
    
    public class UserData implements Serializable{

		private static final long serialVersionUID = 1L;
    	
		private String username="";
		private int count=0;
		private int authnum=0;

		public void setUsername(String username) {
			this.username = username;
		}

		public String getUsername() {
			return username;
		}

		public void setCount(int count) {
			this.count = count;
		}

		public int getCount() {
			return count;
		}

		public void setAuthnum(int authnum) {
			this.authnum = authnum;
		}

		public int getAuthnum() {
			return authnum;
		}
    }
    
    private class ExecuteLater extends AsyncTask<Integer, Void, Void>{

		@Override
		protected Void doInBackground(Integer... arg0) {
			
			AudioTrackr1.play();
            AudioTrackr1.write(rawmusic[arg0[0]], 0, 80000);
			
			synchronized(this){
				try {
					wait(100);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			
			AudioTrackr1.flush();
			AudioTrackr1.stop();
			AudioTrackr1.pause();
			AudioTrackr1.release();
			AudioTrackr1=null;
			System.gc();
			
			synchronized(this){
				try {
					wait(200);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			
			audiorec = new AudioIn(threadHandler);
            new Thread(audiorec).start();
			
			return null;
		}
    	
    }
    
    private class NativeExec extends AsyncTask<String, String, Void> {
    	@Override
    	protected void onProgressUpdate(String... action){
    		Toast toast = Toast.makeText(getApplicationContext(), action[0], Toast.LENGTH_SHORT);
			toast.show();
    	}
    	
		@Override
		protected Void doInBackground(String... arg0) {
			publishProgress(arg0[1]+"...");
			
			Process p;
			try {
				p = Runtime.getRuntime().exec("su");   
				DataOutputStream os = new DataOutputStream(p.getOutputStream());  
				os.writeBytes(arg0[0]+"\n");  
				os.writeBytes("exit\n");
				os.flush();
				p.waitFor();
			} catch (IOException e1) {
				e1.printStackTrace();
			} catch (InterruptedException e) {
				e.printStackTrace();
			} catch (Throwable e){
				e.printStackTrace();
			}
			
			publishProgress(arg0[1]+" finished.");
			
			return null;
		}
    }
    
    
}