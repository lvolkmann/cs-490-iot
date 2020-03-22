# Put images in 'images' directory and see the magic
import os
import cv2
import cvlib as cv
from cvlib.object_detection import draw_bbox
entries = os.listdir('images/')
for entry in entries:
    print(entry)
    im = cv2.imread('images/{}'.format(entry))
    bbox, label, conf = cv.detect_common_objects(im)
    output_image = draw_bbox(im, bbox, label, conf)
    cv2.imwrite(entry, output_image)

