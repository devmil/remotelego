package de.devmil.legologic;

public class ValidationError {
    private DataField mDataField;
    private String mMessage;

    public enum DataField {
        Name,
        MaxSteeringAnglePositive,
        MaxSteeringAngleNegative,
        SteeringOffset
    }

    public ValidationError(DataField dataField, String message) {
        mDataField = dataField;
        mMessage = message;
    }

    public DataField getDataField() {
        return mDataField;
    }

    public String getMessage() {
        return mMessage;
    }

}
