

package com.hejunsaturday.videoinpainting;

import android.util.Log;
import com.hejunsaturday.videoinpainting.onJniCall;

import java.nio.ByteBuffer;


public class VideoUntrunc {
    private static final String TAG = "VideoUntrunc";
	private onJniCall jniCall;


    static {
      //  System.loadLibrary("avuntrunc");    	
        System.loadLibrary("untrunc");
    }


    public void setJniCall(onJniCall jniCall) {
		this.jniCall = jniCall;
    }
    public VideoUntrunc() {
        ;
    }

    public native void untruncVideoFile(String okfile, String corruptfile, String outputfilename, boolean info, boolean analyze, boolean simulate,  boolean drifting, boolean skip_zeros, int mdat_strategy, int analyze_track,int mdat_begin);
	public void onNative(String str) {
		if (jniCall != null) {
			jniCall.onListenerJniCall(str);
		}
	}
}
