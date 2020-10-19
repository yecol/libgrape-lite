
#ifndef GRAPE_APP_VOID_CONTEXT_H_
#define GRAPE_APP_VOID_CONTEXT_H_
#include "grape/app/context_base.h"
#define CONTEXT_TYPE_VOID "void"

namespace grape {

template <typename FRAG_T>
class VoidContext : public ContextBase {
  using fragment_t = FRAG_T;
  using vertex_t = typename fragment_t::vertex_t;

 public:
  explicit VoidContext(const fragment_t& fragment) : fragment_(fragment) {}

  const fragment_t& fragment() { return fragment_; }

  const char* context_type() const override { return CONTEXT_TYPE_VOID; }

 private:
  const fragment_t& fragment_;
};
}  // namespace grape
#endif  // GRAPE_APP_VOID_CONTEXT_H_
