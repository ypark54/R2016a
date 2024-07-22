/* Copyright 2008-2014 The MathWorks, Inc. */

#ifdef SUPPORTS_PRAGMA_ONCE
# pragma once
#endif

#ifndef __slMessagesSfcnCGBridge_hpp
#define __slMessagesSfcnCGBridge_hpp

#include <vector>
#include "version.h"

#include "simulink_spec.h"

#include "simstruct/simstruc.h"

namespace RTWCG  {
    class CGIR_Block_IF;
}
namespace CG {
    class CoreConstructionFacade;
    class Var;
    class Expr;
    class Region;
}

namespace slmsg {

    SIMULINK_EXPORT_EXTERN_C
    CG::Expr* cgPopMessage(SimStruct* S,
                           CG::CoreConstructionFacade* f,
                           int queueType, int portId, CG::Var* localQId);

    SIMULINK_EXPORT_EXTERN_C
    CG::Expr* cgGetMessagePayload(CG::CoreConstructionFacade* f,
                                  CG::Var* msgVar);

    SIMULINK_EXPORT_EXTERN_C
    CG::Expr* cgCreateMessage(SimStruct* S, CG::CoreConstructionFacade* f,
                              CG::Var* u, int queueType, int id, CG::Var* localQId);
    SIMULINK_EXPORT_EXTERN_C
    CG::Region* cgSendMessage(SimStruct* S, CG::CoreConstructionFacade* f,
                            CG::Var* msgVar, int queueType, int prtId, CG::Var* localQId);

    SIMULINK_EXPORT_EXTERN_C
    CG::Expr* cgDestroyMessage(SimStruct *S,
                               CG::CoreConstructionFacade* f,
                               CG::Var* msgVar);

    SIMULINK_EXPORT_EXTERN_C
    CG::Region* cgCreateAndSendMessage(SimStruct* S, CG::CoreConstructionFacade* f, CG::Var* u, int queueType, int id, CG::Var* localQId);

}

#endif

