import runway
from runway.data_types import number, text, image, vector
import pickle
import numpy as np
import re
import dnnlib
import torch
import PIL.Image

@runway.setup(options={'checkpoint': runway.file(extension='.pkl')})
def setup(opts):
    global G

    with open('checkpoints/stylegan3-r-ffhq-1024x1024.pkl', 'rb') as file:
        G = pickle.load(file)['G_ema'].cuda()

    return G

generate_inputs = {
    'z': runway.vector(512, sampling_std=0.5),
    'truncation': runway.number(min=0, max=3, default=0.8, step=0.01)
}

@runway.command('generate', inputs=generate_inputs, outputs={'image': runway.image})
def convert(model, inputs):


    z = inputs['z']
    latents = torch.tensor(z, dtype=torch.float, device="cuda").view(1, 512)

    truncation = inputs['truncation']

    with torch.no_grad():
        img = G(latents, None, truncation)
        
    img = (img.permute(0, 2, 3, 1) * 127.5 + 128).clamp(0, 255).to(torch.uint8)

    return {'image': img[0].cpu().numpy()}


if __name__ == '__main__':
    # run the model server using the default network interface and ports,
    # displayed here for convenience
    runway.run(host='127.0.0.1', port=9000)
