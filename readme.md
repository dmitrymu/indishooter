# INDI Shooting Utility

INDI client to capture images on a simple periodic schedule.

The whole run of the program is a session.  The session is a sequence
of batches executed with certain interval. A batch is a number of
frames shot consecutively.

Frames are being saved as FITS files to the output directory as they shot.  
The directory is created if not exist, otherwise it must be empty.
Files are given unique names that include batch number, frame number 
inside a batch, CCD exposure and gain, e.g. `frame_e1_5_g100_001_002`


```
>indishooter -h
indishooter version 0.0.1
Utility to shoot astrophotos in batches using INDI service
Available options::

General:
  -h [ --help ]                   print help message
  -v [ --version ]                print program version
  -l [ --list ]                   list devices instead of shooting

INDI service:
  -H [ --host ] arg (=localhost)  INDI host
  -P [ --port ] arg (=7624)       INDI port

CCD/Camera:
  -n [ --name ] arg               camera name
  -x [ --exposure ] arg (=1)      exposure in seconds
  -g [ --gain ] arg (=0)          gain
  -o [ --offset ] arg (=10)       offset

Batching:
  -b [ --batch ] arg (=1)         number of frames in the batch
  -i [ --interval ] arg (=0)      interval between batches in seconds
  -c [ --count ] arg (=1)         number of batches

File saving:
  -d [ --dir ] arg (=light)       output directory for FITS files
  -t [ --tag ] arg (=frame)       tag to start each file name

```