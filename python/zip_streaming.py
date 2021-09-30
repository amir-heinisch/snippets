#!/usr/bin/python3.9

# SPDX-License-Identifier: MIT

#
# Streaming zip files via http with naive block cache.
#

__author__ = "Amir Heinisch <mail@amir-heinisch.de>"

import os, re
from urllib.request import Request, urlopen


class URLCache:

    # Cache block size.
    CBLKSIZE = 10 * 1024 * 1024

    def __init__(self, url):
        self.url = url
        self.cursor = 0

        # Get filesize from webserver.
        with urlopen(Request(url, method="HEAD")) as res:
            if res.status == 200:
                self.size = int(res.getheader("Content-Length"))

        # Prepare block cache.
        rest = 0 if self.size % self.CBLKSIZE == 0 else 1
        self.cache = [ None ] * ( self.size // self.CBLKSIZE + rest)

    def seekable(self):
        return True

    def get_size(self):
        return self.size

    def download_range(self, start, end):
        print("Download: (", start, ",", end, ")")

        headers = {
            # TODO: multiple ranges in one request are possible.
            "Range": f"bytes={start}-{end}"
        }

        req = Request(url, headers=headers)
        with urlopen(req) as res:
            return res.read()

    def seek(self, offset, whence=os.SEEK_SET):
        if whence == os.SEEK_SET:
            self.cursor = offset
        elif whence == os.SEEK_CUR:
            self.cursor += offset
        elif whence == os.SEEK_END:
            self.cursor = self.get_size() + offset
        else:
            pass

    def tell(self):
        return self.cursor

    def read(self, count=None):
        if count == None:
            count = self.get_size() - self.cursor

        if (self.cursor + count - 1) >= self.get_size() or count == 0:
            return b""

        sb = self.cursor // self.CBLKSIZE
        eb = (self.cursor + count) // self.CBLKSIZE

        data = []
        for b_idx in range(sb, eb+1):
            blk = b""
            if self.cache[b_idx] == None:
                blk = self.download_range(b_idx*self.CBLKSIZE, b_idx*self.CBLKSIZE+self.CBLKSIZE-1)
                self.cache[b_idx] = blk
            else:
                blk = self.cache[b_idx]
            data.append(blk)

        tr = self.cursor - (sb * self.CBLKSIZE)
        data[0] = data[0][tr:]
        data = b"".join(data)[:count]

        assert len(data) == count
        self.cursor += len(data)

        return data

if __name__ == '__main__':
    import sys, zipfile

    # Get input: filename or url.
    inp = sys.argv[1]

    zf = None

    if inp.startswith("http"):
        url = inp

        # Start local http server in background thread.
        from http.server import HTTPServer
        from threading import Thread

        # Std implementation doesn't implement range header see here:
        # - https://github.com/danvk/RangeHTTPServer/blob/master/RangeHTTPServer/__init__.py
        import RangeHTTPRequestHandler

        httpd = HTTPServer(('127.0.0.1', 8000), RangeHTTPRequestHandler)
        t = Thread(target=httpd.serve_forever, daemon=True)
        t.start()

        zf = zipfile.ZipFile(URLCache(url))
    else:
        filename = inp
        if not zipfile.is_zipfile(filename):
            print("No valid zip given.")
            sys.exit()
        zf = zipfile.ZipFile(filename)

    print(zf.namelist())

    with zf.open("test.txt") as f:
        print(f.read())

    zf.close()

