export interface CameraConfig {
    /**
     * 相机名称
     */
    name: string;
    /**
     * 相机类型.perspective or orthographic
     */
    cameraType: string;
    /**
     * 相机位置
     */
    position?: Array<number>;
    /**
     * 相机焦点
     */
    target?: Array<number>;
    /**
     * 相机激活标志
     */
    active?: boolean;
    /**
     * 相机辅助工具
     */
    helper?: boolean;
    /**
     * 近剪裁平面
     */
    near?: number;
    /**
     * 远剪裁平面
     */
    far?: number;
    /**
     * 透视相机夹角
     */
    fov?: number;
    /**
     * 正视窗口的大小
     */
    frustumSize?: number;
}