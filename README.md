# Skin Whitening in OpenCV and Qt
**This program does this in 3 steps...**
* Detects faces using Haar Cascade and creates mask.
* Apply a Bilateral Filter (blur) to remove skin Irregularities.
* Whiten skin by changing saturation and luminance.

## Build
**Build Dependencies...**
* libqt4-dev
* libopencv-core-dev
* libopencv-imgproc-dev
* libopencv-objdetect-dev

**To build...**
Open terminal and change to source directory.
Run...
`qmake`
`make -j4`


