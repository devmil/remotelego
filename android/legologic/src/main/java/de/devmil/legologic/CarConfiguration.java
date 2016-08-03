package de.devmil.legologic;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

public class CarConfiguration {
    private boolean mIsSteeringInverted;
    private int mMaxSteeringAnglePositive;
    private int mMaxSteeringAngleNegative;
    private int mSteeringOffset;

    private String mName;

    private boolean mHasMovingFrontLights;
    private boolean mHasTrunk;
    private boolean mCanBlink;

    private boolean mIsDataInitialized;
    private boolean mIsNameInitialized;

    private static final int NAME_LENGTH = 20;

    public boolean isDataInitialized() {
        return mIsDataInitialized;
    }

    public boolean isNameInitialized() {
        return mIsNameInitialized;
    }

    public boolean isSteeringInverted() {
        return mIsSteeringInverted;
    }

    public void setSteeringInverted(boolean steeringInverted) {
        mIsSteeringInverted = steeringInverted;
    }

    public int getMaxSteeringAnglePositive() {
        return mMaxSteeringAnglePositive;
    }

    public void setMaxSteeringAnglePositive(int maxSteeringAnglePositive) {
        this.mMaxSteeringAnglePositive = maxSteeringAnglePositive;
    }

    public int getMaxSteeringAngleNegative() {
        return mMaxSteeringAngleNegative;
    }

    public void setMaxSteeringAngleNegative(int maxSteeringAngleNegative) {
        this.mMaxSteeringAngleNegative = maxSteeringAngleNegative;
    }

    public int getSteeringOffset() {
        return mSteeringOffset;
    }

    public void setSteeringOffset(int steeringOffset) {
        this.mSteeringOffset = steeringOffset;
    }

    public String getName() {
        return mName;
    }

    public void setName(String name) {
        this.mName = name;
    }

    public boolean hasMovingFrontLights() {
        return mHasMovingFrontLights;
    }

    public void setHasMovingFrontLights(boolean hasMovingFrontLights) {
        this.mHasMovingFrontLights = hasMovingFrontLights;
    }

    public boolean hasTrunk() {
        return mHasTrunk;
    }

    public void setHasTrunk(boolean hasTrunk) {
        this.mHasTrunk = hasTrunk;
    }

    public boolean canBlink() {
        return mCanBlink;
    }

    public void setCanBlink(boolean canBlink) {
        this.mCanBlink = canBlink;
    }

    public List<ValidationError> validate() {
        List<ValidationError> result = new ArrayList<>();
        //1. angles
        if(mMaxSteeringAnglePositive > 90 || mMaxSteeringAnglePositive < 0) {
            result.add(new ValidationError(ValidationError.DataField.MaxSteeringAnglePositive, "Max positive steering angle has to be between 0 and 90"));
        }
        if(mMaxSteeringAngleNegative < -90 || mMaxSteeringAngleNegative > 0) {
            result.add(new ValidationError(ValidationError.DataField.MaxSteeringAngleNegative, "Max negative steering angle has to be between -90 and 0"));
        }
        if(mSteeringOffset > 90 || mSteeringOffset < -90) {
            result.add(new ValidationError(ValidationError.DataField.SteeringOffset, "Steering offset has to be between -90 and 0"));
        }

        //2. mName availability
        if(mName.equals("")) {
            result.add(new ValidationError(ValidationError.DataField.Name, "Name can not be empty"));
        }

        //3. mName length
        if(mName.length() > NAME_LENGTH) {
            result.add(new ValidationError(ValidationError.DataField.Name, String.format(Locale.getDefault(), "Name can only be %d characters long", NAME_LENGTH)));
        }

        return result;
    }

    public byte[] toDataBytes() {
        List<Byte> result = new ArrayList<>();
        result.add(mIsSteeringInverted ? (byte)0x01 : (byte)0x00);
        result.add((byte) mMaxSteeringAnglePositive);
        result.add((byte) mMaxSteeringAngleNegative);
        result.add((byte) mSteeringOffset);
        result.add(mHasMovingFrontLights ? (byte)1 : (byte)0);
        result.add(mHasTrunk ? (byte)1 : (byte)0);
        result.add(mCanBlink ? (byte)1 : (byte)0);

        byte[] resultBytes = new byte[result.size()];
        for(int i=0; i<result.size(); i++) {
            resultBytes[i] = result.get(i);
        }

        return resultBytes;
    }

    public byte[] toNameBytes() {
        List<Byte> result = new ArrayList<>();
        for(int i=0; i<NAME_LENGTH; i++) {
            if(mName.length() > i) {
                result.add((byte) mName.charAt(i));
            } else {
                result.add((byte)0);
            }
        }

        byte[] resultBytes = new byte[result.size()];
        for(int i=0; i<result.size(); i++) {
            resultBytes[i] = result.get(i);
        }

        return resultBytes;
    }

    public void setNameBytes(byte[] bytes) {
        if(bytes == null) {
            return;
        }

        mIsNameInitialized = true;

        int length = NAME_LENGTH;
        for(int i=0; i<NAME_LENGTH; i++) {
            if(bytes[i] == 0) {
                length = i;
                break;
            }
        }

        byte[] nameBytes = new byte[length];

        for(int i=0; i<length; i++) {
            nameBytes[i] = bytes[i];
        }

        try {
            setName(new String(nameBytes, "UTF-8"));
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
    }

    public void setDataBytes(byte[] bytes) {
        if(bytes == null) {
            return;
        }
        mIsDataInitialized = true;
        int idx = 0;
        setSteeringInverted(bytes[idx] != 0);
        idx++;
        setMaxSteeringAnglePositive((int)bytes[idx]);
        idx++;
        setMaxSteeringAngleNegative((int)bytes[idx]);
        idx++;
        setSteeringOffset((int)bytes[idx]);
        idx++;
        setHasMovingFrontLights(bytes[idx] != 0);
        idx++;
        setHasTrunk(bytes[idx] != 0);
        idx++;
        setCanBlink(bytes[idx] != 0);
        idx++;
    }

    public static CarConfiguration fromDataBytes(byte[] bytes) {
        if(bytes == null) {
            return null;
        }
        CarConfiguration result = new CarConfiguration();
        result.setDataBytes(bytes);

        return result;
    }
}
