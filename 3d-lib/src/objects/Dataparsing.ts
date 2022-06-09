import * as THREE from "three";
import { EventControl } from "../event/EventControl";

export class Dataparsing {
    private xhr: any;
    private _objClass: any;

    constructor(objClass: any) {
        this._objClass = objClass;
        EventControl.addEvent('MSG_XMLHttpRequest_Event_ABORT', ()=> { this.dispose() }, '3D');
    }
    
    /**
     * 销毁XMLHttpRequest请求
     */
    public dispose() {
        //注销XMLHttpRequest请求
        if (this.xhr != undefined) {
            this.xhr.abort();
        }
        //删除事件请求事件
        EventControl.removeEvent('MSG_XMLHttpRequest_Event_ABORT', ()=>{}, '3D');
    }

    /**
     * 解析模型数据
     * @param data 模型数据
     */
    public onParseStl(data: any, tConfig: any) {
        // let newData = JSON.parse(data)

        let newData = data;
        //设置Mesh对象
        let modelObject = new ModelObject();

        modelObject.vertexNormalBuffer = newData.vertexNormalBuffer;
        modelObject.vertexBuffer = newData.vertexBuffer; 
        modelObject.indexBuffer = newData.indexBuffer;

        this.onCreateGeometry(modelObject, tConfig);
    }

    /**
     * 开始创建模型的几何信息
     * @param data 模型几何数据
     */
    public onCreateGeometry(data: any, tConfig: any) {
        let scope = this;

        let indexBuffers = data.indexBuffer;
        let vertexBuffers = data.vertexBuffer;
        let vertexNormalBuffers = data.vertexNormalBuffer

        data.indexBuffer = [];
        data.vertexBuffer = [];
        data.vertexNormalBuffer = [];

        data = null;
        //构件几何体
        let geometry = new THREE.BufferGeometry();
        let vv = new Float32Array(vertexBuffers);
        let vn = new Float32Array(vertexNormalBuffers);
        geometry.setAttribute('position', new THREE.BufferAttribute(vv, 3));
        geometry.setAttribute('normal', new THREE.BufferAttribute(vn, 3));
        geometry.setIndex(indexBuffers);
        // console.log(geometry);

        indexBuffers = null;
        vertexBuffers = null;
        vertexNormalBuffers = null;

        let shaderMaterial = new THREE.MeshPhongMaterial( {
            color: 0x444242,
            shininess: 15,
            emissive: 0x000000,
            specular: 0xb4b6bc,
        });

        let mesh = new THREE.Mesh( geometry, shaderMaterial );
        let box1 = new THREE.Box3();
        box1.expandByObject(mesh);
        let mmaxLen = box1.max.distanceTo(box1.min);
        if (mmaxLen <=10) {
            mesh.scale.set(50,50,50);
        }

        //注意模型反转后y轴与z轴要注意
        mesh.rotation.set( -Math.PI / 2, 0, 0 );
        mesh.name = "modelStl";
        mesh.userData.id = '001';
        mesh.renderOrder = 6;
        mesh.receiveShadow = true;
        mesh.castShadow = true;
        let box = new THREE.Box3();
        box.expandByObject(mesh);
        let center = box.getCenter(new THREE.Vector3());
        mesh.position.set(0-center.x, 0-center.y, 0-center.z);
        if (tConfig.pro3GridHelper) {
            mesh.translateY(-2900);
        }

        //置空模型数据
        data = null;

        // console.log(data);
        //添加模型进入场景
        this._objClass._objectCollection.addStlData(mesh);
  
        //触发模型加载完成事件
        this._objClass._objectCollection.finishLoadTask();

       
        let time = setTimeout(function(){
            //添加模型进入场景
            scope._objClass._objectCollection.addStlData(mesh);

            //触发模型加载完成事件
            scope._objClass._objectCollection.finishLoadTask();
    
            clearTimeout (time);
           
        },100);
    }
}

/**
 * 解析模型数据
 */
export class ModelObject {

    public vertexBuffer: any;
    public indexBuffer: any;
    public faceNormalBuffer: any;
    public vertexNormalBuffer: any;

    constructor() {
        this.faceNormalBuffer = [];
        this.indexBuffer = [];
        this.vertexBuffer = [];
        this.vertexNormalBuffer = [];
    }

    //计算模型的顶点法线
   public computeVertexNormals() {
        if(!this.vertexNormalBuffer.length) {
            this.vertexNormalBuffer = new Array(this.vertexBuffer.length);
            this.calcVertexNormals();
        }
    }

    //判断顶点数组
   public isTrivial() {
        return ( !this.vertexBuffer || this.vertexBuffer.length < 3 ||  !this.indexBuffer || this.indexBuffer.length < 3 );
    };

    //计算模型的面法线
    public calcFaceNormals() {
        var ibuf = this.indexBuffer;
        var i = 0;
        while(i < ibuf.length) {
            do {
            } while (ibuf[i++] != -1);
        }
    };

    //计算模型顶点法线
    public calcVertexNormals() {
        var ibuf = this.indexBuffer;
        var fnbuf = this.faceNormalBuffer;
        var vnbuf = this.vertexNormalBuffer;
        for(var i=0; i<vnbuf.length; i++) {
            vnbuf[i] = 0;
        }

        //
        var i = 0, j = 0, k = 0;
        while(i < ibuf.length) {
            k = ibuf[i++];
            if(k == -1) {
                j += 3;
            }
            else {
                var index = k * 3;
                // add face normal to vertex normal
                vnbuf[index    ] += fnbuf[j    ];
                vnbuf[index + 1] += fnbuf[j + 1];
                vnbuf[index + 2] += fnbuf[j + 2];
            }
        }
        // normalize vertex normals
        // CC3D.Math3D.normalizeVectors(vnbuf, vnbuf);
        var num = vnbuf.length;
		for(var i=0; i<num; i+=3) {
			var x = vnbuf[i    ];
			var y = vnbuf[i + 1];
			var z = vnbuf[i + 2];
			var len = Math.sqrt(x * x + y * y + z * z);
			if(len > 0) {
				len = 1 / len;
				x *= len;
				y *= len;
				z *= len;
			}
			vnbuf[i    ] = x;
			vnbuf[i + 1] = y;
			vnbuf[i + 2] = z;
		}
    }
}

/**
 * 二元流
 */
export class BinaryStream {

    private data: any;
    private offset: any;

    constructor(data: any) {
        this.data = data;
	    this.offset = 0;
    }

    //获取数据长度
    public size() {
        return this.data.length;
    };

    //重置偏移量
    public reset() {
        this.offset = 0;
    };

    //跳动
    public skip(bytesToSkip: any) {
        if(this.offset + bytesToSkip > this.data.length)
            this.offset = this.data.length;
        else
            this.offset += bytesToSkip;
    };

    //
   public eof() {
        return !(this.offset < this.data.length);
    };

    public readUInt8() {
        return this.decodeInt(1, false);
    };

   public readInt8() {
        return this.decodeInt(1, true);
    };

   public readUInt16() {
        return this.decodeInt(2, false);
    };

   public readInt16() {
        return this.decodeInt(2, true);
    };

    public readUInt32() {
        return this.decodeInt(4, false);
    };

   public readInt32() {
        return this.decodeInt(4, true);
    };

    public readFloat32() {
        return this.decodeFloat(4, 23);
    };

   public readFloat64() {
        return this.decodeFloat(8, 52);
    };

   public readBytes(buffer: any, bytesToRead: any) {
        var bytesRead = bytesToRead;
        if(this.offset + bytesToRead > this.data.length)
            bytesRead = this.data.length - this.offset;

        for(var i=0; i<bytesRead; i++) {
            buffer[i] = this.data[this.offset++].charCodeAt(0) & 0xff;
        }
        return bytesRead;
    };

   public decodeInt(bytes: any, isSigned: any) {
        if(this.offset + bytes > this.data.length) {
            this.offset = this.data.length;
            return NaN;
        }

        var rv = 0, f = 1;
        for(var i=0; i<bytes; i++) {
            rv += ((this.data[this.offset++].charCodeAt(0) & 0xff) * f);
            f *= 256;
        }

        if( isSigned && (rv & Math.pow(2, bytes * 8 - 1)) )
            rv -= Math.pow(2, bytes * 8);
        return rv;
    };

   public decodeFloat(bytes: any, significandBits: any) {
        if(this.offset + bytes > this.data.length) {
            this.offset = this.data.length;
            return NaN;
        }

        var mLen = significandBits;
        var eLen = bytes * 8 - mLen - 1;
        var eMax = (1 << eLen) - 1;
        var eBias = eMax >> 1;

        var i = bytes - 1; 
        var d = -1; 
        var s = this.data[this.offset + i].charCodeAt(0) & 0xff; 
        i += d; 
        var bits = -7;
        var e = s & ((1 << (-bits)) - 1);
        s >>= -bits;
        bits += eLen
        while(bits > 0) {
            e = e * 256 + (this.data[this.offset + i].charCodeAt(0) & 0xff);
            i += d;
            bits -= 8;
        }

        var m = e & ((1 << (-bits)) - 1);
        e >>= -bits;
        bits += mLen;
        while(bits > 0) {
            m = m * 256 + (this.data[this.offset + i].charCodeAt(0) & 0xff);
            i += d;
            bits -= 8;
        }
        this.offset += bytes;

        switch(e) {
            case 0:		// 0 or denormalized number
                e = 1 - eBias;
                break;
            case eMax:	// NaN or +/-Infinity
                return m ? NaN : ((s ? -1 : 1) * Infinity);
            default:	// normalized number
                m += Math.pow(2, mLen);
                e -= eBias;
                break;
        }
        return (s ? -1 : 1) * m * Math.pow(2, e - mLen);
    };
}