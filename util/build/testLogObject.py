import ctypes as ct
lib = ct.cdll.LoadLibrary('./libutil.so')

class LogObject(object):
    def __init__(self):
        self.obj = lib.LogObject_new()

    def addFloatItem(self, ingredient):
        lib._addLogObjectFloat(self.obj, ct.c_float(ingredient))
    def addIntItem(self, ingredient):   
        lib._addLogObjectInt(self.obj, ingredient)
    def getLogData(self, index):
        return lib.getLogData(self.obj, index)
       
class Logger(object):
    def __init__(self):
        self.obj = lib.new_Logger()

    def addIntergerDataField(self, label):
        lib.addIntegerDataField(self.obj, label)
    def addFloatDataField(self, label):
        lib.addFloatDataField(self.obj, label)

    def printLogData(self, logData):
        out = ct.c_char_p(lib.printLogData(self.obj, logData.obj))
        return out.value
    
if __name__ == "__main__":
    logger = Logger()
    logger.addIntergerDataField("bacons")
    logger.addIntergerDataField("eggs")
    logger.addIntergerDataField("toasts")
    
    foo = LogObject()
    foo.addIntItem(3)
    foo.addIntItem(30)
    foo.addFloatItem(2.3)
    
    s = logger.printLogData(foo)
    print s
    # foo.printIngredients()
    
