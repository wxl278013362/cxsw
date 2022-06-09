import * as THREE from "three";
// @ts-ignore 
import { Scene3D } from "core/Scene3D";
import { LightConfig } from "../configDefine/LightConfig";

/**
 * 光源管理器
 */
export class LightManager {
    /**
     * 存储光源对象的数组
     */
    private _lightArray: Array<THREE.Light> = [];
  
    /**
     * scene3D对象
     */
    private _scene3D: Scene3D;
    
    constructor(scene3D: Scene3D, config?: Array<LightConfig>) {
        this._scene3D = scene3D;
       
        let hemisphereLight = this.createHemisphereLight('hemisphere_light', 0xffffff, 0xffffff, 0.8);
        this.addLight(hemisphereLight);

        // 添加平光照效果
        let directionalLight = this.createDirectionalLight('directional_light1', 0xffffff, 0.4, new THREE.Vector3(0,0,100));
        this.addLight(directionalLight);
        // let directionalLightHelper = new THREE.DirectionalLightHelper(directionalLight, 10);
        // this._scene3D.addObject(directionalLightHelper);

        let directionalLight1 = this.createDirectionalLight('directional_light2', 0xffffff, .4, new THREE.Vector3(0,0,-100));
        this.addLight(directionalLight1);
        // let directionalLightHelper1 = new THREE.DirectionalLightHelper(directionalLight1, 10);
        // this._scene3D.addObject(directionalLightHelper1);

        let directionalLight2 = this.createDirectionalLight('directional_light3', 0xffffff, 0.3, new THREE.Vector3(100,0,0));
        this.addLight(directionalLight2);
        // let directionalLightHelper2 = new THREE.DirectionalLightHelper(directionalLight2, 10);
        // this._scene3D.addObject(directionalLightHelper2);

        let directionalLight3 = this.createDirectionalLight('directional_light4', 0xffffff, .4, new THREE.Vector3(-100,0,0));
        this.addLight(directionalLight3);
        // let directionalLightHelper3 = new THREE.DirectionalLightHelper(directionalLight3, 10);
        // this._scene3D.addObject(directionalLightHelper3);

        let directionalLight4 = this.createDirectionalLight('directional_light5', 0xffffff, .3, new THREE.Vector3(0,100,0));
        this.addLight(directionalLight4);
        // let directionalLightHelper4 = new THREE.DirectionalLightHelper(directionalLight4, 10);
        // this._scene3D.addObject(directionalLightHelper4);

        let directionalLight5 = this.createDirectionalLight('directional_light6', 0xffffff, 0.3, new THREE.Vector3(0,-100,0));
        this.addLight(directionalLight5);
        // let directionalLightHelper5 = new THREE.DirectionalLightHelper(directionalLight5, 10);
        // this._scene3D.addObject(directionalLightHelper5);
    }

    /**
     * 创建一个环境光源
     * @param name 光源的名称
     * @param color 颜色
     * @param intensity 光的强度
     * @returns 返回创建的光源
     */
    public createAmbientLight(name: string, color?: string | number, intensity?: number): THREE.AmbientLight {
        let light = new THREE.AmbientLight(color, intensity);
        light.name = name;
        return light;
    }
    /**
     * 创建一个点光源
     * @param name 光源的名称
     * @param color 颜色
     * @param intensity 光的强度
     * @param distance 光影响的最远距离。如果为0，则光的影响范围不受限制
     * @param decay 光源的衰减
     * @param position 光源的初始位置
     * @returns 返回创建的光源
     */
    public createPointLight(name: string, color?: string | number, intensity?: number, distance?: number, decay?: number, position?: THREE.Vector3): THREE.PointLight {
        if (position == undefined) {
            position = new THREE.Vector3(10, 50, 50);
        }
        let light = new THREE.PointLight(color, intensity, distance, decay);
        light.name = name;
        light.position.copy(position);

        return light;
    }
    /**
     * 创建一个方向光源
     * @param name 光源的名称
     * @param color 颜色
     * @param intensity 光的强度
     * @param position 光源的初始位置
     * @returns 返回创建的光源
     */
    public createDirectionalLight(name: string, color?: string | number, intensity?: number, position?: THREE.Vector3): THREE.DirectionalLight {
        let light = new THREE.DirectionalLight(color, intensity);
        light.name = name;
        light.castShadow = true;

        if (position != undefined) {
            light.position.copy(position);
        }

       light.shadow.mapSize.width = 3000;
       light.shadow.mapSize.height = 3000;
       var d = 10;
       light.shadow.camera.left = -d;
       light.shadow.camera.right = d;
       light.shadow.camera.top = 10;
       light.shadow.camera.bottom = -10;
       light.shadow.camera.near = 1;
       light.shadow.camera.far = 50;
   
       light.shadow.bias = -0.001;
       light.shadow.mapSize.width = 1024 * 3;
       light.shadow.mapSize.height = 1024 * 4;

        return light;
    }
    /**
     * 创建一个半球光
     * @param name 光源的名称
     * @param color 天空色
     * @param groundColor 地面色
     * @param intensity 光的强度
     * @param position 光源的初始位置
     * @returns 返回创建的光源
     */
    public createHemisphereLight(name: string, color?: string | number, groundColor?: string | number, intensity?: number, position?: THREE.Vector3): THREE.HemisphereLight {
        if (position == undefined) {
            position = new THREE.Vector3(0, 20, -20);
        }
        let light = new THREE.HemisphereLight(color, groundColor, intensity);
        light.name = name;
        light.position.copy(position);

        return light;
    }
    /**
     * 添加光源
     * @param light 光源
     * @param enable 是否在场景中起作用，默认为ture
     */
    public addLight(light: THREE.Light, enable?: boolean) {
        if (enable == undefined) {
            enable = true;
        }
        light.visible = enable;
        // 确保name唯一
        this.removeLight(light.name);
        // 添加到光源管理器中
        this._lightArray.push(light);
        // 光源必须添加到场景中才会起作用
        this._scene3D.addObject(light);
    }
    /**
     * 删除光源
     * @param name 光源的名称
     * @returns 返回删除的光源
     */
    public removeLight(name: string): THREE.Light | undefined {
        let findIndex = -1;
        this.getLight(name, findIndex);
        if (findIndex == -1) {
            return undefined;
        }
        let removeItemArray = this._lightArray.splice(findIndex, 1);
        let removeItem = removeItemArray[0];
        return removeItem;
    }
    /**
     * 根据名称获取光源
     * @param name 名称
     * @param findIndex 如果找到光源，此参数也会改变。相当于一个返回值
     */
    public getLight(name: string, findIndex?: number): THREE.Light | undefined {
        if (findIndex == undefined) {
            findIndex = -1;
        }
        for (let i = 0; i < this._lightArray.length; ++i) {
            if (this._lightArray[i].name == name) {
                findIndex = i;
                break;
            }
        }
        if (findIndex == -1) {
            return undefined;
        }
        return this._lightArray[findIndex];
    }

    public clear() {
        this._lightArray = [];
    }
}