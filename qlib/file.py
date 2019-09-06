import os
import const
import random


def checkIfDirIsWriteable(path):
    filepath = path + '/' + str(random.randint(1000, 9999))
    try:
        print 'Checking writeability ' + path + '',
        with open(filepath, 'w') as f:
            # file opened for writing. write to it here
            print ' - ok'
            os.remove(filepath)
            pass
    except IOError as x:
        print 'error ', x.errno, ',', x.strerror
        if x.errno == errno.EACCES:
            print filepath, 'no permissions'
        elif x.errno == errno.EISDIR:
            print filepath, 'is directory'
        raise Exception('Insufficient Permissions for writing files')

#
# This function checks if the tmp dir and the upload dir
# are both writeable.
#


def checkPermissions():
    # Local folder, used for temp files.
    checkIfDirIsWriteable(const.TMP_DIR)

    # Samba share folder automounted.
    checkIfDirIsWriteable(const.UPLOAD_DIR)
