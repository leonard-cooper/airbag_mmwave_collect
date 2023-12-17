/* ===========================================================================
** Copyright (C) 2021 Infineon Technologies AG
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

/**
 * @file Cube.h
 *
 * \brief \copybrief gr_cube
 *
 * For details refer to \ref gr_cube
 */

#ifndef IFX_BASE_CUBE_H
#define IFX_BASE_CUBE_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include "ifxBase/Defines.h"
#include "ifxBase/Matrix.h"

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/

#define IFX_CUBE_ROWS(c)                ((c)->rows)
#define IFX_CUBE_COLS(c)                ((c)->cols)
#define IFX_CUBE_SLICES(c)              ((c)->slices)
#define IFX_CUBE_DAT(c)                 ((c)->d)
#define IFX_CUBE_SLICE_SIZE(c)          ((size_t)IFX_CUBE_ROWS(c) * (size_t)IFX_CUBE_COLS(c))
#define IFX_CUBE_SIZE(c)                (IFX_CUBE_SLICE_SIZE(c) * (size_t)IFX_CUBE_SLICES(c))
#define IFX_CUBE_SLICE_OFFSET(c, s)     ((size_t)IFX_CUBE_SLICE_SIZE(c) * (size_t)(s))
#define IFX_CUBE_OFFSET(cub, r, c, s)   (IFX_CUBE_SLICE_OFFSET(cub, s) + (size_t)IFX_CUBE_COLS(cub) * (r) + (size_t)(c))
#define IFX_CUBE_SLICE_AT(c, s)         (IFX_CUBE_DAT(c)[IFX_CUBE_SLICE_OFFSET(c, s)])
#define IFX_CUBE_AT(cub, r, c, s)       (IFX_CUBE_DAT(cub)[IFX_CUBE_OFFSET(cub, r, c, s)])

#define IFX_CUBE_BRK_DIM(c1, c2)        IFX_ERR_BRK_COND((IFX_CUBE_ROWS(c1) != IFX_CUBE_ROWS(c2)) || (IFX_CUBE_COLS(c1) != IFX_CUBE_COLS(c2)) || (IFX_CUBE_SLICES(c1) != IFX_CUBE_SLICES(c2)), IFX_ERROR_DIMENSION_MISMATCH)
#define IFX_CUBE_BRV_DIM(c1, c2, a)     IFX_ERR_BRV_COND((IFX_CUBE_ROWS(c1) != IFX_CUBE_ROWS(c2)) || (IFX_CUBE_COLS(c1) != IFX_CUBE_COLS(c2)) || (IFX_CUBE_SLICES(c1) != IFX_CUBE_SLICES(c2)), IFX_ERROR_DIMENSION_MISMATCH, a)


/*
==============================================================================
   3. TYPES
==============================================================================
*/

/**
 * @brief Defines the structure for real Cube data core parameters.
 *        Use type ifx_Cube_R_t for this struct.
 */
struct ifx_Cube_R_s
{
    ifx_Float_t* d;
    uint32_t rows;
    uint32_t cols;
    uint32_t slices;
    uint8_t owns_d;
};

/**
 * @brief Defines the structure for complex Cube data core parameters.
 *        Use type ifx_Cube_C_t for this struct.
 */
struct ifx_Cube_C_s
{
    ifx_Complex_t* d;
    uint32_t rows;
    uint32_t cols;
    uint32_t slices;
    uint8_t owns_d;
};

/**
 * @brief Forward declaration structure to operate on real Cube.
 */
typedef struct ifx_Cube_R_s ifx_Cube_R_t;

/**
 * @brief Forward declaration structure to operate on complex Cube.
 */
typedef struct ifx_Cube_C_s ifx_Cube_C_t;

/*
==============================================================================
   4. FUNCTION PROTOTYPES
==============================================================================
*/

/** @addtogroup gr_cat_SDK_base
  * @{
  */

/** @defgroup gr_cube Cube
  * @brief API for operations on Cube data structures
  *
  * Supports operations on Cube (array of matrices) data structures.
  *
  * @{
  */

/**
 * @brief Initializes a real cube \ref ifx_Cube_R_t with a data element of
 *        specified size, to prevent memory allocation on the heap.
 *
 * @param [in]     cube      Pointer to an allocated cube instance defined
 *                           by \ref ifx_Cube_R_t to be initialized.
 * @param [in]     data      Data pointer to assign the cube.
 * @param [in]     rows      Number of rows in the cube.
 * @param [in]     columns   Number of columns in the cube.
 * @param [in]     slices    Number of slices in the cube.
 *
 */
IFX_DLL_PUBLIC
void ifx_cube_init_r(ifx_Cube_R_t* cube,
                     ifx_Float_t* data,
                     uint32_t rows,
                     uint32_t columns,
                     uint32_t slices);

/**
 * @brief Initializes a complex cube \ref ifx_Cube_C_t with a data element of
 *        specified size, to prevent memory allocation on the heap.
 *
 * @param [in]     cube      Pointer to an allocated cube instance defined
 *                           by \ref ifx_Cube_C_t to be initialized.
 * @param [in]     data      Data pointer to assign the cube.
 * @param [in]     rows      Number of rows in the cube.
 * @param [in]     columns   Number of columns in the cube.
 * @param [in]     slices    Number of slices in the cube.
 */
IFX_DLL_PUBLIC
void ifx_cube_init_c(ifx_Cube_C_t* cube,
                     ifx_Complex_t* data,
                     uint32_t rows,
                     uint32_t columns,
                     uint32_t slices);

/**
 * @brief Allocates memory for a real cube with a specified number of
 *        rows and columns and slices and initializes it to zero.
 *        See \ref ifx_Cube_R_t for more details.
 *
 * @param [in]     rows      Number of rows in the cube.
 * @param [in]     columns   Number of columns in the cube.
 * @param [in]     slices    Number of slices in the cube.
 *
 * @return Pointer to allocated and initialized real cube structure or NULL if allocation failed.
 *
 */
IFX_DLL_PUBLIC
ifx_Cube_R_t* ifx_cube_create_r(uint32_t rows,
                                uint32_t columns,
                                uint32_t slices);

/**
 * @brief Allocates memory for a real cube with a specified number of
 *        rows and columns and slices and initializes it to zero.
 *        See \ref ifx_Cube_C_t for more details.
 *
 * @param [in]     rows      Number of rows in the cube.
 * @param [in]     columns   Number of columns in the cube.
 * @param [in]     slices    Number of slices in the cube.
 *
 * @return Pointer to allocated and initialized real cube structure or NULL if allocation failed.
 *
 */
IFX_DLL_PUBLIC
ifx_Cube_C_t* ifx_cube_create_c(uint32_t rows,
                                uint32_t columns,
                                uint32_t slices);

/**
 * @brief De-initializes a real cube \ref ifx_Cube_R_t.
 *
 * @param [in]     cube      Pointer to an allocated cube instance defined
 *                           by \ref ifx_Cube_R_t to be de-initialized.
 *
 */
IFX_DLL_PUBLIC
void ifx_cube_deinit_r(ifx_Cube_R_t* cube);

/**
 * @brief De-initializes a complex cube \ref ifx_Cube_R_t.
 *
 * @param [in]     cube      Pointer to an allocated cube instance defined
 *                           by \ref ifx_Cube_C_t to be de-initialized.
 *
 */
IFX_DLL_PUBLIC
void ifx_cube_deinit_c(ifx_Cube_C_t* cube);

/**
 * @brief Frees memory for a real cube defined by \ref ifx_cube_create_r
 *        and sets the cube elements to zero.
 *
 * @param [in,out] cube      Pointer to an allocated cube instance defined
 *                           by \ref ifx_Cube_R_t.
 */
IFX_DLL_PUBLIC
void ifx_cube_destroy_r(ifx_Cube_R_t* cube);

/**
 * @brief Frees memory for a complex cube defined by \ref ifx_cube_create_r
 *        and sets the cube elements to zero.
 *
 * @param [in,out] cube      Pointer to an allocated cube instance defined
 *                           by \ref ifx_Cube_C_t.
 */
IFX_DLL_PUBLIC
void ifx_cube_destroy_c(ifx_Cube_C_t* cube);

/**
 * @brief Copy content of cube to target
 *
 * @param [in]  cube    Pointer to real cube
 * @param [in]  target  Pointer to real cube
 */
IFX_DLL_PUBLIC
void ifx_cube_copy_r(const ifx_Cube_R_t* cube, ifx_Cube_R_t* target);

/**
 * @brief Copy content of cube to target
 *
 * @param [in]  cube    Pointer to real cube
 * @param [in]  target  Pointer to real cube
 */
IFX_DLL_PUBLIC
void ifx_cube_copy_c(const ifx_Cube_C_t* cube, ifx_Cube_C_t* target);

/**
 * @brief Clones a real cube
 *
 * @param [in]  cube    Pointer to real cube
 *
 * @return Cloned cube
 */
IFX_DLL_PUBLIC
ifx_Cube_R_t* ifx_cube_clone_r(const ifx_Cube_R_t* cube);

/**
 * @brief Clones a complex cube
 *
 * @param [in]  cube    Pointer to complex cube
 *
 * @return Cloned cube
 */
IFX_DLL_PUBLIC
ifx_Cube_C_t* ifx_cube_clone_c(const ifx_Cube_C_t* cube);

/**
 * @brief Returns a slice of the a real cube in form
 *        of a matrix of type \ref ifx_Matrix_R_t
 *
 * @param [in]     cube                Pointer to real cube which slice shall be returned.
 * @param [in]     slice_index         index of slice to be returned.
 * @param [out]    slice               real matrix output representing the slice.
 *
 */
IFX_DLL_PUBLIC
void ifx_cube_get_slice_r(ifx_Cube_R_t* cube,
                          uint32_t slice_index,
                          ifx_Matrix_R_t* slice);

/**
 * @brief Returns a slice of the a complex cube in form
 *        of a matrix of type \ref ifx_Matrix_C_t
 *
 * @param [in]     cube                Pointer to real cube which slice shall be returned.
 * @param [in]     slice_index         Index of slice to be returned.
 * @param [out]    slice               Complex matrix output representing the slice.
 *
 */
IFX_DLL_PUBLIC
void ifx_cube_get_slice_c(ifx_Cube_C_t* cube,
                          uint32_t slice_index,
                          ifx_Matrix_C_t* slice);

/**
 * @brief Returns a real matrix extracted of a specified column, absolute values,
 *        of the a complex cube defined by \ref ifx_Matrix_R_t.
 *
 * @param [in]     cube                Pointer to real cube which slice shall be returned.
 * @param [in]     column_index        Index of slice to be returned.
 * @param [out]    matrix              Real matrix output containing absolute values of the
 *                                     selected complex matrix of the chosen column index.
 *
 */
IFX_DLL_PUBLIC
void ifx_cube_col_abs_r(ifx_Cube_C_t* cube,
                        uint32_t column_index,
                        ifx_Matrix_R_t* matrix);

/**
 * @brief Clears all elements of real cube defined by \ref ifx_Cube_R_t.
 *
 * @param [in]     cube      Pointer to real cube to be cleared.
 * 
 */
IFX_DLL_PUBLIC
void ifx_cube_clear_r(ifx_Cube_R_t* cube);

/**
 * @brief Clears all elements of complex cube defined by \ref ifx_Cube_C_t.
 *
 * @param [in]     cube      Pointer to complex cube to be cleared.
 * 
 */
IFX_DLL_PUBLIC
void ifx_cube_clear_c(ifx_Cube_C_t* cube);

/**
  * @}
  */
 
/**
  * @}
  */ 

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* IFX_BASE_CUBE_H */
