/*
* Copyright (C) 1994-2023 OpenTV, Inc. and Nagravision S.A.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/
#pragma once
#include "ReactSkia/core_modules/RSkInputEventManager.h"
#include "ReactSkia/LegacyNativeModules/NativeEventEmitter.h"
#include "ReactSkia/utils/RnsUtils.h"
namespace facebook {
namespace xplat {

class RNKeyEventModule : public react::NativeEventEmitter{
 private:
  int callbackId_ = 0;
  void startObserving();
  void stopObserving();
  unsigned int repeatCount_=0;
  folly::dynamic generateEventPayload(react::RSkKeyInput keyInput,unsigned int repeatCount = 0);
 public:
  RNKeyEventModule();
  ~RNKeyEventModule();
  auto getMethods() -> std::vector<Method>;
  std::string getName();
};


}//xplat
}//facebook
