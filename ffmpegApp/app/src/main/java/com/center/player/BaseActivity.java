package com.center.player;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.Rationale;
import com.yanzhenjie.permission.RationaleListener;

/**
 * @author DELL
 */
public class BaseActivity extends Activity {
    private RationaleListener mRationaleListener = new RationaleListener() {
        @Override
        public void showRequestPermissionRationale(int requestCode, final Rationale rationale) {
            new AlertDialog.Builder(BaseActivity.this)
                    .setTitle("友好提醒")
                    .setMessage("没有定位权限将不能为您推荐附近妹子，请把定位权限赐给我吧！")
                    .setPositiveButton("好，给你", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.cancel();
                            rationale.resume();// 用户同意继续申请。
                        }
                    })
                    .setNegativeButton("我拒绝", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.cancel();
                            rationale.cancel(); // 用户拒绝申请。
                        }
                    }).show();
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        super.onCreate(savedInstanceState);
        AndPermission.with(this)
                .requestCode(101)
                .permission(Manifest.permission.WRITE_EXTERNAL_STORAGE,
                        Manifest.permission.READ_EXTERNAL_STORAGE,
                        Manifest.permission.INTERNET)
                .rationale(mRationaleListener)
                .start();
    }

    @Override
    public void setContentView(int layoutResID) {
        super.setContentView(layoutResID);
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    public void onCallPermission(String permissionStr) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {//判断当前系统的SDK版本是否大于23
            //如果当前申请的权限没有授权
            if (!(checkSelfPermission(permissionStr) == PackageManager.PERMISSION_GRANTED)) {
                //第一次请求权限的时候返回false,第二次shouldShowRequestPermissionRationale返回true
                //如果用户选择了“不再提醒”永远返回false。
                if (shouldShowRequestPermissionRationale(Manifest.permission.RECORD_AUDIO)) {
                    Toast.makeText(this, "Please grant the permission this time", Toast.LENGTH_LONG).show();
                }
                //请求权限
                requestPermissions(new String[]{permissionStr}, 1);
            } else {//已经授权了就走这条分支
                //Log.i("wei", "onClick granted");
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if (requestCode == 1) {
            if (permissions[0].equals(Manifest.permission.RECORD_AUDIO) && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                //listernUi();//得到权限之后去做的业务
            } else {//没有获得到权限
                Toast.makeText(this, "你不给权限我就不好干事了啦", Toast.LENGTH_SHORT).show();
            }
        }
    }
}
