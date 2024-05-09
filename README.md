## A Low-Cost Bionic Transradial Electromyographic Prosthesis Using Transformer-Based Deep Learning Algorithms

[International Science and Engineering Fair Project Board](https://projectboard.world/isef/project/robo024t-low-cost-transradial-electromyographic-prosthetic)

### Abstract:

Current commercial and research-grade prosthetics lack fine motor movements and have limited degrees of freedom. These
limitations in function force the amputee to compensate, which causes other health problems, further diminishing their
quality of life. Additionally, commercial prosthetics are expensive with roughly only 5% of people being able to afford
prosthetic devices. In light of the problems, we propose a transformer-based deep learning algorithm to read surface
electromyography (sEMG) signals and a 3D-printed transradial prosthetic capable of independent finger movement. The
transformer model employs attention mechanisms to overcome prior constraints: mode complexity, high computation and
latency, and the consideration of both spatial and temporal information. The overall cost of the prosthetic is less than
$450, more than 160 times cheaper than commercial prosthetics. Furthermore, our prosthetic utilizes high-accuracy servo
motors which allow for fine motor movements and has 15 degrees of freedom, more than three times that of current
research-grade prosthetics and 15 times that of commercial-grade ones. Additionally, the mechanical prosthetic has a
grasp speed of over three times faster than other state-of-the-art prosthetics and is comparable in weight to the human
hand.

### Code Description

The control algorithms for the prosthetic are run on an Orange Pi Zero2W. Data is collected from a EMGDataProvider then
fed into a 512 element circular buffer. Data from the buffer is then fed into the EMGDataClassifier which returns
directional classifications of each finger. These classifications are then used as a part of control algorithms which
determine the position of the finger given a constant velocity and acceleration, and a known starting position. To
account for inconsistencies in predictions, trapezoidal motion profiles were implemented to smooth motion of the servos
and the prosthetic. Outputed joint positions were then converted to servo positions using high order inverse kinematics
before being sent to the physical prosthetic using a PCA9685 control board.

Custom drivers were implemented for the servo controller board (using
Linux's [I2C component](https://www.kernel.org/doc/html/v4.9/driver-api/i2c.html)) found in the `PCA9686` package. The ML model was created in Python using PyTorch then traced using TorchScript before being
reloaded using libtorch into the `classifier` library. Collection of data and finger state prediction was seperated from
the mechanical control algorithms into discrete threads in the `standalone` package.

### Dependencies

#### Python
Classifier Backend
```requirements
pytorch==2.3.0
numpy==1.26.3
```

The OrangePi Zero2W runs Armbian.
The following dependencies were used on board and for the C++ control code:

```requirements
i2c-tools==4.3-2+b3
cmake==3.28.1
ccache
ninja
gdb==13.1-3
libi2c-dev==4.3-2+b3
git
libboost-all-dev==1.74.0.3
```

Pytorch currently doesn't build nightly builds for aarch64 and bust be manually built. These options will disable CUDA,
MKL, and PyBind bindings. Uses Ninja for faster build times.:

```shell
git clone -b v2.3.0 --recurse-submodule https://github.com/pytorch/pytorch.git
cd pytorch && pip3 install -r requirements.txt
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=/usr/bin/ninja -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DUSE_CUDA:BOOL=OFF -DBUILD_CUSTOM_PROTOBUF:BOOL=OFF -DBUILD_PYTHON:BOOL=OFF -DPYTHON_EXECUTABLE:PATH=`which python3` -G Ninja -S /home/prosthetic/pytorch -B /home/prosthetic/pytorch/build
cmake --build /home/prosthetic/pytorch/build --target install -- -j 2
```

The following procedure was used to modify I2C clock-frequency for the PCA driver. This was needed to smooth servo motion.
```shell
dtc -I dtb -O dts sun50i-h618-orangepi-zero2w.dtb -o sun50i-h618-orangepi-zero2w.dts
sudo nano sun50i-h618-orangepi-zero2w.dts
clock-frequency=<400000>
dts -O dtb sun50i-h618-orangepi-zero2w.dts -o sun50i-h618-orangepi-zero2w.dtb
```
