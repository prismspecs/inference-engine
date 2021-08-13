ARG BASE_IMAGE=nvcr.io/nvidia/tensorflow:20.10-tf1-py3
FROM $BASE_IMAGE

RUN pip install scipy==1.3.3
RUN pip install runway-python
RUN apt-get update -y && apt-get install -y ffmpeg
