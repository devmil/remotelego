package de.devmil.legocontrol;

import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.LinearLayout;
import android.widget.Switch;
import android.widget.TextView;

import org.w3c.dom.Text;

import de.devmil.legologic.CarHandler;
import de.devmil.legologic.DirectionControl;
import de.devmil.legologic.ICarHandlerStateListener;

public class ControlActivity extends AppCompatActivity implements ICarHandlerStateListener {

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
    private LinearLayout mLinearLayoutSwitches;
    private Switch mSwitchTrunk;
    private TextView mTextTrunk;
    private Switch mSwitchMFL;
    private TextView mTextMFL;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_control);

        mDirectionControl = (DirectionControl)findViewById(R.id.activity_control_direction);
        mLinearLayoutSwitches = (LinearLayout)findViewById(R.id.activity_control_ll_switches);
        mSwitchTrunk = (Switch)findViewById(R.id.activity_control_sw_trunk);
        mTextTrunk = (TextView)findViewById(R.id.activity_control_tv_trunk);
        mSwitchMFL = (Switch)findViewById(R.id.activity_control_sw_mfl);
        mTextMFL = (TextView)findViewById(R.id.activity_control_tv_mfl);

        mDirectionControl.setDirectionChangedListener(new DirectionControl.IDirectionChangedListener() {
            @Override
            public void onDirectionChanged(int percentSpeed, int percentSteering) {
                CarHandler carHandler = getCarHandler();
                carHandler.setSpeed(percentSpeed);
                carHandler.setSteering(percentSteering);
            }
        });

        mSwitchTrunk.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                CarHandler ch = getCarHandler();
                if(ch != null) {
                    ch.setTrunkState(isChecked ? CarHandler.TrunkState.Open : CarHandler.TrunkState.Closed);
                }
            }
        });
        mSwitchMFL.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                CarHandler ch = getCarHandler();
                if(ch != null) {
                    ch.setMovableFrontLightState(isChecked ? CarHandler.MovableFrontLightState.Active : CarHandler.MovableFrontLightState.Hidden);
                }
            }
        });

        adaptSwitchVisibility();
    }

    private void adaptSwitchVisibility() {
        boolean hasFeature = false;
        CarHandler ch = getCarHandler();
        if(ch == null) {
            return;
        }
        if(ch.hasTrunk()) {
            mSwitchTrunk.setVisibility(View.VISIBLE);
            mTextTrunk.setVisibility(View.VISIBLE);
            hasFeature = true;
        } else {
            mSwitchTrunk.setVisibility(View.GONE);
            mTextTrunk.setVisibility(View.GONE);
        }
        if(ch.hasMovableFrontLight()) {
            mSwitchMFL.setVisibility(View.VISIBLE);
            mTextMFL.setVisibility(View.VISIBLE);
            hasFeature = true;
        } else {
            mSwitchMFL.setVisibility(View.GONE);
            mTextMFL.setVisibility(View.GONE);
        }

        if(hasFeature) {
            mLinearLayoutSwitches.setVisibility(View.VISIBLE);
        } else {
            mLinearLayoutSwitches.setVisibility(View.GONE);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        String address = getIntent().getStringExtra(EXTRA_DEVICE_ADDRESS);
        ((Application) getApplication()).setActiveCar(address);
        CarHandler ch = getCarHandler();
        if(ch != null) {
            ch.setListener(this);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if(!isChangingConfigurations()) {
            CarHandler ch = getCarHandler();
            if(ch != null) {
                ch.setListener(null);
            }
            ((Application) getApplication()).setActiveCar(null);
        }
    }

    @Override
    public void readyStateChanged(final boolean isReady) {
        //TODO: disable control elements when isReady == false
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(isReady) {
                    adaptSwitchVisibility();
                }
            }
        });
    }
}
