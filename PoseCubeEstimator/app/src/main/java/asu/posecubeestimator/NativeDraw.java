package asu.posecubeestimator;

import android.media.Image;
import android.view.Surface;

import java.nio.ByteBuffer;

public class NativeDraw {
    static{
        System.loadLibrary("cubedraw");
    }

    public static void drawPose(Image img, Surface surface, float[] rvec, float[] tvec) {
        drawNativePose(img.getWidth(), img.getHeight(), img.getPlanes()[0].getBuffer(),surface,rvec,tvec);
    }

    public static native void drawNativePose(int width, int height, ByteBuffer buffer, Surface surface,float[] rvec, float[] tvec);

}
