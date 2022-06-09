onmessage = function(e) {
    //通过请求获取数据
    onPosetMessage(e);
};

function onPosetMessage(e) {
    const message = e.data;
    if (message == undefined) return;
    var type = message.type;
    if (type == 'noParticlesLoaded') {
        var url = message.modelUrl;
        var modelSize = message.modelSize;
        var xhr = new XMLHttpRequest();
        xhr.open('GET', url, true);
        // xhr.overrideMimeType('text/plain; charset=x-user-defined');
        xhr.onreadystatechange = function() {
            if (xhr.readyState == 4 && xhr.status == 200) {
               var data = parse(this.responseText, modelSize);
               postMessage(data);
            } 
        };
        // 发送数据请求
        xhr.send();

    } else if (type == 'particlesLoaded') {
        var modelSize = message.modelSize;
        var downloadText = message.downloadText;
        var data = parse(downloadText, modelSize);
        postMessage(data);
    }
}

function parse(data, modelSize) {
    //后端打印Log
    // fnLog(url, 'start');

    var state = { x: 0, y: 0, z: 0, e: 0, f: 0, extruding: false, relative: false };
    var layers = [];

    var teLayer = [];
    var currentLayer = undefined;
    function newLayer( line ) {

        currentLayer = { vertex: [], z: line.z };
        layers.push( currentLayer );
    }

    //Create lie segment between p1 and p2
    function addSegment( p1, p2 ) {
        if ( currentLayer === undefined ) {
            newLayer( p1 );
        }

        if (line.extruding) {
            teLayer.push(p1.x, p1.y, p1.z);
            teLayer.push(p2.x, p2.y, p2.z);

            currentLayer.vertex.push( p1.x, p1.y, p1.z);
            currentLayer.vertex.push( p2.x, p2.y, p2.z);
        } 
    }

    function delta( v1, v2 ) {

        return state.relative ? v2 : v2 - v1;
    }

    function absolute( v1, v2 ) {

        return state.relative ? v1 + v2 : v2;
    }

    var lines = data.replace( /;.+/g, '' ).split( '\n' );
    for ( var i = 0; i < lines.length; i ++ ) {

        var tokens = lines[ i ].split( ' ' );
        var cmd = tokens[ 0 ].toUpperCase();

        //Argumments
        var args = {};
        tokens.splice( 1 ).forEach( function ( token ) {

            if ( token[ 0 ] !== undefined ) {

                var key = token[ 0 ].toLowerCase();
                var value = parseFloat( token.substring( 1 ) );
                args[ key ] = value;
            }
        } );

        //Process commands
        //G0/G1 – Linear Movement
        // F打印头移动速度
        // E出丝速度
        if ( cmd === 'G0' || cmd === 'G1' ) {

            if (lines[i].indexOf("X10.") == -1){
                // console.log(lines[i]);
                var line = {
                    x: args.x !== undefined ? absolute( state.x, args.x ) : state.x,
                    y: args.y !== undefined ? absolute( state.y, args.y ) : state.y,
                    z: args.z !== undefined ? absolute( state.z, args.z ) : state.z,
                    e: args.e !== undefined ? absolute( state.e, args.e ) : state.e,
                    f: args.f !== undefined ? absolute( state.f, args.f ) : state.f,
                };

                //Layer change detection is or made by watching Z, it's made by watching when we extrude at a new Z position
                if ( delta( state.e, line.e ) > 0 ) {

                    line.extruding = delta( state.e, line.e ) > 0;

                    if ( currentLayer == undefined || line.z != currentLayer.z ) {
                        newLayer( line );
                    }
                }

                addSegment( state, line );
                state = line;
            }

        } else if ( cmd === 'G90' ) {

            //G90: Set to Absolute Positioning
            state.relative = false;

        } else if ( cmd === 'G91' ) {

            //G91: Set to state.relative Positioning
            state.relative = false;

        } else if ( cmd === 'G92' ) {
            // G92: Set Position
            var line = state;
            line.x = args.x !== undefined ? args.x : line.x;
            line.y = args.y !== undefined ? args.y : line.y;
            line.z = args.z !== undefined ? args.z : line.z;
            line.e = args.e !== undefined ? args.e : line.e;
            state = line;
        }
    }

    //构建数据创建模型
    //全局gcode数据数组
    var _vertexs = [];
    var _colors = [];
    var _alphs = [];

    var _indexGcodeData = {};
    var _tempDate = {};
    var startIndex = 0;
    var endIndex = 0;
    var flagTag = 0;

    //判断模型文件大小设置抽层标准
    var tModelSize = 1;
    if (modelSize == undefined) {
        if (teLayer.length > 100000) {
            tModelSize = 5;
        } else {
            tModelSize = 1;
        }
        teLayer = [];
    }
    
    if (0 < modelSize && modelSize <= 10) {
        if (teLayer.length > 100000) {
            tModelSize = 5;
        } else {
            tModelSize = 1;
        }
        teLayer = [];

    } else if (10 < modelSize && modelSize <= 50) {
        tModelSize = 10
    } else if (50 < modelSize && modelSize <= 100) {
        tModelSize = 15
    } else if (100 < modelSize && modelSize <= 200) {
        tModelSize = 20
    } else if (200 < modelSize && modelSize <= 300) {
        tModelSize = 25
    } else if (300 < modelSize && modelSize <= 500) {
        tModelSize = 30
    }else if (modelSize > 500) {
        tModelSize = 40
    }

    //用于临时判断模型的定点数据
    teLayer = [];

    //判断模型的层级是否按升序排列
    for (var k = 0; k < 5; ++k) {
        var flag = parseFloat(layers[k].z) < parseFloat(layers[k+1].z);
        if (flag) {
            flagTag = 0;
        } else {
            flagTag = 2;
            break;
        }
    }
    
    //遍历模型的顶点数据
    for ( var i = flagTag; i < layers.length; i ++ ) {
        var layerVertex = [];
        //抽层代码
        if (i == flagTag) {
            var layer = layers[ i ];
            layerVertex = layer.vertex;
            var z = layer.z;
            
            for ( var j = 0; j < layerVertex.length; j ++ ) {
                _vertexs.push(layerVertex[ j ]);
                _colors.push(0.48);
            
                if (j%6 == 0) {
                    _alphs.push(1.0);
                } 
            }
        } else if (i%tModelSize == 0) {
            var layer = layers[ i ];
            layerVertex = layer.vertex;
            var z = layer.z;
            
            for ( var j = 0; j < layerVertex.length; j ++ ) {
                _vertexs.push(layerVertex[ j ]);
                _colors.push(0.48);
            
                if (j%6 == 0) {
                    _alphs.push(1.0);
                } 
            }
        }

        if (i == layers.length-1) {
            var layer = layers[ i ];
            layerVertex = layer.vertex;
            var z = layer.z;
            
            for ( var k = 0; k < layerVertex.length; k ++ ) {
                _vertexs.push(layerVertex[ k ]);
                _colors.push(0.48);
            
                if (k%6 == 0) {
                    _alphs.push(1.0);
                } 
            }
        }

        //计算每个层级的数据长度
        var tempLen = layerVertex.length/3;
        endIndex += tempLen;
        _tempDate = {
            start: startIndex,
            end: endIndex
        }

        //判断字典并构建字典数据
        var findIndex = _indexGcodeData.hasOwnProperty(z);
        if (!findIndex) {
            _indexGcodeData[z] = _tempDate;
        } 
        startIndex = endIndex;
    }

    //构建数据对象
    var objData = {
        vert: _vertexs,
        color: _colors,
        alph: _alphs,
    }
    _vertexs = null;
    _colors = null;
    _alphs = null;

    layers = null
    layerVertex = null
    currentLayer.vertex = null
    currentLayer = undefined;

    data = null;
    lines = [];
    // console.log({dataObj: objData, data: _indexGcodeData});
    return {dataObj: objData, data: _indexGcodeData}
}


