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
#ifndef ONEFLOW_CORE_FRAMEWORK_TENSOR_H_
#define ONEFLOW_CORE_FRAMEWORK_TENSOR_H_

#include "oneflow/core/common/data_type.h"
#include "oneflow/core/common/data_type.cfg.h"
#include "oneflow/core/common/shape_view.h"
#include "oneflow/core/common/shape.h"
#include "oneflow/core/memory/memory_case.pb.h"
#include "oneflow/core/framework/tensor_impl.h"

namespace oneflow {

class Blob;

namespace cfg {

class LogicalBlobId;
class ParallelConf;

}  // namespace cfg

class Tensor {
 public:
  virtual ~Tensor() = default;

  virtual std::shared_ptr<cfg::LogicalBlobId> lbi() const = 0;
  virtual std::string logical_blob_name() const = 0;
  virtual std::string op_name() const = 0;
  virtual std::string blob_name() const = 0;
  virtual std::shared_ptr<Shape> shape() const = 0;
  virtual DataType dtype() const = 0;
  virtual std::shared_ptr<cfg::ParallelConf> parallel_conf() const = 0;
};

namespace compatible_py {

class Distribute;
}

class Device;

namespace one {

class FunctionNode;

class Tensor {
 public:
  virtual ~Tensor() = default;

  // Getters
  virtual const std::shared_ptr<const Shape>& shape() const = 0;
  virtual DataType dtype() const = 0;
  virtual const std::shared_ptr<const ParallelDesc>& parallel_desc() const = 0;
  virtual bool is_lazy() const = 0;
  virtual bool is_consistent() const = 0;

  // Getters for autograd
  // acc_grad is tensor's accumulated grad in more than once backward operation,
  // and now_grad_arg is temporary grad to shared data with different FunctionNode
  virtual const std::shared_ptr<Tensor>& acc_grad() const = 0;
  virtual const std::shared_ptr<TensorArg>& now_grad_arg() const = 0;
  virtual bool requires_grad() const = 0;
  virtual bool is_leaf() const = 0;
  virtual bool retain_grad() const = 0;
  const std::shared_ptr<const FunctionNode>& grad_fn_node() const { return grad_fn_node_; }

  // Setters
  virtual void set_shape(const std::shared_ptr<const Shape>& shape) = 0;
  virtual void set_dtype(DataType dtype) = 0;
  virtual void set_parallel_desc(const std::shared_ptr<const ParallelDesc>& parallel_desc) = 0;

  // Setters for autograd
  virtual void set_acc_grad(const std::shared_ptr<Tensor>& grad) = 0;
  virtual void set_requires_grad(bool requires_grad) = 0;
  virtual void set_retain_grad(bool retain_grad) = 0;
  void set_grad_fn_node(const std::shared_ptr<const FunctionNode>& grad_fn_node) {
    grad_fn_node_ = grad_fn_node;
  }

  // Getters to be deprecated
  virtual const std::shared_ptr<compatible_py::BlobObject>& blob_object() const = 0;

  // Setters to be deprecated
  virtual void set_blob_object(const std::shared_ptr<compatible_py::BlobObject>& blob_object) = 0;

 protected:
  Tensor() = default;
  std::shared_ptr<const FunctionNode> grad_fn_node_;
};

class MirroredTensor final : public Tensor {
 public:
  OF_DISALLOW_COPY_AND_MOVE(MirroredTensor);
  MirroredTensor() = default;
  MirroredTensor(const std::shared_ptr<MirroredTensorImpl>& impl) { impl_ = impl; }
  ~MirroredTensor() = default;

  // Getters
  const std::shared_ptr<const Shape>& shape() const override { return impl_->shape(); }
  DataType dtype() const override { return impl_->dtype(); }
  const std::shared_ptr<const ParallelDesc>& parallel_desc() const override {
    return impl_->parallel_desc();
  }
  const std::shared_ptr<const Device>& device() const { return impl_->device(); }
  bool is_lazy() const override { return impl_->is_lazy(); }
  bool is_consistent() const override { return false; }

  // Getters for autograd
  const std::shared_ptr<Tensor>& acc_grad() const override { return impl_->acc_grad(); }
  const std::shared_ptr<TensorArg>& now_grad_arg() const override { return impl_->now_grad_arg(); }
  bool requires_grad() const override { return impl_->requires_grad(); }
  bool is_leaf() const override { return impl_->is_leaf(); }
  bool retain_grad() const override { return impl_->retain_grad(); }

  // Setters
  void set_shape(const std::shared_ptr<const Shape>& shape) override {
    return impl_->set_shape(shape);
  }
  void set_dtype(DataType dtype) override { return impl_->set_dtype(dtype); }
  void set_parallel_desc(const std::shared_ptr<const ParallelDesc>& parallel_desc) override {
    impl_->set_parallel_desc(parallel_desc);
  }
  void set_device(const std::shared_ptr<const Device>& device) { impl_->set_device(device); }

  // Setters for autograd
  void set_acc_grad(const std::shared_ptr<Tensor>& grad) override { impl_->set_acc_grad(grad); }
  void set_requires_grad(bool requires_grad) override { impl_->set_requires_grad(requires_grad); }
  void set_retain_grad(bool retain_grad) override { impl_->set_requires_grad(retain_grad); }

  // Getters to be deprecated
  const std::shared_ptr<compatible_py::BlobObject>& blob_object() const override {
    return impl_->blob_object();
  }

  // Setters to be deprecated
  void set_blob_object(const std::shared_ptr<compatible_py::BlobObject>& blob_object) override {
    impl_->set_blob_object(blob_object);
  }

 private:
  std::shared_ptr<MirroredTensorImpl> impl_;
};

class ConsistentTensor final : public Tensor {
 public:
  OF_DISALLOW_COPY_AND_MOVE(ConsistentTensor);
  ConsistentTensor() = default;
  ConsistentTensor(const std::shared_ptr<ConsistentTensorImpl>& impl) { impl_ = impl; }
  ~ConsistentTensor() = default;

  // Getters
  const std::shared_ptr<const Shape>& shape() const override { return impl_->shape(); }
  DataType dtype() const override { return impl_->dtype(); }
  const std::shared_ptr<const ParallelDesc>& parallel_desc() const override {
    return impl_->parallel_desc();
  }
  const std::shared_ptr<const compatible_py::Distribute>& distribute() const {
    return impl_->distribute();
  }
  bool is_lazy() const override { return impl_->is_lazy(); }
  bool is_consistent() const override { return true; }

  // Getters for autograd
  const std::shared_ptr<Tensor>& acc_grad() const override { return impl_->acc_grad(); }
  const std::shared_ptr<TensorArg>& now_grad_arg() const override { return impl_->now_grad_arg(); }
  bool requires_grad() const override { return impl_->requires_grad(); }
  bool is_leaf() const override { return impl_->is_leaf(); }
  bool retain_grad() const override { return impl_->retain_grad(); }

  // Setters
  void set_shape(const std::shared_ptr<const Shape>& shape) override {
    return impl_->set_shape(shape);
  }
  void set_dtype(DataType dtype) override { return impl_->set_dtype(dtype); }
  void set_parallel_desc(const std::shared_ptr<const ParallelDesc>& parallel_desc) override {
    impl_->set_parallel_desc(parallel_desc);
  }
  void set_distribute(const std::shared_ptr<const compatible_py::Distribute>& distribute) {
    impl_->set_distribute(distribute);
  }

  // Setters for autograd
  void set_acc_grad(const std::shared_ptr<Tensor>& grad) override { impl_->set_acc_grad(grad); }
  void set_requires_grad(bool requires_grad) override { impl_->set_requires_grad(requires_grad); }
  void set_retain_grad(bool retain_grad) override { impl_->set_requires_grad(retain_grad); }

  // Getters to be deprecated
  const std::shared_ptr<compatible_py::BlobObject>& blob_object() const override {
    return impl_->blob_object();
  }

  // Setters to be deprecated
  void set_blob_object(const std::shared_ptr<compatible_py::BlobObject>& blob_object) override {
    impl_->set_blob_object(blob_object);
  }

 private:
  std::shared_ptr<ConsistentTensorImpl> impl_;
};

}  // namespace one

namespace user_op {

class Tensor {
 public:
  ~Tensor() = default;

  virtual const ShapeView& shape() const = 0;
  virtual MutShapeView* mut_shape() = 0;
  virtual DataType data_type() const = 0;
  virtual const MemoryCase& mem_case() const = 0;
  virtual const void* raw_dptr() const = 0;
  virtual void* mut_raw_dptr() = 0;

  template<typename T = void>
  const T* dptr() const {
    CheckDataType<T>();
    return reinterpret_cast<const T*>(raw_dptr());
  }

  template<typename T = void>
  T* mut_dptr() {
    CheckDataType<T>();
    return reinterpret_cast<T*>(mut_raw_dptr());
  }

 protected:
  template<typename T>
  void CheckDataType() const {
    LOG_IF(FATAL, (std::is_same<T, void>::value == false && std::is_same<T, char>::value == false
                   && data_type() != DataType::kChar && data_type() != GetDataType<T>::value))
        << "tensor data_type mismatched. value: " << DataType_Name(data_type())
        << ", template T:" << DataType_Name(GetDataType<T>::value);
  }
};

}  // namespace user_op

}  // namespace oneflow

#endif  // ONEFLOW_CORE_FRAMEWORK_TENSOR_H_
