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

#pragma once

#include <ocs2_core/constraint/StateInputConstraint.h>

/** Empty constraint with 0 entries */
class TestEmptyConstraint final : public ocs2::StateInputConstraint {
 public:
  using LinearApproximation_t = ocs2::VectorFunctionLinearApproximation;

  TestEmptyConstraint() = default;
  ~TestEmptyConstraint() override = default;
  TestEmptyConstraint* clone() const override { return new TestEmptyConstraint(*this); }

  size_t getNumConstraints(ocs2::scalar_t time) const override { return 0; };

  ocs2::vector_t getValue(ocs2::scalar_t time, const ocs2::vector_t& state, const ocs2::vector_t& input) const override {
    return ocs2::vector_t(0);
  }

  LinearApproximation_t getLinearApproximation(ocs2::scalar_t time, const ocs2::vector_t& state,
                                               const ocs2::vector_t& input) const override {
    LinearApproximation_t linearApproximation;
    linearApproximation.resize(0, state.rows(), input.rows());
    return linearApproximation;
  }
};

/** Linear constraint with 2 entries */
class TestLinearConstraint final : public ocs2::StateInputConstraint {
 public:
  using LinearApproximation_t = ocs2::VectorFunctionLinearApproximation;
  using QuadraticApproximation_t = ocs2::VectorFunctionQuadraticApproximation;

  TestLinearConstraint() = default;
  ~TestLinearConstraint() override = default;
  TestLinearConstraint* clone() const override { return new TestLinearConstraint(*this); }

  size_t getNumConstraints(ocs2::scalar_t time) const override { return 2; }

  ocs2::vector_t getValue(ocs2::scalar_t time, const ocs2::vector_t& state, const ocs2::vector_t& input) const override {
    ocs2::vector_t constraintValues(2);
    constraintValues << 1, 2;
    return constraintValues;
  }

  LinearApproximation_t getLinearApproximation(ocs2::scalar_t time, const ocs2::vector_t& state,
                                               const ocs2::vector_t& input) const override {
    LinearApproximation_t linearApproximation;
    linearApproximation.setZero(2, state.rows(), input.rows());
    linearApproximation.f = getValue(time, state, input);
    linearApproximation.dfdx.row(1).setOnes();
    linearApproximation.dfdu.row(1).setOnes();
    return linearApproximation;
  }

  QuadraticApproximation_t getQuadraticApproximation(ocs2::scalar_t time, const ocs2::vector_t& state,
                                                     const ocs2::vector_t& input) const override {
    QuadraticApproximation_t quadraticApproximation;
    quadraticApproximation.setZero(2, state.rows(), input.rows());
    quadraticApproximation.f = getValue(time, state, input);
    quadraticApproximation.dfdx.row(1).setOnes();
    quadraticApproximation.dfdu.row(1).setOnes();
    quadraticApproximation.dfdxx[1].setOnes();
    quadraticApproximation.dfdux[1].setOnes();
    quadraticApproximation.dfduu[1].setOnes();
    return quadraticApproximation;
  }
};
