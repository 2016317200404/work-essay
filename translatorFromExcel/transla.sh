#!/bin/bash

# 获取当前路径
current_path=$(pwd)

# 遍历当前路径下的所有 .ts 文件
for file in "$current_path"/*.ts; do
    # 检查文件是否存在
    if [ -e "$file" ]; then
        # 执行 lrelease 命令处理 .ts 文件
        lrelease "$file"
    fi
done
