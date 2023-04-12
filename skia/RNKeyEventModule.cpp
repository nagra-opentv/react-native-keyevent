/*
* Copyright (C) 1994-2023 OpenTV, Inc. and Nagravision S.A.
*
* This source code is licensed uander the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/
#include "RNKeyEventModule.h"

namespace facebook {
namespace xplat {

RNKeyEventModule::RNKeyEventModule():NativeEventEmitterModule(getInstance().lock().get()){}

RNKeyEventModule::~RNKeyEventModule() {
  stopObserving();
}

std::string RNKeyEventModule::getName() {
  return "RNKeyEvent";
}

auto RNKeyEventModule::getMethods() -> std::vector<Method> {
  std::vector<Method> supportedMethodsVector  = NativeEventEmitterModule::getMethods();
  return supportedMethodsVector;
}

void RNKeyEventModule::startObserving() {
  RNS_LOG_ERROR("calling the  startObserver");
  auto inputEventManager = react::RSkInputEventManager::getInputKeyEventManager();
  if (!inputEventManager) {
    RNS_LOG_ERROR("Unable to get RSkInputEventManager instance ");
    return;
  }
  callbackId_ = inputEventManager->addKeyEventCallback(
    [&](react::RSkKeyInput keyInput) {
    // lambda for RSkinputEvent manager registation.-starting
      if(keyInput.repeat_ == true){
        repeatCount_++;
      }
      folly::dynamic eventPlayload = generateEventPayload(keyInput);
      sendEventWithName(keyInput.action_ == RNS_KEY_Press?"onKeyDown":"onKeyUp", folly::dynamic(eventPlayload));
      if(repeatCount_ > 0 && keyInput.action_ == RNS_KEY_Release) {
        folly::dynamic eventPlayload = generateEventPayload(keyInput,repeatCount_);
        sendEventWithName("onKeyMultiple",folly::dynamic(eventPlayload));
        repeatCount_ = 0;//rest back once you send the event.  
      }
    }
  );//lambda for RSkinputEvent manager registation-end
}

folly::dynamic RNKeyEventModule::generateEventPayload(react::RSkKeyInput keyInput,unsigned int repeatCount) {
  folly::dynamic eventPlayload = folly::dynamic::object();
  if(repeatCount > 0 ){
    eventPlayload["repeatcount"] = repeatCount;
    //TODO character param need to update
  }

  eventPlayload["keyCode"] = static_cast<int>(keyInput.key_);
  eventPlayload["action"] = static_cast<int>(keyInput.action_);
  eventPlayload["pressedKey"] = static_cast<std::string>(RNSKeyMap[keyInput.key_])  ;
  return eventPlayload;
}

void RNKeyEventModule::stopObserving() {
  if(callbackId_ > 0){
    auto inputEventManager = react::RSkInputEventManager::getInputKeyEventManager();
    if ( !inputEventManager ) {
      RNS_LOG_ERROR("Unable to get RSkInputEventManager instance ");
      return;
    }
    inputEventManager->removeKeyEventCallback(callbackId_);
    callbackId_ = 0;// resetting the callback
  }else{
    RNS_LOG_ERROR("callbackId is invalid callbackId_ :: " << callbackId_);
  }
}

#ifdef __cplusplus
extern "C" {
#endif
RNS_EXPORT_MODULE(RNKeyEvent)
#ifdef __cplusplus
}
#endif


}//xplat
}//facebook