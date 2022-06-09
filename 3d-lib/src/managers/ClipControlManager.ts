import * as THREE from "three";
import { Scene3D } from "../core/Scene3D";
import { EventControl } from "../event/EventControl";
import { SceneConfig } from "../configDefine/SceneConfig";

export class ClipControlManager{ 
    private _scene3D: Scene3D;
    private _temGroup: THREE.Group | undefined | null;
    private _config: SceneConfig;
    private _center: any; 
    public _lineBox: any;
    private _tempData: any = undefined;
    private _modelSize: any = {};

    constructor(scene3D: Scene3D, config: SceneConfig) {
        this._scene3D = scene3D;
        this._config = config;
        this._temGroup = scene3D._temGroup;
        //用来判断是否改变模型的
        EventControl.addEvent('MSG_INIT_CLIP', ()=> {this.initModelBox()}, '3D');
    }

    public initModelBox() {
        let activeData = this._scene3D.getStlActiveData();
        if (activeData == undefined) return;
        
        //计算模型的几何中心
        if(this._temGroup == undefined) return;

        //清除stl外框数据
        this.onClearOutBox();

        //创建stl打印外框的
        let box = new THREE.Box3().expandByObject(this._temGroup);
        if(this._scene3D._meshGroup == undefined) return;
        let centerLen = new THREE.Vector3().subVectors(box.max, box.min).multiplyScalar(0.5);
        this._center = new THREE.Vector3().subVectors(box.max, box.min);
       
        let yData = 1;
        let outGeometry = new THREE.BoxGeometry( 0.5, yData, 0.5 );
      
        let outMaterial = new THREE.MeshStandardMaterial( {
            roughness: 0.5,
            emissive: 0x000000,
            metalness: 0.5,
            transparent: true,
            opacity: 0.8,
        } );

        //stl模型注水效果外包框
        let box3Mesh = new THREE.Mesh(outGeometry, outMaterial);
        box3Mesh.visible = false;
        box3Mesh.name = 'outMesh';
        //45度注水
        if (this._config.pro3GridHelper){
            box3Mesh.translateZ(2900);
        }
       
        this._temGroup.add(box3Mesh);

        if (this._config.pro3GridHelper) {
            //创建模型的外部边框
            this.onCreateGridHelper({Xsize: 200, Ysize: 6000, Zsize: 200});
        }

        //创建云切片-变换功能外边框
        if (!this._config.boxHelper) return;

        //获取模型的初始尺寸
        this.getModelOriginalSize();

        //创建模型的外部边框
        this.onCalculationModelSize({Xsize: 300, Ysize: 300, Zsize: 400});
        
        //模型y轴坐标方向的值
        EventControl.dispatchEvent('MSG_MODEL_CLIP_VALAUE', Math.floor(centerLen.y) + 1, '3D');

        //初始化剖面的初始值
        EventControl.removeEvent('MSG_INIT_CLIP',()=> {this.initModelBox()}, '3D');
    }

    /**
     * 清除stl注水外框
     * @returns 
     */
    private onClearOutBox() {
        if (this._temGroup == undefined) return;
        let mesh1 = this._temGroup.getObjectByName("outMesh");
        if (mesh1 != undefined)  this._temGroup.remove(mesh1);

        let mesh2 = this._temGroup.getObjectByName("boxHelper");
        if (mesh2 != undefined) this._temGroup.remove(mesh2);

        let mGroup = this._temGroup.getObjectByName("print_stl");
        if (mGroup != undefined) {
            mGroup.traverse(function(child){
                if (child instanceof THREE.Mesh) {
                    child.geometry.dispose();
                    child.material.dispose();
                }
            });

            mGroup.clear();
            this._temGroup.remove(mGroup);
        }
    }

    /**
     * stl打印状态下的注水效果外部边框
     * @returns 
     */
    public onChangeOutGeometry() {
        if (this._temGroup == undefined) return;
        let box = undefined;
        box = new THREE.Box3().expandByObject(this._temGroup);
        if(this._scene3D._meshGroup == undefined) return;

        //45度的注水效果
        if (this._config.pro3GridHelper) 
        box = new THREE.Box3().expandByObject(this._scene3D._meshGroup);

        this._center = new THREE.Vector3().subVectors(box.max, box.min);
        let outGeometry= new THREE.BoxGeometry( this._center.x + 0.5, 1, this._center.z + 0.5 );
        
        //@ts-ignore
        let faces = outGeometry.faces;
        for (let i=0; i<faces.length; ++i) {
            if (i==4 || i==5) {
                faces[i].color.setHex( 0x00CFEE );
            } else {
                faces[i].color.setHex( 0x00CFEE );
            }
        }
        let outMaterial = new THREE.MeshStandardMaterial( {
            roughness: 0.5,
            emissive: 0x000000,
            metalness: 0.5,
            transparent: true,
            opacity: 0.4,
            //@ts-ignore
            vertexColors: THREE.FaceColors,
        } );

        let child = this._temGroup?.children;
        for (let i=0; i<child.length; ++i) {
            if (child[i].name == "outMesh") {
                if(child[i] instanceof THREE.Mesh) {
                    child[i].visible = false;
                    if (this._config.pro3GridHelper) {
                        child[i].position.y -= this._center.y/2;
                    }
                  
                    //@ts-ignore
                    child[i].geometry = outGeometry;
                    //@ts-ignore
                    child[i].geometry.verticesNeedUpdate = true;
                    //@ts-ignore
                    child[i].material = outMaterial;
                }
            }
        }

        //创建打印顶部动态面
        let geometry = new THREE.BoxGeometry( this._center.x + 0.5, 0.1, this._center.z + 0.5 );
        let material = new THREE.MeshStandardMaterial({
            color: 0x00CFEE, 
            side: THREE.DoubleSide, 
            emissive: 0x00CFEE, 
            metalness: 0.5,
            transparent: true,
            opacity: 0.5,
        });
      
        let bGroup = new THREE.Group();
        bGroup.name = "print_stl";
        bGroup.visible = false;
        if (this._config.pro3GridHelper) 
        bGroup.translateZ(2900);
        
        bGroup.position.y = -this._center.y/2;
        this._temGroup.add(bGroup);
        
        let plane = new THREE.Mesh(geometry, material);
        plane.frustumCulled = false;
        plane.renderOrder = 2;
        bGroup.add(plane);
     
        let edges = new THREE.EdgesGeometry( geometry );
        let line = new THREE.LineSegments( edges, new THREE.LineBasicMaterial( { color: 0xffffff } ) );
        bGroup.add(line);

        //注水模型外包框
        let box3 = new THREE.BoxHelper( this._temGroup, 0x00CFEE );
        box3.name = "boxHelper";
        box3.visible = false;
        this._temGroup.add( box3 );
    }

    /**
     * 根据机型改变外框尺寸
     * @returns 
     */
    public onChangeModelBoxSize() {
        if (this._scene3D._outBoxGroup == undefined) return;
        let children = this._scene3D._outBoxGroup.children;
        for (let i=0; i< children.length; ++i) {
            let child = children[i];
            //@ts-ignore
            child.geometry.dispose();
            //@ts-ignore
            child.material.dispose();
            this._scene3D._outBoxGroup.children.splice(0, children.length);
        }
    }

    /**
     * 创建45度视角的网格
     * @param boxSize 
     * @returns 
     */
    private onCreateGridHelper(boxSize: any) {
        if(this._scene3D._meshGroup == undefined) return;
        let Xsize = boxSize.Xsize;
        let Ysize = boxSize.Zsize; 
        let Zsize = boxSize.Ysize;

        //创建模型底部的网格线框
        if(this._scene3D._outBoxGroup == undefined) return;
        let size = Zsize, divisions = 300;
        let color1 = new THREE.Color( 0x607d8b );
        let color2 = new THREE.Color( 0x607d8b );

        let center = divisions / 2;
        let step = size / divisions;
        let vertices = [], colors: any=[];

        //X轴方向的线框数据
        for ( let i = 0, j = 0, k = -Zsize/2; i <= divisions; i ++, k += step ) {
            vertices.push( Xsize/2, 0, k, -Xsize/2, 0, k );
            let color = i === center ? color1 : color2;
            color.toArray( colors, j ); j += 3;
            color.toArray( colors, j ); j += 3;
            color.toArray( colors, j ); j += 3;
            color.toArray( colors, j ); j += 3;
        }

        let size1 = Xsize, divisions1 = 30;
        let step1 = size1 / divisions1;

        //Y轴方向的线框数据
        for (let i = 0, j = 0, k = -Xsize/2; i <= divisions1; i ++, k += step1) {
            vertices.push( k, 0, Zsize/2, k, 0, -Zsize/2 );
            let color = i === center ? color1 : color2;

            color.toArray( colors, j ); j += 3;
            color.toArray( colors, j ); j += 3;
            color.toArray( colors, j ); j += 3;
            color.toArray( colors, j ); j += 3;
        }

        //创建线框几何体
        let geometryd = new THREE.BufferGeometry();
        geometryd.setAttribute( 'position', new THREE.Float32BufferAttribute( vertices, 3 ) );
        geometryd.setAttribute( 'color', new THREE.Float32BufferAttribute( colors, 3 ) );

        let material = new THREE.LineBasicMaterial( { vertexColors: true, toneMapped: false, linewidth: 1 } );
        let gridHelper = new THREE.LineSegments( geometryd, material );

        if (this._temGroup == undefined) return;
        let box = new THREE.Box3().expandByObject(this._temGroup);
        let centerLen = new THREE.Vector3().subVectors(box.max, box.min).multiplyScalar(0.5);
        gridHelper.position.y -= centerLen.y;
        // this._scene3D._outBoxGroup.translateY(-50);
        this._scene3D._outBoxGroup.add( gridHelper );
    }

    /**
     * 创建模型的打印台（外部包围框）
     * @param boxSize 模型外框的长宽高的值
     * @returns 
     */
    private onCreateModelBox(boxSize: any) {
        if(this._scene3D._meshGroup == undefined) return;
        let Xsize = boxSize.Xsize;
        let Ysize = boxSize.Zsize; 
        let Zsize = boxSize.Ysize;

        //创建模型底部的网格线框
        if(this._scene3D._outBoxGroup == undefined) return;
        let size = Zsize, divisions = 10;
        let color1 = new THREE.Color( 0x607d8b );
        let color2 = new THREE.Color( 0x607d8b );

        let center = divisions / 2;
        let step = size / divisions;
        let vertices = [], colors: any=[];

        //X轴方向的线框数据
        for ( let i = 0, j = 0, k = -Zsize/2; i <= divisions; i ++, k += step ) {
            vertices.push( Xsize/2, 0, k, -Xsize/2, 0, k );
            let color = i === center ? color1 : color2;
            color.toArray( colors, j ); j += 3;
            color.toArray( colors, j ); j += 3;
            color.toArray( colors, j ); j += 3;
            color.toArray( colors, j ); j += 3;
        }

        let size1 = Xsize, divisions1 = 10;
        let step1 = size1 / divisions1;

        //Y轴方向的线框数据
        for (let i = 0, j = 0, k = -Xsize/2; i <= divisions1; i ++, k += step1) {
            vertices.push( k, 0, Zsize/2, k, 0, -Zsize/2 );
            let color = i === center ? color1 : color2;

            color.toArray( colors, j ); j += 3;
            color.toArray( colors, j ); j += 3;
            color.toArray( colors, j ); j += 3;
            color.toArray( colors, j ); j += 3;
        }

        //创建线框几何体
        let geometryd = new THREE.BufferGeometry();
        geometryd.setAttribute( 'position', new THREE.Float32BufferAttribute( vertices, 3 ) );
        geometryd.setAttribute( 'color', new THREE.Float32BufferAttribute( colors, 3 ) );

        let material = new THREE.LineBasicMaterial( { vertexColors: true, toneMapped: false, linewidth: 1 } );
        let gridHelper = new THREE.LineSegments( geometryd, material );
        gridHelper.renderOrder = 3;
        gridHelper.position.y = -Ysize/2;
        this._scene3D._outBoxGroup.add( gridHelper );

        //调整模型的位置
        EventControl.dispatchEvent('MSG_MODEL_POSITION_COUNT', 1, 'all');
    }

    /**
     * 计算模型初始尺寸
     * @param boxSize 模型尺寸数据
     * @returns 
     */
    public onCalculationModelSize(boxSize: any) {
        if (boxSize == undefined) return;
        
        //判断是否显示包围盒
        if (this._config.boxHelper) {
            //改变机型设置模型外框
            this.onChangeModelBoxSize();

            if(this._scene3D._outBoxGroup == undefined) return;
            let Xsize = boxSize.Xsize;
            let Ysize = boxSize.Zsize; 
            let Zsize = boxSize.Ysize;

            let colors = [];
            for (let i=0; i<24; ++i) {
                if (i>= 12 && i<=13) {
                    //X轴坐标颜色
                    colors.push(0.89, 0.03, 0.83);

                } else if (i>= 4 && i<=5) {
                    //Y轴坐标颜色
                    colors.push(0.05, 0.89, 0.91);

                } else if (i>= 8 && i<=9) {
                    //Z轴坐标颜色
                    colors.push(0, 0, 1);
                }else {
                    colors.push(.9, .9, .1);
                }
            }
            let color = new Float32Array(colors);

            let edgesGeometry = new THREE.BoxGeometry( Xsize, Ysize, Zsize );
            let edges = new THREE.EdgesGeometry(edgesGeometry);
            edges.setAttribute('color', new THREE.BufferAttribute(color, 3));
           
            this._lineBox = new THREE.LineSegments( edges, new THREE.LineBasicMaterial({ vertexColors: true, linewidth: 3}));
            this._lineBox.name = "lineBox";
            this._lineBox.visible = true;
            this._lineBox.castShadow = true;
            this._lineBox.renderOrder = 99;
            this._scene3D._outBoxGroup.add( this._lineBox );

            if(this._scene3D._meshGroup == undefined) return;
            let meshS = new THREE.Box3().setFromObject(this._scene3D._meshGroup);
            let meshSize = meshS.getSize(new THREE.Vector3());
            let meshSizeC = {'x': parseFloat(meshSize.x.toFixed(2)), 'y': parseFloat(meshSize.z.toFixed(2)), 'z': parseFloat(meshSize.y.toFixed(2))}

            //计算模型尺寸
            let rangeX = (1 / meshSize.x) * 100;
            let xMin = rangeX;
            
            let rangeY = (1 / meshSize.y) * 100;
            let yMin = rangeY;

            let rangeZ = (1 / meshSize.z) * 100;
            let zMin = rangeZ;

            let rMin = Math.max(xMin, yMin, zMin);
    
            //计算模型的缩放范围
            let Xrange = ( Xsize / meshSize.x ) * 100;
            let Yrange = ( Ysize / meshSize.y ) * 100;
            let Zrange = ( Zsize / meshSize.z ) * 100;

            let minRange = Math.min(Xrange, Yrange, Zrange);
            let rangeNum = parseFloat(minRange.toFixed(2));

            let xRangeNum = parseFloat(Xrange.toFixed(2));
            let yRangeNum = parseFloat(Yrange.toFixed(2));
            let zRangeNum = parseFloat(Zrange.toFixed(2));

            let tempData = {size: meshSizeC, scaleRange: {min: parseFloat(rMin.toFixed(2)) , max: parseFloat(rangeNum.toFixed(2)) }, 
            boxSize: {x: Xsize, y: Ysize, z: Zsize}, scaleRangeX: {min: parseFloat(xMin.toFixed(2)), max: parseFloat(xRangeNum.toFixed(2)) }, 
            scaleRangeY: {min: parseFloat(zMin.toFixed(2)), max: parseFloat(zRangeNum.toFixed(2)) },
            scaleRangeZ: {min: parseFloat(yMin.toFixed(2)) , max: parseFloat(yRangeNum.toFixed(2)) }, modeSize: this._modelSize};

            this._tempData = tempData;

            //云切片功能中返回模型尺寸事件
            EventControl.dispatchEvent('MSG_GET_MODEL_SIZE', this._tempData, '3D');

            if(this._scene3D._meshGroup == undefined) return;
            if (this._scene3D._meshGroup.children.length == 0) return; 
    
            let box = new THREE.Box3().expandByObject(this._scene3D._meshGroup.children[0]);
            let centerLen = new THREE.Vector3().subVectors(box.max, box.min).multiplyScalar(0.5);
    
            //设置模型组的位置坐标
            this._scene3D._meshGroup.position.y = -(Ysize/2 - centerLen.y);
    
            //根据不同的长宽高创建模型的打印台
            this.onCreateModelBox(boxSize);
        }
    }

     /**
     * 获取模型的初始尺寸
     * @returns null
     */
    private getModelOriginalSize() {
        if(this._scene3D._meshGroup == undefined) return;
        let meshS = new THREE.Box3().setFromObject(this._scene3D._meshGroup);
        let meshSize = meshS.getSize(new THREE.Vector3());
        this._modelSize = {'x': parseFloat(meshSize.x.toFixed(2)), 'y': parseFloat(meshSize.z.toFixed(2)), 'z': parseFloat(meshSize.y.toFixed(2))}
    }

    /**
     * 获取模型初始状态的尺寸
     */
    public getTransformModelSize() {
        return this._tempData;
    }

    /**
     * 获取模型变换后的缩放比例
     * @param scalingMultiple 缩放比例值
     * @returns 模型的缩放比例范围
     */
    public getModelSize(scalingMultiple: number) {
        if (this._scene3D._meshGroup == undefined) return;
        let tempData = {};
        //计算模型的包围盒
        let modelS = new THREE.Box3().setFromObject(this._scene3D._meshGroup);
        let mSize = modelS.getSize(new THREE.Vector3());
        // let modelSize = {'x': parseFloat(mSize.x.toFixed(2)), 'y': parseFloat(mSize.z.toFixed(2)), 'z': parseFloat(mSize.y.toFixed(2))}
        let sizeData = this.getTransformModelSize();

        //计算模型尺寸
        let rangeX = (1 / mSize.x) * 100;
        let xMin = rangeX;
        
        let rangeY = (1 / mSize.y) * 100;
        let yMin = rangeY;

        let rangeZ = (1 / mSize.z) * 100;
        let zMin = rangeZ;

        let rMin = Math.max(xMin, yMin, zMin);
        rMin = rMin * scalingMultiple;

        //计算模型的缩放范围
        let Xrange = ( sizeData.boxSize.x / mSize.x ) * 100;
        let Yrange = ( sizeData.boxSize.y / mSize.y ) * 100;
        let Zrange = ( sizeData.boxSize.z / mSize.z ) * 100;

        let minRange = Math.min(Xrange, Yrange, Zrange);
        minRange = minRange * scalingMultiple;
        let rangeNum = parseFloat(minRange.toFixed(2));

        tempData = {size: sizeData.size, scaleRange: {min: parseFloat(rMin.toFixed(2)) , max: parseFloat(rangeNum.toFixed(2))}, scaleRangeX: {min: sizeData.scaleRangeX.min, max: sizeData.scaleRangeX.max}, 
        scaleRangeY: {min: sizeData.scaleRangeY.min, max: sizeData.scaleRangeY.max},scaleRangeZ: {min: sizeData.scaleRangeZ.min, max: sizeData.scaleRangeZ.max}};
            
        return tempData;
    }

    /**
     * 重置模型大小和缩放比例
     * @param data 变换后的模型大小和缩放比例
     * @returns 
     */
    public setUpdateModelSize(data: any) {
        if (data == undefined) return;

        this._tempData.scaleRange.min = data.scaleRange.min;
        this._tempData.scaleRange.max = data.scaleRange.max;
        this._tempData.size = data.size;

        data = null;
    }

    /**
     * 旋转后改变模型的缩放比例
     * @param data 
     * @returns 
     */
    public setUpdateModelRoateSize(data: any) {
        if (data == undefined) return;
        this._tempData.scaleRange.min = data.scaleRange.min;
        this._tempData.scaleRange.max = data.scaleRange.max;
        this._tempData.size = data.size;

        data = null;
    }
}