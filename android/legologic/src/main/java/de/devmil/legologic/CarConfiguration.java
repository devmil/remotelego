package de.devmil.legologic;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

public class CarConfiguration {
    private boolean isSteeringInverted;
    private int maxSteeringAnglePositive;
    private int maxSteeringAngleNegative;
    private int steeringOffset;

    private String name;

    private boolean hasMovingFrontLights;
    private boolean hasTrunk;
    private boolean canBlink;

    private static final int NAME_LENGTH = 13;

    public boolean isSteeringInverted() {
        return isSteeringInverted;
    }

    public void setSteeringInverted(boolean steeringInverted) {
        isSteeringInverted = steeringInverted;
    }

    public int getMaxSteeringAnglePositive() {
        return maxSteeringAnglePositive;
    }

    public void setMaxSteeringAnglePositive(int maxSteeringAnglePositive) {
        this.maxSteeringAnglePositive = maxSteeringAnglePositive;
    }

    public int getMaxSteeringAngleNegative() {
        return maxSteeringAngleNegative;
    }

    public void setMaxSteeringAngleNegative(int maxSteeringAngleNegative) {
        this.maxSteeringAngleNegative = maxSteeringAngleNegative;
    }

    public int getSteeringOffset() {
        return steeringOffset;
    }

    public void setSteeringOffset(int steeringOffset) {
        this.steeringOffset = steeringOffset;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public boolean hasMovingFrontLights() {
        return hasMovingFrontLights;
    }

    public void setHasMovingFrontLights(boolean hasMovingFrontLights) {
        this.hasMovingFrontLights = hasMovingFrontLights;
    }

    public boolean hasTrunk() {
        return hasTrunk;
    }

    public void setHasTrunk(boolean hasTrunk) {
        this.hasTrunk = hasTrunk;
    }

    public boolean canBlink() {
        return canBlink;
    }

    public void setCanBlink(boolean canBlink) {
        this.canBlink = canBlink;
    }

    public List<ValidationError> validate() {
        List<ValidationError> result = new ArrayList<>();
        //1. angles
        if(maxSteeringAnglePositive > 90 || maxSteeringAnglePositive < 0) {
            result.add(new ValidationError(ValidationError.DataField.MaxSteeringAnglePositive, "Max positive steering angle has to be between 0 and 90"));
        }
        if(maxSteeringAngleNegative < -90 || maxSteeringAngleNegative > 0) {
            result.add(new ValidationError(ValidationError.DataField.MaxSteeringAngleNegative, "Max negative steering angle has to be between -90 and 0"));
        }
        if(steeringOffset > 90 || steeringOffset < -90) {
            result.add(new ValidationError(ValidationError.DataField.SteeringOffset, "Steering offset has to be between -90 and 0"));
        }

        //2. name availability
        if(name.equals("")) {
            result.add(new ValidationError(ValidationError.DataField.Name, "Name can not be empty"));
        }

        //3. name length
        if(name.length() > NAME_LENGTH) {
            result.add(new ValidationError(ValidationError.DataField.Name, String.format(Locale.getDefault(), "Name can only be %d characters long", NAME_LENGTH)));
        }

        return result;
    }

    public byte[] toBytes() {
        List<Byte> result = new ArrayList<>();
        result.add(isSteeringInverted ? (byte)0x01 : (byte)0x00);
        result.add((byte) maxSteeringAnglePositive);
        result.add((byte) maxSteeringAngleNegative);
        result.add((byte) steeringOffset);
        for(int i=0; i<NAME_LENGTH; i++) {
            if(name.length() > i) {
                result.add((byte)name.charAt(i));
            } else {
                result.add((byte)0);
            }
        }
        result.add(hasMovingFrontLights ? (byte)1 : (byte)0);
        result.add(hasTrunk ? (byte)1 : (byte)0);
        result.add(canBlink ? (byte)1 : (byte)0);

        byte[] resultBytes = new byte[result.size()];
        for(int i=0; i<result.size(); i++) {
            resultBytes[i] = result.get(i);
        }

        return resultBytes;
    }

    public static CarConfiguration fromBytes(byte[] bytes) {
        if(bytes == null) {
            return null;
        }
        CarConfiguration result = new CarConfiguration();
        int idx = 0;
        result.setSteeringInverted(bytes[idx] != 0);
        idx++;
        result.setMaxSteeringAnglePositive((int)bytes[idx]);
        idx++;
        result.setMaxSteeringAngleNegative((int)bytes[idx]);
        idx++;
        result.setSteeringOffset((int)bytes[idx]);
        idx++;

        byte[] nameBytes = new byte[NAME_LENGTH];
        for(int i=0; i<NAME_LENGTH; i++) {
            nameBytes[i] = bytes[idx + i];
        }
        idx += NAME_LENGTH;

        try {
            result.setName(new String(nameBytes, "UTF-8"));
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }

        result.setHasMovingFrontLights(bytes[idx] != 0);
        idx++;
        result.setHasTrunk(bytes[idx] != 0);
        idx++;
        result.setCanBlink(bytes[idx] != 0);
        idx++;

        return result;
    }
}
