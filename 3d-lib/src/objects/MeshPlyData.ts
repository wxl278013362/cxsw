import * as THREE from "three";
//设置THRE为全局变量
(window as any).THREE = THREE;
import { MeshData } from './MeshData';
import { EventControl } from "../event/EventControl";
import "three-examples/loaders/PLYLoader";
const fileModule = require('./../../toolbar/thirdjs/HTTP-range_lib_non-concurrent.js');

export class MeshPlyData extends MeshData{
    private _ajax: any;
    private _config: any;
    private _tConfig: any;
    private _chunkSize: any;

    constructor(id: string, parentId: string | undefined, name: string, isActive: boolean, config: any, tConfig: any) {
        super(id, parentId, name, 'ply', isActive);
        this._objectType = "MeshPlyData";
        this._config = config;
        this._tConfig = tConfig;

        this._chunkSize = 500000;

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

    public onLoaderPly(modelUrl: string, objClass: any) {
        let scope = this;

        if (this._config.chunkSize != undefined) this._chunkSize = this._config.chunkSize;
        //@ts-ignore
        let loader = new THREE.PLYLoader();
        
        let shaderMaterial = new THREE.MeshPhongMaterial( {
            color: 0x444242,
            shininess: 15,
            emissive: 0x000000,
            specular: 0xb4b6bc,
        });

        let onProgress = function(xhr:any ) {
            if (scope._ajax == undefined) {
                scope._ajax = xhr;
            }
        };

        let onError = function(e: Error) {
            console.log(e);
            //触发事件
            EventControl.dispatchEvent('MSG_MODEL_URL_ERROR', "ply", '3D');
            // 删除事件
            EventControl.removeEvent('MSG_MODEL_URL_ERROR', ()=>{}, 'UI');
        };

        loader.load(modelUrl, (geometry: any) => {

            geometry.computeVertexNormals();

            let mesh = new THREE.Mesh( geometry, shaderMaterial );
            let box1 = new THREE.Box3();
            box1.expandByObject(mesh);

            let mmaxLen = box1.max.distanceTo(box1.min);
            if (mmaxLen <=1) {
                mesh.scale.set(1000,1000,1000);
            }

            let radius = geometry.boundingSphere.radius;
            if (isNaN(radius)) {
                //触发事件
                EventControl.dispatchEvent('MSG_MODEL_URL_ERROR', "ply", '3D');
                //删除事件
                EventControl.removeEvent('MSG_MODEL_URL_ERROR', ()=>{}, 'UI');
                return;
            }

            //注意模型反转后y轴与z轴要注意
            mesh.rotation.set( -Math.PI / 2, 0, 0 );
            mesh.name = "modelStl";
            mesh.userData = {type: 'ply', id: '001'};
            mesh.renderOrder = 6;
            mesh.receiveShadow = true;
            mesh.castShadow = true;

            let box = new THREE.Box3();
            box.expandByObject(mesh);
            let center = box.getCenter(new THREE.Vector3());
            
            mesh.position.set(0-center.x, 0-center.y, 0-center.z);
            if (scope._tConfig.pro3GridHelper) {
                mesh.translateY(-2900);
            }
        
            //添加模型进入场景
            objClass._objectCollection.addStlData(mesh);
            
            //触发模型加载完成事件
            objClass._objectCollection.finishLoadTask();

        }, onProgress, onError );
    }
}