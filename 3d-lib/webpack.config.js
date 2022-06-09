
const path = require('path');
const webpack = require('webpack');
const CleanWebpackPlugin = require('clean-webpack-plugin');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const CopyWebpackPlugin = require('copy-webpack-plugin');

//配置

//混淆代码的插件
// const UglifyJSPlugin = require('uglifyjs-webpack-plugin')
module.exports = {
    // 指明webpack的工作路径
    context: __dirname,
    // 入口，即从谁开始编译并逐渐添加引用。可以是多个
    entry: {
        'interface': path.resolve(__dirname, './src/index.ts')
    },
    output: {
        path: path.resolve(__dirname, './dist'),
        // path: path.resolve(__dirname, '/../../3D_React/src/static/js'),
        
        // 当入口有多个时，filename可以改为[name].js用于定义不同文件构建后的名字
        // filename: 'threets.js',
        filename: '[name].js',
        library: 'TTS',
        // libraryTarget: 'umd'
        libraryTarget: 'window'
    },
    // resolve: {
    //     extensions: [".tsx", '.ts', ".js", ".css"]
    // },
    // 模式，development or production
    //mode: "development",

    // 开发模式不压缩打包后代码，生产模式压缩打包后代码
    mode: 'development',
    // 生成map，便于调试
    //产生单独的文件,显示行和列
    // devtool: "source-map",
    // devtool: 'inline-source-map',

    module: {
        rules: [
            {
            test: /\.tsx?$/,
            loader: "ts-loader",
            exclude: /node_modules/
        },
      ]
    },
    resolve: { 
        extensions: [".web.ts", ".web.js", ".ts", ".js", "tsx", "jsx",".es6"],
        alias: {
            'three-examples': path.join(__dirname, './node_modules/three/examples/js'),
            // 'vue': 'vue/dist/vue.esm.js'
        }
    },
    devServer: {
        contentBase: path.join(__dirname, "dist"),
        compress: true,
        open: true,
        host: 'localhost',
    },
    performance: {
        hints:'warning',
        //入口起点的最大体积
        maxEntrypointSize: 50000000,
        //生成文件的最大体积
        maxAssetSize: 30000000,
        //只给出 js 文件的性能提示
        assetFilter: function(assetFilename) {
            return assetFilename.endsWith('.js');
        }
    },

    plugins: [
        //打包并时时清除历史数据
        // BrowserSyncPluginConfig,
        new CleanWebpackPlugin([
            path.resolve(__dirname, './dist/**/*.js'),
            path.resolve(__dirname, './dist/**/*.map'),
            path.resolve(__dirname, './dist/**/*.html'),
            path.resolve(__dirname, './dist/**/*.css'),
            path.resolve(__dirname, './build/**/*.ts')
        ]),
        new HtmlWebpackPlugin({
            title: "测试",
            template: "test/index.tmpl.html",// 源模板文件
            filename: "./index.html",//路径相对于output.path而言
            inject: "head",// 可为bool或者string，head or body
            chunks: ["interface"],//和entry对应，需要注入的块
        }),

        //复制静态的js文件到指定的位置
        new CopyWebpackPlugin([{
            from: 'toolbar/**/*',
            to: '',
            toType: 'dir'
        }]),

        //混淆js文件的代码
        // new UglifyJSPlugin({
        //     compress: {
        //       // 在UglifyJs删除没有用到的代码时不输出警告
        //       warnings: false,
        //       // 删除所有的 `console` 语句，可以兼容ie浏览器
        //       drop_console: true,
        //       // 内嵌定义了但是只用到一次的变量
        //       collapse_vars: true,
        //       // 提取出出现多次但是没有定义成变量去引用的静态值
        //       reduce_vars: true,
        //     },
        //     output: {
        //       // 最紧凑的输出
        //       beautify: false,
        //       // 删除所有的注释
        //       comments: false,
        //     },
        //     test: /interface/i,
        // }),
    ]
}