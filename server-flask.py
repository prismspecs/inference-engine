from flask import Flask, jsonify, request, send_file # initialize our Flask application
import json
import io


import pickle
import dnnlib
import torch
import PIL.Image


with open('checkpoints/stylegan3-r-ffhq-1024x1024.pkl', 'rb') as f:
    G = pickle.load(f)['G_ema'].cuda()  # torch.nn.Module

app = Flask(__name__)

@app.route("/", methods=["POST"])

def setName():
    if request.method=='POST':

        data =  request.get_json()

        # print(data['z'])
        jzf = [float(i) for i in data['z']]
        jzft = torch.FloatTensor(jzf)
        jzftr = jzft.reshape([1, 512])

        jt = data["truncation"]
        jtf = float(jt)

        z = jzftr.cuda()
        c = None                                # class labels (not used in this example)
        trunc = jtf
        img = G(z, c, trunc)                           # NCHW, float32, dynamic range [-1, +1], no truncation
        img = (img.permute(0, 2, 3, 1) * 127.5 + 128).clamp(0, 255).to(torch.uint8)
        PIL.Image.fromarray(img[0].cpu().numpy(), 'RGB').save('newtest.png')

                # create file-object in memory
        file_object = io.BytesIO()

        # write PNG in file-object
        PIL.Image.fromarray(img[0].cpu().numpy(), 'RGB').save(file_object, 'PNG')

        # move to beginning of file so `send_file()` it will read from start
        file_object.seek(0)

        return send_file(file_object, mimetype='image/PNG')

        # return send_file("newtest.png", mimetype='image/gif')


if __name__ == '__main__':
    app.run(host='localhost', port=8080)
