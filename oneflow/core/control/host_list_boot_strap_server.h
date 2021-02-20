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
#ifndef ONEFLOW_CORE_CONTROL_HOST_LIST_BOOT_STRAP_SERVER_H_
#define ONEFLOW_CORE_CONTROL_HOST_LIST_BOOT_STRAP_SERVER_H_

#include "oneflow/core/control/boot_strap_server.h"
#include "oneflow/core/job/env_desc.h"

namespace oneflow {

class HostListBootStrapServer final : public BootStrapServer {
 public:
  OF_DISALLOW_COPY_AND_MOVE(HostListBootStrapServer);
  ~HostListBootStrapServer() override = default;

  HostListBootStrapServer(const EnvDesc& env_desc);
  const std::string& this_machine_addr() { return this_machine_addr_; }

 private:
  void OnLoadServer(CtrlCall<CtrlMethod::kLoadServer>* call) override;

  bool is_first_connect_;
  std::string this_machine_addr_;
};

}  // namespace oneflow

#endif  // ONEFLOW_CORE_CONTROL_HOST_LIST_BOOT_STRAP_SERVER_H_
