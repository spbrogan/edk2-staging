## @ WinLcovConvertFileName.py
#
# Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import re
import sys
import time
import shutil

def ConvertFile(covFile, covFileNew):
    f = open(covFile, 'r')
    fn = open(covFileNew, 'wb')
    lines = f.readlines ()
    for line in lines:
        line.strip()
        if cmp (line[0:3], "SF:") != 0:
            fn.write(line)
            continue
        newline = 'SF:/' + line[3]
        for i in line[5:]:
            if i == '\\':
                newline += '/'
            else:      
                newline += i
        fn.write(newline)
    f.close()
    fn.close()

def usage():
    print("Usage:")
    print("\tWinLcovConvertFileName <coverage.info file> <new coverage.info file>")

def main():

    if len(sys.argv) < 3:
        usage()
        return 1

    ConvertFile (sys.argv[1], sys.argv[2])

if __name__ == '__main__':
    sys.exit(main())
