# Import Caffe Model to tiny-dnn
tiny-dnn can import Caffe's trained models.

## Prerequisites for this example
- Google protobuf
- OpenCV

## Build
```bash
cmake .
make
```

## Usage
```bash
./get_tiny_dnn_model_from_caffe *.prototxt *.caffemodel 
```

In the [pre-trained LeNet](https://github.com/BVLC/caffe/tree/master/examples/mnist) model,
```
./get_tiny_dnn_model\
${CAFFE_ROOT}/examples/mnist/lenet.prototxt\
${CAFFE_ROOT}/git/caffe/examples/mnist/lenet_iter_10000.caffemodel
```

The tiny-dnn model will be stored in `caffe_model` file.

## Restrictions
- tiny-dnn's converter only supports single input/single output network without branch.
