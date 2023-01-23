/*
* Copyright (C) 1994-2022 OpenTV, Inc. and Nagravision S.A.
*
* This source code is licensed uander the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/


#include <folly/dynamic.h>
#include <cxxreact/JsArgumentHelpers.h>
#include <cxxreact/Instance.h>
#include "ReactSkia/utils/RnsLog.h"
#include "RNKeyEvent.h"
#include "ReactSkia/sdk/NotificationCenter.h"


using namespace folly;
namespace facebook {
namespace xplat {

using namespace std;
RNKeyEventModule::RNKeyEventModule() {
  RNS_LOG_INFO(" *** calling constructor *** ");
}

RNKeyEventModule::~RNKeyEventModule() {
 RNS_LOG_INFO(" *** calling disstructor *** "); 
}

std::string RNKeyEventModule::getName() {
  return "RNKeyEvent";
}

auto RNKeyEventModule::getMethods() -> std::vector<Method> {
  return {
      Method(
          "addListener",
          [&] (dynamic args) {
            ++keyEventId_;
            if(keyEventId_ == 1){
              auto inputEventManager = react::RSkInputEventManager::getInputKeyEventManager();//nullcheck
              if(!inputEventManager){
                keyEventId_=0;
                return;
              }
              callbackId_ = inputEventManager->registerAddListener( // move the  structure
                [&](string eventType,int tag,rnsKeyAction keyAction,rnsKey keyCode, bool keyRepeat){
                  dynamic obj = dynamic::object("pressedKey", eventType)("action",(int)keyAction)("keyCode",(int)keyCode);
                  string eventName =  keyRepeat? string("onKeyMultiple"): keyAction?string("onKeyUp"):string("onKeyDown");
                  sendEventWithName(eventName,obj);
                }); 
            }
            return;
          }),

      Method(
          "removeListeners",
          [&] (dynamic args){
            if(keyEventId_ == 1 ){
              auto inputEventManager = react::RSkInputEventManager::getInputKeyEventManager();
              inputEventManager->removeListener(callbackId_);
            }
            if(keyEventId_ >= 1){
              --keyEventId_;
            }
            return;
          }),
  };


}

void RNKeyEventModule::sendEventWithName(std::string eventName, folly::dynamic eventData) {
  auto instance = getInstance().lock();
  if(instance) {
    instance->callJSFunction(
            "RCTDeviceEventEmitter", "emit",
            (eventData != nullptr) ?
            folly::dynamic::array(folly::dynamic::array(eventName),eventData):
            folly::dynamic::array(eventName));
  }
}



}//xplat
}//facebook

