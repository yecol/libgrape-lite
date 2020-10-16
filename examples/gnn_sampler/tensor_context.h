/** Copyright 2020 Alibaba Group Holding Limited.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef EXAMPLES_GNN_SAMPLER_TENSOR_CONTEXT_H_
#define EXAMPLES_GNN_SAMPLER_TENSOR_CONTEXT_H_
#include "arrow/tensor.h"
#include "grape/app/context_base.h"

#define CONTEXT_TYPE_TENSOR "tensor"

namespace grape {

template <typename FRAG_T, typename DATA_T>
class TensorContext : public ContextBase {
  using fragment_t = FRAG_T;
  using vertex_t = typename fragment_t::vertex_t;

 public:
  using data_t = DATA_T;
  using arrow_tensor_t =
      arrow::NumericTensor<typename arrow::CTypeTraits<data_t>::ArrowType>;

  explicit TensorContext(const fragment_t& fragment) : fragment_(fragment) {
    if (std::is_same<data_t, int32_t>()) {
      type_ = arrow::int32();
    } else if (std::is_same<data_t, uint32_t>()) {
      type_ = arrow::uint32();
    } else if (std::is_same<data_t, int64_t>()) {
      type_ = arrow::int64();
    } else if (std::is_same<data_t, uint64_t>()) {
      type_ = arrow::uint64();
    } else if (std::is_same<data_t, float_t>()) {
      type_ = arrow::float32();
    } else if (std::is_same<data_t, double_t>()) {
      type_ = arrow::float64();
    } else {
      throw std::runtime_error("Unsupported datatype: " +
                               std::string(typeid(data_t).name()));
    }
  }

  const fragment_t& fragment() { return fragment_; }

  const char* context_type() const override { return CONTEXT_TYPE_TENSOR; }

  std::shared_ptr<arrow_tensor_t> tensor() {
    return std::dynamic_pointer_cast<arrow_tensor_t>(tensor_);
  }

  std::vector<data_t>& data() { return data_; }

  arrow::Status set_shape(const std::vector<int64_t>& shape) {
    if (!shape.empty()) {
      int size = 1;

      for (auto dim_size : shape) {
        size *= dim_size;
      }
      data_.reserve(size);
      shape_ = shape;
      auto buffer = arrow::Buffer::Wrap(data_);
      ARROW_ASSIGN_OR_RAISE(tensor_,
                            arrow::Tensor::Make(type_, buffer, shape_));
      return arrow::Status();
    }
    return arrow::Status(arrow::StatusCode::Invalid, "Empty shape");
  }

  const std::vector<int64_t>& shape() { return shape_; }

 private:
  const fragment_t& fragment_;
  std::vector<data_t> data_;
  std::shared_ptr<arrow::DataType> type_;
  std::shared_ptr<arrow::Tensor> tensor_;
  std::vector<int64_t> shape_;
};

}  // namespace grape

#endif  // EXAMPLES_GNN_SAMPLER_TENSOR_CONTEXT_H_
