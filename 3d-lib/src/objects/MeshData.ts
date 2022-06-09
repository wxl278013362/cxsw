import { VisualObject } from "./VisualObject";
/**
 * 数据层级定义。我们限定数据层级一共分三层，工程级别，单建筑级别，楼层级别
 */
export class DataInfo {
   
    public id: string;

    public name: string;

    public parentId: string | undefined;

    public objectType: string;

    public isActive: boolean;

    constructor(id: string, parentId: string| undefined, name: string, objectType: string, isActive: boolean) {
      
        this.id = id;
        this.name = name;
        this.parentId = parentId;
        this.objectType = objectType;
        this.isActive = isActive;
    }
 
    /**
     * 是否相等
     */
    public equal(other: DataInfo): boolean {
        return (this.id == other.id);
    }
}
/**
 * 所有场景数据的基类
 */
export abstract class MeshData extends VisualObject {
 
    /**
     * 是否是MeshObject
     */
    public isMeshObject: boolean = true;
    /**
     * 数据的层级信息
     */
    protected _dataInfo: DataInfo;

    constructor(id: string, parentId: string | undefined, name: string, objectType: string, isAcrive: boolean) {
        super();
        this._objectType = 'MeshData';
        this._dataInfo = new DataInfo(id, parentId, name, objectType, isAcrive);
    }
    /**
     * 释放当前对象
     */
    public dispose() {
        super.dispose();
    }
    /**
     * 获得数据定义
     */
    public getDataInfo(): DataInfo {
        return this._dataInfo;
    }
   
}