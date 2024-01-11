import struct

def recall(a,b):
    return  1.0*len(set(a).intersection(set(b)))/len(a)

def isFloat(x):
    try:
        float(x)
        return True
    except:
        return False

def read_bin(file_name,k):
    knn_exact = []
    with open(file_name, 'rb') as file:
        res = []
        while True:
            binary_data = file.read(4)
            if not binary_data:
                break
            integer_value = struct.unpack('I', binary_data)[0]
            res.append(integer_value)
            if len(res) == k:
                knn_exact.append(res)
                res = []
    return knn_exact

if __name__ == '__main__':
    read_bin(10)