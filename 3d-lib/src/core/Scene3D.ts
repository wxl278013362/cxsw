import * as THREE from "three";
// @ts-ignore 
import * as Stats from "three-examples/libs/stats.min"; 
import { App } from "./App";
import { CameraManager } from '../managers/CameraManager';
import { InteractionManager } from "../managers/InteractionManager";
import { SceneConfig } from "../configDefine/SceneConfig";

import { StlDataCollection } from "../objects/StlDataCollection";
import { MeshData} from "../objects/MeshData";
import { ClipControlManager } from "../managers/ClipControlManager";
//模型着色
import { ModelColorManager } from "../managers/ModelColorManager";
import { EventControl } from '../event/EventControl';

import {heatMapManager} from '../managers/heatMapManager';
import {LightManager} from '../managers/LightManager';

/**
 * 自定义3D场景。当加载数据时，生成的对象会自动添加到
 */
export class Scene3D {
    public _temGroup: THREE.Group | undefined | null;
    public _meshGroup: THREE.Group | undefined | null;
    public _outBoxGroup: THREE.Group | undefined | null;

    public _endCameraDistance: number = -100;
    public _cameralView: THREE.Vector3 | undefined | null;

    public _active: boolean = false;
    public _tempConfig: SceneConfig | undefined | null;
    public _loadNum: number = 0;
    public _vertexData: any;
    public skip: number = 0;
    public _tempSkip: number = 8;

    public errorInfo: undefined;

    public _enable: any = false;

    public _windowResize: any = false;

    public _windowScale: any = false;

    private _time: any;

    public _modelData: any = undefined;

    public _tdiv: any = undefined;

    public _worker: any;

    public _tGroup: THREE.Group;

    /**
     * 包含此场景的容器id
     */
    private _containerId: string;
    /**
     * 得到场景的HTML容器的id
     */
    public getContainerId(): string {
        return this._containerId;
    }
    /**
     * 包含此场景的容器
     */
    private _container: HTMLElement | undefined;
    /**
     * 得到场景的HTML容器
     */
    public getContainer(): HTMLElement | undefined{
        return this._container;
    }

    private _mainContainer: HTMLElement | null;
    public getMainContainer(): HTMLElement | null {
        return this._mainContainer;
    }

     /**
     * 包含此场景的容器
     */
    public _tempContainer: HTMLElement| undefined | null;

    /**
     * 画布.canvas元素
     */
    private _canvas: HTMLCanvasElement | undefined | null;

    /**
     * 得到画布的canvas元素
     */
    public getCanvas(): HTMLCanvasElement | undefined | null{
        return this._canvas;
    }

    /**
     * webgl渲染器
     */
    private _renderer: THREE.WebGLRenderer | undefined | null;
    /**
     * 得到webgl渲染器
     */
    public getRenderer(): THREE.WebGLRenderer | undefined | null{
        return this._renderer;
    }

    /**
     * 帧数显示器
     */
    private _stats: Stats | undefined;

    /**
     * THREE.Scene
     */
    private _scene: THREE.Scene | undefined | null;

    /**
     * 得到THREE.Scene。
     * 现在这个scene不再开放，要切换场景的数据请使用ObjectCollection提供的DataInfo，再使用changeToLevel函数切换场景
     * 普通的数据添加和删除，请使用addObject,removeObject,getObject这几个函数
     */
    public getScene(): THREE.Scene | undefined | null{
        return this._scene;
    }
    /**
     * 相机管理器
     */
    private _cameraManager: CameraManager | undefined | null;
    /**
     * 得到相机管理器
     */
    public getCameraManager(): CameraManager | undefined | null{
        return this._cameraManager;
    }

    /**
     * 创建热力图对象
     */
    private _heatMapManager: heatMapManager | undefined | null;
    
    public getHeatMapManager(): heatMapManager | undefined | null{
        return this._heatMapManager;
    }
    
    /**
     * 交互控件管理器
     */
    private _interactionManager: InteractionManager | undefined | null;
    /**
     * 得到交互控件管理器
     */
    public getInteractionManager(): InteractionManager | undefined | null {
        return this._interactionManager;
    }
    /**
     * 模型剖切效果
     */
    private _clipControlManager: ClipControlManager | undefined | null;
    public getClipControlManager(): ClipControlManager | undefined | null{
        return this._clipControlManager;
    }

    /**
     * 构建着色
     */
    private _modelColorManager: ModelColorManager | undefined | null;
    public getModelColorManager(): ModelColorManager | undefined | null{
        return this._modelColorManager;
    }

    /**
     * 当前场景绑定的数据集
     */
    private _stlDataCollection: StlDataCollection | undefined | null;

    /**
     * 光源管理器
     */
    private _lightManager: LightManager | undefined | null ;
    public getLightManager(): LightManager | undefined | null{
        return this._lightManager;
    }

    /**
     * 场景的名称
     */
    public name: string;
    /**
     * 当前场景的可用状态
     */
    public enable: boolean = true;
    /**
     * Scene3D构造函数
     * @param name 场景的名称。如：'main_scene', 'left_scene'
     * @param containerId 包含场景的容器的id
     * @param config 场景的配置
     */
    constructor(name: string, containerId: string, config?: SceneConfig) {
        this.name = name;
        this._tempConfig = config;
        let container = document.getElementById(containerId);
        this._mainContainer = container;

        if (container == null) {
            return;
        }
        this._containerId = containerId;
        // 自己再建一个div元素，避免父元素的样式影响到3D的元素
        this._container = document.createElement('div');
        this._container.style.width = '100%';
        this._container.style.height = '100%';
        // this._container.style.top = "1px";
        this._container.style.position = 'relative';
        this._container.id = "3DCanvas";
        this._container.style.alignItems = "center";
        this._container.style.zIndex = '0';
        if (this._mainContainer == null) return;
        this._mainContainer.appendChild(this._container);
       
        //判断是否创建等高线图
        if (config?.heatMapFlag) {
            this._tempSkip = 1;
            this._enable = true;

            //创建2D图坐标选中框
            this._tdiv = document.createElement("div");
            this._tdiv.id = "zuobiao";
            this._tdiv.style.position = 'absolute';
            this._tdiv.style.zIndex = '999';
            this._tdiv.style.width = '30px';
            this._tdiv.style.height = '30px';
            this._tdiv.style.top = '10px';
            this._tdiv.style.display = 'none';
            this._mainContainer.appendChild(this._tdiv);

            //添加选中框图片
            let image = document.createElement('img');
            image.style.width = '30px';
            image.style.height = '30px';
            let url = this._tempConfig?.textureUrl;
            if (url != undefined) {
                image.src = url;
                this._tdiv.appendChild(image);
            }
           
            this._tempContainer = undefined;
            this._tempContainer = document.createElement('div');
            this._tempContainer.style.width = container.clientWidth + 'px';
            this._tempContainer.style.height = container.clientHeight + 'px';
            this._tempContainer.id = "heatMap";
            //设置model-view为非圆角样式
            this._mainContainer.style.borderRadius = '0px';
            this._container.appendChild(this._tempContainer);
        }

        this._renderer = new THREE.WebGLRenderer({antialias: true, preserveDrawingBuffer: true});
        this._renderer.info.autoReset = false;
        
        this._renderer.setPixelRatio(window.devicePixelRatio);
        // 因为有非全屏显示场景以及有偏移的存在，使用clientWidth和clientHeight
        if (this._tempContainer) {
            this._tempContainer.appendChild(this._renderer.domElement);
            this._renderer.setSize(this._tempContainer.clientWidth, this._tempContainer.clientHeight);
            this._canvas = this._renderer.domElement;
            this._canvas.id = "render";
        } else {
            this._container.appendChild(this._renderer.domElement);
            this._renderer.setSize(this._mainContainer.clientWidth, this._mainContainer.clientHeight);
            this._canvas = this._renderer.domElement;
            this._canvas.id = "render";
        }
       
        // 创建three.js的三维场景
        this._scene = new THREE.Scene();
        this._scene.name = this.name;
        this._scene.fog = null;

        //添加组构件
        this._temGroup = new THREE.Group();
        this._scene.add(this._temGroup);
        if (this._tempConfig?.pro3GridHelper) {
            this._temGroup.translateZ(-2900);
        }
       
        this._meshGroup = new THREE.Group();

        // const axesHelper1 = new THREE.AxesHelper( 100 );
        // axesHelper1.rotateX(-Math.PI/2)
        // this._scene.add( axesHelper1 );

        this._temGroup.add(this._meshGroup);
        this._outBoxGroup = new THREE.Group();
        this._temGroup.add(this._outBoxGroup);

        this._tGroup = new THREE.Group();
    
        this._temGroup.add(this._tGroup);

        //初始化对象
        this.initConstObject();

        // 解析配置文件
        if (config == undefined) {
            config = {};
        }
        this.loadConfig(config);
        // 创建管理器
        this.initManager(config);

        // 初始化时添加事件
        window.addEventListener('resize', () => this.onWindowResize(), false);

        //删除场景的模型
        EventControl.addEvent('MSG_MODEL_DELETE', ()=> {this.dispose()}, '3D');
    }

    /**
     * 置空变量
     */
    private initConstObject() {
        this._cameralView = new THREE.Vector3();
    }

    /**
     * 删除模型释放缓存
     * @param node 
     */
    private disposeNode (node: any) {
        if (node instanceof THREE.Mesh || node.type == "Line" || node.type == "LineSegments") {
            if (node.type == "Line" || node.type == "LineSegments") {
                node.geometry.getAttribute('position') .array = null;

                if (node.geometry.getAttribute('color') != undefined) {
                    node.geometry.getAttribute('color').array = null;
                }
                if (node.geometry.getAttribute('transparent') != undefined) {
                    node.geometry.getAttribute('transparent').array = null;
                }
                if (node.geometry.getAttribute('normal') != undefined) {
                    node.geometry.getAttribute('normal').array = null;
                }
                if (node.geometry.getAttribute('instanceAlphStart') != undefined) {
                    node.geometry.getAttribute('instanceAlphStart').data.array = null;
                }
                if (node.geometry.getAttribute('instanceStart') != undefined) {
                    node.geometry.getAttribute('instanceStart').data.array = null;
                }
                if (node.geometry.getAttribute('instanceEnd') != undefined) {
                    node.geometry.getAttribute('instanceEnd').data.array = null;
                }
                if (node.geometry.getAttribute('instanceColorStart') != undefined) {
                    node.geometry.getAttribute('instanceColorStart').data.array = null;
                }
                if (node.geometry.getAttribute('instanceColorEnd') != undefined) {
                    node.geometry.getAttribute('instanceColorEnd').data.array = null;
                }
                node.geometry.needsUpdate = true;
            }
            if (node.geometry){
                //深入销毁辅助模型信息
                if (node.geometry.type == "BufferGeometry") {
                    if (node.geometry.getAttribute('position') != undefined) {
                        node.geometry.attributes.position.array = 0;
                    } 
                    if (node.geometry.getAttribute('normal') != undefined) {
                        node.geometry.attributes.normal.array = 0;
                    }
                    if (node.name == "modelStl") {
                        this._meshGroup?.remove(node);
                    }
                } else if (node.geometry.type == "BoxGeometry") {
                    node.geometry.faces = null;
                    node.geometry.vertices = null;
                    node.geometry.faceVertexUvs = null;

                    if (node.geometry._directGeometry != undefined) {
                        node.geometry._directGeometry.colors = null;
                        node.geometry._directGeometry.groups = null;
                        node.geometry._directGeometry.normals = 0;
                        node.geometry._directGeometry.uvs = 0;
                        node.geometry._directGeometry.vertices = 0;
                    }
                }
                node.geometry.needsUpdate = true;
                //销毁模型顶点数据
                node.geometry.dispose();
                node.geometry = null;
            }
            if (node.material) {
                if (node.material.map) node.material.map.dispose ();
                //销毁模型材质
                node.material.dispose ();  
                node.material = null;
            }
        }
    }   

    /**
     * 清除场景的模型对象
     * 清除其他对象变量
     */
    private disposeHierarchy() {
        //剔除场景中的模型
        if (this._temGroup == undefined) return;

         //将模型清出场景
         if (this._scene == undefined) return;
         this._scene.remove(this._temGroup);
 
         let node = this._temGroup.children;
         for (let i=0; i<node.length; ++i) {
 
             let child = node[i];
             if (child.type == 'Mesh') {
                 this.onFindNextChild(child);
 
             } else if (child.type == 'Group') {
                 this.onFindNextData(child as THREE.Group);
             }
         }

        if (this._stlDataCollection != undefined) {
            this._stlDataCollection.disposeActiveMesh();
        }

        this._temGroup.clear();
        if ( this._meshGroup == undefined) return;
        this._meshGroup.clear();

        if (this._outBoxGroup == undefined) return;
        this._outBoxGroup.clear();

        let caDiv = document.getElementById('3DCanvas');
        if (caDiv != undefined) {
            //@ts-ignore
            caDiv.parentNode.removeChild(caDiv);
        }
      
        //清空数据集
        this.unbindStlDataCollection();

        //清空场景所有子集
        this._scene.clear();

        //设置窗口的视角
        if (this._cameraManager == undefined) return;
        this._cameraManager.onWindowResize();

        this.render();
    }

    /**
     * 遍历下一个模型数据
     * @param node 模型节点数据
     */
    private onFindNextChild(node: any) {
        if (node.children.length > 0) {
            for (let i=0; i<node.children.length; ++i) {
                this.disposeNode(node);
                this.disposeNode(node.children[i]);
            }
        } else {
            this.disposeNode(node);
        }
    }

    /**
     * 判断下一个模型节点是否为THREE.Group类型
     * @param group 
     * @returns 
     */
    private onFindNextData(group: THREE.Group) {
        if (group == undefined) return;
        let scope = this;
        let children = group.children;
        for (let i=0; i<children.length; ++i) {

            if (children[i].type == "Group") {

                children[i].traverse(function(mesh: any){
                    if (mesh.type == "Group") {
                        scope.onFindNextData(mesh);

                    } else {
                        scope.onFindNextChild(mesh);
                    }
                });
            } else{
                this.onFindNextChild(children[i]);
            }
        }
    }

    /**
     * 置空对象
     */
    private setNullAnyObject() {
        //清空初始视角数据对象
        this._cameralView = null;
        this._tempConfig = null;

        //清空组对象
        this._temGroup = null;
        this._meshGroup = null;
        this._outBoxGroup = null;

        //清空数据集及常亮
        this._stlDataCollection = null;
        this._endCameraDistance = 0;
        this._loadNum = 0;

        //清空相应画布
        this._container = undefined;
        if (this._canvas != undefined) {
            this._canvas.width = 0; 
            this._canvas.height = 0;
        }
        this._canvas = null;
        
        this._tempContainer = null;
        this._mainContainer = null;
        this._interactionManager = null;
        
        //清空剖切对象
        this._clipControlManager = null;
        //清空着色对象
        this._modelColorManager = null;

        //清空光源对象
        this._lightManager?.clear();
        this._lightManager = undefined;

        //清空场景
        if (this._scene == undefined) return;
        this._scene.clear();
        this._scene = undefined;

        //清除等高线数据及对象
        if (this._heatMapManager == undefined) return;
        this._heatMapManager.dispose();
        this._heatMapManager = undefined;

        //清空颜色值刻度
        let heatMap = document.getElementById("heatMap");
        if (heatMap != undefined) {
            let container = document.getElementById(this._containerId);
            if (container) {
                container.removeChild(heatMap);
                heatMap = null;
            }
        }
    }

    /**
     * 清除渲染器
     */
    private clearRenderer() {
        //清空相机对象
        this._cameraManager = null;

        if (this._renderer == undefined) return;
        this._renderer.getContext().canvas.width = 1;
        this._renderer.getContext().canvas.height = 1;

        this._renderer.setSize(1,1);
        this._renderer.info.programs = [];
        this._renderer.info.memory.geometries = 0;
        this._renderer.info.memory.textures = 0;

        //设置丢失渲染器的上下文
        this._renderer.forceContextLoss();
        this._renderer.dispose();
        this._renderer.getContext() === null;

        this._renderer.clearColor();
        this._renderer.clearDepth();
        this._renderer.clearStencil();
       
        this._renderer = null;
    }
    
    /**
     * 注销监听事件
     */
    private disposeEvent() {
        //删除加载进度事件
        EventControl.removeEvent('MSG_LOAD_3DS_DATA_PROGRESS', ()=> {}, '3D');
        //模型加载错误提示
        EventControl.removeEvent('MSG_MODEL_URL_ERROR', ()=> {}, 'UI');

        EventControl.removeEvent('MSG_LOAD_3DS_DATA_FINISH', ()=> {}, '3D');
        
        EventControl.removeEvent('MSG_CHANGE_MODEL_FOV', ()=> {}, '3D');

        EventControl.removeEvent('MSG_MODEL_POSITION_COUNT', ()=> {}, '3D');

        EventControl.removeEvent('MSG_INIT_CLIP', ()=> {}, '3D');

        //模型剖切值事件
        EventControl.removeEvent('MSG_MODEL_CLIP_VALAUE', ()=> {}, 'UI');

        //删除水桶效果着色事件
        EventControl.removeEvent('MSG_RENDER_MODEL_LAYER_COLOR', ()=> {}, '3D');
        
        //删除水桶效果事件
        EventControl.removeEvent('MSG_RENDER_MODEL_LAYER_PRINTER', ()=> {}, '3D');

        //删除二维平面点选事件
        EventControl.removeEvent('MSG_TOUCH_GET_COORDINATES', ()=> {}, '3D');

        //终止Ajax请求
        EventControl.removeEvent('MSG_Ajax_Event_ABORT', ()=> {}, '3D');

        //获取模型的大小
        EventControl.removeEvent('MSG_GET_MODEL_SIZE', ()=> {}, '3D');

        //创建热力图事件
        EventControl.removeEvent('MSG_CREATE_HEATMAP_MODEL', ()=> {}, '3D');

        //模型随之鼠标的缩放事件
        EventControl.removeEvent('MSG_ON_MODEL_ZOOM', ()=> {}, '3D');

        //销毁所有场景对象的事件
        EventControl.removeEvent('MSG_MODEL_DELETE', ()=> {}, '3D');

        //销毁分片加载事件
        EventControl.removeEvent('MSG_ON_MODEL_LOAD_STOP', ()=> {}, '3D');

        //清空已注册的事件
        EventControl.disposeEvent();

        //销毁注册事件
        this.getInteractionManager()?.dispose();
    }

    /**         
     * 释放场景的资源
     */
    public dispose() {
        // 首先不再渲染场景
        this.enable = false;

        if (this._worker != undefined) {
            this._worker.terminate();
            this._worker = null;
        }
        
        EventControl.dispatchEvent('MSG_Ajax_Event_ABORT', null, 'all');

        window.removeEventListener('resize', () => this.onWindowResize(), false);

        if (this._container == undefined) return;
        let pageContainer = this._container.parentNode;
        if (pageContainer) {
            pageContainer.removeChild(this._container);
        }

        //删除模型
        this.disposeHierarchy();

        // 置空其他变量
        this.setNullAnyObject();

        //清空渲染器
        this.clearRenderer();

        //注销监听事件
        this.disposeEvent();

        let _sceneManager = App.getInstance().getSceneManager();
        //删除当前的场景
        if ((_sceneManager != undefined) || (_sceneManager != null)) {
            _sceneManager.getActiveScene3D("main_scene");
            _sceneManager = null;
        }
       
        let _dataManager = App.getInstance().getDataManager();
        if ((_dataManager != undefined) || (_dataManager != null)) {
            //清除当前的数据集
            _dataManager.clearStl();
            _dataManager = null;
        }
    }

    /**
     * 解析配置文件
     * @param config 
     */
    private loadConfig(config: SceneConfig) {
        // 清除背景色
        if (this._renderer == undefined) return;
        if (this._scene == undefined) return;
        if (config.clearColor) {
            this._renderer.setClearColor(config.clearColor);
        }
        else {
            this._renderer.setClearColor(0x000000);
        }
        // 背景
        switch (config.backgroundType) {
            case 'color': {
                if (config.backgroundColor) {
                    this._scene.background = new THREE.Color(config.backgroundColor);
                }
                break;
            }
            case 'image': {
                if (config.backgroundUrl) {
                    let texture = new THREE.TextureLoader().load(config.backgroundUrl);
                    texture.minFilter = THREE.LinearFilter;
                    this._scene.background = texture;
                }
                break;
            }
            default: {
                break;
            }
        }
        // if (config.statsHelper) {
        //     // 创建页面帧数刷新控件
        //     this._stats = new Stats();
        //     if (this._container == undefined) return;
        //     this._container.appendChild(this._stats.domElement);
        // }
    }
    /**
     * 初始化管理器
     * @param config 
     */
    private initManager(config: SceneConfig) {
        // 相机和光源管理器
        this._cameraManager = new CameraManager(this, undefined);
       
        // 再交互控件管理器和普通控件管理器
        this._interactionManager = new InteractionManager(this, config);
        this._clipControlManager = new ClipControlManager(this, config);

        //构件着色
        this._modelColorManager = new ModelColorManager(this);
      
        //创建热力图对象
        if (config.heatMapFlag) {
            this._heatMapManager = new heatMapManager(this);
        }
        
        //创建光源管理器
        this._lightManager = new LightManager(this);
    }
   
    /**
     * 窗口尺寸变化事件
     */
    public onWindowResize() {
        if (this._renderer == undefined) return;
       
        this._enable = true;

        this._windowResize = true;

        if (this._tempConfig?.heatMapFlag) {
            if (this._tempContainer != undefined) {
                // 画布的大小重新调整
                this._renderer.setSize(this._tempContainer.clientWidth, this._tempContainer.clientHeight);
            } 
        } else {
            if (this._mainContainer != undefined) {
                // 画布的大小重新调整
                this._renderer.setSize(this._mainContainer.clientWidth, this._mainContainer.clientHeight);
            }
        }
       
        // 相机需要根据画布的容器大小重新调整
        if (this._cameraManager == undefined) return;
        this._cameraManager.onWindowResize();
    }

    /**
     * 当前场景的绘制入口
     */
    public render() {
        if (!this.enable) {
            return;
        }

        if (!this._tempConfig?.heatMapFlag) {
            if (!this._enable) {
                if (this._windowScale) {
                    this._enable = true;
                    this._windowScale = false;
                } else {
                    return
                }
            }
        }
    
        //设置跳帧
        if(this.skip !== 0) {
            this.skip = ++this.skip % this._tempSkip;
            return;
        } else {
            this.skip = ++this.skip % this._tempSkip;
        }
       
        if (this._cameraManager == undefined) return;
        let activeCamera = this._cameraManager.getActiveCamera();
        if (activeCamera == undefined) {
            return;
        }
        this._cameraManager.onWindowResize();

        // console.log(activeCamera.zoom)

        let flagContainer = document.getElementById(this._containerId);
        if (flagContainer == undefined) {
            EventControl.dispatchEvent('MSG_Ajax_Event_ABORT', null, 'all');
            this.dispose();
        }

        if (this._interactionManager != undefined) {
            this._interactionManager.update();
        };

        // 实时设置模型的分辨率
        if (this._meshGroup != undefined ) {
            if (this._meshGroup.children.length != 0) {
                let mainContainer = this.getMainContainer();
                if (this._meshGroup.children[0].userData.type == 'gcode') {
                    //@ts-ignore
                    this._meshGroup.children[0].material.resolution.set(mainContainer.clientWidth, mainContainer.clientHeight)
                }
            }
        };
       
        if (this._renderer == undefined) return;
        this._renderer.setRenderTarget( null );

        if (this._scene == undefined) return;
        this._renderer.render(this._scene, activeCamera);
        // console.log(activeCamera);

        //判断模型是否已经渲染出来
        if (this._renderer.info.memory.geometries > 0) {
            let scope = this;
            if (!this._tempConfig?.heatMapFlag) {
                if (scope._windowResize) {
                    this._time = setTimeout(function() {
                        scope._enable = false;
                        scope._windowResize = false;
                        clearTimeout(scope._time);
    
                    }, 100)
                } else {
                    scope._enable = false;
                }
            }
        }

        this._renderer.info.reset();
        // this._stats.update();
    }
    /**
     * 添加普通的THREE.Object3D对象
     * @param object 
     */
    public addObject(object: THREE.Object3D) {
        if (this._scene == undefined) return;
        this._scene.add(object);
    }

    /**
     * 移除普通的THREE.Object3D对象
     * @param object 
     */
    public removeObject(object: THREE.Object3D) {
        if (this._scene == undefined) return;
        this._scene.remove(object);
    }
  
    public setStlDataAddScene() {
        if (this._temGroup == undefined) return;
        this._temGroup.children = [];
    }

    //清除stl模型
    private unStlActivateData() {
        //清空stl的数据集
        if (this._stlDataCollection != undefined) {
            this._stlDataCollection.clear();
        }
    }

    /**
     * stl类型数据集
     * @param objectCollection 
     */
    public bindMaxDataCollection(objectCollection: StlDataCollection) {
        this._stlDataCollection = objectCollection;
        if (this._modelData.modelSize <= 10) {
            this._tempSkip = 4;
        } else if (10 < this._modelData.modelSize && this._modelData.modelSize <= 20) {
            this._tempSkip = 10;
        } else if (this._modelData.modelSize > 20) {
            this._tempSkip = 12;
        }

        let scope = this;
        objectCollection.onLoadMaxComplete = function() {
            let data = objectCollection.getStlData();
            if (data == undefined) return;
            if (scope._tempConfig?.pro3GridHelper) {
                if (data[0].userData.type == 'gcode') {
                    if (scope._meshGroup == undefined) return;
                    scope._meshGroup.translateZ(2900);
                }
            }
            if (data != undefined) {
                for (let i=0; i<data.length; ++i) {
                    if (scope._meshGroup == undefined) return;
                    scope._meshGroup.add(data[i]);
                    scope._enable = true;
                }
            }
        }
    }

    /**
     * 解绑stl数据集
     */
    public unbindStlDataCollection() {
        this.unStlActivateData();
        this._stlDataCollection = undefined;
    }

    /**
     * 获取当前所有场景的数据集
     * @returns Array<MeshData> | undefined
     */
    public getActiveAllData(): Array<MeshData> | undefined {
        if (this._stlDataCollection == undefined) {
            return undefined;
        }
        return this._stlDataCollection.getActiveAllData();
    }

    /**
    * 获取当前激活的stl模型的数据
    * @returns 返回数据类型any
    */
    public getStlActiveData(): any | undefined {
        if (this._stlDataCollection == undefined) {
            return undefined;
        }
        return this._stlDataCollection.getStlActiveData();
    }

    /**
     * 获取stl模型数据
     * @returns 返回数据为THREE.Group[] | undefined
     */
    public getStlDataModel(): THREE.Group[] | undefined {
        if (this._stlDataCollection == undefined) {
            return undefined;
        }
        return this._stlDataCollection.getStlData();
    }

    /**
     * 获取gcode模型的字典数据
     * @returns 返回数据类型any
     */
    public getIndexGcodeData(): any {
        if (this._stlDataCollection!= undefined) {
            let indexData = this._stlDataCollection.getGcodeIndexData();
            return indexData;
        }
    }

    /**
     * 设置热力图的视角
     */
    public setHeatMapFov() {
        if (this._interactionManager == undefined) return;
        this._interactionManager.heatMapFov();
    }
}
