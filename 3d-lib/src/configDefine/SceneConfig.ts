import { CameraConfig } from "./CameraConfig";

export interface SceneConfig {
    /**
     * 清除背景色
     */
    clearColor?: string | number;
    /**
     * 背景类型. color or image
     */
    backgroundType?: string;
    /**
     * 背景颜色
     */
    backgroundColor?: string | number;
    /**
     * 背景纹理路径
     */
    backgroundUrl?: string;
    /**
     * 贴图路径
     */
    textureUrl?: string;

    /**
     * 着色器路径
     */
    shaderUrl?: string;
    /**
     * 是否创建辅助坐标轴
     */
    coordinateHelper?: boolean;
    /**
     * 是否创建帧数统计器
     */
    statsHelper?: boolean;
    /**
     * 相机配置文件
     */
    cameraConfig?: Array<CameraConfig>;

    sceneName?: string,

    server?: string,
    
    boxHelper?: boolean;

    heatMapFlag?: boolean;

    pro3GridHelper?: boolean;
}