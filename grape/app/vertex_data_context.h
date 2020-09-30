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

#ifndef GRAPE_APP_VERTEX_DATA_CONTEXT_H_
#define GRAPE_APP_VERTEX_DATA_CONTEXT_H_
#include "grape/app/context_base.h"
#include "grape/utils/vertex_array.h"
#define CONTEXT_TYPE_VERTEX_DATA "vertex_data"

namespace grape {

template <typename FRAG_T, typename DATA_T>
class VertexDataContext : public ContextBase {
  using fragment_t = FRAG_T;
  using vertex_t = typename fragment_t::vertex_t;
  using vid_t = typename fragment_t::vid_t;
  using oid_t = typename fragment_t::oid_t;
  using data_t = DATA_T;
  using vertex_array_t = typename fragment_t::template vertex_array_t<data_t>;

 public:
  void set_fragment(std::shared_ptr<fragment_t> &fragment) {
    fragment_ = fragment;
    data_.Init(fragment->InnerVertices());
  }

  const fragment_t& fragment() {
    return *fragment_;
  }

  std::string context_type() const override { return CONTEXT_TYPE_VERTEX_DATA; }

  void SetValue(const data_t& data) { data_.SetValue(data); }

  void SetValue(vertex_t v, const data_t& data) { data_[v] = data; }

  const data_t& GetValue(vertex_t v) const { return data_[v]; }

  vertex_array_t& data() { return data_; }

 private:
  std::shared_ptr<fragment_t> fragment_;
  vertex_array_t data_;
};

}  // namespace grape

#endif  // GRAPE_APP_VERTEX_DATA_CONTEXT_H_
