# Hints

1. 文件名不是完整性证明；先计算实际 SHA-256。
2. 校验通过后再复制到 inactive slot。
3. pending boot 只有在健康检查通过后才能确认，否则回滚。
