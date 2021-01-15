import passpy

def show(e):
    key = store.get_key(e).split('\n', 1)[0]
    print(e + '\t' + key)

def sub(d):
    (dirs, entries) = store.list_dir(d)
    for _d in dirs:
        sub(_d)
    for e in entries:
        show(e)

store = passpy.Store()
sub('.')

