/*
* Copyright (C) 1994-2023 OpenTV, Inc. and Nagravision S.A.
*
* This source code is licensed uander the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/
#include <folly/dynamic.h>
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>

#include "RNKeyEvent.h"

namespace facebook {
namespace xplat {

RNKeyEventModule::RNKeyEventModule() {}

RNKeyEventModule::~RNKeyEventModule() {
  stopObserving();
}

std::string RNKeyEventModule::getName() {
  return "RNKeyEvent";
}

auto RNKeyEventModule::getMethods() -> std::vector<Method> {
  return {
      Method(
          "addListener",
          [&] (folly::dynamic args) {
            RNS_LOG_DEBUG("addListener args "<< args[0]);
            listenerCount_++;
            if (listenerCount_ == 1) { 
              startObserving();
            }
          }),// end of addListener lambda

      Method(
          "removeListeners",
          [&] (folly::dynamic args) {
            RNS_LOG_DEBUG(" removeListeners args "<< args[0]<<" listenerCount_ "<<listenerCount_);
            int  removeCount = args[0].asInt();;
            listenerCount_ = std::max(listenerCount_ - removeCount, 0);
            if (listenerCount_ == 0) {
              stopObserving();
            }
          }),
  };
}

void RNKeyEventModule::startObserving(){
    auto inputEventManager = react::RSkInputEventManager::getInputKeyEventManager();
    if ( !inputEventManager ) {
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
          sendEventWithName(keyInput.action_ == RNS_KEY_Press?"onKeyDown":"onKeyUp",eventPlayload);
          if(repeatCount_ > 0 && keyInput.action_ == RNS_KEY_Release){
            folly::dynamic eventPlayload = generateEventPayload(keyInput,repeatCount_);
            sendEventWithName("onKeyMultiple",eventPlayload);
            repeatCount_ = 0;//rest back once you send the event.  
          }
        }
    );//lambda for RSkinputEvent manager registation-end
}
folly::dynamic RNKeyEventModule::generateEventPayload(react::RSkKeyInput keyInput,unsigned int repeatCount){
  
  folly::dynamic eventPlayload = folly::dynamic::object();
  if(repeatCount > 0 ){
    eventPlayload["repeatcount"] = repeatCount;
    //TODO character param need to update
  }

  eventPlayload["keyCode"] = (int) keyInput.key_;
  eventPlayload["action"] = (int) keyInput.action_;
  eventPlayload["pressedKey"] = (std::string) RNSKeyMap[keyInput.key_];
  return eventPlayload;
}
void RNKeyEventModule::stopObserving(){
  auto inputEventManager = react::RSkInputEventManager::getInputKeyEventManager();
  if ( !inputEventManager ) {
    RNS_LOG_ERROR("Unable to get RSkInputEventManager instance ");
    return;
  }
  if(callbackId_ > 0){
    inputEventManager->removeKeyEventCallback(callbackId_);
  }
  else{
    RNS_LOG_ERROR("callbackId is invalid callbackId_ :: " << callbackId_);
  }
}

void RNKeyEventModule::sendEventWithName(std::string eventName, folly::dynamic eventData) {
  auto instance = getInstance().lock();
  if ( instance ) {
    instance->callJSFunction(
            "RCTDeviceEventEmitter", "emit",
            (eventData != nullptr) ?
            folly::dynamic::array(folly::dynamic::array(eventName),eventData):
            folly::dynamic::array(eventName));
  }
}

}//xplat
}//facebook
