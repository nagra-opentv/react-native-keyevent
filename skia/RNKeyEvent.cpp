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

RNKeyEventModule::RNKeyEventModule() {
  RNS_LOG_DEBUG("  calling constructor  ");
}

RNKeyEventModule::~RNKeyEventModule() {
  RNS_LOG_DEBUG("   calling distructor  ");
  auto inputEventManager = react::RSkInputEventManager::getInputKeyEventManager();
  if ( !inputEventManager ) {
    RNS_LOG_ERROR("Unable to get RSkInputEventManager instance ");
    return;
  }
  inputEventManager->removeKeyEventCallback(callbackId_);
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
            if(args[0].asString() == "onKeyDown")
              isKeyDownEventRegister_ = true;
            if(args[0].asString() == "onKeyUp")
              isKeyUpEventRegister_ = true;
            if(args[0].asString() == "onKeyMultiple")
              isKeyMultipleEventRegister_ = true;
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
              RNS_LOG_ERROR("calling stop stopObserving");
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
          folly::dynamic obj = folly::dynamic::object("pressedKey", RNSKeyMap[keyInput.key_])("action",(int)keyInput.action_)("keyCode",(int)keyInput.key_);
          std::string eventName;
          if(isKeyDownEventRegister_ && !keyInput.action_ ){
            eventName="onKeyDown";
          }    
          if(isKeyUpEventRegister_ && keyInput.action_){
            eventName="onKeyUp";
          }

          if(isKeyMultipleEventRegister_ && keyInput.repeat_){
            eventName="onKeyMultiple";
          }
          sendEventWithName(eventName,obj);
        }
    );//lambda for RSkinputEvent manager registation-end
}

void RNKeyEventModule::stopObserving(){
  auto inputEventManager = react::RSkInputEventManager::getInputKeyEventManager();
  if ( !inputEventManager ) {
    RNS_LOG_ERROR("Unable to get RSkInputEventManager instance ");
    return;
  }
  inputEventManager->removeKeyEventCallback(callbackId_);
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
