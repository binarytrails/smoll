#!/usr/bin/python3
# https://gist.github.com/higarmi/6708779#gistcomment-1703164

def flatten(d,result=None,index=None,Key=None):
    if result is None:
        result = {}
    if isinstance(d, (list, tuple)):
        for indexB, element in enumerate(d):
            if Key is not None:
                newkey = Key
            flatten(element,result,index=indexB,Key=newkey)            
    elif isinstance(d, dict):        
        for key in d:
            value = d[key]         
            if Key is not None and index is not None:
                newkey = "_".join([Key,(str(key).replace(" ", "") + str(index))])
            elif Key is not None:
                newkey = "_".join([Key,(str(key).replace(" ", ""))])
            else:
                newkey= str(key).replace(" ", "")
            flatten(value,result,index=None,Key=newkey)        
    else:
        result[Key]=d        
    return result

if (__name__ == '__main__'):
    print(flatten({1:{2:{3:{4}}}}))
