import { StlDataCollection } from "../objects/StlDataCollection";
/**
 * 数据管理器。所有加载的数据都会被存放在数据管理器中，只有需要显示的数据，才会被添加到scene中
 */
export class DataManager {
    /**
     * 存储数据集的通容器
     */
    private _stlDataColArray: Array<StlDataCollection> = [];

    constructor() {}

    public addMaxDataCollection(dataCollection: StlDataCollection) {
        let findIndex = -1;
        this.getStlDataCollection(dataCollection.name, findIndex);
        // 没有重复名称的对象，直接添加
        if (findIndex == -1) {
            this._stlDataColArray.push(dataCollection);
        }
        // 出现相同名称的数据集时是直接将原来的数据替换呢？
        // 还是说出现了同名的数据集时直接抛一个异常，用户必须自己先删除同名数据集，再添加新的数据集？
        else {
            let removeArray = this._stlDataColArray.splice(findIndex, 1);
            let removeItem = removeArray[0];
            this._stlDataColArray.push(dataCollection);
        }
    }

    //add stl data
    public getStlDataCollection(name: string, findIndex?: number): StlDataCollection | undefined {
        //清空stl数据集
        this.clearStl();
        findIndex = -1;
        for (let i = 0; i < this._stlDataColArray.length; ++i) {
            let item = this._stlDataColArray[i];
            if (item.name == name) {
                findIndex = i;
                break;
            }
        }
        if (findIndex == -1) {
            return undefined;
        }
        let item = this._stlDataColArray[findIndex];
        return item;
    }
   
    public clearStl() {
        for (let i = 0; i < this._stlDataColArray.length; ++i) {
            let object = this._stlDataColArray[i];
            object.dispose();
            
        }
        this._stlDataColArray = [];
    }
}