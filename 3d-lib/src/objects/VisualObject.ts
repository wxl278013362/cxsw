
import * as THREE from "three";
/**
 * 所有物体的基类
 */
export abstract class VisualObject extends THREE.Group {
    /**
     * 物体的类型
     */
    protected _objectType: string = 'VisualObject';
    /**
     * 得到物体的类型
     */
    public getObjectType(): string {
        return this._objectType;
    }
    /**
     * 每个数据的名称
     */
    // public name: string;
    /**
     * 是否是VisualObject
     */
    public isVisualObject: boolean = true;
    /**
     * @param config 配置文件
     */
    constructor() {
        super();
    }
    /**
     * 释放当前对象
     */
    public dispose() {
        while (this.children.length > 0) {
            let child = this.children[0];
            this.remove(child);
        }
    }
}