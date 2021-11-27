import tensorflow as tf
from tensorflow.keras.models import load_model
import sys

# Python用モデルのパラメータをテキストに出力する

def get_layer_index(model, layer_name, not_found=None):
    for i, l in enumerate(model.layers):
        if l.name == layer_name:
            return i
    return not_found

model = load_model('models/' + sys.argv[1])

layer_names = ['diagonal8', 'edge2X', 'triangle']
names = []
for name in layer_names:
    names.append(name + '_dense0')
    names.append(name + '_dense1')
    names.append(name + '_out')
names.append('add_dense0')
names.append('add_dense1')
names.append('all_dense0')

with open('models/' + sys.argv[2], 'w') as f:
    for name in names:
        i = get_layer_index(model, name)
        try:
            #print(i, model.layers[i])
            dammy = model.layers[i]
            j = 0
            while True:
                try:
                    print(model.layers[i].weights[j].shape)
                    if len(model.layers[i].weights[j].shape) == 2:
                        for ii in range(model.layers[i].weights[j].shape[1]):
                            for jj in range(model.layers[i].weights[j].shape[0]):
                                f.write('{:.14f}'.format(model.layers[i].weights[j].numpy()[jj][ii]) + '\n')
                    elif len(model.layers[i].weights[j].shape) == 1:
                        for ii in range(model.layers[i].weights[j].shape[0]):
                            f.write('{:.14f}'.format(model.layers[i].weights[j].numpy()[ii]) + '\n')
                    j += 1
                except:
                    break
        except:
            break