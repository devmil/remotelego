package de.devmil.legocontrol;

import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.SeekBar;

public class ControlActivity extends AppCompatActivity {

    private static final String EXTRA_DEVICE_ADDRESS = "DEVICE_ADDRESS";

    public static Intent getLaunchIntent(Context context, BluetoothDevice device) {
        Intent result = new Intent(context, ControlActivity.class);

        result.putExtra(EXTRA_DEVICE_ADDRESS, device.getAddress());

        return  result;
    }

    private CarHandler mCarHandler;
    private SeekBar mSeekSpeed;
    private SeekBar mSeekSteer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_control);

        mSeekSpeed = (SeekBar)findViewById(R.id.activity_control_seekspeed);
        mSeekSteer = (SeekBar)findViewById(R.id.activity_control_seeksteer);

        mSeekSpeed.setMax(200);
        mSeekSpeed.setProgress(100);
        mSeekSteer.setMax(200);
        mSeekSteer.setProgress(100);

        mSeekSpeed.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                int speed = progress - 100;
                mCarHandler.setSpeed(speed);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        mSeekSteer.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                int percent = progress - 100;
                mCarHandler.setSteering(percent);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();

        String address = getIntent().getStringExtra(EXTRA_DEVICE_ADDRESS);

        mCarHandler = new CarHandler(this, address);
        mCarHandler.connect(this);
    }

    @Override
    protected void onPause() {
        super.onPause();
        mCarHandler.disconnect();
        mCarHandler = null;
    }
}
