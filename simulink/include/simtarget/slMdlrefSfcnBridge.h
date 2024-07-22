/* Copyright 2013-2015 The MathWorks, Inc. */

#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#ifndef SLMDLREFSFCNBRIDGE_H
#define SLMDLREFSFCNBRIDGE_H

#include "simulink_spec.h"

SIMULINK_EXPORT_EXTERN_C void slmrPortSampleTimeMapAddStructs(
    SimStruct* S, int numStructs, size_t* allStructs);

SIMULINK_EXPORT_EXTERN_C void slmrSetDataTypeOverrideSettings(
    SimStruct* sfcnS, int dataTypeOverrideMode,
    int dataTypeOverrideAppliesTo );

SIMULINK_EXPORT_EXTERN_C void slmrSetDataDictionarySet(SimStruct* sfcnS, const char* ddSetString);

SIMULINK_EXPORT_EXTERN_C void slmrSetIsInlineParamsOn(SimStruct* S, bool isInlineParamsOn);

SIMULINK_EXPORT_EXTERN_C void slmrSetRootHardwareSemantics(SimStruct* S, bool useHardwareSemantics);

SIMULINK_EXPORT_EXTERN_C void slmrSetHasPotentialStates(SimStruct* S, bool hasPotentialStates);

SIMULINK_EXPORT_EXTERN_C void slmrSetHasHardwareSemantics(SimStruct* S, bool hasHardwareSemantics);

SIMULINK_EXPORT_EXTERN_C void slmrSetHasNonVirtualConstantTs(SimStruct* S, bool hasNonVirtualConstantTs);

SIMULINK_EXPORT_EXTERN_C void slmrSetUnionTsContainedTs(SimStruct* S,
                                            int unionTsIdx,
                                            int containedTsIdx);

SIMULINK_EXPORT_EXTERN_C int_T slmrGetParentTidFromExpFcnMdlChildTid(SimStruct* S,
                                                         int_T      childTid);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefFcnCallPortGroupSetSpecifiedTs(SimStruct* S,
                                                             double     period,
                                                             double     offset);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefFcnCallPortGroupSetIsPeriodicFcnCall(
    SimStruct* S, bool val);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefFcnCallPortGroupAddPortGroupWithDataTransferConnection(
    SimStruct* S, int_T portGroupIdx);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefFcnCallPortGroupAddMuxedPortGroup(
    SimStruct* S, int_T portGroupIdx);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefFcnCallPortGroupSetSchedulingPriority(
    SimStruct* S, int_T schedulingPriority);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefSetNumInportFcnCallPortGroups(
    SimStruct* S, int_T val);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefFcnCallPortGroupAddCallerName(
    SimStruct* S, const char *callerName, const char *fullPath);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefFcnCallPortGroupSetSimulinkFunctionName(
    SimStruct* S, const char *SimulinkFunctionName, const char *SimulinkFunctionFullPath);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefFcnCallPortGroupSetIsSimulinkFunction(
    SimStruct* S, bool val);

SIMULINK_EXPORT_EXTERN_C void slmrSetHasDataLoggedInLegacyFormat(SimStruct* S,
                                                     boolean_T  val);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefInitPortGroupsInSameRate(SimStruct* S);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefPortGroupsInSameRateAddMergeGroup(
    SimStruct* S, const char* rootMergeBlkName);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefPortGroupsInSameRateAddMergedPortGroup(
    SimStruct* S, int portGroupIdx);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefPortGroupsInSameRateInitDSMEntry(
    SimStruct* S, const char* dsmName);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefPortGroupsInSameRateAddDSMPortGroupIdx(
    SimStruct* S, int portGroupIdx);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefPortGroupsInSameRateInitGlobalDSMEntry(
    SimStruct* S, const char* dsmName);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefPortGroupsInSameRateAddGlobalDSMPortGroupIdx(
    SimStruct* S, int portGroupIdx);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefAddCompTsOfGlobalDSMAccessedByDescExpFcnMdlToHash(
    SimStruct  *S,
    const char *dsmName,
    double      period,
    double      offset,
    const char *descExpFcnMdlPath);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefSetHasDescExpFcnMdl(SimStruct* S,
                                                              boolean_T  val);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefSetOutputPortDrivenByNonCondExecStateflow(
    SimStruct* S, int portIdx, boolean_T val);

SIMULINK_EXPORT_EXTERN_C void slmrModelRefSetOutputPortDrivenByResetITVS(
    SimStruct* S, int portIdx, boolean_T val);

SIMULINK_EXPORT_EXTERN_C void slmrSetHasNonBuiltinLoggedState(
    SimStruct* S, const size_t numModels, const char **modelNames);

typedef void (*mdlSystemInitializeFcn)(SimStruct *S);
typedef void (*mdlSystemResetFcn)(SimStruct *S);

SIMULINK_EXPORT_EXTERN_C void slmrRegisterSystemInitializeMethod(
    SimStruct* S, mdlSystemInitializeFcn fcn);

SIMULINK_EXPORT_EXTERN_C void slmrRegisterSystemResetMethod(
    SimStruct* S, mdlSystemResetFcn fcn);

SIMULINK_EXPORT_EXTERN_C void slmrAccelRunBlockSystemInitialize(
    SimStruct *S, int sysidx, int blkidx);

SIMULINK_EXPORT_EXTERN_C void slmrAccelRunBlockSystemReset(
    SimStruct *S, int sysidx, int blkidx);
SIMULINK_EXPORT_EXTERN_C void slmrModelRefSetHasBlocksWithCustomSimState(
    SimStruct* S, boolean_T  val);
SIMULINK_EXPORT_EXTERN_C void slmrModelRefRegisterSimStateChecksum(
    SimStruct* S, const char* mdlname, const uint32_T* chksum);
SIMULINK_EXPORT_EXTERN_C void slmrSetModelRefLoggingSaveFormat(
    SimStruct *S, int format);

SIMULINK_EXPORT_EXTERN_C void slmrSetModelRefVirtualBusInport(
    SimStruct *S, int);

SIMULINK_EXPORT_EXTERN_C void slmrSetModelRefVirtualBusOutport(
    SimStruct *S, int);

SIMULINK_EXPORT_EXTERN_C void slmrSetModelRefOriginalInportBusType(
    SimStruct *S, int, const char*);

SIMULINK_EXPORT_EXTERN_C void slmrSetModelRefOriginalOutportBusType(
    SimStruct *S, int, const char*);

SIMULINK_EXPORT_EXTERN_C void slmrInitializeIOPortDataVectors(
    SimStruct *S, int nIPorts, int nOPorts);

SIMULINK_EXPORT_EXTERN_C void slmrRegisterTypeReplacement(
    SimStruct* S, const char* aStructTypeName, const char* aBusTypeName);

SIMULINK_EXPORT_EXTERN_C void slmrSetSparseJacobianDataFileName(SimStruct* S, const char * dataFile);

SIMULINK_EXPORT_EXTERN_C void slmrSetModelRefMaxFreqHz(
    SimStruct *S, double);

SIMULINK_EXPORT_EXTERN_C void slmrSetModelRefAutoSolverStatusFlags(
    SimStruct *S, int );

#endif
