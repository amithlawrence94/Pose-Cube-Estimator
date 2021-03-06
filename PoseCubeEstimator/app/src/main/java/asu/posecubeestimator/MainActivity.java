package asu.posecubeestimator;

import android.content.Context;
import android.media.Image;
import android.os.Bundle;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.widget.Button;

import androidx.appcompat.app.AppCompatActivity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

import asu.asuforia.Asuforia;
import asu.asuforia.PoseListener;
import asu.asuforia.Camera2Setup;

public class MainActivity extends AppCompatActivity {
    // As we need to draw cube on the captured frame we will use TextureView
    TextureView textureView;
    File mReferenceImage;
    Asuforia asuforia;
    Button btn_estimation;

    // Loads the reference images on application launch (needs to be done only once!)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        textureView = (TextureView)findViewById(R.id.texureSurface);
        btn_estimation =(Button)findViewById(R.id.btn_estimation);



        // loading the reference image from assests folder to android file system.
        // This is done so that the c++ code can access the image using its absolute path

        try{
            InputStream is = getResources().openRawResource(R.raw.reference);

            File cascadeDir = getDir("ref", Context.MODE_PRIVATE);

            // Get the absolute path of the reference image in android file system
            mReferenceImage = new File(cascadeDir, "referenceImage.jpg");
            FileOutputStream os = new FileOutputStream(mReferenceImage);

            byte[] buffer = new byte[4096];
            int bytesRead;
            while ((bytesRead = is.read(buffer)) != -1) {
                os.write(buffer, 0, bytesRead);
            }

            is.close();
            os.close();

        }
        catch (Exception e)
        {
            e.printStackTrace();
        }


        final Surface[] surface = new Surface[1];

        // Create a PoseListner object which is used an interface with Asuforia library.
        // Therefore, implements callback functions i.e onPose, and textureAvailable.
        //
        // onPose -> gets rvec and tvec and calls nativeDraw method
        final PoseListener poseListener = new PoseListener() {
            @Override
            public void onPose(Image image, float rvec[], float tvec[]) {
                // Once the pose estimations is available,
                // NativeDraw is used to draw virual cube on the captured frame
                NativeDraw.drawPose(image,surface[0],rvec,tvec);
            }

            @Override
            public void textureAvailable() {
                surface[0] =  new Surface(textureView.getSurfaceTexture());
            }
        };

        // Instantiate the library module Asuforia.
        // This object is created with the path of the reference image, the surface, and the
        // object of the poseListener interface.
        asuforia = new Asuforia(poseListener,mReferenceImage.getAbsolutePath(),surface[0]);
        asuforia.startEstimation(textureView,MainActivity.this,poseListener);
//

        btn_estimation.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String text = btn_estimation.getText().toString();
                if(text.equals("Start Estimation"))
                {
                    asuforia.onForeground();
                    btn_estimation.setText("Stop Estimation");
                }
                else
                {
                    asuforia.endEstimation();
                    btn_estimation.setText("Start Estimation");
                }
            }
        });

    }

    @Override
    protected void onResume() {
        super.onResume();
        asuforia.onForeground();

    }
    @Override
    protected void onPause() {
        super.onPause();
        asuforia.onBackground();
    }


}
