import subprocess
import numpy as np

OFF_PATH = '/home/meetshah1995/data/PrincetonShapeBenchmark/benchmark/db/'
CLA_FILE = '/home/meetshah1995/data/PrincetonShapeBenchmark/benchmark/classification/v1/base/test.cla'

def readClaFile(filename):
    lines = tuple(open(filename, 'r'))
    n_classses = int(lines[1].split()[0])
    n_models = int(lines[1].split()[1])
    class_dic = {}
    class_list = []
    count = 0
    class_count = 0
    for i in range(len(lines[3:])):
        split = lines[i].split()
        if lines[i] == '':
            pass
        elif len(split) == 3:
            if split[1] != '0' and split[2] != '0':
                count += int(split[2])
                class_dic['_'.join(split[0:1])] = [ len(class_list), int(lines[i].split()[2]) ]
                class_list.append(map(int, lines[i+1:i+1+int(split[2])]))
                class_count += 1
            elif split[1] == '0' and split[2] != '0':
                count += int(split[2])
                class_count += 1
                class_dic[split[0]] = [ len(class_list), int(lines[i].split()[2]) ]
                class_list.append(map(int, lines[i+1:i+1+int(split[2])]))
    return class_dic, class_list

def getShapeDescriptor():
    shape_desc_list = []
    d, l = readClaFile(CLA_FILE)
    for i in range(len(l)):
        for j in range(len(l[i])):
            name = str(l[i][j])
            file_path = OFF_PATH + str(l[i][j] / 100) + '/m' + name + '/m' + name + '.off' 
            result = subprocess.check_output(["./meshdesc", file_path, "vol2bbox"])
            print file_path, 'Vol2BBOXRatio is:', float(result.split('\n')[3])
            shape_desc_list.append([i, l[i][j], float(result.split('\n')[3])])
    np.savetxt("shape_desc.csv", shape_desc_list, delimiter=",", fmt='%s')

if __name__ == '__main__':
    result = subprocess.check_output(["./meshdesc", "./dat/m61.off", "vol2bbox"])
    print 'Vol2BBOXRatio is:', float(result.split('\n')[3])
    getShapeDescriptor()