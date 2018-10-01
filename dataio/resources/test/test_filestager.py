#!/usr/bin/env python

import sys
if sys.version_info[:2] < (2,6):
    sys.exit(0)

from icecube import dataio
from icecube.dataio.I3FileStagerFile import I3FileStagerFile

from icecube import icetray
import os
# icetray.logging.I3Logger.global_logger = icetray.I3NullLogger()
icetray.logging.set_level('TRACE')

dataio.set_local_scratch_dir('.')

def test_scratchdir(url=os.path.expandvars("file://$I3_BUILD/env-shell.sh")):
    stager = I3FileStagerFile()
    # directory is created lazily
    scratch_dir = stager.scratch_dir
    assert(scratch_dir is None)
    # now it should exist
    stager.GetReadablePath(url)
    scratch_dir = stager.scratch_dir
    assert(os.path.isdir(scratch_dir))
    del stager
    # now it should be gone
    assert(not os.path.isdir(scratch_dir))

def _test_stage(url, minsize=100):
    import ssl
    if 'https' in url:
        # TODO: Make the actual cacert work.
        #       It just seems to hang when using this line.
        #stager = I3FileStagerFile(ssl={'cafile':'cacert.pem','capath':os.getcwd()})
        try:
            # accept self-signed certs
            stager = I3FileStagerFile(ssl={'context':ssl._create_unverified_context()})
        except AttributeError:
            stager = I3FileStagerFile()
    else:
        stager = I3FileStagerFile()
    local_fname = stager.GetReadablePath(url)
    assert(os.path.exists(str(local_fname)))
    assert(os.stat(str(local_fname)).st_size > minsize)
    local_fname = str(local_fname)
    # check that staged files are really deleted
    if stager.CanStageIn(url):
        assert(not os.path.exists(str(local_fname)))

def _make_http(port=None,usessl=False,basic_auth=False):

    try:
        from BaseHTTPServer import BaseHTTPRequestHandler,HTTPServer
    except ImportError:
        from http.server import BaseHTTPRequestHandler,HTTPServer
    import random
    import ssl
    import threading
    import subprocess
    
    data = b''.join([b'test' for _ in range(1000)])
    
    if basic_auth:
        class Handle(BaseHTTPRequestHandler):
            def do_HEAD(self):
                self.send_response(200)
                self.send_header('Content-type','text')
                self.end_headers()
            def do_AUTHHEAD(self):
                self.send_response(401)
                self.send_header('WWW-Authenticate', 'Basic realm=\"Test\"')
                self.send_header('Content-type','text')
                self.end_headers()
            def do_GET(self):
                if self.headers['Authorization'] == None:
                    self.do_AUTHHEAD()
                    self.wfile.write('no auth header received')
                elif self.headers['Authorization'] == 'Basic dGVzdDp0ZXN0':
                    self.do_HEAD()
                    self.wfile.write(bytearray(self.headers['Authorization'],'ascii'))
                    self.wfile.write(data)
                else:
                    self.do_AUTHHEAD()
                    self.wfile.write(self.headers['Authorization'])
                    self.wfile.write('not authenticated')
    else:
        class Handle(BaseHTTPRequestHandler):
            def do_HEAD(self):
                self.send_response(200)
                self.send_header('Content-type','text')
                self.end_headers()
            def do_GET(self):
                self.do_HEAD()
                self.wfile.write(data)
    
    if not port:
        while True:
            try:
                port = random.randint(10000,50000)
                httpd = HTTPServer(('localhost', port), Handle)
            except socket.error:
                continue
            break
    else:
        httpd = HTTPServer(('localhost', port), Handle)
    if usessl:
        print('ssl')
        p = subprocess.Popen(['openssl','req','-new','-x509',
                              '-keyout','privkey.pem',
                              '-out','cacert.pem','-days','1',
                              '-batch','-passout','pass:passkey',
                              '-subj', '/'],
                              stdin=subprocess.PIPE)
        p.communicate(input=b'passkey')
        if p.returncode:
            raise Exception('cannot generate self-signed cert')
        try:
            if subprocess.call(['openssl','rsa','-in','privkey.pem','-out','key.pem','-passin','pass:passkey']):
                raise Exception('error removing password from key.pem')
            open('key.pem','a').write(open('cacert.pem').read())
            httpd.socket = ssl.wrap_socket(httpd.socket, certfile='key.pem',
                                           server_side=True)
        except Exception:
            os.remove('privkey.pem')
            os.remove('cacert.pem')
            if os.path.exists('key.pem'):
                os.remove('key.pem')
            raise

    t = threading.Thread(target=httpd.serve_forever)
    t.start()
    def shutdown():
        httpd.shutdown()
        t.join()
        if os.path.exists('privkey.pem'):
            os.remove('privkey.pem')
        if os.path.exists('key.pem'):
            os.remove('key.pem')
        if os.path.exists('cacert.pem'):
            os.remove('cacert.pem')
    return port,shutdown


def test_double_stage():
    """
    Stager should download files only once and return a handle to the existing
    file if it's still live
    """
    port,stop = _make_http()
    url = "http://localhost:%d"%port
    try:
        stager = I3FileStagerFile()
        f1 = stager.GetReadablePath(url)
        f2 = stager.GetReadablePath(url)
        assert(str(f1) == str(f2))
        assert(os.path.exists(str(f1)))
        assert(os.path.exists(str(f2)))
        del f1
        assert(os.path.exists(str(f2)))
        f2path = str(f2)
        del f2
        assert(not os.path.exists(f2path))
    finally:
        stop()

def test_http():
    port,stop = _make_http()
    address = "http://localhost:%d"%port
    try:
        _test_stage(address)
    finally:
        stop()

def test_http_with_auth():
    port,stop = _make_http(basic_auth=True)
    address = "http://test:test@localhost:%d"%port
    try:
        _test_stage(address)
    finally:
        stop()

def test_https():
    port,stop = _make_http(usessl=True)
    address = "https://localhost:%d"%port
    try:
        _test_stage(address)
    finally:
        stop()

def test_https_with_auth():
    port,stop = _make_http(usessl=True,basic_auth=True)
    address = "https://test:test@localhost:%d"%port
    try:
        _test_stage(address)
    finally:
        stop()

def test_file():
    _test_stage(os.path.expandvars("file://$I3_BUILD/env-shell.sh"))

def test_file_with_relative_path():
    relative = os.path.relpath(os.path.expandvars('$I3_BUILD/env-shell.sh'))
    try:
        _test_stage('file://'+relative)
        raise AssertionError("An error should have been raised")
    except RuntimeError:
        pass
    
def test_file_implicit():
    _test_stage(os.path.expandvars("$I3_BUILD/env-shell.sh"))

if __name__ == "__main__":
    for k, v in list(locals().items()):
        if k.startswith("test") and hasattr(v, "__call__"):
            v()
