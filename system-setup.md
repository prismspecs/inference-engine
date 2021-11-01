# extract openframeworks to Desktop/...
cd ~/Desktop/of_v0.11.2_linux64gcc6_release/scripts/linux/ubuntu
sudo ./install_dependencies.sh
sudo ./install_codecs.sh
cd ..
./compileOF.sh

sudo snap install --classic code


sudo add-apt-repository ppa:appimagelauncher-team/stable
sudo apt-get update
sudo apt-get install appimagelauncher

# double click runway appimage, integrate and run

sudo apt install git

cd ~/Desktop/of_v0.11.2_linux64gcc6_release/apps/
git clone https://github.com/hiroMTB/vscode_oF.git

cd vscode_oF
git clone https://github.com/prismspecs/ganthro-of-runway.git

cd ~/Desktop/of_v0.11.2_linux64gcc6_release/addons

git clone https://github.com/runwayml/ofxRunway.git
git clone https://github.com/bakercp/ofxHTTP.git
git clone https://github.com/bakercp/ofxIO.git
git clone https://github.com/bakercp/ofxMediaType.git
git clone https://github.com/bakercp/ofxNetworkUtils.git
git clone https://github.com/bakercp/ofxSSLManager.git
git clone https://github.com/danomatika/ofxMidi.git
git clone https://github.com/mneunomne/ofxGLFWJoystick.git

cd ~/Desktop/of_v0.11.2_linux64gcc6_release/apps/vscode_oF/ganthro-of-runway
make

# install nvidia docker container
# https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html#docker

sudo apt install curl

curl https://get.docker.com | sh \
  && sudo systemctl --now enable docker
  
  distribution=$(. /etc/os-release;echo $ID$VERSION_ID) \
   && curl -s -L https://nvidia.github.io/nvidia-docker/gpgkey | sudo apt-key add - \
   && curl -s -L https://nvidia.github.io/nvidia-docker/$distribution/nvidia-docker.list | sudo tee /etc/apt/sources.list.d/nvidia-docker.list
   
sudo apt-get update
   
sudo apt-get install -y nvidia-docker2

sudo systemctl restart docker

sudo docker run --rm --gpus all nvidia/cuda:11.0-base nvidia-smi

sudo groupadd docker

sudo usermod -aG docker $USER

newgrp docker 

docker run hello-world

# restart

# ok... now need to add the model and create runwayml workspace
# runway -> models -> my models -> create model -> develop
# move checkpoints folder to ganthro-of-runway


# build the docker image
sudo apt install docker.io
cd ~/Desktop/of_v0.11.2_linux64gcc6_release/apps/vscode_oF/ganthro-of-runway
docker build . --tag my/tf1

# okay here maybe I should replace with ./start-game.sh
docker run --expose 9000 --network="host" -v $PWD:/workspace/ --gpus all -it my/tf1



