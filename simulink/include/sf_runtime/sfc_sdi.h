/* Copyright 2013-2014 The MathWorks, Inc. */

#ifndef _sf_runtime_sf_sdi_h_
#define _sf_runtime_sf_sdi_h_

#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#include "sf_runtime_spec.h"

typedef enum {
    SF_DATA_TYPE_BEGIN = 0,
    SF_DATA_TYPE_DOUBLE = SF_DATA_TYPE_BEGIN,
    SF_DATA_TYPE_SINGLE,
    SF_DATA_TYPE_INT8,
    SF_DATA_TYPE_UINT8,
    SF_DATA_TYPE_INT16,
    SF_DATA_TYPE_UINT16,
    SF_DATA_TYPE_INT32,
    SF_DATA_TYPE_UINT32,
    SF_DATA_TYPE_BOOLEAN,
    SF_DATA_TYPE_LAST
} SF2SdiBuiltInDataTypes;

typedef enum {
    SF_DIMENSIONS_BEGIN = 0,
    SF_DIMENSIONS_MODE_FIXED = SF_DIMENSIONS_BEGIN,
    SF_DIMENSIONS_MODE_VARIABLE,
    SF_DIMENSIONS_LAST
} SF2SdiDimsMode;

typedef enum {
    SF_COMPLEXITY_BEGIN = 0,
    SF_REAL = SF_COMPLEXITY_BEGIN,
    SF_COMPLEX,
    SF_COMPLEXITY_LAST
} SF2SdiComplexity;

typedef enum {
    SF_SAMPLE_TIME_BEGIN = 0,
    SF_SAMPLE_TIME_CONTINUOUS = SF_SAMPLE_TIME_BEGIN,
    SF_SAMPLE_TIME_DISCRETE,
    SF_SAMPLE_TIME_LAST
} SF2SdiUpdateMethod;

LIBMWSF_RUNTIME_API void sdi_database_initialize(SimStruct* S);
LIBMWSF_RUNTIME_API void sdi_database_terminate(SimStruct* S);

LIBMWSF_RUNTIME_API void* sdi_register_self_activity_signal(
    SimStruct* S,
    char* targetName, char* ssid);

LIBMWSF_RUNTIME_API void* sdi_register_child_activity_signal(
    SimStruct* S,
    char* targetName,
    char* parentStateRelativePath,
    int numChildStates, char* stateNames[], int* values, int signalSize);

LIBMWSF_RUNTIME_API void* sdi_register_leaf_activity_signal(
    SimStruct* S,
    char* targetName,
    char* parentStateRelativePath,
    int numLeafStates, char* stateNames[], int* values, int signalSize);

LIBMWSF_RUNTIME_API void* sdi_register_enum_data_type_signal(
    SimStruct* S,
    char* targetName,
    char* parentStateRelativePath,
    char* enumTypeName,
    int numLiterals,
    char* literals[],
    int* enumValues,
    int storageSize, int isSigned,
    int numDims, int* dimsArray,
    int uMethod,
    int dimsMode,
    int complexity);

LIBMWSF_RUNTIME_API void* sdi_register_builtin_data_type_signal(
    SimStruct* S,
    char* targetName,
    char* parentStateRelativePath,
    int numDims,
    int* dims,
    int updateMethod,
    int typ,
    int dimsMode,
    int complexity);

LIBMWSF_RUNTIME_API void sdi_stream_signal(SimStruct* S, void* queueH, void* data);
LIBMWSF_RUNTIME_API void sdi_update_self_activity_signal(SimStruct* S, void* queueH, int value);
LIBMWSF_RUNTIME_API void sdi_update_leaf_activity_signal(SimStruct* S, void* queueH, int value);

LIBMWSF_RUNTIME_API void sdi_write_last_self_activity_signal(SimStruct* S, void* queueH);
LIBMWSF_RUNTIME_API void sdi_write_last_leaf_activity_signal(SimStruct* S, void* queueH);

#endif

