#include "../Include/CrGlobal.h"

#ifdef SLOT_COUNT
struct MotionCoord_t StoredPos[SLOT_COUNT] = {0};
#endif

BOOL EnableArc = 0;
BOOL EnableSerialOnOff = 0;
BOOL EnableBinaryFileTransfer = 0;
BOOL EnableEeprom = 0;
BOOL EnableVolumetric = 0;
BOOL EnableAutoReportTemper = 0;
BOOL EnableAutoLevel = 0;
BOOL EnableRunOutSensor = 0;
BOOL EnableBedProbe = 0;
BOOL EnableMeshLeveling = 0;
BOOL EnableSetProgressManually = 0;
BOOL EnablePsuCtrl = 0;
BOOL EnableCaseLight = 0;
BOOL EnableCaseLightBrightness = 0;
BOOL EnableEmergencyParser = 0;
BOOL EnablePromptSupport = 0;
BOOL EnableSdSupport = 0;
BOOL EnableAutoReportSdStatus = 0;
BOOL EnableLongFileNameSupport = 0;
BOOL EnableThermallySafe = 0;
BOOL EnableMotionModes = 0;
BOOL EnableBabySteps = 0;
BOOL EnableHeatedChamber = 0;
BOOL EnableGeometryReport = 0;
