import * as THREE from "three";
import { InteractionType, InteractionControl } from "../interactions/InteractionControl";
import { Scene3D } from "../core/Scene3D";
import { App } from "../core/App";
import { SceneConfig } from "../configDefine/SceneConfig";
import { OrbitControls } from "../interactions/OrbitControls";
import { EventControl } from "../event/EventControl";

/**
 * 控制器关系
 */
export interface InteractionRelation {
    /**
     * 类型
     */
    type: InteractionType,
    /**
     * 与该类型可以共存的控制器
     */
    coexistence: Array<InteractionType>
}
/**
 * 交互控件管理器
 */
export class InteractionManager {
    private _app: App;
    private _scene3D: Scene3D;
    private _config: SceneConfig;
    private _temGroup: THREE.Group | undefined | null;
    public orbitControl: any;

    private count = 1;
    private firstTime: any = null;
    private timmer: any = null;
    private _scalingMultiple: number = 1;
    private _rotateFlag = false;
    private _rotateFlagX = false;
    private _rotateFlagY = false;
    private _rotateFlagZ = false;

    private _scaleFlag = false;

    private _orientation = false;

    private _tenum: number = 0;
    private _tenumIndex: number = 0;

    private _tMatrix: THREE.Matrix4;

    private _arrow: any = [];
    private _roateData: {[key:string] : any} = {};

    //设置平面相机的top和bottom的值
    private _tempOrCamera:{[key:string] : any} = {
        "3": {top: 1.4, bottom: -1.4}, 
        "4": {top: 1.9, bottom: -1.9}, 
        "5": {top: 2.4, bottom: -2.4}, 
        "6": {top: 2.8, bottom: -2.8}
    };
    
    /**
     * 存储所有的交互控件
     */
    private _interactionArray: Array<InteractionControl> = [];
    /**
     * 基础控制器
     */
    private _baseInteractions: Array<InteractionType> = [InteractionType.Selector, InteractionType.Orbit, InteractionType.ViewCube];
    /**
     * 控制器之间的关系
     */
    private _interactionRelations: Array<InteractionRelation> = [
        {   // 探针
            type: InteractionType.Detection,
            coexistence: [InteractionType.Selector, InteractionType.Orbit, InteractionType.ViewCube]
        },
    ];

    constructor(scene3D: Scene3D, config: SceneConfig) {
        this._app = App.getInstance();
        this._scene3D = scene3D;
        this._config = config;
        this._temGroup = scene3D._temGroup;

        this._tenum = 0;
        this._tenumIndex = 0;

        this._arrow = [];
    
        //模型视角的修改
        EventControl.addEvent('MSG_CHANGE_MODEL_FOV', ()=> {this.changeFov()}, '3D');

        if (!this._config.pro3GridHelper) {
            // 调整相机的视角
            EventControl.addEvent('MSG_MODEL_POSITION_COUNT', (length: number)=> {this.changePosition(length)}, '3D');
        } else {
            EventControl.addEvent('MSG_MODEL_POSITION_COUNT', (length: number)=> {this.change45FovPosition(length)}, '3D');
        }
       
        EventControl.addEvent('MSG_LOAD_3DS_DATA_FINISH', ()=> {this.initControl()}, '3D');

        //鼠标实时缩放事件
        EventControl.addEvent('MSG_ON_MODEL_ZOOM', (data: any)=> {this.onModelZoom(data)}, '3D');

        if (config?.heatMapFlag == undefined) {
            //PC双击事件
            this._scene3D.getCanvas()?.addEventListener('dblclick', (event) => this.onDbclick(event), false);
            
            //触屏双击事件
            this._scene3D.getCanvas()?.addEventListener('touchstart', (event) => this.onClick(event), false);
        }
    }

    private onModelZoom(data: any) {
        if (this._scene3D._meshGroup == undefined) return;
        if (this._scene3D._meshGroup.children.length != 0) {
            let child = this._scene3D._meshGroup.children[0];
            if (child.userData.type != 'gcode') return;
            if (data == undefined) return;
            
            //@ts-ignore
            if (data.flag == 'sub') {
                //@ts-ignore
                child.material.linewidth += data.num;
                // @ts-ignore
                if (child.material.linewidth >= 5) {
                    //@ts-ignore
                    child.material.linewidth = 5;
                }
            } else{
                //@ts-ignore
                child.material.linewidth -= data.num;
                //@ts-ignore
                if (child.material.linewidth <= 2) {
                    //@ts-ignore
                    child.material.linewidth = 2;
                } 
            }
        }
    }

    /**
     * 画布双击事件
     * @param event 
     */
    private onDbclick(event: MouseEvent) {
        if(!this._scene3D.enable) return;
        event.preventDefault();

        //视角复位功能
        this.resetCameraFov();
    }

    /**
     * 模拟触屏的双击事件
     * @param event 
     * @returns 
     */
    private onClick(event: TouchEvent) {
        if(!this._scene3D.enable) return;
        event.preventDefault();

        if (event.touches.length != 1) return;

        if(this.count == 1) {
            this.firstTime = new Date().getTime();
            this.count++;
            let scope = this;
            this.timmer = setTimeout(function () {
                scope.count = 1;
            }, 300)
        }

        if(this.count == 2) {
            this.count ++ ;
        } else {
            var endTime = new Date().getTime();
            if ((endTime - this.firstTime) < 300) {
                //视角复位功能
                this.resetCameraFov();
                this.count = 1;
                clearTimeout(this.timmer);
            }
        }
    }

    /**
     * 初始化控制器
     */
    private initControl() {
        //改变stl注水效果的外框几何信息
        if (!this._config.boxHelper) {
            this._scene3D.getClipControlManager()?.onChangeOutGeometry();
        }
    }

    /**
     * 45度视角的情况下模型的视角位置显示
     * @param length 
     * @returns 
     */
    private change45FovPosition(length: any) {
        let mainContainer = this._scene3D.getMainContainer();
        if (mainContainer == null) return;
        let w = mainContainer.clientWidth;
        let h = mainContainer.clientHeight;
        if (w > h) {
            this._orientation = true;
        } else {
            this._orientation = false;
        }

        let boxdd = new THREE.Box3();
        if (this._scene3D._meshGroup == undefined) return;
        boxdd.expandByObject(this._scene3D._meshGroup);

        let center = boxdd.getCenter(new THREE.Vector3());
        let cameraManager = this._scene3D.getCameraManager();
        if (cameraManager == undefined) return;

        let activeCamera1 = cameraManager.getActiveCamera();
        if (activeCamera1 == undefined) return;
        let distance = center.distanceTo(activeCamera1.position.clone());
        let box = undefined;
        
        box = new THREE.Box3().expandByObject(this._scene3D._meshGroup);
        let subLen = new THREE.Vector3().subVectors(box.max, box.min).multiplyScalar(0.5);

        let activeCamera = cameraManager.getActiveCamera() as THREE.PerspectiveCamera;
        if (activeCamera == undefined ) return;
        let subDistance = distance-subLen.z;

        let mmaxLen = (box.max.distanceTo(box.min))+(box.max.distanceTo(box.min)) * 0.5;
      
        if (subDistance < 0) {
            let zData = (Math.abs(subLen.z) + distance + Math.abs(activeCamera.position.z));
            activeCamera.position.z = zData;
            activeCamera.position.y = 200;
            this._scene3D._endCameraDistance = activeCamera.position.z;
            if (this._scene3D._cameralView == null) return;
            this._scene3D._cameralView.set(activeCamera.position.x, activeCamera.position.y, activeCamera.position.z);
        } else {
            if (length <= 1) {
                activeCamera.position.z += Math.abs(subDistance);
                this._scene3D._endCameraDistance = activeCamera.position.z;
        
                if (this._scene3D._endCameraDistance > 150 ) {
                    if (mmaxLen >=100) {
                        if (mmaxLen < 200) {

                            if (!this._orientation) {
                                activeCamera.position.z = 100;
                                activeCamera.position.x = 123;
                                activeCamera.position.y = 100;
                            } else {
                                activeCamera.position.z = 100;
                                activeCamera.position.x = 70;
                                activeCamera.position.y = 80;
                            }
                        } else{
                            //竖屏
                            if (!this._orientation) {
                                activeCamera.position.z = mmaxLen-50;
                                activeCamera.position.x = 190;
                                activeCamera.position.y = 160;

                            } else {
                                activeCamera.position.z = mmaxLen-100;
                                activeCamera.position.x = 147;
                                activeCamera.position.y = 117;
                            }
                        }
        
                    } else if (mmaxLen <= 100) {
                        if (mmaxLen >= 50) {
                            //竖屏
                            if (!this._orientation) {
                                activeCamera.position.z = 97;
                                activeCamera.position.y = 70;
                                activeCamera.position.x = 99;
                            } else {
                                activeCamera.position.z = 30;
                                activeCamera.position.y = 28;
                                activeCamera.position.x = 30;
                            }
                        } else{
                            //竖屏
                            if (!this._orientation) {
                                activeCamera.position.z = mmaxLen;
                                activeCamera.position.x = 30;
                                activeCamera.position.y = 20;
                                
                            } else {
                                activeCamera.position.z = 35;
                                activeCamera.position.x = 27;
                                activeCamera.position.y = 15;
                            }
                        }
                    }
                    this._scene3D._endCameraDistance = activeCamera.position.z;
                    if (this._scene3D._cameralView == null) return;
                    this._scene3D._cameralView.set(activeCamera.position.x, activeCamera.position.y, activeCamera.position.z);
                } 
            }
        }
    }

    /**
     * 非45度视角的情况下计算模型的视角位置
     * @param length 模型个数
     * @returns 
     */
    private changePosition(length: any) {
        let mainContainer = this._scene3D.getMainContainer();
        if (mainContainer == null) return;
        let w = mainContainer.clientWidth;
        let h = mainContainer.clientHeight;
        if (w > h) {
            this._orientation = true;
        } else {
            this._orientation = false;
        }

        let boxdd = new THREE.Box3();
        if (this._temGroup == undefined) return;
        if (this._scene3D._meshGroup == undefined) return;
        boxdd.expandByObject(this._temGroup);

        let center = boxdd.getCenter(new THREE.Vector3());
        let cameraManager = this._scene3D.getCameraManager();
        if (cameraManager == undefined) return;

        let activeCamera1 = cameraManager.getActiveCamera();
        if (activeCamera1 == undefined) return;
        let distance = center.distanceTo(activeCamera1.position.clone());
        let box = undefined;
        
        box = new THREE.Box3().expandByObject(this._temGroup);
        let subLen = new THREE.Vector3().subVectors(box.max, box.min).multiplyScalar(0.5);

        let activeCamera = cameraManager.getActiveCamera() as THREE.PerspectiveCamera;
        if (activeCamera == undefined ) return;
        let subDistance = distance-subLen.z;

        let mmaxLen = (box.max.distanceTo(box.min))+(box.max.distanceTo(box.min)) * 0.5;
        if (subDistance < 0) {
            let zData = (Math.abs(subLen.z) + distance + Math.abs(activeCamera.position.z));
            activeCamera.position.z = zData;
            activeCamera.position.y = 200;
            this._scene3D._endCameraDistance = activeCamera.position.z;
            if (this._scene3D._cameralView == null) return;
            this._scene3D._cameralView.set(activeCamera.position.x, activeCamera.position.y, activeCamera.position.z);
        } else {
            if (length <= 1) {
                activeCamera.position.z += Math.abs(subDistance);
                this._scene3D._endCameraDistance = activeCamera.position.z;
        
                if (this._scene3D._endCameraDistance > 150 ) {
                    if (mmaxLen >=100) {
                        if (mmaxLen < 200) {
                            activeCamera.position.z = 100;
                        } else{
                            //竖屏
                            if (!this._orientation) {
                                activeCamera.position.z = mmaxLen-40;
                            } else {
                                activeCamera.position.z = mmaxLen-80;
                            }
                        }

                        //竖屏
                        if (!this._orientation) {
                            activeCamera.position.x = -3;
                            activeCamera.position.y = 70;
                        } else {
                            activeCamera.position.x = -3;
                            activeCamera.position.y = 40;
                        }
                       
                    } else if (mmaxLen <= 100) {
                        //竖屏
                        if (!this._orientation) {
                            if (mmaxLen >= 50) {
                                activeCamera.position.z = 60;
                            } else{
                                activeCamera.position.z = mmaxLen;
                            }
                            activeCamera.position.y = 4;
                            activeCamera.position.x = -1.5;
                        } else {
                            if (mmaxLen >= 50) {
                                activeCamera.position.z = 35;
                            } else{
                                activeCamera.position.z = mmaxLen;
                            }
                            activeCamera.position.y = 4;
                            activeCamera.position.x = -1.5;
                        }
                    }
                    this._scene3D._endCameraDistance = activeCamera.position.z;
                    if (this._scene3D._cameralView == null) return;
                    this._scene3D._cameralView.set(activeCamera.position.x, activeCamera.position.y, activeCamera.position.z);
                } 
            }
        }
        EventControl.removeEvent( 'MSG_MODEL_POSITION_COUNT', (length: number)=> {this.changePosition(length)}, 'all');
    }

    // 改变相机视角
    public changeFov() {
        let cameraManager = this._scene3D.getCameraManager();
        if (cameraManager == undefined) return;
        
        let activeCamera = cameraManager.getActiveCamera();
        if (activeCamera != undefined ) {
            let inter = this._scene3D.getInteractionManager();
            if (inter == undefined) return;
            //判断轨道控制器是否已经添加
            if (inter.orbitControl != undefined) {
                if (this._config.pro3GridHelper) {
                    activeCamera.position.set(272.87, 235.51, 234.36);
                } else {
                    activeCamera.position.set(14.09, 175.97, 860.65);
                }
               
                if (this._scene3D._cameralView == null) return;
                this._scene3D._cameralView.set(activeCamera.position.x, activeCamera.position.y, activeCamera.position.z);
                // 设置焦点
                this.orbitControl.target.set(0,0,0);
                return;
            }
            // 以后我想将第二个参数去掉，在轨道控制器中实现获取激活相机
            this.orbitControl = new OrbitControls(this._scene3D, activeCamera);
            this.orbitControl.minDistance = 1.0;
            this.orbitControl.rotateSpeed = 1.5;
            if (!this._config.pro3GridHelper) {
                this.orbitControl.enablePan = false;
            }
            // activeCamera.position.set(14.09, 175.97, 860.65);
            activeCamera.position.set(272.87, 235.51, 234.36);

            if (this._scene3D._cameralView == null) return;
            this._scene3D._cameralView.set(activeCamera.position.x, activeCamera.position.y, activeCamera.position.z);
            // 设置焦点
            this.orbitControl.target.set(0,0,0);
            this.addInteraction(this.orbitControl);
        }
        EventControl.removeEvent( 'MSG_CHANGE_MODEL_FOV', ()=> {this.changeFov()}, 'all');
    }

    /**
     * 重置控制器的enable属性，内部使用
     */
    public _resetInteractionsEnable() {
        for (let i = 0; i < this._interactionArray.length; ++i) {
            let interaction = this._interactionArray[i];
            // 如果是场景必需的基础控制器，则设为true
            if (this.isBaseInteractions(interaction.type)) {
                interaction.setEnableDirect(true);
            }
            else {
                interaction.setEnableDirect(false);
            }
        }
    }
    /**
     * 设置所有控制器，内部使用
     * @param type 开启控制器的类型
     */
    public _enableInteractions(type: InteractionType) {
        // 寻找该控制器类型的共存关系
        for (let i = 0; i < this._interactionRelations.length; ++i) {
            if (this._interactionRelations[i].type == type) {
                let coexistence = this._interactionRelations[i].coexistence;
                for (let j = 0; j < this._interactionArray.length; ++j) {
                    let interaction = this._interactionArray[j];
                    if (interaction.type == type) {
                        interaction.setEnableDirect(true);
                    }
                    // 如果不是共存关系，则设为false，其他不处理
                    else if (coexistence.indexOf(interaction.type) == -1) {
                        interaction.setEnableDirect(false);
                    }
                }
                break;
            }
        }
    }

    /**
     * 通过交互控件的名称和类型设置交互控件的状态
     * @param name 如果是相机控件，就使用相机的名称；如果是数据，就使用数据的uuid
     * @param type 交互控件的类型
     * @param enable 激活状态
     */
    public setInteractionEnable(type: InteractionType, enable: boolean) {
        for (let i = 0; i < this._interactionArray.length; ++i) {
            let item = this._interactionArray[i];
            if (item.type == type) {
                item.setEnable( enable );
                break;
            }
        }
    }
    /**
     * 
     * @param type 判断是否属于基础控制器类别
     */
    public isBaseInteractions(type: InteractionType): boolean {
        return (this._baseInteractions.indexOf(type) > -1);
    }
    /**
     * 添加一个交互控件
     * @param control 
     */
    public addInteraction(control: InteractionControl) {
        // 确保同一对象的同一类型的控件只能有一个
        if (this.hasInteraction(control.type)) {
            return;
        }
        this._interactionArray.push(control);
    }
    /**
     * 判断是否已经存在了交互控件
     * @param name 控件名称
     * @param type 控件类型
     */
    public hasInteraction(type: InteractionType): boolean {
        for (let i = 0; i < this._interactionArray.length; ++i) {
            let item = this._interactionArray[i];
            if (item.type == type) {
                return true;
            }
        }
        return false;
    }

    /**
     * 移除一个交互控件
     * @param name 控件名称
     * @param type 控件类型
     */
    public removeInteraction(type: InteractionType): InteractionControl | undefined {
        let findIndex = -1;
        this.getInteraction(type, findIndex);
        if (findIndex == -1) {
            return undefined;
        }
        let removeItemArray = this._interactionArray.splice(findIndex, 1);
        let removeItem = removeItemArray[0];
        return removeItem;
    }

    /**
     * 获得一个交互控件
     * @param name 控件名称
     * @param type 控件类型
     * @param findIndex 可选，返回值。返回交互控件的索引
     */
    public getInteraction(type: InteractionType, findIndex?: number): InteractionControl | undefined {
        findIndex = -1;
        for (let i = 0; i < this._interactionArray.length; ++i) {
            let item = this._interactionArray[i];
            if (item.type == type) {
                findIndex = i;
                break;
            }
        }
        if (findIndex == -1) {
            return undefined;
        }
        return this._interactionArray[findIndex];
    }

    /**
     * 等高线热力图视角
     */
    public heatMapFov() {
        let cameraManager = this._scene3D.getCameraManager();
        if (cameraManager == undefined) return;
        let activeCamera = cameraManager.getActiveCamera();
        if (activeCamera != undefined ) {
            // 以后我想将第二个参数去掉，在轨道控制器中实现获取激活相机
            this.orbitControl = new OrbitControls(this._scene3D, activeCamera);
            this.orbitControl.rotateSpeed = 0.5;
            activeCamera.position.set(2.7, 3.6, 4.5);
            this.orbitControl.enablePan = false;
            this.orbitControl.enableRotate = true;
            this.orbitControl.enableZoom = true;
            // 设置焦点
            this.orbitControl.target.set(0,0,0);
            this.addInteraction(this.orbitControl);
        }
    }

    /**
     * 切换热力图的3D视角
     */
    public changeThreeViewFov() {
        let cameraManager = this._scene3D.getCameraManager();
        if (cameraManager == undefined) return;
        //激活平面相机
        let perspective_camera = cameraManager.setCameraActive("perspective_camera");
        let activeCamera = cameraManager.getActiveCamera();
        if (activeCamera != undefined ) {
            activeCamera.position.set(2.7, 3.6, 4.5);
            if (this.orbitControl == undefined) return;
            this.orbitControl.enablePan = false;
            this.orbitControl.enableRotate = true;
            this.orbitControl.enableZoom = true;
            // 设置焦点
            this.orbitControl.target.set(0,0,0);
            //关闭点选构件
            this.setInteractionEnable(InteractionType.Selector, false);

            //设置3D等高图
            let heatMapManager = this._scene3D.getHeatMapManager();
            if (heatMapManager == undefined) return;
            heatMapManager.onHideAnyModel(true);
        }
    }

    /**
     * 设置热力图的2D视角
     */
    public changeTwoViewFov() {
        let cameraManager = this._scene3D.getCameraManager();
        if (cameraManager == undefined) return;
        //激活平面相机
        let orthographic_camera = cameraManager.setCameraActive("orthographic_camera");
        let activeCamera = cameraManager.getActiveCamera() as THREE.OrthographicCamera;

        if (activeCamera != undefined ) {
            // 以后我想将第二个参数去掉，在轨道控制器中实现获取激活相机
            this.orbitControl.minPolarAngle = -Math.PI/2;
            this.orbitControl.maxPolarAngle = Math.PI/2;
            let heatMapManager = this._scene3D.getHeatMapManager();
            if (heatMapManager == undefined) return;
            let len = heatMapManager._tempLen;
            let top = this._tempOrCamera[len].top;
            let bottom = this._tempOrCamera[len].bottom;
            activeCamera.top = top;
            activeCamera.bottom = bottom;
            //更新相机的矩阵
            activeCamera.updateProjectionMatrix();
            // 设置焦点
            this.orbitControl.target.set(0,0,0);
        }

        //设置二维等高图
        let heatMapManager = this._scene3D.getHeatMapManager();
        if (heatMapManager == undefined) return;
        heatMapManager.onHideAnyModel(false);
    }

    /**
     * 恢复3D初始视角
     */
    public resetCameraFov() {
        let cameraManager = this._scene3D.getCameraManager();
        if (cameraManager == undefined) return;

        let activeCamera = cameraManager.getActiveCamera();
        if (activeCamera != undefined ){
            if (this._scene3D._cameralView == null) return;
            activeCamera.position.set(this._scene3D._cameralView.x, this._scene3D._cameralView.y, this._scene3D._cameralView.z );
            if (this.orbitControl == undefined) return;
            this.orbitControl.target.set(0,0,0);
            this.orbitControl.enableZoom = true;
            this.orbitControl.enableRotate = true;
            this.orbitControl.enablePan = false;
        }
    }

    /**
     * 转换为平面视角
     */
    public changeToPlaneFov(){
        let cameraManager = this._scene3D.getCameraManager();
        if (cameraManager == undefined) return;
        let activeCamera = cameraManager.getActiveCamera();
        if (activeCamera != undefined ){
            activeCamera.position.x = 0;
            if (this._scene3D._cameralView == null) return;
            activeCamera.position.set(this._scene3D._cameralView.x, this._scene3D._cameralView.y, this._scene3D._cameralView.z );
            if (this.orbitControl == undefined) return;
            this.orbitControl.target.set(0,0,0);
            this.orbitControl.enableZoom = false;
            this.orbitControl.enableRotate = false;
            this.orbitControl.enablePan = false;
        }
    }

    /**
     * 渲染时调用
     */
    public update() {
        for (let i = 0; i < this._interactionArray.length; ++i) {
            let interactionControl = this._interactionArray[i];
            if (interactionControl.getEnable()) {
                interactionControl.update();
            }
        }
    }

    /**
     * 当模型变换涉及到三个坐标轴时变换方法有x--y--z依次变换
     * @param degree 
     */
    private onSetModelRotate() {
        //x轴变换
        if (this._scene3D._meshGroup == undefined) return;

        //重置模型的初始位置
        this.onLocalResetModel();

        if (this._roateData.flag != undefined && this._roateData.flag == false) return;
        if (this._roateData.x != undefined) {
            this._scene3D._meshGroup.rotation.x = THREE.MathUtils.degToRad(this._roateData.x);
            this._scene3D._meshGroup.updateMatrix()
            this._scene3D._tGroup.rotation.x = THREE.MathUtils.degToRad(this._roateData.x);
            this._scene3D._tGroup.updateMatrix();
            this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();

            // if (this._scene3D._tGroup.userData.scaleMatrix == undefined) {
            //     this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
            // } else {
            //     this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.userData.scaleMatrix.multiply(this._scene3D._tGroup.matrix.clone());
            // }
        }
        
        //y轴变换
        if (this._roateData.y != undefined) {
            //修改变换模式
            this._scene3D._meshGroup.rotation.y = THREE.MathUtils.degToRad(-this._roateData.y);
            this._scene3D._meshGroup.updateMatrix();
    
            this._scene3D._tGroup.rotation.z = THREE.MathUtils.degToRad(-this._roateData.y);
            this._scene3D._tGroup.updateMatrix();
            
            this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
            // if (this._scene3D._tGroup.userData.scaleMatrix == undefined) {
            //     this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
            // } else {
            //     this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.userData.scaleMatrix.multiply(this._scene3D._tGroup.matrix.clone());
            // }
        }
        
        //z轴变换
        if (this._roateData.z != undefined) {
            //修改变换模式
            if (this._roateData.y == undefined) {
                let useDegree = parseFloat(this._roateData.z)
                this._scene3D._meshGroup.rotation.y = THREE.MathUtils.degToRad(180-useDegree);
                this._scene3D._meshGroup.updateMatrix();
        
                this._scene3D._tGroup.rotation.z = THREE.MathUtils.degToRad(180-useDegree);
                this._scene3D._tGroup.updateMatrix();

            } else {
                let useDegree = parseFloat(this._roateData.y);
                if (useDegree == 90) {
                    useDegree = 100;
                }
                this._scene3D._meshGroup.rotation.y = THREE.MathUtils.degToRad(180-useDegree);
                this._scene3D._meshGroup.updateMatrix();
        
                this._scene3D._tGroup.rotation.z = THREE.MathUtils.degToRad(180-useDegree);
                this._scene3D._tGroup.updateMatrix();
            }
            this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
            // if (this._scene3D._tGroup.userData.scaleMatrix == undefined) {
            //     this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
            // } else {
            //     this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.userData.scaleMatrix.multiply(this._scene3D._tGroup.matrix.clone());
            // }

            // let useDegree = parseFloat(this._roateData.z)
            // this._scene3D._meshGroup.rotation.z = THREE.MathUtils.degToRad(-useDegree);
            // this._scene3D._meshGroup.updateMatrix();
    
            // this._scene3D._tGroup.rotation.y = THREE.MathUtils.degToRad(useDegree);
            // this._scene3D._tGroup.updateMatrix();
            // this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix;
        }
    }

    /**
     * 设置模型旋转功能
     * @param coordinates 旋转坐标
     * @param degree 旋转角度
     */
    public onModelRotate(coordinates: string, degree: any) {
        if (this._scene3D._meshGroup == undefined) return;
        let tempData = undefined;

        this._scaleFlag = false;

        let useDegree = parseFloat(degree)
        
        let tMatrix = new THREE.Matrix4();
        let scope = this;
        if (this._tenum == 0) {
            this._scene3D._tGroup = this._scene3D._meshGroup.clone();
            this._scene3D._tGroup.matrixAutoUpdate = false;
            this._tenum ++;
        }
        switch(coordinates) {
            case 'x':
                //旋转操作
                this._arrow.push('x');

                if (!this._roateData.hasOwnProperty('x')) {
                    this._roateData['x'] = useDegree;
                } else {
                    this._roateData.x = useDegree;
                }
               
                let flagx = this._arrow.every(function(el: any) { 
                    return el == scope._arrow[0]; 
                });

                if (flagx) {
                    this._scene3D._meshGroup.rotation.x = THREE.MathUtils.degToRad(useDegree);
                    this._scene3D._meshGroup.updateMatrix();
                    this._rotateFlagZ = true;
                    this._rotateFlagY = true;

                    //操作矩阵
                    this._scene3D._tGroup.rotation.x = THREE.MathUtils.degToRad(useDegree);
                    this._scene3D._tGroup.updateMatrix();
                    tMatrix.makeRotationX(THREE.MathUtils.degToRad(-useDegree));

                    if (this._scene3D._meshGroup.userData.scaleMatrix == undefined) {
                        this._scene3D._meshGroup.userData.tempMatrix = tMatrix.clone();
                    } else {
                        this._scene3D._meshGroup.userData.tempMatrix = tMatrix.multiply(this._scene3D._meshGroup.userData.scaleMatrix);
                    }
                } else {

                    this._scene3D._meshGroup.rotation.x = THREE.MathUtils.degToRad(-useDegree);
                    this._scene3D._meshGroup.updateMatrix();
                    this._scene3D._tGroup.rotation.x = THREE.MathUtils.degToRad(-useDegree);
                    this._scene3D._tGroup.updateMatrix();

                    this._rotateFlagZ = true;
                    this._rotateFlagY = true;

                    this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
                } 

                if (!flagx && Object.keys(this._roateData).length == 3 || scope._arrow.length > 2) {
                    this._roateData.flag = true;
                    this.onSetModelRotate();
                }

                break;

            case 'y':
                this._arrow.push('y');

                if (!this._roateData.hasOwnProperty('y')) {
                    this._roateData['y'] = useDegree;
                } else {
                    this._roateData.y = useDegree;
                }
               
                let flagy = this._arrow.every(function(el: any) { 
                    return el == scope._arrow[0]; 
                });

                if (flagy) {
                    this._scene3D._meshGroup.rotation.z = THREE.MathUtils.degToRad(-useDegree);
                    this._scene3D._meshGroup.updateMatrix();
                    this._rotateFlagY = true;
                    this._rotateFlagX = true;

                    //操作矩阵
                    this._scene3D._tGroup.rotation.y = THREE.MathUtils.degToRad(useDegree);
                    this._scene3D._tGroup.updateMatrix();
                    tMatrix.makeRotationY(THREE.MathUtils.degToRad(-useDegree));
                    // this._scene3D._meshGroup.userData.tempMatrix = tMatrix.clone();

                    if (this._scene3D._meshGroup.userData.scaleMatrix == undefined) {
                        this._scene3D._meshGroup.userData.tempMatrix = tMatrix.clone();
                    } else {
                        this._scene3D._meshGroup.userData.tempMatrix = tMatrix.multiply(this._scene3D._meshGroup.userData.scaleMatrix);
                    }
                    
                } else {
                    if (scope._arrow.length == 2 && scope._arrow[0] == 'x') {
                        this._scene3D._meshGroup.rotation.z = THREE.MathUtils.degToRad(-useDegree);
                        this._scene3D._meshGroup.updateMatrix();
    
                        this._scene3D._tGroup.rotation.y = THREE.MathUtils.degToRad(useDegree);
                        this._scene3D._tGroup.updateMatrix();

                        if (this._scene3D._tGroup.userData.scaleMatrix == undefined) {
                            this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
                        } else {
                            this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.userData.scaleMatrix.multiply(this._scene3D._tGroup.matrix.clone());
                        }
    
                    } else if (scope._arrow.length == 2 && scope._arrow[0] == 'z') {
                        this._scene3D._meshGroup.rotation.y = THREE.MathUtils.degToRad(useDegree);
                        this._scene3D._meshGroup.updateMatrix();

                        this._scene3D._tGroup.rotation.z = THREE.MathUtils.degToRad(useDegree);
                        this._scene3D._tGroup.updateMatrix();
    
                        // this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
                        if (this._scene3D._tGroup.userData.scaleMatrix == undefined) {
                            this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
                        } else {
                            this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.userData.scaleMatrix.multiply(this._scene3D._tGroup.matrix.clone());
                        }
                    } 
                    // else if (scope._arrow.length >= 2 && scope._arrow[1] == 'y') {

                    //     this._scene3D._meshGroup.rotation.z = THREE.MathUtils.degToRad(useDegree);

                    //     this._scene3D._tGroup.rotation.y = THREE.MathUtils.degToRad(-useDegree);
                    //     this._scene3D._tGroup.updateMatrix();
    
                    //     this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix;

                    // } else if (scope._arrow.length >= 2 && scope._arrow[1] == 'z') {

                    //     this._scene3D._meshGroup.rotation.y = THREE.MathUtils.degToRad(-useDegree);

                    //     this._scene3D._tGroup.rotation.z = THREE.MathUtils.degToRad(-useDegree);
                    //     this._scene3D._tGroup.updateMatrix();
    
                    //     this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix;

                    // } else if (scope._arrow[0] == 'y' && scope._arrow[1] == 'z' && scope._arrow[2] == 'z') {
                    //     this.onSetModelRotate();

                    // } else if (scope._arrow[0] == 'y' && scope._arrow[1] == 'y' && scope._arrow[2] == 'x') {
                    //     this.onSetModelRotate();
                    // } 
                    else if (!flagx && Object.keys(this._roateData).length == 3 || scope._arrow.length > 2) {
                        this._roateData.flag = true;
                        this.onSetModelRotate();
                    }
                }
                break;

            case 'z':
                this._arrow.push('z');

                if (!this._roateData.hasOwnProperty('z')) {
                    this._roateData['z'] = useDegree;
                } else {
                    this._roateData.z = useDegree;
                }

                let flagz = this._arrow.every(function(el: any) { 
                    return el == scope._arrow[0]; 
                });

                if (flagz) {
                    this._scene3D._meshGroup.rotation.y = THREE.MathUtils.degToRad(-useDegree);
                    this._scene3D._meshGroup.updateMatrix();
                    this._rotateFlagZ = true;
                    this._rotateFlagX = true;

                    //操作矩阵
                    this._scene3D._tGroup.rotation.z = THREE.MathUtils.degToRad(-useDegree);
                    this._scene3D._tGroup.updateMatrix();

                    tMatrix.makeRotationZ(THREE.MathUtils.degToRad(useDegree));
                    // this._scene3D._meshGroup.userData.tempMatrix = tMatrix.clone();
                    if (this._scene3D._meshGroup.userData.scaleMatrix == undefined) {
                        this._scene3D._meshGroup.userData.tempMatrix = tMatrix.clone();
                    } else {
                        this._scene3D._meshGroup.userData.tempMatrix = tMatrix.multiply(this._scene3D._meshGroup.userData.scaleMatrix);
                    }
                } else {

                    if (scope._arrow.length == 2 && scope._arrow[0] == 'x') {
                        this._scene3D._meshGroup.rotation.y = THREE.MathUtils.degToRad(useDegree);
                        this._scene3D._meshGroup.updateMatrix();

                        this._scene3D._tGroup.rotation.z = THREE.MathUtils.degToRad(useDegree);
                        this._scene3D._tGroup.updateMatrix();
    
                        // this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
                        if (this._scene3D._tGroup.userData.scaleMatrix == undefined) {
                            this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
                        } else {
                            this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.userData.scaleMatrix.multiply(this._scene3D._tGroup.matrix.clone());
                        }

                    } else if (scope._arrow.length == 2 && scope._arrow[0] == 'y') {

                        this._scene3D._meshGroup.rotation.z = THREE.MathUtils.degToRad(-useDegree);
                        this._scene3D._meshGroup.updateMatrix();
    
                        this._scene3D._tGroup.rotation.y = THREE.MathUtils.degToRad(useDegree);
                        this._scene3D._tGroup.updateMatrix();
    
                        // this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
                        if (this._scene3D._tGroup.userData.scaleMatrix == undefined) {
                            this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
                        } else {
                            this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.userData.scaleMatrix.multiply(this._scene3D._tGroup.matrix.clone());
                        }
                    } 

                    else if (!flagx && Object.keys(this._roateData).length == 3 || scope._arrow.length > 2) {
                        this._roateData.flag = true;
                        this._roateData.y = useDegree;
                        this.onSetModelRotate();
                    }
                }

                break;

            default:
                break;
        }
       
        this._rotateFlag = true;
        
        //计算模型的尺寸
        tempData = this.getTransformRotateModelData(undefined);  
      
        //改变模型变换的尺寸和缩放比例
        if (tempData != undefined) {
            this._scene3D.getClipControlManager()?.setUpdateModelRoateSize(tempData);
        }           
        return tempData
    }

    /**
     * 设置模型缩放功能
     * @param coordinates 缩放坐标
     * @param percentage 缩放比例值
     * @param equalRatioValue 等比缩放值
     */
    public onModelScale(coordinates: string, percentage: any, equalRatioValue: number) {
        if (this._scene3D._meshGroup == undefined) return;
        let tempData = undefined;
      
        let scaleMatrix = new THREE.Matrix4();

        let rx = this._scene3D._meshGroup.rotation.x;
        let ry = this._scene3D._meshGroup.rotation.y;
        let rz = this._scene3D._meshGroup.rotation.z;

        if (rx == 0 && ry ==0 && rz == 0) {
            this._rotateFlagX = false;
            this._rotateFlagY = false;
            this._rotateFlagZ = false;
            this._rotateFlag = false;
        }

        //克隆新的模型
        if (this._tenum == 0) {
            this._scene3D._tGroup = this._scene3D._meshGroup.clone();
            this._scene3D._tGroup.matrixAutoUpdate = false;
            this._tenum ++;
        }

        switch(coordinates) {
            case 'x':
                //缩放操作
                if (this._rotateFlagX) {
                    this._scaleFlag = true;
                   //等比例缩放比例值
                   scaleMatrix.makeScale(equalRatioValue, 1, 1);
                   //根据缩放比例进行模型变换
                   scaleMatrix.multiply(this._scene3D._meshGroup.matrix);
                   this._scene3D._meshGroup.matrix = scaleMatrix;
                
                   this._scene3D._meshGroup.scale.setFromMatrixScale(this._scene3D._meshGroup.matrix);
                   this._scene3D._meshGroup.updateMatrix();
                   
                   // this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._meshGroup.matrix;

                   let scaleMatrix3 = new THREE.Matrix4();
                   scaleMatrix3.makeScale(equalRatioValue, 1, 1);
                   scaleMatrix3.multiply(this._scene3D._tGroup.matrix);
                   this._scene3D._tGroup.matrix = scaleMatrix3;
                   this._scene3D._tGroup.scale.setFromMatrixScale(this._scene3D._tGroup.matrix);
                   this._scene3D._tGroup.updateMatrix();

                   this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                   this._scene3D._tGroup.userData.scaleMatrix = this._scene3D._tGroup.matrix.clone().transpose();

                   this._arrow.push('scaleFlag');

                } else {
                    this._scaleFlag = false;
                    this._scene3D._meshGroup.scale.x = percentage;
                    this._scene3D._meshGroup.updateMatrix();

                    this._scene3D._tGroup.scale.x = percentage;
                    this._scene3D._tGroup.updateMatrix();

                    if (this._arrow.length == 0) {
                        this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
                        this._scene3D._tGroup.userData.scaleMatrix = this._scene3D._tGroup.matrix.clone();
                    } else {
                        this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                        this._scene3D._tGroup.userData.scaleMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                    }
                }

                tempData = this.getTransformScaleModelData('x', percentage, undefined);
                break;

            case 'z':
                //缩放操作
                if (this._rotateFlagY ) {
                    this._scaleFlag = true;
                    //等比例缩放比例值
                    scaleMatrix.makeScale(1, equalRatioValue, 1);
                    //根据缩放比例进行模型变换
                    scaleMatrix.multiply(this._scene3D._meshGroup.matrix);
                    this._scene3D._meshGroup.matrix = scaleMatrix;
                    this._scene3D._meshGroup.scale.setFromMatrixScale(this._scene3D._meshGroup.matrix);
                    this._scene3D._meshGroup.updateMatrix();
                    // this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._meshGroup.matrix;

                    let scaleMatrix3 = new THREE.Matrix4();
                    scaleMatrix3.makeScale(1, equalRatioValue, 1);
                    scaleMatrix3.multiply(this._scene3D._tGroup.matrix);
                    this._scene3D._tGroup.matrix = scaleMatrix3;
                    this._scene3D._tGroup.scale.setFromMatrixScale(this._scene3D._tGroup.matrix);
                    this._scene3D._tGroup.updateMatrix();

                    this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                    this._scene3D._tGroup.userData.scaleMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                    this._arrow.push('scaleFlag');

                } else {
                    this._scaleFlag = false;
                    this._scene3D._meshGroup.scale.y = percentage;
                    this._scene3D._meshGroup.updateMatrix()
                   
                    this._scene3D._tGroup.scale.y = percentage;
                    this._scene3D._tGroup.updateMatrix();

                    if (this._arrow.length == 0) {
                        this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
                        this._scene3D._tGroup.userData.scaleMatrix = this._scene3D._tGroup.matrix.clone();
                    } else {
                        this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                        this._scene3D._tGroup.userData.scaleMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                    }
                }
                tempData = this.getTransformScaleModelData('z', percentage, undefined);
                break;

            case 'y':
                //缩放操作
                if (this._rotateFlagZ) {
                    this._scaleFlag = true;
                    //等比例缩放比例值
                    scaleMatrix.makeScale(1, 1, equalRatioValue);
                    //根据缩放比例进行模型变换
                    scaleMatrix.multiply(this._scene3D._meshGroup.matrix);
                   
                    this._scene3D._meshGroup.matrix = scaleMatrix;
                    this._scene3D._meshGroup.scale.setFromMatrixScale(this._scene3D._meshGroup.matrix);
                    this._scene3D._meshGroup.matrixWorldNeedsUpdate = true;
                    // this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._meshGroup.matrix;

                    let scaleMatrix3 = new THREE.Matrix4();
                    scaleMatrix3.makeScale(1, 1, equalRatioValue);
                    scaleMatrix3.multiply(this._scene3D._tGroup.matrix);
                    this._scene3D._tGroup.matrix = scaleMatrix3;
                    this._scene3D._tGroup.scale.setFromMatrixScale(this._scene3D._tGroup.matrix);
                    this._scene3D._tGroup.updateMatrix();

                    this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                    this._scene3D._tGroup.userData.scaleMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                    this._arrow.push('scaleFlag');
                    
                } else {
                    this._scaleFlag = false;
                    this._scene3D._meshGroup.scale.z = percentage;
                    this._scene3D._meshGroup.updateMatrix();

                    this._scene3D._tGroup.scale.z = percentage;
                    this._scene3D._tGroup.updateMatrix();

                    if (this._arrow.length == 0) {
                        this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
                        this._scene3D._tGroup.userData.scaleMatrix = this._scene3D._tGroup.matrix.clone();
                    } else {
                        this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                        this._scene3D._tGroup.userData.scaleMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                    }
                }
               
                tempData = this.getTransformScaleModelData('y', percentage, undefined);
                break;

            case 'all':
                if (this._rotateFlagX || this._rotateFlagY || this._rotateFlagZ) {
                    this._scaleFlag = true;
                    //等比例缩放比例值
                    scaleMatrix.makeScale(equalRatioValue, equalRatioValue, equalRatioValue);
                    //根据缩放比例进行模型变换
                    scaleMatrix.multiply(this._scene3D._meshGroup.matrix);
                    this._scene3D._meshGroup.matrix = scaleMatrix;
                    this._scene3D._meshGroup.scale.setFromMatrixScale(this._scene3D._meshGroup.matrix);
                    this._scene3D._meshGroup.updateMatrix();

                    let scaleMatrix2 = new THREE.Matrix4();
                    //等比例缩放比例值
                    scaleMatrix2.makeScale(equalRatioValue, equalRatioValue, equalRatioValue);
                    //根据缩放比例进行模型变换
                    scaleMatrix2.multiply(this._scene3D._tGroup.matrix);
                    this._scene3D._tGroup.matrix = scaleMatrix2;
                    this._scene3D._tGroup.scale.setFromMatrixScale(this._scene3D._tGroup.matrix);
                    this._scene3D._tGroup.updateMatrix();

                    this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                    this._scene3D._tGroup.userData.scaleMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                    this._arrow.push('scaleFlag');

                } else {
                    this._scaleFlag = false;
                    //等比例缩放比例值
                    scaleMatrix.makeScale(equalRatioValue, equalRatioValue, equalRatioValue);
                    //根据缩放比例进行模型变换
                    scaleMatrix.multiply(this._scene3D._meshGroup.matrix);
                    this._scene3D._meshGroup.matrix = scaleMatrix;
                    this._scene3D._meshGroup.scale.setFromMatrixScale(this._scene3D._meshGroup.matrix);
                    this._scene3D._meshGroup.updateMatrix();

                    //关联旋转功能
                    var scaleMatrix2 = new THREE.Matrix4();
                    scaleMatrix2.makeScale(equalRatioValue, equalRatioValue, equalRatioValue);
                    //根据缩放比例进行模型变换
                    scaleMatrix2.multiply(this._scene3D._tGroup.matrix);
                    this._scene3D._tGroup.matrix = scaleMatrix;
                    this._scene3D._tGroup.scale.setFromMatrixScale(this._scene3D._tGroup.matrix);
                    this._scene3D._tGroup.updateMatrix();
                    if (this._arrow.length == 0) {
                        this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone();
                        this._scene3D._tGroup.userData.scaleMatrix = this._scene3D._tGroup.matrix.clone();
                    }
                    else {
                        this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                        this._scene3D._tGroup.userData.scaleMatrix = this._scene3D._tGroup.matrix.clone().transpose();
                    }

                    // //关联旋转功能
                    // if (this._scene3D._meshGroup.userData.tempMatrix == undefined) {
                    //     this._scene3D._meshGroup.userData.tempMatrix = this._scene3D._meshGroup.matrix.clone();
                    //     this._scene3D._tGroup.userData.scaleMatrix = this._scene3D._meshGroup.matrix.clone();

                    // } else {
                    // let scaleMatrix2 = new THREE.Matrix4();
                    // scaleMatrix2.makeScale(equalRatioValue, equalRatioValue, equalRatioValue);
                    // this._scene3D._meshGroup.userData.tempMatrix = scaleMatrix2.multiply(this._scene3D._meshGroup.userData.tempMatrix);
                    // this._scene3D._tGroup.userData.scaleMatrix = scaleMatrix2.multiply(this._scene3D._meshGroup.userData.tempMatrix);
                    // }
                }
                
                //计算模型的尺寸
                tempData = this.getTransformScaleModelData('all', percentage, equalRatioValue);

            default:
                break;
        }
       
        //重置模型的缩放比例
        if (tempData != undefined) {
            this._scene3D.getClipControlManager()?.setUpdateModelSize(tempData);
        }   
        return tempData;
    }
    /**
     * 
     * @returns 
     */
    private getTransformScaleModelData(type: any, percentage: any, equalRatioValue: any) {
        if (this._scene3D._meshGroup == undefined) return;

        let tempData = {};
        //计算模型的包围盒
        let modelS = new THREE.Box3().expandByObject(this._scene3D._meshGroup);

        let mSize = modelS.getSize(new THREE.Vector3());
        let modelSize = {'x': parseFloat(mSize.x.toFixed(2)), 'y': parseFloat(mSize.z.toFixed(2)), 'z': parseFloat(mSize.y.toFixed(2))}
        let sizeData = this._scene3D.getClipControlManager()?.getTransformModelSize();

        if (percentage != undefined) {
            this._scalingMultiple = percentage;
        }

        if (this._rotateFlag) {
            switch (type) {
                case 'x': 
                    modelSize.x = sizeData.modeSize.x * percentage;
                break;
    
                case 'y':
                    modelSize.y = sizeData.modeSize.y * percentage;
                    break;
                   
                case 'z':
                    modelSize.z = sizeData.modeSize.z * percentage;
                break;    
    
                case 'all':
                    modelSize.x = sizeData.size.x * equalRatioValue;
                    modelSize.y = sizeData.size.y * equalRatioValue;
                    modelSize.z = sizeData.size.z * equalRatioValue;
                    break;
            }
        }
       
        //计算模型尺寸
        let rangeX = (1 / mSize.x) * 100;
        let xMin = rangeX;
        
        let rangeY = (1 / mSize.y) * 100;
        let yMin = rangeY;

        let rangeZ = (1 / mSize.z) * 100;
        let zMin = rangeZ;

        let rMin = Math.max(xMin, yMin, zMin);
        rMin = this._scalingMultiple * rMin;

        //计算模型的缩放范围
        let Xrange = ( sizeData.boxSize.x / mSize.x ) * 100;
        let Yrange = ( sizeData.boxSize.y / mSize.y ) * 100;
        let Zrange = ( sizeData.boxSize.z / mSize.z ) * 100;

        let minRange = Math.min(Xrange, Yrange, Zrange);
        minRange = minRange * this._scalingMultiple;
        let rangeNum = parseFloat(minRange.toFixed(2));

        //重新调整模型的位置
        let box = new THREE.Box3().expandByObject(this._scene3D._meshGroup.children[0]);
        let centerLen = new THREE.Vector3().subVectors(box.max, box.min).multiplyScalar(0.5);
        if ((sizeData.boxSize.y/2) < centerLen.y) {
            this._scene3D._meshGroup.position.y = -(centerLen.y - sizeData.boxSize.y/2);
        } else {
            this._scene3D._meshGroup.position.y = -(sizeData.boxSize.y/2 - centerLen.y);
        }
       
        tempData = {size: modelSize, scaleRange: {min: parseFloat(rMin.toFixed(2)) , max: parseFloat(rangeNum.toFixed(2))}, scaleRangeX: {min: sizeData.scaleRangeX.min, max: sizeData.scaleRangeX.max}, 
        scaleRangeY: {min: sizeData.scaleRangeY.min, max: sizeData.scaleRangeY.max}, scaleRangeZ: {min: sizeData.scaleRangeZ.min, max: sizeData.scaleRangeZ.max}};
            
        return tempData;
    }

    /**
     * 
     * @returns 
     */
    private getTransformRotateModelData(percentage: any) {
        if (this._scene3D._meshGroup == undefined) return;

        let tempData = {};
        //计算模型的包围盒
        let modelS = new THREE.Box3().expandByObject(this._scene3D._meshGroup);

        let mSize = modelS.getSize(new THREE.Vector3());
        let modelSize = {'x': parseFloat(mSize.x.toFixed(2)), 'y': parseFloat(mSize.z.toFixed(2)), 'z': parseFloat(mSize.y.toFixed(2))}
        let sizeData = this._scene3D.getClipControlManager()?.getTransformModelSize();

        if (percentage != undefined) {
            this._scalingMultiple = percentage;
        }
       
        //计算模型尺寸
        let rangeX = (1 / mSize.x) * 100;
        let xMin = rangeX;
        
        let rangeY = (1 / mSize.y) * 100;
        let yMin = rangeY;

        let rangeZ = (1 / mSize.z) * 100;
        let zMin = rangeZ;

        let rMin = Math.max(xMin, yMin, zMin);
        rMin = this._scalingMultiple * rMin;

        //计算模型的缩放范围
        let Xrange = ( sizeData.boxSize.x / mSize.x ) * 100;
        let Yrange = ( sizeData.boxSize.y / mSize.y ) * 100;
        let Zrange = ( sizeData.boxSize.z / mSize.z ) * 100;

        let minRange = Math.min(Xrange, Yrange, Zrange);
        minRange = minRange * this._scalingMultiple;
        let rangeNum = parseFloat(minRange.toFixed(2));

        //重新调整模型的位置
        let box = new THREE.Box3().expandByObject(this._scene3D._meshGroup.children[0]);
        let centerLen = new THREE.Vector3().subVectors(box.max, box.min).multiplyScalar(0.5);
        if ((sizeData.boxSize.y/2) < centerLen.y) {
            this._scene3D._meshGroup.position.y = -(centerLen.y - sizeData.boxSize.y/2);
        } else {
            this._scene3D._meshGroup.position.y = -(sizeData.boxSize.y/2 - centerLen.y);
        }

        tempData = {size: modelSize, scaleRange: {min: parseFloat(rMin.toFixed(2)) , max: parseFloat(rangeNum.toFixed(2))}, scaleRangeX: {min: sizeData.scaleRangeX.min, max: sizeData.scaleRangeX.max}, 
        scaleRangeY: {min: sizeData.scaleRangeY.min, max: sizeData.scaleRangeY.max}, scaleRangeZ: {min: sizeData.scaleRangeZ.min, max: sizeData.scaleRangeZ.max}};
            
        return tempData;
    }

    private onLocalResetModel() {
        if (this._scene3D._meshGroup == undefined) return;
        
        //恢复旋转视角
        this._scene3D._meshGroup.rotation.set(0, 0, 0);

        this._scene3D._tGroup.rotation.set(0, 0, 0);

        //恢复缩放视角
        // this._scene3D._meshGroup.scale.set(1, 1, 1);
        // this._scene3D._tGroup.scale.set(1, 1, 1);

        this._rotateFlagX = false;
        this._rotateFlagY = false;
        this._rotateFlagZ = false;
        this._rotateFlag = false;

        //重新调整模型的位置
        this._scene3D._meshGroup.position.y = 0;

        this._scene3D._tGroup.position.y = 0;

        let sizeData = this._scene3D.getClipControlManager()?.getTransformModelSize();
        let box = new THREE.Box3().expandByObject(this._scene3D._meshGroup);
        let centerLen = new THREE.Vector3().subVectors(box.max, box.min).multiplyScalar(0.5);
        this._scene3D._meshGroup.position.y = -(sizeData.boxSize.y/2 - centerLen.y);

        this._scene3D._tGroup.position.y = -(sizeData.boxSize.y/2 - centerLen.y);

        //更新相机的坐标
        this._scene3D._meshGroup.updateMatrix();

        this._scene3D._tGroup.updateMatrix();

        //调整相机的视角
        this.resetCameraFov();
    }


    /**
     * 模型复位功能
     * 恢复到初始状态
     */
    public onResetModel() {
        if (this._scene3D._meshGroup == undefined) return;

        this._roateData.x = 0;
        this._roateData.y = 0;
        this._roateData.y = 0;
        this._roateData.flag = false;

        this._arrow = [];
        this._roateData = {};

        //恢复旋转视角
        this._scene3D._meshGroup.rotation.set(0, 0, 0);

        this._scene3D._tGroup.rotation.set(0, 0, 0);

        //恢复缩放视角
        this._scene3D._meshGroup.scale.set(1, 1, 1);

        this._scene3D._tGroup.scale.set(1, 1, 1);

        this._rotateFlagX = false;
        this._rotateFlagY = false;
        this._rotateFlagZ = false;
        this._rotateFlag = false;

        //重新调整模型的位置
        this._scene3D._meshGroup.position.y = 0;

        this._scene3D._tGroup.position.y = 0;

        let sizeData = this._scene3D.getClipControlManager()?.getTransformModelSize();
        let box = new THREE.Box3().expandByObject(this._scene3D._meshGroup);
        let centerLen = new THREE.Vector3().subVectors(box.max, box.min).multiplyScalar(0.5);
        this._scene3D._meshGroup.position.y = -(sizeData.boxSize.y/2 - centerLen.y);

        this._scene3D._tGroup.position.y = -(sizeData.boxSize.y/2 - centerLen.y);

        //更新相机的坐标
        this._scene3D._meshGroup.updateMatrix();

        this._scene3D._tGroup.updateMatrix();

        //调整相机的视角
        this.resetCameraFov();
    }

    /**
     * 获取模型变换的世界矩阵
     * @returns 返回变换后的模型矩阵数据
     */
    public getModelMatrixWorld() {
        if (this._scene3D._meshGroup == undefined) return;
        //获取模型变换后的矩阵
        let scope = this;
        let len = this._arrow.length;
        let flag = this._arrow.every(function(el: any){
            return el == scope._arrow[0]
        })
       
        if (flag || scope._arrow[len-1] == 'scaleFlag') {
            let elementData = this._scene3D._meshGroup.userData.tempMatrix;
            this._scene3D._tGroup.matrixAutoUpdate = false;
            this._scene3D._tGroup.updateMatrix();
            //设置模型归位
            let data = elementData.clone();
            data.setPosition(0,0,0);
            return data;

        } else {
            let elementData = this._scene3D._meshGroup.userData.tempMatrix;
            let data = elementData.clone();

            data.setPosition(0,0,0);
            let dd = data.transpose();
            return dd;
        }
    }

    /**
     * 比较模型和机型的大小
     * @returns 返回值为boolean（true/false）
     */
    public onCompareModelSize() {
        if (this._scene3D._meshGroup == undefined) return;
        let modelS = new THREE.Box3().setFromObject(this._scene3D._meshGroup);
        let mSize = modelS.getSize(new THREE.Vector3());
        let axisSize = this._scene3D.getClipControlManager()?.getTransformModelSize();

        let flagX = axisSize.boxSize.x > mSize.x ? true: false;
        let flagY = axisSize.boxSize.y > mSize.y ? true: false;
        let flagZ = axisSize.boxSize.z > mSize.z ? true: false;

        if (flagX && flagY && flagZ) {
           return true
        } else{
            return false;
        }
    }

   /**
    * 销毁事件
    */
    public dispose() {
        this._scene3D.getCanvas()?.removeEventListener('dblclick', (event) => this.onDbclick(event), false);
        this._scene3D.getCanvas()?.removeEventListener('touchstart', (event) => this.onClick(event), false);
    }

    /**
     * 获取模型变换后的模型矩阵
     * @returns 返回矩阵数组
     */
    public getModelChangeMatrix(){
        if (this._scene3D._meshGroup == undefined) return;
        let matrixs = this._scene3D._meshGroup.matrix;
        let matrix = matrixs.clone();
        return matrix.elements;
    }

    /**
     * 通过传值设置模型矩阵
     * @param elementData 矩阵数组
     * @returns null
     */
    public setModelMatrix(elementData: any) {
        if (this._scene3D._meshGroup == undefined) return;
        let matrix4 = new THREE.Matrix4().fromArray(elementData);
        this._scene3D._meshGroup.position.set(0,0,0);
        this._scene3D._meshGroup.applyMatrix4(matrix4);
    }
    
    /**
     * 获取模型场景的快照功能
     * @param flag (true/false)
     */
    public onDrawingBuffer(flag: boolean): any {
        let scope = this;
        if (flag) {
           // 快照功能
           let render = scope._scene3D.getRenderer();
           if (render == undefined) return;
           let imgData = render.domElement.toDataURL();

           //返回快照图片base64位
           return imgData;
        } else {
            return undefined;
        }
    }
}