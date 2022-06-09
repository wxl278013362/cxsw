import { Scene3D } from "../core/Scene3D";
/**
 * 场景管理器
 */
export class Scene3DManager {
    /**
     * 所有场景的管理器
     */
    private _scene3DArray: Array<Scene3D> = [];

    /**
     * 添加场景
     * @param scene3D 
     */
    public addScene3D(scene3D: Scene3D) {
        // 确保名称唯一
        this.removeScene3D(scene3D.name);
        // 添加到场景管理器中
        this._scene3DArray.push(scene3D);
        // 释放事件
        // EventControl.dispatchEvent('MSG_SCENE3D_ADD', scene3D, scene3D.name);
    }

    /**
     * 通过名称删除场景
     * @param name 场景的名称
     */
    public removeScene3D(name: string): Scene3D | undefined {
        let findIndex = -1;
        this.getScene3D(name, findIndex);
        if (findIndex == -1) {
            return undefined;
        }
        let removeItemArray = this._scene3DArray.splice(1, findIndex);
        let removeItem = removeItemArray[0];
        // 释放事件
        // EventControl.dispatchEvent('MSG_SCENE3D_REMOVE', removeItem, name);
        return removeItem;
    }

    /**
     * 通过名称得到场景
     * @param name 场景的名称
     * @param findIndex 得到的场景的索引
     * @returns 若存在，则返回场景；若不存在，则返回undefined
     */
    public getScene3D(name: string, findIndex?: number): Scene3D | undefined {
        if (findIndex == undefined) {
            findIndex = -1;
        }
        for (let i = 0; i < this._scene3DArray.length; ++i) {
            if (this._scene3DArray[i].name == name) {
                findIndex = i;
                break;
            }
        }
        if (findIndex == -1) {
            return undefined;
        }
        return this._scene3DArray[findIndex];
    }

    /**
     * 获取当前激活场景
     * @param name 场景名称
     */
    public getActiveScene3D(name: string): Scene3D | undefined {
        for (let i = 0; i < this._scene3DArray.length; ++i) {

            if (this._scene3DArray[i].name == name) {
                this._scene3DArray = [];
            }
        }
        return undefined;
    }

    /**
     * 通过索引来得到场景
     * @param index 索引
     */
    public getScene3DByIndex(index?: number): Scene3D | undefined {
        if (index == undefined) {
            index = 0;
        }
        if (index > this._scene3DArray.length - 1) {
            return undefined;
        }
        return this._scene3DArray[index];
    }
    
    /**
     * 得到所有场景的名称
     */
    public getScene3DNameList(): Array<string> {
        let sceneNameList = Array<string>();
        if (this._scene3DArray.length == 0) {
            return sceneNameList;
        }
        for (let i = 0; i < this._scene3DArray.length; ++i) {
            sceneNameList.push(this._scene3DArray[i].name);
        }
        return sceneNameList;
    }

    public clear() {
        for (let i = 0; i < this._scene3DArray.length; ++i) {
            let scene3D = this._scene3DArray[i];
            scene3D.dispose();
        }
        this._scene3DArray = [];
    }

    public activeScene(name: string) {
        for (let i = 0; i < this._scene3DArray.length; ++i) {
            if (this._scene3DArray[i].name != name) {
                this._scene3DArray[i].enable = false;
            } else {
                this._scene3DArray[i].enable = true;
            }
        }
    }
}