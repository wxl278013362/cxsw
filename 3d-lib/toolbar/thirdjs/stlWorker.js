importScripts('./three.js', './STLLoader.js');
onmessage = function(e) {
    //通过请求获取数据
    onPosetMessage(e);
};

function onPosetMessage(e) {
    const message = e.data;
    if (message == undefined) return;

    var type = message.type;
    if (type == 'noParticlesLoaded') {
        var modelUrl = message.modelUrl;
        var modelSize = message.modelSize;
    
        let manager = new THREE.LoadingManager();
        let urlData = modelUrl;
    
        //@ts-ignore
        let loader = new THREE.STLLoader(manager);
        onLoader(loader, urlData);

    } else if (type == 'particlesLoaded') {
        let loader = new THREE.STLLoader();
        let downloadText = message.downloadText;
        let geometry = loader.parse(downloadText);
        //worker返回模型的数据
        postMessage(geometry);
    }
}

function onLoader(loader, urlData) {
    loader.load(urlData, (geometry)=> {
        //worker返回模型的数据
        postMessage(geometry);
    });
}
