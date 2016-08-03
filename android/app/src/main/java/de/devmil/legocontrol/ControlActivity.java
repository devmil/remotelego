package de.devmil.legocontrol;

import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.AppCompatCheckBox;
import android.support.v7.widget.ListViewCompat;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.ScrollView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

import java.util.List;
import java.util.Locale;

import de.devmil.legologic.CarConfiguration;
import de.devmil.legologic.CarHandler;
import de.devmil.legologic.DirectionControl;
import de.devmil.legologic.ICarHandlerDataListener;
import de.devmil.legologic.ICarHandlerStateListener;
import de.devmil.legologic.ValidationError;

public class ControlActivity extends AppCompatActivity implements ICarHandlerStateListener, ICarHandlerDataListener {

    private static final String EXTRA_DEVICE_ADDRESS = "DEVICE_ADDRESS";

    static Intent getLaunchIntent(Context context, BluetoothDevice device) {
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
    LinearLayout mllSettingsOption;

    private Switch mSwitchTrunk;
    private Switch mSwitchMFL;
    private RadioButton mrbBlinkOff;
    private RadioButton mrbBlinkL;
    private RadioButton mrbBlinkR;
    private RadioButton mrbBlinkLR;

    private Button btnSettings;

    private View vSettings;

    private Button btnSettingsSave;
    private Button btnSettingsDiscard;

    //Settings
    private AppCompatCheckBox mChkSteeringInverted;
    private EditText mEditSteeringMaxAnglePositive;
    private EditText mEditSteeringMaxAngleNegative;
    private EditText mEditSteeringOffset;
    private EditText mEditSettingsName;
    private AppCompatCheckBox mChkCanBlink;
    private AppCompatCheckBox mChkHasMovingFrontLights;
    private AppCompatCheckBox mChkHasTrunk;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_control);

        mDirectionControl = (DirectionControl)findViewById(R.id.activity_control_direction);
        mLinearLayoutSwitches = (LinearLayout)findViewById(R.id.activity_control_ll_switches);

        mllTrunk = (LinearLayout)findViewById(R.id.activity_control_ll_trunk);
        mllMFL = (LinearLayout)findViewById(R.id.activity_control_ll_mfl);
        mllBlink = (LinearLayout)findViewById(R.id.activity_control_ll_blink);
        mllSettingsOption = (LinearLayout)findViewById(R.id.activity_control_ll_settings);

        mSwitchTrunk = (Switch)findViewById(R.id.activity_control_sw_trunk);
        mSwitchMFL = (Switch)findViewById(R.id.activity_control_sw_mfl);
        mrbBlinkOff = (RadioButton)findViewById(R.id.activity_control_rb_blink_off);
        mrbBlinkL = (RadioButton)findViewById(R.id.activity_control_rb_blink_l);
        mrbBlinkR = (RadioButton)findViewById(R.id.activity_control_rb_blink_r);
        mrbBlinkLR = (RadioButton)findViewById(R.id.activity_control_rb_blink_lr);

        btnSettings = (Button)findViewById(R.id.activity_control_ll_settings_btn);

        vSettings = findViewById(R.id.activity_control_Settings);

        btnSettingsSave = (Button)findViewById(R.id.activity_control_settings_btnSave);
        btnSettingsDiscard = (Button)findViewById(R.id.activity_control_settings_btnDiscard);

        mChkSteeringInverted = (AppCompatCheckBox)findViewById(R.id.activity_control_settings_chkSteeringInverted);
        mEditSteeringMaxAnglePositive = (EditText)findViewById(R.id.activity_control_settings_editSteeringMaxAnglePositive);
        mEditSteeringMaxAngleNegative = (EditText)findViewById(R.id.activity_control_settings_editSteeringMaxAngleNegative);
        mEditSteeringOffset = (EditText)findViewById(R.id.activity_control_settings_editSteeringOffset);
        mEditSettingsName = (EditText)findViewById(R.id.activity_control_settings_editName);
        mChkCanBlink = (AppCompatCheckBox)findViewById(R.id.activity_control_settings_chkFeatureBlink);
        mChkHasMovingFrontLights = (AppCompatCheckBox)findViewById(R.id.activity_control_settings_chkFeatureFrontLights);
        mChkHasTrunk = (AppCompatCheckBox)findViewById(R.id.activity_control_settings_chkFeatureTrunk);

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

        btnSettings.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setConfigurationValuesToView();
                vSettings.setVisibility(View.VISIBLE);
                mDirectionControl.setVisibility(View.GONE);
                mLinearLayoutSwitches.setVisibility(View.GONE);
            }
        });

        btnSettingsSave.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(saveConfiguration()) {
                    vSettings.setVisibility(View.GONE);
                    mDirectionControl.setVisibility(View.VISIBLE);
                    adaptSwitchVisibility();
                }
            }
        });
        btnSettingsDiscard.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                vSettings.setVisibility(View.GONE);
                mDirectionControl.setVisibility(View.VISIBLE);
                adaptSwitchVisibility();
            }
        });

        configureEditTextValidation(mEditSettingsName, new IValidator() {
            @Override
            public boolean isValid(CarConfiguration configuration) {
                List<ValidationError> errors = configuration.validate();
                for(ValidationError error : errors) {
                    if(error.getDataField() == ValidationError.DataField.Name) {
                        return false;
                    }
                }
                return true;
            }
        });

        configureEditTextValidation(mEditSteeringMaxAnglePositive, new IValidator() {
            @Override
            public boolean isValid(CarConfiguration configuration) {
                List<ValidationError> errors = configuration.validate();
                for(ValidationError error : errors) {
                    if(error.getDataField() == ValidationError.DataField.MaxSteeringAnglePositive) {
                        return false;
                    }
                }
                return true;
            }
        });

        configureEditTextValidation(mEditSteeringMaxAngleNegative, new IValidator() {
            @Override
            public boolean isValid(CarConfiguration configuration) {
                List<ValidationError> errors = configuration.validate();
                for(ValidationError error : errors) {
                    if(error.getDataField() == ValidationError.DataField.MaxSteeringAngleNegative) {
                        return false;
                    }
                }
                return true;
            }
        });

        configureEditTextValidation(mEditSteeringOffset, new IValidator() {
            @Override
            public boolean isValid(CarConfiguration configuration) {
                List<ValidationError> errors = configuration.validate();
                for(ValidationError error : errors) {
                    if(error.getDataField() == ValidationError.DataField.SteeringOffset) {
                        return false;
                    }
                }
                return true;
            }
        });

        adaptSwitchVisibility();
        adaptModelState();
    }

    interface IValidator {
        boolean isValid(CarConfiguration configuration);
    }

    private void configureEditTextValidation(final EditText editText, final IValidator validator) {
        editText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
            }

            @Override
            public void afterTextChanged(Editable s) {
                CarConfiguration configuration = getCarConfigurationFromViews();
                if(configuration == null) {
                    return;
                }
                if(!validator.isValid(configuration)) {
                    editText.setBackgroundColor(Color.argb(60, 255, 0, 0));
                } else {
                    editText.setBackgroundColor(Color.TRANSPARENT);
                }
            }
        });
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

        if(ch != null && ch.hasConfiguration()) {
            mllSettingsOption.setVisibility(View.VISIBLE);
            hasFeature = true;
        } else {
            mllSettingsOption.setVisibility(View.GONE);
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

    private void setConfigurationValuesToView() {
        CarHandler ch = getCarHandler();
        if(ch == null) {
            return;
        }

        CarConfiguration configuration = ch.getConfiguration();

        if(configuration == null) {
            return;
        }

        mChkSteeringInverted.setChecked(configuration.isSteeringInverted());
        mEditSteeringMaxAnglePositive.setText(String.format(Locale.getDefault(), "%d", configuration.getMaxSteeringAnglePositive()));
        mEditSteeringMaxAngleNegative.setText(String.format(Locale.getDefault(), "%d", configuration.getMaxSteeringAngleNegative()));
        mEditSteeringOffset.setText(String.format(Locale.getDefault(), "%d", configuration.getSteeringOffset()));
        mEditSettingsName.setText(configuration.getName());
        mChkCanBlink.setChecked(configuration.canBlink());
        mChkHasMovingFrontLights.setChecked(configuration.hasMovingFrontLights());
        mChkHasTrunk.setChecked(configuration.hasTrunk());
    }

    private CarConfiguration getCarConfigurationFromViews() {
        try {
            CarConfiguration configuration = new CarConfiguration();
            configuration.setSteeringInverted(mChkSteeringInverted.isChecked());
            configuration.setMaxSteeringAnglePositive(Integer.parseInt(mEditSteeringMaxAnglePositive.getText().toString()));
            configuration.setMaxSteeringAngleNegative(Integer.parseInt(mEditSteeringMaxAngleNegative.getText().toString()));
            configuration.setSteeringOffset(Integer.parseInt(mEditSteeringOffset.getText().toString()));
            configuration.setName(mEditSettingsName.getText().toString());
            configuration.setCanBlink(mChkCanBlink.isChecked());
            configuration.setHasMovingFrontLights(mChkHasMovingFrontLights.isChecked());
            configuration.setHasTrunk(mChkHasTrunk.isChecked());

            return configuration;
        }
        catch(Exception e) {
            return null;
        }
    }

    private boolean saveConfiguration() {
        CarHandler ch = getCarHandler();
        if(ch == null) {
            return false;
        }

        CarConfiguration configuration = getCarConfigurationFromViews();
        if(configuration == null) {
            return false;
        }

        //Validate
        List<ValidationError> validationErrors = configuration.validate();
        if(validationErrors.size() > 0) {
            StringBuilder msgBuilder = new StringBuilder();
            for(ValidationError error : validationErrors) {
                if(msgBuilder.length() > 0) {
                    msgBuilder.append("\r\n");
                }
                msgBuilder.append(error.getMessage());
            }
            Toast.makeText(this, msgBuilder.toString(), Toast.LENGTH_LONG).show();
            return false;
        }

        ch.setConfiguration(configuration);

        return true;
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

    @Override
    public void carHandlerDataChanged() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                setConfigurationValuesToView();
            }
        });
    }
}
