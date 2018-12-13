# Mari
This is just a temporary code release

## Get Started
In order to make your computer ready to compile our code, first run the script *setup*.
```shell
./setup
```
This script will guide through all the required steps to make your environment ready. You will be asked to place an SDK inside the newly created *sdk* folder. We have used the version 2.1.4.13 from Aldebaran which can be found in [Aldebaran Community](https://community.ald.softbankrobotics.com/en/resources/software) website or in our [Mari Assets](https://github.com/AlexanderSilvaB/Mari-Assets) repository. 

On running this script without any arguments, a toolchain called **rinobot-robot** will be created on your system. You can also setup with a custom toolchain name. To do that just run the script with a toolchain name as argument.
```shell
./setup toolchain-name
```

## Install on robot
To install our code release on NAO, use the *sync* script. This script will configure the robot and copy all needed files to the correct places. On the first time you run this script on any robot is needed that you use the argument *--all* to make sure that all needed settings will be made on the robot. In this script is always required to pass a robot IP or hostname as an argument.

You can use the *--build* argument to force the code compilation on sync. We recommend that you always use this parameter so the code changes will always be compiled before sync.

To configure and sync a robot for the first time you can use:
```shell
./sync robot-hostname --all
```

To sync the robot after the first configuration you just need to use:
```shell
./sync robot-hostname --build
```

By default this script uses a toolchain called **rinobot-toolchain**, if you change your toolchain name with the script *setup* you will need to pass the correct toolchain name as an argument.
```shell
./sync robot-hostname toolchain-name --build
```

## Structure
Our repository is structured in a way that 3 main folders are used, as described bellow:
* **root**: This will be copied to the robot root directory

* **src**: This is complete source code

* **supervisor**: Here lives our Supervisor software

There is also 3 main script that are used:
* **setup**: This script is used to configure the user machine

* **sync**: This script is responsible to configure the robot and copy the binaries and data needed to run our software.

* **build**: This is the script used to compile the source code. Is also possible to not run this script directly and instead use the argument *--build* in the *sync* script.