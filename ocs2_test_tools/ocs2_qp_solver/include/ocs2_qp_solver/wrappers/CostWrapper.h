/******************************************************************************
Copyright (c) 2017, Farbod Farshidian. All rights reserved.

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

//
// Created by rgrandia on 25.02.20.
//

#pragma once

#include <ocs2_core/cost/CostFunctionBase.h>

#include <ocs2_qp_solver/QpSolverTypes.h>

namespace ocs2 {
namespace qp_solver {

/**
 * Wrapper class that wraps a CostFunctionBase of any size and provides a dynamic size interface.
 * The wrapper clones the cost function upon construction, and owns the clone.
 * This class is not thread safe, because the underlying cost function is not thread safe.
 */
class CostWrapper {
 public:
  /** Constructor */
  CostWrapper(const ocs2::CostFunctionBase& costFunction)  // NOLINT(google-explicit-constructor)
      : p_(costFunction.clone()) {}

  /** Copy constructor clones the underlying handle and cost */
  CostWrapper(const CostWrapper& other) : p_(other.p_->clone()) {}

  /** Copy assignment operator */
  CostWrapper& operator=(const CostWrapper& other) {
    *this = CostWrapper(other);
    return *this;
  }

  /** Move constructor moves the cost */
  CostWrapper(CostWrapper&&) noexcept = default;

  /** Move assignments moves the cost */
  CostWrapper& operator=(CostWrapper&&) noexcept = default;

  /** Evaluate the cost */
  scalar_t getCost(scalar_t t, const vector_t& x, const vector_t& u);

  /** Gets the cost approximation */
  ScalarFunctionQuadraticApproximation getQuadraticApproximation(scalar_t t, const vector_t& x, const vector_t& u);

  /** Evaluate the terminal cost */
  scalar_t getTerminalCost(scalar_t t, const vector_t& x);

  /** Gets the terminal cost approximation */
  ScalarFunctionQuadraticApproximation getTerminalQuadraticApproximation(scalar_t t, const vector_t& x);

 private:
  /** Cost function */
  std::unique_ptr<ocs2::CostFunctionBase> p_;
};

}  // namespace qp_solver
}  // namespace ocs2
