interface IPwm {
    open(periodUs: number);
    setPercentage(percentage: number);
}