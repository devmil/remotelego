interface IPwm {
    open(periodUs: number);
    setPercentage(percentage: number);
    setPulseWidth(width: number);
}