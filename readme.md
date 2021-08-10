# dependencies

cd of/addons
```
git clone https://github.com/runwayml/ofxRunway.git
git clone https://github.com/bakercp/ofxHTTP.git
git clone https://github.com/bakercp/ofxIO.git
git clone https://github.com/bakercp/ofxMediaType.git
git clone https://github.com/bakercp/ofxNetworkUtils.git
git clone https://github.com/bakercp/ofxSSLManager.git
```

# hosting on runway
+ Hosting on Runway requires runway_model.py, runway.yml. 
+ I then set up a virtualenv using [these instructions](https://gist.github.com/frfahim/73c0fad6350332cef7a653bcd762f08d)
+ first download python 3.7 https://linuxize.com/post/how-to-install-python-3-7-on-ubuntu-18-04/
```
virtualenv -p /usr/bin/python3.7 venv
source venv/bin/activate
pip3 install -r requirements.txt
deactivate
```


## useful links
https://help.runwayml.com/hc/en-us/articles/4401808774419-Adding-Models
[stylegan w runway implementation](https://github.com/agermanidis/stylegan)

