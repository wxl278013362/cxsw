import * as THREE from "three";
import { Scene3D } from "../core/Scene3D";
import { EventControl } from "../event/EventControl";

export class heatMapManager {
    private _scene3D: Scene3D;
    private _temGroup: THREE.Group | undefined | null;
    private _pgeometry: any;
    private _tempCtx: any;
    private _shader: any;
    private _heatMap: any;
    public _tempLen: any;
    private _tempDiv: any;

    private _lengthX: number = 220;
    private _lengthY: number = 220;
    private _coordinates: any = {};

    private _mapData: any[];
    private _canvas: any; 
    private _tempContext: any;
    private _textArray: any = {};
    private _mouseDownPoint = new THREE.Vector2(0, 0);
    private _mouseUpPoint = new THREE.Vector2(0, 0);
    private _isMoveLength = 0.5;

    private _top: any;
    private _left: any;
    private _type: any;
    private _colorData: {[key:string] : any} = {};
    private _distance: any;
    private _tnewData: any = [];

    //设置热力图的色卡步长
    private _indexData: {[key:string] : any} = [
        ['2.0', {r: 233, g: 146, b: 103, total: 482, key: '2.0'}],
        {r: 243, g: 166, b: 63, total: 472, key: '1.6'},
        {r: 252, g: 186, b: 21, total: 459, key: '1.2'},
        {r: 217, g: 209, b: 7, total: 433, key: '0.8'},
        {r: 156, g: 233, b: 7, total: 396, key: '0.4'},
        {r: 100, g: 255, b: 13, total: 368, key: '0.0'},
        {r: 62, g: 255, b: 102, total: 419, key: '-0.4'},
        {r: 24, g: 255, b: 187, total: 466, key: '-0.8'},
        {r: 27, g: 242, b: 231, total: 500, key: '-1.2'},
        {r: 65, g: 217, b: 228, total: 510, key: '-1.6'},
        {r: 109, g: 195, b: 226, total: 530, key: '-2.0'},
    ];

    //根据不同的步长设置颜色值
    private colorData: any = [
        {r: 109, g: 195, b: 226, total: 530, key: '2.5'},

        {r: 233, g: 146, b: 103, total: 482, key: '2.0'},

        {r: 236, g: 151, b: 93, total: 482, key: '1.9'},
        {r: 238, g: 156, b: 83, total: 482, key: '1.8'},
        {r: 241, g: 161, b: 73, total: 482, key: '1.7'},

        {r: 243, g: 166, b: 63, total: 472, key: '1.6'},

        {r: 245, g: 171, b: 53, total: 472, key: '1.5'},
        {r: 248, g: 176, b: 42, total: 472, key: '1.4'},
        {r: 250, g: 181, b: 32, total: 472, key: '1.3'},

        {r: 252, g: 186, b: 21, total: 459, key: '1.2'},

        {r: 243, g: 192, b: 18, total: 459, key: '1.1'},
        {r: 235, g: 198, b: 14, total: 459, key: '1.0'},
        {r: 226, g: 203, b: 11, total: 459, key: '0.9'},

        {r: 217, g: 209, b: 7, total: 433, key: '0.8'},

        {r: 202, g: 215, b: 7, total: 396, key: '0.7'},
        {r: 187, g: 221, b: 7, total: 396, key: '0.6'},
        {r: 171, g: 227, b: 7, total: 396, key: '0.5'},

        {r: 156, g: 233, b: 7, total: 396, key: '0.4'},

        {r: 142, g: 240, b: 9, total: 396, key: '0.3'},
        {r: 128, g: 245, b: 11, total: 396, key: '0.2'},
        {r: 114, g: 250, b: 12, total: 396, key: '0.1'},

        {r: 100, g: 255, b: 13, total: 368, key: '0.0'},

        {r: 91, g: 255, b: 35, total: 419, key: '-0.1'},
        {r: 81, g: 255, b: 58, total: 419, key: '-0.2'},
        {r: 72, g: 255, b: 80, total: 419, key: '-0.3'},

        {r: 62, g: 255, b: 102, total: 419, key: '-0.4'},

        {r: 53, g: 255, b: 187, total: 466, key: '-0.5'},
        {r: 43, g: 255, b: 187, total: 466, key: '-0.6'},
        {r: 34, g: 255, b: 187, total: 466, key: '-0.7'},

        {r: 24, g: 255, b: 187, total: 466, key: '-0.8'},

        {r: 25, g: 252, b: 198, total: 466, key: '-0.9'},
        {r: 26, g: 249, b: 209, total: 466, key: '-1.0'},
        {r: 26, g: 245, b: 220, total: 466, key: '-1.1'},

        {r: 27, g: 242, b: 231, total: 500, key: '-1.2'},

        {r: 37, g: 236, b: 230, total: 500, key: '-1.3'},
        {r: 46, g: 230, b: 230, total: 500, key: '-1.4'},
        {r: 56, g: 223, b: 229, total: 500, key: '-1.5'},

        {r: 65, g: 217, b: 228, total: 510, key: '-1.6'},

        {r: 76, g: 212, b: 228, total: 510, key: '-1.7'},
        {r: 87, g: 206, b: 227, total: 510, key: '-1.8'},
        {r: 98, g: 201, b: 227, total: 510, key: '-1.9'},

        {r: 109, g: 195, b: 226, total: 530, key: '-2.0'},
        {r: 109, g: 195, b: 226, total: 530, key: '-2.5'},
    ];

    constructor(scene3D: Scene3D){
        this._scene3D = scene3D;
        this._temGroup = scene3D._temGroup;
        //注册事件
        EventControl.addEvent('MSG_CREATE_HEATMAP_MODEL', (data: any)=> {this.createHeatMap(data)}, '3D');

        for (let i=0; i<this.colorData.length; ++i) {
            let key = this.colorData[i].key;
            this._colorData[key] = this.colorData[i];
        }
    }

    /**
     * 创建等高线热力图
     * @param data 热力图数据
     */
    private createHeatMap(dataObj: any) {
        if (dataObj == undefined) return;

        this._left = dataObj.elementLeft;
        this._top = dataObj.elementTop;
        this._type = dataObj.type;
        let data = dataObj.threeData;

        if (data.length == 0) return;

        //判断采点数据
        let len = Math.sqrt(data.length);
        this._tempLen = len;
        this._mapData = new Array(data.length)

        let newData = [];
        let index = 0;

        switch(len) {
            case 3:
            for(var i=0;i<data.length;i+=3){
                let tData = [data[i], data[i+1], data[i+2]];
                // if (index == 1) tData.reverse();
                for (let n=0; n<tData.length; ++n) {
                    newData.push(tData[n]);
                }
                index++;
            }
            break;

            case 4:
            for(var i=0;i<data.length;i+=4){
                let tData = [data[i], data[i+1], data[i+2],data[i+3]];
                // if (index == 0 || index == 2) tData.reverse();
                for (let n=0; n<tData.length; ++n) {
                    newData.push(tData[n]);
                }
                index++;
            }
            break;

            case 5:
                for(var i=0;i<data.length;i+=5){
                    let tData = [data[i], data[i+1], data[i+2], data[i+3], data[i+4]];
                    // if (index == 1) tData.reverse();
                    for (let n=0; n<tData.length; ++n) {
                        newData.push(tData[n]);
                    }
                    index++;
                }
            break;

            case 6:
                for(var i=0;i<data.length;i+=6){
                    let tData = [data[i], data[i+1], data[i+2], data[i+3], data[i+4], data[i+5]];
                    // if (index == 1) tData.reverse();
                    for (let n=0; n<tData.length; ++n) {
                        newData.push(tData[n]);
                    }
                    index++;
                }
                break;

            default:
            break;
        }
        this._tnewData = newData;
        
        //等高线面板
        this._pgeometry = new THREE.PlaneBufferGeometry(len, len, len-1, len-1);
        let posotionAttribute = this._pgeometry.getAttribute('position');

        //修改模型顶点坐标
        this._distance = parseFloat(((len/2)/5).toFixed(2));
        for (let j = 0; j < posotionAttribute.count; ++j) {
            posotionAttribute.array[j*3+2] = -newData[j] * this._distance;
        }
        posotionAttribute.needsUpdate = true;
        
        //原点网格
        this.createGridHelper(len);
        this._heatMap = undefined;
        //创建热力图贴图
        this._heatMap = document.createElement('canvas');
        this._heatMap.width = 60; 
        this._heatMap.height = this._type == 'portrait'? 440: 376;
     
        this._heatMap.id = 'heatMap';
        this._heatMap.style.right = '5px';
        this._heatMap.style.position = 'absolute';

        this._heatMap.style.top = '0px';
        this._heatMap.style.zIndex = '1';
        // this._heatMap.borderRadius = '10px';
        this._heatMap.style.border = '#1px solid yellow';
 
        let main = document.getElementById('3DCanvas');
        if (main == null) return;
        main.appendChild(this._heatMap);

        let c = this._heatMap;
        this._tempCtx = c.getContext('2d');
        if (this._tempCtx == null) return;
        let gradient = this._tempCtx.createLinearGradient(0, c.height, 0, 0);

        gradient.addColorStop(0, '#6dbde1');
        gradient.addColorStop(.25, '#07ffe8');
        gradient.addColorStop(.5, '#66ff07');
        gradient.addColorStop(.75, '#ffc107');
        gradient.addColorStop(1, '#e78d71');
    
        this._tempCtx.fillStyle = gradient;
        this._tempCtx.rect(0, 0, 30, c.height);
        this._tempCtx.fill();
        this._tempCtx.closePath();

        //设置模型的热力图
        let heatMap = document.createElement('canvas');
        heatMap.width = 60; 
        heatMap.height = this._type == 'portrait'? 440: 376;
        let b = heatMap;
        let tempCtx: any;
        tempCtx = b.getContext('2d');
        let gradientCavas = tempCtx.createLinearGradient(0, b.height, 0, 0);
       
        let tData: any =[];
        for (let k=0; k<newData.length; ++k) {
            let ttData = newData[k].toFixed(1);
            if (ttData == 0 || ttData == -0) ttData = '0.0';
            tData.push(ttData);
        }

        let colorData = [];
        for (let t=0; t<tData.length; ++t) {
            let index = tData[t];
            if (parseFloat(index)  > 2.0) {
                index = '2.0';
            } else if (parseFloat(index) < -2.0){
                index = '-2.0';
            }
            let color = this._colorData[index]
            colorData.push(color.r/255, color.g/255, color.b/255);
        }

        this._pgeometry.addAttribute('color', new THREE.Float32BufferAttribute(colorData, 3));
       
        var materialc = new THREE.MeshPhongMaterial({
            side: THREE.DoubleSide,
            //@ts-ignore
            vertexColors: THREE.VertexColors,
            // wireframe: true
        });
       
        // let mesh = new THREE.Mesh(this._pgeometry, this._shader);
        let mesh = new THREE.Mesh(this._pgeometry, materialc);

        mesh.rotateX(Math.PI * 0.5);
        mesh.name = "heatMesh";

        if (this._temGroup == undefined) return;
        this._temGroup.add(mesh);

        //对数据进行排序
        data.sort().reverse();
        let t = 1;
        for (let n = 0; n<data.length; ++n) {
            let len = data.length; 
            let tem = data[n];
            
            if (tem >= 0) {
               this._mapData[n] = tem;
            } else {
                let tem1 = data[len-t];
                if (tem1 > tem) {
                    this._mapData[n] = tem1;
                    this._mapData[len-t] = tem;
                }
                ++t;
            }
        }

        //设置热力图视角
        this._scene3D.setHeatMapFov();

        //绘制颜色值刻度
        this.drawText(this._distance, len/2);

        //绘制坐标轴
        let geometry = new THREE.CylinderGeometry( .03, .03, 1, 8 );
        let cylinder = new THREE.Mesh( geometry, new THREE.MeshBasicMaterial( {color: 0x0000ff} ) );
        let box1 = new THREE.Box3().setFromObject(cylinder);
        let center1 = new THREE.Vector3().subVectors(box1.max, box1.min).multiplyScalar(0.5);
        cylinder.name = 'Helper';
        cylinder.position.set(-len/2, -len/2 + center1.y, len/2);
        this._temGroup.add( cylinder );

        let cylinder2 = new THREE.Mesh( geometry.clone(), new THREE.MeshBasicMaterial( {color: 0x00ff00} ) );
        let box2 = new THREE.Box3().setFromObject(cylinder2);
        let center2 = new THREE.Vector3().subVectors(box2.max, box2.min).multiplyScalar(0.5);
        cylinder2.name = 'Helper';
        cylinder2.position.set(-len/2, -len/2, len/2 - center2.y);
        cylinder2.rotateX(-Math.PI * 0.5);
        this._temGroup.add( cylinder2 );

        let cylinder3 = new THREE.Mesh( geometry.clone(), new THREE.MeshBasicMaterial( {color: 0xff0000} ) );
        let box3 = new THREE.Box3().setFromObject(cylinder2);
        let center3 = new THREE.Vector3().subVectors(box3.max, box3.min).multiplyScalar(0.5);
        cylinder3.name = 'Helper';
        cylinder3.position.set(-len/2 + center3.z, -len/2, len/2);
        cylinder3.rotateZ(-Math.PI * 0.5);
        this._temGroup.add( cylinder3 );

        //注销事件
        EventControl.removeEvent('MSG_CREATE_HEATMAP_MODEL', (data: any)=> {this.createHeatMap(data)}, '3D');
    }

    /**
     * 绘制热力图的网格间距
     * @param len 网格间距
     */
    private createGridHelper(len: any) {
        let gC = 0xffffff;
        let gcp = new THREE.GridHelper(len, len, gC, gC);
        gcp.name = 'Helper';
        //@ts-ignore
        let gcpAttribute = gcp.geometry.attributes.position;
        let gcount = gcpAttribute.count;
        for (let n=0; n<gcount; ++n) {
            //@ts-ignore
            gcpAttribute.array[n*3+1] = -len/2
        }
        gcpAttribute.needsUpdate = true;
        if (this._temGroup == undefined) return;
        this._temGroup.add(gcp);

        let gcp1 = gcp.clone();
        gcp1.name = 'Helper';
        gcp1.rotateX(Math.PI * 0.5);
        this._temGroup.add(gcp1);

        let gcp2 = gcp.clone();
        gcp2.name = 'Helper';
        gcp2.rotateZ(-Math.PI * 0.5);
        this._temGroup.add(gcp2);
    }
    /**
     * 绘制颜色值刻度尺
     * @param distance 刻度间距
     * @param len 刻度值
     */
    private drawText(distance: any, len: any) {
        let disLen = Math.floor(this._heatMap.height/10)-0.1;
        for(var i=0; i<11; ++i){
            this._tempCtx.beginPath();
            this._tempCtx.fillStyle = '#ffffff';
            if ((len-(i*distance)).toFixed(1) == '-0.0') {
                this._tempCtx.fillText(0.0, 40, i * disLen);
                this._indexData[i].key = 0.0;
            } else {
                if (this._type == 'portrait') {
                     if (i == 0) {
                        this._tempCtx.fillText((len-(i*distance)).toFixed(1), 40, i*disLen + 8.5);
                    } else {
                        this._tempCtx.fillText((len-(i*distance)).toFixed(1), 40, i*disLen);
                    }

                } else if (this._type == 'landscape') {
                    if (i == 0) {
                        this._tempCtx.fillText((len-(i*distance)).toFixed(1), 40, i*disLen + 8.5);
                    } else if(i == 10) {
                        this._tempCtx.fillText((len-(i*distance)).toFixed(1), 40, i*disLen + 5.5);
                    } else {
                        this._tempCtx.fillText((len-(i*distance)).toFixed(1), 40, i*disLen);
                    }
                }

                this._indexData[i].key = (len-(i*distance)).toFixed(1);
            }
            this._tempCtx.closePath();
        }
    }

    /**
     * 判断模型是否显示或者隐藏
     * @param flag 显示隐藏状态
     */
    public onHideAnyModel(flag: any) {
        let scope = this;
        if (this._scene3D._temGroup == undefined) return;

        //关闭坐标选中框
        this._scene3D._tdiv.style.display = 'none';

        this._scene3D._temGroup.traverse(function(child){
            if (child.name == 'Helper') {
                child.visible = flag;
                if (flag) {
                    let pageContainer = scope._tempDiv.parentNode;
                    if (pageContainer) {
                        pageContainer.removeChild(scope._tempDiv);
                    }
                }
            }
        });

        //渲染
        this._scene3D.render();

        //隐藏3D图形
        this._scene3D._temGroup.traverse(function(child){
            child.visible = flag;
        });

        if (!flag) {
            let mainContainer = this._scene3D.getMainContainer();
            if (mainContainer == undefined) return;
            //创建热力图贴图
            this._tempDiv = document.createElement('div');
            this._tempDiv.style.width = mainContainer.clientWidth - 70 + 'px';
            this._tempDiv.style.height = mainContainer.clientHeight + 'px';

            this._tempDiv.style.position = 'absolute';
            this._tempDiv.id = "tempDiv";
            this._tempDiv.style.top = '0px';
            this._tempDiv.style.backgroundColor = "#101216";
            this._tempDiv.style.top = '0px';
            this._tempDiv.style.zIndex = '3';

            let containerId = this._scene3D.getContainerId();
            let main = document.getElementById(containerId? containerId : 'heatMap');
            if (main == null) return;
            main.appendChild(this._tempDiv);
           
            // 快照功能
            let render = this._scene3D.getRenderer();
            if (render == undefined) return;
            let imgData = render.domElement.toDataURL();

            let clX = mainContainer.clientWidth - 70;
            let clY = mainContainer.clientHeight;
           
            this._canvas = document.createElement('canvas');
            this._canvas.width = clX;
            this._canvas.height = clY;
            this._canvas.style.zIndex = '99';

            this._tempDiv.appendChild(this._canvas);
            this._tempContext = this._canvas.getContext('2d');

            //绘制快照图片
            let img = new Image();
            img.src = imgData;
            img.onload = function(){
                if (scope._tempContext == null) return;
                scope._tempContext.drawImage(img, 0, 0, clX, clY);
            }
            //计算分区布局
            scope.onDivPartition();
        }
    }

    /**
     * 注册鼠标鼠标事件和触控事件
     * @returns null
     */
    private onDivPartition() {
        if (this._tempDiv == undefined) return;
        this._tempDiv.addEventListener('mousedown', (event: MouseEvent) => this.onMouseDown(event), false);
        this._tempDiv.addEventListener('mouseup', (event: MouseEvent) => this.onMouseUp(event), false);

        //触控事件
        this._tempDiv.addEventListener('touchstart', (event: TouchEvent) => this.onTouchStart(event), false);
        this._tempDiv.addEventListener('touchend', (event: TouchEvent) => this.onTouchEnd(event), false);
    }

    /**
     * 鼠标按下时间内
     * @param event 
     * @returns 
     */
    private onMouseDown(event: MouseEvent) {
        event.preventDefault();
        // 左键点击选中
        if (event.button != 0) {
            return;
        } 
        this._mouseDownPoint.x = event.x;
        this._mouseDownPoint.y = event.y;
    }

    /**
     * 鼠标抬起事件
     * @param event 
     * @returns 
     */
    private onMouseUp(event: MouseEvent) {
        event.preventDefault();
        // 左键点击选中
        if (event.button != 0) {
            return;
        }
        this._mouseUpPoint.x = event.x;
        this._mouseUpPoint.y = event.y;
        let distance = this._mouseUpPoint.distanceTo(this._mouseDownPoint);
        if (distance > this._isMoveLength) {
            return;
        }
        this._mouseDownPoint.set(0, 0);
        this._mouseUpPoint.set(0, 0);

        let clientX = event.clientX - this._left; 
        let clientY = event.clientY - this._top;

        let currentX = (clientX/2).toFixed(2);
        let currentY = (this._lengthY - (clientY/2)).toFixed(2);
        this._coordinates = {x: currentX, y: currentY, offset: 0.0};

        if (this._type == "portrait") {
            let temValue = parseFloat((220/this._tempLen).toFixed(2));
            switch(this._tempLen){
                case 3:
                    if (parseFloat(currentY) <= temValue) {
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[0];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[1];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[2];
                        } else if (parseFloat(currentX) > temValue * 3) {
                            this._coordinates.offset = this._tnewData[2];
                        }
                    } else if (parseFloat(currentY) > temValue && parseFloat(currentY) <= temValue * 2) {
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[3];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[4];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[5];
                        } else if (parseFloat(currentX) > temValue * 3) {
                            this._coordinates.offset = this._tnewData[5];
                        } 
                    } else if (parseFloat(currentY) > temValue *2 && parseFloat(currentY) < temValue * 3){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[6];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[7];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        } else if (parseFloat(currentX) > temValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        }
                    } else if (parseFloat(currentY) >= temValue * 3){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[6];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[7];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        } else if (parseFloat(currentX) > temValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        }
                    }
                    break;
    
                case 4:
                    if (parseFloat(currentY) <= temValue) {
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[0];
                        } else if (parseFloat(currentX) >= temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[1];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[2];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[3];
                        } else if (parseFloat(currentX) > temValue * 4) {
                            this._coordinates.offset = this._tnewData[3];
                        }
                    } else if (parseFloat(currentY) > temValue && parseFloat(currentY) <= temValue * 2) {
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[4];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[5];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[6];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[7];
                        } else if (parseFloat(currentX) > temValue * 4) {
                            this._coordinates.offset = this._tnewData[7];
                        } 
                    } else if (parseFloat(currentY) > temValue *2 && parseFloat(currentY) <= temValue * 3){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[8];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[9];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[10];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[11];
                        } else if (parseFloat(currentX) > temValue * 4) {
                            this._coordinates.offset = this._tnewData[11];
                        }
                    } else if (parseFloat(currentY) > temValue * 3 && parseFloat(currentY) < temValue * 4){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[12];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[13];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[14];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        } else if (parseFloat(currentX) > temValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        }
                    } else if (parseFloat(currentY) >= temValue * 4){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[12];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[13];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[14];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        } else if (parseFloat(currentX) > temValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        }
                    }
                    break;
                    
                default:
                    break;    
           }

        } else {
            let temValue = parseFloat((220/this._tempLen).toFixed(2));
            let lemValue = parseFloat((293/this._tempLen).toFixed(2));
            switch(this._tempLen){
                case 3:
                    if (parseFloat(currentY) <= temValue) {
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[0];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[1];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[2];
                        } else if (parseFloat(currentX) > lemValue * 3) {
                            this._coordinates.offset = this._tnewData[2];
                        }
                    } else if (parseFloat(currentY) > temValue && parseFloat(currentY) <= temValue * 2) {
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[3];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[4];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[5];
                        } else if (parseFloat(currentX) > lemValue * 3) {
                            this._coordinates.offset = this._tnewData[5];
                        } 
                    } else if (parseFloat(currentY) > temValue *2 && parseFloat(currentY) < temValue * 3){
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[6];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[7];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        } else if (parseFloat(currentX) > lemValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        }
                    } else if (parseFloat(currentY) >= temValue * 3){
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[6];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[7];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        } else if (parseFloat(currentX) > lemValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        }
                    }
                    break;
    
                case 4:
                    if (parseFloat(currentY) <= temValue) {
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[0];
                        } else if (parseFloat(currentX) >= lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[1];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[2];
                        } else if (parseFloat(currentX) > lemValue * 3 && parseFloat(currentX) <= lemValue * 4) {
                            this._coordinates.offset = this._tnewData[3];
                        } else if (parseFloat(currentX) > lemValue * 4) {
                            this._coordinates.offset = this._tnewData[3];
                        }
                    } else if (parseFloat(currentY) > temValue && parseFloat(currentY) <= temValue * 2) {
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[4];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[5];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[6];
                        } else if (parseFloat(currentX) > lemValue * 3 && parseFloat(currentX) <= lemValue * 4) {
                            this._coordinates.offset = this._tnewData[7];
                        } else if (parseFloat(currentX) > lemValue * 4) {
                            this._coordinates.offset = this._tnewData[7];
                        } 
                    } else if (parseFloat(currentY) > temValue *2 && parseFloat(currentY) <= temValue * 3){
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[8];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[9];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[10];
                        } else if (parseFloat(currentX) > lemValue * 3 && parseFloat(currentX) <= lemValue * 4) {
                            this._coordinates.offset = this._tnewData[11];
                        } else if (parseFloat(currentX) > lemValue * 4) {
                            this._coordinates.offset = this._tnewData[11];
                        }
                    } else if (parseFloat(currentY) > temValue * 3 && parseFloat(currentY) < temValue * 4){
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[12];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[13];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[14];
                        } else if (parseFloat(currentX) > lemValue * 3 && parseFloat(currentX) <= lemValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        } else if (parseFloat(currentX) > lemValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        }
                    } else if (parseFloat(currentY) >= temValue * 4){
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[12];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[13];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[14];
                        } else if (parseFloat(currentX) > lemValue * 3 && parseFloat(currentX) <= lemValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        } else if (parseFloat(currentX) > lemValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        }
                    }
                    break;
                    
                default:
                    break;    
           }
        }

        EventControl.dispatchEvent('MSG_TOUCH_GET_COORDINATES', this._coordinates, '3D');
    }
 
    //触控开始事件
    private onTouchStart(event: TouchEvent) {
        event.preventDefault();
        if (event.changedTouches.length == 0) return;
    }

    /**
     * 触控结束事件
     * @param event 
     * @returns null 
     */
    private onTouchEnd(event: TouchEvent) {
        event.preventDefault();
        if (event.changedTouches.length == 0) return;
        let touchList = event.changedTouches[0];

        let clientX = touchList.clientX - this._left; 
        let clientY = touchList.clientY - this._top;

        let currentX = (clientX/2).toFixed(2);
        let currentY = (this._lengthY - (clientY/2)).toFixed(2);
        
        this._coordinates = {x: currentX, y: currentY, offset: 0.0};

        this._scene3D._tdiv.style.display = 'block';

        this._scene3D._tdiv.style.left = clientX -15 + 'px';
        this._scene3D._tdiv.style.top = clientY - 15 + 'px';

        if (this._type == "portrait") {
            let temValue = parseFloat((220/this._tempLen).toFixed(2));
            switch(this._tempLen){
                case 3:
                    if (parseFloat(currentY) <= temValue) {
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[0];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[1];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[2];
                        } else if (parseFloat(currentX) > temValue * 3) {
                            this._coordinates.offset = this._tnewData[2];
                        }
                    } else if (parseFloat(currentY) > temValue && parseFloat(currentY) <= temValue * 2) {
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[3];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[4];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[5];
                        } else if (parseFloat(currentX) > temValue * 3) {
                            this._coordinates.offset = this._tnewData[5];
                        } 
                    } else if (parseFloat(currentY) > temValue *2 && parseFloat(currentY) < temValue * 3){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[6];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[7];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        } else if (parseFloat(currentX) > temValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        }
                    } else if (parseFloat(currentY) >= temValue * 3){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[6];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[7];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        } else if (parseFloat(currentX) > temValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        }
                    }
                   
                    break;
    
                case 4:
                    if (parseFloat(currentY) <= temValue) {
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[0];
                        } else if (parseFloat(currentX) >= temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[1];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[2];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[3];
                        } else if (parseFloat(currentX) > temValue * 4) {
                            this._coordinates.offset = this._tnewData[3];
                        }
                    } else if (parseFloat(currentY) > temValue && parseFloat(currentY) <= temValue * 2) {
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[4];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[5];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[6];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[7];
                        } else if (parseFloat(currentX) > temValue * 4) {
                            this._coordinates.offset = this._tnewData[7];
                        } 
                    } else if (parseFloat(currentY) > temValue *2 && parseFloat(currentY) <= temValue * 3){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[8];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[9];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[10];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[11];
                        } else if (parseFloat(currentX) > temValue * 4) {
                            this._coordinates.offset = this._tnewData[11];
                        }
                    } else if (parseFloat(currentY) > temValue * 3 && parseFloat(currentY) < temValue * 4){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[12];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[13];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[14];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        } else if (parseFloat(currentX) > temValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        }
                    } else if (parseFloat(currentY) >= temValue * 4){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[12];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[13];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[14];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        } else if (parseFloat(currentX) > temValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        }
                    }
                    break;

                case 5:
                    if (parseFloat(currentY) <= temValue) {
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[0];
                        } else if (parseFloat(currentX) >= temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[1];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[2];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[3];
                        } else if (parseFloat(currentX) > temValue * 4 && parseFloat(currentX) <= temValue * 5) {
                            this._coordinates.offset = this._tnewData[4];
                        }else if (parseFloat(currentX) > temValue * 5) {
                            this._coordinates.offset = this._tnewData[4];
                        }
                    } else if (parseFloat(currentY) > temValue && parseFloat(currentY) <= temValue * 2) {
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[5];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[6];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[7];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[8];
                        } else if (parseFloat(currentX) > temValue * 4 && parseFloat(currentX) <= temValue * 5) {
                            this._coordinates.offset = this._tnewData[9];
                        } else if (parseFloat(currentX) > temValue * 5) {
                            this._coordinates.offset = this._tnewData[9];
                        } 
                    } else if (parseFloat(currentY) > temValue *2 && parseFloat(currentY) <= temValue * 3){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[10];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[11];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[12];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[13];
                        } else if (parseFloat(currentX) > temValue * 4 && parseFloat(currentX) <= temValue * 5) {
                            this._coordinates.offset = this._tnewData[14];
                        } else if (parseFloat(currentX) > temValue * 5) {
                            this._coordinates.offset = this._tnewData[14];
                        }

                    } else if (parseFloat(currentY) > temValue * 3 && parseFloat(currentY) < temValue * 4){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[15];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[16];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[17];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[18];
                        } else if (parseFloat(currentX) > temValue * 4 && parseFloat(currentX) <= temValue * 5) {
                            this._coordinates.offset = this._tnewData[19];
                        } else if (parseFloat(currentX) > temValue * 5 ) {
                            this._coordinates.offset = this._tnewData[19];
                        }
                    } else if (parseFloat(currentY) >= temValue * 4){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[20];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[21];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[22];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[23];
                        } else if (parseFloat(currentX) > temValue * 4 && parseFloat(currentX) <= temValue * 5) {
                            this._coordinates.offset = this._tnewData[24];
                        } else if (parseFloat(currentX) > temValue * 5) {
                            this._coordinates.offset = this._tnewData[24];
                        }
                    }
                    break;
                    
                default:
                    break;    
           }

        } else {
            let temValue = parseFloat((220/this._tempLen).toFixed(2));
            let lemValue = parseFloat((293/this._tempLen).toFixed(2));
            switch(this._tempLen){
                case 3:
                    if (parseFloat(currentY) <= temValue) {
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[0];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[1];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[2];
                        } else if (parseFloat(currentX) > lemValue * 3) {
                            this._coordinates.offset = this._tnewData[2];
                        }
                    } else if (parseFloat(currentY) > temValue && parseFloat(currentY) <= temValue * 2) {
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[3];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[4];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[5];
                        } else if (parseFloat(currentX) > lemValue * 3) {
                            this._coordinates.offset = this._tnewData[5];
                        } 
                    } else if (parseFloat(currentY) > temValue *2 && parseFloat(currentY) < temValue * 3){
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[6];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[7];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        } else if (parseFloat(currentX) > lemValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        }
                    } else if (parseFloat(currentY) >= temValue * 3){
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[6];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[7];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        } else if (parseFloat(currentX) > lemValue * 3) {
                            this._coordinates.offset = this._tnewData[8];
                        }
                    }
                   
                    break;
    
                case 4:
                    if (parseFloat(currentY) <= temValue) {
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[0];
                        } else if (parseFloat(currentX) >= lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[1];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[2];
                        } else if (parseFloat(currentX) > lemValue * 3 && parseFloat(currentX) <= lemValue * 4) {
                            this._coordinates.offset = this._tnewData[3];
                        } else if (parseFloat(currentX) > lemValue * 4) {
                            this._coordinates.offset = this._tnewData[3];
                        }
                    } else if (parseFloat(currentY) > temValue && parseFloat(currentY) <= temValue * 2) {
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[4];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[5];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[6];
                        } else if (parseFloat(currentX) > lemValue * 3 && parseFloat(currentX) <= lemValue * 4) {
                            this._coordinates.offset = this._tnewData[7];
                        } else if (parseFloat(currentX) > lemValue * 4) {
                            this._coordinates.offset = this._tnewData[7];
                        } 
                    } else if (parseFloat(currentY) > temValue *2 && parseFloat(currentY) <= temValue * 3){
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[8];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[9];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[10];
                        } else if (parseFloat(currentX) > lemValue * 3 && parseFloat(currentX) <= lemValue * 4) {
                            this._coordinates.offset = this._tnewData[11];
                        } else if (parseFloat(currentX) > lemValue * 4) {
                            this._coordinates.offset = this._tnewData[11];
                        }
                    } else if (parseFloat(currentY) > temValue * 3 && parseFloat(currentY) < temValue * 4){
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[12];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[13];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[14];
                        } else if (parseFloat(currentX) > lemValue * 3 && parseFloat(currentX) <= lemValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        } else if (parseFloat(currentX) > lemValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        }
                    } else if (parseFloat(currentY) >= temValue * 4){
                        if (parseFloat(currentX) <= lemValue) {
                            this._coordinates.offset = this._tnewData[12];
                        } else if (parseFloat(currentX) > lemValue && parseFloat(currentX) <= lemValue * 2) {
                            this._coordinates.offset = this._tnewData[13];
                        } else if (parseFloat(currentX) > lemValue * 2 && parseFloat(currentX) <= lemValue * 3) {
                            this._coordinates.offset = this._tnewData[14];
                        } else if (parseFloat(currentX) > lemValue * 3 && parseFloat(currentX) <= lemValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        } else if (parseFloat(currentX) > lemValue * 4) {
                            this._coordinates.offset = this._tnewData[15];
                        }
                    }
                    break;

                case 5:
                    if (parseFloat(currentY) <= temValue) {
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[0];
                        } else if (parseFloat(currentX) >= temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[1];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[2];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[3];
                        } else if (parseFloat(currentX) > temValue * 4 && parseFloat(currentX) <= temValue * 5) {
                            this._coordinates.offset = this._tnewData[4];
                        }else if (parseFloat(currentX) > temValue * 5) {
                            this._coordinates.offset = this._tnewData[4];
                        }
                    } else if (parseFloat(currentY) > temValue && parseFloat(currentY) <= temValue * 2) {
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[5];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[6];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[7];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[8];
                        } else if (parseFloat(currentX) > temValue * 4 && parseFloat(currentX) <= temValue * 5) {
                            this._coordinates.offset = this._tnewData[9];
                        } else if (parseFloat(currentX) > temValue * 5) {
                            this._coordinates.offset = this._tnewData[9];
                        } 
                    } else if (parseFloat(currentY) > temValue *2 && parseFloat(currentY) <= temValue * 3){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[10];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[11];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[12];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[13];
                        } else if (parseFloat(currentX) > temValue * 4 && parseFloat(currentX) <= temValue * 5) {
                            this._coordinates.offset = this._tnewData[14];
                        } else if (parseFloat(currentX) > temValue * 5) {
                            this._coordinates.offset = this._tnewData[14];
                        }

                    } else if (parseFloat(currentY) > temValue * 3 && parseFloat(currentY) < temValue * 4){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[15];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[16];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[17];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[18];
                        } else if (parseFloat(currentX) > temValue * 4 && parseFloat(currentX) <= temValue * 5) {
                            this._coordinates.offset = this._tnewData[19];
                        } else if (parseFloat(currentX) > temValue * 5 ) {
                            this._coordinates.offset = this._tnewData[19];
                        }
                    } else if (parseFloat(currentY) >= temValue * 4){
                        if (parseFloat(currentX) <= temValue) {
                            this._coordinates.offset = this._tnewData[20];
                        } else if (parseFloat(currentX) > temValue && parseFloat(currentX) <= temValue * 2) {
                            this._coordinates.offset = this._tnewData[21];
                        } else if (parseFloat(currentX) > temValue * 2 && parseFloat(currentX) <= temValue * 3) {
                            this._coordinates.offset = this._tnewData[22];
                        } else if (parseFloat(currentX) > temValue * 3 && parseFloat(currentX) <= temValue * 4) {
                            this._coordinates.offset = this._tnewData[23];
                        } else if (parseFloat(currentX) > temValue * 4 && parseFloat(currentX) <= temValue * 5) {
                            this._coordinates.offset = this._tnewData[24];
                        } else if (parseFloat(currentX) > temValue * 5) {
                            this._coordinates.offset = this._tnewData[24];
                        }
                    }
                    break;
                    
                default:
                    break;    
           }
        }
       
        EventControl.dispatchEvent('MSG_TOUCH_GET_COORDINATES', this._coordinates, '3D');
    }

    /**
     * 置空数据
     * @returns null
     */
    public dispose() {
        if (this._tempDiv == undefined) return;
        //删除Div
        let pageContainer = this._tempDiv.parentNode;
        if (pageContainer) {
            pageContainer.removeChild(this._tempDiv);
        }

        //删除触控事件
        this._tempDiv.removeEventListener('touchstart', (event: TouchEvent) => this.onTouchStart(event), false);
        this._tempDiv.removeEventListener('touchend', (event: TouchEvent) => this.onTouchEnd(event), false);

        //置空对象
        this._indexData = [];
        this._mapData = [];
        this._pgeometry = undefined;
        this._temGroup = undefined;
        this._tempLen = undefined;
        this._tempCtx = undefined;
        this._shader = undefined;
        this._heatMap = undefined;
        this._tempDiv = undefined;
        this._canvas = undefined;
        this._tempContext = undefined;
        this._coordinates = undefined;
    }
}