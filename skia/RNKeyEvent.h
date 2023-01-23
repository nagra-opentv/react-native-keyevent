/*
* Copyright (C) 1994-2022 OpenTV, Inc. and Nagravision S.A.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/
#pragma once
#include <fstream>
#include <thread>
#include "rns_shell/platform/linux/TaskLoop.h"
#include <cxxreact/CxxModule.h>
#include "ReactSkia/utils/RnsUtils.h"
#include "ReactSkia/core_modules/RSkInputEventManager.h"

using namespace std;
using namespace folly;
namespace facebook {
namespace xplat {
using namespace std;
class RNKeyEventModule : public module::CxxModule{
 private:
  int  keyEventId_= 0;
  int callbackId_ = 0;
  void eventHandleCallback(string eventName, int tag,rnsKeyAction keyAction,rnsKey keyCode);
  void sendEventWithName(std::string eventName, folly::dynamic eventData);
 public:
  RNKeyEventModule();
  ~RNKeyEventModule();
  auto getMethods() -> std::vector<Method>;
  std::string getName();
};
#ifdef __cplusplus
extern "C" {
#endif

xplat::module::CxxModule* RNKeyEventModuleCls(void) {
  return new RNKeyEventModule();
}

#ifdef __cplusplus
}
#endif
}//xplat
}//facebook
