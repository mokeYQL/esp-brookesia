#!/usr/bin/env python3
# Python脚本：编译所有C++知识点案例文件

import os
import subprocess
import re

# 设置参数
CPP_DIR = "e:\\3.esp-brookesia\\esp-brookesia\\products\\C++"
COMPILER = "g++"
COMPILE_FLAGS = ["-std=c++17", "-Wall", "-Wextra", "-pedantic"]

# 获取所有cpp文件
def get_all_cpp_files():
    cpp_files = []
    for i in range(1, 51):
        file_name = f"{i:03d}_*.cpp"
        # 使用glob来匹配文件名
        import glob
        matched_files = glob.glob(os.path.join(CPP_DIR, file_name))
        if matched_files:
            cpp_files.extend(matched_files)
    return sorted(cpp_files)

# 编译单个文件
def compile_file(file_path):
    file_name = os.path.basename(file_path)
    exe_name = os.path.splitext(file_name)[0] + ".exe"
    exe_path = os.path.join(CPP_DIR, exe_name)
    
    print(f"编译文件: {file_name}")
    
    # 构建编译命令
    cmd = [COMPILER] + COMPILE_FLAGS + [file_path, "-o", exe_path]
    
    try:
        # 执行编译命令
        result = subprocess.run(cmd, cwd=CPP_DIR, capture_output=True, text=True, encoding='utf-8')
        
        if result.returncode == 0:
            print(f"✓ 编译成功: {file_name}")
            return True, exe_path
        else:
            print(f"✗ 编译失败: {file_name}")
            print(f"错误信息:\n{result.stderr}")
            return False, None
    except Exception as e:
        print(f"✗ 编译异常: {file_name}")
        print(f"异常信息: {str(e)}")
        return False, None

# 删除所有exe文件
def delete_all_exe_files():
    print("\n删除所有生成的.exe文件...")
    exe_files = glob.glob(os.path.join(CPP_DIR, "*.exe"))
    for exe_file in exe_files:
        try:
            os.remove(exe_file)
            print(f"删除: {os.path.basename(exe_file)}")
        except Exception as e:
            print(f"删除失败: {os.path.basename(exe_file)} - {str(e)}")

# 主函数
def main():
    import glob
    
    cpp_files = get_all_cpp_files()
    print(f"找到 {len(cpp_files)} 个cpp文件")
    
    success_count = 0
    failed_files = []
    exe_files = []
    
    # 编译所有文件
    for cpp_file in cpp_files:
        success, exe_path = compile_file(cpp_file)
        if success:
            success_count += 1
            if exe_path:
                exe_files.append(exe_path)
        else:
            failed_files.append(os.path.basename(cpp_file))
    
    # 打印编译结果
    print(f"\n编译结果: {success_count}/{len(cpp_files)} 成功")
    
    if failed_files:
        print(f"失败的文件: {', '.join(failed_files)}")
        print("\n请修复上述文件的编译错误后再运行此脚本")
    else:
        print("所有文件编译成功!")
        delete_all_exe_files()

if __name__ == "__main__":
    main()
