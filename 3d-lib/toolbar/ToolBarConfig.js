var app, bb, dd
function testFunc1() {
    // window.TTS.EventControl.dispatchEvent('MSG_ON_MODEL_LOAD_STOP', null, 'all');
    window.TTS.EventControl.dispatchEvent('MSG_MODEL_DELETE', null, 'all');
    app = TTS.App.getInstance();
    bb = app.createScene('main_scene', 'model_view',{
        clearColor: 0x000000,
        sceneName: 'main_scene',
        
        //是否为分片加载
        // particlesLoaded: true,
        // boxHelper: true,
        // statsHelper: true
        //模型45度展示效果
        // pro3GridHelper: true
    });
    // var modelUrlArray1 = "./亭与塔.stl";
    var modelUrlArray1 = "./CE3_外壳.gcode";
    // var modelUrlArray1 = "http://172.23.208.174:9005/pfiles/Ender-3_Big-Ender3.cxcode";
    // var modelUrlArray1 = 'http://172.23.208.174:8003/settings/machine_file/?method=get_cxcode_stl_content&file_name=Ender-3_Big-Ender3.cxcode&file_path=/mnt/UDISK/.crealityprint/upload/Ender-3_Big-Ender3.cxcode';

    dataConfigArray = [
        {
            id: '2',
            dataType: 'gcode',
            // portType: 'qt',
            isActive: true,
            url: modelUrlArray1,    
            length: 1,
            chunkSize: 500000,
            modelSize: 14
        },
    ];
    var objectColss = app.createStlCollection('objCollection', dataConfigArray);
    bb.bindMaxDataCollection(objectColss);

    // var xhr = new XMLHttpRequest();
    // xhr.open('GET', "./toolbar/model/stl/stl.json", true);
    // // xhr.overrideMimeType('text/plain; charset=x-user-defined');
    // xhr.onreadystatechange = function() {
    //    if (xhr.readyState==4 && xhr.status==200) {
        
    //     data = this.responseText;
    //     geomData = JSON.parse(data); 
    //     console.log(geomData.data);
    //        dataConfigArray = [
    //         {
    //             id: '2',
    //             dataType: 'stl',
    //             portType: 'qt',
    //             isActive: true,  
    //             url: undefined, 
    //             data: geomData,
    //             length: 1,
    //         },
    //     ];
    
    //     var objectColss = app.createStlCollection('objCollection', dataConfigArray);
    //     bb.bindMaxDataCollection(objectColss);

    //    } 
    // };
    // // 发送数据请求
    // xhr.send();

    TTS.EventControl.addEvent('MSG_GET_MODEL_SIZE', (data)=> {
        console.log(data)
    }, 'UI');

    window.TTS.EventControl.addEvent('MSG_MODEL_URL_ERROR', (data)=>{
        console.log("加载错误")
    }, 'UI');

    window.TTS.EventControl.addEvent('MSG_LOAD_3DS_DATA_PROGRESS', (data)=>{
        if (data.loadedCount == data.requestCount) {
            console.log(data)
            // bb.getModelColorManager().setModelAlph(0.05, '1.2');
            var timeOut = setTimeout(function(){
                // var imgData = bb.getInteractionManager().onDrawingBuffer(true);
                // console.log(imgData)
                // var _canvas = document.getElementById('img');
                // _canvas.src = imgData;
                //@ts-ignore
                // var _tempContext = _canvas.getContext('2d');

                //绘制快照图片
                // var img = new Image();
                // img.src = imgData;
                // img.onload = function(){
                //     if (_tempContext == null) return;
                //     _tempContext.drawImage(img, 0, 0, 800, 1000);
                // }

                // console.log(img)
                // var arr = imgData.split(','), mime = arr[0].match(/:(.*?);/)[1],
                // bstr = atob(arr[1]), n = bstr.length, u8arr = new Uint8Array(n);
                // while(n--){
                //     u8arr[n] = bstr.charCodeAt(n);
                // }

                // console.log(new File([u8arr], "filename", {type:mime}));
                // console.log(new Blob([u8arr], {type:mime}));

                clearTimeout(timeOut);

            },100);
        }
    }, 'UI');


    // window.TTS.EventControl.dispatchEvent('MSG_MODEL_DELETE', null, 'all');
    // var app = TTS.App.getInstance();
    // var bb = app.createScene('main_scene', 'model_view',{
    //     // clearColor: '#000000',
    //     sceneName: 'main_scene',
    //     // boxHelper: true,
    // });
    

    // // window.scene = bb._scene;
    // var modelUrlArray1 = "./toolbar/model/stl/CE3_外壳.gcode";
    // dataConfigArray = [
    //     {
    //         id: '2',
    //         dataType: 'stl',
    //         isActive: true,
    //         url: modelUrlArray1,    
    //         length: 1,
    //     },
    // ];

    // var objectColss = app.createStlCollection('objCollection', dataConfigArray);
    // bb.bindMaxDataCollection(objectColss);

    // window.TTS.EventControl.addEvent('MSG_LOAD_3DS_DATA_PROGRESS', (data)=>{
    //     if (data.loadedCount == data.requestCount) {
    //         console.log("加载完成");
    //     }
    // }, 'UI');

}

function testFunc2() {

    // if (main != undefined || main != null) {
    //     bb = main;
    // }
    // window.TTS.EventControl.dispatchEvent('MSG_MODEL_DELETE', null, 'UI');
    // app = TTS.App.getInstance();
    // bb = app.createScene('main_scene', 'model_view',{
    //     clearColor: '#000000',
    //     // statsHelper: true,
    //     sceneName: 'main_scene',
    //     heatMapFlag: true,
    //     textureUrl: '../../toolbar/image/zuobiao.png',
    // });

    // var data = [
    //     -0.108, 
    //     0.001, 
    //     0.112, 
    //     0.22
    //     ,-0.108
    //     ,0.016
    //     ,0.116,
    //     0.231
    //     ,-0.327
    //     // ,-0.152
    //     // , -0.032
    //     // , 0.142
    //     // , -0.496
    //     // , -0.296
    //     // , -0.131
    //     // , 0.115
    //     // ,-0.950, -0.857, -0.595, 0.000, 0.001, 0.064, 1.033, 0.806, 0.658
    //     // ,-0.146, -0.067, 0.012, 0.091,-0.105, -0.020, 0.089, 0.141,-0.258
    //     ]
    // var data = [-0.850, -0.757, -0.595, 0.000, 0.001, 0.064, 0.033, 0.606, 0.658];
    // var data = [-1.146, -0.067, 0.012, 0.091,-1.105, -0.020, 0.089, 0.141,-0.258, -0.127, -0.003, 0.146,-0.324, -0.178, -0.015, 1.196]
    // var data = [-1.431, -0.237, 0.837, -1.093, 0.079, 1.274, -1.066, 0.032, 1.076];
    // var data = [
    //     0.245,
    //     -0.216,
    //     -0.661,
    //     1.384,
    //     0.008,
    //     -0.426,
    //     0.34,
    //     -0.012,
    //     -0.33
    // ]

    // var data = [
    //   0.923,
    //   -0.039,
    //   -0.876,
    //   -1.557,
    //   1.292,
    //   0.237,
    //   -0.633,
    //   -1.273,
    //   1.644,
    //   0.628,
    //   -0.202,
    //   -0.919,
    //   1.984,
    //   0.974,
    //   0.074,
    //   -0.613
    // ]

    // var data = [1.298, 0.892, 0.471, 0.327, -0.009, 0.840, 0.425, 0.163, -0.039, -0.153, 0.498, 0.205, -0.074, -0.132, -0.199, 0.344, 0.116, -0.043, -0.111, -0.039, 0.106, -0.006, -0.116, -0.023, 0.041]
    
    // TTS.EventControl.dispatchEvent('MSG_CREATE_HEATMAP_MODEL', {
    //     elementLeft: 41,
    //     elementTop: 287,
    //     threeData: data,
    //     type: 'portrait'
    // }, 'UI');
   
    // TTS.EventControl.addEvent('MSG_TOUCH_GET_COORDINATES', (data)=> {
    //     console.log(data)
    //     var _canvas = document.getElementById('img');

    // }, 'UI');

    // window.TTS.EventControl.dispatchEvent('MSG_MODEL_DELETE', null, 'all');
    // let app = TTS.App.getInstance();
    // let bb = app.createScene('main_scene', 'model_view',{
    //     clearColor: '#000000',
    //     // backgroundType: 'image',
    //     // backgroundUrl: '',
    //     // statsHelper: true,
    //     sceneName: 'main_scene',
    // });

    // var modelUrlArray1 = "./toolbar/model/stl/recorderv2.2.stl";
    // dataConfigArray = [
    //     {
    //         id: '2',
    //         dataType: 'stl',
    //         isActive: true, 
    //         url: modelUrlArray1,    
    //         length: 1,
    //     },
    // ];

    // var objectColss = app.createStlCollection('objCollection', dataConfigArray);
    // bb.bindMaxDataCollection(objectColss);

    // window.TTS.EventControl.addEvent('MSG_LOAD_3DS_DATA_PROGRESS', (data)=>{
    //     if (data.loadedCount == data.requestCount) {
    //         bb.getModelColorManager().setModelAlph(0.05);
    //         // console.log("222222222222")
    //     }
    // }, 'UI');

    // var ff = bb.getInteractionManager().onModelScale('x', 20, false);
 

    // bb.getInteractionManager().onModelRotate('y', 45);

    // var ff = bb.getInteractionManager().onModelScale('y', 50, false);
    // console.log(ff);
    // bb.getInteractionManager().onModelScale('x', 1050, true);
    // bb.getInteractionManager().onResetModel();

    // console.log(dd)
    // window.TTS.EventControl.dispatchEvent('MSG_MODEL_DELETE', null, 'all');

    //  let data = bb.getClipControlManager().getModelSize(2.49);
    // console.log(data);

    // var data = bb.getInteractionManager().onModelRotate('z', 30);
    bb.getInteractionManager().onResetModel();
    bb.onWindowResize();
}

function testFunc3(main) {
    if (main != undefined || main != null) {
        bb = main;
    }
    var modelId = {
        '6': true,
    };

    // window.TTS.EventControl.dispatchEvent('MSG_MODEL_DELETE', null, 'all');
    // app = TTS.App.getInstance();
    // bb = app.createScene('main_scene', 'model_view',{
    //     clearColor: '#000',
    //     sceneName: 'main_scene',
    //     // boxHelper: true,
    // });

    // // window.scene = bb._scene;
    // var modelUrlArray1 = "./toolbar/model/stl/youhua/Empty.stl";
    // dataConfigArray = [
    //     {
    //         id: '2',
    //         dataType: 'stl',
    //         isActive: true,
    //         url: modelUrlArray1,    
    //         length: 1,
    //     },
    // ];

    // var objectColss = app.createStlCollection('objCollection', dataConfigArray);
    // bb.bindMaxDataCollection(objectColss);

    // TTS.EventControl.addEvent('MSG_GET_MODEL_SIZE', (data)=> {
    //     console.log(data)
    // }, 'UI');


    // window.TTS.EventControl.addEvent('MSG_MODEL_URL_ERROR', (data)=>{
    //     console.log("加载错误")
    // }, 'UI');

    // window.TTS.EventControl.addEvent('MSG_LOAD_3DS_DATA_PROGRESS', (data)=>{
    //     if (data.loadedCount == data.requestCount) {
    //         console.log(bb);
    //         // bb.getModelColorManager().setIndexModelAlph(0.05);
    //     }
    // }, 'UI');
    
    // bb.getClipControlManager().operationClipModel(true, -0.8);
    var color = [255, 0.0, 0.0];
    // bb.getModelColorManager().setStlModelColor(color);
    // bb.getModelColorManager().setModelAlph(0.5);
    
    // 开始剖切操作
    // bb.getClipControlManager().operationClipModel(true, 0.5);

    // bb.getModelColorManager().setModelAlph("0.25");
    // var layer = [0.25, 0.4, 0.65, 0.8, 0.95, 1.1, 1.4, 47, 47.3, 47.6, 47.9, 47.15, 49.1, 48.65];
    // for (var key in layer) {
    //     TTS.EventControl.dispatchEvent('MSG_RENDER_MODEL_ALPH', {layer: layer[key]}, 'UI');
    // }

    // let progress = {progress: 0.1}
    // window.TTS.EventControl.dispatchEvent('MSG_RENDER_MODEL_PROGRESS', progress, 'UI');
    // bb.getModelColorManager().setIndexModelAlph({type:printer_state, data: {Z: 19.6}});
    // window.TTS.EventControl.dispatchEvent('MSG_WEBSOCKET_DATA', null, 'UI');

    //复位功能
    // let dd = bb.getInteractionManager().onResetModel();
    // console.log(dd)
    // bb.getInteractionManager().heatMapFov();

    // var dc = document.getElementById("model_view");
    // document.body.removeChild(dc);
    // bb.getHeatMapManager().createHeatMap();

    // window.TTS.EventControl.dispatchEvent('MSG_MODEL_DELETE', null, 'all');

    // bb.getHeatMapManager().three3DViewToTwoView();

    // app.FPS = 60;
    // app.getSceneManager().clear();

    // window.TTS.EventControl.dispatchEvent('MSG_MODEL_DELETE', null, 'all');

    // var modelUrlArray1 = "./toolbar/model/stl/Lowpolyring.stl";
    // dataConfigArray = [
    //     {
    //         id: '2',
    //         dataType: 'stl',
    //         isActive: true,
    //         url: modelUrlArray1,    
    //         length: 1,
    //     },
    // ];

    // var objectColss = app.createStlCollection('objCollection', dataConfigArray);
    // bb.bindMaxDataCollection(objectColss);
// 
    // window.TTS.EventControl.dispatchEvent('MSG_MODEL_DELETE', null, 'UI');
    // console.log(window.TTS.EventControl.getEvent());
    // console.log(window.TTS.EventControl.disposeEvent());
    // console.log(window.TTS.EventControl.getEvent());
    // console.log(bb);

    // bb.getInteractionManager().changeTwoViewFov();
  
    // bb.getInteractionManager().onModelRotate('x', 45);
    // bb.getInteractionManager().onResetModel();

    // bb.getInteractionManager().onModelScale('x', 103, true);
    // console.log(bb);
    // let data = bb.getInteractionManager().getModelMatrixWorld();
    // console.log(data);
    // // bb.getHeatMapManager().onHideAnyModel(false);
    // var data = bb.getInteractionManager().onModelScale('z', 0.5, false);
    // console.log(data)
    // console.log(bb)

    // bb.getInteractionManager().setModelMatrix(dd);
    // window.TTS.EventControl.dispatchEvent('MSG_RENDER_MODEL_LAYER_PRINTER', 2.7, 'UI');

    // var data = bb.getInteractionManager().onModelScale('all', 8.67, 1.2)
    // console.log(data);
    //  let data = bb.getClipControlManager().getModelSize(3.75);
    // console.log(data);

    var data = bb.getInteractionManager().onModelRotate('z', 30);
    bb.onWindowResize();
    let dataMatrix = bb.getInteractionManager().getModelMatrixWorld();
    console.log(dataMatrix);
}

function testFunc4(main) {
    // if (main != undefined || main != null) {
    //     bb = main;
    // }

    // var modelId = {
    //     '6': true,
    // };
    // // window.TTS.EventControl.dispatchEvent('MSG_ON_MODEL_LOAD_STOP', null, 'all');
    // window.TTS.EventControl.dispatchEvent('MSG_MODEL_DELETE', null, 'all');
    // app = TTS.App.getInstance();
    // bb = app.createScene('main_scene', 'model_view',{
    //     clearColor: 0x000000,
    //     sceneName: 'main_scene',
    //     boxHelper: true,
    //     // statsHelper: true
    //     // pro3GridHelper: true
    // });

    // // window.scene = bb._scene;
    // // var modelUrlArray1 = "http://172.28.1.116:9005/pfiles/aaaa.gcode";
    // // var modelUrlArray1 = "http://172.28.3.176/pfiles/20%E5%B0%8F%E7%8B%97.gcode";
    // // var modelUrlArray1 = "http://172.28.3.168/pfiles/phoneStand.gcode";
    // // var modelUrlArray1 = "http://172.28.3.168/pfiles/yuan.stl";
    // // var modelUrlArray1 = "http://172.28.0.56/pfiles/SuperBigery.gcode";

    // // var modelUrlArray1 = './toolbar/model/stl/dog-2.5H.gcode'
    // var modelUrlArray1 = "./toolbar/model/stl/luonv.stl";
    // // var modelUrlArray1 = "./toolbar/model/stl/Maotouying.gcode";
    // // var modelUrlArray1 = "http://172.28.0.165:9005/pfiles/11min_FK.gcode";
    // // var modelUrlArray1 = "http://172.28.0.165:9005/pfiles/yuan.stl";
    // // var modelUrlArray1 = "http://172.28.0.165:9005/pfiles/yuan.stl";
    
    // dataConfigArray = [
    //     {
    //         id: '2',
    //         dataType: 'stl',
    //         isActive: true,
    //         url: modelUrlArray1,    
    //         length: 1,
    //         chunkSize: 50000000
    //     },
    // ];

    // var objectColss = app.createStlCollection('objCollection', dataConfigArray);
    // bb.bindMaxDataCollection(objectColss);

    // TTS.EventControl.addEvent('MSG_GET_MODEL_SIZE', (data)=> {
    //     console.log(data)
    // }, 'UI');

    // window.TTS.EventControl.addEvent('MSG_MODEL_URL_ERROR', (data)=>{
    //     console.log("加载错误")
    // }, 'UI');

    // window.TTS.EventControl.addEvent('MSG_LOAD_3DS_DATA_PROGRESS', (data)=>{
    //     if (data.loadedCount == data.requestCount) {
    //         // bb.getModelColorManager().setModelAlph(0.01, '0');
    //     }
    // }, 'UI');


    // var data = bb.getInteractionManager().onModelScale('x',100, true);
    // console.log(data);

    // bb.getInteractionManager().changeTwoViewFov();
    // window.TTS.EventControl.dispatchEvent('MSG_RENDER_MODEL_LAYER_PRINTER', 'finished', 'UI');
    // bb.getClipControlManager().onCalculationModelSize({Xsize: 500, Ysize: 500, Zsize: 800})
    // var dd = bb.getInteractionManager().onModelRotate('x', 90);
    // console.log(dd);

    // var data = bb.getInteractionManager().onModelScale('z', 33.33, 8.88);
    // console.log(data);

    // var dd = bb.getInteractionManager().onModelRotate('x', 40);
    // console.log(dd);

    // var dd = bb.getInteractionManager().onModelRotate('y', 45);
    // console.log(dd);

    // var dd = bb.getInteractionManager().onModelRotate('x', 90);
    // console.log(dd);


    // window.TTS.EventControl.dispatchEvent('MSG_MODEL_DELETE', null, 'UI');
    // console.log(bb)

    // var data = bb.getInteractionManager().onModelRotate('x', 45);
    // console.log(data);
    // bb.getClipControlManager().onCalculationModelSize({Xsize: 200, Ysize: 200, Zsize: 200}, 2)

    // window.TTS.EventControl.dispatchEvent('MSG_RENDER_MODEL_LAYER_PRINTER', 0.3, 'UI');
    // num += 0.2;

    // clearInterval(time);

    // bb.getModelColorManager().setStlModelColor([0,1,0])
    // window.TTS.EventControl.dispatchEvent('MSG_RENDER_MODEL_LAYER_PRINTER', 4.16, 'UI');
    // bb.getModelColorManager().setModelAlph(1.0);

    var data = bb.getInteractionManager().onModelRotate('x', 60);
    bb.onWindowResize();

    let dataMatrix = bb.getInteractionManager().getModelMatrixWorld();
    console.log(dataMatrix);
}

var num = [
 0,
 0.2,
 0.4,
 0.6,
 0.8,
 1,
 1.2,
 1.4,
 1.6,
 1.8,
 2,
 2.2,
 2.4,
 2.6,
 2.8,
 3,
 3.2,
 3.4,
 3.6,
 3.8,
 4,
 4.2,
 4.4,
 4.6,
 4.8,
 5,
 5.2,
 5.4,
 5.6,
 5.8,
 6,
 6.2,
 6.4,
 6.6,
 6.8,
 7,
 7.2,
 7.4,
 7.6,
 7.8,
 8,
 8.2,
 8.4,
 8.6,
 8.8,
 9,
 9.2,
 9.4,
 9.6,
 9.8,
 10,
 10,
   ]
var time;
var index = 0;
function testFunc5(main) {
    if (main != undefined || main != null) {
        bb = main;
    }
    var modelId = {
        '6': true,
    };
    // window.TTS.EventControl.dispatchEvent('MSG_ON_MODEL_LOAD_STOP', null, 'all');
    // window.TTS.EventControl.dispatchEvent('MSG_MODEL_DELETE', null, 'all');
    // console.log(app);

    // var data = bb.getInteractionManager().onModelRotate('z', 60);
    // console.log(data);
    // bb.getInteractionManager().changeTwoViewFov();
    // bb.getSelectionManager().onDeleteModel();
    // window.TTS.EventControl.dispatchEvent('MSG_RENDER_MODEL_LAYER_PRINTER', 2.5, 'UI');
    // TTS.EventControl.dispatchEvent('MSG_Ajax_Event_ABORT', null, 'UI');
    // window.TTS.EventControl.dispatchEvent('MSG_MODEL_DELETE', null, 'UI');
    // var data = bb.getInteractionManager().onModelScale('x', 8.67, 14.34);
    // console.log(data);

    // var data = bb.getInteractionManager().onModelScale('x',8.67);
    // console.log(data);

    // bb.getInteractionManager().onResetModel();

    // var data = bb.getInteractionManager().onModelScale('y', 25);
    // console.log(data);

    // var data = bb.getInteractionManager().onModelScale('z',6.75);
    // console.log(data);

    // bb.getClipControlManager().onCalculationModelSize({Xsize: 400, Ysize: 150, Zsize: 405})

    // bb.getClipControlManager().onCalculationModelSize({Xsize: 200, Ysize: 200, Zsize: 200})

    // let data = bb.getClipControlManager().getModelSize('all');
    // console.log(data);

    // bb.getInteractionManager().setModelMatrix(data);
    // dd = bb.getInteractionManager().changeTwoViewFov();
    // console.log(dd);
    // window.TTS.EventControl.dispatchEvent('MSG_RENDER_MODEL_LAYER_PRINTER', 1, 'UI');
    // time = setInterval(function(){
    //     // num = parseFloat(num.toFixed(1))
    //     if(num[index] == 10) {
    //         // num = "finished";
    //         // window.TTS.EventControl.dispatchEvent('MSG_RENDER_MODEL_LAYER_PRINTER', 'finished', 'UI');
    //         window.TTS.EventControl.dispatchEvent('MSG_RENDER_MODEL_LAYER_PRINTER', num[index], 'UI');
    //         clearInterval(time);
    //     }
    //     window.TTS.EventControl.dispatchEvent('MSG_RENDER_MODEL_LAYER_PRINTER', num[index], 'UI');
    //     // console.log(num[index]);
    //     index += 1;
    // },500);
    // var color = [0,102,203];
    // bb.getModelColorManager().setStlModelColor(color)

    var data = bb.getInteractionManager().onModelScale('x', 0.6, 0.6);
    bb.onWindowResize();

    let dataMatrix = bb.getInteractionManager().getModelMatrixWorld();
    console.log(dataMatrix);

}