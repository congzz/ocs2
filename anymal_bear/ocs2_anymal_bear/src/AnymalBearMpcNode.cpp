/*
 * AnymalMPC.cpp
 *
 *  Created on: Apr 15, 2018
 *      Author: farbod
 */

#include <ocs2_quadruped_interface/QuadrupedMpcNode.h>

#include "ocs2_anymal_bear/AnymalBearInterface.h"

int main(int argc, char* argv[]) {
  if (argc <= 1) {
    throw std::runtime_error("No task file specified. Aborting.");
  }
  const std::string taskName(argv[1]);  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic))

  // Initialize ros node
  ros::init(argc, argv, "anymal_bear_mpc");
  ros::NodeHandle nodeHandle;

  auto anymalInterface = anymal::getAnymalBearInterface(taskName);
  ocs2::MPC_Settings mpcSettings;
  mpcSettings.loadSettings(anymal::getTaskFilePathBear(taskName));
  ocs2::SLQ_Settings slqSettings;
  slqSettings.loadSettings(anymal::getTaskFilePathBear(taskName));
  quadrupedMpcNode(nodeHandle, *anymalInterface, mpcSettings, slqSettings);

  return 0;
}
