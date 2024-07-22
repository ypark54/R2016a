#ifndef drive_engine_h
#define drive_engine_h
#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */
boolean_T driveDynamicSfcnOutputs(DriveSimulationData *simdata); boolean_T
driveEventSfcnInitialize(DriveSimulationData *simdata); boolean_T
driveEventSfcnOutputs(DriveSimulationData *simdata); boolean_T
driveEventSfcnTerminate(DriveSimulationData *simdata); boolean_T
driveEventSfcnZeroCrossings(DriveSimulationData *simdata); boolean_T
driveKinematicSfcnDerivatives(DriveSimulationData *simdata); boolean_T
driveKinematicSfcnInitialize(DriveSimulationData *simdata); boolean_T
driveKinematicSfcnOutputs(DriveSimulationData *simdata); boolean_T
driveKinematicSfcnProjection(DriveSimulationData *simdata); boolean_T
driveParameterSfcnInitialize(DriveSimulationData *simdata); boolean_T
driveParameterSfcnOutputs(DriveSimulationData *simdata); const DriveErrorRecord
*driveGetErrorMsg(void);
#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* drive_engine_h */
