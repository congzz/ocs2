/******************************************************************************
Copyright (c) 2020, Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#include "ocs2_oc/synchronized_module/LoopshapingSynchronizedModule.h"

namespace ocs2 {

LoopshapingSynchronizedModule::LoopshapingSynchronizedModule(
    std::shared_ptr<LoopshapingDefinition> loopshapingDefinitionPtr,
    std::vector<std::shared_ptr<SolverSynchronizedModule>> synchronizedModulesPtrArray)
    : loopshapingDefinitionPtr_(std::move(loopshapingDefinitionPtr)),
      synchronizedModulesPtrArray_(std::move(synchronizedModulesPtrArray)) {}

void LoopshapingSynchronizedModule::preSolverRun(scalar_t initTime, scalar_t finalTime, const vector_t& currentState,
                                                 const CostDesiredTrajectories& costDesiredTrajectory) {
  if (!synchronizedModulesPtrArray_.empty()) {
    const auto systemState = loopshapingDefinitionPtr_->getSystemState(currentState);

    for (auto& module : synchronizedModulesPtrArray_) {
      module->preSolverRun(initTime, finalTime, systemState, costDesiredTrajectory);
    }
  }
}

void LoopshapingSynchronizedModule::postSolverRun(const PrimalSolution& primalSolution) {
  if (!synchronizedModulesPtrArray_.empty()) {
    PrimalSolution systemPrimalSolution;
    systemPrimalSolution.timeTrajectory_ = primalSolution.timeTrajectory_;
    systemPrimalSolution.modeSchedule_ = primalSolution.modeSchedule_;
    systemPrimalSolution.stateTrajectory_.reserve(primalSolution.stateTrajectory_.size());
    systemPrimalSolution.inputTrajectory_.reserve(primalSolution.inputTrajectory_.size());
    for (size_t k = 0; k < primalSolution.stateTrajectory_.size(); ++k) {
      const auto systemState = loopshapingDefinitionPtr_->getSystemState(primalSolution.stateTrajectory_[k]);
      const auto systemInput = loopshapingDefinitionPtr_->getSystemInput(systemState, primalSolution.inputTrajectory_[k]);
      systemPrimalSolution.stateTrajectory_.push_back(std::move(systemState));
      systemPrimalSolution.inputTrajectory_.push_back(std::move(systemInput));
    }

    for (auto& module : synchronizedModulesPtrArray_) {
      module->postSolverRun(systemPrimalSolution);
    }
  }
}

}  // namespace ocs2
