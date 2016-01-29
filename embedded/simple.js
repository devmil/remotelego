var mraa = require('mraa');
var egpio = require('./peripherals/edison/EdisonGpio');
var epwm = require('./peripherals/edison/EdisonPwm');
var gpioOnOff = new egpio.EdisonGpio(14);
gpioOnOff.open(true);
gpioOnOff.write(false);
var gpioDirection = new egpio.EdisonGpio(15);
gpioDirection.open(true);
gpioDirection.write(false);
var pwmServo = new epwm.EdisonPwm(13);
pwmServo.open(20000); //20ms
// var pinServo = new mraa.Gpio(14); //=13
// pinServo.dir(mraa.DIR_OUT);
var pwmMotor = new epwm.EdisonPwm(12);
pwmMotor.open(1000);
pwmMotor.setPercentage(1);
// setTimeout(() => {
//     gpioOnOff.write(true);
//     pwmMotor.setPercentage(0.8);
//     pwmServo.setPercentage(0.1);
// }, 4000);
// setTimeout(() => {
//     gpioDirection.write(true);
//     pwmMotor.setPercentage(0.0);
//     pwmServo.setPercentage(0.05);
// }, 8000);
// 
// setTimeout(() => {
//     pwmMotor.setPercentage(0.5);
//     pwmServo.setPercentage(0.2);
// }, 12000);
// setTimeout(() => {
//     gpioDirection.write(false);
// }, 16000);
// setTimeout(() => {
//     gpioOnOff.write(false);
//     pwmServo.setPercentage(0.0);
//     pwmMotor.setPercentage(0.0);
//     process.exit();
// }, 20000);
//take #2
// gpioOnOff.write(true);
// gpioDirection.write(true);
// pwmMotor.setPercentage(1);
// 
// setTimeout(() => {
//     pwmMotor.setPercentage(0.8);
// }, 2000);
// 
// setTimeout(() => {
//     pwmMotor.setPercentage(0.6);
// }, 4000);
// 
// setTimeout(() => {
//     pwmMotor.setPercentage(0.4);
// }, 6000);
// 
// setTimeout(() => {
//     pwmMotor.setPercentage(0.2);
// }, 8000);
// 
// setTimeout(() => {
//     gpioOnOff.write(false);
//     gpioDirection.write(false);
//     pwmServo.setPercentage(0.0);
//     pwmMotor.setPercentage(0.0);
//     process.exit();
// }, 10000);
//Servo pulse test
setTimeout(function () {
    pwmServo.setPulseWidth(500);
}, 2000);
setTimeout(function () {
    pwmServo.setPulseWidth(2540);
}, 5000);
setTimeout(function () {
    pwmServo.setPulseWidth(1500);
    process.exit();
}, 10000);
//Servo test
// var current: number = 0;
// 
// setInterval(() => {
//     pwmServo.setPercentage(current);
//     current += 0.01;
//     if(current >= 1) {
//         pwmServo.setPercentage(0);
//         process.exit();
//     }
// }, 1000);
// var servoTimer = new NanoTimer();
// var servoCount = 0;
// servoTimer.setInterval(() => {
//     if(servoCount == 0) {
//         pinServo.write(1);
//     } else if(servoCount == 2) {
//         pinServo.write(0);
//     }
//     
//     servoCount++;
//     
//     if(servoCount == 40) { //20ms
//         servoCount = 0;
//     }
// }, '', '500u'); 
