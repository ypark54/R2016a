#ifndef drive_structures_h
#define drive_structures_h
#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */
typedef enum DriveImpactType_tag{ DRIVEFREE, DRIVEWAITFREE, DRIVEPOSITIVEIMPULSE,
DRIVENEGATIVEIMPULSE, DRIVEPOSITIVECONTACT, DRIVENEGATIVECONTACT }
DriveImpactType; typedef enum DriveSignalType_tag{ DRIVEUNDEFINEDTRANSDUCER,
DRIVEANGLE, DRIVEMOTION, DRIVEVELOCITY, DRIVEACCELERATION, DRIVEEXTERNALTORQUE,
DRIVECOMPUTEDTORQUE } DriveSignalType; typedef enum DriveRtpType_tag{
DRIVERTP_INERTIA , DRIVERTP_SIMPLEGEARRATIO , DRIVERTP_PLANETPLANETGEARRATIO ,
DRIVERTP_RINGPLANETGEARRATIO , DRIVERTP_INITIALCONDITION } DriveRtpType; typedef
enum DriveBlockType_tag{ DRIVEUNDEFINEDBLOCK , DRIVEWELD , DRIVEIMPACT ,
DRIVESOLVER , DRIVECLUTCH , DRIVEINERTIA , DRIVEFREEGEAR , DRIVEPLANETRING ,
DRIVEEFFICIENCY , DRIVETRANSDUCER , DRIVESIMPLEGEAR , DRIVESTRAINGAUGE ,
DRIVEVARIABLEGEAR , DRIVEPLANETPLANET , DRIVELOSSYPLANETRING ,
DRIVELOSSYPLANETPLANET , DRIVENUMBLOCKTYPES } DriveBlockType; typedef enum
DriveEvent_tag { DRIVEUNLOCKED = 0, DRIVELOCKED, DRIVEFORWARD, DRIVEREVERSE,
DRIVEWAITINGFORWARD, DRIVEWAITINGREVERSE, DRIVE_NUMMODES } DriveEventType;
typedef enum DriveEfficiencyMode_tag{ DRIVENOPOWER, DRIVEPOWERIN, DRIVEPOWEROUT,
DRIVETRIGGERZERO, DRIVETRIGGERPOSITIVE, DRIVETRIGGERNEGATIVE }
DriveEfficiencyMode; typedef struct DriveSignal_tag{ char_T *flag; real_T data;
DriveSignalType type; } DriveSignal; typedef struct DriveParameter_tag{ char_T
*flag; DriveRtpType type; } DriveParameter; typedef struct DriveSolver_tag{
boolean_T linearizationModeIsSet; boolean_T implementModeIteration; real_T
defaultClutchVelocityTolerance; boolean_T
defaultAutomaticVarStepClutchVelocityTolerance; } DriveSolver; typedef struct
DriveWeld_tag{ boolean_T isWeld; } DriveWeld; typedef struct DriveInertia_tag{
real_T inertia; real_T initialPosition; real_T initialVelocity; int32_T
numFlanges; boolean_T ipWasSet; boolean_T ivWasSet; } DriveInertia; typedef
struct DriveImpact_tag{ DriveImpactType mode; int32_T baseFlange; int32_T
followerFlange; int32_T lagrangeIndex; real_T negativeDeadZone; real_T
positiveDeadZone; real_T lastImpactTime; real_T lastReleaseTime; real_T
velocityTolerance; real_T initialBaseAngle; real_T initialFollowerAngle; real_T
restitutionCoefficient; boolean_T forceImpactDetection; boolean_T
positiveContactReleased; boolean_T negativeContactReleased; boolean_T
lockWhenLinearizing; boolean_T lockWhenApplyingInitialConditions; } DriveImpact;
typedef struct DriveEfficiency_tag{ boolean_T isFixed; boolean_T isDiscrete;
boolean_T isContinuous; boolean_T lossPower; boolean_T lossTorque; boolean_T
forceUnityEfficiency; int32_T lagrangeIndex; char_T *triggerFlag; char_T
*lossPowerFlag; char_T *lossTorqueFlag; char_T *efficiencyFlag; real_T trigger;
real_T efficiencyFactor; real_T adjustedEfficiencyFactor; real_T
velocityTolerance; DriveEfficiencyMode triggerMode; DriveEfficiencyMode
efficiencyMode; } DriveEfficiency; typedef struct DriveLossyPlanetRing_tag{
boolean_T lossPower; boolean_T lossTorque; boolean_T forceUnityEfficiency;
int32_T ringFlange; int32_T planetFlange; int32_T carrierFlange; real_T ratio;
int32_T lagrangeIndex; char_T *lossPowerFlag; char_T *lossTorqueFlag; real_T
efficiencyFactor; real_T adjustedEfficiencyFactor; real_T velocityTolerance;
DriveEfficiencyMode efficiencyMode; } DriveLossyPlanetRing; typedef struct
DriveLossyPlanetPlanet_tag{ boolean_T lossPower; boolean_T lossTorque; boolean_T
forceUnityEfficiency; int32_T sunFlange; int32_T planetFlange; int32_T
carrierFlange; real_T ratio; int32_T lagrangeIndex; char_T *lossPowerFlag; char_T
*lossTorqueFlag; real_T efficiencyFactor; real_T adjustedEfficiencyFactor; real_T
velocityTolerance; DriveEfficiencyMode efficiencyMode; } DriveLossyPlanetPlanet;
typedef struct DriveClutch_tag{ int32_T inputFlange; int32_T outputFlange;
int32_T lagrangeIndex; char_T *modeFlag; char_T *velocityFlag; char_T
*kineticFrictionTorqueFlag; char_T *posStaticFricPeakFlag; char_T
*negStaticFricPeakFlag; real_T lockedTime; real_T switchedToWait; real_T
kineticFrictionTorque; real_T posStaticFrictionTorque; real_T
negStaticFrictionTorque; real_T relativeVelocity; real_T relativePosition; real_T
velocityTolerance; real_T relativeAcceleration; DriveEventType mode; boolean_T
oneWay; boolean_T outputMode; boolean_T outputFriction; boolean_T lockAtStart;
boolean_T automaticVarStepVelocityTolerance; boolean_T
useDefaultVelocityTolerance; } DriveClutch; typedef struct DriveStrainGauge_tag{
char_T *flag; int32_T measurementFlange; int32_T lagrangeIndex; }
DriveStrainGauge; typedef struct DriveSimpleGear_tag{ boolean_T reversing;
int32_T inputFlange; int32_T outputFlange; real_T ratio; } DriveSimpleGear;
typedef struct DriveVariableGear_Tag{ boolean_T reversing; boolean_T coriolis;
boolean_T isDiscrete; boolean_T isContinuous; int32_T independentIndex; int32_T
inputFlange; int32_T outputFlange; int32_T lagrangeIndex; real_T ratio; real_T
dotRatio; real_T trigger; char_T *triggerFlag; char_T *ratioFlag; char_T
*velocityFlag; DriveEfficiencyMode triggerMode; } DriveVariableGear; typedef
struct DrivePlanetRing_tag{ int32_T ringFlange; int32_T planetFlange; int32_T
carrierFlange; real_T ratio; } DrivePlanetRing; typedef struct
DrivePlanetPlanet_tag{ int32_T carrierFlange; int32_T sunFlange; int32_T
planetFlange; real_T ratio; } DrivePlanetPlanet; typedef struct
DriveTransducer_tag{ boolean_T motionDriven; real_T position; real_T velocity;
real_T acceleration; char_T *positionFlag; char_T *velocityFlag; char_T
*accelerationFlag; int32_T numSignals; int32_T lagrangeIndex; DriveSignal
*signal; } DriveTransducer; typedef struct DriveBlock_tag{ int32_T numWeld;
int32_T numImpact; int32_T numClutch; int32_T numSolver; int32_T numFlanges;
int32_T numInertia; int32_T numPlanetRing; int32_T numSimpleGear; int32_T
numVariableGear; int32_T numTransducer; int32_T numEfficiency; int32_T
numStrainGauge; int32_T numPlanetPlanet; int32_T numLossyPlanetRing; int32_T
numLossyPlanetPlanet; DriveBlockType type; char_T *name; char_T *visibleName;
int32_T *flange; int32_T numParameters; DriveParameter *parameter; DriveWeld
*weld; DriveImpact *impact; DriveSolver *solver; DriveClutch *clutch;
DriveInertia *inertia; DrivePlanetRing *planetRing; DriveSimpleGear *simpleGear;
DriveEfficiency *efficiency; DriveTransducer *transducer; DriveVariableGear
*variableGear; DriveStrainGauge *strainGauge; DrivePlanetPlanet *planetPlanet;
DriveLossyPlanetRing *lossyPlanetRing; DriveLossyPlanetPlanet *lossyPlanetPlanet;
} DriveBlock; typedef struct DriveIoSizesInformation_tag{ int32_T numModes;
int32_T numEventInputs; int32_T numEventOutputs; int32_T numDynamicInputs;
int32_T numZeroCrossings; int32_T numDynamicOutputs; int32_T numKinematicInputs;
int32_T numKinematicOutputs; int32_T numRtParameters; boolean_T
needPositionStates; } DriveIoSizesInformation; typedef struct
DriveCompilerData_tag { int32_T numBlocks; char_T *name; DriveBlock *block;
DriveIoSizesInformation ioSizes; real_T mdlHandle; } DriveCompilerData; typedef
struct DriveErrorRecord_tag { const char_T *errorMsg; const char_T *blocks[(10)];
boolean_T errorFlag; } DriveErrorRecord; typedef struct DriveRunTimeData_tag
DriveRunTimeData; typedef struct DriveStaticMatrix_tag DriveStaticMatrix; typedef
struct DriveDynamicMatrix_tag DriveDynamicMatrix; typedef struct
DriveLinearizationData_tag DriveLinearizationData; typedef boolean_T
(*DriveCodeGenFunction)(const DriveCompilerData *, char *, int32_T, char *,
int32_T); typedef struct DriveLine_tag{ real_T *state; real_T *discreteState;
boolean_T haveValidEngine; DriveErrorRecord *engineError; DriveRunTimeData
*runTimeData; DriveCompilerData *compilerData; DriveStaticMatrix *staticMatrix;
DriveDynamicMatrix *dynamicMatrix; DriveLinearizationData *linearizationData;
char *moduleName; void *table; real_T time; boolean_T ismajortimestep; boolean_T
isRTW; boolean_T needToReadRtp; int32_T numDStates; int32_T numCStates; }
DriveLine; typedef DriveLine *(*DriveAccessFunction)(const int32_T); typedef void
(*DestroyStoredDataFunction)(const int32_T); typedef boolean_T
(*RtwGenerateCodeFunction)(const DriveCompilerData
*,char*,int32_T,char*,int32_T);
#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* drive_structures_h */
