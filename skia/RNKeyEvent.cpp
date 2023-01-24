/*
* Copyright (C) 1994-2023 OpenTV, Inc. and Nagravision S.A.
*
* This source code is licensed uander the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/


#include <cxxreact/JsArgumentHelpers.h>
#include <cxxreact/Instance.h>

#include "RNKeyEvent.h"
#include "ReactSkia/utils/RnsLog.h"

namespace facebook {
namespace xplat {

RNKeyEventModule::RNKeyEventModule() {
  RNS_LOG_DEBUG("  calling constructor  ");
}

RNKeyEventModule::~RNKeyEventModule() {
  RNS_LOG_DEBUG("   calling disstructor  ");
  auto inputEventManager = react::RSkInputEventManager::getInputKeyEventManager();
  if ( !inputEventManager ) {
    RNS_LOG_ERROR("Unable to get RSkInputEventManager instance ");
    return;
  }
  inputEventManager->removeListener(callbackId_);
}

std::string RNKeyEventModule::getName() {
  return "RNKeyEvent";
}

auto RNKeyEventModule::getMethods() -> std::vector<Method> {
  return {
      Method(
          "addListener",
          [&] (folly::dynamic args) {
            ++keyEventId_;
            if ( keyEventId_ == 1 ) {
              auto inputEventManager = react::RSkInputEventManager::getInputKeyEventManager();
              if ( !inputEventManager ) {
                RNS_LOG_ERROR("Unable to get RSkInputEventManager instance ");
                keyEventId_=0;// set the keyEventId_
                return;
              }
              callbackId_ = inputEventManager->registerAddListener(
                [&](rnsKeyAction keyAction,rnsKey keyCode, bool keyRepeat) {
                  // lambda for RSkinputEvent manager registation.-starting
		  folly::dynamic obj = folly::dynamic::object("pressedKey", RNSKeyMap[keyCode])("action",(int)keyAction)("keyCode",(int)keyCode);
		  std::string eventName =  keyRepeat? std::string("onKeyMultiple"): keyAction? std::string("onKeyUp"): std::string("onKeyDown");
                  sendEventWithName(eventName,obj);
                });//lambda for RSkinputEvent manager registation-end
            }
            return;
          }),// end of addListener lambda

      Method(
          "removeListeners",
          [&] (folly::dynamic args) {
            if ( keyEventId_ == 1 ) {
              auto inputEventManager = react::RSkInputEventManager::getInputKeyEventManager();
              if ( !inputEventManager ) {
                RNS_LOG_ERROR("Unable to get RSkInputEventManager instance ");
                return;
              }
              inputEventManager->removeListener(callbackId_);
            }
            if ( keyEventId_ >= 1 ) {
              --keyEventId_;
            }
            return;
          }),
  };
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

