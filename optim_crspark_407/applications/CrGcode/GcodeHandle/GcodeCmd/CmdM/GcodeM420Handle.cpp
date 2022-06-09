#include "GcodeM420Handle.h"
#include "CrModel/CrMotion.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"
#include "CrInc/CrMsgType.h"
#include "Common/Include/CrEepromApi.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef HAS_LEVELING

#define CMD_M420 ("CMD_M420")

#define NOLESS(v, n) \
  do{ \
    __typeof__(v) _n = (n); \
    if (_n > v) v = _n; \
  }while(0)

#define NOMORE(v, n) \
  do{ \
    __typeof__(v) _n = (n); \
    if (_n < v) v = _n; \
  }while(0)
#define NEAR_ZERO(x)    (x >= -0.000001f && x <= 0.000001f)


FLOAT Zvalues[GRID_MAX_POINTS_X][GRID_MAX_POINTS_Y];
static BOOL LevelingActive = false;
static U8 UblStoragesSlot = 0;
extern struct Coordinate3d_t CurCoordination;

VOID SetUblStorageSlot(U8 Val)
{
    UblStoragesSlot = Val;
}

U8 GetUblStorageSlot(VOID)
{
    return UblStoragesSlot;
}

U16 CalcNumMeshes(VOID)
{
    /* TODO return (meshes_end - meshes_start_index()) / MESH_STORE_SIZE;*/
    return 0;
}

VOID LoadMesh(U8 Slot)
{
    //TODO...
}

/* Set Z fade height */
VOID SetZfadeHeight(const FLOAT Zfh)
{
    SettingData.PlannerFadeHeightZ = Zfh > 0 ? Zfh : 0.0f;
}

/**
 * Produce one of these mesh maps:
 *   0: Human-readable
 *   1: CSV format for spreadsheet import
 *   2: TODO: Display on Graphical LCD
 *   4: Compact Human-Readable
 */
VOID DisplayMap(const U8 Map_Type)
{
    //TODO...
}

BOOL LevelingIsValid(VOID)
{
    return true;
}

VOID SetBedLevingEnabled(const BOOL Enable)
{
    if ( Enable != LevelingActive )
    {
        if ( LevelingActive )
        {
            //M420 TODO
            // change unleveled current_position to physical current_position without moving steppers.
            LevelingActive = false;
        }
        else
        {
            // TODO change physical current_position to unleveled current_position without moving steppers.
            LevelingActive = true;
        }
    }
}

BOOL UblMeshIsvalid(VOID)
{
    return true;
}

VOID AdjustMeshToMean(const BOOL Cflag, FLOAT Offset)
{
    //TODO
    //参考Marlin函数adjust_mesh_to_mean
}

VOID BedLevelVirtInterpolate(VOID)
{
    //TODO
    //参考Marlin函数bed_level_virt_interpolate
}

FLOAT ProbeMinX(VOID)
{
    return 0.0f;
}

FLOAT ProbeMaxX(VOID)
{
    return 0.0f;
}

FLOAT ProbeMinY(VOID)
{
    return 0.0f;
}

FLOAT ProbeMaxY(VOID)
{
    return 0.0f;
}

VOID BilinearStartSet(FLOAT x, FLOAT y)
{
    
}

VOID BilinearGridSpacingSet(FLOAT x, FLOAT y)
{
    
}

S32 random(S32 Min, S32 Max)
{
    return Min + rand() % (Max - Min);
}

VOID PrintBilinearLevelingGrid(VOID)
{
    
}

VOID PrintBilinearLevelingGridVirt(VOID)
{
    
}

VOID MblReportMesh(VOID)
{
    
}

static VOID ReportCurrentPosition()
{
    struct MotionPositionMsg_t Pos = {0};
    if ( !MotionModel )
    {
        return ;
    }
    Pos = MotionModel->GetCurPosition();
    //报告本地坐标（加上偏移值的坐标）
    printf("X: %f Y: %f Z: %f E: %f",Pos.PosX,Pos.PosY,Pos.PosZ,Pos.PosE);//未加上偏移值

    //报告当前坐标
    printf("X: %f Y: %f Z: %f E: %f",Pos.PosX,Pos.PosY,Pos.PosZ,Pos.PosE);
}

S32 GcodeM420Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M420, strlen(CMD_M420)) != 0 )
    {
        return CrErr;
    }

    struct ArgM420_t *CmdArg = (struct ArgM420_t *)Arg;
    S32 Offset = strlen(CMD_M420) + 1; //加1是越过分隔符
    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M420 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_C:
                CmdArg->HasC = true;
                CmdArg->C = atoi(Opt + 1);
                break;

            case OPT_L:
                CmdArg->HasL = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueL = false;
                }
                else
                {
                    CmdArg->HasValueL = true;
                    CmdArg->L = atoi(Opt + 1);
                }
                break;

            case OPT_S:
                CmdArg->HasS = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueS = false;
                }
                else
                {
                    CmdArg->HasValueS = true;
                    CmdArg->S = atoi(Opt + 1);
                }
                break;

            case OPT_T:
                CmdArg->HasT = true;
                CmdArg->T = atoi(Opt + 1);
                break;

            case OPT_V:
                CmdArg->HasV = true;
                CmdArg->V = atoi(Opt + 1);
                break;

            case OPT_Z:
                CmdArg->HasZ = true;
                CmdArg->Z = atof(Opt + 1);
                break;

            default:
            {
                printf("Gcode M420 has no define opt = %s\n", CmdBuff);
                break;
            }
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM420Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    struct ArgM420_t *Param = (struct ArgM420_t *)ArgPtr;
    BOOL ToEnable;
    if ( Param->HasS )
    {
        if ( !Param->HasValueS || !Param->S )   //空值和非0均为true
        {
            ToEnable = true;
        }
    }
    else
    {
        ToEnable = LevelingActive;
    }

#ifdef MARLIN_DEV_MODE
    if ( Param->S == 2 )
    {
        FLOAT Xmin = ProbeMinX();
        FLOAT Xmax = ProbeMaxX();
        FLOAT Ymin = ProbeMinY();
        FLOAT Ymax = ProbeMaxY();

    #ifdef AUTO_BED_LEVELING_BILINEAR
        BilinearStartSet(Xmin, Ymin);
        BilinearGridSpacingSet((Xmax - Xmin) / (GRID_MAX_CELLS_X), (Ymax - Ymin) / (GRID_MAX_CELLS_Y));
    #endif

        for ( U8 i = 0; i < GRID_MAX_POINTS_X; i++ )
        {
            for ( U8 j = 0; i < GRID_MAX_POINTS_Y; j++ )
            {
                Zvalues[i][j] = 0.001 * random(-200, 200);
            #ifdef EXTENSIBLE_UI
                onMeshUpdate(i, j, Zvalues[i][j]);
            #endif
            }
        }
    }
#endif

    struct MotionPositionMsg_t OldPos = MotionModel->GetCurPosition();

    if ( Param->HasS && !ToEnable )
    {
        SetBedLevingEnabled(false);
    }

#ifdef AUTO_BED_LEVELING_UBL
    // L to load a mesh from the EEPROM.
    if ( Param->HasL )
    {
        SetBedLevingEnabled(false);

    #ifdef EEPROM_SETTINGS

        const U8 StorageSlot = Param->HasValueL ? Param->L : UblStoragesSlot;
        const U16 Num = CalcNumMeshes();
        if ( !Num )
        {
          printf("EEPROM storage not available!!!");
          return CrErr;
        }

        LoadMesh(StorageSlot);
        SetUblStorageSlot(StorageSlot);

    #else

        printf("?EEPROM storage not available!!!\n");
        return CrErr;

    #endif
    }

    if ( Param->HasL || Param->HasV )
    {
        DisplayMap(Param->T);
        if ( !UblMeshIsvalid() )
        {
            printf("Mesh is in valid, Storage slot: %d\n", UblStoragesSlot);
        }
    }
#endif  // AUTO_BED_LEVELING_UBL

#ifdef HAS_MESH

    if ( LevelingIsValid() )
    {
        // Subtract the given value or the mean from all mesh values
        if ( Param->HasC )
        {
            const FLOAT Cval = Param->C;
        #ifdef AUTO_BED_LEVELING_UBL

            SetBedLevingEnabled(false);
            AdjustMeshToMean(true, Param->C);

        #else
            #ifdef M420_C_USE_MEAN
                // Get the sum and average of all mesh values
                FLOAT MeshSsum = 0;
                for ( U8 i = 0; i < GRID_MAX_POINTS_X; i++ )
                {
                    for ( U8 j = 0; i < GRID_MAX_POINTS_Y; j++ )
                    {
                        MeshSsum += Zvalues[i][j];
                    }
                }
                const FLOAT Zmean = MeshSsum / float(GRID_MAX_POINTS);
            #else
                // Find the low and high mesh values.
                FLOAT LowVal = 100, HighVal = -100;
                for ( U8 i = 0; i < GRID_MAX_POINTS_X; i++ )
                {
                    for ( U8 j = 0; i < GRID_MAX_POINTS_Y; j++ )
                    {
                        const FLOAT z = Zvalues[i][j];
                        NOMORE(LowVal, z);
                        NOLESS(HighVal, z);
                    }
                }
                const FLOAT Zmean = (LowVal + HighVal) / 2.0 + Cval ;
            #endif
            // If not very close to 0, adjust the mesh
            if ( !NEAR_ZERO(Zmean) )
            {
                SetBedLevingEnabled(false);
                for ( U8 i = 0; i < GRID_MAX_POINTS_X; i++ )
                {
                    for ( U8 j = 0; i < GRID_MAX_POINTS_Y; j++ )
                    {
                        Zvalues[i][j] -= Zmean;
                    #ifdef EXTENSIBLE_UI
                        onMeshUpdate(i, j, Zvalues[i][j]);
                    #endif
                    }
                }
            #ifdef ABL_BILINEAR_SUBDIVISION
                BedLevelVirtInterpolate();
            #endif
            }

        #endif
        }
    }
    else if ( ToEnable || Param->HasV )
    {
        printf("Invalid mesh!!!\n");
        goto EXIT_M420;
    }

#endif  // HAS_MESH

    // V to print the matrix or mesh
    if ( Param->HasV )
    {
    #if ABL_PLANAR
        printf("Bed Level Correction Matrix");
    #else
        if ( LevelingIsValid() )
        {
        #ifdef AUTO_BED_LEVELING_BILINEAR
            PrintBilinearLevelingGrid();
            #ifdef ABL_BILINEAR_SUBDIVISION
                PrintBilinearLevelingGridVirt();
            #endif
        #elif defined MESH_BED_LEVELING
            printf("Mesh Bed Level data:");
            MblReportMesh();
        #endif
        }
    #endif
    }
#ifdef ENABLE_LEVELING_FADE_HEIGHT

    if ( Param->HasZ )
    {
        SetZfadeHeight(Param->Z);
    }

#endif
    // Enable leveling if specified, or if previously active
    SetBedLevingEnabled(ToEnable);

#ifdef HAS_MESH
      EXIT_M420:
#endif
    // Error if leveling failed to enable or reenable
    if ( ToEnable && !LevelingActive )
    {
        printf("Failed to enable Bed Leveling!!!\n");
    }
    printf("Bed Leveling:%d\n", LevelingActive);
#ifdef ENABLE_LEVELING_FADE_HEIGHT
    if ( SettingData.PlannerFadeHeightZ > 0.0 )
    {
        printf("Fade Height: %4f\n", SettingData.PlannerFadeHeightZ);
    }
    else
    {
        printf("Fade Height Off\n");
    }
#endif

    ReportCurrentPosition();

    return GCODE_EXEC_OK;
}

S32 GcodeM420Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

#endif
