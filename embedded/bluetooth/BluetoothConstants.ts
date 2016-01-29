export class BluetoothConstants {
    
    private static sServiceLegoCarGuid: string = '40480f29-7bad-4ea5-8bf8-499405c9b324';
    
    private static sCharacteristicSpeedGuid: string = '8d8ba32b-96be-4590-910b-c756c5222c9f';
    private static sCharacteristicSteerGuid: string = '7baf8dca-2bfc-47fb-af29-042fccc180eb';
    
    public static get SERVICE_LEGOCAR_GUID(): string {
        return BluetoothConstants.sServiceLegoCarGuid;
    }

    public static get CHARACTERISTIC_SPEED_GUID(): string {
        return BluetoothConstants.sCharacteristicSpeedGuid;
    }
    
    public static get CHARACTERISTIC_STEER_GUID(): string {
        return BluetoothConstants.sCharacteristicSteerGuid;
    }
    
}