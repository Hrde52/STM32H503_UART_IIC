## git
…or create a new repository on the command line
```
echo "# STM32H503_UART_IIC" >> README.md
git init
git add README.md
git commit -m "first commit"
git branch -M main
git remote add origin git@github.com:Hrde52/STM32H503_UART_IIC.git
git push -u origin main
```

…or push an existing repository from the command line
```
git remote add origin git@github.com:Hrde52/STM32H503_UART_IIC.git
git branch -M main
git push -u origin main
```

## 推送
```bash
git add .
git commit -m "完成flash参数写入与读取"
git push -u origin test
```

## 克隆仓库：

```bash
git clone git@github.com:Hrde52/CS32F031G6_DTS6012M.git
cd CS32F031G6_DTS6012M  # 进入克隆下来的项目目录
```
获取并切换到远程的 test 分支：
现在使用 git switch 来创建本地 test 分支并跟踪远程的 origin/test：

```bash
git switch -c test origin/test
```
这个命令会自动拉取 test 分支的最新代码。

