# Open source copyright declaration based on BSD open source template:
# http://www.opensource.org/licenses/bsd-license.php
#
# This file is part of the OP2 distribution.
#
# Copyright (c) 2011, Florian Rathgeber and others. Please see the AUTHORS
# file in the main source directory for a full list of copyright holders.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * The name of Florian Rathgeber may not be used to endorse or promote
#       products derived from this software without specific prior written
#       permission.
#
# THIS SOFTWARE IS PROVIDED BY Florian Rathgeber ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL Florian Rathgeber BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

project(OP2-Jac2)

# Require CMake 2.8
cmake_minimum_required(VERSION 2.8)

include(../common.cmake)

# simple sequential version
if (TARGET op2_seq)
  add_executable(jac2_seq jac.cpp res.h update.h)
  target_link_libraries(jac2_seq op2_seq)
  install(TARGETS jac2_seq RUNTIME DESTINATION ${OP2_APPS_DIR} COMPONENT RuntimeExecutables)
endif()

# x86 version using kernel files generated by op2.m
if (TARGET op2_openmp)
  add_executable(jac2_openmp jac_op.cpp jac_kernels.cpp res.h update.h)
  target_link_libraries(jac2_openmp op2_openmp)
  set_target_properties(jac2_openmp PROPERTIES COMPILE_FLAGS
    "${OpenMP_CXX_FLAGS}" LINK_FLAGS "${OpenMP_CXX_FLAGS}")
  install(TARGETS jac2_openmp RUNTIME DESTINATION ${OP2_APPS_DIR} COMPONENT RuntimeExecutables)
endif()

# CUDA version using kernel files generated by op2.m
if (TARGET op2_cuda)
  find_package(CUDA)
  if (CUDA_FOUND)
    cuda_add_executable(jac2_cuda jac_op.cpp jac_kernels.cu res.h update.h)
    target_link_libraries(jac2_cuda op2_cuda)
    install(TARGETS jac2_cuda RUNTIME DESTINATION ${OP2_APPS_DIR} COMPONENT RuntimeExecutables)
  endif()
endif()

# MPI with sequential-nodes version
if (TARGET op2_mpi)
  add_definitions(${OP2_MPI_DEFINITIONS})
  include_directories(${OP2_MPI_INCLUDE_DIRS})
  add_executable(jac2_mpi jac_mpi.cpp res.h update.h)
  target_link_libraries(jac2_mpi op2_mpi)
  install(TARGETS jac2_mpi RUNTIME DESTINATION ${OP2_APPS_DIR} COMPONENT RuntimeExecutables)
endif()
