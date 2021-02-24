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
#include "oneflow/core/graph/compute_task_node.h"
#include "oneflow/core/graph/logical_node.h"

namespace oneflow {

class UnpackCompTaskNode final : public CompTaskNode {
 public:
  OF_DISALLOW_COPY_AND_MOVE(UnpackCompTaskNode);
  UnpackCompTaskNode() = default;
  ~UnpackCompTaskNode() override = default;

  TaskType GetTaskType() const override { return TaskType::kUnpack; }

  void ProduceAllRegstsAndBindEdges() override;
  void ConsumeAllRegsts() override;

 private:
  void BuildExecGphAndRegst() override;
  void InferProducedDataRegstTimeShape() override;
};

void UnpackCompTaskNode::ProduceAllRegstsAndBindEdges() {
  ProduceRegst("out", false);
  ForEachOutDataEdge([&](TaskEdge* edge) { BindEdgeWithProducedRegst(edge, "out"); });
}

void UnpackCompTaskNode::ConsumeAllRegsts() {
  ConsumeRegst("in", SoleInDataEdge()->GetSoleRegst());
}

void UnpackCompTaskNode::BuildExecGphAndRegst() {
  std::shared_ptr<const Operator> op = logical_node()->SoleOp();
  ExecNode* exec_node = mut_exec_gph().NewNode();
  exec_node->mut_op() = op;
  exec_node->BindBnWithRegst(op->SoleIbn(), GetSoleConsumedRegst("in"));

  std::shared_ptr<RegstDesc> out_regst = GetProducedRegst("out");
  out_regst->AddLbi(op->BnInOp2Lbi(op->SoleObn()));
  exec_node->BindBnWithRegst(op->SoleObn(), out_regst);
  exec_node->InferBlobDescs(parallel_ctx());
}

void UnpackCompTaskNode::InferProducedDataRegstTimeShape() {
  auto TimeShape4Ibn = [&](const std::string& ibn) -> const Shape* {
    return GetSoleConsumedRegst("in")->data_regst_time_shape().get();
  };
  std::shared_ptr<Shape> time_shape(new Shape());
  logical_node()->SoleOp()->InferOutputBlobTimeShape(TimeShape4Ibn, parallel_ctx(),
                                                     time_shape.get());
  ForEachProducedDataRegst([time_shape](const std::string& name, RegstDesc* regst) {
    *regst->mut_data_regst_time_shape() = time_shape;
  });
}

REGISTER_USER_OP_COMP_TASK_NODE_TYPE("unpack", UnpackCompTaskNode);
REGISTER_USER_OP_INDEPENDENT_AREA_ID("unpack")

REGISTER_COMPUTE_TASK_NODE_STREAM_INDEX_GETTER(DeviceType::kGPU, TaskType::kUnpack)                               \
  .SetStreamIndexGetterFn([](const CompTaskNode* comp_task_node,                                                  \
                             std::function<uint32_t(int task_type)> Counter,                                      \
                             std::function<uint32_t(const TaskNode*)> AllocateCpuStreamIndexEvenly) -> uint32_t { \
      return CudaStreamIndex::kCompute;                                                                           \
  });

REGISTER_COMPUTE_TASK_NODE_STREAM_INDEX_GETTER(DeviceType::kCPU, TaskType::kUnpack)                               \
  .SetStreamIndexGetterFn([](const CompTaskNode* comp_task_node,                                                  \
                             std::function<uint32_t(int task_type)> Counter,                                      \
                             std::function<uint32_t(const TaskNode*)> AllocateCpuStreamIndexEvenly) -> uint32_t { \
    if (comp_task_node->IsIndependent()) {                                                                        \
      return StreamIndex::Independent(TaskType::kUnpack, Counter);                                                \
    } else {                                                                                                      \
      return AllocateCpuStreamIndexEvenly(comp_task_node);                                                        \
    }                                                                                                             \
  });

}  // namespace oneflow
