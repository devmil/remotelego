interface ICarModel {
    
    setSteering(percent: number);
    
    getSteering(): number;
    
    setMotorSpeed(speedPercentage: number); 
    
    getMotorSpeed(): number;
    
    setConnected(isConnected: boolean);
}