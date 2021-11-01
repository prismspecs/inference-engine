# Dependencies

cd of/addons
```
git clone https://github.com/runwayml/ofxRunway.git
git clone https://github.com/bakercp/ofxHTTP.git
git clone https://github.com/bakercp/ofxIO.git
git clone https://github.com/bakercp/ofxMediaType.git
git clone https://github.com/bakercp/ofxNetworkUtils.git
git clone https://github.com/bakercp/ofxSSLManager.git
git clone https://github.com/bakercp/ofxSerial.git
git clone https://github.com/mneunomne/ofxGLFWJoystick.git
```

# Running local in develop mode on runway

Build the docker from Dockerfile
```
docker build . --tag my/tf1
```

Run a docker container (the latest image automatically loads the python script)
```
docker run --expose 9000 --network="host" -v $PWD:/workspace/ --gpus all -it my/tf1
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

