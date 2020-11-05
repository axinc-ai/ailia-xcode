# ailia-xcode

Project sample of ailia SDK for xcode

## Build environment

Xcode 11.3

## Setup

Download u2net_opset11.onnx in ./u2net folder.

```
wget https://storage.googleapis.com/ailia-models/u2net/u2net_opset11.onnx
```

Put ailia-1.2.4-xc7.a to ./u2net folder.

Expected Layout

```
./u2net.xcodeproj
./u2net/ailia-1.2.4-xc7.a
./u2net/u2net_opset11.onnx
./u2net/u2net_opset11.prototxt
```

## Function

Perform background cropping on the input image using u2net.

## Implementation Detail

ailia SDK was implemented in [ViewController.mm](./u2net/ViewController.mm).
