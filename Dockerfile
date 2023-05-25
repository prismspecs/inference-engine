ARG BASE_IMAGE=pytorch/pytorch:1.10.0-cuda11.3-cudnn8-devel
FROM $BASE_IMAGE

RUN pip install scipy==1.3.3
RUN pip install runway-python
# for generating videos etc from server.py
#RUN pip install moviepy
#RUN pip install opensimplex
#RUN apt-get update && apt-get install -y ffmpeg

RUN apt-key adv --fetch-keys https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/3bf863cc.pub
RUN apt-key adv --fetch-keys https://developer.download.nvidia.com/compute/machine-learning/repos/ubuntu1804/x86_64/7fa2af80.pub

# for torch stuff
RUN pip install torch_utils
RUN pip install ninja

# make all files created belong to me instead of docker root
RUN usermod -u 1000 www-data
RUN usermod -G staff www-data

# networking
EXPOSE 9000

# autorun script
CMD ["python3", "flask-server.py"]
