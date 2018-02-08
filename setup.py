import os, zipfile

_script_path = os.path.realpath(__file__)

deps_pre_path = os.path.dirname(_script_path) + '/engine/dependencies/'
deps = {
    'boost':'builds.txt',
    'ffmpeg':'version.txt',
    'luaJIT':'version.txt',
    'msgpack':'COPYING',
    'nanomsg':'COPYING',
    'tbb':'version.txt',
    'vulkan':'SPIRVExtension.vsix',
}

print 'make wolf\'s dependencies ready'
for dep in deps:
    print 'uncompressing ' + dep
    _path_file = deps_pre_path + dep
    _z = zipfile.ZipFile(_path_file + '.zip')
    _z.extractall(deps_pre_path)
    _z.close()
    print 'verifying ' + dep
    if os.path.isfile(_path_file + '/' + deps[dep] ):
        print (dep + ' verified successfully')
    else: 
        print 'could not find \'' + deps_pre_path + '/' + dep + '/' + deps[dep] + '\''
print "exiting..."
quit()

