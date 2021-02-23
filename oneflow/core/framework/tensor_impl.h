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

#ifndef ONEFLOW_CORE_FRAMEWORK_TENSOR_IMPL_H_
#define ONEFLOW_CORE_FRAMEWORK_TENSOR_IMPL_H_

#include "oneflow/core/common/util.h"
#include "oneflow/core/common/data_type.h"
#include "oneflow/core/common/shape.h"
#include "oneflow/core/job/placement.cfg.h"
#include "oneflow/core/framework/object.h"

namespace oneflow {

namespace compatible_py {
class Distribute;
}

namespace one {

class Device;

class TensorImpl {
 public:
  virtual ~TensorImpl() = default;

  // getters
  virtual std::shared_ptr<Shape> shape() const = 0;
  virtual DataType dtype() const = 0;
  virtual std::shared_ptr<cfg::ParallelConf> parallel_conf() const = 0;
  virtual bool is_lazy() const = 0;

  // setters
  virtual void set_shape(const std::shared_ptr<Shape>& shape) = 0;
  virtual void set_dtype(DataType dtype) = 0;
  virtual void set_parallel_conf(const std::shared_ptr<cfg::ParallelConf>& parallel_conf) = 0;

  // getters will be deprecated
  virtual std::shared_ptr<compatible_py::BlobObject> blob_object() const = 0;

  // setters will be deprecated
  virtual void set_blob_object(const std::shared_ptr<compatible_py::BlobObject>& blob_object) = 0;
};

class MirroredTensorImpl : public TensorImpl {
 public:
  virtual ~MirroredTensorImpl() = default;

  // getters
  virtual std::shared_ptr<Device> device() const = 0;

  // setters
  virtual void set_device(const std::shared_ptr<Device>& device) = 0;
};

class ConsistentTensorImpl : public TensorImpl {
 public:
  virtual ~ConsistentTensorImpl() = default;

  // getters
  virtual std::shared_ptr<compatible_py::Distribute> distribute() const = 0;

  // setters
  virtual void set_distribute(const std::shared_ptr<compatible_py::Distribute>& distribute) = 0;
};

class LazyMirroredTensorImpl : public MirroredTensorImpl {
 public:
  OF_DISALLOW_COPY_AND_MOVE(LazyMirroredTensorImpl);
  LazyMirroredTensorImpl(const std::shared_ptr<Shape>& shape, DataType dtype,
                         const std::shared_ptr<Device>& device)
      : shape_(shape), dtype_(dtype), device_(device) {}
  ~LazyMirroredTensorImpl() = default;

  std::shared_ptr<Shape> shape() const override { return shape_; }
  void set_shape(const std::shared_ptr<Shape>& shape) override { shape_ = shape; }
  DataType dtype() const override { return dtype_; }
  void set_dtype(DataType dtype) override { dtype_ = dtype; }
  std::shared_ptr<cfg::ParallelConf> parallel_conf() const override { return parallel_conf_; }
  void set_parallel_conf(const std::shared_ptr<cfg::ParallelConf>& parallel_conf) override {
    parallel_conf_ = parallel_conf;
  }
  std::shared_ptr<Device> device() const override { return device_; }
  void set_device(const std::shared_ptr<Device>& device) override { device_ = device; }
  bool is_lazy() const override { return true; }
  void set_blob_object(const std::shared_ptr<compatible_py::BlobObject>& blob_object) override {
    UNIMPLEMENTED();
  }
  std::shared_ptr<compatible_py::BlobObject> blob_object() const override { UNIMPLEMENTED(); }

 private:
  std::shared_ptr<Shape> shape_;
  DataType dtype_;
  std::shared_ptr<Device> device_;
  std::shared_ptr<cfg::ParallelConf> parallel_conf_;
};

class EagerMirroredTensorImpl : public MirroredTensorImpl {
 public:
  OF_DISALLOW_COPY_AND_MOVE(EagerMirroredTensorImpl);
  EagerMirroredTensorImpl(const std::shared_ptr<Shape>& shape, DataType dtype,
                          const std::shared_ptr<Device>& device)
      : shape_(shape), dtype_(dtype), device_(device) {}
  ~EagerMirroredTensorImpl() = default;
  std::shared_ptr<Shape> shape() const override { return shape_; }
  void set_shape(const std::shared_ptr<Shape>& shape) override { shape_ = shape; }
  DataType dtype() const override { return dtype_; }
  void set_dtype(DataType dtype) override { dtype_ = dtype; }
  std::shared_ptr<cfg::ParallelConf> parallel_conf() const override { return parallel_conf_; }
  void set_parallel_conf(const std::shared_ptr<cfg::ParallelConf>& parallel_conf) override {
    parallel_conf_ = parallel_conf;
  }
  std::shared_ptr<Device> device() const override { return device_; }
  void set_blob_object(const std::shared_ptr<compatible_py::BlobObject>& blob_object) override {
    blob_object_ = blob_object;
  }
  std::shared_ptr<compatible_py::BlobObject> blob_object() const override { return blob_object_; }
  void set_device(const std::shared_ptr<Device>& device) override { device_ = device; }
  bool is_lazy() const override { return false; }

 private:
  std::shared_ptr<Shape> shape_;
  DataType dtype_;
  std::shared_ptr<Device> device_;
  std::shared_ptr<cfg::ParallelConf> parallel_conf_;
  std::shared_ptr<compatible_py::BlobObject> blob_object_;
};

class LazyConsistentTensorImpl : public ConsistentTensorImpl {
 public:
  OF_DISALLOW_COPY_AND_MOVE(LazyConsistentTensorImpl);
  LazyConsistentTensorImpl(const std::shared_ptr<Shape>& shape, DataType dtype,
                           const std::shared_ptr<compatible_py::Distribute>& distribute,
                           const std::shared_ptr<cfg::ParallelConf>& parallel_conf)
      : shape_(shape), dtype_(dtype), parallel_conf_(parallel_conf), distribute_(distribute) {}
  ~LazyConsistentTensorImpl() = default;
  
  std::shared_ptr<Shape> shape() const override { return shape_; }
  void set_shape(const std::shared_ptr<Shape>& shape) override { shape_ = shape; }
  DataType dtype() const override { return dtype_; }
  void set_dtype(DataType dtype) override { dtype_ = dtype; }
  std::shared_ptr<cfg::ParallelConf> parallel_conf() const override { return parallel_conf_; }
  void set_parallel_conf(const std::shared_ptr<cfg::ParallelConf>& parallel_conf) override {
    parallel_conf_ = parallel_conf;
  }
  void set_distribute(const std::shared_ptr<compatible_py::Distribute>& distribute) override {
    distribute_ = distribute;
  }
  std::shared_ptr<compatible_py::Distribute> distribute() const override { return distribute_; }
  void set_blob_object(const std::shared_ptr<compatible_py::BlobObject>& blob_object) override {
    UNIMPLEMENTED();
  }
  std::shared_ptr<compatible_py::BlobObject> blob_object() const override { UNIMPLEMENTED(); }
  bool is_lazy() const override { return true; }

 private:
  std::shared_ptr<Shape> shape_;
  DataType dtype_;
  std::shared_ptr<cfg::ParallelConf> parallel_conf_;
  std::shared_ptr<compatible_py::Distribute> distribute_;
};

class EagerConsistentTensorImpl : public ConsistentTensorImpl {
 public:
  OF_DISALLOW_COPY_AND_MOVE(EagerConsistentTensorImpl);
  EagerConsistentTensorImpl(const std::shared_ptr<Shape>& shape, DataType dtype,
                            const std::shared_ptr<compatible_py::Distribute>& distribute,
                            const std::shared_ptr<cfg::ParallelConf>& parallel_conf)
      : shape_(shape), dtype_(dtype), parallel_conf_(parallel_conf), distribute_(distribute) {}
  ~EagerConsistentTensorImpl() = default;
  std::shared_ptr<Shape> shape() const override { return shape_; }
  void set_shape(const std::shared_ptr<Shape>& shape) override { shape_ = shape; }
  DataType dtype() const override { return dtype_; }
  void set_dtype(DataType dtype) override { dtype_ = dtype; }
  std::shared_ptr<cfg::ParallelConf> parallel_conf() const override { return parallel_conf_; }
  void set_parallel_conf(const std::shared_ptr<cfg::ParallelConf>& parallel_conf) override {
    parallel_conf_ = parallel_conf;
  }
  void set_distribute(const std::shared_ptr<compatible_py::Distribute>& distribute) override {
    distribute_ = distribute;
  }
  std::shared_ptr<compatible_py::Distribute> distribute() const override { return distribute_; }
  void set_blob_object(const std::shared_ptr<compatible_py::BlobObject>& blob_object) override {
    blob_object_ = blob_object;
  }
  std::shared_ptr<compatible_py::BlobObject> blob_object() const override { return blob_object_; }
  bool is_lazy() const override { return false; }

 private:
  std::shared_ptr<Shape> shape_;
  DataType dtype_;
  std::shared_ptr<cfg::ParallelConf> parallel_conf_;
  std::shared_ptr<compatible_py::Distribute> distribute_;
  std::shared_ptr<compatible_py::BlobObject> blob_object_;
};

}  // namespace one

}  // namespace oneflow
#endif
