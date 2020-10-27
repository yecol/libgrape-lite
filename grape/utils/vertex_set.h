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

#ifndef GRAPE_UTILS_VERTEX_SET_H_
#define GRAPE_UTILS_VERTEX_SET_H_

#include <utility>

#include "grape/utils/bitset.h"
#include "grape/utils/vertex_array.h"

namespace grape {

/**
 * @brief A vertex set with dense vertices.
 *
 * @tparam vid_t Vertex ID type.
 */
template <typename VERTICES_T>
class DenseVertexSet {
  using vid_t = typename VERTICES_T::vid_t;

 public:
  DenseVertexSet() = default;

  ~DenseVertexSet() = default;

  void Init(const VERTICES_T& vertices, int thread_num = 1) {
    auto size = vertices.size();

    range_ = vertices;
    if (size == 0) {
      beg_ = 0;
    } else {
      beg_ = vertices[0].GetValue();
    }

    bs_.init(size);
    if (thread_num == 1) {
      bs_.clear();
    } else {
      bs_.parallel_clear(thread_num);
    }
  }

  void Insert(Vertex<vid_t> u) { bs_.set_bit(u.GetValue() - beg_); }

  bool InsertWithRet(Vertex<vid_t> u) {
    return bs_.set_bit_with_ret(u.GetValue() - beg_);
  }

  void Erase(Vertex<vid_t> u) { bs_.reset_bit(u.GetValue() - beg_); }

  bool EraseWithRet(Vertex<vid_t> u) {
    return bs_.reset_bit_with_ret(u.GetValue() - beg_);
  }

  bool Exist(Vertex<vid_t> u) const { return bs_.get_bit(u.GetValue() - beg_); }

  const VERTICES_T& Vertices() const { return range_; }

  size_t Count() const { return bs_.count(); }

  size_t ParallelCount(int thread_num) const {
    return bs_.parallel_count(thread_num);
  }

  size_t PartialCount(vid_t beg, vid_t end) const {
    return bs_.partial_count(beg - beg_, end - beg_);
  }

  size_t ParallelPartialCount(int thread_num, vid_t beg, vid_t end) const {
    return bs_.parallel_partial_count(thread_num, beg - beg_, end - beg_);
  }

  void Clear() { bs_.clear(); }

  void ParallelClear(int thread_num) { bs_.parallel_clear(thread_num); }

  void Swap(DenseVertexSet<VERTICES_T>& rhs) {
    std::swap(beg_, rhs.beg_);
    range_.Swap(rhs.range_);
    bs_.swap(rhs.bs_);
  }

  Bitset& GetBitset() { return bs_; }

  const Bitset& GetBitset() const { return bs_; }

  bool Empty() const { return bs_.empty(); }

  bool PartialEmpty(vid_t beg, vid_t end) const {
    return bs_.partial_empty(beg - beg_, end - beg_);
  }

 private:
  vid_t beg_;
  VERTICES_T range_;
  Bitset bs_;
};

}  // namespace grape

#endif  // GRAPE_UTILS_VERTEX_SET_H_
