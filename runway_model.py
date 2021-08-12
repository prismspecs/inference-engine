import runway
from runway.data_types import number, text, image, vector
import pickle
import numpy as np
import tensorflow as tf
import dnnlib.tflib as tflib
import runway

import re

import dnnlib
import dnnlib.tflib as tflib


np.random.seed(0)
tf.random.set_random_seed(0)


# Setup the model, initialize weights, set the configs of the model, etc.
# Every model will have a different set of configurations and requirements.
# Check https://docs.runwayapp.ai/#/python-sdk to see a complete list of
# supported configs. The setup function should return the model ready to be
# used.


@runway.setup(options={'checkpoint': runway.file(extension='.pkl')})
def setup(opts):
    global Gs
    tflib.init_tf()
    with open(opts['checkpoint'], 'rb') as file:
        G, D, Gs = pickle.load(file)

    # Turn this on to develop locally
    #with open('model.pkl', 'rb') as file:
    #    G, D, Gs = pickle.load(file)

    return Gs

generate_inputs = {
    'z': runway.vector(512, sampling_std=0.5),
    'truncation': runway.number(min=0, max=3, default=0.8, step=0.01)
}

@runway.command('generate', inputs=generate_inputs, outputs={'image': runway.image})
def convert(model, inputs):
    z = inputs['z']
    truncation = inputs['truncation']
    latents = z.reshape((1, 512))
    label = np.zeros([1] + Gs.input_shapes[1][1:])
    fmt = dict(func=tflib.convert_images_to_uint8, nchw_to_nhwc=True)
    images = model.run(latents, label, truncation_psi=truncation, randomize_noise=False, output_transform=fmt)
    output = np.clip(images[0], 0, 255).astype(np.uint8)
    return {'image': output}


if __name__ == '__main__':
    # run the model server using the default network interface and ports,
    # displayed here for convenience
    runway.run(host='0.0.0.0', port=8000)
