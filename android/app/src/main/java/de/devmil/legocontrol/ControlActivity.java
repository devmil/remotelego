package de.devmil.legocontrol;

import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import de.devmil.legologic.CarHandler;
import de.devmil.legologic.DirectionControl;

public class ControlActivity extends AppCompatActivity {

    private static final String EXTRA_DEVICE_ADDRESS = "DEVICE_ADDRESS";

    public static Intent getLaunchIntent(Context context, BluetoothDevice device) {
        Intent result = new Intent(context, ControlActivity.class);

        result.putExtra(EXTRA_DEVICE_ADDRESS, device.getAddress());

        return  result;
    }

    private CarHandler getCarHandler() {
        return ((Application)getApplication()).getCarHandler();
    }

    private DirectionControl mDirectionControl;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_control);

        mDirectionControl = (DirectionControl)findViewById(R.id.activity_control_direction);

        mDirectionControl.setDirectionChangedListener(new DirectionControl.IDirectionChangedListener() {
            @Override
            public void onDirectionChanged(int percentSpeed, int percentSteering) {
                CarHandler carHandler = getCarHandler();
                carHandler.setSpeed(percentSpeed);
                carHandler.setSteering(percentSteering);
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();

        String address = getIntent().getStringExtra(EXTRA_DEVICE_ADDRESS);
        ((Application) getApplication()).setActiveCar(address);
    }

    @Override
    protected void onPause() {
        super.onPause();
        if(!isChangingConfigurations()) {
            ((Application) getApplication()).setActiveCar(null);
        }
    }
}
