package de.devmil.legowear;

import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class ControlActivity extends Activity {

    private static final String EXTRA_DEVICE_ADDRESS = "DEVICE_ADDRESS";

    public static Intent getLaunchIntent(Context context, BluetoothDevice device) {
        Intent result = new Intent(context, ControlActivity.class);

        result.putExtra(EXTRA_DEVICE_ADDRESS, device.getAddress());

        return  result;
    }

    private CarHandler mCarHandler;
    private DirectionControl mDirectionControl;
    private Button mButtonExit;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_control);

        mDirectionControl = (DirectionControl)findViewById(R.id.activity_control_direction);
        mButtonExit = (Button)findViewById(R.id.activity_control_btnExit);

        mDirectionControl.setDirectionChangedListener(new DirectionControl.IDirectionChangedListener() {
            @Override
            public void onDirectionChanged(int percentSpeed, int percentSteering) {
                mCarHandler.setSpeed(percentSpeed);
                mCarHandler.setSteering(percentSteering);
            }
        });

        mButtonExit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
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
