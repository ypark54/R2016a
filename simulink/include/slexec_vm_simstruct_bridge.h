#ifndef SLEXEC_VM_SIMSTRUCT_BRIDGE_H
#define SLEXEC_VM_SIMSTRUCT_BRIDGE_H
/**
 * @file slexec_vm_simstruct_bridge.h
 *
 * This header is the exported C interface of the SimStruct and related
 * data structures, for use in VM applications.
 * 
 */

/* Copyright 2015 The MathWorks, Inc. */

#include "tmwtypes.h"

#ifdef BUILDING_SLEXEC_SIMBRIDGE
 /* being included from inside slexec_simbridge module */
  #include "package.h"
  #define SLEXEC_SIMBRIDGE_PUBLISHED_C extern "C" DLL_EXPORT_SYM
#else
 /* being included from outside, such as raccel code */
  #if defined(__cplusplus)
 /* Needed for non-inlined C++ S-functions */
    #define SLEXEC_SIMBRIDGE_PUBLISHED_C extern "C"
  #else 
    #define SLEXEC_SIMBRIDGE_PUBLISHED_C extern
  #endif
#endif

SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_ssCallAccelRunBlock(
    void* S, int_T sysIdx, int_T blkIdx, int_T blkFcn);
SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_slmrAccelRunBlockSystemInitialize(
    void* S, int_T sysIdx, int_T blkIdx);
SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_slmrAccelRunBlockSystemReset(
    void* S, int_T sysIdx, int_T blkIdx);
SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_ssCallAccelModelBlockFcnCallInput(
    void* S, int_T sysIdx, int_T blkIdx, int_T portIdx, int_T tid, int_T blkFcn);
SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_ssCallAccelCopyCacheForIIS(
    void* S, int_T sysIdx, int_T fromCache);
SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_ssCallAccelSetDims(
    void* S, int_T sysIdx, int_T blkIdx, int_T outIdx, int_T ruleIdx);

SLEXEC_SIMBRIDGE_PUBLISHED_C void* vm_ssGetRootDWork(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C void* vm__ssGetModelBlockIO(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C const void* vm_ssGetConstBlockIO(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C double* vm_ssGetModelRtp(void* S);

SLEXEC_SIMBRIDGE_PUBLISHED_C void* vm_ssGetU(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C void* vm_ssGetUByIndex(void* S, int_T index);
SLEXEC_SIMBRIDGE_PUBLISHED_C void* vm_ssGetX(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C void* vm_ssGetdX(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C void* vm_ssGetAbsTolVector(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C void* vm_ssGetY(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C void* vm_ssGetYByIndex(void* S, int_T index);

SLEXEC_SIMBRIDGE_PUBLISHED_C int vm_ssIsSampleHit(void* S, int_T sti,
                                                  int_T tid);
SLEXEC_SIMBRIDGE_PUBLISHED_C void vm__ssSetSampleHit(void* S, int_T sti,
                                                     int_T v);
SLEXEC_SIMBRIDGE_PUBLISHED_C int vm_ssIsSpecialSampleHit(void* S, int_T my_sti,
                                                         int_T promoted_sti,
                                                         int_T tid);

SLEXEC_SIMBRIDGE_PUBLISHED_C uint32_T vm_ssGetClockTick(void* S, int_T sti);
SLEXEC_SIMBRIDGE_PUBLISHED_C uint32_T vm_ssGetClockTickH(void* S, int_T sti);

SLEXEC_SIMBRIDGE_PUBLISHED_C double* vm_ssGetSolverZcSignalVector(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C uint8_T* vm_ssGetPrevZCSigState(void* S);

SLEXEC_SIMBRIDGE_PUBLISHED_C int_T vm_ssIsMajorTimeStep(void* S);

SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_ssSetSolverNeedsReset(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_ssSetStopRequested(void* S, int_T val);

SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_srClearBC(int8_T* state);
SLEXEC_SIMBRIDGE_PUBLISHED_C void  vm_srUpdateBC(int8_T* state);

SLEXEC_SIMBRIDGE_PUBLISHED_C double vm_ssGetT(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C double vm_ssGetTaskTime(void* S, int_T sti);
SLEXEC_SIMBRIDGE_PUBLISHED_C void vm__ssSetTaskTime(void* S, int_T sti,
                                                    double time);
SLEXEC_SIMBRIDGE_PUBLISHED_C double vm_ssGetTStart(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C double vm_ssGetTFinal(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C double vm_ssGetTimeOfLastOutput(void* S);

SLEXEC_SIMBRIDGE_PUBLISHED_C double vm__ssGetVarNextHitTime(void* S,
                                                            int_T varId);
SLEXEC_SIMBRIDGE_PUBLISHED_C void vm__ssSetVarNextHitTime(void* S, int_T varId,
                                                          double tNextMin);
SLEXEC_SIMBRIDGE_PUBLISHED_C uint8_T
    vm_ssGetTNextWasAdjusted(void* S, int_T varTsId);

SLEXEC_SIMBRIDGE_PUBLISHED_C uint8_T* vm_ssGetContStateDisabled(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_ssSetBlkStateChange(void* S);

SLEXEC_SIMBRIDGE_PUBLISHED_C uint8_T vm_ssIsFirstInitCond(void* S);

SLEXEC_SIMBRIDGE_PUBLISHED_C const char_T* vm_ssGetErrorStatus(void* S);
SLEXEC_SIMBRIDGE_PUBLISHED_C uint8_T vm_getHasError(void* S);

SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_ReadLocalDSM(void *S, int_T dsmIdx, const char_T *blockPath, const char_T* idx, int_T idxWidth);
SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_WriteLocalDSM(void *S, int_T dsmIdx, const char_T *blockPath, const char_T* idx, int_T idxWidth);
SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_ReadGlobalDSM(void *S, int_T dsmIdx, const char_T *blockPath, const char_T* idx, int_T idxWidth);
SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_WriteGlobalDSM(void *S, int_T dsmIdx, const char_T *blockPath, const char_T* idx, int_T idxWidth);

SLEXEC_SIMBRIDGE_PUBLISHED_C void vm_ssDummy();

#endif
