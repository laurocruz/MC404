import os

files = os.listdir("../casos-de-teste/corretos/")

files = sorted(files)

os.system("make")

for i in range(0, len(files)-3, 2):
    inp = "../casos-de-teste/corretos/" + files[i]
    res = "../casos-de-teste/corretos/" + files[i+1]
    out = "../casos-de-teste/corretos/out.txt"

    print("---------------" + files[i] + " " + files[i+1] + "------------------")
    try :
        os.remove(out)
    except :
        pass
    os.system("./ias-as " + inp + " " + out)
    print("DIFF:")
    os.system("diff " + res + " " + out)
    print("--------------------------------------------------------------------")
    print(" ")
    print(" ")

try :
    os.remove(out)
except :
    pass

files = os.listdir("../casos-de-teste/errados/")

files = sorted(files)

for f in files:
    inp = "../casos-de-teste/errados/" + f

    print("-----------------------" + f + "---------------------------")

    os.system("./ias-as " + inp)

    print("----------------------------------------------------------------------")

os.system("make clean")
