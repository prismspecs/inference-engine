#!/usr/bin/env python3
from http.server import BaseHTTPRequestHandler, HTTPServer
import logging
import requests
import json

import pickle
import dnnlib
import torch
import PIL.Image

with open('checkpoints/stylegan3-r-ffhq-1024x1024.pkl', 'rb') as f:
    G = pickle.load(f)['G_ema'].cuda()  # torch.nn.Module

class S(BaseHTTPRequestHandler):
    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()

    def do_GET(self):
        logging.info("GET request,\nPath: %s\nHeaders:\n%s\n", str(self.path), str(self.headers))
        self._set_response()
        self.wfile.write("GET request for {}".format(self.path).encode('utf-8'))

    def do_POST(self):
        content_length = int(self.headers['Content-Length']) # <--- Gets the size of data
        post_data = self.rfile.read(content_length) # <--- Gets the data itself
        #logging.info("POST request,\nPath: %s\nHeaders:\n%s\n\nBody:\n%s\n",
                #str(self.path), str(self.headers), post_data.decode('utf-8'))

        # self._set_response()
        # self.wfile.write("POST request for {}".format(self.path).encode('utf-8'))

        # lots of transformations before the json data can be used as an input tensor
        j = json.loads(post_data)
        jz = j["z"]
        jzf = [float(i) for i in jz]
        jzft = torch.FloatTensor(jzf)
        jzftr = jzft.reshape([1, 512])

        # same for truncation
        jt = j["truncation"]
        jtf = float(jt)

        #z = torch.randn([1, G.z_dim]).cuda()    # latent codes
        z = jzftr.cuda()
        c = None                                # class labels (not used in this example)
        trunc = jtf
        img = G(z, c, trunc)                           # NCHW, float32, dynamic range [-1, +1], no truncation
        img = (img.permute(0, 2, 3, 1) * 127.5 + 128).clamp(0, 255).to(torch.uint8)
        PIL.Image.fromarray(img[0].cpu().numpy(), 'RGB').save('newtest.png')


def run(server_class=HTTPServer, handler_class=S, port=8080):
    logging.basicConfig(level=logging.INFO)
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    logging.info('Starting httpd...\n')
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    logging.info('Stopping httpd...\n')

if __name__ == '__main__':
    from sys import argv

    if len(argv) == 2:
        run(port=int(argv[1]))
    else:
        run()
