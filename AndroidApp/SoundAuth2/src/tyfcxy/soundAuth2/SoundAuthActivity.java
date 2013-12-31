package tyfcxy.soundAuth2;

import android.app.Activity;
import tyfcxy.soundAuth2.R;
import tyfcxy.soundAuth2.WifiMon;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.AssetManager;
import android.view.View.OnClickListener;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.view.View.OnTouchListener;

import java.lang.reflect.Array;

import android.graphics.Paint;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import android.widget.EditText;
import android.util.Log;

import java.io.IOException;

import android.view.View;
import android.os.Bundle;
import android.view.Display;
import android.graphics.Point;
import android.graphics.Bitmap;

import java.io.FileNotFoundException;

import android.widget.SeekBar;
import android.widget.Button;
import android.graphics.Canvas;
import android.graphics.drawable.BitmapDrawable;

import java.io.InputStream;

import android.view.MotionEvent;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

public class SoundAuthActivity extends Activity implements OnClickListener, OnSeekBarChangeListener, OnTouchListener
{
    private EditText authstr;
    //private Bitmap bbitmap;
    private Canvas canvas;
    private EditText cntstr;
    protected Button generate_button;
    float histx;
    float histy;
    InputStream[][] is;
    private SeekBar mSeekBar;
    byte[] music;
    int musicLength;
    public int musickind;
    private Paint paint;
    int[] pass;
    private myView passview;

    private int q1cnt;
    private int q2cnt;
    private int q3cnt;
    private int q4cnt;
    byte[][][] rawmusic;
    int view_height;
    int view_width;
    AudioTrack AudioTrackr1=null;
    private static WifiMon wifimon;
    
    public boolean front=true;

    public SoundAuthActivity()
    {

        int[] r2 = {4 , 16};
        is = (InputStream[][]) Array.newInstance(InputStream.class, r2);

        pass = new int[32];
        musickind = 0;
        paint = new Paint();
        //bbitmap = null;
        
        int minSize =AudioTrack.getMinBufferSize( 48000, 
        		AudioFormat.CHANNEL_OUT_MONO, 
        		AudioFormat.ENCODING_PCM_16BIT );
        
        AudioTrackr1 = new AudioTrack(AudioManager.STREAM_MUSIC, 
        		48000, AudioFormat.CHANNEL_OUT_MONO, 
        		AudioFormat.ENCODING_PCM_16BIT, minSize*2, 
        		AudioTrack.MODE_STREAM);
    }

    public static byte[] SHAsum(byte[]  r0) throws java.security.NoSuchAlgorithmException
    {
        return MessageDigest.getInstance("SHA-1").digest(r0);
    }

    private void generatesound() throws java.io.IOException, java.security.NoSuchAlgorithmException
    {

        long l0, l2;
        int i1, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13;
        byte[] r1, r2;
        musicLength = 0;
        music = new byte[172800];
        l0 = (long) Integer.parseInt(authstr.getText().toString());
        i1 = Integer.parseInt(cntstr.getText().toString());
        r1 = new byte[12];
        l2 = l0;

        for (i3 = 0; l2 - 0L > 0; i3 = i11)
        {
            i11 = i3 + 1;
            r1[i3] = (byte) (int) (255L & l2);
            l2 = l2 >> 8;
        }

        for (i4 = i1 , i5 = 8; i4 > 0; i5 = i12)
        {
            i12 = i5 + 1;
            r1[i5] = (byte) (i4 & 255);
            i4 = i4 >> 8;
        }

        r2 = SoundAuthActivity.SHAsum(r1);
        
        String str="";
		for (int i=0; i<16; i++){
			str+=((r2[i]<0)?((int)r2[i]+256):(r2[i]))+", ";
		}
		Log.i("SDAuth", "Gen: "+str);

        for (i6 = 0; i6 < 16; i6++)
        {
            pass[1 + i6 * 2] = 15 & r2[i6];
            pass[i6 * 2] = (240 & r2[i6]) >> 4;
        }

        //Log.e("AudioTrack", (new StringBuilder("music length: ")).append(musicLength).toString());

        for (i7 = 0; i7 < 2400; i7++)
        {
            music[i7 + musicLength] = (byte) (byte) 0;
        }

        musicLength = 2400 + musicLength;

        for (i8 = 0; i8 < 32; i8++)
        {
            //i13 = (int) pcms[musickind][pass[i8]].length();
        	i13=4800;
            //Log.e("AudioTrack", (new StringBuilder("music length: ")).append(musicLength).toString());
            System.arraycopy(rawmusic[musickind][pass[i8]], 0, music, musicLength, 4800);
            musicLength = i13 + musicLength;
        }

        //i9 = (int) pcms[musickind][pass[31]].length() / 2;
        i9=2400;
        //Log.e("AudioTrack", (new StringBuilder("music length: ")).append(musicLength).toString());
        System.arraycopy(rawmusic[musickind][pass[31]], 0, music, musicLength, 2400);
        musicLength = i9 + musicLength;
        //Log.e("AudioTrack", (new StringBuilder("music length: ")).append(musicLength).toString());

        for (i10 = 0; i10 < 4000; i10++)
        {
            music[i10 + musicLength] = (byte) (byte) 0;
        }

        musicLength = 4000 + musicLength;
        //Log.e("AudioTrack", (new StringBuilder("music length: ")).append(musicLength).toString());
    }

    public void onClick(View  r1)
    {

        String r3;
        if (r1 == generate_button)
        {
            try
            {
                this.generatesound();
            }
            catch (IOException $r14)
            {
                $r14.printStackTrace();
            }
            catch (NoSuchAlgorithmException $r15)
            {
                $r15.printStackTrace();
            }

            this.play();
            r3 = (new StringBuilder(String.valueOf(new String("")))).append(1 + Integer.parseInt(cntstr.getText().toString())).toString();
            cntstr.setText(r3);
            
            SharedPreferences localStorage = getSharedPreferences("SoundAuth", MODE_PRIVATE);
            int cnt = Integer.parseInt(cntstr.getText().toString());
            Editor editor = localStorage.edit();
            
            editor.putInt("cnt", cnt);
            
            editor.commit();
        }
    }

    public void onCreate(Bundle  r1)
    {
    	
    	super.onCreate(r1);

        int i0, i1, i4;
        Display r3;
        Point r4;
        //Bitmap r5;
        
        this.setContentView(R.layout.main);
        int[] r2 = {4 , 16 , 4800};
        rawmusic = (byte[][][]) Array.newInstance(Byte.TYPE, r2);

        for (i0 = 0; i0 < 16; i0++)
        {
            try
            {
            	AssetManager mgr = getAssets();
            	is[0][i0] = mgr.open(new String("pcms/")+(i0 + 5)+new String(".raw"));
            	is[1][i0] = mgr.open(new String("set1/")+(i0 + 5)+new String(".raw"));
            	is[2][i0] = mgr.open(new String("set2/")+(i0 + 5)+new String(".raw"));
            	is[3][i0] = mgr.open(new String("set3/")+(i0 + 5)+new String(".raw"));
            }
            catch (FileNotFoundException $r125)
            {
                $r125.printStackTrace();
            } catch (IOException e) {
				e.printStackTrace();
			} catch (Throwable e){
				e.printStackTrace();
			}
            
            //Log.e("SoundAuth", (new StringBuilder("reading file: ")).append(Environment.getExternalStorageDirectory().getAbsolutePath()).append("/pcms/").append(i0 + 5).append(".raw").toString());
        }

        i1 = 0;

        while (i1 < 16)
        {
            i4 = 0;

            try
            {
            	
              for (int i=0; i<4; i++){
            	is[i4][i1].read(rawmusic[i4][i1], 0, is[i4][i1].available());
                i4++;
                //Log.i("SoundAuth", "Finished processing: "+i4+", "+i1+".");
        	  }
              
            }
            catch (IOException localIOException)
            {
                localIOException.printStackTrace();
            }
            catch (Throwable e){
            	e.printStackTrace();
            }

            i1++;
        }

        paint.setAntiAlias(true);
        generate_button = (Button) this.findViewById(R.id.generate);
        mSeekBar = (SeekBar) this.findViewById(R.id.seek);
        mSeekBar.setOnSeekBarChangeListener(this);
        generate_button.setOnClickListener(this);
        authstr = (EditText) this.findViewById(R.id.authnum);
        cntstr = (EditText) this.findViewById(R.id.cntnum);
        
        passview = (myView) this.findViewById(R.id.view1);
        passview.setOnTouchListener(this);
        r3 = this.getWindowManager().getDefaultDisplay();
        r4 = new Point();
        r3.getSize(r4);
        //i2 = r4.x;
        //i3 = -380 + r4.y;
        view_width = 480;
        view_height = 474;
        //i2 = 480;
        //i3 = 474;
        
        //r5 = Bitmap.createBitmap(i2, i3, Bitmap.Config.ARGB_8888);
        //bbitmap = Bitmap.createBitmap(i2, i3, Bitmap.Config.ARGB_8888);
        canvas = new Canvas();
        //canvas.setBitmap(bbitmap);
        
        //paint.setColor(-16777216);
        //canvas.drawPaint(paint);
        
        //paint.setColor(-16711936);
        
        //grid.draw(canvas);
        //canvas.setBitmap(r5);
        
        /*paint.setColor(-16777216);
        canvas.drawPaint(paint);
        paint.setColor(-16711936);*/
        
        BitmapDrawable smallbg = (BitmapDrawable) getResources().getDrawable(R.drawable.smallbg);
        Bitmap smallbg_bmp = smallbg.getBitmap().copy(Bitmap.Config.ARGB_8888, true);
        canvas.setBitmap(smallbg_bmp);
        
        //canvas.drawLine(0.0F, (float) (view_height / 2), (float) view_width, (float) (view_height / 2), paint);
        //canvas.drawLine((float) (view_width / 2), 0.0F, (float) (view_width / 2), (float) view_height, paint);
        passview.tbitmap = smallbg_bmp;
        passview.invalidate();
    }

    public void onProgressChanged(SeekBar  r1, int  i0, boolean  z0)
    {
        musickind = i0 / 26;
    }

    public void onStartTrackingTouch(SeekBar  r1)
    {

    }

    public void onStopTrackingTouch(SeekBar  r1)
    {

    }

    public boolean onTouch(View  r1, MotionEvent  r2)
    {

        boolean z0;
        float f0, f1;
        int i36;
        z0 = true;

        if (r1 != passview)
        {
            z0 = false;
        }
        else
        {
            if (r2.getActionMasked() != 2)
            {
                i36 = 1;

                if (r2.getActionMasked() != i36)
                {
                    if (r2.getActionMasked() == 0)
                    {
                        histx = r2.getX();
                        histy = r2.getY();
                        q1cnt = 0;
                        q2cnt = 0;
                        q3cnt = 0;
                        q4cnt = 0;
                    }
                }
                else
                {
                    /*paint.setColor(-16777216);
                    canvas.drawPaint(paint);*/
                	
                	/*BitmapDrawable smallbg = (BitmapDrawable) getResources().getDrawable(R.drawable.smallbg);
                    Bitmap smallbg_bmp = smallbg.getBitmap();
                    canvas.setBitmap(smallbg_bmp);*/
                	
                	BitmapDrawable smallbg = (BitmapDrawable) getResources().getDrawable(R.drawable.smallbg);
                	canvas.drawBitmap(smallbg.getBitmap(), 0, 0, paint);
                    //Bitmap smallbg_bmp = smallbg.getBitmap().copy(Bitmap.Config.ARGB_8888, true);
                    //canvas.setBitmap(smallbg_bmp);
                    //passview.tbitmap=smallbg_bmp;
                	
                    passview.invalidate();
                    Log.i("SoundAuth", (new StringBuilder("Stats:")).append(q1cnt).append(" ").append(q2cnt).append(" ").append(q3cnt).append(" ").append(q4cnt).toString());

                    if ((double) ((float) q1cnt / (float) q4cnt) - 0.75 > 0 &&  ((double) ((float) q1cnt / (float) q4cnt) - 1.25 < 0 && ((float) q1cnt + (float) q4cnt) / ((float) q2cnt + (float) q3cnt) - 5.0F > 0) )
                    {
                        this.onClick(generate_button);
                    }
                }
            }
            else
            {
                f0 = r2.getX();
                f1 = r2.getY();
                
                paint.setColor(-65536);
                paint.setStrokeWidth(3.0F);
                
                float[] pts = {histx, histy, f0, f1};
                
                canvas.drawLines(pts, paint);
                histx = f0;
                histy = f1;

                if (f0 - (float) (view_width / 2) >= 0)
                {
                    if (f1 - (float) (view_height / 2) >= 0)
                    {
                        q4cnt = 1 + q4cnt;
                    }
                    else
                    {
                        q3cnt = 1 + q3cnt;
                    }
                }
                else
                {
                    if (f1 - (float) (view_height / 2) >= 0)
                    {
                        q2cnt = 1 + q2cnt;
                    }
                    else
                    {
                        q1cnt = 1 + q1cnt;
                    }
                }

                passview.invalidate();
            }
        }

        return z0;
    }
    
    public void onPause(){
    	super.onPause();
    	
    	//save auth & cnt
    	SharedPreferences localStorage = getSharedPreferences("SoundAuth", MODE_PRIVATE);
    	
    	long auth = (long) Integer.parseInt(authstr.getText().toString());
        int cnt = Integer.parseInt(cntstr.getText().toString());
        
        Editor editor = localStorage.edit();
        
        editor.putLong("auth", auth);
        editor.putInt("cnt", cnt);
        
        editor.commit();
        
        wifimon = new WifiMon(this);
        new Thread(wifimon).start();
        
        front = false;
    }

    public void onResume(){
    	super.onResume();
    	
    	SharedPreferences localStorage = getSharedPreferences("SoundAuth", MODE_PRIVATE);
    	authstr.setText(String.valueOf(localStorage.getLong("auth", 0)));
    	cntstr.setText(String.valueOf(localStorage.getInt("cnt", 0)));
    	
    	front = true;
    	
    	if (wifimon!=null) wifimon.close();
    	wifimon=null;
    }
    
    public void play()
    {

        try
        {
            Log.i("AudioTrack", "Reached");
            
            AudioTrackr1.play();
            AudioTrackr1.write(music, 0, musicLength);
            Log.i("AudioTrack", "Reached: "+musicLength);
        }
        catch (Throwable $r5)
        {
            Log.e("AudioTrack", "Playback Failed");
        }
        try {
        	synchronized(this){
        		wait(50L);
        	}
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		AudioTrackr1.stop();
		AudioTrackr1.pause();
    }
}
