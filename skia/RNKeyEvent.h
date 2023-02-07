/*
* Copyright (C) 1994-2023 OpenTV, Inc. and Nagravision S.A.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/
#pragma once
#include <cxxreact/CxxModule.h>
#include "ReactSkia/core_modules/RSkInputEventManager.h"
#include "ReactSkia/utils/RnsUtils.h"


namespace facebook {
namespace xplat {
class RNKeyEventModule : public module::CxxModule{
 private:
  int  listenerCount_= 0;
  int callbackId_ = 0;
  void startObserving();
  void stopObserving();
  unsigned int repeatCount_=0;
  folly::dynamic generateEventPayload(react::RSkKeyInput keyInput,unsigned int repeatCount = 0);
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
  RNS_EXPORT_MODULE(RNKeyEventModule)
#ifdef __cplusplus
}
#endif
}//xplat
}//facebook
