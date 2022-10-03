// 忽略证书相关错误
app.commandLine.appendSwitch('ignore-certificate-errors')
// 我们需要导入两个electron模块
const { app, BrowserWindow } = require('electron')
const path = require('path')

// 创建一个createWindow()函数，用于将index.html加载到新BrowserWindow实例中
const createWindow = () => {
    const win = new BrowserWindow({
        width: 800,
        height: 600,
        autoHideMenuBar:true,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js')
        }
    })
    let promise = win.loadFile('index.html');
}
// 调用这个createWindow()函数来打开你的窗口
app.whenReady().then(() => {
    createWindow()

// 监听窗口激活的事件
    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0){
            createWindow()
        }
    })

})

// 监控窗口全部关闭的事件
app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') app.quit()
})
