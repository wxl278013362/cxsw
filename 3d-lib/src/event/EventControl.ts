/**
 * 想做一个事件的整理，将所有的事件都放在一起
 */
export enum Event3D {
    onModelPosition                     =               'MSG_MODEL_POSITION_COUNT',
    onModelPositionNew                  =               'MSG_MODEL_POSITION_COUNT_NEW',
    onNextModel                         =               'MSG_NEXT_MODEL',
    onNextLoadModel                     =               'MSG_NEXT_LOAD_MODEL',
    onChangeModelFov                    =               'MSG_CHANGE_MODEL_FOV',
    onValueClip                         =               'MSG_MODEL_CLIP_VALAUE',
    onDeleteModel                       =               'MSG_MODEL_DELETE',
    onRenderModelLayer                  =               'MSG_RENDER_MODEL_LAYER_PRINTER',
    onRenderModelProgress               =               'MSG_RENDER_MODEL_PROGRESS',
    onRenderModelColor                  =               'MSG_RENDER_MODEL_LAYER_COLOR',
    onModelUrlError                     =               'MSG_MODEL_URL_ERROR',
    onCreateHeatMapModel                =               'MSG_CREATE_HEATMAP_MODEL',
    onGetVertexData                     =               'MSG_GET_VERTEX_DATA',
    onStartAnimationFrame               =               'MSG_START_ANIMATION_FRAME',
    onLoadModel                         =               'MSG_LOAD_3DS_DATA_PROGRESS',
    onTouchGetCoordinates               =               'MSG_TOUCH_GET_COORDINATES',
    onAjaxEventAbort                    =               'MSG_Ajax_Event_ABORT',
    onGetModelSize                      =               'MSG_GET_MODEL_SIZE',
    onModelLoadFinished                 =               'MSG_LOAD_3DS_DATA_FINISH',
    onModelZoom                         =               'MSG_ON_MODEL_ZOOM',
    onModelLoadStop                     =               'MSG_ON_MODEL_LOAD_STOP',
};
/**
 * 事件控制
 */
export class EventControl {
    /**
     * 自定义事件控制器实体
     */
    private static _listeners: {
        [msg: string]: {
            [role: string] : Array<Function>
        }
    } = {};

    /**
     * 添加自定义的消息事件
     * @param msg 消息事件的名称
     * @param callback 回调函数
     * @param role 创建事件的角色，如'3D'
     */
    public static addEvent(msg: string, callback: (data: any) => void, role: string) {
        if (this._listeners[msg] == undefined) {
            this._listeners[msg] = {};
        }
        if (this._listeners[msg][role] == undefined) {
            this._listeners[msg][role] = [];
        }
        if (this._listeners[msg][role].indexOf(callback) == -1) {
            this._listeners[msg][role].push(callback);
        }
    }

    /**
     * 监测是否已将侦听器添加到事件类型
     * @param msg 事件类型
     * @param callback 侦听器
     * @param role 创建事件的角色，如'3D'
     */
    public static hasEvent(msg: string, callback: (data: any) => void, role: string): boolean{
        if (this._listeners == undefined ) {
            return false;
        }
        let listeners = this._listeners;
        if (listeners[msg] == undefined) {
            return false;
        }
        if (listeners[msg][role] == undefined) {
            return false;
        }
        // let findIndex = listeners[msg][role].indexOf(callback);
        // if (findIndex == -1) {
        //     return false;
        // }

        let findIndex = listeners[msg][role]
        if (findIndex == undefined) {
            return false;
        }
        return true;
    }

    /**
     * 从事件类型中删除侦听器
     * @param msg 事件类型
     * @param callback 侦听器
     * @param role 创建事件的角色，如'3D'
     */
    public static removeEvent(msg: string, callback: (data: any) => void, role: string) {
        if (this.hasEvent(msg, callback, role)) {
            let listenerArray = this._listeners[msg][role];
            // let index = listenerArray.indexOf(callback);
            // if (index != -1) {
            //     listenerArray.splice(index, 1);
            // }

            //删除事件
            for (let i=0; i<listenerArray.length; ++i) {
                listenerArray.splice(i, 1);
            }
        }
    }

    /**
     * 清空已注册事件
     */
    public static disposeEvent() {
        this._listeners = {};
    }

    /**
     * 触发事件类型
     * @param msg 事件类型
     * @param argjObejct 参数
     * @param fromRole 发送者
     * @param toRoles 接收者
     */
    public static dispatchEvent(msg: string, argjObejct: any, fromRole: string, toRoles?: Array<string>) {
        if (this._listeners == undefined) {
            return;
        }
        if (this._listeners[msg] == undefined) {
            return;
        }
        if (toRoles == undefined) {
            let roleListenerArray = this._listeners[msg];
            for (let role in roleListenerArray) {
                // 内部使用函数调用，跨模块才能使用事件驱动
                if (role == fromRole) {
                    continue;
                }
                let listenerArray = roleListenerArray[role];
                if (listenerArray != undefined) {
                    let array = listenerArray.slice(0);
                    for (let i = 0; i < array.length; ++i) {
                        let callback = array[i];
                        if (callback) {
                            callback.call(this, argjObejct);
                        }
                        else {
                            // console.log('3d, error, empty callback function, msg:' + msg + ',role:' + role);
                        }
                    }
                }
            }
        }
        else {
            let roleListenerArray = this._listeners[msg];
            for (let role in roleListenerArray) {
                // 内部使用函数调用，跨模块才能使用事件驱动
                if (role == fromRole) {
                    continue;
                }
                // 只对目标对象role释放事件
                let findIndex = toRoles.indexOf(role);
                if (findIndex == -1) {
                    continue;
                }
                let listenerArray = roleListenerArray[role];
                if (listenerArray != undefined) {
                    let array = listenerArray.slice(0);
                    for (let i = 0; i < array.length; ++i) {
                        let callback = array[i];
                        if (callback) {
                            callback.call(this, argjObejct);
                        }
                        else {
                            // console.log('3d, error, empty callback function, msg:' + msg + ',role:' + role);
                        }
                    }
                }
            }
        }
    }
}