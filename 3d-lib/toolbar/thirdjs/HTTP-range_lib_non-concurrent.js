let xhr = null
let cb = null

// --- S 非并发分片下载 ---
function fileFragmentDownload (url, _chunkSize = 1000, type = 'stl', _cb = () => {}) {
  cb = _cb
  return new Promise((resolve, reject) => {
    var downloadText = '',
      totalSize = 1,
      chunkSize = _chunkSize,
      stlLength = 1,
      etag

    // cxcode模型的预请求获取模型大小相关信息
    const fileType = url.split('.').pop()
    let Range = fileType.toLowerCase() === 'cxcode' ? 'bytes=0-14' : 'bytes=0-0'
    // console.log('aaa ', downloadText);

    // 先发一个预请求
    getData(url, {
      responseType: 'arraybuffer',
      Range: Range
    })
      .then(res => {
        // 如果status为200, 则说明文件为空, 后续无需递归
        if (res.status === 200) {
          resolve(res.response)
          return
        } else if (res.status === 206) {
          let headers = parseHeaders(res)
          // console.log('headers = ', headers)
          totalSize = +headers['content-range'].split('/')[1]
          etag = headers['etag']

          if(fileType.toLowerCase() === 'cxcode'){ // cxcode模型
            // let total = uniCodeToString(res.response.substring(7,11))
            stlLength = Number(uniCodeToString(res.response.substring(11,15)))
            console.log(stlLength,'stlLength')
            if(type === 'stl'){
              // 增加1000byte是为了解决大模型数据不全问题
              totalSize = Number(stlLength) + 100
              downloadFile(15, 15 + chunkSize, url)
            }else{
              const start = stlLength + 15
              downloadFile(start, start + chunkSize, url)
            }
          } else { //stl或者gcode模型
            // console.log('totalSize / etag = ', totalSize, etag)
            // 递归
            // count = totalSize / chunkSize % 1 === 0 ? totalSize / chunkSize : Math.ceil(totalSize / chunkSize)
            downloadFile(0, chunkSize, url)
          }
        } else {
          resolve('')
        }
      })
      .catch(err => {
        cb()
        reject(err)
      })

    function downloadFile (startRang, endRang, filePath) {
      // 下载
      if (totalSize && totalSize <= startRang) {
        totalSize = 0
        downloadText = ''
        return
      }
      // 设置请求头
      const headers = {
        Range: `bytes=${startRang}-${endRang}`
      }
      // 递归获取数据
      getData(filePath, headers)
        .then(res => {
          const data = res.response
          downloadText += data
          startRang = endRang + 1
          if (totalSize - endRang > chunkSize) {
            endRang += chunkSize
          } else {
            endRang = totalSize
          }
          // 终止递归: 返回空文件, 清空文件缓存
          // 终止递归: etag错误
          let correntHeaders = parseHeaders(res)
          let currentEtag = correntHeaders['etag']

          // 这里是否可以不需要 ??
          // console.log(`currentEtag , etag = ${currentEtag}, ${etag}`);

          if (currentEtag !== etag) {
            downloadText = ''
            cb()
            resolve({downloadText, totalSize: 0})
            return
          }
          // 如果报文的最后字节数为总字节长度-1,则传输完毕, 应该返回文件内容
          if (correntHeaders.end === totalSize - 1 || correntHeaders.end >= totalSize) {
            // const fileType = url.split('.').pop()
            // if(fileType.toLowerCase() === 'cxcode' && type === 'stl' && downloadText.length > totalSize){
            //   downloadText = downloadText.substring(15,totalSize + 1)
            //   console.log(downloadText, totalSize)
            //   resolve({downloadText, totalSize})
            // }else{
            //   resolve({downloadText, totalSize})
            // }
            resolve({downloadText, totalSize})
          }
          // 分片 => 递归
          downloadFile(startRang, endRang, filePath)
        })
        .catch(err => {
          downloadText = ''
          cb()
          reject(err)
        })
    }

  })
}
// --- E 非并发分片下载 ---

// --- S 下载数据 ---
function getData (filePath, headers) {
  return new Promise((resolve, reject) => {
    // // 1. 创建一个 new XMLHttpRequest 对象
    xhr = new XMLHttpRequest()
    // 2. 配置
    xhr.open('GET', filePath)
    // 2.1 设置请求头
    xhr.setRequestHeader('Range', headers.Range)
    xhr.overrideMimeType('text/plain; charset=x-user-defined')
    // 3. 通过网络发送请求
    xhr.send()
    // 4. 当接收到响应后，将调用此函数
    xhr.onload = function () {
      if (xhr.status === 206 || xhr.status === 200) {
        resolve(xhr)
      } else {
        reject(undefined)
      }
    }

  })
}
// --- E 下载数据 ---

// --- S 解析headers ---
function parseHeaders (res) {
  let headers = {}
  let allResponseHeaders = false
  try {
    allResponseHeaders = res.getAllResponseHeaders()
    allResponseHeaders.split('\r\n').forEach(item => {
      if (item.length) {
        const targetIndex = item.indexOf(':')
        headers[item.substring(0, targetIndex)] = item.substring(
          targetIndex + 1
        )
      }
    })
    headers['end'] = +headers['content-range'].split('/')[0].split('-')[1]
    return headers
  } catch {
    return allResponseHeaders
  }
}
// --- E 解析headers ---

// ---unicode转字符串开始---
function uniCodeToString(str){
  // 方式二
  var hexCharCode = [];
  if (str === "") {
    return "";
  } else {
    for (var i = 0; i < str.length; i++) {
      const curCode = str.charCodeAt(i)
      const uniArray = uint8Array(curCode)
      hexCharCode.push(uniArray.toString(16));
    }
  }
  hexCharCode = hexCharCode.reverse()
  const newStr = parseInt(hexCharCode.join(""), 16).toString(10)
  return newStr
}
// --- unicode转字符串结束---


// --- unicode转16进制越界处理开始---
function uint8Array(s) {
  var c = s;
  var n = 0;
  var ba = new Array()
    if (c < 128) {
      ba[n++] = c;
    }
    else if ((c > 127) && (c < 2048)) {
      ba[n++] = (c >> 6) | 192;
      ba[n++] = (c & 63) | 128;
    }
    else if ((c > 2047) && (c < 65536)) {
      ba[n++] = (c >> 12) | 224;
      ba[n++] = ((c >> 6) & 63) | 128;
      ba[n++] = (c & 63) | 128;
    }
    else {
      ba[n++] = (c >> 18) | 240;
      ba[n++] = ((c >> 12) & 63) | 128;
      ba[n++] = ((c >> 6) & 63) | 128;
      ba[n++] = (c & 63) | 128;
    }
    if (c == 0) {
      ba[n++] = c + '0';
    }
  const unitArray = ba[ba.length - 1]
  return unitArray;
}
// --- unicode转16进制越界处理结束---


function stop () {
  xhr.abort()
  cb()
}

export { fileFragmentDownload, stop }
