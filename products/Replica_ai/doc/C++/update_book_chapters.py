#!/usr/bin/env python3
# Python脚本：为C++知识点案例添加书籍章节信息

import os
import re

# 设置文件路径
MAP_FILE = "e:\\3.esp-brookesia\\esp-brookesia\\products\\C++\\book_chapters_map.txt"
CPP_DIR = "e:\\3.esp-brookesia\\esp-brookesia\\products\\C++"

# 读取映射文件
def read_map_file():
    map_data = {}
    with open(MAP_FILE, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            parts = line.split('|', 3)
            if len(parts) != 4:
                print(f"警告：无效行格式: {line}")
                continue
            file_name, knowledge_point, cpp_primer, effective_cpp = parts
            map_data[file_name] = {
                'knowledge_point': knowledge_point,
                'cpp_primer': cpp_primer,
                'effective_cpp': effective_cpp
            }
    return map_data

# 更新单个文件
def update_file(file_path, map_info):
    print(f"处理文件: {os.path.basename(file_path)}")
    
    # 读取文件内容
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 查找头部注释
    header_pattern = r'(/\*.*?\*/)'
    match = re.search(header_pattern, content, re.DOTALL)
    
    if not match:
        print(f"警告：未找到头部注释: {file_path}")
        return
    
    header = match.group(0)
    
    # 检查是否已经添加了书籍信息
    if 'C++ Primer' in header:
        print(f"书籍信息已存在，跳过: {file_path}")
        return
    
    # 提取知识点和源项目信息
    knowledge_point_match = re.search(r'知识点：(.*)', header)
    source_match = re.search(r'源项目：(.*)', header)
    
    if not knowledge_point_match or not source_match:
        print(f"警告：无法提取知识点或源项目信息: {file_path}")
        return
    
    knowledge_point = knowledge_point_match.group(1).strip()
    source_project = source_match.group(1).strip()
    
    # 构造新的头部
    new_header = f"/*\n * 知识点：{knowledge_point}\n * 源项目：{source_project}\n * C++ Primer中文版（第五版）：{map_info['cpp_primer']}\n * Effective+C++中文版（第三版）：{map_info['effective_cpp']}\n */"
    
    # 替换旧头部
    new_content = content.replace(header, new_header)
    
    # 保存文件
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(new_content)
    
    print(f"更新完成: {file_path}")

# 主函数
def main():
    map_data = read_map_file()
    
    for file_name, map_info in map_data.items():
        file_path = os.path.join(CPP_DIR, file_name)
        if os.path.exists(file_path):
            update_file(file_path, map_info)
        else:
            print(f"警告：文件不存在: {file_path}")
    
    print("所有文件处理完成!")

if __name__ == "__main__":
    main()
