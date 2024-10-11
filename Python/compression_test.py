import numpy as np
import matplotlib.pyplot as plt
import imageio.v3 as iio


def ind2sub(sub,cols,rows):
   row=np.floor(sub/cols)
   col=sub%cols
   return int(col),int(row)

def rle_encode(data):
    encoded_data = []
    
    for i in np.arange(0, data.shape[0]):
        
        encoded_data.append(1)
        encoded_data.append((data[i,0]))
                
        

        for j in np.arange(1, data.shape[1]):
            
                
                

            if data[i,j]==encoded_data[-1] and encoded_data[-2]<255:
                #if same color, and count not more than 255, count is also encoded on 8bits, 255max
                encoded_data[-2] += 1
            else:
                encoded_data.append(1)
                encoded_data.append((data[i,j]))
        

    return encoded_data

def rle_decode(encoded_data,decoded_data):
    #decoded_data is a preallocated buffer with the right size

    decoded_index = 0
    
    for i in range(0,len(encoded_data),2):
        count = encoded_data[i]
        val = encoded_data[i+1]
        
        for j in range(count):
            decoded_data[decoded_index] = val
            decoded_index += 1

    return decoded_data

def RGB888ToRRTB332(rgb):
    #print(str( "{:08b}".format(rgb[0]) )+str( "  {:08b}".format(rgb[1]) )+str( "  {:08b}".format(rgb[2]) ))
    val = np.uint8( (rgb[0] & 0b11100000) | (rgb[1] & 0b11100000) >> 3 | (rgb[2] & 0b11000000) >> 6)
    #print(str( "{:08b}".format(val) ))
    return val

def RGB332ToRGB888(v332):
    rgb = np.array([0,0,0])
    #print(str( "{:08b}".format(v332) ))
    rgb[0] = (v332 & 0b11100000)
    rgb[1] = (v332 & 0b00011100) << 3
    rgb[2] = (v332 & 0b00000011) << 6
    return  rgb


def convertImage332To888(img):
    dataRGB888 = np.zeros( (height,width,3),dtype=np.uint8)
    for i in range(0,img.shape[0]):
        for j in range(0,img.shape[1]):
            rgb = RGB332ToRGB888(img[i,j])
            dataRGB888[i,j,0] = np.uint8(rgb[0])
            dataRGB888[i,j,1] = np.uint8(rgb[1])
            dataRGB888[i,j,2] = np.uint8(rgb[2])
    return dataRGB888


def convertImage888To332(img):
    
    dataRGB332 = np.zeros( (height,width),dtype=np.uint8) 
    for i in range(0,img.shape[0]):
        for j in range(0,img.shape[1]):
            rgb = tuple(img[i,j])
            v = RGB888ToRRTB332(rgb)
            dataRGB332[i,j] = v
    
    return dataRGB332

import cv2

def kmeans_color_quantization(image, k=8, rounds=1):
    data = image.reshape((-1, 3))                          
    data = np.float32(data)
    criteria =  (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 10000, 0.0001)    
    _, labels, centers = cv2.kmeans(data,k, None, criteria, rounds, cv2.KMEANS_RANDOM_CENTERS)

    centers = np.uint8(centers)
    res = centers[labels.flatten()]
    return res.reshape((image.shape))




path = ".\\python\\images\\"
dataRGB888 = iio.imread(path + 'face_hr_02-uni.png') 

dataRGB888 = kmeans_color_quantization(dataRGB888,16);

height = dataRGB888.shape[0]
width = dataRGB888.shape[1]

#compress color on 8bits
data = convertImage888To332(dataRGB888)

encoded_data = rle_encode(data)

decoded_data = [0] * data.shape[0]*data.shape[1]
decoded_data = rle_decode(encoded_data,decoded_data)
decoded_data = np.reshape(decoded_data,data.shape)
img = convertImage332To888(decoded_data)

print("compression :  ")
print("original: "+str(data.shape[0]*data.shape[1]))
print("compressed: "+str(len(encoded_data)))

fig = plt.figure
plt.imshow(img)
plt.show()




#write to file
fid = open(path+"img_rle.h","w")


nb_parts =1
fid.write("static const int img_height = "+str(height)+";\n")
fid.write("static const int img_width = "+str(width)+";\n")
fid.write("static const int nb_parts = "+str(nb_parts)+";\n")

stride = int(len(encoded_data)/nb_parts)
if stride%2 != 0:
    stride+=1

split_indexes = np.arange(0,len(encoded_data),stride)
split_indexes = np.concatenate((split_indexes,[len(encoded_data)]),axis=0)


for i in range(0,split_indexes.size-1):
    
    encoded_data_local = encoded_data[split_indexes[i]:split_indexes[i+1]]
    fid.write("static const int img_rle_"+str(i)+"_length = "+str(len(encoded_data_local))+";\n")
    fid.write("static const uint8_t img_rle_"+str(i)+"[] PROGMEM  = {")
    f_flag = True
    
    for d in encoded_data_local:
        if f_flag:
            f_flag = False
        else:
            fid.write(",")

        fid.write(str(d))
    fid.write("};\n")


fid.close()
