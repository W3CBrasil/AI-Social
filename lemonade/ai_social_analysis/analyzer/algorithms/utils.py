import inspect

def function_defaults(f):

    argspec = inspect.getargspec(f)
    
    values = argspec[3]
    if values:
        names = argspec[0][-len(values):]
    else:
        values = []
        names = []

    return dict(zip(names, values))

