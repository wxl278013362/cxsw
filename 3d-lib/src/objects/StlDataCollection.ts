import { DataConfig } from '../configDefine/DataConfig';
import { MeshData, DataInfo } from './MeshData';
import { EventControl } from '../event/EventControl';
import { StlDataFactory } from "./StlDataFactory";

/**
 * 数据集.每个场景同一时刻只能绑定一个数据集
 */
export class StlDataCollection {
    /**
     * 数据集请求的数据个数
     */
    private _requestCount: number = 0;
    /**
     * 数据集已经加载的数据个人
     */
    private _loadedCount: number = 0;

    /**
     * 存储解析的数据对象
     */
    private _dataArray: Array<MeshData> = [];

    /**
     * 用于暂存数据
     */

    public _indexGcodeData: {[key:string] : any} = {};

    /**
     * 当前场景中每次呈现的理论上都只有一个MeshObject数据
     */
    private _activeData: MeshData | undefined = undefined;

    private _activeAllData: Array<MeshData> = [];

    /**
     * 数据集的名称
     */
    public name: string;
    /**
     * 是否是数据集
     */
    public isDataCollection: boolean = true;
    /**  
     * 数据集是否已经加载完毕
     */
    public isMaxLoaded: boolean = false;
    /**
     * 加载完毕时的回调函数
     */
    public onLoadMaxComplete: Function | undefined = undefined;

    private _nextNumber: number = 0;

    private _modelLength: number = 0; 

    private _maxModelData: Array<any> = [];

    private _config: any = undefined;

    public _data: any;

    public _temScene3D: any;

    /**
     * 数据集构造函数
     * @param name 数据集的名称
     */
    constructor(name: string, config: any, temScene3D: any) {
        this.name = name;
        this._config = config;

        this._temScene3D = temScene3D;
    }

    /**
     * 卸载数据
     */
    public dispose() {
        this._dataArray = [];
        this._activeData = undefined;
        this._activeAllData = [];
    }

    /**
     * 新增一个数据请求。由ObjectFactory调用
     */
    public createLoadTask() {
        this._requestCount++;
    }

    /**
     * 完成一个数据请求。由ObjectFactory调用
     */
    public finishLoadTask() {
        this._loadedCount++;
        // EventControl.dispatchEvent('MSG_LOAD_3DS_DATA_PROGRESS', {loadedCount: this._requestCount, requestCount: this._modelLength}, 'all');
        if (this.onLoadMaxComplete) {
            this.isMaxLoaded = true;
            this.onLoadMaxComplete(); 
        }

        if (this._loadedCount >= this._requestCount) {   
            //模型加载错误提示  
            // EventControl.dispatchEvent('MSG_LOAD_3DS_DATA_PROGRESS', {loadedCount: this._requestCount, requestCount: this._modelLength}, 'all');
            
            //设置模型的剖切功能
            EventControl.dispatchEvent('MSG_INIT_CLIP', null, 'all');

            EventControl.dispatchEvent('MSG_CHANGE_MODEL_FOV', null, 'all');
            //调整模型的位置
            EventControl.dispatchEvent('MSG_MODEL_POSITION_COUNT', this._modelLength, 'all');

            //初始化控制器
            EventControl.dispatchEvent('MSG_LOAD_3DS_DATA_FINISH', null, 'all');

            EventControl.dispatchEvent('MSG_LOAD_3DS_DATA_PROGRESS', {loadedCount: this._requestCount, requestCount: this._modelLength, vertexData: this._data}, 'all');
        }
    }

    /**
     * 通过数据集加载数据
     * @param configArray 
     */
    public loadData(configArray: Array<DataConfig>) {
        this._modelLength = configArray.length;
        let config = configArray[0];
        let dataType = config.dataType;

        switch (dataType) {
            case "stl":
                this.onLoadStlData(configArray, this._temScene3D);
            break;

            case "gcode":
                this.onLoadGcodeData(configArray, this._temScene3D);
                break;

            case "ply":
                    this.onLoadStlData(configArray, this._temScene3D);
                break;

            case "":
                console.error('dataType must be \'stl\' of \'gcode\', now we get undefined.')
            break;
            default:
                break;
        }
    }

    /**
     * 加载stl数据
     * @param configArray stl数据属性
     */
    private onLoadStlData(configArray: Array<DataConfig>, temScene3D: any) {
        let config = configArray[0];
        //第一个模型加载
        if (this._nextNumber == 0) {
            let objectFactory = new StlDataFactory(this, config, this._config, temScene3D);
            objectFactory.onStlData(config);
        }  
    }

    /**
     * 加载gcode数据
     * @param configArray gcode数据属性
     */
    private onLoadGcodeData(configArray: Array<DataConfig>, temScene3D: any) {
        let config = configArray[0];
        //第一个模型加载
        if (this._nextNumber == 0) {
            let objectFactory = new StlDataFactory(this, config, this._config, temScene3D);
            objectFactory.onStlData(config);
        }  
    }

    /**
     * 添加数据到数据集中
     * @param data 数据对象
     */
    public addData(data: MeshData) {
        let findIndex = this._dataArray.indexOf(data);
        if (findIndex == -1) {
            this._dataArray.push(data);
        }
    }

   /**
    * 添加max模型数据
    * @param data
    */
    public addStlData(data: any) {
        this._maxModelData.push(data);
    }

    public getStlData(): any[] {
        return this._maxModelData;
    }

    /**
     * 通过数据的层级信息得到数据
     * @param dataInfo 层级信息
     */
    public getData(dataInfo: DataInfo): MeshData | undefined {
        for (let i = 0; i < this._dataArray.length; ++i) {
            let data = this._dataArray[i];
            if (data.getDataInfo().equal(dataInfo)) {
                return data;
            }
        }
        return undefined;
    }
    /**
     * 是否存在数据
     * @param name 数据的名称
     */
    public hasData(dataInfo: DataInfo): boolean {
        let isExist = false;
        for (let i = 0; i < this._dataArray.length; ++i) {
            let data = this._dataArray[i];
            if (data.getDataInfo().equal(dataInfo)) {
                isExist = true;
                break;
            }
        }
        return isExist;
    }
    /**
     * 获得此数据集中的所有的数据的层级信息
     */
    public getDataInfos(): Array<DataInfo> {
        let dataInfos = new Array<DataInfo>();
        for (let i = 0; i < this._dataArray.length; ++i) {
            let data = this._dataArray[i];
            let dataInfo = data.getDataInfo();
            dataInfos.push(dataInfo);
        }
        return dataInfos;
    }

    /**
     * 设定当前数据集的激活对象
     * @param data 需要设定的对象
     * 后期可能会取消此方法
     */
    public setActiveData(data: MeshData | undefined) {
        if (data == undefined) {
            this._activeData = undefined;
            return;
        }
        let findIndex = this._dataArray.indexOf(data);
        if (findIndex == -1) {
            return;
        }
        this._activeData = data;
    }

    /**
     * 所有场景的数据集
     * @param data 
     */
    public setActiveAllData(data: MeshData | undefined) {
        if (data == undefined) {
            this._activeData = undefined;
            return;
        }
        let findIndex = this._dataArray.indexOf(data);
        if (findIndex == -1) {
            return;
        }
       this._activeAllData.push(data);
    }

    /**
     * 返回当前数据的激活对象
     */
    public getActiveData(): MeshData | undefined {
        return this._activeData;
    }

    /**
     * 返回所有数据
     */
    public getActiveAllData(): Array<MeshData> | undefined {
        return this._activeAllData;
    }

    public getStlActiveData(): THREE.Group[] {
        return this._maxModelData;
    }

    public getLevelData(): MeshData | undefined{
        // 先写个假的
        let stlIndex = -1;
        for (let i = 0; i < this._dataArray.length; ++i) {
            let data = this._dataArray[i];
            if (data.getObjectType() == 'STLData'){
                if (stlIndex == -1) {
                    stlIndex = this._dataArray.length-1;
                }
            }
        }
        
        if (stlIndex != -1) {
            return this._dataArray[stlIndex];
        }
        return undefined;
    }

    public getdataArray(){
        return this._dataArray;
    }

    public removeDataArray(num: number) {
        this._dataArray.splice(num, 1);
    }

    public clear() {
        this._dataArray = [];
    }

    /**
     * 获取顶点索引数据
     */
    public getGcodeIndexData(): any{
        return this._indexGcodeData;
    }

    public disposeActiveMesh(){
        this._maxModelData = [];
        this._indexGcodeData = {};
    }
}