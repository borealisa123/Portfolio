from keras import backend as K
K.clear_session()

from keras.datasets import mnist
from keras import models
from keras import layers
from keras.utils import to_categorical
import pandas as pd

#(train_images, train_labels), (test_images, test_labels) = mnist.load_data()
train = pd.read_csv("C:/Users/Aurora/Desktop/deep_learning/digit-recognizer/train.csv")
test = pd.read_csv("C:/Users/Aurora/Desktop/deep_learning/digit-recognizer/train.csv")

train_labels = train["label"]
train_images = train.drop(labels = ["label"],axis = 1)
del train 

test_labels = test["label"]
test_images = test.drop(labels = ["label"],axis = 1)
del test

# 2d tensor
train_images = train_images.values.reshape((-1, 28*28))
test_images = test_images.values.reshape((-1, 28*28))
train_images = train_images.astype('float32')/255
test_images = test_images.astype('float32')/255

train_labels = to_categorical(train_labels)
test_labels = to_categorical(test_labels)

val_images = train_images[:6000]
val_labels = train_labels[:6000]
partial_train_images = train_images[6000:]
partial_train_labels = train_labels[6000:]

# added another layer? doesn't seem to help
network = models.Sequential()
network.add(layers.Dense(512, activation='relu', input_shape=(28*28,)))
network.add(layers.Dense(512, activation='relu'))
network.add(layers.Dense(10, activation='softmax'))
network.compile(optimizer='rmsprop',
                loss='categorical_crossentropy',
                metrics=['accuracy'])

network.fit(partial_train_images,
            partial_train_labels,
            epochs=6,
            batch_size=128,
            validation_data=(val_images, val_labels))

test = network.evaluate(test_images, test_labels)
print(test)