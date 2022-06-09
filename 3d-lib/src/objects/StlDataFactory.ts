//解压缩插件
import { DataConfig } from "../configDefine/DataConfig";

import { StlDataCollection } from "./StlDataCollection";
import { STLData } from "./STLData";
import { gcodeDatas } from "./gcodeDatas";
import { MeshPlyData } from "./MeshPlyData";

/**
 * 数据工厂类，专门用于加载数据。每一个数据工厂都对一个数据集负责，当
 * 数据工厂完成数据处理的工作后，会将数据成品添加到数据集中
 */
export class StlDataFactory {
    /**
     * 当前数据工厂绑定的数据集
     */
    private _objectCollection: StlDataCollection;
    /**
     * 保存数据集的配置文件
     */
    private _config: DataConfig;
    private _objectData: any;

    private _tConfig: any = undefined;

    private _temScene3D: any;

    constructor(objectCollection: StlDataCollection, config: DataConfig, tConfig: any, temScene3D: any) {
        this._objectCollection = objectCollection;
        this._config = config;
        this._tConfig = tConfig;

        this._temScene3D = temScene3D;
    }

    public onStlData(urlConfig: any) {
        this._objectCollection.createLoadTask();
        let self = this;
        let urlData = urlConfig;
        let dataType = urlData.dataType;
        let url = urlData.url;
        
        switch (dataType) {
            case "stl": 
                this._objectData = new STLData(urlConfig.id, urlConfig.parentId, urlConfig.name, urlConfig.isActive, urlConfig, this._tConfig, this._temScene3D);
                this._objectData.onLoaderStl(url, self);
                break;

            case "ply": 
                this._objectData = new MeshPlyData(urlConfig.id, urlConfig.parentId, urlConfig.name, urlConfig.isActive, urlConfig, this._tConfig);
                this._objectData.onLoaderPly(url, self);
                break;

            case "gcode": 
                this._objectData = new gcodeDatas(urlConfig.id, urlConfig.parentId, urlConfig.name, urlConfig.isActive, urlConfig, this._tConfig, this._temScene3D );
                this._objectData.onLoaderGcode(url, self);
                
                break;

            default:
                break;
        }
    }
}