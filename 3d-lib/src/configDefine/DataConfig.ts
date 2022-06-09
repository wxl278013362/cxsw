export interface DataConfig {
    /**
     * 数据的id，即数据的唯一标识符
     */
    id: string;
    /**
     * 该数据的父节点Id
     */
    parentId?: string;
    /**
     * 数据的名称
     */
    name: string;
    /**
     * 数据的类型；根据数据类型的不同，将对应不同的解析方式
     */
    // dataType: string;
    /**
     * 数据的url地址
     */
    url: any;
    /**
     * 材质url
     */
    mtlUrl: string;
    /**
     * 数据整体的偏移量
     */
    position?: Array<number>;
    /**
     * 数据整体的旋转量
     */
    rotation?: Array<number>;
    /**
     * 数据整体的缩放量
     */
    scale?: Array<number>;

    length?: number;

    dataType: string;

    isActive: boolean
}