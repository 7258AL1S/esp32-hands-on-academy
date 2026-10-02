# H13：OTA 实验部署与救援

先完成稳定版本并验证串口恢复。OTA 不是把课程根目录的 host executable 上传。

1. 核对板上 Flash ≥4 MB，保存当前配置、分区表、stable app 与恢复指令。
2. 将 H13 examples/partitions.csv 复制到工程根；menuconfig 选择 custom partition table，
   设置 4 MB Flash、rollback 和 certificate bundle。两个 app 槽均 0x1E0000。
3. 用完整 `idf.py -p <port> flash` 首次部署新 bootloader/分区/stable app；分区改变前
   明确数据损失与迁移，不能对未知已有数据直接 erase。
4. 构建候选：修改工程 VERSION 或显式 PROJECT_VER，确认 `.bin` 为该项目 app，
   不超过 app 槽。将 app bin 放到自己可控制的 HTTPS 服务。
5. 服务器证书链须可由设备 certificate bundle 验证；私有 CA 用明确的 PEM 信任，
   不启用 skip certificate check。记录服务器地址、版本与目标芯片。
6. menuconfig 的 OTA URL 初始 example.invalid 故意不可用，填真实 URL。
   示例的 app_main 每次启动尝试一次下载，下载成功后不自动重启；正式工具改为
   用户操作/版本策略触发，避免重复下载。同版本不更新的判断属于 Build 挑战。
7. 下载成功后手动复位，读取 running partition/version/pending 状态。
   用明确定义的健康判定确认；“得到 IP”仅为示例健康条件。
8. 下一候选故意在确认前自检失败/复位，验证旧槽恢复。记录真正的 boot 分区和版本。
   断线下载失败要保留旧启动分区。

串口救援按自己的稳定工程 README：恢复保存的分区与 bootloader 配置，build 并 flash。
app OTA 不能默认替换 bootloader/分区表。远程版本降级、签名、密钥、Secure Boot/
Flash Encryption 是部署设计，不能从 TLS 下载成功推导它们已完成。
