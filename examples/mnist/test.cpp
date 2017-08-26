/*
    Copyright (c) 2013, Taiga Nomi and the respective contributors
    All rights reserved.

    Use of this source code is governed by a BSD-style license that can be found
    in the LICENSE file.
*/
#include <iostream>
#include <stdint.h>
#include "tiny_dnn/tiny_dnn.h"

using namespace tiny_dnn;
using namespace tiny_dnn::activation;
using namespace std;

// rescale output to 0-100
template <typename Activation>
double rescale(double x) {
  Activation a(1);
  return 100.0 * (x - a.scale().first) / (a.scale().second - a.scale().first);
}


bool has_suffix(const std::string &str, const std::string &suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void recognize(const std::string &dictionary, const std::string &src_filename) {
  network<sequential> nn;

  nn.load(dictionary);

  vec_t data;
  float* image;
  uint16_t height;
  uint16_t width;

  // read image from pgm or float file
  if(has_suffix(src_filename, ".pgm")) {
    cout << "pgm file\n";
    read_pgm(&image, src_filename.c_str(), &height, &width, 0);
  } else if(has_suffix(src_filename, ".float")) {
    cout << "float file\n";
    read_float(&image, src_filename.c_str(), &height, &width);
  }

  for(int i = 0; i < (height*width); i++) {
      data.push_back(image[i]);
  }

  
  float* input;
  input = (float*) malloc(1024*sizeof(float));


  for(int i = 0; i < height*width; i++) {
      input[i] = data.at(i);
  }

  write_pgm(input, height, width, "input.pgm");
  write_float(input, height, width, "input.float");

  free(input);

  // recognize
  auto res = nn.predict(data);
  vector<pair<double, int>> scores;

  // sort & print top-3
  for (int i = 0; i < 10; i++)
    scores.emplace_back(rescale<tanh_layer>(res[i]), i);

  sort(scores.begin(), scores.end(), greater<pair<double, int>>());

  for (int i = 0; i < 3; i++)
    cout << scores[i].second << "," << scores[i].first << endl;

  // save outputs of each layer
  /*
  for (size_t i = 0; i < nn.depth(); i++) {
    auto out_img  = nn[i]->output_to_image();
    auto filename = "layer_" + std::to_string(i) + ".png";
    out_img.save(filename);
  }
  // save filter shape of first convolutional layer
  {
    auto weight   = nn.at<convolutional_layer>(0).weight_to_image();
    auto filename = "weights.png";
    weight.save(filename);
  }
  */

  for (size_t i = 0; i < nn.depth(); i++) {
     //printf("layer %zu:\n", i);
     vec_t* output= nn[i]->output_to_vec();
     std::vector<shape3d> out_shape = nn[i]->out_shape();

     float* image;
     image = (float*) malloc(out_shape[0].depth_*out_shape[0].height_*out_shape[0].width_*sizeof(float));

     for(int depth = 0; depth < out_shape[0].depth_; depth++) {
         for(int row = 0; row < out_shape[0].height_; row++) {
             for(int column = 0; column < out_shape[0].width_; column++) {
                 image[depth*out_shape[0].height_*out_shape[0].width_ + row*out_shape[0].width_ + column] = (float) output->at(depth*out_shape[0].height_*out_shape[0].width_ + row*out_shape[0].width_ + column);
             }
         }
     }
     string filename;

     filename = "layer_" + std::to_string(i) + ".pgm";
     write_pgm(image, out_shape[0].depth_*out_shape[0].height_, out_shape[0].width_, filename.c_str());

     filename = "layer_" + std::to_string(i) + ".float";
     write_float(image, out_shape[0].depth_*out_shape[0].height_, out_shape[0].width_, filename.c_str());
     free(image);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << "please specify image file" << std::endl;
    return 0;
  }
  recognize("LeNet-model", argv[1]);
}
