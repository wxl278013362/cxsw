#ifndef _GCODETYPE_H
#define _GCODETYPE_H

#include "CrInc/CrType.h"

#define MAX_STRING_LENGTH 64    //字符串的最大空间

#define CMD_BUFF_LEN 8

//Gcode命令中常用到的操作的名称
#define OPT_A  'A'
#define OPT_B  'B'
#define OPT_C  'C'
#define OPT_D  'D'
#define OPT_E  'E'
#define OPT_F  'F'
#define OPT_G  'G'
#define OPT_H  'H'
#define OPT_I  'I'
#define OPT_J  'J'
#define OPT_K  'K'
#define OPT_L  'L'
#define OPT_M  'M'
#define OPT_N  'N'
#define OPT_O  'O'
#define OPT_P  'P'
#define OPT_Q  'Q'
#define OPT_R  'R'
#define OPT_S  'S'
#define OPT_T  'T'
#define OPT_U  'U'
#define OPT_V  'V'
#define OPT_W  'W'
#define OPT_X  'X'
#define OPT_Y  'Y'
#define OPT_Z  'Z'

#define OPT_NON  '!'
#define OPT_A1 "A1"
#define OPT_E1 "E1"

/*********************其他常量 begin*****************************/
#define CMD_ARG_SEPARATE (' ')
/*********************Gcode 命令操作常量 begin*****************************/

/*********************Gcode 命令类型 begin*****************************/
#define GCODE_CMD_G ('G')
#define GCODE_CMD_M ('M')
#define GCODE_CMD_T ('T')
/*********************Gcode 命令类型 begin*****************************/

//Gcode 的G1命令和G0命令的参数
struct ArgG1AndG0_t
{
    BOOL    HasE;
    BOOL    HasF;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    FLOAT   E;      //喂入挤出机的长丝长度
    FLOAT   F;     //进给速率
    FLOAT   X;      //X轴的坐标
    FLOAT   Y;      //Y轴的坐标
    FLOAT   Z;      //Z轴的坐标
};

//Gcode G2命令和G3命令的参数
struct ArgG2AndG3_t
{
    BOOL    HasE;
    BOOL    HasI;
    BOOL    HasJ;
    BOOL    HasP;
    BOOL    HasR;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    FLOAT   E;    //挤出的量
    FLOAT   I;    //当前X位置的偏移量
    FLOAT   J;    //当前Y位置的偏移量
    FLOAT   P;    //制定完整的圆
    FLOAT   R;    //当前XY位置的半径
    FLOAT   X;    //X轴上的坐标
    FLOAT   Y;    //Y轴上的坐标
    FLOAT   Z;    //Z轴上的坐标
};

//Gcode G4命令的参数
struct ArgG4_t
{
    BOOL    HasP;
    BOOL    HasS;

    U32     TimeP;   //停留时间（单位毫秒）
    U32     TimeS;   //停留时间（单位秒） （S的优先级高于P）
};

//Gcode G5命令的参数
struct ArgG5_t
{
    BOOL    HasE;
    BOOL    HasF;
    BOOL    HasI;
    BOOL    HasJ;
    BOOL    HasP;
    BOOL    HasQ;
    BOOL    HasX;
    BOOL    HasY;

    FLOAT   E;      //喂入挤出机的长丝长度
    FLOAT   F;      //移动的最大进给速度
    FLOAT   I;        //从起点到第一个控制点的X增量偏移
    FLOAT   J;        //从起点到第一个控制点的Y增量偏移
    FLOAT   P;        //从起点到第二个控制点的X增量偏移
    FLOAT   Q;        //从起点到第二个控制点的Y增量偏移
    FLOAT   X;        //X轴上的坐标
    FLOAT   Y;        //Y轴上的坐标
};

//Gcode G6命令的参数
struct ArgG6_t
{
    BOOL HasE;
    BOOL HasI;
    BOOL HasS;
    BOOL HasR;
    BOOL HasX;
    BOOL HasY;
    BOOL HasZ;

    BOOL  E;        //1为正，0为负
    U32   I;        //设置页面索引
    U32   S;        //每秒步数
    U32   R;        //将要采取的步数
    BOOL  X;        //1为正，0为负
    BOOL  Y;        //1为正，0为负
    BOOL  Z;        //1为正，0为负
};

//Gcode G10命令的参数
struct ArgG10_t
{
    BOOL    HasS;

    S32     S;   //交换回缩，（要求挤出机的个数大于1）
};

//Gcode G12命令的参数
struct ArgG12_t
{
    BOOL    HasP;
    BOOL    HasS;
    BOOL    HasT;
    BOOL    HasR;
    
    S32     P;  //图案样式选择       范围<0|1|2>
    S32     S;  //模式的重复次数
    S32     T;  //锯齿形图案中的三角形数
    FLOAT   R;  //清洁圈半径
};

//Gcode G26命令的参数
struct ArgG26_t
{
    BOOL    HasB;
    BOOL    HasC;
    BOOL    HasD;
    BOOL    HasK;
    BOOL    HasH;
    BOOL    HasF;
    BOOL    HasI;
    BOOL    HasL;
    BOOL    HasO;
    BOOL    HasP;
    BOOL    HasQ;
    BOOL    HasR;
    BOOL    HasS;
    BOOL    HasU;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    IsCurX_Pos; //是否是当前X位置
    BOOL    IsCurY_Pos; //是否是当前Y位置
    
    S16     B;  //热床温度（默认60℃）
    BOOL    C;  //是否继续最接近的点
    BOOL    D;    //启用/禁用水平补偿
    BOOL    K;    //是否保持加热器开启
    S16     H;    //热端温度(默认205℃)
    FLOAT   F;    //长丝直径（默认1.75mm）
    FLOAT   I;    //材料预设
    FLOAT   L;    //层高
    FLOAT   O;    //软泥量（默认0.3mm）
    FLOAT   P;    //素数长度
    FLOAT   Q;    //回缩倍数（默认1.0）
    FLOAT   R;    //G26命令重复数(默认是999)
    FLOAT   S;    //喷出尺寸（默认0.4mm）
    FLOAT   U;    //随机偏差（默认50）
    FLOAT   X;    //X位置（默认是当前X位置）
    FLOAT   Y;    //Y位置（默认是当前Y位置）
};

//Gcode G27命令参数
struct ArgG27_t
{
    BOOL    HasP;

    U8      P;        //Z轴动作     范围<0|1|2>
};

//Gcode G28命令的参数
struct ArgG28_t
{
    BOOL    HasO;   //
    BOOL    HasR;    //归位前升高距离
    BOOL    HasX;    //标记返回X轴圆点
    BOOL    HasY;    //标记返回Y轴圆点
    BOOL    HasZ;    //标记返回Z轴圆点
};

//Gcode G29命令(3点调平)的参数
struct ArgG29ThreePoint_t
{
    BOOL    HasA;
    BOOL    HasC;
    BOOL    HasD;
    BOOL    HasE;
    BOOL    HasJ;
    BOOL    HasO;
    BOOL    HasQ;
    BOOL    HasV;

    BOOL    A;      //中止调平程序
    BOOL    C;      //创建假点进行测试
    BOOL    D;        //空运行模式
    BOOL    E;        //默认情况下，G29将接合Z探针，测试床，然后脱离。包括“ E”以接合/分离每个样品的Z探针。如果您有固定的Z探针，则不会产生任何额外的影响。（无PROBE_MANUALLY）
    BOOL    J;        //Jettison将调平数据存储在SRAM中并关闭调平补偿。EEPROM中的数据不受影响
//    FLOAT    O;        //可选的。如果已经启用了调平，则不进行调平就退出。
    BOOL    Q;        //查询当前调平状态
    U8      V;        //设置详细级别    范围<0-4>
};

//Gcode G29命令（线性）的参数
struct ArgG29Linear_t
{
    BOOL    HasA;
    BOOL    HasC;
    BOOL    HasD;
    BOOL    HasE;
    BOOL    HasJ;
    BOOL    HasQ;
    BOOL    HasT;
    BOOL    HasV;
    BOOL    HasB;
    BOOL    HasF;
    BOOL    HasH;
    BOOL    HasL;
    BOOL    HasO;
    BOOL    HasP;
    BOOL    HasR;    
    BOOL    HasS;
    BOOL    HasX;
    BOOL    HasY;

    BOOL    A;      //中止调平程序
    BOOL    C;      //创建假网格进行测试
    BOOL    D;        //空运行模式
    BOOL    E;        //默认情况下，G29将接合Z探针，测试床，然后脱离。包括“ E”以接合/分离每个样品的Z探针。如果您有固定的Z探针，则不会产生任何额外的影响。（无PROBE_MANUALLY）
    BOOL    J;        //Jettison将调平数据存储在SRAM中并关闭调平补偿。EEPROM中的数据不受影响
    BOOL    T;        //生成床拓扑报告
    BOOL    Q;        //查询当前调平状态（PROBE_MANUALLY，DEBUG_LEVELING_FEATURE）
    U8      V;        //设置详细级别    范围<0-4>
    FLOAT   B;        //探测网格的后极限
    FLOAT   F;        //探测网格的前极限
    FLOAT   H;        //要探测区域的正方形宽度和高度
    FLOAT   L;        //探测网格的左极限
//    BOOL    O;        //可选的。如果已经启用了调平，则不进行调平就退出。
    S32     P;        //要探测的正方形网格的大小-P x P点
    FLOAT   R;        //探测网格的正确极限
    FLOAT   S;        //探针点之间的XY传播速度
    S32     X;        //要探测的列数
    S32     Y;        //要探测的行数
};

//Gcode G29命令（双线性）的参数
struct ArgG29Bilinear_t
{
    BOOL    HasA;
    BOOL    HasC;
    BOOL    HasD;
    BOOL    HasE;
    BOOL    HasJ;
    BOOL    HasQ;
    BOOL    HasW;
    BOOL    HasV;
    BOOL    HasB;
    BOOL    HasF;
    BOOL    HasH;
    BOOL    HasL;
    BOOL    HasO;
    BOOL    HasR;
    BOOL    HasS;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    BOOL    A;      //中止调平程序
    BOOL    C;      //创建假网格进行测试
    BOOL    D;        //空运行模式
    BOOL    E;        //默认情况下，G29将接合Z探针，测试床，然后脱离。包括“ E”以接合/分离每个样品的Z探针。如果您有固定的Z探针，则不会产生任何额外的影响。（无PROBE_MANUALLY）
    BOOL    J;        //（无W）Jettison将调平数据存储在SRAM中并关闭调平补偿。EEPROM中的数据不受影响
    BOOL    W;        //编写网格Z偏移。
    BOOL    Q;        //查询当前调平状态（PROBE_MANUALLY，DEBUG_LEVELING_FEATURE）
    U8      V;        //设置详细级别    范围<0-4>
    FLOAT   B;        //探测网格的后极限
    FLOAT   F;        //探测网格的前极限
    FLOAT   H;        //要探测区域的正方形宽度和高度
    FLOAT   L;        //探测网格的左极限
//    BOOL    O;        //可选的。如果已经启用了调平，则不进行调平就退出。
    FLOAT   R;        //探测网格的正确极限
    FLOAT   S;        //探针点之间的XY传播速度
    FLOAT   X;        //设置网格值（PROBE_MANUALLY）时指定X。 //注意有时是整数
    FLOAT   Y;        //设置网格值时指定Y。               //注意有时是整数
    FLOAT   Z;        //设置网格值时指定Z偏移。
};

//Gcode G29命令（手动调平MBL）的参数
struct ArgG29Mbl_t
{
    BOOL    HasI;
    BOOL    HasJ;
    BOOL    HasS;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    U16     S;      //范围<0|1|2|3|4|5>. S0： 生成网格报告（请参见下面的示例）。S1： 开始探测网格点。S2： 探测下一个网格点。S3： 使用X Y Z参数手动修改单个点。（另请参见M421。）S4： 设置全局Z偏移。正值远离床；负值更接近。S5： 重置并禁用网格。
    S32     I;      //使用S3修改的网格值的（0…n-1）X索引。
    S32     J;        //使用S3可以修改的网格值的（0…n-1）Y索引。
    S32     X;        //使用S3修改的网格值的（1…n）X计数。
    S32     Y;        //使用S3修改的网格值的（1…n）Y计数
    FLOAT   Z;        //使用S3，新的网格Z值。
};

//Gcode G29命令（统一调平UBL）的参数
struct ArgG29Ubl_t
{
    BOOL    HasA;
    BOOL    HasB;
    BOOL    HasD;
    BOOL    HasE;
    BOOL    HasP;
    BOOL    HasT;
    BOOL    HasU;
    BOOL    HasV;
    BOOL    HasW;
    BOOL    HasC;
    BOOL    HasF;
    BOOL    HasH;
    BOOL    HasI;
    BOOL    HasJ;
    BOOL    HasK;
    BOOL    HasL;
    BOOL    HasQ;
    BOOL    HasR;
    BOOL    HasS;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    IsCurX;  //是否是当前默认值
    BOOL    IsCurY;  //是否是当前默认值
    
    BOOL    A;      //激活统一床平整
    BOOL    B;      //名片模式
    BOOL    D;        //禁用统一床水准仪
    BOOL    E;        //在探测到E每个点后（P1仅）存放探头
    S8      P;        //范围<0|1|2|3|4|5|6>
    BOOL    T;        //拓扑：在输出中包括拓扑图， 范围<0|1>
    BOOL    U;        //放平：探测外围以协助踩踏床
    U8      V;        //设置详细级别    范围<0|1|2|3|4>
    U8      W;        //显示有价值的UBL数据。
    FLOAT   C;        //:
    FLOAT   F;        //渐变高度
    FLOAT   H;        //高度：
    S32     I;        //    使多个网格点无效（默认为1）
    S32     J;        //    网格（或三点）调平：
    S32     K;        //Kompare：从当前网格中减去给定索引的存储网格（差异）
    S32     L;        //    加载网格的索引。
    S32     Q;        //测试图案。
    FLOAT   R;        //重复计数。
    FLOAT   S;        //将网格保存到给定插槽中的EEPROM中
    FLOAT   X;        //所有阶段和命令的X位置（默认值：当前X）
    FLOAT   Y;        //所有阶段和命令的Y位置（默认值：当前Y）
};

//所有G29命令的联合体
union LevelingBedModel_u
{
    struct ArgG29ThreePoint_t    ThreePoint;
    struct ArgG29Linear_t        Linear;
    struct ArgG29Bilinear_t      Bilinear;
    struct ArgG29Mbl_t           Mbl;
    struct ArgG29Ubl_t           Ubl;
};

//Gcode G30命令的参数
struct ArgG30_t
{
    BOOL    HasE;
    BOOL    HasX;
    BOOL    HasY;

    BOOL      E;        //为每个点接合探针。
    FLOAT     X;        //X探头位置
    FLOAT     Y;        //Y探头位置
};

//Gcode G33命令的参数
struct ArgG33_t
{
    BOOL    HasC;
    BOOL    HasE;
    BOOL    HasF;
    BOOL    HasP;
    BOOL    HasT;
    BOOL    HasV;

    U32     C;      //精度。如果省略，迭代将以可达到的最佳精度停止。如果设置，则迭代将以设置的精度停止。
    BOOL    E;      //为每个点接合探针。
    S8      F;        //迭代次数
    S8      P;        //探测点数 范围<0|1|2|3|4-10>
    BOOL    T;        //禁用塔角校正校准
    S8      V;        //详细级别 范围<0|1|2|3|>。
};

//Gcode G34命令的参数
struct ArgG34_t
{
    BOOL    HasA;
    BOOL    HasE;
    BOOL    HasI;
    BOOL    HasT;

    FLOAT   A;      //放大率-必须介于0.5-2.0之间
    BOOL    E;      //探测每个点后，存放探针
    U8      I;        //探测点数 范围<1-30>
    FLOAT   T;        //禁用塔角校正校准
};

//Gcode G35命令的参数
struct ArgG35_t
{
    BOOL    HasS;

    U8      S;        //螺纹类型 范围<30|31|40|41|50|51>。 S30： M3顺时针, S31： M3逆时针方向, S40： M4顺时针, S41： M4逆时针, S50： M5顺时针, S51： M5逆时针.
};

//Gcode G38命令的参数
struct ArgG38_t
{
    BOOL    HasF;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;
    BOOL    HasE;
    BOOL    HasSubCode;

    BOOL    HasValueF;
    BOOL    HasValueX;
    BOOL    HasValueY;
    BOOL    HasValueZ;
    BOOL    HasValueE;
    BOOL    HasValueSubCode;

    FLOAT   F;      //进给速度
    FLOAT   X;      //目标X
    FLOAT   Y;        //目标Y
    FLOAT   Z;        //目标Z
    FLOAT   E;
    U8      SubCode;
};

//Gcode G42命令的参数
struct ArgG42_t
{
    BOOL    HasF;
    BOOL    HasI;
    BOOL    HasJ;

    FLOAT   F;      //进给速率
    S32     I;      //网格坐标的列
    S32     J;      //网格坐标的行
};

struct ArgG53_t
{
    BOOL    HasCmd;
    S8      Cmd[64];
};

//Gcode G60命令的参数
struct ArgG60_t
{
    BOOL    HasS;

    U8      S;        //内存插槽索引
};

//Gcode G61命令的参数
struct ArgG61_t
{
    BOOL    HasF;
    BOOL    HasS;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    FLOAT   F;      //移动进给速度
    S32     S;      //内存插槽的索引（如果未指定，默认为0）
};

//Gcode G76命令的参数
struct ArgG76_t
{
    BOOL    HasB;        //仅校准床
    BOOL    HasP;        //仅校准探头
};

//Gcode G92命令的参数
struct ArgG92_t
{
    BOOL    HasE;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;
    S32     SubCode;
    FLOAT   E;      //新的挤出机位置
    FLOAT   X;      //新的X轴位置
    FLOAT   Y;      //新的Y轴位置
    FLOAT   Z;      //新的Z轴位置
};

//Gcode G425命令的参数
struct ArgG425_t
{
    BOOL    HasB;
    BOOL    HasT;
    BOOL    HasU;
    BOOL    HasV;

    S32     T;      //刀头索引，仅校准一个刀头
    FLOAT   U;      //探头距立方体多远
//    FLOAT    B;        //新的Y轴位置
//    FLOAT    V;        //新的Z轴位置
};

/*******************GCode M命令的结构体定义****************************/
//Gcode M0和M1命令的参数
struct ArgM0AndM1_t
{
    BOOL    HasP;
    BOOL    HasS;
    BOOL    HasString;

    S32     P;                       //过期时间， 单位毫秒
    S32     S;                        //过期时间， 单位秒
    S8      Str[MAX_STRING_LENGTH];    //显示的可选信息
};

//Gcode M3命令的参数
struct ArgM3_t
{
    BOOL    HasI;
    BOOL    HasO;
    BOOL    HasS;

    BOOL    I; //在线模式
    U8      O;
    FLOAT   S;
};

//Gcode M4命令的参数
struct ArgM4_t
{
    BOOL    HasI;
    BOOL    HasO;
    BOOL    HasS;

    BOOL    I; //在线模式
    U8      O;
    FLOAT   S;
};

struct ArgM5_t
{
    BOOL    HasT;

    BOOL    T;
};

//Gcode M16命令的参数
struct ArgM16_t
{
    S8        Str[MAX_STRING_LENGTH]; //在线模式
};

//Gcode M17命令的参数
struct ArgM17_t
{
    BOOL    HasE;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    BOOL    E;      //启用E
    BOOL    X;      //启用X
    BOOL    Y;        //启用Y
    BOOL    Z;        //启用Z
};

//Gcode M18、M84命令的参数
struct ArgM18OrM84_t
{
    BOOL    HasE;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;
    BOOL    HasS;

    BOOL    E;      //禁用E
    BOOL    X;      //禁用X
    BOOL    Y;        //禁用Y
    BOOL    Z;        //禁用Z
    S32     S;        //非活动超时。如果未指定，请立即禁用
};

//Gcode M23命令的参数
struct ArgM23_t
{
    S8        FileName[MAX_STRING_LENGTH]; //在线模式
};

//Gcode M24命令的参数
struct ArgM24_t
{
    BOOL    HasS;
    BOOL    HasT;

    S32     S;      //要恢复的位置
    S32     T;      //打印开始后经过的时间
};

//Gcode M26命令的参数
struct ArgM26_t
{
    BOOL    HasS;

    S32     S; //下一个文件的读取位置
};

//Gcode M27命令的参数
struct ArgM27_t
{
    BOOL    HasC;
    BOOL    HasS;

    S32     S;      //自动报告之间的间隔。S0禁用。
    S32     C;      //报告当前文件的文件名和长文件名。
};

//Gcode M28命令的参数
struct ArgM28_t
{
    BOOL      BinaryModel;   //是否是二进制模式，0表示字符模式，1表示二进制模式
    S8        FileName[MAX_STRING_LENGTH]; //要写入文件的名称
};

//Gcode M30命令的参数
struct ArgM30_t
{
    S8        FileName[MAX_STRING_LENGTH]; //要删除文件的名称
};

//Gcode M32命令的参数
struct ArgM32_t
{
    BOOL    HasP;
    BOOL    HasS;
    BOOL    HasStr; 

    S32     P;      //子程序标识
    S32     S;      //自动报告之间的间隔。S0禁用。
    S8      Str[MAX_STRING_LENGTH]; 
};

//Gcode M33命令的参数
struct ArgM33_t
{
    S8        Path[MAX_STRING_LENGTH]; //要删除文件的名称
};

//Gcode M34命令的参数
struct ArgM34_t
{
    BOOL    HasF;
    BOOL    HasS;

    S8      F;      //文件夹排序  ，-1：文件夹优先于文件，0：不排序，1：文件优先于文件夹
    BOOL    S;      //是否排序
};

//Gcode M42命令的参数
struct ArgM42_t
{
    BOOL    HasI;
    BOOL    HasM;
    BOOL    HasP;
    BOOL    HasS;

    BOOL    I;      //忽略马林使用的引脚保护
    BOOL    M;      //管脚模式
    U16     S;        //要设置的状态。PWM引脚可以设置在0-255之间
    S32     P;        //数字管脚编号
};

//Gcode M43命令的参数
struct ArgM43_t
{
    BOOL    HasE;
    BOOL    HasI;
    BOOL    HasP;
    BOOL    HasT;
    BOOL    HasS;
    BOOL    HasW;
    BOOL    HasR;
    BOOL    HasL;

    BOOL    E;      //监视制动块
    S8      S;        //使用P指定伺服索引（0-3）。如果省略P，则默认为0
    S16     I;      //报告值时忽略保护
    S32     P;        //数字管脚编号
    S16     W;        //监视管脚
    S16     T;        //切换管脚
    S16     R;        //次数
    S16     L;        //结束管脚
};

//Gcode M43 T命令的参数
struct ArgM43T_t
{
    BOOL    HasI;
    BOOL    HasL;
    BOOL    HasP;
    BOOL    HasR;
    BOOL    HasS;
    BOOL    HasW;
//    BOOL    IsDefaultL;  //是否是
//    BOOL    IsDefaultW;  //是否是

    S8      I;      //忽略marlin引脚保护标志
    S8      R;        //重复脉冲次数，如果未给定，则默认为1。在继续下一个引脚之前，在每个引脚上重复此次数的脉冲。如果未给定，则默认为1。
    S16     L;        //结束端号。如果未给出，将默认为该板定义的最后一个管脚
    S16     P;        //起始端号。如果未给定，将默认为0
    S32     W;        //等待时间（毫秒）转换。如果未给出，则默认为500。
    S32     S;        //用指定的分度值对伺服执行Z伺服探头测试。
};

//Gcode M48命令的参数
struct ArgM48_t
{
    BOOL    HasE;
    BOOL    HasL;
    BOOL    HasP;
    BOOL    HasS;
    BOOL    HasV;
    BOOL    HasX;
    BOOL    HasY;  

    S32     E;      //为每个探头接合
    S32     L;        //要探测的支腿数
    S32     P;        //要执行的探测数
    S8      S;        //<0|1> , 0:圆形图案, 1:星形图案。
    S16     V;        //详细级别
    FLOAT   X;        //X 位置
    FLOAT   Y;        //Y 位置
};

//Gcode M73命令的参数
struct ArgM73_t
{
    BOOL        HasP;

    S16         P; //当前打印进度百分比
};

//Gcode M80命令的参数
struct ArgM80_t
{
    BOOL        HasS;

    S16         S; //报告电源状态
};

//Gcode M85命令的参数
struct ArgM85_t
{
    BOOL        HasS;

    S32         S; //最大非活动秒数
};

//Gcode M92命令的参数
struct ArgM92_t
{
    BOOL    HasE;
    BOOL    HasT;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    S16     T;      //目标挤出机
    FLOAT   E;        //E轴每单位步数
    FLOAT   X;      //X轴每单位步数
    FLOAT   Y;        //Y轴每单位步数
    FLOAT   Z;        //Z轴每单位步数
};

//Gcode M100命令的参数
struct ArgM100_t
{
    BOOL    HasC;    
    BOOL    HasD;    //将空闲内存块从\uUbrkVAL转储到堆栈指针。
    BOOL    HasF;   //返回内存池中的可用字节数以及定义内存池的其他重要统计信息。
    BOOL    HasI;   //初始化空闲内存池，以便可以监视它并打印定义空闲内存池的重要统计信息。

    S32     C;      //空闲内存池中损坏的n个位置，并报告损坏的位置。这对于检查M100 D和M100 F命令的正确性非常有用。
};

//Gcode M104命令的参数
struct ArgM104_t
{
    BOOL    HasB;
    BOOL    HasF;
    BOOL    HasI;
    BOOL    HasS;
    BOOL    HasT;
    BOOL    IsDefaultT;  //是否省略了热端索引

    S8      F;      //AUTOTEMP标志。忽略禁用自动温度。
    S16     B;      //最高自动温度
    S16     S;        //目标温度或最小自动温度
    S32     I;      //物料预置索引
    S32     T;        //热端索引，如果省略，将使用当前活动的热端。
};

//Gcode M105命令的参数
struct ArgM105_t
{
    BOOL    HasT;

    S32     T;      //热端索引
};

//Gcode M106命令的参数
struct ArgM106_t
{
    BOOL    HasI;
    BOOL    HasP;
    BOOL    HasS;
    BOOL    HasT;

    S32     I;        //材料预设索引
    S32     P;      //风扇索引
    FLOAT   S;        //速度
    S32     T;      //二级速度  M106 P<fan> T3-255为设置次要速度<fan>。M106 P<fan> T2 使用设置的二级速度。M106 P<fan> T1 恢复以前的风扇速度。
};

//Gcode M107命令的参数
struct ArgM107_t
{
    BOOL    HasP;

    S32     P;      //风扇索引
};

//Gcode M109命令的参数
struct ArgM109_t
{
    BOOL    HasB;
    BOOL    HasF;
    BOOL    HasI;
    BOOL    HasR;
    BOOL    HasS;
    BOOL    HasT;
    BOOL    IsDefaultT;  //是否是默认热端

    S16     F;        //自动温度标志。忽略禁用自动温度。
    S16     B;      //最高自动温度。
    S16     R;      //目标温度（等待冷却或加热）。
    S16     S;        //目标温度（仅在加热时等待）。另外AUTOTEMP：最低自动温度。
    S32     I;        //材料预设索引
    S32     T;        //热端索数。如果省略，将使用当前活动的hotend。
};

//Gcode M110命令的参数
struct ArgM110_t
{
    BOOL    HasN;

    S32     N;      //行号
};

//Gcode M111命令的参数
struct ArgM111_t
{
    BOOL    HasS;

    S16     S;      //调试标志位
};

//Gcode M113命令的参数
struct ArgM113_t
{
    BOOL    HasS;

    S16     S;      //保持活动间隔时间
};

//Gcode M114命令的参数
struct ArgM114_t
{
    BOOL    HasD;
    BOOL    HasE;
    BOOL    HasR;

    BOOL    D;      //详细信息
};

//Gcode M117命令的参数
struct ArgM117_t
{
    BOOL    HasStr;

    S8      Str[MAX_STRING_LENGTH];      //LCD状态信息
};

//Gcode M118命令的参数
struct ArgM118_t
{
    BOOL    HasA1;  //  前缀"//" 表示注释或操作命令。
    BOOL    HasE1;    //  前置echo:的消息。
    BOOL    HasP;
    BOOL    HasStr;    

    S32     P;
    S8      Str[MAX_STRING_LENGTH];      //消息字符串
};

//Gcode M122命令的参数
struct ArgM122_t
{
    BOOL    HasE;
    BOOL    HasI;
    BOOL    HasS;
    BOOL    HasV;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    S8      E;        //仅目标E驱动程序。
    S8      I;      //标记以使用当前设置重新初始化步进驱动器。
    S8      S;      //标记以启用/禁用连续调试报告。
    S8      V;        //报告原始寄存器数据。
    S8      X;        //仅目标X驱动程序。
    S8      Y;        //仅目标Y驱动程序。
    S8      Z;        //仅目标Z驱动程序。
};

//Gcode M125命令的参数
struct ArgM125_t
{
    BOOL    HasL;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;
    BOOL    HasP;

    BOOL    P;      //
    FLOAT   L;      //缩回长度
    FLOAT   X;      //停在的X位置
    FLOAT   Y;      //停在的Y位置
    FLOAT   Z;      //停在的Z位置
};

//Gcode M126命令的参数
struct ArgM126_t
{
    BOOL    HasS;

    S32     S;      //阀门压力
};

//Gcode M128命令的参数
struct ArgM128_t
{
    BOOL    HasS;

    S32     S;      //阀门压力
};

//Gcode M140命令的参数
struct ArgM140_t
{
    BOOL    HasI;
    BOOL    HasS;

    S16     I;      //材料预设索引
    S32     S;      //目标温度。
};

//Gcode M141命令的参数
struct ArgM141_t
{
    BOOL    HasS;

    S32     S;      //目标温度或最低自动温度
};

//Gcode M145命令的参数
struct ArgM145_t
{
    BOOL    HasB;
    BOOL    HasF;
    BOOL    HasH;
    BOOL    HasS;

    S16     B;        //热床温度
    S16     H;        //热端温度
    S32     S;        //材料索引
    FLOAT   F;        //风扇转速
};

//Gcode M149命令的参数
struct ArgM149_t
{
    BOOL    HasC;   //摄氏温度
    BOOL    HasF;    //华氏温度
    BOOL    HasK;    //开尔文
};

//Gcode M150命令的参数
struct ArgM150_t
{
    BOOL    HasB;
    BOOL    HasI;
    BOOL    HasP;
    BOOL    HasR;
    BOOL    HasU;
    BOOL    HasW;
    BOOL    HasS;

    U8      B;        //蓝色分量从0到255
    U8      P;        //亮度从0到255（需要NEOPIXEL_LED）
    U8      R;        //红色分量从0到255
    U8      U;        //绿色分量从0到255
    U8      W;        //白色分量从0到255（RGBW_LED或NEOPIXEL_LED仅）
    U32     I;        //从0到n的索引（需要NEOPIXEL_LED）
    S32     S;
};

//Gcode M155命令的参数
struct ArgM155_t
{
    BOOL    HasS;

    S32     S;      //自动报告之间的时间间隔（以秒为单位）。S0禁用
};

//Gcode M163命令的参数
struct ArgM163_t
{
    BOOL    HasP;
    BOOL    HasS;

    S16     S;      //组件索引。
    FLOAT   P;      //混合系数
};

//Gcode M164命令的参数
struct ArgM164_t
{
    BOOL    HasS;
    BOOL    HasValueS;
    S32     S;      //工具索引
};

//Gcode M165命令的参数
struct ArgM165_t
{
    BOOL    HasA;
    BOOL    HasB;
    BOOL    HasC;
    BOOL    HasD;
    BOOL    HasH;
    BOOL    HasI;

    FLOAT   A;        //混合系数1
    FLOAT   B;      //混合系数2
    FLOAT   C;        //混合系数3
    FLOAT   D;        //混合系数4
    FLOAT   H;        //混合系数5
    FLOAT   I;      //混合系数6
};

//Gcode M166命令的参数
struct ArgM166_t
{
    BOOL    HasA;
    BOOL    HasI;
    BOOL    HasJ;
    BOOL    HasS;
    BOOL    HasT;
    BOOL    HasZ;

    BOOL    S;        //在手动模式下启用/禁用渐变
    S32     I;      //启动模拟工具
    S32     J;        //结束虚拟工具
    S8   T;        //重新分配给渐变的工具索引
    FLOAT   A;        //起始Z高度
    FLOAT   Z;      //结束Z高度
};

//Gcode M190命令的参数
struct ArgM190_t
{
    BOOL    HasI;
    BOOL    HasR;
    BOOL    HasS;

    S32     I;        //材料预设索引
    S16     R;      //目标温度（等待冷却或加热）
    S16     S;        //目标温度（仅在加热时等待）
};

//Gcode M191命令的参数
struct ArgM191_t
{
    BOOL    HasR;
    BOOL    HasS;

    S16     R;      //目标温度（等待冷却或加热）
    S16     S;        //目标温度（仅在加热时等待）
};

//Gcode M192命令的参数
struct ArgM192_t
{
    BOOL    HasR;
    BOOL    HasS;

    S16     R;        //目标温度（等待冷却或加热）
    S16     S;        //目标温度（仅在加热时等待）
};

//Gcode M200命令的参数
struct ArgM200_t
{
    BOOL    HasD;
    BOOL    HasT;
    BOOL    HasL;
    BOOL    HasS;

    S16     T;      //挤出机的索引
    S16     S;      //挤出机容积模式启动
    FLOAT   D;      //细丝直径
    FLOAT   L;      //挤出速度的极限（单位： mm3/sec）

};

//Gcode M201命令的参数
struct ArgM201_t
{
    BOOL    HasE;
    BOOL    HasT;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    S16     T;        //目标挤出机索引
    FLOAT   E;      //E轴最大加速度
    FLOAT   X;        //X轴最大加速度
    FLOAT   Y;        //Y轴最大加速度
    FLOAT   Z;      //Z轴最大加速度
};

//Gcode M203命令的参数
struct ArgM203_t
{
    BOOL    HasE;
    BOOL    HasT;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    S16     T;        //目标挤出机索引
    FLOAT   E;      //E轴最大加速度
    FLOAT   X;        //X轴最大加速度
    FLOAT   Y;        //Y轴最大加速度
    FLOAT   Z;      //Z轴最大加速度
};

//Gcode M204命令的参数
struct ArgM204_t
{
    BOOL    HasP;
    BOOL    HasR;
    BOOL    HasS;
    BOOL    HasT;

    FLOAT   P;      //打印加速度
    FLOAT   R;        //收回加速度
    FLOAT   S;        //用于加速运动的旧版参数。设置打印和行进加速。
    FLOAT   T;      //行驶加速
};

//Gcode M205命令的参数
struct ArgM205_t
{
    BOOL    HasB;
    BOOL    HasE;
    BOOL    HasJ;
    BOOL    HasS;
    BOOL    HasT;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    S32     B;      //最小段时间（微秒）
    FLOAT   E;        //E最大加速度（单位/秒）
    FLOAT   J;        //结点偏差（需要JUNCTION_DEVIATION）
    FLOAT   S;      //印刷移动的最小进给速度（单位/秒）
    FLOAT   T;      //行程最小进给速度（单位/秒）
    FLOAT   X;        //X最大加速度（单位/秒）
    FLOAT   Y;        //Y最大加速度（单位/秒）
    FLOAT   Z;      //Z最大加速度（单位/秒）
};

//Gcode M206命令的参数
struct ArgM206_t
{
    BOOL    HasP;
    BOOL    HasT;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    FLOAT   P;      //SCARA Psi偏移量
    FLOAT   T;      //SCARA Theta偏移量
    FLOAT   X;        //X原点偏移
    FLOAT   Y;        //Y原点偏移
    FLOAT   Z;      //Z原点偏移
};

//Gcode M207命令的参数
struct ArgM207_t
{
    BOOL    HasF;
    BOOL    HasS;
    BOOL    HasW;
    BOOL    HasZ;

    FLOAT   F;      //退刀速度（单位/分钟）
    FLOAT   S;      //收放长度
    FLOAT   W;        //缩回交换长度（多挤出机）
    FLOAT   Z;      //收回时Z抬起高度
};

//Gcode M208命令的参数
struct ArgM208_t
{
    BOOL    HasF;
    BOOL    HasR;
    BOOL    HasS;
    BOOL    HasW;

    FLOAT   F;      //恢复进给速度（单位/分钟）
    FLOAT   R;      //交换恢复进给速度（单位/分钟）
    FLOAT   S;      //额外的恢复长度
    FLOAT   W;        //额外的恢复交换长度。
};

//Gcode M209命令的参数
struct ArgM209_t
{
    BOOL    HasS;

    BOOL    S;  //  开启/关闭自动缩进
};

//Gcode M211命令的参数
struct ArgM211_t
{
    BOOL    HasS;

    BOOL    S;  //  软件停止状态（S1 =启用S0 =禁用）
};

//Gcode M217命令的参数
struct ArgM217_t
{
    BOOL    HasA;
    BOOL    HasB;
    BOOL    HasE;
    BOOL    HasF;
    BOOL    HasG;
    BOOL    HasL;
    BOOL    HasQ;
    BOOL    HasP;
    BOOL    HasR;
    BOOL    HasS;
    BOOL    HasT;
    BOOL    HasU;
    BOOL    HasV;
    BOOL    HasW;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    BOOL    A;      //
    S16     P;      //主进给速度
    S16     R;      //回缩进给速度
    S16     U;      //
    S16     F;      //
    S16     G;      //
    S16     X;      //驻车X位置
    S16     Y;      //驻车Y位置
    S16     L;      //
    S16     T;      //
    FLOAT   S;      //交换长度
    FLOAT   B;      //交换长度
    FLOAT   E;      //交换长度
    FLOAT   V;      //
    FLOAT   W;      //
    FLOAT   Z;      //Z上升速率
};

//Gcode M218命令的参数
struct ArgM218_t
{
    BOOL    HasT;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;
    BOOL    IsDefaultT;  //热端索引是否是默认情况

    S16     T;      //热端索引。默认情况下为主动挤出机。（如果小于0为默认情况）
    FLOAT   X;      //热端X偏移
    FLOAT   Y;      //热端Y偏移
    FLOAT   Z;      //热端Z偏移。需要DUAL_X_CARRIAGE或SWITCHING_NOZZLE。
};

//Gcode M220命令的参数
struct ArgM220_t
{
    BOOL    HasB;
    BOOL    HasR;
    BOOL    HasS;

    FLOAT   B;        //备用电流因素
    FLOAT   R;        //恢复最后保存的因子
    FLOAT   S;        //进给率百分比.
};

//Gcode M221命令的参数
struct ArgM221_t
{
    BOOL    HasS;
    BOOL    HasT;
    BOOL    IsDefaultT;  //是否是默认挤出机

    S8      T;        //目标挤出机索引。默认为活动挤出机
    FLOAT   S;      //进给率百分比.
};

//Gcode M226命令的参数
struct ArgM226_t
{
    BOOL    HasS;
    BOOL    HasP;

    S16     S;      //状态0或1。默认值-1表示反相。
    S32     P;        //引脚编号
};

//Gcode M240命令的参数
struct ArgM240_t
{
    BOOL    HasA;
    BOOL    HasB;
    BOOL    HasD;
    BOOL    HasF;
    BOOL    HasI;
    BOOL    HasJ;
    BOOL    HasP;
    BOOL    HasR;
    BOOL    HasS;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;
    BOOL    IsDefaultF;
    BOOL    IsDefaultI;
    BOOL    IsDefaultJ;
    BOOL    HasValueA;
    BOOL    HasValueB;
    BOOL    HasValueD;
    BOOL    HasValueF;
    BOOL    HasValueI;
    BOOL    HasValueJ;
    BOOL    HasValueP;
    BOOL    HasValueR;
    BOOL    HasValueS;
    BOOL    HasValueX;
    BOOL    HasValueY;
    BOOL    HasValueZ;

    FLOAT   A;      //偏移到X返回位置
    FLOAT   B;      //偏移到Y返回位置
    S32     D;      //按住快门开关的持续时间。(单位：ms)
    FLOAT   F;        //主照片的进给速度移动。如果省略，将使用原点进给速度。
    FLOAT   I;        //快门开关X位置。如果省略，则应用照片移动X位置。
    FLOAT   J;      //快门开关Y位置。如果省略，则应用照片移动Y位置。
    S32     P;      //按下快门开关后延迟。 (单位：ms)
    FLOAT   R;      //缩回/恢复长度。
    FLOAT   S;      //撤回/恢复进给率。
    FLOAT   X;        //主照片移动X位置。
    FLOAT   Y;        //主照片移动到Y位置。
    FLOAT   Z;      //主照片移动Z抬高。
};

//Gcode M250命令的参数
struct ArgM250_t
{
    BOOL    HasC;

    S32     C;  //  对比值
};

//Gcode M260命令的参数
struct ArgM260_t
{
    BOOL    HasA;
    BOOL    HasB;
    BOOL    HasR;
    BOOL    HasS;

    S32     A;        //发送到的总线地址
    U8      B;        //要添加到缓冲区的字节数
    BOOL    R;        //重置并倒回I2C缓冲区
    BOOL    S;        //发送标志。将缓冲区刷新到总线。
};

//Gcode M261命令的参数
struct ArgM261_t
{
    BOOL    HasA;
    BOOL    HasB;

    S16     B;      //请求的字节数
    S32     A;        //从中请求字节的总线地址
};

//Gcode M280命令的参数
struct ArgM280_t
{
    BOOL    HasP;
    BOOL    HasS;
    //BOOL    IsDefaultS;

    S32     P;        //伺服索引
    S32     S;      //设定伺服位置。省略以读取当前位置。
};

//Gcode M281命令的参数
struct ArgM281_t
{
    BOOL    HasL;
    BOOL    HasP;
    BOOL    HasU;

    FLOAT   L;        //部署角度（以度为单位）。
    S32     P;        //伺服索引
    FLOAT   U;      //收起角度（以度为单位）。
};

//Gcode M290命令的参数
struct ArgM290_t
{
    BOOL    HasP;
    BOOL    HasS;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    S16     P;      //使用P0离开测头Z偏置不受影响。
    FLOAT   S;      //Z的别名
    FLOAT   X;        //X轴上的距离
    FLOAT   Y;        //Y轴上的距离
    FLOAT   Z;      //Z轴上的距离
};

//Gcode M300命令的参数
struct ArgM300_t
{
    BOOL    HasP;
    BOOL    HasS;
    
    S32     P;        //持续时间（1秒） 单位是ms
    S32     S;      //频率（260Hz）单位是Hz
};

//Gcode M301命令的参数
struct ArgM301_t
{
    BOOL    HasC;
    BOOL    HasD;
    BOOL    HasE;
    BOOL    HasF;
    BOOL    HasI;
    BOOL    HasL;
    BOOL    HasP;

    S16     E;      //设定挤出机索引。默认值0。
    FLOAT   C;      //C项
    FLOAT   D;      //衍生价值
    FLOAT   F;      //衍生价值
    FLOAT   I;      //积分值
    FLOAT   L;      //挤出扩展队列长度
    FLOAT   P;      //比例价值
};

//Gcode M302命令的参数
struct ArgM302_t
{
    BOOL    HasP;
    BOOL    HasS;
    
    S32     P;        //标记以允许在任何温度下挤出
    S32     S;      //最低温度以确保安全挤出
};

//Gcode M303命令的参数
struct ArgM303_t
{
    BOOL    HasC;
    BOOL    HasD;
    BOOL    HasE;
    BOOL    HasS;
    BOOL    HasU;

    S16     C;      //周期。至少需要3个循环。默认值5。
    S16     D;      //切换PID_DEBUG输出激活。
    S16     E;        //热端索引（加热床为-1）。默认值0。
    S16     S;        //目标温度
    S16     U;      //使用PID结果。（否则只需将其打印出来。）
};

//Gcode M304命令的参数
struct ArgM304_t
{
    BOOL    HasD;
    BOOL    HasI;
    BOOL    HasP;

    FLOAT   D;        //衍生价值
    FLOAT   I;        //积分值
    FLOAT   P;      //比例价值
};

//Gcode M305命令的参数
struct ArgM305_t
{
    BOOL    HasB;
    BOOL    HasC;
    BOOL    HasP;
    BOOL    HasR;
    BOOL    HasT;

    S16     P;        //热敏电阻表索引
    FLOAT   B;        //热敏电阻“β”值
    FLOAT   R;        //上拉电阻值
    FLOAT   T;        //25℃时的电阻
    FLOAT   C;        //斯坦哈特哈特系数“ C”
};

//Gcode M350命令的参数
struct ArgM350_t
{
    BOOL    HasB;
    BOOL    HasE;
    BOOL    HasS;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    U8      B;      //为第5个步进驱动器设置微步进。<1|2|4|8|16>
    U8      E;      //为E0步进驱动器设置微步进。<1|2|4|8|16>
    U8      S;      //为所有5个步进驱动器设置微步进。<1|2|4|8|16>
    U8      X;        //为X步进驱动器设置微步进。<1|2|4|8|16>
    U8      Y;        //为Y步进驱动器设置微步进。<1|2|4|8|16>
    U8      Z;      //为Z步进驱动器设置微步进。<1|2|4|8|16>
};

//Gcode M351命令的参数
struct ArgM351_t
{
    BOOL    HasB;
    BOOL    HasE;
    BOOL    HasS;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    U8      B;      //设置第五个步进驱动器的MS1 / 2引脚。<0|1>
    U8      E;      //设置E步进驱动器的MS1 / 2引脚。<0|1>
    U8      S;      //选择要为所有指定轴设置的销。<1|2>。S1： 为所有已设置的轴选择引脚MS1。 S2： 为所有已设置的轴选择引脚MS2。
    U8      X;        //设置X步进驱动器的MS1 / 2引脚。<0|1>
    U8      Y;        //设置Y步进驱动器的MS1 / 2引脚。<0|1>
    U8      Z;      //设置Z步进驱动器的MS1 / 2引脚。<0|1>
};

//Gcode M355命令的参数
struct ArgM355_t
{
    BOOL    HasP;
    BOOL    HasS;
    BOOL    HasValueP;
    BOOL    HasValueS;

    U8      P;        //将亮度因子设置为0至255。
    BOOL    S;      //打开或关闭机箱灯。
};

//Gcode M380命令的参数
struct ArgM380_t
{
    BOOL    HasS;   //

    S16     S;      //挤出机电磁阀索引
};

//Gcode M381命令的参数
struct ArgM381_t
{
    BOOL    HasS;   //没有索引，则停用所有的螺线管

    S16     S;      //螺线管索引
};

//Gcode M403命令的参数
struct ArgM403_t
{
    BOOL    HasE;
    BOOL    HasF;
    
    U8      F;        //长丝类型<0|1|2>。F0： 默认值（PLA，PETG等）F1： 柔丝 F2： 聚乙烯醇
    S32     E;      //索引 ,MMU2插槽[0..4]设置材料类型
};

//Gcode M404命令的参数
struct ArgM404_t
{
    BOOL    HasW;

    FLOAT   W;        //新的名义上的宽度值
};

//Gcode M405命令的参数
struct ArgM405_t
{
    BOOL    HasD;
    BOOL    IsDefaultD;  //是否是默认情况

    FLOAT   D;        //从测量点到热端的距离。如果未给出，将使用以前的值。默认启动值由设置MEASUREMENT_DELAY_CM。
};

//Gcode M412命令的参数
struct ArgM412_t
{
    BOOL    HasS;   //如果省略，将报告当前启用状态。

    BOOL    S;      //启用或禁用细丝跳动检测。
};

//Gcode M413命令的参数
struct ArgM413_t
{
    BOOL    HasS;   //如果省略，将报告当前启用状态。
    BOOL    HasR;
    BOOL    HasL;
    BOOL    HasW;
    BOOL    HasP;
    BOOL    HasO;
    BOOL    HasE;
    BOOL    HasV;

    BOOL    S;      //以启用或禁用掉电恢复
};

//Gcode M420命令的参数
struct ArgM420_t
{
    BOOL    HasC;
    BOOL    HasL;
    BOOL    HasS;
    BOOL    HasT;
    BOOL    HasV;
    BOOL    HasZ;
    BOOL    HasValueL;
    BOOL    HasValueS;

    U8      T;        //格式化以打印网格数据<0|1|4>。T0： 可读。 T1： CSV。 T4： 紧凑
    BOOL    S;      //设置启用或禁用。需要有效的网格以启用床平整。如果网格无效/不完整的校平将不会启用。
    U16     C;      //将网格居中于最低和最高点的平均值上
    U16     V;        //详细：打印存储的网格/矩阵数据
    U32     L;      //从EEPROM索引加载网格（需要AUTO_BED_LEVELING_UBL和EEPROM_SETTINGS）
    FLOAT   Z;      //设置Z的淡入高度
};

//Gcode M421命令的参数
struct ArgM421_t
{
    BOOL    HasC;
    BOOL    HasI;
    BOOL    HasJ;
    BOOL    HasN;
    BOOL    HasQ;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    BOOL    HasValueI;  //I = (HasValueI == true) ? I : -1;
    BOOL    HasValueJ;
    BOOL    HasValueQ;
    BOOL    HasValueX;
    BOOL    HasValueY;
    BOOL    HasValueZ;

    S16     C;      //将网格点设置为最接近当前喷嘴位置（AUTO_BED_LEVELING_UBL仅）
    S16     N;      //将网格点设置为undefined（AUTO_BED_LEVELING_UBL仅）
    S32     I;      //X索引到网格数组
    S32     J;      //Y索引到网格阵列
    FLOAT   Q;      //要添加到现有Z值的值
    FLOAT   X;        //X位置（应该非常靠近网格线）（AUTO_BED_LEVELING_UBL仅）
    FLOAT   Y;        //Y位置（应该非常靠近网格线）（AUTO_BED_LEVELING_UBL仅）
    FLOAT   Z;      //设置的新Z值
};

//Gcode M422命令的参数
struct ArgM422_t
{
    BOOL    HasS;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasR;
    BOOL    HasW;

    S32     S;        //Z步进索引
    FLOAT   X;        //X位置
    FLOAT   Y;      //Y位置
    S32     W;
};

//Gcode M425命令的参数
struct ArgM425_t
{
    BOOL    HasF;    //
    BOOL    HasS;     //
    BOOL    HasX;     //
    BOOL    HasY;    //
    BOOL    HasZ;    //
    BOOL    HasZM;   //当MEASURE_BACKLASH_WHEN_PROBING启用时，负载测量的间隙进入间隙距离参数

    FLOAT   F;        //启用或禁用反冲校正，或设置中间淡出（0.0 =无； 1.0 = 100％）
    FLOAT   S;      //齿隙校正的传播距离
    FLOAT   X;      //将反冲距离设置为X（毫米；禁用为0）
    FLOAT   Y;        //将反冲距离设置为Y（毫米；禁用为0）
    FLOAT   Z;      //将反冲距离设置为Z（毫米；禁用为0）
    FLOAT   ZM;      //当MEASURE_BACKLASH_WHEN_PROBING启用时，负载测量的间隙进入间隙距离参数
};

struct ArgM430_t
{
    BOOL HasI;
    BOOL HasV;
    BOOL HasW;

    BOOL I;
    BOOL V;
    BOOL W;
};

//Gcode M486命令的参数
struct ArgM486_t
{
    BOOL    HasC;
    BOOL    HasP;
    BOOL    HasS;
    BOOL    HasT;
    BOOL    HasU;

    U16     C;        //取消当前对象。
    S32     P;      //取消具有给定索引的对象。
    U32     S;      //设置当前对象的索引。如果具有给定索引的对象已被取消，这将导致固件跳至下一个对象。值-1用于指示不是对象的某些内容，不应跳过。
    U32     T;        //重置状态并设置对象数。
    U32     U;      //用给定索引取消取消对象。
};

//Gcode M503命令的参数
struct ArgM503_t
{
    BOOL    HasS;   //

    BOOL    S;      //详细的输出标志。（true如果省略。）
};


//Gcode M504命令的参数 在MARLIN_DEV_MODE开启后才有效果
struct ArgM504_t
{
    BOOL    HasW;   //
    BOOL    HasV;
    BOOL    HasT;
    BOOL    HasR;

    U16    W;      //
    U16    R;
    U8     V;
    U16    T;
};

//Gcode M540命令的参数
struct ArgM540_t
{
    BOOL    HasS;   //

    S16     S;      //在结束器命中时无论是1还是非0都结束SD卡打印。
};

//Gcode M569命令的参数
struct ArgM569_t
{
    BOOL    HasE;    //
    BOOL    HasI;     //
    BOOL    HasT;   //
    BOOL    HasX;     //
    BOOL    HasY;    //
    BOOL    HasZ;    //
    BOOL    HasS;

    S8      S;      //步进器的模式
//    S8      E;      //E0步进器的步进模式
//    S8      X;      //X步进器的步进模式
//    S8      Y;      //Y步进器的步进模式
//    S8      Z;      //Z步进器的步进模式
    S16     I;      //双步进的索引。使用I1了X2，Y2，和/或Z2，以及I2为Z3。
    S16     T;      //E轴的索引（工具）编号。如果未指定，则为E0挤出机。
};

//Gcode M575命令的参数
struct ArgM575_t
{
    BOOL    HasB;
    BOOL    HasP;

    S16     P;        //串行端口索引。
    S32     B;        //要设置的波特率。允许的值为：2400（24），9600（96），19200（19，192），38400（38，384），57600（57，576），115200（115，1152），250000（250），500000（500），1000000
};

//Gcode M600命令的参数
struct ArgM600_t
{
    BOOL    HasB;
    BOOL    HasE;
    BOOL    HasL;
    BOOL    HasT;
    BOOL    HasU;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;
    BOOL    HasR;

    S16     B;      //提示用户更换灯丝的蜂鸣次数（默认FILAMENT_CHANGE_ALERT_BEEPS）
    S16     T;      //目标挤出机索引
    FLOAT   R;      //恢复的温度
    FLOAT   E;      //移至更改位置之前先缩回（负值，默认PAUSE_PARK_RETRACT_LENGTH）
    FLOAT   L;      //负载长度，对于鲍登来说更长（负）
    FLOAT   U;      //卸料量（负）
    FLOAT   X;      //长丝更换的X位置
    FLOAT   Y;      //长丝更换的Y位置
    FLOAT   Z;      //长丝更换位置的Z相对升程
};

//Gcode M603命令的参数
struct ArgM603_t
{
    BOOL    HasL;
    BOOL    HasT;
    BOOL    HasU;

    S8      T;        //目标挤出机索引。
    FLOAT   L;        //负载长度，对于更长
    FLOAT   U;        //卸料量（负）
};

//Gcode M605命令的参数
struct ArgM605_t
{
    BOOL    HasR;
    BOOL    HasS;
    BOOL    HasX;
    BOOL    HasW;
    BOOL    HasE;
    BOOL    HasP;

    BOOL    HasValueS;
    BOOL    HasValueE;
    BOOL    HasValueP;
    
    S8      S;        //为所有指定轴设置的销<0|1|2>。0：完全控制模式，1：自动停车模式，2：复制模式。
    S8      W;
    S32     R;        //温差适用于E1。（要求DUAL_X_CARRIAGE）
    FLOAT   X;        //双X托架之间的X距离。（要求DUAL_X_CARRIAGE）
    S16     E;
    S16     P;
};

//Gcode M665 Delta命令的参数
struct ArgM665Delta_t
{
    BOOL    HasB;
    BOOL    HasH;
    BOOL    HasL;
    BOOL    HasR;
    BOOL    HasS;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    FLOAT   B;      //增量校准半径
    FLOAT   H;      //三角洲高度
    FLOAT   L;      //对角杆
    FLOAT   R;      //三角半径
    FLOAT   S;      //每秒段数
    FLOAT   X;        //Alpha（1号塔）角度修剪
    FLOAT   Y;        //Beta（第2座）斜角装饰
    FLOAT   Z;      //伽玛（3号塔）角修剪
};

//Gcode M665 Scara命令的参数
struct ArgM665Scara_t
{
    BOOL    HasA;
    BOOL    HasB;
    BOOL    HasP;
    BOOL    HasS;
    BOOL    HasT;
    BOOL    HasX;
    BOOL    HasY;

    FLOAT   A;      //Theta-Psi偏移量，别名 P
    FLOAT   B;      //Theta偏移量，别名 T
    FLOAT   P;      //Theta-Psi偏移，添加到肩角（A / X）
    FLOAT   S;      //每秒段数
    FLOAT   T;      //θ偏移，添加到肘部（B / Y）角
    FLOAT   X;        //Theta-Psi偏移量，别名 P
    FLOAT   Y;        //Theta偏移量，别名 T
};

union ArgM665_t
{
    struct ArgM665Delta_t Delta;
    struct ArgM665Scara_t Scara;
};

//Gcode M665 Delta命令的参数
struct ArgM666Delta_t
{
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    FLOAT   X;        //调整X执行器挡块
    FLOAT   Y;        //调整Y执行器挡块
    FLOAT   Z;        //调整Z执行器挡块
};

//Gcode M665 dual endstop命令的参数
struct ArgM666DualEndStop_t
{
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    FLOAT   X;        //X轴制动块的偏移
    FLOAT   Y;        //Y轴制动块的偏移
    FLOAT   Z;        //Z轴制动块的偏移
    S32     S;        //Z轴的编号
};

union ArgM666_t     //根据宏开关来选择具体的内容
{
    struct ArgM666Delta_t Delta;
    struct ArgM666DualEndStop_t EndStop;
};

struct ArgM672_t
{
    BOOL    HasR;
    BOOL    HasS;

    BOOL    HasValueS;

    U8      R;
    S8      S;
};

//Gcode M701 命令的参数
struct ArgM701_t
{
    BOOL    HasL;
    BOOL    HasT;
    BOOL    HasZ;
    BOOL    IsDefaultT;  //是否是缺省的T

    S32     T;        //可选的挤出机索引。如果省略，则为当前挤出机。
    FLOAT   L;        //插入的挤出距离（正值）（手动重新加载）
    FLOAT   Z;      //将Z轴移动此距离
};

//Gcode M702 命令的参数
struct ArgM702_t
{
    BOOL    HasU;
    BOOL    HasT;
    BOOL    HasZ;
    BOOL    IsDefaultT;  //是否是缺省的T

    S32     T;        //可选的挤出机索引。如果省略，则为当前挤出机。
    FLOAT   U;        //撤回距离（手动重新加载）
    FLOAT   Z;      //将Z轴移动此距离
};

//Gcode M710命令的参数
struct ArgM710_t
{
    BOOL    HasA;
    BOOL    HasD;
    BOOL    HasI;
    BOOL    HasS;
    BOOL    HasR;

    BOOL    A;      //设置是否自动设置风扇速度。关闭时，控制器风扇的速度将保持在原来的位置。
    BOOL    R;      //将所有设置重置为默认值。可以包含其他参数来覆盖。
    S32     D;      //设置额外的持续时间（单位：秒），以在关闭电动机后保持较高的风扇速度。
    FLOAT   I;      //设置电动机关闭时控制器风扇的速度。
    FLOAT   S;      //设置电动机活动时控制器风扇的速度。
};

//Gcode M851命令的参数
struct ArgM851_t
{
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    FLOAT   X;        //Z探针X偏移
    FLOAT   Y;        //Z探针Y偏移
    FLOAT   Z;      //Z探针Z偏移
};

//Gcode M852命令的参数
struct ArgM852_t
{
    BOOL    HasI;
    BOOL    HasJ;
    BOOL    HasK;
    BOOL    HasS;

    FLOAT   I;        //XY轴的偏斜校正系数。
    FLOAT   J;        //XZ轴的偏斜校正系数
    FLOAT   K;      //YZ轴的偏斜校正系数
    FLOAT   S;      //I仅启用XY偏斜校正时的别名
};

//M860x系列共有命令参数结构体
struct ArgM86x_t
{
    BOOL    HasA;
    BOOL    HasI;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;
    BOOL    HasE;
    BOOL    HasValueA;
    BOOL    HasValueI;

    U8      A;      //Module I2C address.  [30, 200]
    U8      I;      //Module index.  [0, I2CPE_ENCODER_CNT - 1]
    S8      X;      //X axis encoder
    S8      Y;      //Y axis encoder
    S8      Z;      //Z axis encoder
    S8      E;      //E axis encoder

};

//Gcode M860命令的参数
struct ArgM860_t
{
    struct ArgM86x_t *Common;
    BOOL    HasO;
    BOOL    HasU;

    BOOL    O;      //Include homed zero-offset in returned position.
    BOOL    U;      //Units in mm or raw step count.
};

//Gcode M861命令的参数
struct ArgM861_t
{
    struct ArgM86x_t *Common;
};

//Gcode M862命令的参数
struct ArgM862_t
{
    struct ArgM86x_t *Common;
};

struct ArgM863_t
{
    struct ArgM86x_t *Common;

    BOOL    HasP;
    BOOL    HasValueP;
    U8      P;
};

struct ArgM864_t
{
    struct ArgM86x_t *Common;

    BOOL    HasS;
    BOOL    HasValueS;
    U8      S;
};

struct ArgM865_t
{
    struct ArgM86x_t *Common;
};

struct ArgM866_t
{
    struct ArgM86x_t *Common;

    BOOL    HasR;
    U8      R;
};

struct ArgM867_t
{
    struct ArgM86x_t *Common;

    BOOL    HasS;
    BOOL    HasValueS;
    U8      S;
};

struct ArgM868_t
{
    struct ArgM86x_t *Common;

    BOOL    HasT;
    BOOL    HasValueT;
    FLOAT   T;

};

struct ArgM869_t
{
    struct ArgM86x_t *Common;
};

//Gcode M871命令的参数
struct ArgM871_t
{
    BOOL    HasB;   //
    BOOL    HasE;    //
    BOOL    HasI;     //
    BOOL    HasP;     //
    BOOL    HasR;    //
    BOOL    HasV;    //

    S16     R;        //将所有Z调整值重置为出厂默认值（零）。
    S32     B;      //将值存储为床V索引处的Z调整值I。
    S32     E;        //将值V作为Z调整值存储I在挤出机的索引处。
    S32     I;      //存储给定Z调整值的索引。
    S32     P;        //将值V作为Z调整值存储I在Probe的索引处。
    S32     V;      //Z调整值，以微米为单位。
};

//Gcode M876命令的参数
struct ArgM876_t
{
    BOOL    HasS;   //
    S16     S;        //将所有Z调整值重置为出厂默认值（零）。
};

//Gcode M900命令的参数
struct ArgM900_t
{
    BOOL    HasK;
    BOOL    HasL;
    BOOL    HasS;
    BOOL    HasT;
    BOOL    IsDefaultK;

    S16     T;        //挤出机到K，L和S将适用。需要EXTRA_LIN_ADVANCE_K。
    FLOAT   L;        //设置指定挤出机的第二个K因子。需要EXTRA_LIN_ADVANCE_K。请注意，该因素可能无效，并且直到下一个因素才会生效M900 S1。
    FLOAT   K;      //为指定的挤出机设置的K因子。如果省略，则保持不变。将此值设置得更高些，可以使灯丝更柔韧或灯丝路径更长。有了EXTRA_LIN_ADVANCE_K这台主 K系数。请注意，该因素可能无效，并且直到下一个因素才会生效M900 S0。
    FLOAT   S;      //选择插槽并激活最后存储的值。需要EXTRA_LIN_ADVANCE_K。
};

//Gcode M906命令的参数
struct ArgM906_t
{
    BOOL    HasE;
    BOOL    HasI;
    BOOL    HasT;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;
    BOOL    IsDefaultT; 

    S32     I;      //（> = 1.1.9）双步进器的索引。使用I1了X2，Y2，和/或Z2。
    S32     T;      //（> = 1.1.9）E轴的索引（刀具）编号。如果未指定，则为E0挤出机。
    FLOAT   E;      //E0步进器的电流
    FLOAT   X;        //X步进电流
    FLOAT   Y;        //Y步进电流
    FLOAT   Z;      //Z步进电流
};

//Gcode M907命令的参数
struct ArgM907_t
{
    BOOL    HasB;
    BOOL    HasC;
    BOOL    HasD;
    BOOL    HasE;
    BOOL    HasS;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    FLOAT   B;      //E1步进器的电流（需要DIGIPOTSS_PIN或DIGIPOT_I2C）
    FLOAT   C;      //E2步进器的电流（需要DIGIPOT_I2C）
    FLOAT   D;      //E3步进器的电流（需要DIGIPOT_I2C）
    FLOAT   E;      //E0步进器的电流
    FLOAT   S;      //在所有步进器上将此电流设置为（Requires DIGIPOTSS_PIN或DAC_STEPPER_CURRENT）
    FLOAT   X;      //X步进器（和的Y步进器MOTOR_CURRENT_PWM_XY）的电流
    FLOAT   Y;      //当前的Y向步进（使用X同MOTOR_CURRENT_PWM_XY）的电流
    FLOAT   Z;      //Z步进电流
};

//Gcode M908命令的参数
struct ArgM908_t
{
    BOOL    HasP;
    BOOL    HasS;
    
    U32     P;        //引脚（即地址，通道）
    FLOAT   S;      //当前值
};

//Gcode M912命令的参数
struct ArgM912_t
{
    BOOL    HasE;
    BOOL    HasI;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;
    BOOL    IsDefaultI;

    S8      I;      //要设置的步进数。如果省略，则指定所有轴。I0： 都。I1： 基本（X，Y，Z）步进器。I2： 第二个（X2，Y2，Z2）步进器。I3： 第三（Z3）步进器。
    S8      E;      //清除所有或一个E步进驱动器的超温警告标志。E-： 全部E。E0-4： E索引。
    S8      X;        //清除X和/或X2步进驱动器“过热警告”标志
    S8      Y;        //清除Y和/或Y2步进驱动器“过热警告”标志。
    S8      Z;      //清除Z和/或Z2和/或Z3步进驱动器“超温预警”标志。
};

//Gcode M913命令的参数
struct ArgM913_t
{
    BOOL    HasE;
    BOOL    HasI;
    BOOL    HasT;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;
    BOOL    IsDefaultT; 

    S32     I;      //（> = 1.1.9）双步进器的索引。使用I1了X2，Y2，和/或Z2。
    S32     T;      //（> = 1.1.9）E轴的索引（刀具）编号。如果未指定，则为E0挤出机。
    S32     E;      //将E0_HYBRID_THRESHOLDand E1_HYBRID_THRESHOLD和E2_HYBRID_THRESHOLDand E3_HYBRID_THRESHOLD设置为提供的值。
    S32     X;        //将X_HYBRID_THRESHOLD和设置X2_HYBRID_THRESHOLD为提供的值。
    S32     Y;        //将Y_HYBRID_THRESHOLD和设置Y2_HYBRID_THRESHOLD为提供的值。
    S32     Z;      //将Z_HYBRID_THRESHOLD和设置Z2_HYBRID_THRESHOLD为提供的值。
};

//Gcode M914命令的参数
struct ArgM914_t
{
    BOOL    HasI;
    BOOL    HasX;
    BOOL    HasY;
    BOOL    HasZ;

    S32     I;      //（> = 1.1.9）双步进器的索引。使用I1了X2，Y2，和/或Z2。
    S32     X;        //X步进驱动器的灵敏度。
    S32     Y;        //Y步进驱动器的灵敏度。
    S32     Z;      //Z步进驱动器的灵敏度。
};

//Gcode M915命令的参数
struct ArgM915_t
{
    BOOL    HasZ;
    BOOL    HasS;
    
    U32     S;        //用于提升移动的当前值。（默认值：CALIBRATION_CURRENT）
    FLOAT   Z;      //过去Z_MAX_POS要移动Z轴的额外距离。（默认值：CALIBRATION_EXTRA_HEIGHT）
};

//Gcode M928命令的参数
struct ArgM928_t
{    
    U8        FileName[MAX_STRING_LENGTH];    //日志文件的文件名
};

//Gcode M997命令的参数
struct ArgM997_t
{
    BOOL    HasS;

    BOOL    S;   //
};

//Gcode M999命令的参数
struct ArgM999_t
{
    BOOL    HasS;

    BOOL    S;        //恢复而不刷新命令缓冲区。默认行为是刷新串行缓冲区，并从N接收到的最后一行开始向主机请求重新发送。
};

//Gcode M7219命令的参数
struct ArgM7219_t
{
    BOOL    HasC;
    BOOL    HasD;
    BOOL    HasF;
    BOOL    HasI;
    BOOL    HasP;
    BOOL    HasR;
    BOOL    HasU;
    BOOL    HasV;
    BOOL    HasX;
    BOOL    HasY;

    S32     C;      //将指定的列设置C为位模式V。
    S32     D;      //将Max7219本机行直接设置为（由指定的单位U）为8位模式V。
    S32     F;      //通过打开所有LED填充矩阵
    S32     I;      //初始化（清除）所有矩阵。
    S32     P;      //打印LED阵列状态以进行调试。
    S32     R;      //将指定的行设置R为位模式V。
    S32     U;      //用于D指定要设置的矩阵单位。
    S32     V;      //值使用时应用C，R或X/ Y参数。
    S32     X;        //将矩阵LED在给定X位置设置为该V值。如果未V给出，则切换LED状态。
    S32     Y;        //将矩阵LED在给定Y位置设置为该V值。如果未V给出，则切换LED状态。
};

//Gcode T命令的参数
struct ArgT_t   //工具选择
{
    U16     T_Num;  //工具编号 范围：<0-6>
};

enum WorkspacePlane_t
{
    WORKSPACE_PLANE_XY = 0U,
    WORKSPACE_PLANE_ZX,
    WORKSPACE_PLANE_YZ
};

#endif


