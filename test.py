import numpy as np
import pickle
import dnnlib
import dnnlib.tflib as tflib
import PIL

from time import sleep



def generate_images(Gs, z, truncation):
    #print('Loading networks from "%s"...' % network_pkl)
    #_G, _D, Gs = load_networks(network_pkl)


    Gs_kwargs = dnnlib.EasyDict()
    Gs_kwargs.output_transform = dict(func=tflib.convert_images_to_uint8, nchw_to_nhwc=True)
    Gs_kwargs.randomize_noise = False
    if truncation is not None:
        Gs_kwargs.truncation_psi = truncation


    images = Gs.run(z, None, **Gs_kwargs) 
    PIL.Image.fromarray(images[0], 'RGB').save("test.png")


def load_networks(path_or_url):

    stream = open(path_or_url, 'rb')

    tflib.init_tf()
    with stream:
        G, D, Gs = pickle.load(stream, encoding='latin1')

    return G, D, Gs


def main():
    print("loading network")
    _G, _D, Gs = load_networks("checkpoints/checkpoint.pkl")
    
    while(True):
        z = np.random.randn(1, 512)
        generate_images(Gs, z, 1.0)
        sleep(0.5)


if __name__ == "__main__":
    main()
    

#z = np.random.randn(1, 512)
#generate_images("checkpoints/checkpoint.pkl",z,1.0)
