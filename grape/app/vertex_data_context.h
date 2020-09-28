#ifndef GRAPE_APP_VERTEX_DATA_CONTEXT_H_
#define GRAPE_APP_VERTEX_DATA_CONTEXT_H_
#include "grape/grape.h"
#define CONTEXT_TYPE_VERTEX_DATA "vertex_data"
namespace grape {

template <typename FRAG_T, typename DATA_T>
class VertexDataContext : public ContextBase<FRAG_T> {
  using fragment_t = FRAG_T;
  using vertex_t = typename fragment_t::vertex_t;
  using vid_t = typename fragment_t::vid_t;
  using oid_t = typename fragment_t::oid_t;

 public:
  void set_fragment(std::shared_ptr<FRAG_T> &fragment) {
    fragment_ = fragment;
    data_.Init(fragment->InnerVertices());
  }

  const FRAG_T& fragment() {
    return *fragment_;
  }

  const char* context_type() const { return CONTEXT_TYPE_VERTEX_DATA; }

  void SetValue(const DATA_T& data) { data_.SetValue(data); }

  void SetValue(vertex_t v, const DATA_T& data) { data_[v] = data; }

  const DATA_T& GetValue(vertex_t v) const { return data_[v]; }

 protected:
  std::shared_ptr<FRAG_T> fragment_;

 private:
  grape::VertexArray<DATA_T, vid_t> data_;
};

}  // namespace grape

#endif  // GRAPE_APP_VERTEX_DATA_CONTEXT_H_
