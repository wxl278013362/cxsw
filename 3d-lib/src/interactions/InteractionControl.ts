import * as THREE from "three";
import { Scene3D } from "../core/Scene3D";

/**
 * 选择器类型
 */
export enum InteractionType {
    // 默认
    Default,
    // 相机基础类
    Camera,
    // 相机轨道控制器
    Orbit,
    // 物体交互基础类
    Object,
    // 探针
    Detection,
    // 选择
    Selector,
    // 编辑
    Editor,
    // 创建
    Creator,
    // 框选放大
    FrameZoom,
    // 视图盒子
    ViewCube,
    // 框选选中
    SelectionBox,
    // 设备布点
 
}

/**
 * 选择集的对象类型
 */
export interface Intersection {
    /**
     * 距离
     */
    distance: number;
    /**
     * 交互点
     */
    point: THREE. Vector3;
    /**
     * 具体的对象
     */
    object: THREE.Object3D;
    /**
     * 交互对象的优先级
     */
    priority: number;
}

/**
 * 交互控件基类
 */
export abstract class InteractionControl {
    /**
     * 绑定的Scene3D
     */
    protected _scene3D: Scene3D;
    /**
     * 控件是否可用，只提供给3D内部使用
     */
    protected enable: boolean = true;
    /**
     * 交互控件的名称。从绑定的对象中获得
     */
    public name: string;
    /**
     * 控件类型
     */
    public type: InteractionType = InteractionType.Default;
    /**
     * 交互控件优先级别。值[0, 99]。值越小优先级越高
     */
    public priority: number = 99;
    /**
     * 绑定需要交互的页面元素
     * @param scene3D 场景绘制
     */
    constructor(scene3D: Scene3D) {
        this._scene3D = scene3D;
    }
    /**
     * 一些交互控件必须每一帧都刷新
     */
    public update() {}
    /**
     * 释放资源
     */
    public dispose() {}
    /**
     * 得到控件的可用状态
     */
    public getEnable(): boolean {
        return this.enable;
    }
    /**  
     * 设置enable属性
     * @param enable 
     */
    public setEnable(enable: boolean) {
        //如果是基础控制器类别，直接设置enable属性
        let inter = this._scene3D.getInteractionManager();
        if (inter == undefined) return;
        if (inter.isBaseInteractions(this.type)) {
            this.enable = enable;
        }
        else {
            if (enable) {
                inter._enableInteractions(this.type);
            }
            else {
                inter._resetInteractionsEnable();
            }
        }
    }
    /**
     * 直接设定交互控件的enable属性
     * @param enable 
     */
    public setEnableDirect(enable: boolean) {
        this.enable = enable;
    }
}
/**
 * 相机交互控件基类
 */
export abstract class CameraInteractionControl extends InteractionControl {
    /**
     * 绑定的相机，目前只支持PerspectiveCamera和OrthographicCamera两种类型的相机
     */
    protected _bindCamera: THREE.OrthographicCamera | THREE.PerspectiveCamera;
    /**
     * 相机交互控件基类构造函数
     * @param scene3D 场景绘制
     * @param camera 相机
     */
    constructor(scene3D: Scene3D, camera: THREE.OrthographicCamera | THREE.PerspectiveCamera) {
        super(scene3D);
        this.name = camera.name;
        this._bindCamera = camera;
        this.type = InteractionType.Camera;
    }
    /**
     * 重新绑定一个相机
     * @param camera 新相机
     */
    public setCamera(camera: THREE.OrthographicCamera | THREE.PerspectiveCamera) {
        this.name = camera.name;
        this._bindCamera = camera;
    }
}
/**
 * 物体交互控件的基类
 */
export abstract class ObjectInteractionControl extends InteractionControl {
    
    /**
     * 物体交互控件构造函数
     * @param scene3D 场景绘制
     * @param object 需要交互的物体
     */
    constructor(scene3D: Scene3D) {
        super(scene3D);
        this.type = InteractionType.Object;
    }
}