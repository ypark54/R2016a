/******************************************************************
 *
 *  File: raccel_utils.c
 *
 *
 *  Abstract:
 *      - provide utility functions for rapid accelerator 
 *
 * Copyright 2007-2015 The MathWorks, Inc.
 ******************************************************************/

/* INCLUDES */
#include  <stdio.h>
#include  <stdlib.h>

#include  <string.h>
#include  <math.h>
#include  <float.h>
#include  <ctype.h>

/*
 * We want access to the real mx* routines in this file and not their RTW
 * variants in rt_matrx.h, the defines below prior to including simstruc.h
 * accomplish this.
 */
#include  "mat.h"
#define TYPEDEF_MX_ARRAY
#define rt_matrx_h
#include "simstruc.h"
#undef rt_matrx_h
#undef TYPEDEF_MX_ARRAY

#include "dt_info.h"
#include "rtw_capi.h"
#include "rtw_modelmap.h"
#include "common_utils.h"
#include "raccel_utils.h"

/* external variables */
extern const char  *gblParamFilename;
extern const char  *gblInportFileName;

extern const int_T gblNumToFiles;
extern FNamePair   *gblToFNamepair;

extern const int_T gblNumFrFiles;
extern FNamePair   *gblFrFNamepair;

extern int_T         gblParamCellIndex;

/* for tuning struct params using the C-API */
extern rtwCAPI_ModelMappingInfo* rt_modelMapInfoPtr;

static PrmStructData gblPrmStruct;

/* global variables */
void* gblLoggingInterval = NULL;


/*==================*
 * NON-Visible routines *
 *==================*/

/* Function: rt_FreeParamStructs ===========================================
 * Abstract:
 *      Free and NULL the fields of all 'PrmStructData' structures.
 *      PrmStructData contains an array of ParamInfo structures, with one such
 *      structure for each non-struct data type, and one such structure for 
 *      each leaf of every struct parameter. 
 */
void rt_FreeParamStructs(PrmStructData *paramStructure)
{
    if (paramStructure != NULL) {
        int          i;
        int          nNonStructDataTypes  = paramStructure->nNonStructDataTypes;
        int          nStructLeaves        = paramStructure->nStructLeaves;
        ParamInfo   *paramInfo            = paramStructure->paramInfo;

        if (paramInfo != NULL) {
            for (i=0; i < nNonStructDataTypes+nStructLeaves; i++) {
                /*
                 * Must free "stolen" parts of matrices with
                 * mxFree (they are allocated with mxCalloc).
                 */
                mxFree(paramInfo[i].rVals);
                mxFree(paramInfo[i].iVals);
            }
            free(paramInfo);
        }

        paramStructure->nTrans              = 0;
        paramStructure->nNonStructDataTypes = 0;
        paramStructure->nStructLeaves       = 0;
        paramStructure->paramInfo           = NULL;
    }
} /* end rt_FreeParamStructs */



/* Function: rt_GetNumStructLeaves ============================
 * Abstract:
 *  Recursive function to traverse a parameter structure and return the
 *  number of leaves needing a 'paramInfo'.  Initially called by
 *  rt_GetNumStructLeavesAndNumNonStructDataTypes() whenever a struct 
 *  parameter is encountered in the rtp MATFile, then recursively called 
 *  while processing the structure to find all of the leaf elements.
 */
int rt_GetNumStructLeaves(uint16_T dTypeMapIdx, uint16_T dimMapIdx, uint16_T fixPtIdx)
{
    int     i;
    int     locNParamStructs = 0;
    uint8_T numDims;
    uint_T  dimArrayIdx;
    uint_T  numArrayElements = 1;

    const rtwCAPI_ModelMappingInfo *mmi      = rt_modelMapInfoPtr;
    const rtwCAPI_DataTypeMap      *dTypeMap = rtwCAPI_GetDataTypeMap(mmi);
    const rtwCAPI_ElementMap       *elemMap  = rtwCAPI_GetElementMap(mmi);
    const rtwCAPI_DimensionMap     *dimMap   = rtwCAPI_GetDimensionMap(mmi);
    const rtwCAPI_FixPtMap         *fixPtMap = rtwCAPI_GetFixPtMap(mmi);
    const uint_T                   *dimArray = rtwCAPI_GetDimensionArray(mmi);

    uint16_T dTypeMapNumElems   = rtwCAPI_GetDataTypeNumElements(dTypeMap, dTypeMapIdx);
    uint16_T dTypeMapElemMapIdx = rtwCAPI_GetDataTypeElemMapIndex(dTypeMap, dTypeMapIdx);

    /* found either a non-struct leaf (dTypeMapNumElems == 0) or a struct leaf that is a
     * fixed-point value */
    if (dTypeMapNumElems==0 || fixPtIdx > 0) {
        return(1);
    }

    /* found a non-fixed-point struct field; recurse */
    for (i=0; i<dTypeMapNumElems; i++) {
        uint16_T elDTypeMapIdx = rtwCAPI_GetElementDataTypeIdx(elemMap, dTypeMapElemMapIdx+i);
        uint16_T elDimMapIdx   = rtwCAPI_GetElementDimensionIdx(elemMap, dTypeMapElemMapIdx+i);
        uint16_T elFixPtIdx    = rtwCAPI_GetElementFixPtIdx(elemMap, dTypeMapElemMapIdx+i);
        locNParamStructs += rt_GetNumStructLeaves(elDTypeMapIdx, elDimMapIdx, elFixPtIdx);
    }

    /* at this point, locNParamStructs will contain the number of leaves in a single element of 
     * the current struct array. multiplying locNParamStructs by the number of elements in the
     * current struct array will give the total number of leaves. */
    numDims          = rtwCAPI_GetNumDims(dimMap, dimMapIdx);
    dimArrayIdx      = rtwCAPI_GetDimArrayIndex(dimMap, dimMapIdx);
    for (i=0; i<numDims; ++i) {
        numArrayElements *= dimArray[dimArrayIdx + i];
    }
    
    return(locNParamStructs * numArrayElements);

} /* end rt_GetNumStructLeaves */




/* Function: rt_GetNumStructLeavesAndNumNonStructDataTypes ===================================
 * Abstract:
 *   Count the number of non-struct data types, and the number of leaves of all struct
 *   parameters.
 */
void rt_GetNumStructLeavesAndNumNonStructDataTypes(const mxArray *paParamStructs,
                                                  int nTrans,
                                                  int *numStructLeaves,
                                                  int *numNonStructDataTypes,
                                                  const char **result)
{
    int idx;
    rtwCAPI_ModelMappingInfo      *mmi        = rt_modelMapInfoPtr;
    const rtwCAPI_BlockParameters *blkPrms    = rtwCAPI_GetBlockParameters(mmi);
    const rtwCAPI_ModelParameters *mdlPrms    = rtwCAPI_GetModelParameters(mmi);

    *numStructLeaves       = 0;
    *numNonStructDataTypes = 0;

    for (idx=0; idx<nTrans; idx++) {
        mxArray *valueMat = NULL;
        valueMat = mxGetField(paParamStructs,idx,"values");
        
        /* 
         * If the value field of a parameter in the RTP is empty, count the parameter
         * as a non-struct datatype (struct parameters must have non-empty value fields).
         * For struct / bus parameters, and only for those types of parameters, 
         * rtp.parameters.values is a cell array.
         */
        if (valueMat == NULL || !mxIsCell(valueMat)) {
            /* new non-struct data type found */
            (*numNonStructDataTypes)++;
        } else {
            /* struct-type found; count all leaves of each struct parameter */
            mxArray  *structParamInfo         = NULL;
            int       parameterIdx            = 0;
            int       numParamsOfCurrentType  = 0;

            /* valueMat may be an empty cell array; if so, continue */
            mxArray  *paramValue = mxGetCell(valueMat, 0);
            if (paramValue == NULL) {
                continue;
            }

            /* structParamInfo (contains c-api index) */
            structParamInfo = mxGetField(paParamStructs, idx, "structParamInfo");
            if (structParamInfo == NULL) {
                *(result) = "The RTP entry for struct parameters must have a nonempty structParamInfo field.";
            }

            if (mxGetN(valueMat) != mxGetN(structParamInfo)) {
                *(result) = "Invalid rtp: the length of the structParamInfo field must equal the length of the values field.";
            }

            /* Get number of parameters of the current type */
            numParamsOfCurrentType = mxGetN(structParamInfo);

            for (parameterIdx = 0; parameterIdx < numParamsOfCurrentType; ++parameterIdx) {
                double   *pr                      = NULL;
                mxArray  *mat                     = NULL;
                int       numStructArrayElements  = 0;
                bool      modelParam              = false;
                int       capiIdx                 = -1;
                int       dTypeMapIdx             = -1;
                int       dimMapIdx               = -1;
                int       fixPtIdx                = -1;

                /* parameter type (model or block) */
                mat = mxGetField(structParamInfo, 0, "ModelParam");
                if (mat == NULL) {
                    *(result) = "The structParamInfo field for struct parameters must have a nonempty modelParam field.";
                    return;
                }
                pr  = mxGetPr(mat);
                modelParam = (bool)pr[0];

                /* c-api index */
                mat = mxGetField(structParamInfo, 0, "CAPIIdx");
                if (mat == NULL) {
                    *(result) = "The structParamInfo field for struct parameters must have a nonempty CAPIIndex field.";
                    return;
                }
                pr  = mxGetPr(mat);
                capiIdx = (int)pr[0];

                /* c-api datatype map index and dimension map index */
                dTypeMapIdx = (modelParam) ?
                    rtwCAPI_GetModelParameterDataTypeIdx(mdlPrms, capiIdx) :
                    rtwCAPI_GetBlockParameterDataTypeIdx(blkPrms, capiIdx);

                dimMapIdx = (modelParam) ?
                    rtwCAPI_GetModelParameterDimensionIdx(mdlPrms, capiIdx) :
                    rtwCAPI_GetBlockParameterDimensionIdx(blkPrms, capiIdx);
                
                fixPtIdx = (modelParam) ? 
                    rtwCAPI_GetModelParameterFixPtIdx(mdlPrms, capiIdx) :
                    rtwCAPI_GetBlockParameterFixPtIdx(blkPrms, capiIdx);


                /* 1. Get number of leaves in one struct of the current type */
            
                (*numStructLeaves) += rt_GetNumStructLeaves(dTypeMapIdx, dimMapIdx, fixPtIdx);
            }
        }
    }
} /* end rt_GetNumStructLeavesAndNumNonStructDataTypes */



/* Function: rt_CreateParamInfoForNonStructDataType ===========================
 * Abstract:
 *  Create a ParamInfo struct for a non-struct data type.
 */
void rt_CreateParamInfoForNonStructDataType(mxArray* mat, 
                                         const mxArray* paParamStructs,   
                                         const int paramStructIndex,
                                         PrmStructData *paramStructure,
                                         int paramInfoIndex)
{
    ParamInfo   *paramInfo              = NULL;
    double      *pr                     = NULL;
    paramInfo = &paramStructure->paramInfo[paramInfoIndex];
    paramInfo->structLeaf = false;

    paramInfo->elSize = (int) mxGetElementSize(mat);
    paramInfo->nEls   = (int) mxGetNumberOfElements(mat);

    paramInfo->rVals  = mxGetData(mat);
    mxSetData(mat,NULL);

    if (mxIsNumeric(mat)) {
        paramInfo->iVals  = mxGetImagData(mat);
        mxSetImagData(mat,NULL);
    }  

    /* Grab the datatype id. */
    mat = mxGetField(paParamStructs,paramStructIndex,"dataTypeId");
    pr  = mxGetPr(mat);

    paramInfo->dataType = (int)pr[0];

    /* Grab the complexity. */
    mat = mxGetField(paParamStructs,paramStructIndex,"complex");
    pr  = mxGetPr(mat);

    paramInfo->complex = (bool)pr[0];

    /* Grab the dtTransIdx */
    mat = mxGetField(paParamStructs,paramStructIndex,"dtTransIdx");
    pr  = mxGetPr(mat);

    paramInfo->dtTransIdx = (int)pr[0];
} /* end rt_CreateParamInfoForNonStructDataType */




/* Function: rt_CopyStructFromStructArray ======================================
 * Abstract:
 *   Given an mxArray of that is an array of structs, and given an index, 
 *   create a copy of the struct in the array that's at the given index.  
 *   This function assumes that the argument structArray is an mxStruct.
 */
void rt_CopyStructFromStructArray(mxArray *structArray,
                                  mxArray **destination,
                                  int structArrayIndex)
{
    const char **fieldNames      = NULL;
    int          numFields       = 0;
    int          fieldIndex      = 0;
    
    numFields = mxGetNumberOfFields(structArray);

    /*  get field names */
    fieldNames = (const char **) malloc(sizeof(const char *) * numFields);
    for (fieldIndex = 0; fieldIndex < numFields; ++fieldIndex) {
        fieldNames[fieldIndex] = mxGetFieldNameByNumber(structArray, fieldIndex);
    }

    *destination = mxCreateStructMatrix(1, 1, numFields, fieldNames);
    
    for (fieldIndex = 0; fieldIndex < numFields; ++fieldIndex) {
        mxArray *field = mxGetField(structArray, 
                                    structArrayIndex, 
                                    fieldNames[fieldIndex]);
        mxSetField(*destination, 0, fieldNames[fieldIndex], field);
    }
}


/* Function: rt_GetStructLeafInfo ================================================
 * Abstract:
 *  Recursive function to traverse the fields of a single struct parameter with
 *  value valueMat, and to create a ParamInfo structure for each leaf of the
 *  parameter.
 */
void rt_GetStructLeafInfo(uint16_T dTypeMapIdx,
                          uint16_T dimMapIdx,
                          uint16_T fixPtIdx,
                          void *baseAddr,
                          mxArray *valueMat,
                          boolean_T modelParam,
                          PrmStructData *paramStructure,
                          int *paramInfoIndex,
                          const char** result)
{
    rtwCAPI_ModelMappingInfo  *mmi              = rt_modelMapInfoPtr;
    rtwCAPI_DataTypeMap const *dTypeMap         = rtwCAPI_GetDataTypeMap(mmi);
    uint16_T                   dTypeMapNumElems = rtwCAPI_GetDataTypeNumElements(dTypeMap, dTypeMapIdx);

    /* fixPtIdx will be > 0 if and only if the parameter currently being inspected is a fixed-point
     * parameter. */
    if (dTypeMapNumElems == 0 || fixPtIdx > 0) {
        /* This is a leaf of a struct. Increment paramInfoIndex. */
        ParamInfo *paramInfo = &paramStructure->paramInfo[(*paramInfoIndex)++];
        paramInfo->structLeaf = true;

        /* Record parameter type */
        paramInfo->modelParam = modelParam;

        /* Record data type ID */
        paramInfo->dataType = dTypeMapIdx;

        /* Grab the complexity */
        paramInfo->complex = (boolean_T) rtwCAPI_GetDataIsComplex(dTypeMap, dTypeMapIdx);

        /* Set the parameter (destination) address. */
        paramInfo->prmAddr = baseAddr;

        /* Grab the data and any attributes.  We "steal" the data from the mxArray. */
        if (valueMat) {
            paramInfo->elSize = mxGetElementSize(valueMat);
            paramInfo->nEls   = mxGetNumberOfElements(valueMat);
            /*
             * mxSetImagData does not work if valueMat is not numeric (if, for example,
             * valueMat is logical). Note that the values of boolean, non-struct-leaf 
             * parameters with boolean are stored as uint8 in the rtp, while boolean,
             * struct-leaves are stored as logicals.
             *
             */
            paramInfo->rVals  = mxGetData(valueMat);
            mxSetData(valueMat,NULL);

            if (mxIsNumeric(valueMat)) {
                paramInfo->iVals  = mxGetImagData(valueMat);
                mxSetImagData(valueMat,NULL);
            } 

        } else {
            paramInfo->nEls   = 0;
            paramInfo->elSize = 0;
            paramInfo->rVals  = NULL;
            paramInfo->iVals  = NULL;
        }

        return;
    } else {
        /* This is a struct, possibly a struct array; recurse over its fields */     
        int                          i                  = 0;
        uint16_T                     dTypeMapElemMapIdx = rtwCAPI_GetDataTypeElemMapIndex(dTypeMap, dTypeMapIdx);
        rtwCAPI_ElementMap const    *elemMap            = rtwCAPI_GetElementMap(mmi);
        rtwCAPI_DimensionMap const  *dimMap             = rtwCAPI_GetDimensionMap(mmi);

        for (i=0; i<dTypeMapNumElems; i++) {       
            uint16_T         elDTypeMapIdx         = rtwCAPI_GetElementDataTypeIdx(elemMap, dTypeMapElemMapIdx+i);
            uint16_T         elOffset              = rtwCAPI_GetElementOffset(elemMap, dTypeMapElemMapIdx+i);
            uint16_T         elFixPtIdx            = rtwCAPI_GetElementFixPtIdx(elemMap, dTypeMapElemMapIdx+i);
            uint16_T         elDimMapIdx           = rtwCAPI_GetElementDimensionIdx(elemMap, dTypeMapElemMapIdx+i);
            const char_T*    fieldName             = rtwCAPI_GetElementName(elemMap, dTypeMapElemMapIdx+i);
            mxArray         *subMat                = mxGetField(valueMat, 0, fieldName);
            uint8_T          elNumDims             = rtwCAPI_GetNumDims(dimMap, elDimMapIdx);
            uint_T           elDimArrayIdx         = rtwCAPI_GetDimArrayIndex(dimMap, elDimMapIdx);
            const uint_T    *dimArray              = rtwCAPI_GetDimensionArray(mmi);
            uint16_T         elNumElems            = rtwCAPI_GetDataTypeNumElements(dTypeMap, elDTypeMapIdx);            
            uint_T           numArrayElements      = 1;
            int              dimensionLoopCounter;

            for (dimensionLoopCounter=0; dimensionLoopCounter<elNumDims; ++dimensionLoopCounter) {
                numArrayElements *= dimArray[elDimArrayIdx + dimensionLoopCounter];
            }
            
            if (numArrayElements == 1 || elFixPtIdx > 0 || elNumElems == 0) {
                /* this field is either not a struct, or is a 1x1 struct array */
                rt_GetStructLeafInfo(elDTypeMapIdx, 
                                     elDimMapIdx,
                                     elFixPtIdx,
                                     (unsigned char *) baseAddr+elOffset,
                                     subMat,
                                     modelParam,
                                     paramStructure,
                                     paramInfoIndex,
                                     result);
            } else {
                /* non 1x1 struct array */
                int j=0;
                void* localBaseAddr = baseAddr;
                for (j=0; j<numArrayElements; ++j) {
                    mxArray* subMatElement = NULL;
                    uint16_T elStructSize  = rtwCAPI_GetDataTypeSize(dTypeMap, elDTypeMapIdx);
                    rt_CopyStructFromStructArray(subMat,
                                                 &subMatElement,
                                                 j);
                    rt_GetStructLeafInfo(elDTypeMapIdx, 
                                         elDimMapIdx,
                                         elFixPtIdx, 
                                         (unsigned char *)localBaseAddr+elOffset,
                                         subMatElement,
                                         modelParam,
                                         paramStructure,
                                         paramInfoIndex,
                                         result);
                    localBaseAddr = (void*)((char*)localBaseAddr + elStructSize);                    
                }
            }
        }
    }
} /* end rt_GetStructLeafInfo */



/* Function: rt_GetInfoFromOneStructOrBusParam ==================================
 * Abstract:
 *   Create ParamInfo structures for each leaf of a struct parameter. 
 */
void rt_GetInfoFromOneStructOrBusParam(mxArray *valueMat,
                                       PrmStructData *paramStructure,
                                       int* paramInfoIndex,
                                       boolean_T modelParam,
                                       int capiIdx,
                                       const char** result)
{
    if (valueMat == NULL) {
        *(result) = "the value field of an RTP entry for some struct param is empty (rt_GetInfoFromOneStructParam)";
        return;
    }

    if (!mxIsStruct(valueMat)) {
        /* This function should deal only with struct params */
        *(result) = "non-struct param found where only struct params were expected (rt_GetInfoFromOneStructParam)";
        return;
    } else {
        int     loopIdx;
        int     addrIdx     = -1;
        void   *baseAddr    = NULL;

        rtwCAPI_ModelMappingInfo      *mmi         = rt_modelMapInfoPtr;
        rtwCAPI_BlockParameters const *blkPrms     = rtwCAPI_GetBlockParameters(mmi);
        rtwCAPI_ModelParameters const *mdlPrms     = rtwCAPI_GetModelParameters(mmi);
        void **                        addrMap     = rtwCAPI_GetDataAddressMap(mmi);
        const rtwCAPI_DataTypeMap     *dTypeMap    = rtwCAPI_GetDataTypeMap(mmi);
        const rtwCAPI_DimensionMap    *dimMap      = rtwCAPI_GetDimensionMap(mmi);
        uint16_T                       dTypeMapIdx;
        uint16_T                       dimMapIdx;

        uint_T                      numArrayElements = 1;
        uint16_T                    structSizeInBytes;
        uint16_T                    fixPtIdx;

        /* Calculate base address of this parameter. */
        addrIdx  = (modelParam) ? rtwCAPI_GetModelParameterAddrIdx(mdlPrms, capiIdx) :
            rtwCAPI_GetBlockParameterAddrIdx(blkPrms, capiIdx);
        baseAddr = rtwCAPI_GetDataAddress(addrMap, addrIdx);

        /* Get the datatype */
        dTypeMapIdx = (modelParam) ?
            rtwCAPI_GetModelParameterDataTypeIdx(mdlPrms, capiIdx) :
            rtwCAPI_GetBlockParameterDataTypeIdx(blkPrms, capiIdx);

        /* Get the dimension index */
        dimMapIdx = (modelParam) ?
            rtwCAPI_GetModelParameterDimensionIdx(mdlPrms, capiIdx) :
            rtwCAPI_GetBlockParameterDimensionIdx(blkPrms, capiIdx);

        /* Get the size of the struct */
        structSizeInBytes = rtwCAPI_GetDataTypeSize(dTypeMap, dTypeMapIdx);

        /* Get fixed-point index. This should be 0 for struct parameters. */
        fixPtIdx = modelParam ? rtwCAPI_GetModelParameterFixPtIdx(mdlPrms, capiIdx) : 
            rtwCAPI_GetBlockParameterFixPtIdx(blkPrms, capiIdx);
        if (fixPtIdx != 0) {
            *(result) = "Fixed-point index of struct params should be 0. (rt_GetInfoFromOneStructParam)";
            return;
        }

        /* Recursively traverse the struct param, creating a new ParamInfo structure for each leaf of the parameter. */
        numArrayElements = mxGetNumberOfElements(valueMat);

        if (numArrayElements == 1) {
            rt_GetStructLeafInfo(dTypeMapIdx, 
                                 dimMapIdx,
                                 fixPtIdx,
                                 baseAddr,
                                 valueMat,
                                 modelParam,
                                 paramStructure,
                                 paramInfoIndex,
                                 result);
        } else {
            void* localBaseAddr = baseAddr;
            for (loopIdx = 0; loopIdx < numArrayElements; loopIdx++) {
                mxArray* valueMatElement = NULL;
                rt_CopyStructFromStructArray(valueMat,
                                             &valueMatElement, 
                                             loopIdx);

                
                rt_GetStructLeafInfo(dTypeMapIdx, 
                                     dimMapIdx,
                                     fixPtIdx,
                                     localBaseAddr,
                                     valueMatElement,
                                     modelParam,
                                     paramStructure,
                                     paramInfoIndex,
                                     result);
                localBaseAddr = (void*)((char*)localBaseAddr+ structSizeInBytes);
            }
        }
    }
}  /* end rt_GetInfoFromOneStructOrBusParam */


/* Function: rt_CreateParamInfosForStructOrBusType ===========================
 * Abstract:
 *  Create a ParamInfo struct for a non-struct data type.
 */
const char *rt_CreateParamInfosForStructOrBusType(mxArray* mat, 
                                                  const mxArray* paParamStructs,
                                                  const int paramStructIndex,
                                                  PrmStructData *paramStructure,
                                                  int *paramInfoIndex)
{
    const mxArray   *structParamInfo         = NULL;
    const mxArray   *temp                    = NULL;                
    mxArray         *valueMxArray            = NULL;
    int              capiIndex               = -1;
    int              structParamInfoIndex    = 0;
    int              structParamInfoLength   = 0;
    double          *pr                      = NULL;
    boolean_T        modelParam              = false;
    int              numParamsOfCurrentType  = 0;
    const char      *result                  = NULL;

    structParamInfo = mxGetField(paParamStructs,
                                 paramStructIndex,
                                 "structParamInfo");

    if (structParamInfo == NULL) {
        result = "Some struct parameter has an empty structParamInfo field.";
        return result;
    }

    structParamInfoLength = mxGetN(structParamInfo);
                
    /* rtp.parameters(i).structParamInfo and rtp.parameters(i).values should have the same length. */
    numParamsOfCurrentType = mxGetN(mat);
    if (structParamInfoLength != numParamsOfCurrentType) {
        result = "Invalid rtp format: rtp.parameters(i).structParamInfo and rtp.parameters(i).values do not have the same length, for some i.";
        return result;
    }
                
    for (structParamInfoIndex = 0; structParamInfoIndex < numParamsOfCurrentType; ++structParamInfoIndex) {
        /* Is the parameter a model parameter? */
        temp = mxGetField(structParamInfo, structParamInfoIndex, "ModelParam");
        if (temp) {
            pr = mxGetPr(temp);
            modelParam = (boolean_T)pr[0];
        } else {
            modelParam = 0;
        }

        /* C-API index (for struct params only) */
        temp = mxGetField(structParamInfo, structParamInfoIndex, "CAPIIdx");
        if (temp) {
            pr = mxGetPr(temp);
            capiIndex = (int) pr[0];
        } else {
            capiIndex = -1;
        }             

        /* struct parameter values in the rtp are contained in a cell array */
        if (mxGetM(mat) != 1) {
            result = "For struct types, rtp.parameters.values should be a cell array whose first dimension is equal to 1.";
            return result;
        }

        /* get parameter value */
        valueMxArray = mxGetCell(mat, structParamInfoIndex);
        if (valueMxArray == NULL) {
            continue;
        } else if (!mxIsStruct(valueMxArray)) {
            result = "A non-struct value was found in the rtp where a struct value was expected.";
            return result;
        }

        rt_GetInfoFromOneStructOrBusParam(valueMxArray,
                                          paramStructure,
                                          paramInfoIndex,
                                          modelParam,
                                          capiIndex,
                                          &result);

        if (result != NULL) {
            return result;
        }
    }

    return result;
} /* end rt_CreateParamInfoForStructOrBusType */



/* Function: rt_ReadParamStructMatFile=======================================
 * Abstract:
 *  Reads a matfile containing a new parameter structure.  It also reads the
 *  model checksum and compares this with the RTW generated code's checksum
 *  before inserting the new parameter structure.
 *
 * Returns:
 *	NULL    : success
 *	non-NULL: error string
 */
const char *rt_ReadParamStructMatFile(PrmStructData **prmStructOut,
                                      const SimStruct * S,
                                      int           cellParamIndex)
{
    int           paramInfoIndex      = 0;
    int           paramStructIndex    = 0;
    int           nStructLeaves       = 0;
    int           nNonStructDataTypes = 0;
    int           nTrans              = 0;
    int           nParamInfos         = 0;
    MATFile       *pmat               = NULL;
    mxArray       *pa                 = NULL;
    const mxArray *paParamStructs     = NULL;
    PrmStructData *paramStructure     = NULL;
    const char    *result             = NULL; /* assume success */

    paramStructure = &gblPrmStruct;

    /**************************************************************************
     * Open parameter MAT-file, read checksum, swap rtP data for type Double *
     **************************************************************************/

    if ((pmat=matOpen(gblParamFilename,"r")) == NULL) {
        result = "could not find MAT-file containing new parameter data";
        goto EXIT_POINT;
    }

    /*
     * Read the param variable. The variable name must be passed in
     * from the generated code.
     */
    if ((pa=matGetNextVariable(pmat,NULL)) == NULL ) {
        result = "error reading RTP from MAT-file "
            "(matGetNextVariable)";
        goto EXIT_POINT;
    }

    /* Should be 1x1 structure */
    if (!mxIsStruct(pa) ||
        mxGetM(pa) != 1 || mxGetN(pa) != 1 ) {
        result = "RTP must be a 1x1 structure";
        goto EXIT_POINT;
    }

    /* look for modelChecksum field */
    {
        const double  *newChecksum;
        const mxArray *paModelChecksum;

        if ((paModelChecksum = mxGetField(pa, 0, "modelChecksum")) == NULL) {
            result = "parameter variable must contain a modelChecksum field";
            goto EXIT_POINT;
        }

        /* check modelChecksum field */
        if (!mxIsDouble(paModelChecksum) || mxIsComplex(paModelChecksum) ||
            mxGetNumberOfDimensions(paModelChecksum) > 2 ||
            mxGetM(paModelChecksum) < 1 || mxGetN(paModelChecksum) !=4 ) {
            result = "invalid modelChecksum in parameter MAT-file";
            goto EXIT_POINT;
        }

        newChecksum = mxGetPr(paModelChecksum);

        paramStructure->checksum[0] = newChecksum[0];
        paramStructure->checksum[1] = newChecksum[1];
        paramStructure->checksum[2] = newChecksum[2];
        paramStructure->checksum[3] = newChecksum[3];
    }

    /* be sure checksums all match */
    if (paramStructure->checksum[0] != ssGetChecksum0(S) ||
        paramStructure->checksum[1] != ssGetChecksum1(S) ||
        paramStructure->checksum[2] != ssGetChecksum2(S) ||
        paramStructure->checksum[3] != ssGetChecksum3(S) ) {
        result = "model checksum mismatch - incorrect parameter data "
            "specified";
        goto EXIT_POINT;
    }


    /*
     * Get the "parameters" field from the structure.  It is an
     * array of structures.
     */
    if ((paParamStructs = mxGetField(pa, 0, "parameters")) == NULL) {
        goto EXIT_POINT;
    }

    /*
     * If the parameters field is a cell array then pick out the cell
     * array pointed to by the cellParamIndex
     */
    if ( mxIsCell(paParamStructs) ) {
        /* check that cellParamIndex is in range */
        size_t size = mxGetM(paParamStructs) * mxGetN(paParamStructs);
        if (cellParamIndex > 0 && cellParamIndex <= (int) size){
            paParamStructs = mxGetCell(paParamStructs, cellParamIndex-1);
        }else{
            result = "Invalid index into parameter cell array";
            goto EXIT_POINT;
        }
        if (paParamStructs == NULL) {
            result = "Invalid parameter field in parameter structure";
            goto EXIT_POINT;
        }
    }

    /* the number of data-types in the RTP */
    nTrans = (int) mxGetNumberOfElements(paParamStructs);
    if (nTrans == 0) goto EXIT_POINT;

    /*
     * Validate the array fields - only check the first element of the
     * array since all elements of a structure array have the same
     * fields.
     *
     * It is assumed that if the proper data fields exists, that the
     * data is correct.
     */
    {
        mxArray *dum;

        if ((dum = mxGetField(paParamStructs, 0, "dataTypeName")) == NULL) {
            result = "parameters struct must contain a dataTypeName field";
            goto EXIT_POINT;
        }

        if ((dum = mxGetField(paParamStructs, 0, "dataTypeId")) == NULL) {
            result = "parameters struct must contain a dataTypeId field";
            goto EXIT_POINT;
        }

        if ((dum = mxGetField(paParamStructs, 0, "complex")) == NULL) {
            result = "parameters struct must contain a complex field";
            goto EXIT_POINT;
        }

        if ((dum = mxGetField(paParamStructs, 0, "dtTransIdx")) == NULL) {
            result = "parameters struct must contain a dtTransIdx field";
            goto EXIT_POINT;
        }
    }

    /* 
     * Calculate the total number of ParamInfo structures needed. Each non-struct
     * data type is given one ParamInfo, and each leaf of every struct 
     * parameter is given one ParamInfo.
     */ 
    rt_GetNumStructLeavesAndNumNonStructDataTypes(paParamStructs, 
                                                  nTrans,
                                                  &nStructLeaves,
                                                  &nNonStructDataTypes,
                                                  &result);

    if (result != NULL) goto EXIT_POINT;

    paramStructure->nTrans = nTrans;
    paramStructure->nStructLeaves = nStructLeaves;
    paramStructure->nNonStructDataTypes = nNonStructDataTypes;

    /*
     * Allocate the ParamInfo's.
     * The total number of ParamInfos needed is nStructLeaves + nNonStructDataTypes,
     * 
     */
    nParamInfos = nStructLeaves + nNonStructDataTypes;
    paramStructure->paramInfo = (ParamInfo *) calloc(nParamInfos,
                                                     sizeof(ParamInfo));
    
    if (paramStructure->paramInfo == NULL) {
        result = "Memory allocation error";
        goto EXIT_POINT;
    }

    /* Get the new parameter data for each data type. */
    for (paramStructIndex=0; paramStructIndex < nTrans; paramStructIndex++) 
    {
        mxArray     *mat                    = NULL;              
        mat = mxGetField(paParamStructs,paramStructIndex,"values");

        if (mat == NULL) {
            ParamInfo   *paramInfo              = NULL;
            paramInfo = &paramStructure->paramInfo[paramInfoIndex];
            paramInfoIndex++;
            paramInfo->nEls   = 0;
            paramInfo->elSize = 0;

            paramInfo->rVals  = NULL;
            paramInfo->iVals  = NULL;
        } else {           
            /* struct parameters, and only struct parameters, are stored in a cell array */
            if (!mxIsCell(mat)) {
                /* Non-struct data type */
                rt_CreateParamInfoForNonStructDataType(mat,
                                                       paParamStructs,
                                                       paramStructIndex,
                                                       paramStructure, 
                                                       paramInfoIndex);
                paramInfoIndex++;
            } else {
                /* This is a struct param; grab information from rtp.parameters(i).structParamInfo 
                 * paramInfoIndex is incremented inside rt_CreateParamInfosForStructOrBusType
                 */
                result = rt_CreateParamInfosForStructOrBusType(mat, 
                                                              paParamStructs,
                                                              paramStructIndex,
                                                              paramStructure,
                                                              &paramInfoIndex);
            }
        }
    } 

EXIT_POINT:
    mxDestroyArray(pa);

    if (pmat != NULL) {
        matClose(pmat); pmat = NULL;
    }

    if (result != NULL) {
        rt_FreeParamStructs(paramStructure);
        paramStructure = NULL;
    }
    *prmStructOut = paramStructure;
    return(result);
} /* end rt_ReadParamStructMatFile */






/* Function: ReplaceRtP ========================================================
 * Abstract
 *  Initialize the rtP structure using the parameters from the specified
 *  'paramStructure'.  The 'paramStructure' contains parameter info that was
 *  read from a mat file (see raccel_mat.c/rt_ReadParamStructMatFile).
 */
static const char *ReplaceRtP(const SimStruct *S,
                              const PrmStructData *paramStructure)
{
    int                     i;
    const char              *errStr                = NULL;
    const ParamInfo         *paramInfo             = paramStructure->paramInfo;
    int                     nStructLeaves          = paramStructure->nStructLeaves;
    int                     nNonStructDataTypes    = paramStructure->nNonStructDataTypes;
    const DataTypeTransInfo *dtInfo                = (const DataTypeTransInfo *)ssGetModelMappingInfo(S);
    DataTypeTransitionTable *dtTable               = dtGetParamDataTypeTrans(dtInfo);
    uint_T                  *dataTypeSizes         = dtGetDataTypeSizes(dtInfo);
    rtwCAPI_ModelMappingInfo  *mmi                 = rt_modelMapInfoPtr;
    rtwCAPI_DataTypeMap const *dTypeMap            = rtwCAPI_GetDataTypeMap(mmi);

    for (i=0; i<nStructLeaves+nNonStructDataTypes; i++) {
        bool structLeaf    = paramInfo[i].structLeaf;
        bool complex       = (bool)paramInfo[i].complex;        
        int  dtTransIdx    = paramInfo[i].dtTransIdx;
        int  dataType      = paramInfo[i].dataType;
        int  dtSize        = 0;
        int  nEls          = 0;
        int  elSize        = 0;
        int  nParams       = 0;
        char *address      = NULL;
        char *dst          = NULL;

        dtSize        = structLeaf ? rtwCAPI_GetDataTypeSize(dTypeMap, dataType) :
            (int)dataTypeSizes[dataType];        

        /*
         * The datatype-size table (dataTypeSizes) contains only real data types 
         * whereas the c-api includes both real and complex datatypes (cint32_T, for 
         * example). If dtSize was obtained from the c-api, then it must be divided
         * by two for nParams to be correct.
         */

        dtSize = (complex && structLeaf) ? (dtSize / 2) : dtSize;

        nEls          = paramInfo[i].nEls;
        elSize        = paramInfo[i].elSize;
        nParams       = (elSize*nEls)/dtSize;

        if (!nEls) continue;

        if (!structLeaf) {
            address = dtTransGetAddress(dtTable, dtTransIdx);
            /*
             * Check for consistent element size.  paramInfo->elSize is the size
             * as stored in the parameter mat-file.  This should match the size
             * used by the generated code (i.e., stored in the SimStruct).
             */
            if ((dataType <= 13 && elSize != dtSize) ||
                (dataType > 13 && (dtSize % elSize != 0))){
                errStr = "Parameter data type sizes in MAT-file not same "
                    "as data type sizes in RTW generated code";
                goto EXIT_POINT;
            }
        } else{
            address = paramInfo[i].prmAddr;
        }

        if (!complex) {
            (void)memcpy(address, paramInfo[i].rVals,nParams*dtSize);
        } else {
            /*
             * Must interleave the real and imaginary parts.  Simulink style.
             */
            int  j;
            const char *realSrc = (const char *)paramInfo[i].rVals;
            const char *imagSrc = (const char *)paramInfo[i].iVals;
            dst = address;

            for (j=0; structLeaf ? j<nEls : j<nParams; j++) {
                /* Copy real part. */
                (void)memcpy(dst,realSrc,dtSize);
                dst     += dtSize;
                realSrc += dtSize;

                /* Copy imag part. */
                (void)memcpy(dst,imagSrc,dtSize);
                dst     += dtSize;
                imagSrc += dtSize;
            }
        }
    }        

EXIT_POINT:
    return(errStr);
} /* end ReplaceRtP */


/*==================*
 * Visible routines *
 *==================*/

/* Function: rt_RapidReadMatFileAndUpdateParams ========================================
 *
 */
void rt_RapidReadMatFileAndUpdateParams(const SimStruct *S)
{
    const char*    result         = NULL;
    PrmStructData* paramStructure = NULL;

    if (gblParamFilename == NULL) goto EXIT_POINT;

    /* checksum comparison is performed in rt_ReadParamStructMatFile */
    result = rt_ReadParamStructMatFile(&paramStructure, S, gblParamCellIndex);
    if (result != NULL) goto EXIT_POINT;

    /* Replace the rtP structure */
    result = ReplaceRtP(S, paramStructure);
    if (result != NULL) goto EXIT_POINT;

  EXIT_POINT:
    if (paramStructure != NULL) {
        rt_FreeParamStructs(paramStructure);
    }
    if (result) ssSetErrorStatus(S, result);
    return;

} /* rt_RapidReadMatFileAndUpdateParams */


/* EOF raccel_utils.c */

/* LocalWords:  RSim matrx smaple matfile rb scaler Tx gbl tu Datato TUtable
 * LocalWords:  UTtable FName FFname raccel el fromfile npts npoints nchannels
 * LocalWords:  curr tfinal timestep Gbls Remappings DType rtp CAPI rsim
 */

