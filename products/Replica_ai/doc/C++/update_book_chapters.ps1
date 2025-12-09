# PowerShell脚本：为C++知识点案例添加书籍章节信息

# 设置文件路径
$mapFile = "e:\3.esp-brookesia\esp-brookesia\products\C++\book_chapters_map.txt"
$cppDir = "e:\3.esp-brookesia\esp-brookesia\products\C++"

# 读取映射文件
$mapContent = Get-Content $mapFile | Where-Object { $_ -notmatch '^#' -and $_ -notmatch '^\s*$' }

# 遍历映射文件中的每一行
foreach ($line in $mapContent) {
    # 解析行内容
    $parts = $line -split '\|', 4
    if ($parts.Length -ne 4) {
        Write-Warning "Invalid line format: $line"
        continue
    }
    
    $fileName = $parts[0]
    $knowledgePoint = $parts[1]
    $cppPrimer = $parts[2]
    $effectiveCpp = $parts[3]
    
    $filePath = Join-Path -Path $cppDir -ChildPath $fileName
    
    # 检查文件是否存在
    if (-not (Test-Path $filePath)) {
        Write-Warning "File not found: $filePath"
        continue
    }
    
    Write-Output "Processing file: $fileName"
    
    # 读取文件内容
    $content = Get-Content $filePath -Encoding UTF8
    
    # 查找头部注释
    $headerStart = $content.IndexOf('/*')
    $headerEnd = $content.IndexOf('*/', $headerStart + 2)
    
    if ($headerStart -eq -1 -or $headerEnd -eq -1) {
        Write-Warning "Header comment not found in $fileName"
        continue
    }
    
    # 提取现有头部
    $headerLines = $content[$headerStart..$headerEnd]
    $headerText = $headerLines -join "`n"
    
    # 检查是否已经添加了书籍信息
    if ($headerText -match 'C\+\+ Primer') {
        Write-Output "Book information already exists in $fileName, skipping"
        continue
    }
    
    # 构造新的头部
    $newHeader = "/*
 * 知识点：$knowledgePoint
 * 源项目：$($headerLines[2].Trim(' *'))
 * C++ Primer中文版（第五版）：$cppPrimer
 * Effective+C++中文版（第三版）：$effectiveCpp
 */"
    
    # 替换旧头部
    $newContent = $content
    $newContent[$headerStart..$headerEnd] = $newHeader -split "`n"
    
    # 保存文件
    $newContent -join "`n" | Set-Content $filePath -Encoding UTF8
    
    Write-Output "Updated file: $fileName"
}

Write-Output "All files processed!"
