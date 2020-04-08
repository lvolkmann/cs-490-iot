# load and evaluate a saved model
# from numpy import loadtxt
# from keras.models import load_model
# import numpy as np
# from keras.models import Sequential
# from keras.layers import Dense, Dropout, Activation, Flatten
# from keras.layers import Convolution2D, MaxPooling2D
# from keras.optimizers import Adam
# from keras.utils import np_utils
# from sklearn import metrics
from keras.models import model_from_json
import pickle
from keras.models import load_model
import librosa
import numpy as np
import sklearn.preprocessing
from sklearn.preprocessing import LabelEncoder
import time


# Thanks to: https://github.com/mikesmales/Udacity-ML-Capstone
def extract_feature(file_name):
    try:
        audio_data, sample_rate = librosa.load(file_name, res_type='kaiser_fast')
        mfccs = librosa.feature.mfcc(y=audio_data, sr=sample_rate, n_mfcc=40)
        mfccsscaled = np.mean(mfccs.T, axis=0)

    except Exception as e:
        print("Error encountered while parsing file: ", file_name)
        return None, None

    return np.array([mfccsscaled])

# Thanks to: https://github.com/mikesmales/Udacity-ML-Capstone
def print_prediction(file_name):
    prediction_feature = extract_feature(file_name)

    predicted_vector = model.predict_classes(prediction_feature)

    predicted_class = le.inverse_transform(predicted_vector)
    print("The predicted class is:", predicted_class[0], '\n')

    predicted_proba_vector = model.predict_proba(prediction_feature)
    predicted_proba = predicted_proba_vector[0]
    for i in range(len(predicted_proba)):
        category = le.inverse_transform(np.array([i]))
        print(category[0], "\t\t : ", format(predicted_proba[i], '.32f'))


# Model adapted from: https://github.com/mikesmales/Udacity-ML-Capstone
# load model
model = load_model('model2.h5')

# summarize model
model.summary()

# load y values to shape label encoder
y = pickle.load(open("y.p", "rb"))
le = LabelEncoder()
le.fit(y)

# run prediction
start = time.time()
print_prediction("7383-3-1-0.wav")  # dog barking wav
print("RUNTIME: {}".format(time.time() - start))