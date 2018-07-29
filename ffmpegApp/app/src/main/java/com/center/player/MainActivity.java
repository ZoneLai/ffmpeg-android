package com.center.player;

import android.os.Bundle;
import android.os.Environment;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

/**
 * @author DELL
 */
public class MainActivity extends BaseActivity {

    static {
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
        System.loadLibrary("player");
    }

    private SurfaceView mSurfaceView;
    private EditText mEditText;
    private SurfaceHolder mSurfaceHolder;
    private Button mButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();
        mButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View arg0) {
                String folderurl = Environment.getExternalStorageDirectory().getPath();
                String urltext_input = mEditText.getText().toString();
                final String filePath = folderurl + "/com.uni.pano/" + urltext_input;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        nativePlay(filePath, mSurfaceHolder.getSurface());
                    }
                }).start();
            }
        });
    }

    private void initView() {
        mButton = findViewById(R.id.bt_play);
        mEditText = findViewById(R.id.et_url);
        mSurfaceView = findViewById(R.id.sv_display);
        mSurfaceHolder = mSurfaceView.getHolder();
    }

    private native static int nativePlay(final String filePath, Surface surface);
}
