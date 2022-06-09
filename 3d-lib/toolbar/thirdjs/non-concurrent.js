let xhr = null
let cb = null

// --- S 非并发分片下载 ---
function fileFragmentDownload (url, _chunkSize = 1000, _cb) {
  cb = _cb
  return new Promise((resolve, reject) => {
    var downloadText = '',
      totalSize = 1,
      chunkSize = _chunkSize,
      etag

    // console.log('aaa ', downloadText);

    // 先发一个预请求
    getData(url, {
      responseType: 'arraybuffer',
      Range: 'bytes=0-0'
    })
      .then(res => {
        // 如果status为200, 则说明文件为空, 后续无需递归
        // console.log('res  ====== ', res)
        if (res.status === 200) {
          resolve(res.response)
          return
        } else if (res.status === 206) {
          let headers = parseHeaders(res)
          // console.log('headers = ', headers)
          totalSize = +headers['content-range'].split('/')[1]
          etag = headers['etag']
          // console.log('totalSize / etag = ', totalSize, etag)
          // 递归
          // count = totalSize / chunkSize % 1 === 0 ? totalSize / chunkSize : Math.ceil(totalSize / chunkSize)
          downloadFile(0, chunkSize, url)
        } else {
          resolve('')
        }
      })
      .catch(err => {
        cb()
        reject(err)
      })

    function downloadFile (startRang, endRang, filePath) {
      // console.log('222');
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
          // 终止递归: 返回空文件, 情况文件缓存
          // 终止递归: etag错误
          let correntHeaders = parseHeaders(res)
          let currentEtag = correntHeaders['etag']

          // 这里是否可以不需要 ??
          // console.log(`currentEtag , etag = ${currentEtag}, ${etag}`);

          if (currentEtag !== etag) {
            downloadText = ''
            cb()
            console.log('这里是否可以不需要');
            resolve(downloadText)
            // resolve('downloadText')
            return
          }
          // 如果报文的最后字节数为总字节长度-1,则传输完毕, 应该返回文件内容
          if (correntHeaders.end === totalSize - 1) {
            // resolve(downloadText)
            resolve(downloadText)
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

function stop () {
  if (xhr != undefined) {
    if (xhr.currentTarget != null) {
      xhr.currentTarget.abort();
    } else if (xhr.target != null) {
      xhr.target.abort();
    } else {
      xhr.abort();
    }
}
  // xhr.abort()

  cb()
}

export { fileFragmentDownload, stop}
