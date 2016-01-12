var plan = require('flightplan');

// configuration
plan.target('staging', {
  host: 'raspberrypi-2-wifi',
  username: 'pi',
  password: 'raspberry',
  agent: process.env.SSH_AUTH_SOCK,
}, {
  FileOwnerName: 'pi'    
});

plan.target('staging_wired', {
  host: 'raspberrypi-2',
  username: 'pi',
  password: 'raspberry',
  agent: process.env.SSH_AUTH_SOCK,
}, {
  FileOwnerName: 'pi'    
});

plan.target('staging_flo', {
  host: 'fexdevpi',
  username: 'pi',
  password: 'raspberry',
  agent: process.env.SSH_AUTH_SOCK,
}, {
  FileOwnerName: 'pi'    
});

plan.target('staging_v1', {
  host: 'raspberrypi-1-wifi',
  username: 'pi',
  password: 'raspberry',
  agent: process.env.SSH_AUTH_SOCK,
}, {
  FileOwnerName: 'pi'    
});

plan.target('staging_v1_wired', {
  host: 'raspberrypi-1',
  username: 'pi',
  password: 'raspberry',
  agent: process.env.SSH_AUTH_SOCK,
}, {
  FileOwnerName: 'pi'    
});

plan.target('edison_wifi', {
  host: 'edison',
  username: 'root',
  password: 'edisonedison',
  agent: process.env.SSH_AUTH_SOCK,
}, {
  FileOwnerName: 'pi'    
});

var tmpDir = 'raspberry-led-' + new Date().getTime();

// run commands on localhost
plan.local(function(local) {
  local.log('Run build');

  local.log('Copy files to remote hosts');
  var filesToCopy = local.exec('git ls-files', {silent: true});
  // rsync files to all the target's remote hosts
  local.transfer(filesToCopy, '~/tmp/' + tmpDir);
});

// run commands on the target's remote hosts
plan.remote(function(remote) {
    remote.log('Deleting led content');
    remote.exec('mkdir -p ~/apps');
    remote.exec('mkdir -p ~/apps/led');
    remote.exec('mkdir -p ~/apps/led/dummy');
    
    var filesOut = remote.exec('find ~/apps/led/ -type f -maxdepth 1').stdout;
    if(filesOut != null) { 
        var files = filesOut.split('\n');
        files.forEach(function(file) {
            if("" != file) {
                remote.exec('rm ' + file);
            }
        });
    }
    
    var directoriesOut = remote.exec('ls -d ~/apps/led/*/').stdout;
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
    remote.sudo('cp -R ~/tmp/' + tmpDir + '/. ~/apps/led/', {user: plan.runtime.options.FileOwnerName});
    remote.rm('-rf ~/tmp/' + tmpDir);
    //install module dependencies
    remote.exec('cd ~/apps/led; npm update');
});
