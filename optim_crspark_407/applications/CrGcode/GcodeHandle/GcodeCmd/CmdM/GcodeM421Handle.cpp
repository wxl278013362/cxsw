#include "GcodeM421Handle.h"
#include "CrModel/CrMotion.h"
#include "../../GcodeHandle.h"
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrGpio/CrGpio.h"
#include "CrInc/CrMsgType.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef AUTO_BED_LEVELING_BILINEAR

#define WITHIN(N,L,H)       ((N) >= (L) && (N) <= (H))

#define CMD_M421 ("M421")

#ifdef AUTO_BED_LEVELING_BILINEAR
    FLOAT AblZvalues[GRID_MAX_POINTS_X][GRID_MAX_POINTS_Y];
#endif
#ifdef MESH_BED_LEVELING
    FLOAT MblZvalues[GRID_MAX_POINTS_X][GRID_MAX_POINTS_Y];
#endif
#ifdef AUTO_BED_LEVELING_UBL
    FLOAT UblZvalues[GRID_MAX_POINTS_X][GRID_MAX_POINTS_Y];
#endif

struct XYU8_t
{
    U8 X;
    U8 Y;
};
static VOID onMeshUpdate(const S8 Xpos, const S8 Ypos, const float Zval)
{
      // Called when any mesh points are updated
}

#ifdef MESH_BED_LEVELING
static void MblSetZ(const U8 x, const U8 y, const FLOAT Value)
{
    MblZvalues[x][y] = Value;
}
#endif

#ifdef AUTO_BED_LEVELING_BILINEAR
static S32 AutoBedLevelingBilinear(struct ArgM421_t * Param)
{
    if ( !Param )
    {
        return CrErr;
    }

    if ( !(Param->HasI && Param->HasValueI) )
    {
        Param->I = -1;
    }

    if ( !(Param->HasJ && Param->HasValueJ) )
    {
        Param->J = -1;
    }

    BOOL HasZ = (Param->HasZ && Param->HasValueZ);
    BOOL HasQ = !HasZ && (Param->HasQ && Param->HasValueQ);
    if ( HasZ || Param->HasQ )
    {
        if ( WITHIN(Param->I, -1, GRID_MAX_POINTS_X - 1) &&  WITHIN(Param->J, -1, GRID_MAX_POINTS_Y - 1) )
        {
            U8 Sx = Param->I >= 0 ? Param->I : 0, Ex = Param->I >= 0 ? Param->I : GRID_MAX_POINTS_X - 1,
                Sy = Param->J >= 0 ? Param->J : 0, Ey = Param->J >= 0 ? Param->J : GRID_MAX_POINTS_Y - 1;
            for (U8 i = Sx; i <= Ex; i++)
            {
                for (U8 j = Sy; j <= Ey; j++)
                {
                    AblZvalues[i][j] = Param->Z + (Param->HasQ ? AblZvalues[i][j] : 0);
                #ifdef EXTENSIBLE_UI
                    onMeshUpdate(i, j, AblZvalues[i][j]);
                #endif
                }
            }
            //TODO marlin端此处未启用宏未作进一步处理
        #ifdef ABL_BILINEAR_SUBDIVISION
            BedLevelVirtInterpolate();
        #endif
        }
        else
        {
            printf("Mesh point out of range!!!\n");
        }
    }
    else
    {
        printf("M421 incorrect parameter usage!!!\n");
    }

    return GCODE_EXEC_OK;
}
#endif

#ifdef MESH_BED_LEVELING
static S32 MeshBedLeveling(struct ArgM421_t * Param)
{
    if ( !Param )
    {
        return CrErr;
    }

    BOOL HasX = Param->HasX;
    BOOL HasI = Param->HasI;
    S8 Ix = HasI ? (Param->HasValueI ? Param->I : 0) : (HasX ? (Param->HasValueX ? Param->X : 0) : -1);
    BOOL HasY = Param->HasY;
    BOOL HasJ = Param->HasJ;
    S8 Iy = HasJ ? (Param->HasValueJ ? Param->J : 0) : (HasY ? (Param->HasValueY ? Param->Y : 0) : -1);
    BOOL HasZ = Param->HasZ;
    BOOL HasQ = !HasZ && Param->HasQ;

    if ( S32(HasI && HasJ) + S32(HasX && HasY) != 1 || !(HasZ || HasQ) )
    {
        printf("M421 incorrect parameter usage\n");
    }
    else if ( Ix < 0 || Iy < 0 )
    {
        printf("Mesh point out of range\n");
    }
    else
    {
        MblSetZ(Ix, Iy, (Param->HasValueZ ? Param->Z : 0) + (HasQ ? MblZvalues[Ix][Iy] : 0));
    }

    return GCODE_EXEC_OK;
}
#endif

#ifdef AUTO_BED_LEVELING_UBL
static S32 AutoBedLevelingUbl(struct ArgM421_t * Param)
{
    if ( !Param )
    {
        return CrErr;
    }

    XYU8_t ij = {0};
    ij.X = (Param->HasI && Param->HasValueI) ? Param->I : -1;
    ij.Y = (Param->HasJ && Param->HasValueJ) ? Param->J : -1;

    BOOL hasI = (ij.X >= 0),
         hasJ = (ij.Y >= 0),
         hasC = Param->HasC,
         hasN = Param->HasN,
         hasZ = Param->HasZ,
         hasQ = !hasZ && Param->HasQ;

    if ( hasC )
    {
        //TODO 参考Marlin函数 find_closest_mesh_point_of_type(const MeshPointType type, const xy_pos_t &pos, const bool probe_relative/*=false*/, MeshFlags *done_flags/*=nullptr*/)
        //ij = ubl.find_closest_mesh_point_of_type(REAL, current_position);
    }

    if (S32 (hasC) + S32(hasI && hasJ) != 1 || !(hasZ || hasQ || hasN) )
    {
        printf("M421 incorrect parameter usage\n");
    }
    else if ( !WITHIN(ij.X, 0, GRID_MAX_POINTS_X - 1) || !WITHIN(ij.Y, 0, GRID_MAX_POINTS_Y - 1) )
    {
        printf("Mesh point out of range\n");
    }
    else
    {
        FLOAT zVal = UblZvalues[ij.X][ij.Y];
        zVal = hasN ? NULL : (Param->HasValueZ ? Param->Z : 0) + (hasQ ? zVal : 0);
    #ifdef EXTENSIBLE_UI
        onMeshUpdate(ij.X, ij.Y, zVal);
    #endif
    }

    return GCODE_EXEC_OK;
}
#endif

S32 GcodeM421Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return CrErr;
    }

    rt_kprintf("CmdBuff %s \n ", CmdBuff);
    S32 Len = strlen(CmdBuff);
    if ( strncmp(CmdBuff, CMD_M421, strlen(CMD_M421)) != 0 )
    {
        return CrErr;
    }

    struct ArgM421_t *CmdArg = (struct ArgM421_t *)Arg;
    S32 Offset = strlen(CMD_M421) + 1; //加1是越过分隔符
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
            printf("Gcode M421 has no param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_C:
                CmdArg->HasC = true;
                CmdArg->C = atoi(Opt + 1);
                break;

            case OPT_N:
                CmdArg->HasN = true;
                CmdArg->N = atoi(Opt + 1);
                break;

            case OPT_I:
                CmdArg->HasI = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueI = false;     //不传入I值，默认赋值 -1
                }
                else
                {
                    CmdArg->HasValueI = true;
                    CmdArg->I = atoi(Opt + 1);
                }
                break;

            case OPT_J:
                CmdArg->HasJ = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueJ = false;     //不传入J值，默认赋值 -1
                }
                else
                {
                    CmdArg->HasValueJ = true;
                    CmdArg->J = atoi(Opt + 1);
                }
                break;

            case OPT_Q:
                CmdArg->HasQ = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueQ = false;
                }
                else
                {
                    CmdArg->HasValueQ = true;
                    CmdArg->Q = atof(Opt + 1);
                }
                break;

            case OPT_X:
                CmdArg->HasX = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueX = false;
                }
                else
                {
                    CmdArg->HasValueX = true;
                    CmdArg->X = atof(Opt + 1);
                }
                break;

            case OPT_Y:
                CmdArg->HasY = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueY = false;
                }
                else
                {
                    CmdArg->HasValueY = true;
                    CmdArg->Y = atof(Opt + 1);
                }
                break;

            case OPT_Z:
                CmdArg->HasZ = true;
                if ( OptLen == 1 )
                {
                    CmdArg->HasValueZ = false;
                }
                else
                {
                    CmdArg->HasValueZ = true;
                    CmdArg->Z = atof(Opt + 1);
                }
                break;

            default:
                printf("Gcode M421 has no define opt = %s\n", CmdBuff);
                break;
        }

        Offset += strlen(Opt) + 1; //此处的1是分隔符
    }

    return GCODE_EXEC_OK;
}

S32 GcodeM421Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr )
    {
        return CrErr;
    }

    struct ArgM421_t *Param = (struct ArgM421_t *)ArgPtr;

#ifdef AUTO_BED_LEVELING_BILINEAR
    if ( !AutoBedLevelingBilinear(Param) )
    {
        return CrErr;
    }
#elif defined MESH_BED_LEVELING
    if ( !MeshBedLeveling(Param) )
    {
        return CrErr;
    }
#elif defined AUTO_BED_LEVELING_UBL
    if ( !AutoBedLevelingUbl(Param) )
    {
        return CrErr;
    }
#endif

    return GCODE_EXEC_OK;
}

S32 GcodeM421Reply(VOID *ReplyResult, S8 *Buff)
{
    return 0;
}

#endif //AUTO_BED_LEVELING_BILINEAR
