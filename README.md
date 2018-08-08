# corrosion
A New and Cheap Methodology to Study Tridimensionally Pitting Corrosion in Stainless Steel

Volumetric pitting corrosion software guide


1 INSTALLATION

Download

```
$ git clone https://github.com/ocuadrosl/corrosion
```


Install
```
$ cd corrosion
$ mkdir build
$ cd build
$ cmake ..
$ make
```


Update

```
$ cd corrosion
$ git pull
$ cd build
$ make
```


Execute

```
$ cd corrosion
$ ./build/buildCorrosionAxial <inputFilePath>
```


Requirements

```
1. C++17
2. Insight Segmentation and Registration Toolkit (ITK) 
3. Linux kernel 4.13 or superior

```

2 CONFIGURATION FILE

Defaul values



```
fixedImage ""
conversionFactor 1
imageFormat tif
inputDir .
outputDir .
testName corrosion
breakPoint -1
reducedRadius -1
statistics3D 1
statistics2D 1
abradedHeightByLayer
```
Symbols meaning
```
-1 = false/not/null/
. = local directory
"" = null string
```

Example 
```
fixedImage input/phantom/28.png
conversionFactor 1866.876435473
imageFormat png
inputDir input/eletrodo5_Imagens_tratadas
outputDir output
testName electrodo5Tratadas
breakPoint 18
reducedRadius 890
statistics3D 0
statistics2D 0
abradedHeightByLayer
0.000
...
...
0.008
```




