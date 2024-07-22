#ifndef drive_simulation_data_h
#define drive_simulation_data_h
#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */
typedef struct DriveSimulationData_tag { int32_T *iwork; const real_T *inputs[6];
real_T *outputs[6]; real_T *contstates; real_T *discstates; real_T *dx; int32_T
*mode; real_T *zcsignals; real_T *abstol; boolean_T autoabstol; boolean_T
consistencyChecking; boolean_T isvarstepsolver; DriveLine *driveLine; }
DriveSimulationData;
#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* drive_simulation_data_h */
