var plan = require('flightplan');
var params = require('./flightplan_params');

// configuration
plan.target('raspberrypi-2-wifi', {
  host: params.FlightplanParameters.getRaspberryPi2WifiParameters().getHost(),
  username: params.FlightplanParameters.getRaspberryPi2WifiParameters().getUserName(),
  password: params.FlightplanParameters.getRaspberryPi2WifiParameters().getPassword(),
  agent: process.env.SSH_AUTH_SOCK,
}, {
  FileOwnerName: params.FlightplanParameters.getRaspberryPi2WifiParameters().getUserName(),
  AppDirectoryName: params.FlightplanParameters.getRaspberryPi2WifiParameters().getAppDirectoryName(),
  AppFileName: params.FlightplanParameters.getRaspberryPi2WifiParameters().getAppFileName()
});

plan.target('raspberrypi-2-jeep', {
  host: params.FlightplanParameters.getRaspberryPi2JeepParameters().getHost(),
  username: params.FlightplanParameters.getRaspberryPi2JeepParameters().getUserName(),
  password: params.FlightplanParameters.getRaspberryPi2JeepParameters().getPassword(),
  agent: process.env.SSH_AUTH_SOCK,
}, {
  FileOwnerName: params.FlightplanParameters.getRaspberryPi2JeepParameters().getUserName(),
  AppDirectoryName: params.FlightplanParameters.getRaspberryPi2JeepParameters().getAppDirectoryName(),
  AppFileName: params.FlightplanParameters.getRaspberryPi2JeepParameters().getAppFileName()
});

plan.target('edison', {
  host: params.FlightplanParameters.getEdisonParameters().getHost(),
  username: params.FlightplanParameters.getEdisonParameters().getUserName(),
  password: params.FlightplanParameters.getEdisonParameters().getPassword(),
  agent: process.env.SSH_AUTH_SOCK,
}, {
  FileOwnerName: params.FlightplanParameters.getEdisonParameters().getUserName(),
  AppDirectoryName: params.FlightplanParameters.getEdisonParameters().getAppDirectoryName(),
  AppFileName: params.FlightplanParameters.getEdisonParameters().getAppFileName()
});

plan.target('fexdev', {
  host: params.FlightplanParameters.getRaspberryPiFexDevParameters().getHost(),
  username: params.FlightplanParameters.getRaspberryPiFexDevParameters().getUserName(),
  password: params.FlightplanParameters.getRaspberryPiFexDevParameters().getPassword(),
  agent: process.env.SSH_AUTH_SOCK,
}, {
  FileOwnerName: params.FlightplanParameters.getRaspberryPiFexDevParameters().getUserName(),
  AppDirectoryName: params.FlightplanParameters.getRaspberryPiFexDevParameters().getAppDirectoryName(),
  AppFileName: params.FlightplanParameters.getRaspberryPiFexDevParameters().getAppFileName()
});


var tmpDir = 'raspberry-legotruck-' + new Date().getTime();

// run commands on localhost
plan.local(function(local) {
  local.log('Run build');
  local.exec('tsc');

  local.log('Copy files to remote hosts');
  var filesToCopy = local.exec('git ls-files', {silent: true});
  // rsync files to all the target's remote hosts
  local.transfer(filesToCopy, '~/tmp/' + tmpDir);
});

// run commands on the target's remote hosts
plan.remote(function(remote) {
    var appDirectory = plan.runtime.options.AppDirectoryName;
    remote.log('Deleting ' + appDirectory + ' content');
    remote.exec('mkdir -p ~/apps');
    remote.exec('mkdir -p ~/apps/' + appDirectory);
    remote.exec('mkdir -p ~/apps/' + appDirectory + '/dummy');
    
    var filesOut = remote.exec('find ~/apps/' + appDirectory + '/ -type f -maxdepth 1').stdout;
    if(filesOut != null) { 
        var files = filesOut.split('\n');
        files.forEach(function(file) {
            if("" != file) {
                remote.exec('rm ' + file);
            }
        });
    }
    
    var directoriesOut = remote.exec('ls -d ~/apps/' + appDirectory + '/*/').stdout;
    if(directoriesOut != null) {
    var directories = directoriesOut.split('\n');
        directories.forEach(function(dir) {
            if("" != dir
                && dir.indexOf("node_modules") < 0
                && dir.indexOf("forever") < 0) {
                    remote.rm('-rf ' + dir);
            }
        });
    } 

    remote.log('Move folder to app root');
    remote.sudo('cp -R ~/tmp/' + tmpDir + '/. ~/apps/' + appDirectory + '/', {user: plan.runtime.options.FileOwnerName});
    remote.rm('-rf ~/tmp/' + tmpDir);
    
    //copy application file
    remote.exec('mv ~/apps/' + appDirectory + '/' + plan.runtime.options.AppFileName + '.js ~/apps/' + appDirectory + '/app.js')
    //install module dependencies
    remote.exec('cd ~/apps/' + appDirectory + '; npm update');
});
