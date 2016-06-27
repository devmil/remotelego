export enum TrunkState {
    Unknown = 0,
    Closed = 1,
    Open = 2
}

export enum FrontLightState {
    Unknown = 0,
    Hidden = 1,
    Active = 2
}

export enum BlinkMode {
    Off = 0,
    Right = 1,
    Left = 2,
    Both = 3
}

export class Protocol {
    public static get SPEED_CHARACTERISTIC_UUID() : string { return "8d8ba32b-96be-4590-910b-c756c5222c9f"; }
	public static get STEER_CHARACTERISTIC_UUID() : string { return  "7baf8dca-2bfc-47fb-af29-042fccc180eb"; }
    public static get TRUNK_CHARACTERISTIC_UUID() : string { return "e0af3340-022e-47e1-a263-d68887dc41d4"; }
    public static get FRONT_LIGHT_CHARACTERISTIC_UUID() : string { return "fa10e4de-259e-4d23-9f59-45a9c66802ca"; }
    public static get BLINK_CHARACTERISTIC_UUID() : string { return "aad03b81-f2ea-47db-ae1e-7c2f9e86e93e"; }
} 