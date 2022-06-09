import { Scene3D } from "./Scene3D";
import { Scene3DManager } from "../managers/Scene3DManager";
import { DataManager } from "../managers/DataManager";
import { DataConfig } from "../configDefine/DataConfig";
import { SceneConfig } from "../configDefine/SceneConfig";
import { StlDataCollection } from "../objects/StlDataCollection";
/**
 * app就是一个window页面上的3D实例，只能被创建一次。
 * 它管理绘制场景的工具Scene3D和绘制的原料Object
 */
export class App {
    /**
     * 一个window页面中对应的唯一app
     */
    private static _appInstance: App | undefined = undefined;

    private _config: any = undefined;

    private _temScene3D: any = undefined;

    /**
     * 使用一个静态函数确保一个页面只能有一个app
     */
    public static getInstance(): App {
        if (this._appInstance == undefined) {
            this._appInstance = new App();
        }
        return this._appInstance;
    }
    /**
     * 场景管理器
     */
    private _sceneManager: Scene3DManager | undefined | null;
    /**
     * 数据管理器
     */
    private _dataManager: DataManager | undefined | null;
    /**
     * 将构造函数设置为private避免在外部实例化
     */
    constructor() {
        this._sceneManager = new Scene3DManager();
        this._dataManager = new DataManager();
    }
    /**
     * 得到数据集管理器(内存)
     */
    public getDataManager(): DataManager | undefined | null{
        return this._dataManager;
    }
    /**
     * 得到场景管理器
     */
    public getSceneManager(): Scene3DManager | undefined| null{
        return this._sceneManager;
    }
    /**
     * 创建一个场景
     * @param name 场景的名称
     * @param containerId 场景的容器id
     * @param config 场景的配置文件
     */
    public createScene(name: string, containerId: string, config?: SceneConfig): Scene3D | undefined | null {
        this._config = config;
     
        if (this._sceneManager != undefined) {
            let scene3D = this._sceneManager.getScene3D(name);
            
            if (scene3D) {
                if (scene3D.getContainerId() == containerId) {
                    return scene3D;
                }
                this._sceneManager.removeScene3D(name);
            }
            //通过名字创建3D场景
            scene3D = new Scene3D(name, containerId, config);

            //添加临时场景
            this._temScene3D = scene3D;

            this._sceneManager.addScene3D(scene3D);
            return scene3D;
        }
        return;
    }
    /**
     * 得到场景
     * @param name 场景的名称
     */
    public getScene(name?: string): Scene3D | undefined | null{
        if (this._sceneManager == undefined) return;
        if (name == undefined) {
            return this._sceneManager.getScene3DByIndex();
        }
        return this._sceneManager.getScene3D(name);
    }
    /**
     * 根据数据配置文件创建一个数据集
     * @param dataConfigArray 数据配置文件。数组
     */
    public createStlCollection(name: string, dataConfigArray: Array<DataConfig>) {
        if (this._dataManager == undefined) return;
        //保存模型数据信息
        this._temScene3D._modelData = dataConfigArray[0];

        let dataCollection = this._dataManager.getStlDataCollection(name);
        if (dataCollection) {
            return dataCollection;
        }
        
        dataCollection = new StlDataCollection(name, this._config, this._temScene3D);
        dataCollection.loadData(dataConfigArray);
        
        // 添加到内存管理
        this._dataManager.addMaxDataCollection(dataCollection);
        return dataCollection;
    }

    /**
     * 释放3D
     */
    public dispose() {
        if (this._dataManager == undefined || this._sceneManager == undefined) return;
        this._sceneManager.clear();
        // this._dataManager.clear();
        this._dataManager.clearStl();
    }
    /**
     * 整个页面的渲染入口
     */
    public render() {
        if (this._sceneManager == undefined) return;
        let scene3DNameArray = this._sceneManager.getScene3DNameList();
        for (let i = 0; i < scene3DNameArray.length; ++i) {
            let scene3D = this._sceneManager.getScene3D(scene3DNameArray[i]);
            if (scene3D == undefined) {
                continue;
            }
            scene3D.render();
        }
    }
}