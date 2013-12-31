package tyfcxy.soundAuth2;

import java.util.List;
import java.util.concurrent.Semaphore;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiManager.WifiLock;
import android.util.Log;

public class WifiMon implements Runnable{

	WifiManager wifiman=null;
	public Activity parent;
	boolean stopped = false;
	public static final Semaphore sem = new Semaphore(1, true);
	private WifiLock wilock;
	
	WifiMon(Activity act){
		parent=act;
	}
	
	public void run() {
		try {
			sem.acquire();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		
		wifiman = (WifiManager) parent.getSystemService(Context.WIFI_SERVICE);
		wifiman.setWifiEnabled(true);
		wilock = wifiman.createWifiLock(WifiManager.WIFI_MODE_SCAN_ONLY, "SoundAuth");
		wilock.acquire();
		
		int left=0;
		boolean started=false;
		
		while (!stopped){
			wifiman.startScan();
			try {
				synchronized(this){
					wait(900);
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			List<ScanResult> results = wifiman.getScanResults();
			boolean gotSig=false;
			for (ScanResult result:results){
				int siglevel = WifiManager.calculateSignalLevel(result.level, 200000000);
				//Log.i("SoundAuth", "DB: "+siglevel);
				if (result.SSID.equals(new String("3FFD26ACE268"))){
					gotSig = true;
					if (/*siglevel>=199999999*/ result.level>-27 ){
						Log.i("SoundAuth", "DB: "+siglevel+", sig: "+result.level);
						if (left<5) continue;
						if (((SoundAuthActivity)parent).front) break;
						Log.i("SoundAuth", "Entered range");
						Intent i = new Intent();
						i.setAction(Intent.ACTION_MAIN);
						i.addCategory(Intent.CATEGORY_LAUNCHER);
						i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
						ComponentName cn = new ComponentName(parent, SoundAuthActivity.class);
						i.setComponent(cn);
						parent.startActivity(i);
						left=0;
						started=true;
						break;
					}else{
						if (result.level<-65){
							left++;
						}
						Log.i("SoundAuth", "DB: "+result.level);
					}
				}
				if (started) break;
				if (((SoundAuthActivity)parent).front) break;
			}
			if (!gotSig) left++;
			if (((SoundAuthActivity)parent).front) break;
			if (started) break;
		}
		
		sem.release();
		wilock.release();
	}
	
	public void close(){
		stopped=true;
	}
	
}