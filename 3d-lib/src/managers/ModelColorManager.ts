
import * as THREE from "three";
import { Scene3D } from "../core/Scene3D";
import { EventControl } from "../event/EventControl";

/**
 * 构件着色管理器
 */
export class ModelColorManager {
    /**
     * 绑定的场景
     */
    private _scene3D: Scene3D;
    /**
     * 构件数据
     */
    private _printerColorData: {[key:string] : any} = {};

    constructor(scene3D: Scene3D) {
        this._scene3D = scene3D;
        EventControl.addEvent('MSG_RENDER_MODEL_LAYER_PRINTER', (data: any)=> {this.setIndexModelAlph(data)}, '3D');
    }

    //修改整个模型的颜色
    public setStlModelColor(colorData: any){
        let modelData = this._scene3D.getStlDataModel();
        if (modelData != undefined) {
            for (let i=0; i<modelData.length; ++i) {
                let type = modelData[i].userData.type;
                if (type === 'stl') {
                    if (colorData == undefined) return;
                    let r = parseFloat((colorData[0]/255).toFixed(1));
                    let g = parseFloat((colorData[1]/255).toFixed(1));
                    let b = parseFloat((colorData[2]/255).toFixed(1));
                    //@ts-ignore
                    modelData[i].material.color = new THREE.Color(r, g, b);
                } 
            }
        }
    }

    /**
     * 设置模型的透明值
     * @param alph 需要设置的透明值
     */
    public setModelAlph(alph: any, key: any) {
        if (this._scene3D._meshGroup == undefined) return;
        let box = new THREE.Box3();
        box.expandByObject(this._scene3D._meshGroup);
        let mmaxLen = box.max.distanceTo(box.min);
        if (mmaxLen <= 30) {
            alph = 0.2
        }
        //设置模型的深度渲染
        if (this._scene3D._meshGroup == undefined) return;
        let children = this._scene3D._meshGroup.children;
        if (children.length != 0) {
            if (children[0].userData.type == 'gcode') {
                if (alph < 1.0) {
                    //@ts-ignore
                    children[0].material.depthWrite = false;
                } else {
                    //@ts-ignore
                    children[0].material.depthWrite = true;
                }
            }
        }

        //设置模型的透明度和打印层的高亮
        let modelData = this._scene3D.getStlDataModel();
        if (key == 'finished') return;
        
        if (modelData != undefined) {
            for (let i=0; i<modelData.length; ++i) {
                if (alph != undefined) {
                    // @ts-ignore
                    let transparentAttribute = modelData[i].geometry.getAttribute('instanceAlphStart');
                    let count = transparentAttribute.data.count;
                    for (let j = 0; j < count; ++j) {
                        transparentAttribute.data.array[j] = alph;
                    }
                    //@ts-ignore
                    transparentAttribute.needsUpdate = true;
                }
            }

            if (key == undefined) return;
            let keyData = parseFloat(key);
            let indexData = this._scene3D.getIndexGcodeData();
            if (indexData == undefined) return;

            //判断传递的数据是否属于层级
            if (keyData == 0) {
                for (let dkey in indexData) {
                    let tkey = parseFloat(dkey);
                    if (tkey== keyData) {
                        break;
                    } else {
                        return;
                    }
                }
            } 
            
            let ttData = undefined;
            //找不到对应的层数可以向上取值
            if (indexData[keyData] == undefined) {
               for (let key in indexData) {
                   if (parseFloat(key) >= keyData) {
                      ttData = key;
                      break;
                   }
               }
            } else {
                ttData = keyData;
            }
            
            if (ttData == undefined) return;

            //设置模型的透明值
            let cStart = indexData[ttData].start;
            let cEend = indexData[ttData].end;

            //计算透明值的开始索引和结束索引
            let startAlphData = (cStart * 3 / 6);
            let endAlphData = (cEend * 3 / 6);

            //@ts-ignore
            let transparentAttribute = modelData[0].geometry.getAttribute('instanceAlphStart');
            for (let n = 0; n < endAlphData; ++n) {
                transparentAttribute.data.array[n] = 1.0;
            }
            // @ts-ignore
            transparentAttribute.needsUpdate = true;

            //设置当前打印的层级高亮
            // @ts-ignore
            let colorsAttribute = modelData[0].geometry.getAttribute('instanceColorStart');
            //@ts-ignore
            let colorsAttributeEnd = modelData[0].geometry.getAttribute('instanceColorEnd');

            //设置模型颜色高亮
            for (let k = cStart; k < cEend; ++k) {
                 //修改前端的颜色值
                 colorsAttribute.data.array[3*k] = 0;
                 colorsAttribute.data.array[3*k + 1] = .87;
                 colorsAttribute.data.array[3*k + 2] = 1;

                 //修改末端的颜色值
                 colorsAttributeEnd.data.array[3*k] = 0;
                 colorsAttributeEnd.data.array[3*k + 1] = .87;
                 colorsAttributeEnd.data.array[3*k + 2] = 1;
            }
            
            //@ts-ignore
            colorsAttribute.needsUpdate = true;
            //@ts-ignore
            colorsAttributeEnd.needsUpdate = true;

            //保存注水的模型索引
            this._printerColorData[key]={
                "start": cStart,
                "end": cEend
            }
        }
    }

    /**
     * 设置模型打印的层级高亮效果
     * @param zData 层级数据
     * @returns 
     */
    public setIndexModelAlph(zData: any) {
        if (zData == undefined || zData == "") return;
        let key = parseFloat(zData);
        let modelData = this._scene3D.getStlDataModel();
        if (modelData != undefined) {
            if (modelData[0] == undefined) return;
            //获取模型的类型
            let type = modelData[0].userData.type;
            if (type == 'stl') {
                if (this._scene3D._temGroup == undefined) return;
                let box = new THREE.Box3().expandByObject(this._scene3D._temGroup);
                let center = new THREE.Vector3().subVectors(box.max, box.min);
                let len = this._scene3D._temGroup.children.length;
                
                for (let i=0; i<len; ++i) {
                    let child = this._scene3D._temGroup.children[i];
                    if (child.name == 'print_stl') {
                        child.visible = true;
                        child.position.y = (-center.y/2 + 0.1 + Math.abs(key));
                    }
                    if (child.name == 'outMesh') {
                        child.visible = true;
                        
                        //@ts-ignore
                        let vertices = child.geometry.vertices;
                        if (zData == "finished") {
                             EventControl.removeEvent('MSG_RENDER_MODEL_LAYER_PRINTER', ()=> {}, '3D');
                             let outMaterial = new THREE.MeshStandardMaterial( {
                                 color: 0x00CFEE,
                                 roughness: 0.5,
                                 emissive: 0x000000,
                                 metalness: 0.5,
                                 transparent: true,
                                 opacity: 0.8,
                             } );
                             //@ts-ignore
                             child.material = outMaterial;
                             return;
                        }

                        for (let j=0; j<vertices.length; ++j) {
                            if (vertices[j].y < 0) {
                                vertices[j].y = -Math.abs(key)/2
                            } else {
                                vertices[j].y = Math.abs(key)/2;
                            }
                        }
                        // @ts-ignore
                        child.geometry.verticesNeedUpdate = true;
                        child.position.y = (-center.y/2 + 0.1 + Math.abs(key)/2);
                    }
                }
            } else if (type == 'gcode') {
                if (zData == "finished") {
                    this.clearPrinterLayerColor();
                    EventControl.removeEvent('MSG_RENDER_MODEL_LAYER_PRINTER', ()=> {}, '3D');

                    if (this._scene3D._meshGroup == undefined) return;
                    let children = this._scene3D._meshGroup.children;
                    if (children.length != 0) {
                        //@ts-ignore
                        children[0].material.depthWrite = true;
                    }
                    return;
                }

                let indexData = this._scene3D.getIndexGcodeData();
                for (let i=0; i<modelData.length; ++i) {
                    if (indexData[key] == undefined) {
                        return;
                    };
                    //清除打印时的高亮颜色
                    this.clearPrinterLayerColor();

                    let cStart = indexData[key].start;
                    let cEend = indexData[key].end;
    
                    this._printerColorData[key]={
                        "start": cStart,
                        "end": cEend
                    }

                    //计算透明值的开始索引和结束索引
                    let startAlphData = (cStart * 3 / 6);
                    let endAlphData = (cEend * 3 / 6);

                    //@ts-ignore
                    let transparentAttribute = modelData[i].geometry.getAttribute('instanceAlphStart');
                    for (let i = startAlphData; i < endAlphData; ++i) {
                        transparentAttribute.data.array[i] = 1.0;
                    }
                    // @ts-ignore
                    transparentAttribute.needsUpdate = true;

                    // @ts-ignore
                    let colorsAttribute = modelData[i].geometry.getAttribute('instanceColorStart');
                    //@ts-ignore
                    let colorsAttributeEnd = modelData[i].geometry.getAttribute('instanceColorEnd');
    
                    for (let j = cStart; j < cEend; ++j) {
                        //修改前端的颜色值
                        colorsAttribute.data.array[3*j] = 0;
                        colorsAttribute.data.array[3*j + 1] = .87;
                        colorsAttribute.data.array[3*j + 2] = 1;

                        //修改末端的颜色值
                        colorsAttributeEnd.data.array[3*j] = 0;
                        colorsAttributeEnd.data.array[3*j + 1] = .87;
                        colorsAttributeEnd.data.array[3*j + 2] = 1;
                    }
                    //@ts-ignore
                    colorsAttribute.needsUpdate = true;
                    //@ts-ignore
                    colorsAttributeEnd.needsUpdate = true;
                }
            }
        }
    }

    /**
     * 清空实时打印的高亮面
     * @returns 
     */
    public clearPrinterLayerColor() {
        let modelData = this._scene3D.getStlDataModel();
        if (modelData != undefined) {
            if (this._printerColorData == undefined) return;
            for (let i=0; i<modelData.length; ++i){
                for (let key in this._printerColorData) {
                    // let start = this._printerColorData[key].start;
                    let end = this._printerColorData[key].end;
                    let alphData = (end * 3 / 6);
                    // @ts-ignore
                    let transparentAttribute = modelData[i].geometry.getAttribute('instanceAlphStart');
                    for (let n = 0; n < alphData; ++n) {
                        transparentAttribute.data.array[n] = 1.0;
                    }
                    transparentAttribute.needsUpdate = true;

                    //@ts-ignore
                    let colorAttribute = modelData[i].geometry.getAttribute('instanceColorStart');
                    //@ts-ignore
                    let colorsAttributeEnd = modelData[i].geometry.getAttribute('instanceColorEnd');
                    for (let j = 0; j < end; ++j) {
                        //前段的颜色值设置
                        colorAttribute.data.array[3*j] = .48;
                        colorAttribute.data.array[3*j + 1] = .48;
                        colorAttribute.data.array[3*j + 2] = .48;

                        //后段的颜色值的设置
                        colorsAttributeEnd.data.array[3*j] = .84;
                        colorsAttributeEnd.data.array[3*j + 1] = .84;
                        colorsAttributeEnd.data.array[3*j + 2] = .84;
                    }
                    //@ts-ignore
                    colorAttribute.needsUpdate = true;
                    //@ts-ignore
                    colorsAttributeEnd.needsUpdate = true;
                }
            }
            //清空字典的数据
            this._printerColorData = {};
        }
    }
}