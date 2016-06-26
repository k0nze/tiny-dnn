/*
    Copyright (c) 2016, Taiga Nomi, Edgar Riba
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include "tiny_cnn/core/params/conv_params.h"

namespace tiny_cnn {
namespace core {
namespace kernels {

void tiny_conv2d_back_kernel(const conv_params& params,
                             const vec_t& prev_out,
                             const vec_t& W,
                             vec_t&       dW,
                             vec_t&       db,
                             vec_t&       curr_delta,
                             vec_t*       prev_delta) {
    // propagate delta to previous layer
    for_i(params.in.depth_, [&](int inc) {
        for (cnn_size_t outc = 0; outc < params.out.depth_; outc++) {
            if (!params.tbl.is_connected(outc, inc)) continue;

            cnn_size_t idx = 0;
            idx = params.in.depth_ * outc + inc;
            idx = params.weight.get_index(0, 0, idx);
            const float_t *pw = &W[idx];

            idx = params.out.get_index(0, 0, outc);
            const float_t *pdelta_src = &curr_delta[idx];

            idx = params.in_padded.get_index(0, 0, inc);
            float_t *pdelta_dst = &(*prev_delta)[idx];

            for (cnn_size_t y = 0; y < params.out.height_; y++) {
                for (cnn_size_t x = 0; x < params.out.width_; x++) {
                    const float_t * ppw = pw;

                    idx = y * params.out.width_ + x;
                    const float_t ppdelta_src = pdelta_src[idx];

                    float_t * ppdelta_dst = pdelta_dst +
                          y * params.h_stride * params.in_padded.width_ +
                          x * params.w_stride;

                    for (cnn_size_t wy = 0; wy < params.weight.height_; wy++) {    // NOLINT
                        for (cnn_size_t wx = 0; wx < params.weight.width_; wx++) { // NOLINT
                            idx = wy * params.in_padded.width_ + wx;
                            ppdelta_dst[idx] += *ppw++ * ppdelta_src;
                        }
                    }
                }
            }
        }
    });

    // accumulate dw
    for_i(params.in.depth_, [&](int inc) {
        for (cnn_size_t outc = 0; outc < params.out.depth_; outc++) {
            if (!params.tbl.is_connected(outc, inc)) continue;

            for (cnn_size_t wy = 0; wy < params.weight.height_; wy++) {
                for (cnn_size_t wx = 0; wx < params.weight.width_; wx++) {
                    float_t dst = float_t(0);

                    cnn_size_t idx = 0;
                    idx = params.in_padded.get_index(wx, wy, inc);
                    const float_t * prevo = &prev_out[idx];

                    idx = params.out.get_index(0, 0, outc);
                    const float_t * delta = &curr_delta[idx];

                    for (cnn_size_t y = 0; y < params.out.height_; y++) {
                        dst += vectorize::dot(
                            prevo + y * params.in_padded.width_,
                            delta + y * params.out.width_,
                            params.out.width_);
                    }

                    idx = params.in.depth_ * outc + inc;
                    dW[params.weight.get_index(wx, wy, idx)] += dst;
                }
            }
        }
    });

    // accumulate db
    if (params.has_bias) {
        //vec_t& db = *in_grad[2];

        for (cnn_size_t outc = 0; outc < params.out.depth_; outc++) {
            cnn_size_t idx = params.out.get_index(0, 0, outc);
            const float_t * delta = &curr_delta[idx];
            const float_t * deltaa = delta + params.out.width_ *
                                             params.out.height_;
            db[outc] += std::accumulate(delta, deltaa, float_t(0));
        }
    }
}

}  // namespace kernels
}  // namespace core
}  // namespace tiny_cnn