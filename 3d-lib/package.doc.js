var packageconfig = {
    // 因为有了webpack，所以很多设置在webpack中声明。此文件需要的属性就很少了
    // 工程的名称
    "name": "babylonjs-gui",
    // 版本
    "version": "1.0.0",
    // 描述
    "description": "The js GUI library is an extension you can use to generate interactive user interface.\r It is build on top of the DynamicTexture.",
    // 脚本。方便调试
    "scripts": {
        "start:server": "webpack-dev-server",
        "start:watch": "webpack -w",
        "build:dev": "webpack",
        "build:prod": "webpack --mode=production",
        "test": "echo \"Error: no test specified\" && exit 1"
    },
    // 地址。可不设置
    "repository": {
        "type": "git",
        "url": "git+https://github.com/BabylonJS/Babylon.js.git"
    },
    // 关键词。便于在npm中搜索到
    "keywords": [
        "3d",
        "webgl",
        "viewer"
    ],
    // 授权。 ISC，MIT，Apache-2.0，UNLICENSED。不想公开的项目设置为UNLICENSED
    "license": "Apache2",
    // 反馈bug的地址
    "bugs": {
        "url": "https://github.com/BabylonJS/Babylon.js/issues"
    },
    // 主页地址
    "homepage": "https://github.com/BabylonJS/Babylon.js#readme",
    // 开发环境。可使用npm install安装
    "devDependencies": {
        "@types/node": "^10.5.2",
        "clean-webpack-plugin": "^0.1.19",
        "ts-loader": "^4.0.0",
        "typescript": "~3.0.1",
        "webpack": "^4.16.0",
        "webpack-cli": "^3.0.8",
        "webpack-dev-server": "^3.1.4",
        "webpack-glsl-loader": "^1.0.1"
    },
    // 运行环境。可使用npm install安装
    "dependencies": {
        "babylonjs": "^3.3.0",
        "dts-bundle-webpack": "^1.0.1"
    }
}
