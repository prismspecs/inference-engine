# Dependencies

cd of/addons
```
git clone https://github.com/runwayml/ofxRunway.git
git clone https://github.com/fred-dev/ofxHTTP.git
git clone https://github.com/fred-dev/ofxIO.git
git clone https://github.com/bakercp/ofxMediaType.git
git clone https://github.com/bakercp/ofxNetworkUtils.git
git clone https://github.com/bakercp/ofxSSLManager.git
git clone https://github.com/bakercp/ofxSerial.git
git clone https://github.com/mneunomne/ofxGLFWJoystick.git
git clone https://github.com/danomatika/ofxMidi.git
```

# OpenFrameworks bugs
OF is riddled with bugs nowadays. In particular, this is an important fix re: Poco
[Link to discussion](https://github.com/openframeworks/openFrameworks/issues/6041)

The essential steps are to go into ofxPoco/addon_config.mk file and comment out the first ADDON_INCLUDES line, then remove or rename the /libs folder in ofxPoco as well
then ```make clean```

I also had to make changes to ofxIO:
https://github.com/fred-dev/ofxIO/commit/f1e96d278ac6940291509e8f1e75506789ad9217
and https://github.com/fred-dev/ofxHTTP
these are reflected in the addons git clones above

# Serial bugs

On Ubuntu, I had to

```
sudo usermod -a -G dialout <username>
sudo apt remove brltty 
```


# Running local in develop mode on runway

Build the docker from Dockerfile
```
docker build . --tag inference-engine
```

Run a docker container (the latest image automatically loads the python script)
```
docker run --expose 9000 --network="host" -v $PWD:/workspace/ --gpus all -it inference-engine
```

then run the python
```
python3 runway_model.py
```

then hit Connect in Runway

then run the OF app

## Previous build instructions for posterity

Run a docker container, expose port 9000 and 127.0.0.1, mount current working directory to /workspace/ in the container, enable gpu, and load the tf1 image into a bash terminal

```
docker run --expose 9000 --network="host" -v $PWD:/workspace/ --gpus all -it my/tf1 /bin/bash
```

# Notes

The dnnlib I was using before was incompatible with SG2-ada, so keep an eye out for that. I borrowed a lot from https://github.com/usufyan29/stylegan2_runway


# Useful links

https://help.runwayml.com/hc/en-us/articles/4401808774419-Adding-Models
[stylegan w runway implementation](https://github.com/agermanidis/stylegan)

