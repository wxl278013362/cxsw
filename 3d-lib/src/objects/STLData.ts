import * as THREE from "three";
//设置THRE为全局变量
(window as any).THREE = THREE;
import { MeshData } from './MeshData';
import { EventControl } from "../event/EventControl";
import "three-examples/loaders/STLLoader";
const fileModule = require('./../../toolbar/thirdjs/HTTP-range_lib_non-concurrent.js');
import { Dataparsing } from "../objects/Dataparsing";

export class STLData extends MeshData{
    private _ajax: any;
    private _config: any;
    private _tConfig: any;
    private _chunkSize: any;

    private _temScene3D: any;

    constructor(id: string, parentId: string | undefined, name: string, isActive: boolean, config: any, tConfig: any, temScene3D: any) {
        super(id, parentId, name, 'stl', isActive);
        this._objectType = "STLData";
        this._config = config;
        this._tConfig = tConfig;

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
        //注销ajax请求
        if (this._ajax != undefined) {
            if (this._ajax.currentTarget != null) {
                this._ajax.currentTarget.abort();
            } else {
                this._ajax.target.abort();
            }
            
            this._ajax = null;
        }
        EventControl.dispatchEvent('MSG_MODEL_URL_ERROR', "stl", '3D');
        EventControl.removeEvent('MSG_LOAD_3DS_DATA_PROGRESS', ()=> {}, 'UI');
    }
 
    public onLoaderStl(modelUrl: string, objClass: any) {
        let scope = this;

        if (this._tConfig.particlesLoaded) {
            //分片加载
            //@ts-ignore
            let loader = new THREE.STLLoader();
            let shaderMaterial = new THREE.MeshPhongMaterial( {
                color: 0x444242,
                shininess: 15,
                emissive: 0x000000,
                specular: 0xb4b6bc,
            });

            if (this._config.chunkSize != undefined) this._chunkSize = this._config.chunkSize;
            
            fileModule.fileFragmentDownload(modelUrl, this._chunkSize, this._config.dataType, () => {} ).then((fileConten: any) =>{
                if (fileConten == "" ) return;
              
                let modelSize = this._config.modelSize;
                // this._temScene3D._worker = new Worker('/stlWorker.js');
                this._temScene3D._worker = new Worker('./../../toolbar/thirdjs/stlWorker.js');
                this._temScene3D._worker.onmessage = function(e: any){
                   scope.onCreateMesh(e.data, objClass)
                };

                let type = 'particlesLoaded';
                let downloadText = fileConten.downloadText;
                this._temScene3D._worker.postMessage({downloadText, modelSize, type});
                
            }).catch((err: any) => {
                console.log("分片加载异常");
                //终止请求
                // this.onLoadStop();
                //触发事件
                EventControl.dispatchEvent('MSG_MODEL_URL_ERROR', "stl", '3D');
            });

        } else {
            //非分片加载源码
            let onProgress = function(xhr:any ) {
                if (scope._ajax == undefined) {
                    scope._ajax = xhr;
                }
            };

            let onError = function(e: Error) {
                console.log(e);
                //触发事件
                EventControl.dispatchEvent('MSG_MODEL_URL_ERROR', "stl", '3D');
                // 删除事件
                EventControl.removeEvent('MSG_MODEL_URL_ERROR', ()=>{}, 'UI');
            };

            //进入qt端
            if (this._config.portType == 'qt') {
                if (this._config.data != "" || this._config.data != undefined) {
                    // 创建模型解析对象
                    let dataparsing = new Dataparsing(objClass);
                    //传路径加载模型数据
                    dataparsing.onParseStl(scope._config.data, scope._tConfig);
                }
            } else {
                let modelSize = this._config.modelSize;
                // this._temScene3D._worker = new Worker('/stlWorker.js');
                this._temScene3D._worker = new Worker('./../../toolbar/thirdjs/stlWorker.js');
                this._temScene3D._worker.onmessage = function(e: any){
                   console.log(e.data);
                   scope.onCreateMesh(e.data, objClass)
                };

                let type = 'noParticlesLoaded';
                this._temScene3D._worker.postMessage({modelUrl, modelSize, type});
            }
        }
    }

    /**
     * 创建模型网格
     * @param geometrys 
     * @param objClass 
     * @returns 
     */
    private onCreateMesh(geometrys: any, objClass: any) {
        let geometry = new THREE.BufferGeometry();
        geometry.setAttribute('position', new THREE.BufferAttribute(geometrys.vertices, 3));
        geometry.setAttribute('normal', new THREE.BufferAttribute(geometrys.normals, 3));

        geometrys.vertices = null;
        geometrys.normals = null;

        geometrys = null;
    
        let mat = new THREE.Matrix4();
        mat.makeScale(0.5, 0.5, 0.5);

        geometry.applyMatrix4(mat);
       
        //创建模型材质
        let shaderMaterial = new THREE.MeshPhongMaterial( {
            color: 0x444242,
            shininess: 15,
            emissive: 0x000000,
            specular: 0xb4b6bc,
        });

        geometry.computeBoundingSphere();
        geometry.computeVertexNormals();

        let mesh = new THREE.Mesh( geometry, shaderMaterial );
        let box1 = new THREE.Box3();
        box1.expandByObject(mesh);
        let mmaxLen = box1.max.distanceTo(box1.min);
        if (mmaxLen <=1) {
            mesh.scale.set(1000,1000,1000);
        }

        //@ts-ignore
        let radius = geometry.boundingSphere.radius;
        if (isNaN(radius)) {
            //触发事件
            EventControl.dispatchEvent('MSG_MODEL_URL_ERROR', "stl", '3D');
            //删除事件
            EventControl.removeEvent('MSG_MODEL_URL_ERROR', ()=>{}, 'UI');
            return;
        }

        //注意模型反转后y轴与z轴要注意
        mesh.rotation.set( -Math.PI / 2, 0, 0 );
        mesh.name = "modelStl";
        mesh.userData = {type: 'stl', id: '001'};
        mesh.renderOrder = 6;
        mesh.receiveShadow = true;
        mesh.castShadow = true;
        
        let box = new THREE.Box3();
        box.expandByObject(mesh);
        let center = box.getCenter(new THREE.Vector3());
        // this._center = new THREE.Vector3().subVectors(box.max, box.min);
        mesh.position.set(0-center.x, 0-center.y, 0-center.z);
        if (this._tConfig.pro3GridHelper) {
            mesh.translateY(-2900);
        }
        //获取模型的顶点
        // objClass._objectCollection._data = mesh.geometry.attributes.position.array.length;
        //添加模型进入场景

        objClass._objectCollection.addStlData(mesh);
        //触发模型加载完成事件
        objClass._objectCollection.finishLoadTask();
    }
}