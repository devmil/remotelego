interface IPeripheralAccess {
    getGpio(pin: number) : IGpio;
    getPwm(pin: number) : IPwm;
}