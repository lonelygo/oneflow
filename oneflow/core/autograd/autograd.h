/*
Copyright 2020 The OneFlow Authors. All rights reserved.

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

#ifndef ONEFLOW_CORE_AUTOGRAD_AUTOGRAD_H_
#define ONEFLOW_CORE_AUTOGRAD_AUTOGRAD_H_

#include <vector>
#include <memory>
#include "oneflow/core/framework/tensor_list.h"

namespace oneflow {

struct AutoGradUtil final {
  // Export to python interface as flow.autograd.backward()
  std::shared_ptr<TensorList> Backward(const std::shared_ptr<TensorList>& outputs,
                                       const std::shared_ptr<TensorList>& out_grads,
                                       bool retain_graph, bool create_graph);

  // Export to python interface as flow.autograd.grad()
  std::shared_ptr<TensorList> Grad(const std::shared_ptr<TensorList>& outputs,
                                   const std::shared_ptr<TensorList>& inputs,
                                   const std::shared_ptr<TensorList>& out_grads, bool retain_graph,
                                   bool create_graph);
};

}  // namespace oneflow

#endif  // ONEFLOW_CORE_AUTOGRAD_AUTOGRAD_H_