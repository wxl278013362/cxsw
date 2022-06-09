import * as THREE from "three";
//设置THRE为全局变量
(window as any).THREE = THREE;
import { MeshData } from './MeshData';
import { EventControl } from "../event/EventControl";
//LineMaterial和LineGeometry两个js文件如果是install过后必须把原来的js文件拷贝到相应的路径，
//因为这两个文件在开发过程中已经做修改与官方提供的有差异
import { LineMaterial } from '../../node_modules/three/examples/jsm/lines/LineMaterial';
import { LineGeometry } from '../../node_modules/three/examples/jsm/lines/LineGeometry';
import "three-examples/loaders/GCodeLoader";
const fileModule = require('./../../toolbar/thirdjs/HTTP-range_lib_non-concurrent.js');

export class gcodeDatas extends MeshData {
    private _objClass: any;
    private _ajax: any;
    private _loader: any;
    private _lineWidth = 2;
    private _tConfig: any;
    private _config: any;
    private _chunkSize: any;

    private _temScene3D: any

    constructor(id: string, parentId: string | undefined, name: string, isActive: boolean, config: any, tConfig: any, temScene3D: any) {
        super(id, parentId, name, 'gcode', isActive);
        this._objectType = 'GData';
        this._tConfig = tConfig;
        this._config = config;
        this._chunkSize = 500000;
        this._temScene3D = temScene3D;

        EventControl.addEvent('MSG_Ajax_Event_ABORT', ()=> { this.dispose() }, '3D');
        EventControl.addEvent('MSG_ON_MODEL_LOAD_STOP', ()=> { this.onLoadStop() }, '3D');
    }

    /**
     * 终止分片加载
     */
    private onLoadStop() {
        fileModule.stop();
    }

    public dispose() {
        if (this._ajax != undefined) {
            if (this._ajax.currentTarget != null) {
                this._ajax.currentTarget.abort();
            } else {
                this._ajax.target.abort();
            }
            
            this._ajax = null;
        }
        EventControl.dispatchEvent('MSG_MODEL_URL_ERROR', 'gcode', '3D');
        EventControl.removeEvent('MSG_LOAD_3DS_DATA_PROGRESS', ()=> {}, 'UI');
    }

    //通过路径获取外部文件
    public onLoaderGcode(modelUrl: string, objClass: any) {
        let scope = this;
        scope._objClass = objClass;
        //@ts-ignore
        scope._loader = new THREE.GCodeLoader();

        if (this._config.chunkSize != undefined) this._chunkSize = this._config.chunkSize;

        if (this._tConfig.particlesLoaded) {
           
            //分片加载
            fileModule.fileFragmentDownload(modelUrl, this._chunkSize, this._config.dataType, () => {} ).then((fileConten: any) =>{
                if (fileConten == "") return;
               
                let modelSize = fileConten.totalSize/1024/1024;

                // this._temScene3D._worker = new Worker('/worker.js');
                this._temScene3D._worker = new Worker('./../../toolbar/thirdjs/worker.js');
                this._temScene3D._worker.onmessage = function(e: any){
                    const message = e.data;
                    let obj = message.dataObj;
                    let data = message.data;
                    scope._objClass._objectCollection._indexGcodeData = data;
                    //根据数据绘制模型
                    scope.onRenderGcode(obj);
                    message.dataObj = null;
                    message.data = null;
                };

                let downloadText = fileConten.downloadText;
                let type = 'particlesLoaded';
                this._temScene3D._worker.postMessage({downloadText, modelSize, type});

            }).catch((err: any) => {
                console.log("分片加载异常");
                //终止请求
                // this.onLoadStop();
                EventControl.dispatchEvent('MSG_MODEL_URL_ERROR', 'gcode', '3D');
               
            });

        } else {
            //非分片加载源码
            let onProgress = function (xhr: any) {
                scope._ajax = xhr;
            };
            let onError = function () {
                //触发事件
                EventControl.dispatchEvent('MSG_MODEL_URL_ERROR', 'gcode', '3D');
                //删除事件
                EventControl.removeEvent('MSG_MODEL_URL_ERROR', () => { }, '3D');
            };

            if (this._config.portType == 'qt') {
                if (this._config.data != "" || this._config.data != undefined) {
                    //新数据格式
                    //自己测试用的
                    // scope.onRenderGcode(this._config.data.data);
                    //提交用的
                    scope.onRenderGcode(this._config.data);
                    scope._objClass._objectCollection._indexGcodeData = this._config.data.indexGcodeData;
        
                    this._config.data = null;
                } else {
                    EventControl.dispatchEvent('MSG_MODEL_URL_ERROR', 'gcode', '3D');
                    //删除事件
                    EventControl.removeEvent('MSG_MODEL_URL_ERROR', () => { }, '3D');
                }

            } else {
                let modelSize = this._config.modelSize;
                // this._temScene3D._worker = new Worker('/worker.js');
                this._temScene3D._worker = new Worker('./../../toolbar/thirdjs/worker.js');
                this._temScene3D._worker.onmessage = function(e: any){
                    const message = e.data;
                    let obj = message.dataObj;
                    let data = message.data;
                    scope._objClass._objectCollection._indexGcodeData = data;
                    //根据数据绘制模型
                    scope.onRenderGcode(obj);
                    message.dataObj = null;
                    message.data = null;
                };

                let type = 'noParticlesLoaded';
                this._temScene3D._worker.postMessage({modelUrl, modelSize, type});
            }
        }
    }

    /**
     * 创建模型几何数据，构建模型对象
     * @param geomData 模型稽几何数据
     * @returns 
     */
    public onRenderGcode(geomData: any) {
        if (geomData == undefined) return;
        let vertexData = geomData.vert;
        let colorData = geomData.color;
        let alphData = geomData.alph;

        this._objClass._objectCollection._data = vertexData.length/3;
        
        //创建线几何对象
        let geometry = new LineGeometry();
        //设置几何数据属性
        geometry.setPositions(vertexData);
        geometry.setColors(colorData);
        geometry.setAlphs(alphData);

         //创建模型的材质信息
        let shaderMaterial = new LineMaterial({
            linewidth: 2, // in pixels
            vertexColors: true,
            resolution: new THREE.Vector2(1024, 1024),
            transparent: true,
        });
        
         // 创建gcode模型的对象
        let mesh = new THREE.Line(geometry, shaderMaterial);
        let box1 = new THREE.Box3();
        box1.expandByObject(mesh);
        let mmaxLen = box1.max.distanceTo(box1.min);
        if (mmaxLen <= 10) {
            mesh.scale.set(100, 100, 100);
        }

        //对模型进行矩阵变化
        let matrix = new THREE.Matrix4();
        if (!this._tConfig.pro3GridHelper) {
            matrix.makeRotationAxis(new THREE.Vector3(1, 0, 0), -Math.PI/2);
            matrix.multiply(mesh.matrix);
            mesh.matrix = matrix;
            mesh.rotation.setFromRotationMatrix(mesh.matrix);
        }
       
        mesh.userData = { type: 'gcode' };
        mesh.renderOrder = 1;
        let box = new THREE.Box3();
        box.expandByObject(mesh);
        let center = box.getCenter(new THREE.Vector3());
        let centerLen = new THREE.Vector3().subVectors(box.max, box.min).multiplyScalar(0.5);
    
        mesh.position.set(0 - center.x, 0 - center.y, 0 - center.z);

        if (this._tConfig.pro3GridHelper) {
            mesh.position.x += centerLen.x/1.5;
        }

        //当设备是qt时设置一个定时器作用是能够确保模型正常显示
        if (this._config.portType == 'qt') {
            let scope = this;
            let time = setTimeout(function() {
                //添加模型进入场景
                scope._objClass._objectCollection.addStlData(mesh);
                //触发模型加载完成事件
                scope._objClass._objectCollection.finishLoadTask();
                geomData = null;
                vertexData = null;
                colorData = null;
                alphData = null;
                clearTimeout (time);
            },100);

        } else {
            //添加模型进入场景
            this._objClass._objectCollection.addStlData(mesh);

            //触发模型加载完成事件
            this._objClass._objectCollection.finishLoadTask();

            geomData = null;
            vertexData = null;
            colorData = null;
            alphData = null;
        }
    }
}