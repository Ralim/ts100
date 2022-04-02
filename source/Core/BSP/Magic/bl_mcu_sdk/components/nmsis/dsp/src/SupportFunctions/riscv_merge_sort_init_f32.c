/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_merge_sort_init_f32.c
 * Description:  Floating point merge sort initialization function
 *
 * $Date:        23 April 2021
 * $Revision:    V1.9.0
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2010-2021 ARM Limited or its affiliates. All rights reserved.
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dsp/support_functions.h"

/**
  @ingroup groupSupport
 */

/**
  @addtogroup Sorting
  @{
 */


  /**
   * @param[in,out]  S            points to an instance of the sorting structure.
   * @param[in]      dir          Sorting order.
   * @param[in]      buffer       Working buffer.
   */
void riscv_merge_sort_init_f32(riscv_merge_sort_instance_f32 * S, riscv_sort_dir dir, float32_t * buffer)
{
    S->dir    = dir;
    S->buffer = buffer;
}
/**
  @} end of Sorting group
 */
