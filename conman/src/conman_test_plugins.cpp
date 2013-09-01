/** Copyright (c) 2013, Jonathan Bohren, all rights reserved. 
 * This software is released under the BSD 3-clause license, for the details of
 * this license, please see LICENSE.txt at the root of this repository. 
 */

#include <rtt/Component.hpp>

#include <conman/conman_test_plugins.h>
#include <conman/hook.h>

TestEffortController::TestEffortController(std::string const& name) :
  RTT::TaskContext(name)
{
  using namespace conman;
  // Create RTT ports
  this->addPort("effort_in", effort_in_).doc("Effort input.");
  this->addPort("effort_out", effort_out_).doc("Effort output := input + 1.");

  // Load block interface
  boost::shared_ptr<conman::Hook> conman_hook = conman::Hook::GetHook(this);

  if(conman_hook.get()) {

    // Add the ports to conman
    conman_hook->setInputExclusivity("effort_in", Exclusivity::EXCLUSIVE);

  } else {
    RTT::log(RTT::Fatal) << "Could not load conman hook." << RTT::endlog();
  }
}

bool TestEffortController::configureHook() {

  return true;
}

bool TestEffortController::startHook() {

  // Ready if the input is connected
  bool ready = effort_in_.connected();

  return ready;
}

void TestEffortController::updateHook() {
  double effort;
  effort_in_.read(effort);
  effort_out_.write(effort + 1);
  RTT::log(RTT::Debug) << this->getName() << " updateHook." << RTT::endlog();
}

ORO_LIST_COMPONENT_TYPE(TestEffortController)
ORO_CREATE_COMPONENT_LIBRARY()