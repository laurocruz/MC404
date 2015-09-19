import os

files = os.listdir("./casos-de-teste/corretos/")

files = sorted(files)

for i in range(0, len(files)-3, 2):
    inp = "casos-de-teste/corretos/" + files[i]
    res = "casos-de-teste/corretos/" + files[i+1]
    out = "casos-de-teste/corretos/out.txt"

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
