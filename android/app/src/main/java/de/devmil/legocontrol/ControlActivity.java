package de.devmil.legocontrol;

import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
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

    LinearLayout mllTrunk;
    LinearLayout mllMFL;
    LinearLayout mllBlink;

    private Switch mSwitchTrunk;
    private Switch mSwitchMFL;
    private RadioButton mrbBlinkOff;
    private RadioButton mrbBlinkL;
    private RadioButton mrbBlinkR;
    private RadioButton mrbBlinkLR;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_control);

        mDirectionControl = (DirectionControl)findViewById(R.id.activity_control_direction);
        mLinearLayoutSwitches = (LinearLayout)findViewById(R.id.activity_control_ll_switches);

        mllTrunk = (LinearLayout)findViewById(R.id.activity_control_ll_trunk);
        mllMFL = (LinearLayout)findViewById(R.id.activity_control_ll_mfl);
        mllBlink = (LinearLayout)findViewById(R.id.activity_control_ll_blink);

        mSwitchTrunk = (Switch)findViewById(R.id.activity_control_sw_trunk);
        mSwitchMFL = (Switch)findViewById(R.id.activity_control_sw_mfl);
        mrbBlinkOff = (RadioButton)findViewById(R.id.activity_control_rb_blink_off);
        mrbBlinkL = (RadioButton)findViewById(R.id.activity_control_rb_blink_l);
        mrbBlinkR = (RadioButton)findViewById(R.id.activity_control_rb_blink_r);
        mrbBlinkLR = (RadioButton)findViewById(R.id.activity_control_rb_blink_lr);

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

        mrbBlinkOff.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked) {
                    CarHandler ch = getCarHandler();
                    if(ch != null) {
                        ch.setBlinkMode(CarHandler.BlinkMode.Off);
                    }
                }
            }
        });
        mrbBlinkL.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked) {
                    CarHandler ch = getCarHandler();
                    if(ch != null) {
                        ch.setBlinkMode(CarHandler.BlinkMode.Left);
                    }
                }
            }
        });
        mrbBlinkR.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked) {
                    CarHandler ch = getCarHandler();
                    if(ch != null) {
                        ch.setBlinkMode(CarHandler.BlinkMode.Right);
                    }
                }
            }
        });
        mrbBlinkLR.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked) {
                    CarHandler ch = getCarHandler();
                    if(ch != null) {
                        ch.setBlinkMode(CarHandler.BlinkMode.Both);
                    }
                }
            }
        });

        adaptSwitchVisibility();
        adaptModelState();
    }

    private void adaptSwitchVisibility() {
        boolean hasFeature = false;
        CarHandler ch = getCarHandler();
        if(ch != null && ch.hasTrunk()) {
            mllTrunk.setVisibility(View.VISIBLE);
            hasFeature = true;
        } else {
            mllTrunk.setVisibility(View.GONE);
        }
        if(ch != null && ch.hasMovableFrontLight()) {
            mllMFL.setVisibility(View.VISIBLE);
            hasFeature = true;
        } else {
            mllMFL.setVisibility(View.GONE);
        }
        if(ch != null && ch.hasBlinkOption()) {
            mllBlink.setVisibility(View.VISIBLE);
            hasFeature = true;
        } else {
            mllBlink.setVisibility(View.GONE);
        }

        if(hasFeature) {
            mLinearLayoutSwitches.setVisibility(View.VISIBLE);
        } else {
            mLinearLayoutSwitches.setVisibility(View.GONE);
        }
    }

    private void adaptModelState() {
        CarHandler ch = getCarHandler();
        if(ch != null) {
            switch(ch.getBlinkMode()) {
                case Off:
                    mrbBlinkOff.setChecked(true);
                    break;
                case Left:
                    mrbBlinkL.setChecked(true);
                    break;
                case Right:
                    mrbBlinkR.setChecked(true);
                    break;
                case Both:
                    mrbBlinkLR.setChecked(true);
                    break;
                default:
                    break;
            }
            switch(ch.getMovableFrontLightState()) {
                case Hidden:
                    mSwitchMFL.setChecked(false);
                    break;
                case Active:
                    mSwitchMFL.setChecked(true);
                    break;
                default:
                    break;
            }
            switch(ch.getTrunkState()) {
                case Closed:
                    mSwitchTrunk.setChecked(false);
                    break;
                case Open:
                    mSwitchTrunk.setChecked(true);
                    break;
                default:
                    break;
            }
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
                    adaptModelState();
                }
            }
        });
    }
}
