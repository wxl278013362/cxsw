import * as THREE from "three";
import { Scene3D } from "../core/Scene3D";
import { CameraConfig } from "../configDefine/CameraConfig";

/**
 * 相机管理器
 */
export class CameraManager {
    /**
     * 存储camera对象的数组
     */
    private _cameraArray: Array<THREE.OrthographicCamera | THREE.PerspectiveCamera> = [];
    /**
     * scene3D对象
     */
    private _scene3D: Scene3D;
    /**
     * 透视相机支持更改的参数
     */
    private _perspectiveCameraParam: Array<string> = ['fov', 'near', 'far', 'position', 'target'];
    /**
     * 正视相机支持更改的参数
     */
    private _orthographicCameraParam: Array<string> = ['frustumSize', 'near', 'far', 'position', 'target'];
    /**
     * 相机辅助工具
     */
    private _cameraHelperArray: Array<THREE.CameraHelper> = [];

    constructor(scene3D: Scene3D, config?: Array<CameraConfig>) {
        this._scene3D = scene3D;
        if (config == undefined) {
            let container = scene3D.getContainer();
            if (container == undefined) return;
            let aspect = container.clientWidth / container.clientHeight;
            // 默认创建一个透视相机
            let perspectiveCamera = this.createPerspectiveCamera('perspective_camera', 40, aspect, 1.0, 800000, new THREE.Vector3(0, 190, 200));
            if (!this._scene3D._tempConfig?.pro3GridHelper) {
                perspectiveCamera.fov = 60;
            }
            this.addCamera(perspectiveCamera, true); 
            // 新增一个正视相机
            let orthographicCamera = this.createOrthographicCameraByFrustum('orthographic_camera', 5, aspect, 1.0, 2000, new THREE.Vector3(0, 700, 0));
            this.addCamera(orthographicCamera);
        }
        else {
            this.loadConfig(config);
        }
    }
    /**
     * 释放相机管理器的资源
     */
    public dispose() {
    }
    /**
     * 通过解析相机配置文件来创建相机
     * @param config 
     */
    private loadConfig(configArray: Array<CameraConfig>) {
        let container = this._scene3D.getContainer();
        if (container == undefined) return;
        let aspect = container.clientWidth / container.clientHeight;
        for (let i = 0; i < configArray.length; ++i) {
            let config = configArray[i];
            let type = config.cameraType;
            let name = config.name;
            let near = config.near || 1.0;
            let far = config.far || 2000;
            let active = config.active || false;
            let position = undefined;
            if (config.position != undefined) {
                position = new THREE.Vector3().fromArray(config.position);
            }
            let target = undefined;
            if (config.target != undefined) {
                target = new THREE.Vector3().fromArray(config.target);
            }
            if (type == "perspective") {
                let fov = config.fov || 60;
                let perspectiveCamera = this.createPerspectiveCamera(name, fov, aspect, near, far, position, target);
                this.addCamera(perspectiveCamera, active);
                if (config.helper) {
                    // this.createCameraHelper(name);
                }
            }
            else if (type === "orthographic") {
                let frustumSize = config.frustumSize || 500;
                let orthographicCamera = this.createOrthographicCameraByFrustum(name, frustumSize, aspect, near, far, position, target);
                this.addCamera(orthographicCamera, active);
                if (config.helper) {
                    // this.createCameraHelper(name);
                }
            }
        }
    }
    /**
     * 创建一个透视相机
     * @param name 透视相机的名称，使用者需要确保这个名称的唯一性
     * @param fov 透视相机的夹角
     * @param aspect canvas的宽高比
     * @param near 近裁剪
     * @param far 远裁剪
     * @param position 相机位置
     * @param target 相机焦点
     * @returns 返回创建的相机
     */
    public createPerspectiveCamera(name: string, fov: number, aspect: number, near: number, far: number, position?: THREE.Vector3, target?: THREE.Vector3): THREE.PerspectiveCamera {
        if (target == undefined) {
            target = new THREE.Vector3(0, 0, 0);
        }
        if (position == undefined) {
            position = new THREE.Vector3(0, 200, 200);
        }
        let camera = new THREE.PerspectiveCamera(fov, aspect, near, far);
        camera.name = name;
        camera.position.copy(position);
        camera.lookAt(target);
        // 记录相机的焦点
        camera.userData.target = target.toArray();
        camera.userData.isActive = false;

        return camera;
    }
    /**
     * 创建一个正视相机
     * @param name 正视相机的名称，使用者需要确保这个名称的唯一性
     * @param left 左边界
     * @param right 右边界
     * @param top 上边界
     * @param bottom 底边界
     * @param near 近裁剪
     * @param far 远裁剪
     * @param position 相机位置
     * @param target 相机焦点
     * @returns 返回创建的相机
     */
    public createOrthographicCamera(name: string, left: number, right: number, top: number, bottom: number, near: number, far: number, position?: THREE.Vector3, target?: THREE.Vector3): THREE.OrthographicCamera {
        if (target == undefined) {
            target = new THREE.Vector3(0, 0, 0);
        }
        if (position == undefined) {
            position = new THREE.Vector3(0, 200, 200);
        }
        let camera = new THREE.OrthographicCamera(left, right, top, bottom, near, far);
        camera.name = name;
        camera.position.copy(position);
        camera.lookAt(target);
        // 计算正视窗口的大小和宽高比
        let frustumSize = top - bottom;// height
        let width = right - left;
        let aspect = width / frustumSize;
        camera.userData.frustumSize = frustumSize;
        camera.userData.aspect = aspect;
        camera.userData.isActive = false;
        // 记录相机的焦点
        camera.userData.target = target.toArray();

        return camera;
    }
    /**
     * 通过正视窗口的大小和宽高比创建一个正视相机
     * @param name 正视相机的名称，使用者需要确保这个名称的唯一性
     * @param frustumSize 正视窗口的大小
     * @param aspect 正视窗口的宽高比
     * @param near 近裁剪
     * @param far 远裁剪
     * @param position 相机位置
     * @param target 相机焦点
     * @returns 返回创建的相机
     */
    public createOrthographicCameraByFrustum(name: string, frustumSize: number, aspect: number, near: number, far: number, position?: THREE.Vector3, target?: THREE.Vector3): THREE.OrthographicCamera {
        let left = frustumSize * aspect / -2;
        let right = frustumSize * aspect / 2;
        let top = frustumSize / 2;
        let bottom = frustumSize / -2;
        let camera = this.createOrthographicCamera(name, -1.5, 1.5, top, bottom, near, far, position, target);

        return camera;
    }
    /**
     * 将相机添加到管理器中。如果需要添加的相机和已有的相机名称重复，则会覆盖掉原来的相机
     * @param camera 需要添加的相机对象
     * @param isActive 可选参数，相机的激活状态
     */
    public addCamera(camera: THREE.OrthographicCamera | THREE.PerspectiveCamera, isActive?: boolean) {
        if (isActive == undefined) {
            isActive = false;
        }
        // 保证相机数组中的相机名称唯一
        this.removeCamera(camera.name);
        // 添加相机到管理器中
        this._cameraArray.push(camera);
        // 触发添加相机的添加事件
        // EventControl.dispatchEvent('MSG_CAMERA_ADD', camera, this._name);
        // 如果有第二个激活参数，则处理相机的激活
        if (isActive) {
            this.setCameraActive(camera.name);
        }
    }
    /**
     * 将相机从管理器中移除。如果移除的相机是激活的相机，则将管理器中的第一个相机设置成激活状态。如果管理器中
     * 只有这一个相机，则不进行删除，并释放错误
     * @param name 
     * @returns 返回被删除的相机
     */
    public removeCamera(name: string): THREE.Camera | undefined {
        let findIndex = -1;
        this.getCamera(name, findIndex);
        if (findIndex == -1) {
            return undefined;
        }
        // 找到了相机，而且管理器中只有这一个相机，则释放错误，但不进行删除
        if (this._cameraArray.length == 1) {
            return undefined;
        }
        let removeItemArray = this._cameraArray.splice(findIndex, 1);
        let removeItem = removeItemArray[0];
        // 触发添加相机的移除事件
        // EventControl.dispatchEvent('MSG_CAMERA_REMOVE', removeItem, this._name);
        // 如果移除的刚好是激活的相机，则将管理器中第一个相机改变为激活状态
        if (removeItem.userData.isActive) {
            this.setCameraActive(this._cameraArray[0].name);
        }
        // 返回被移除的相机
        return removeItem;
    }
    /**
     * 通过相机的名称获得相机
     * @param name 相机名称
     * @param findIndex 找到的相机的索引。如果找到的话，此索引值会改变。相当于也是一个返回的参数
     * @returns 返回名称对应的相机，如果没有，则返回undefined
     */
    public getCamera(name: string, findIndex?: number): THREE.PerspectiveCamera | THREE.OrthographicCamera | undefined {
        if (findIndex == undefined) {
            findIndex = -1;
        }
        for (let i = 0; i < this._cameraArray.length; ++i) {
            if (this._cameraArray[i].name == name) {
                findIndex = i;
                break;
            }
        }
        if (findIndex == -1) {
            return undefined;
        }
        return this._cameraArray[findIndex];
    }
    /**
     * 得到相机的激活状态
     * @param name 相机名称
     * @returns 注：如果没找到名称对应的相机，则返回false
     */
    public getCameraActive(name: string): boolean {
        let camera = this.getCamera(name);
        if (camera == undefined) {
            return false;
        }
        return camera.userData.isActive;
    }
    /**
     * 设置相机的激活状态。相机管理器中始终只能有一个相机处于激活状态。
     * 如果将一个非激活状态的相机设置为激活状态，则需要将当前处于激活状态的相机改变成非激活状态
     * @param name 相机名称
     */
    public setCameraActive(name: string) {
        let findIndex = -1;
        let targetCamera = this.getCamera(name, findIndex);
        // 如果相机要设置的状态和当前状态一致，则可以直接返回
        if (targetCamera == undefined) {
            return;
        }
        // 此相机本来就是激活相机，直接返回
        if (targetCamera.userData.isActive) {
            return;
        }
        // 将原先的激活相机变为非激活
        let activeCamera = this.getActiveCamera();
        if (activeCamera) {
            activeCamera.userData.isActive = false;
        }
        // 将目标相机改为激活状态
        targetCamera.userData.isActive = true;
       
    }
    /**
     * 得到激活相机
     * @returns 如果有，就返回激活相机；没有就返回undefined
     */
    public getActiveCamera(): THREE.PerspectiveCamera | THREE.OrthographicCamera | undefined {
        let findIndex = -1;
        for (let i = 0; i < this._cameraArray.length; ++i) {
            if (this._cameraArray[i].userData.isActive) {
                findIndex = i;
                break;
            }
        }
        if (findIndex == -1) {
            return undefined;
        }
        return this._cameraArray[findIndex];
    }
    
    /**
     * 页面窗口大小改变事件
     */
    public onWindowResize() {
        let activeCamera = this.getActiveCamera();
        if (activeCamera == undefined) {
            return;
        }

        let renderer = this._scene3D.getRenderer();
        if (this._scene3D._tempConfig?.heatMapFlag) {
            if (this._scene3D._tempContainer != undefined) {
                if (renderer == undefined) return;
                renderer.setSize(this._scene3D._tempContainer.clientWidth, this._scene3D._tempContainer.clientHeight);
                // 画布的大小重新调整
                let aspect = this._scene3D._tempContainer.clientWidth / this._scene3D._tempContainer.clientHeight;
                // @ts-ignore
                if (activeCamera.isPerspectiveCamera) {
                    let perspectiveCamera = activeCamera as THREE.PerspectiveCamera;
                    perspectiveCamera.aspect = aspect;
                    perspectiveCamera.updateProjectionMatrix();
                }
            } 
        } else {
            let mainContainer = this._scene3D.getMainContainer();
            if (mainContainer != undefined) {
                if (renderer == undefined) return;
                renderer.setSize(mainContainer.clientWidth, mainContainer.clientHeight);
                // 画布的大小重新调整
                let aspect = mainContainer.clientWidth / mainContainer.clientHeight;
                // @ts-ignore
                if (activeCamera.isPerspectiveCamera) {
                    let perspectiveCamera = activeCamera as THREE.PerspectiveCamera;
                    perspectiveCamera.aspect = aspect;
                    perspectiveCamera.updateProjectionMatrix();
                }
            }
        }
    }
    /**
     * 渲染时调用
     */
    public update() {
        for (let i = 0; i < this._cameraHelperArray.length; ++i) {
            let cameraHelper = this._cameraHelperArray[i];
            if (cameraHelper.visible) {
                cameraHelper.update();
            }
        }
    }
}
