from keras import backend as K
K.clear_session()

from keras.datasets import mnist
from keras import models
from keras import layers
from keras.utils import to_categorical
import pandas as pd

'''
(train_images, train_labels), (test_images, test_labels) = mnist.load_data()

train_images = train_images.reshape((60000,28,28,1))
test_images = test_images.reshape((10000,28,28,1))
train_images = train_images.astype('float32')/255
test_images = test_images.astype('float32')/255

train_labels = to_categorical(train_labels)
test_labels = to_categorical(test_labels)

val_images = train_images[:10000]
val_labels = train_labels[:10000]
partial_train_images = train_images[10000:]
partial_train_labels = train_labels[10000:]
'''

train = pd.read_csv("C:/Users/Aurora/Desktop/deep_learning/digit-recognizer/train.csv")
test = pd.read_csv("C:/Users/Aurora/Desktop/deep_learning/digit-recognizer/train.csv")

train_labels = train["label"]
train_images = train.drop(labels = ["label"],axis = 1)
del train 

test_labels = test["label"]
test_images = test.drop(labels = ["label"],axis = 1)
del test


train_images = train_images.values.reshape((-1, 28, 28, 1))
test_images = test_images.values.reshape((-1, 28, 28, 1))
train_images = train_images.astype('float32')/255
test_images = test_images.astype('float32')/255

train_labels = to_categorical(train_labels)
test_labels = to_categorical(test_labels)

val_images = train_images[:6000]
val_labels = train_labels[:6000]
partial_train_images = train_images[6000:]
partial_train_labels = train_labels[6000:]

model = models.Sequential()
model.add(layers.Conv2D(32, (3, 3),
                        activation='relu',
                        input_shape=(28, 28, 1)))
model.add(layers.MaxPooling2D((2, 2)))
model.add(layers.Conv2D(64, (3, 3), activation='relu'))
model.add(layers.MaxPooling2D((2, 2)))
model.add(layers.Conv2D(64, (3, 3), activation='relu'))
model.add(layers.Flatten())
model.add(layers.Dense(64, activation='relu'))
model.add(layers.Dense(10, activation='softmax'))

model.compile(optimizer='rmsprop',
              loss='categorical_crossentropy',
              metrics=['accuracy'])

'''
datagen = ImageDataGenerator(
	featurewise_center=False,  # set input mean to 0 over the dataset
	samplewise_center=False,  # set each sample mean to 0
	featurewise_std_normalization=False,  # divide inputs by std of the dataset
	samplewise_std_normalization=False,  # divide each input by its std
	zca_whitening=False,  # apply ZCA whitening
	rotation_range=10,  # randomly rotate images in the range (degrees, 0 to 180)
	zoom_range = 0.1, # Randomly zoom image 
	width_shift_range=0.1,  # randomly shift images horizontally (fraction of total width)
	height_shift_range=0.1,  # randomly shift images vertically (fraction of total height)
	horizontal_flip=False,  # randomly flip images
	vertical_flip=False)  # randomly flip images

datagen.fit(partial_train_images)
'''

model.fit(partial_train_images,
          partial_train_labels,
          epochs=6, 
          batch_size=64,
          validation_data=(val_images, val_labels))

test = model.evaluate(test_images, test_labels)
print(test)