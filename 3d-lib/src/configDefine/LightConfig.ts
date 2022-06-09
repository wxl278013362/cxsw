export interface LightConfig {
    /**
     * 光源名称
     */
    name: string;
    /**
     * 光源的类型. point or ambient or directional or hemisphere
     */
    type: string;
    /**
     * 光的强度
     */
    intensity?: number;
    /**
     * 光的颜色
     */
    color?: string | number;
    /**
     * 半球光源独有。地面色
     */
    groundColor?: string | number;
    /**
     * 光源的初始位置
     */
    position?: Array<number>;
    /**
     * 点光源独有。光影响的最远距离。如果为0，则光的影响范围不受限制
     */
    distance?: number;
    /**
     * 点光源独有。光源的衰减
     */
    decay?: number;
}