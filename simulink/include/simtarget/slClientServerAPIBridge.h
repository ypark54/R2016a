/* Copyright 2013-2014 The MathWorks, Inc. */

#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#ifndef SLCLIENTSERVERAPIBRIDGE_H
#define SLCLIENTSERVERAPIBRIDGE_H

#ifdef __cplusplus
#include <string>
#include <vector>
#endif

#include "simulink_spec.h"

typedef struct _ssFcnCallArgInfo_tag {
    DimsInfo_T *dimsInfo;
    int_T      dataType;
    int_T      argumentType;
    int_T      complexSignal;
} _ssFcnCallArgInfo;

typedef struct _ssFcnCallStatusArgInfo_tag {
    boolean_T  returnStatus;
    int_T      dataType;
} _ssFcnCallStatusArgInfo;

typedef struct _ssFcnCallExecArgInfo_tag {
    void       *dataPtr;
    int_T      dataSize;
} _ssFcnCallExecArgInfo;

typedef struct _ssFcnCallExecArgs_tag {
    int_T                  numInArgs;
    int_T                  numOutArgs;
    _ssFcnCallExecArgInfo  *inArgs;
    _ssFcnCallExecArgInfo  *outArgs;
    _ssFcnCallExecArgInfo  *statusArg;
} _ssFcnCallExecArgs;

typedef struct _ssFcnCallInfo_tag {
    int_T                    numInArgs;
    int_T                    numOutArgs;
    int_T                    numInOutArgs;
    int_T                    numCallers;
    _ssFcnCallArgInfo        *inArgs;
    _ssFcnCallArgInfo        *outArgs;
    int_T                    *inOutArgs;
    int_T                    *inArgsSymbDims;
    int_T                    *outArgsSymbDims;
    int_T                    returnArgIndex;
    const char               *returnArgName;
    const char               **callerBlockPaths;
    const char               **argNames;
    const char               **argIndices;
    struct {
        unsigned int canBeInvokedConcurrently : 1;
    } flags;
} _ssFcnCallInfo;

typedef void (*SimulinkFunctionPtr)(SimStruct *S, int tid, _ssFcnCallExecArgs *args);

SIMULINK_EXPORT_EXTERN_C void slcsInitFcnCallInfo(_ssFcnCallInfo   *info,
                                                  int_T             numInArgs,
                                                  int_T             numOutArgs,
                                                  _ssFcnCallArgInfo *inArgs,
                                                  _ssFcnCallArgInfo *outArgs);

SIMULINK_EXPORT_EXTERN_C _ssFcnCallExecArgs *slcsCreateFcnCallExecArgs(int_T numInArgs,
                                                                       int_T numOutArgs);

SIMULINK_EXPORT_EXTERN_C void slcsDestroyFcnCallExecArgs(_ssFcnCallExecArgs *execArgs);

SIMULINK_EXPORT_EXTERN_C void slcsSetCanBeInvokedConcurrently(_ssFcnCallInfo *info, boolean_T val);
SIMULINK_EXPORT_EXTERN_C void slcsSetCallerBlockPaths(_ssFcnCallInfo *info,
                                                      int_T           nCallers,
                                                      const char    **callerBlockPaths);
SIMULINK_EXPORT_EXTERN_C void slcsSetReusedInOutArgs(_ssFcnCallInfo *info,
                                                     int_T  nInOutArgs,
                                                     int_T *inOutArgs);

SIMULINK_EXPORT_EXTERN_C void slcsSetSymbolicDims(_ssFcnCallInfo *info,
                                                  int_T *arginSymbDims,
                                                  int_T *argoutSymbDims);

SIMULINK_EXPORT_EXTERN_C void slcsSetArgumentIndices(_ssFcnCallInfo *info,
                                                     const char **argNames,
                                                     const char **argIndices);

SIMULINK_EXPORT_EXTERN_C void slcsSetReturnArgIndex(_ssFcnCallInfo *info,
                                                    int_T returnArgIndex);
SIMULINK_EXPORT_EXTERN_C void slcsSetReturnArgName(_ssFcnCallInfo *info,
                                                    const char * returnArgName);

SIMULINK_EXPORT_EXTERN_C bool slcsHasSimulinkFunctionsDefined(SimStruct* S);

#ifdef __cplusplus
SIMULINK_EXPORT_EXTERN_C void slcsGetDefinedSimulinkFunctions(SimStruct* S, std::vector<std::string> * slFcnList);
#endif

SIMULINK_EXPORT_EXTERN_C void slcsRegisterSimulinkFunction(
    SimStruct* S, const char * fcnName, SimulinkFunctionPtr fcnPtr,
    _ssFcnCallInfo callInfo, const char * relativePathToFunction);

SIMULINK_EXPORT_EXTERN_C void slcsRequestService(
    SimStruct* S, const char * fcnName, _ssFcnCallExecArgs args);

SIMULINK_EXPORT_EXTERN_C void slcsInvokeSimulinkFunction(
    SimStruct* S, const char * fcnName, _ssFcnCallExecArgInfo *args);

SIMULINK_EXPORT_EXTERN_C  void slcsRegisterCallerBlock(
    SimStruct* S, const char * fcnName, _ssFcnCallInfo callInfo,
    const char * relativePathToCaller);

SIMULINK_EXPORT_EXTERN_C void slcsRegisterCallgraph(SimStruct *S,
                                                    const char *callerFcnName,
                                                    const char *relativePathToFunction,
                                                    const char *calledFcnName,
                                                    const char *calledFcnPath,
                                                    const char *callerInfo);

SIMULINK_EXPORT_EXTERN_C const void *slcsGetSimulinkFunctionPathChecksum(SimStruct  *S,
                                                                         const char *fcnName);

SIMULINK_EXPORT_EXTERN_C _ssFcnCallInfo slcsGetFcnCallInfo(
    SimStruct* S, const char * fcnName);

SIMULINK_EXPORT_EXTERN_C void slcsFreeFcnCallInfo(
    _ssFcnCallInfo callInfo);

SIMULINK_EXPORT_EXTERN_C void slcsUpdateServerSFcnCatalog(
    SimStruct* S, const char * fcnName, void * fPtr);

SIMULINK_EXPORT_EXTERN_C bool slcsIsFunctionRegistered(
    SimStruct* S, const char * fcnName);

SIMULINK_EXPORT_EXTERN_C bool slcsIsFunctionRegisteredWithModel(
    double modelHandle, double blockHandle, const char * fcnName);

SIMULINK_EXPORT_EXTERN_C const char * slcsGetScopedFcnName(
    SimStruct* S, const char * fcnName);

SIMULINK_EXPORT_EXTERN_C
void slcgxeBlkRequestService(SimStruct* S, const char* fcnName, const char* fullPath, int numInputs,
                          void* inArgs, void* inSizes, int numOutputs,
                         void* outArgs, int* outSizes);
#endif
