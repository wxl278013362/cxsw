var tsconfig = {
    // tsconfig.json文件详细配置说明：https://www.tslang.cn/docs/handbook/compiler-options.html
    // compilerOptions:编译选项,可以被忽略，这时编译器会使用默认值
    "compilerOptions": {
        // 以严格模式解析并为每个源文件生成 "use strict"语句。默认false
        "alwaysStrict": true,
        // 解析非相对模块名的基准目录，在使用paths时必须设置baseUrl
        "baseUrl": "./src",
        "paths": {//模块名到基于baseUrl的路径映射的列表
        },
        // 输入文件的字符集。
        // "charset": "utf8",
        // 生成相应的 .d.ts文件。默认false
        "declaration": true,
        // 生成声明文件的输出路径
        // "declarationDir": "XXX",
        // 给源码里的装饰器声明加上设计类型元数据。默认false
        "emitDecoratorMetadata": true,
        // 启用实验性的ES装饰器。默认false
        "experimentalDecorators": true,
        // 编译过程中需要引入的库文件的列表。es5的就加这些
        "lib": [
            "es5",
            "dom",
            "es2015.promise",
            "es2015.collection",
            "es2015.iterable"
        ],
        // 指定生成哪个模块系统代码： "None"， "CommonJS"， "AMD"， "System"， "UMD"， "ES6"或 "ES2015"
        "module": "commonjs",
        //决定如何处理模块。或者是"Node"对于Node.js/io.js，或者是"Classic"（默认）
        "moduleResolution": "node",
        // 在表达式和声明上有隐含的any类型时报错。默认false
        "noImplicitAny": true,
        // 不是函数的所有返回路径都有返回值时报错。默认false
        "noImplicitReturns": true,
        // 当 this表达式的值为any类型的时候，生成一个错误。默认false
        "noImplicitThis": true,
        // 不包含默认的库文件（ lib.d.ts）
        // "noLib": false,
        // 若有未使用的局部变量则抛错。默认false
        "noUnusedLocals": true,
        // 若有未使用的参数则抛错
        // "noUnusedParameters": false,
        // 重定向输出目录。此值是相对于webpack.config.json中的output: path路径的
        "outDir": "../build",
        // 将输出文件合并为一个文件。合并的顺序是根据传入编译器的文件顺序和import的文件顺序决定的
        // "outFile": "../build/babylon.gui.d.ts",
        // 仅用来控制输出的目录结构
        // "rootDir": "./src/",
        // 忽略 库的默认声明文件的类型检查。默认false
        "skipDefaultLibCheck": true,
        // 忽略所有的声明文件（ *.d.ts）的类型检查。默认false
        "skipLibCheck": true,
        // 生成相应的 .map文件
        // "sourceMap": false,
        // 在严格的 null检查模式下,null和undefined值不包含在任何类型里，只允许用它们自己和any来赋值（有个例外， undefined可以赋值到 void）。默认false
        "strictNullChecks": true,
        // 指定ECMAScript目标版本 "ES3"（默认）， "ES5"， "ES6"/ "ES2015"， "ES2016"， "ES2017"或 "ESNext"。
        "target": "es5",
        // 要包含的类型声明文件名列表；如果指定了types，只有被列出来的包才会被包含进来
        "types": [
            "node",
            "babylonjs"
        ],
        // 要包含的类型声明文件路径列表
        // typeRoots: []
    }
};